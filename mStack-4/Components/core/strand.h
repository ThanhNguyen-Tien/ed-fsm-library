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

		void done()
		{
			busy_ = false;
			next_();
		}

		void done(uint8_t error)
		{
			busy_ = false;
			if (finished_ != nullptr)
				finished_->post(error);
			next_();
		}

	private:
		enum EventType : uint32_t
		{
			VOID = 1,
			CALLBACK,
			DELAY
		};

		void next_()
		{
			// LDREX/STREX (or atomic intrinsic) to protect flag busy_
			// Only threads that successfully change busy_ from false to true are allowed to post executeEvent_
			if (__atomic_test_and_set(&busy_, __ATOMIC_ACQUIRE))
			{
				return; // Currently processing or already in a queue.
			}

			// If queue empty then return busy_ = false
			if (queue_.empty())
			{
				busy_ = false;
				return;
			}

			if (!executeEvent_.post())
			{
				busy_ = false;
			}
		}

		void timeout_()
		{
			busy_ = false;
			next_();
		}

		void execute_()
		{
			EventSlot_t *slot = queue_.peekTail();
			if (slot == nullptr)
			{
				busy_ = false;
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
