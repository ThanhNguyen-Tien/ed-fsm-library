#ifndef CORE_EVENT_QUEUE_H_
#define CORE_EVENT_QUEUE_H_

#include <core/base.h>
#include <core/config.h>
#include <core/system.h>
#include <core/queue.h>
#include <telematry/telemetry.h>

namespace core
{
    static constexpr uint32_t READY_BIT = 0x80000000U; // Bit 31
    static constexpr uint32_t PRIO_MASK = 0x70000000U; // Bits 30-28
    static constexpr uint32_t PRIO_SHIFT = 28U;
    static constexpr uint32_t ID_MASK = 0x000000FFU; // Bits 7-0

    struct EventSlot_t
    {
        uint32_t header; // READY_BIT(31)-PRIO(30-28)----EVENT_ID(7-0)
        EventPayload payload;
        uint32_t timestamp;
    };

    struct QueueConfig
    {
        EventSlot_t *buffer;
        uint16_t size;
    };

    template <uint8_t N = 8>
    class EventQueue
    {
        static_assert(N > 0 && N <= 8, "EventQueue only supports 1 to 8 priorities");

    public:
        virtual ~EventQueue() = default;

        EventQueue(const QueueConfig *configs, uint8_t num_priorities)
            : bitmap_(0),
              num_prio_(num_priorities > N ? N : num_priorities),
              poolSize_(0)
        {
            for (size_t i = 0; i < EVENT_POOL_SIZE; i++)
            {
                events_[i] = nullptr;
            }

            for (uint8_t i = 0; i < num_prio_; i++)
            {
                queues_[i].init(configs[i].buffer, configs[i].size);
            }
        }

        uint32_t getPeakUsed(uint8_t prio) const
        {
            if (prio >= num_prio_)
                return 0;
            return queues_[prio].peakUsed();
        }

        void resetEventsMeasurements()
        {
            for (size_t i = 0; i < poolSize_; i++)
            {
                Event *ev = this->events_[i];
                if (ev == nullptr)
                    continue;

                ev->timeExecution.min_time = UINT32_MAX;
                ev->timeExecution.max_time = 0U;
                ev->timeExecution.value = 0U;

                ev->latency.min_time = UINT32_MAX;
                ev->latency.max_time = 0U;
                ev->latency.value = 0U;
            }
        }

        inline bool post(uint8_t event_id, const EventPayload &payload)
        {
            if (event_id >= poolSize_)
            {
                Error_Handler();
                return false;
            }

            Event *e = events_[event_id];
            uint8_t p = e->getPriority(); // Range 0-7 (P7 is highest)

            if (p >= N)
            {
                Error_Handler();
                return false;
            }

            EventSlot_t *slot = queues_[p].reserveAtomic();
            if (slot == nullptr)
            {
                Telemetry::log(TelemetryType::EV_QUEUE_FULL, (uint16_t)event_id);
                return false;
            }

            slot->payload = payload;
            slot->timestamp = DWT->CYCCNT;

            // Pack Header: [READY(31)][PRIO(30-28)][ID(7-0)]
            uint32_t header_ = READY_BIT | (static_cast<uint32_t>(p) << PRIO_SHIFT) | event_id;

            // Release Barrier: Ensure the payload and timestamp are written before updating event_id
            __atomic_store_n(&slot->header, header_, __ATOMIC_RELEASE);

            // Set bit at position p to indicate Queue p has events
            __atomic_or_fetch(&this->bitmap_, (1 << p), __ATOMIC_RELAXED);
            return true;
        }

        inline bool next()
        {
            // Acquire Barrier: sync with Producers to see latest bitmap and event_id updates
            uint32_t mask = __atomic_load_n(&this->bitmap_, __ATOMIC_ACQUIRE);
            if (mask == 0)
                return false;

            // Find highest priority Queue with events (bit 7 is highest priority)
            uint32_t p_bitmap = 31 - __builtin_clz(mask);

            auto &q = queues_[p_bitmap];
            EventSlot_t *slot = q.peekTail();
            if (slot == nullptr)
            {
                Telemetry::log(TelemetryType::QUEUE_UNEXPECTED_EMPTY, p_bitmap);
                return false;
            }

            uint32_t header_ = __atomic_load_n(&slot->header, __ATOMIC_ACQUIRE);
            if (!(header_ & READY_BIT))
            {
                Telemetry::log(TelemetryType::LOCK_FREE_YIELD);
                return false;
            }

            // Extract priority and event_id from header
            uint8_t p_slot = static_cast<uint8_t>((header_ & PRIO_MASK) >> PRIO_SHIFT);
            uint8_t event_id = static_cast<uint8_t>(header_ & ID_MASK);

            if (event_id < poolSize_)
            {
                Event *e = events_[event_id];
                uint32_t exec_start = DWT->CYCCNT;

                e->latency.value = exec_start - slot->timestamp;
                e->execute(slot->payload);
                e->timeExecution.value = DWT->CYCCNT - exec_start;

                if (e->latency.value > e->latency.max_time)
                    e->latency.max_time = e->latency.value;
                if (e->latency.value < e->latency.min_time)
                    e->latency.min_time = e->latency.value;
                if (e->timeExecution.value > e->timeExecution.max_time)
                    e->timeExecution.max_time = e->timeExecution.value;
                if (e->timeExecution.value < e->timeExecution.min_time)
                    e->timeExecution.min_time = e->timeExecution.value;
            }

            // Clean up slot for next producer (Release Barrier to ensure all reads/writes to slot are done before clearing event_id)
            __atomic_store_n(&slot->header, 0, __ATOMIC_RELEASE);
            queues_[p_slot].pop();

            // Double-check to clear bitmap bit if Queue is now empty (another producer might have posted after we peaked but before we popped)
            if (queues_[p_slot].empty())
            {
                __atomic_and_fetch(&this->bitmap_, ~(1 << p_slot), __ATOMIC_RELAXED);
                if (!queues_[p_slot].empty())
                {
                    __atomic_or_fetch(&this->bitmap_, (1 << p_slot), __ATOMIC_RELAXED);
                }
            }
            return true;
        }

    private:
        uint8_t registerEvent_(Event *event)
        {
            if (this->poolSize_ >= EVENT_POOL_SIZE)
            {
                Error_Handler();
                return 0xFF;
            }
            uint8_t id = this->poolSize_;
            this->events_[id] = event;
            this->poolSize_++;
            return id;
        }

        Queue<EventSlot_t> queues_[N];
        Event *events_[EVENT_POOL_SIZE];
        uint32_t bitmap_;
        uint8_t num_prio_;
        uint8_t poolSize_;

        friend class Event;
        friend class Engine;
        friend class Strand;
    };
} // namespace core

#endif /* CORE_EVENT_QUEUE_H_ */
