#ifndef CORE_EVENT_QUEUE_H_
#define CORE_EVENT_QUEUE_H_

#include <core/base.h>
#include <core/system.h>
#include <core/queue.h>

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

		uint16_t getNumOfByteHeaps() const
		{
			return this->numOfByteHeaps_;
		}

		void addNumOfByteHeap(uint32_t val)
		{
			this->numOfByteHeaps_ += val;
		}

		void resetEventsMeasurements()
		{
			for (size_t i = 0; i < EVENT_POOL_SIZE; i++)
			{
				Event *ev = events_[i];
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
			EventSlot_t *slot = evQueue.peekTail();
			if (slot == nullptr) // Queue Empty
			{
				return false;
			}

			uint32_t raw_id = slot->event_id;
			if (!(raw_id & READY_BIT)) // Producer hasn't finished writing the event, wait for next turn (Yield)
			{
				return false;
			}

			uint32_t event_id = raw_id & ~READY_BIT; // Masking to remove Ready Bit (bit 31)

			if (event_id < poolSize_)
			{
				Event *e = events_[event_id];

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

			slot->event_id = 0; // Clean
			evQueue.pop();		// Increase Tail to free up a slot in Ring Buffer
			return true;
		}

		inline bool postSlot(uint8_t index, const EventPayload &payload)
		{
			auto *s = evQueue.reserveAtomic(); // Protect by LDREX/STREX
			if (s == nullptr)
			{
				Error_Handler();
				return false;
			}

			s->payload = payload;
			s->timestamp = DWT->CYCCNT;

			__DMB(); // Ensure payload/timestamp write successfully before set READY_BIT

			// Set ID and Ready Bit
			s->event_id = static_cast<uint32_t>(index) | READY_BIT;

			return true;
		}

		inline void post(uint8_t index)
		{
			EventPayload p;
			p.u = 0;
			postSlot(index, p);
		}

	private:
		uint8_t registerEvent_(Event *event)
		{
			if (poolSize_ >= EVENT_POOL_SIZE)
				Error_Handler();

			events_[poolSize_] = event;
			return poolSize_++;
		}

	private:
		Event *events_[EVENT_POOL_SIZE];
		alignas(4) EventSlot_t buffer_[EVENT_QUEUE_SIZE];
		Queue<EventSlot_t> evQueue;
		uint16_t numOfByteHeaps_ = 0U;
		uint8_t poolSize_ = 0;

		friend class Event;
		friend class Engine;
		friend class Strand;
	};

} // namespace core

#endif /* CORE_EVENT_QUEUE_H_ */
