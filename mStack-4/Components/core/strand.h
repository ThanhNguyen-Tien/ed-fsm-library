#ifndef CORE_STRAND_H
#define CORE_STRAND_H
#include <core/event.h>
#include <core/queue.h>
#include <core/system.h>
#include <core/event-queue.h>
#include <core/engine.h>
#include <core/timer.h>

namespace core
{

    class Strand : public Component
    {
    public:
        Strand(Queue<EventSlot_t> &queue) : queue_(queue) {}

        /**
         * @brief Post an EmptyEvent to the Strand (SPSC).
         * Only callable from a single producer context (e.g., State Machine).
         */
        bool post(EmptyEvent *event, ByteEvent *finished = nullptr)
        {
            EventSlot_t *slot = queue_.reserve();
            if (slot == nullptr)
            {
                Telemetry::log(TelemetryType::STRAND_QUEUE_FULL, event->index_);
                return false;
            }

            uint32_t id = (finished != nullptr) ? (CALLBACK << 16U) | (static_cast<uint32_t>(finished->index_) << 8U) : (VOID << 16U);
            id |= (event->index_ & 0xFFU);

            slot->event_id = id;
            slot->payload.u = 0;

            queue_.commit();
            next_();
            return true;
        }

        /**
         * @brief Post a FixedEvent with payload to the Strand (SPSC).
         */
        template <typename E>
        bool post(FixedEvent<E> *event, const E &e, ByteEvent *finished = nullptr)
        {
            EventSlot_t *slot = queue_.reserve();
            if (slot == nullptr)
            {
                Telemetry::log(TelemetryType::STRAND_QUEUE_FULL, event->index_);
                return false;
            }

            uint32_t id = (finished != nullptr) ? (CALLBACK << 16U) | (static_cast<uint32_t>(finished->index_) << 8U) : (VOID << 16U);

            id |= (event->index_ & 0xFFU);

            // Handle Small vs Big FixedEvent payload
            if (sizeof(E) <= sizeof(uint32_t))
            {
                slot->payload.u = 0;
                memcpy(&slot->payload.u, &e, sizeof(E));
            }
            else
            {
                void *mem = event->allocPayload();
                if (!mem)
                {
                    Telemetry::log(TelemetryType::MEMPOOL_ALLOC_FAIL, event->index_);
                    queue_.decresePeakOne(); // Roll back peak count since allocation failed
                    return false;            // Note: reserve was done, but we don't commit, head doesn't move
                }
                memcpy(mem, &e, sizeof(E));
                slot->payload.p = mem;
            }

            slot->event_id = id;
            queue_.commit();
            next_();
            return true;
        }

        /**
         * @brief Post a delay event to the Strand (SPSC).
         */
        void delay(uint32_t ms)
        {
            EventSlot_t *slot = queue_.reserve();
            if (slot == nullptr)
                return;

            slot->event_id = (DELAY << 16U);
            slot->payload.u = ms;

            queue_.commit();
            next_();
        }

        void done()
        {
            __atomic_clear(&busy_, __ATOMIC_RELEASE);
            next_();
        }

        void done(uint8_t error)
        {
            __atomic_clear(&busy_, __ATOMIC_RELEASE);
            if (finished_ != nullptr)
            {
                finished_->post(error);
            }
            next_();
        }

    private:
        enum EventType : uint32_t
        {
            VOID = 1,
            CALLBACK,
            DELAY
        };

        /**
         * @brief Triggers the next execution trigger if not busy.
         */
        void next_()
        {
            // Use atomic test-and-set to ensure only one trigger is active
            if (__atomic_test_and_set(&busy_, __ATOMIC_ACQUIRE))
            {
                return;
            }

            if (queue_.empty())
            {
                __atomic_clear(&busy_, __ATOMIC_RELEASE);
                return;
            }

            if (!executeEvent_.post())
            {
                __atomic_clear(&busy_, __ATOMIC_RELEASE);
            }
        }

        void execute_()
        {
            EventSlot_t *slot = queue_.peekTail();
            if (slot == nullptr)
            {
                __atomic_clear(&busy_, __ATOMIC_RELEASE);
                return;
            }

            uint32_t id = slot->event_id;
            uint32_t type = (id >> 16U) & 0xFFU;

            if (type == DELAY)
            {
                timer_.start(slot->payload.u, 1);
                finished_ = nullptr;
                // busy_ remains true, cleared in timeout_ or done()
            }
            else
            {
                if (type == CALLBACK)
                {
                    uint8_t cb_idx = static_cast<uint8_t>((id >> 8U) & 0xFFU);
                    finished_ = (cb_idx < events_.poolSize_) ? (ByteEvent *)events_.events_[cb_idx] : nullptr;
                }
                else
                {
                    finished_ = nullptr;
                }

                uint8_t ev_idx = static_cast<uint8_t>(id & 0xFFU);
                if (ev_idx < events_.poolSize_)
                {
                    events_.events_[ev_idx]->execute(slot->payload);
                }
                // busy_ remains true, cleared by USER calling done()
            }

            queue_.pop();
        }

        void timeout_()
        {
            __atomic_clear(&busy_, __ATOMIC_RELEASE);
            next_();
        }

    private:
        ByteEvent *finished_ = nullptr;
        EmptyEvent executeEvent_ = EmptyEvent(this, static_cast<EmptyEvent::Handler>(&Strand::execute_));
        EventQueue &events_ = Engine::instance().events();
        Timer timer_ = Timer(this, static_cast<Timer::Handler>(&Strand::timeout_));
        Queue<EventSlot_t> &queue_;
        volatile bool busy_ = false;
    };
}

#define M_STRAND(name, size)                        \
private:                                            \
    QUEUE_DEF(name##Queue, size, core::EventSlot_t) \
public:                                             \
    core::Strand name##Strand{core::Strand(name##Queue##_)};

#endif // STRAND_H
