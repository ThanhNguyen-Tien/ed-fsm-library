#ifndef CORE_EVENT_QUEUE_H_
#define CORE_EVENT_QUEUE_H_

#include <core/base.h>
#include <core/system.h>
#include <core/queue.h>
#include <telematry/telemetry.h>

namespace core
{
	static constexpr uint32_t READY_BIT = 0x80000000U;

	struct EventSlot_t
	{
		uint32_t event_id;
		EventPayload payload;
		uint32_t timestamp;
	};

	class EventQueue
	{
	public:
		virtual ~EventQueue() = default;
		EventQueue() : evQueue(buffer_, EVENT_QUEUE_SIZE)
		{
		}

		uint8_t getMaxPeak() const
		{
			return this->evQueue.peakUsed();
		}

		void resetEventsMeasurements()
		{
			for (size_t i = 0; i < EVENT_POOL_SIZE; i++)
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

		inline bool next()
		{
			EventSlot_t *slot = this->evQueue.peekTail();
			if (slot == nullptr) // Queue Empty
			{
				return false;
			}

			uint32_t raw_id = __atomic_load_n(&slot->event_id, __ATOMIC_ACQUIRE);
			if (!(raw_id & READY_BIT)) // Producer hasn't finished writing the event, wait for next turn (Yield)
			{
				Telemetry::log(TelemetryType::LOCK_FREE_YIELD);
				return false;
			}

			uint32_t event_id = raw_id & ~READY_BIT; // Masking to remove Ready Bit (bit 31)

			if (event_id < this->poolSize_)
			{
				Event *e = this->events_[event_id];

				uint32_t exec_start = DWT->CYCCNT;
				e->latency.value = exec_start - slot->timestamp;
				e->execute(slot->payload);
				e->timeExecution.value = DWT->CYCCNT - exec_start;

				if (e->latency.value > e->latency.max_time)
				{
					e->latency.max_time = e->latency.value;
				}
				else
				{
				}
				if (e->latency.value < e->latency.min_time)
				{
					e->latency.min_time = e->latency.value;
				}
				else
				{
				}

				if (e->timeExecution.value > e->timeExecution.max_time)
				{
					e->timeExecution.max_time = e->timeExecution.value;
				}
				else
				{
				}
				if (e->timeExecution.value < e->timeExecution.min_time)
				{
					e->timeExecution.min_time = e->timeExecution.value;
				}
				else
				{
				}
			}
			else
			{
				Error_Handler();
				return false;
			}

			__atomic_store_n(&slot->event_id, 0, __ATOMIC_RELEASE); // Clear READY_BIT and ID
			this->evQueue.pop();											// Increase Tail to free up a slot in Ring Buffer
			return true;
		}

		inline bool postSlot(uint8_t index, const EventPayload &payload)
		{
			auto *s = this->evQueue.reserveAtomic(); // Protect by LDREX/STREX
			if (s == nullptr)
			{
				//				Error_Handler();
				Telemetry::log(TelemetryType::EV_QUEUE_FULL, index);
				return false;
			}

			s->payload = payload;
			s->timestamp = DWT->CYCCNT;

			__atomic_store_n(&s->event_id, static_cast<uint32_t>(index) | READY_BIT, __ATOMIC_RELEASE);

			return true;
		}

	private:
		uint8_t registerEvent_(Event *event)
		{
			if (this->poolSize_ >= EVENT_POOL_SIZE)
				Error_Handler();

			this->events_[poolSize_] = event;
			return this->poolSize_++;
		}

	private:
		Event *events_[EVENT_POOL_SIZE];
		alignas(4) EventSlot_t buffer_[EVENT_QUEUE_SIZE];
		Queue<EventSlot_t> evQueue;
		uint8_t poolSize_ = 0;

		friend class Event;
		friend class Engine;
		friend class Strand;
	};

} // namespace core

#endif /* CORE_EVENT_QUEUE_H_ */
