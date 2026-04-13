#ifndef CORE_EVENT_QUEUE_H_
#define CORE_EVENT_QUEUE_H_

#include <core/base.h>
#include <core/config.h>
#include <core/queue/mpsc-queue.h>
#include <core/system.h>
#include <telematry/telemetry.h>

namespace core
{
    union EventHeader_u
    {
        struct
		{
            uint32_t eventId : 16;  // Bits 0-15
            uint32_t length  : 8;   // Bits 16-23
            uint32_t prio    : 3;   // Bits 24-26 (Range 0-7)
            uint32_t res     : 4;   // Bits 27-30 (Reserved)
            uint32_t ready   : 1;   // Bit 31
        } bits;
        uint32_t raw;
    };

    template <uint8_t N = 8>
    class EventQueue {
    public:
        EventQueue(I_MpscQueue* const* internalQueues, uint8_t num_prio)
            : queues_(internalQueues),
              bitmap_(0),
			  poolSize_(0),
              num_prio_(num_prio > N ? N : num_prio)
        {
            for (uint16_t i = 0; i < EVENT_POOL_SIZE; i++) {
                events_[i] = nullptr;
            }
        }

        inline bool post(uint16_t event_id, const uint32_t* payload, uint8_t chunks)
        {
            if (event_id >= poolSize_) return false;

            Event* e = events_[event_id];
            uint8_t p = e->getPriority();
            I_MpscQueue* q = queues_[p];
            uint32_t startHead;
            uint8_t totalReq = 2 + chunks; // Header + TimeStamp + Payload

            if (!q->reserveChunks(totalReq, startHead))
            {
            	return false;
            }

            uint32_t* buf = q->getBuffer();
            uint32_t mask = q->getMask();

            // Write Timestamp to second slot
            buf[(startHead + 1) & mask] = DWT->CYCCNT;

            // Write Payload to the next slots
            for (uint32_t i = 0; i < chunks; i++) {
                buf[(startHead + 2 + i) & mask] = payload[i];
            }

            EventHeader_u h;
            h.bits.eventId = event_id;
            h.bits.length = chunks;
            h.bits.prio = p;
            h.bits.ready = 1;

            __DMB();

            // Write Header to the first slot to trigger Consumer
            buf[startHead & mask] = h.raw;

            __atomic_or_fetch(&this->bitmap_, (1 << p), __ATOMIC_RELAXED);
            return true;
        }

        inline bool next()
        {
            uint32_t mask = __atomic_load_n(&this->bitmap_, __ATOMIC_ACQUIRE);
            if (mask == 0) return false;

            uint8_t p = 31 - __builtin_clz(mask);
            I_MpscQueue* q = queues_[p];

            uint32_t currTail = q->getTail();
            uint32_t qMask = q->getMask();
            uint32_t* buf = q->getBuffer();

            EventHeader_u h;
            h.raw = buf[currTail & qMask];

            if (!h.bits.ready)
			{
//            	Telemetry::log(TelemetryType::LOCK_FREE_YIELD, p);
            	return false;
			}

            uint8_t len = h.bits.length;
            Event* e = events_[h.bits.eventId];

            if (e)
            {
                uint32_t ts = buf[(currTail + 1) & qMask];
                uint32_t exec_start = DWT->CYCCNT;

                e->latency.value = exec_start - ts;
                void* payloadPtr = nullptr;

                if (len > 0) {
                    for (uint32_t i = 0; i < len; i++) {
                        tempData_[i] = buf[(currTail + 2 + i) & qMask];
                    }
                    payloadPtr = static_cast<void*>(tempData_);
                }

                e->execute(payloadPtr);
                e->timeExecution.value = DWT->CYCCNT - exec_start;

                // TODO: Cập nhật min/max latency/exec tại đây
            }
            else
            {
            	// Event ID không hợp lệ, có thể log lỗi hoặc bỏ qua
				Telemetry::log(TelemetryType::QUEUE_UNEXPECTED_EMPTY, h.bits.eventId);
				// Tiếp tục xử lý để dọn dẹp slot
            }

            // Cleanup
            q->clearSlot(currTail); // Only Clean Header is needed as payload will be overwritten by Producer

            __DMB();
            uint8_t totalSlots = 2 + len;
            q->updateTail(totalSlots);

            if (q->isEmpty_()) {
                __atomic_and_fetch(&this->bitmap_, ~(1 << p), __ATOMIC_RELAXED);
            }
            return true;
        }

    private:
        uint16_t registerEvent_(Event *event)
        {
            if (this->poolSize_ >= EVENT_POOL_SIZE)
            {
                Error_Handler();
                return 0xFF;
            }
            uint16_t id = this->poolSize_;
            this->events_[id] = event;
            this->poolSize_++;
            return id;
        }

        I_MpscQueue* const* queues_;
        Event* events_[EVENT_POOL_SIZE];
        volatile uint32_t bitmap_;
        uint16_t poolSize_;
        uint8_t num_prio_;
        static uint32_t tempData_[MAX_EVENT_PAYLOAD_CHUNKS];

        friend class Event;
        friend class Engine;
        template <uint16_t S>
        friend class Strand;
    };
} // namespace core

#endif /* CORE_EVENT_QUEUE_H_ */
