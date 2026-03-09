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
		Strand(Queue<EventSlot_t> &queue) : queue_(queue)
		{
		}

		bool post(EmptyEvent *event, ByteEvent *finished = nullptr)
		{
			EventSlot_t *slot = queue_.reserveAtomic();
			if (slot == nullptr)
			{
				Error_Handler();
				return false;
			}

			uint32_t id = (finished != nullptr) ? (CALLBACK << 16U) | (static_cast<uint32_t>(finished->index_) << 8U) : (VOID << 16U);
			id |= (event->index_ & 0xFFU);

			slot->timestamp = DWT->CYCCNT;
			__DMB();
			slot->event_id = id | READY_BIT;

			next_();
			return true;
		}

		template <typename E>
		bool post(FixedEvent<E> *event, const E &e, ByteEvent *finished = nullptr)
		{
			EventSlot_t *slot = queue_.reserveAtomic();
			if (slot == nullptr)
			{
				Error_Handler();
				return false;
			}

			uint32_t id = 0;
			if (finished != nullptr)
			{
				id |= (CALLBACK << 16U);
				id |= (static_cast<uint32_t>(finished->index_) << 8U);
			}
			else
			{
				id |= (VOID << 16U);
			}
			id |= (event->index_ & 0xFFU);

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
					Error_Handler();
					return false;
				}
				memcpy(mem, &e, sizeof(E));
				slot->payload.p = mem;
			}
			slot->timestamp = DWT->CYCCNT;

			__DMB();

			slot->event_id = id | READY_BIT;

			next_();
			return true;
		}

		bool delay(uint32_t ms)
		{
			EventSlot_t *slot = queue_.reserveAtomic();
			if (slot == nullptr)
			{
				Error_Handler();
				return false;
			}

			slot->payload.u = ms;
			slot->timestamp = DWT->CYCCNT;
			__DMB();
			slot->event_id = (DELAY << 16U) | READY_BIT;

			next_();
			return true;
		}

		/**
		 * @brief Signals that the current event processing is finished.
		 *
		 * This must be called by the user (usually at the end of an event handler)
		 * to allow the Strand to move to the next queued event.
		 */
		void done()
		{
		    /*
		     * ATOMIC STEP: Atomically set 'busy_' to 'false'.
		     * __ATOMIC_RELEASE acts as a Memory Barrier, ensuring all data processed
		     * in the current task is globally visible to other cores/DMA before we unlock.
		     */
		    __atomic_clear(&busy_, __ATOMIC_RELEASE);

		    /* Trigger the next task if any */
		    next_();
		}

		/**
		 * @brief Signals completion with an error/status byte.
		 * @param error Status code to be sent back via the 'finished_' event.
		 */
		void done(uint8_t error)
		{
		    __atomic_clear(&busy_, __ATOMIC_RELEASE);

		    /* Optional: Notify a listener that this Strand sequence has ended/errored */
		    if (finished_ != nullptr) {
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
		 * @brief Attempts to trigger the next event execution in the Strand.
		 *
		 * This function uses a Lock-Free "Test-and-Set" mechanism to ensure that
		 * ONLY ONE thread/interrupt can trigger the execution process at any given time.
		 * It prevents race conditions where multiple interrupts might try to post
		 * the executeEvent_ simultaneously.
		 */
		void next_()
		{
		    /*
		     * ATOMIC STEP: Read 'busy_' and set it to 'true' in a single, indivisible hardware operation.
		     * __atomic_test_and_set returns the PREVIOUS value of 'busy_'.
		     * - If it returns 'true': Someone else is already processing. We skip (Abort).
		     * - If it returns 'false': We successfully "locked" the Strand. We proceed.
		     * __ATOMIC_ACQUIRE ensures subsequent memory reads don't happen before this lock.
		     */
		    if (__atomic_test_and_set(&busy_, __ATOMIC_ACQUIRE))
		    {
		        return; // Strand is currently busy or an execution is already scheduled.
		    }

		    /*
		     * If the queue is empty, we must release the 'busy' lock so future 'post'
		     * calls can trigger the Strand again.
		     */
		    if (queue_.empty())
		    {
		        __atomic_clear(&busy_, __ATOMIC_RELEASE);
		        return;
		    }

		    /*
		     * Try to post the execution trigger to the System Engine.
		     * If the system queue is full (post fails), we must release the 'busy' lock
		     * so the next call to next_() has a chance to retry.
		     */
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

			// Check Ready Bit (In case the producer hasn't finished writing the event)
			uint32_t raw_id = slot->event_id;
			if (!(raw_id & READY_BIT))
			{
				// Repost itself to the EventQueue to wait for the next turn (Yield)
				executeEvent_.post();
				return;
			}

			uint32_t type = (raw_id >> 16U) & 0x7FU; // Masking to remove Ready Bit (bit 31)

			if (type == DELAY)
			{
				uint32_t time = slot->payload.u;
				timer_.start(time, 1);
				finished_ = nullptr;
			}
			else
			{
				// Handle VOID or CALLBACK
				if (type == CALLBACK)
				{
					uint8_t cb_idx = static_cast<uint8_t>((raw_id >> 8U) & 0xFFU);
					finished_ = (cb_idx < events_.poolSize_) ? (ByteEvent *)events_.events_[cb_idx] : nullptr;
				}
				else
				{
					finished_ = nullptr;
				}

				uint8_t ev_idx = static_cast<uint8_t>(raw_id & 0xFFU);
				if (ev_idx < events_.poolSize_)
				{
					events_.events_[ev_idx]->execute(slot->payload);
				}
			}

			slot->event_id = 0;
			queue_.pop();
		}

	private:
		ByteEvent *finished_ = nullptr;
		EmptyEvent executeEvent_ = EmptyEvent(this,
											  static_cast<EmptyEvent::Handler>(&Strand::execute_));
		EventQueue &events_ = Engine::instance().events();
		Timer timer_ = Timer(this, static_cast<Timer::Handler>(&Strand::done));
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
