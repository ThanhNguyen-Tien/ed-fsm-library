#ifndef CORE_STRAND_H
#define CORE_STRAND_H
#include <core/event.h>
#include <core/queue.h>
#include <core/system.h>
#include <core/event-queue.h>
#include <core/engine.h>
#include <core/timer.h>

namespace core {

class Strand: public Component, public AbstractEventQueue {
public:
	Strand(Queue<uint8_t> &queue) :
			queue_(queue) {
	}

	void post(EmptyEvent *event, ByteEvent *finished = nullptr) {
		if (queue_.freeSpace() < 3) {
			return;
		}

		DISABLE_INTERRUPT;
		if (finished != nullptr) {
			queue_.push(CALLBACK);
			queue_.push(finished->index_);
		} else {
			queue_.push(VOID);
		}
		queue_.push(event->index_);
		ENABLE_INTERRUPT;

		next_();
	}

	template<typename E>
	void post(FixedEvent<E> *event, const E &e, ByteEvent *finished = nullptr) {
		if (queue_.freeSpace() < sizeof(E) + 3)
			return;

		DISABLE_INTERRUPT;
		if (finished != nullptr) {
			queue_.push(CALLBACK);
			queue_.push(finished->index_);
		} else {
			queue_.push(VOID);
		}

		if (sizeof(E) <= sizeof(double))	// SmallFixedEvent
				{
			pushFixed(event->index_, (uint8_t*) &e, sizeof(E));
		} else	// BigFixedEvent
		{
			MemPool<E> *tempPool = static_cast<BigFixedEvent<E>*>(event)->pool_;
			if (tempPool == nullptr)
				Error_Handler();
			void *mem = tempPool->Alloc();
			if (!mem) {
#ifdef RELEASE
					return;
	#else
				Error_Handler();
#endif
			}

			memcpy(mem, &e, sizeof(E));
			pushFixed(event->index_, (uint8_t*) &mem, sizeof(void*));
		}
		ENABLE_INTERRUPT;

		next_();
	}

	void delay(uint32_t ms) {
		if (queue_.freeSpace() < 5) {
			return;
		}

		DISABLE_INTERRUPT;
		queue_.push(DELAY);
		queue_.push((ms >> 24) & 0xFF);
		queue_.push((ms >> 16) & 0xFF);
		queue_.push((ms >> 8) & 0xFF);
		queue_.push(ms & 0xFF);
		ENABLE_INTERRUPT;
		next_();
	}

	void done() {
		busy_ = false;
		next_();
	}

	void done(uint8_t error) {
		busy_ = false;
		if (finished_ != nullptr)
			finished_->post(error);
		next_();
	}
private:
	inline void popFixed(uint8_t *data, size_t size) override
	{
		for (size_t i = 0; i < size; i++) {
			data[i] = queue_.pop();
		}
	}

	inline void pushFixed(uint8_t index, uint8_t *data, size_t size) override
	{
		queue_.push(index);
		for (uint8_t i = 0; i < size; i++) {
			queue_.push(data[i]);
		}
	}

	enum EventType {
		VOID = 1, CALLBACK, DELAY
	};

	void next_() {
		if (busy_ || queue_.empty())
			return;
		executeEvent_.post();
		busy_ = true;
	}

	void timeout_() {
		busy_ = false;
		next_();
	}

	void execute_() {
		uint8_t type = queue_.pop();
		if (type == DELAY) {
			uint32_t time = 0U;
			time = queue_.pop();
			time <<= 8;
			time += queue_.pop();
			time <<= 8;
			time += queue_.pop();
			time <<= 8;
			time += queue_.pop();
			timer_.start(time, 1);
			finished_ = nullptr;
		} else if (type == VOID) {
			finished_ = nullptr;
			uint8_t index = 0U;
			index = queue_.pop();
			if (index < events_.poolSize_) {
				Event *e = events_.events_[index];
				e->execute(this);
			}
		} else if (type == CALLBACK) {
			uint8_t index = 0U;
			index = queue_.pop();
			if (index < events_.poolSize_) {
				finished_ = (ByteEvent*) events_.events_[index];
			} else {
				finished_ = nullptr;
			}

			index = queue_.pop();
			if (index < events_.poolSize_) {
				Event *e = events_.events_[index];
				e->execute(this);
			}
		}
	}
private:
	ByteEvent *finished_ = nullptr;
	EmptyEvent executeEvent_ = EmptyEvent(this,
			static_cast<EmptyEvent::Handler>(&Strand::execute_));
	EventQueue &events_ = Engine::instance().events();
	Timer timer_ = Timer(this, static_cast<Timer::Handler>(&Strand::timeout_));
	Queue<uint8_t> &queue_;
	bool busy_ = false;
};
}

#define M_STRAND(name, size)\
private:\
    QUEUE_DEF(name##Queue, size, uint8_t)\
public:\
    core::Strand name##Strand { core::Strand(name##Queue##_) };

#endif // STRAND_H
