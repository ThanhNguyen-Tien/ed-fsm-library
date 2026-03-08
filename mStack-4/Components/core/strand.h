#ifndef CORE_STRAND_H
#define CORE_STRAND_H
#include <core/event.h>
#include <core/queue.h>
#include <core/system.h>
#include <core/event-queue.h>
#include <core/engine.h>
#include <core/timer.h>

namespace core {

class Strand: public Component {
public:
	Strand(Queue<EventSlot_t> &queue) :
			queue_(queue) {
	}

	void post(EmptyEvent *event, ByteEvent *finished = nullptr) {
		EventSlot_t slot {};

		if (finished != nullptr) {
			slot.event_id |= (CALLBACK) << 16U;
			slot.event_id |= (static_cast<uint32_t>(finished->index_) << 8U);
		} else {
			slot.event_id |= (VOID) << 16U;
		}
		slot.event_id |= (event->index_)&0xFFU;

		CRITICAL_SECTION_PRIORITY_BEGIN(1)
		if(!queue_.push(slot)) {
			Error_Handler();	// FIXME: Log error instead of halting system
		}
		CRITICAL_SECTION_PRIORITY_END

		next_();
	}

	template<typename E>
	void post(FixedEvent<E> *event, const E &e, ByteEvent *finished = nullptr) {
		EventSlot_t slot {};

		if (finished != nullptr) {
			slot.event_id |= (CALLBACK) << 16U;
			slot.event_id |= (static_cast<uint32_t>(finished->index_) << 8U);
		} else {
			slot.event_id |= (VOID) << 16U;
		}
		slot.event_id |= (event->index_)&0xFFU;

	    if /*constexpr*/ (sizeof(E) <= sizeof(uint32_t))
	    {
	        /* SmallFixedEvent */
	        slot.payload.u = 0;
	        memcpy(&slot.payload.u, &e, sizeof(E));
	    }
	    else
	    {
	        /* BigFixedEvent */
	        void* mem = event->allocPayload();

	        if (!mem)
	        {
	#ifdef RELEASE
	            return;
	#else
	            Error_Handler();
	#endif
	        }

	        memcpy(mem, &e, sizeof(E));
	        slot.payload.p = mem;
	    }

        CRITICAL_SECTION_PRIORITY_BEGIN(1)
        if (!queue_.push(slot)) {
            Error_Handler();
        }
        CRITICAL_SECTION_PRIORITY_END

	    next_();
	}

	void delay(uint32_t ms) {
		EventSlot_t slot {};

		slot.event_id |= (DELAY) << 16U;
		slot.payload.u = ms;

		CRITICAL_SECTION_PRIORITY_BEGIN(1)
		if(!queue_.push(slot)) {
			Error_Handler();	// FIXME: Log error instead of halting system
		}
		CRITICAL_SECTION_PRIORITY_END

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
	enum EventType : uint32_t {
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
		EventSlot_t slot {};
		if(!queue_.pop(slot)) {
			Error_Handler();	// FIXME: Log error instead of halting system
		}
		uint32_t type = (slot.event_id >> 16U) & 0xFFU;

		if (type == DELAY) {
			uint32_t time = slot.payload.u;
			timer_.start(time, 1);
			finished_ = nullptr;
		} else if (type == VOID) {
			finished_ = nullptr;
			uint8_t index = static_cast<uint8_t>(slot.event_id & 0xFFU);
			if (index < events_.poolSize_) {
				Event *e = events_.events_[index];
				e->execute(slot.payload);
			}
		} else if (type == CALLBACK) {
			uint8_t index = static_cast<uint8_t>((slot.event_id >> 8U) & 0xFFU);
			if (index < events_.poolSize_) {
				finished_ = (ByteEvent*) events_.events_[index];
			} else {
				finished_ = nullptr;
			}

			index = static_cast<uint8_t>(slot.event_id & 0xFFU);
			if (index < events_.poolSize_) {
				Event *e = events_.events_[index];
				e->execute(slot.payload);
			}
		}
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

#define M_STRAND(name, size)\
private:\
    QUEUE_DEF(name##Queue, size, core::EventSlot_t)\
public:\
    core::Strand name##Strand { core::Strand(name##Queue##_) };

#endif // STRAND_H
