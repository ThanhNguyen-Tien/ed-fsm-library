#ifndef CORE_EVENT_QUEUE_H_
#define CORE_EVENT_QUEUE_H_

#include <core/base.h>
#include <core/system.h>
#include <core/queue.h>

namespace core {
class EventQueue: public AbstractEventQueue {
public:
	virtual ~EventQueue() = default;
	EventQueue() = default;

	uint16_t getMinAvail() const {
		return this->minAvail_;
	}

	uint16_t getNumOfByteHeaps() const {
		return this->numOfByteHeaps_;
	}

	void addNumOfByteHeap(uint32_t val) {
		this->numOfByteHeaps_ += val;
	}

	inline bool next() {

		if (evQueue.empty()) {
			return false;
		}
		uint8_t index = evQueue.pop();
		if (index < poolSize_) {
#ifdef MONITOR_EVENT_TIME_EXECUTION
			Event *e = events_[index];
			uint32_t exec_start = DWT->CYCCNT;
			e->execute(this);
			e->timeExecution.last_exec_time = DWT->CYCCNT - exec_start;
			if (e->timeExecution.last_exec_time > e->timeExecution.max_time)
				e->timeExecution.max_time = e->timeExecution.last_exec_time;
			if (e->timeExecution.last_exec_time < e->timeExecution.min_time)
				e->timeExecution.min_time = e->timeExecution.last_exec_time;
#else
			Event *e = events_[index];
			e->execute(this);
#endif
		}
		else
		{
			Error_Handler();
		}
		return true;
	}

	inline void post(uint8_t index) {
		CRITICAL_SECTION;
		uint16_t avail = evQueue.freeSpace();
		if (avail < 1) {
#ifdef RELEASE
				minAvail_ = 0;
				return;
#else
			Error_Handler();
#endif
		}
		evQueue.push(index);
		avail -= 1;
		if (avail < minAvail_) {
			minAvail_ = avail;
		}
	}

	inline void pushFixed(uint8_t index, uint8_t *data, size_t size) override
	{
		CRITICAL_SECTION;
		uint16_t avail = evQueue.freeSpace();
		if (avail < size + 1) {
#ifdef RELEASE
				minAvail_ = 0;
				return;
#else
			Error_Handler();
#endif
		}
		evQueue.push(index);
		for (size_t i = 0; i < size; i++) {
			evQueue.push(data[i]);
		}
		avail -= (size + 1);
		if (avail < minAvail_) {
			minAvail_ = avail;
		}
	}

	inline void popFixed(uint8_t *data, size_t size) override
	{
		for (size_t i = 0; i < size; i++) {
			data[i] = evQueue.pop();
		}
	}

private:
	uint8_t registerEvent_(Event *event) {
		events_[poolSize_] = event;
		if (poolSize_ >= EVENT_POOL_SIZE) {
			Error_Handler();
		}
		return poolSize_++;
	}

private:
	Event *events_[EVENT_POOL_SIZE];
	uint32_t numOfByteHeaps_ = 0U;
	uint16_t minAvail_ = EVENT_QUEUE_SIZE;
	uint8_t buffer_[EVENT_QUEUE_SIZE];
	Queue<uint8_t> evQueue = Queue<uint8_t>(buffer_, EVENT_QUEUE_SIZE);
	uint8_t poolSize_ = 0;

	friend class Event;
	friend class Engine;
	friend class Strand;
};
}

#endif /* CORE_EVENT_QUEUE_H_ */
