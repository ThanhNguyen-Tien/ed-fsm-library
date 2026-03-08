#ifndef CORE_EVENT_QUEUE_H_
#define CORE_EVENT_QUEUE_H_

#include <core/base.h>
#include <core/system.h>
#include <core/queue.h>

namespace core {
struct EventSlot_t {
	uint32_t event_id;
	EventPayload payload;
	uint32_t timestamp;
};

class EventQueue {
public:
	virtual ~EventQueue() = default;
	EventQueue() :
			evQueue(buffer_, EVENT_QUEUE_SIZE) {
	}

	uint8_t getMaxPeak() const {
		return this->evQueue.peakUsed();
	}

	uint16_t getNumOfByteHeaps() const {
		return this->numOfByteHeaps_;
	}

	void addNumOfByteHeap(uint32_t val) {
		this->numOfByteHeaps_ += val;
	}

	void resetEventsMeasurements() {
		for (size_t i = 0; i < EVENT_POOL_SIZE; i++) {
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

	__attribute__((always_inline)) inline bool next() {
		if (evQueue.empty()) {
			return false;
		}

		EventSlot_t slot { };
		if (!evQueue.pop(slot)) {
			Error_Handler();	// FIXME: Log error instead of halting system
			return false;
		}

		if (slot.event_id < poolSize_) {
			Event *e = events_[slot.event_id];

			uint32_t exec_start = DWT->CYCCNT;
			e->latency.value = exec_start - slot.timestamp;
			e->execute(slot.payload);
			e->timeExecution.value = DWT->CYCCNT - exec_start;

			if (e->latency.value > e->latency.max_time) {
				e->latency.max_time = e->latency.value;
			} else {
			}
			if (e->latency.value < e->latency.min_time) {
				e->latency.min_time = e->latency.value;
			} else {
			}

			if (e->timeExecution.value > e->timeExecution.max_time) {
				e->timeExecution.max_time = e->timeExecution.value;
			} else {
			}
			if (e->timeExecution.value < e->timeExecution.min_time) {
				e->timeExecution.min_time = e->timeExecution.value;
			} else {
			}
		} else {
			Error_Handler();
			return false;
		}
		return true;
	}

	__attribute__((always_inline)) inline bool postSlot(uint8_t index,
			const EventPayload &payload) {
		CRITICAL_SECTION_PRIO(1)

		auto *s = evQueue.reserve();
		if (s == nullptr) {
			Error_Handler();   // queue full
			return false;
		}

		s->event_id = index;
		s->payload = payload;
		s->timestamp = DWT->CYCCNT;

		__DMB();	// ensure payload visible before publish
		evQueue.commit();

		return true;
	}

	__attribute__((always_inline)) inline void post(uint8_t index) {
		EventPayload p;
		p.u = 0;
		postSlot(index, p);
	}

private:
	uint8_t registerEvent_(Event *event) {
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
