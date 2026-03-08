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

	bool post(EmptyEvent *event, ByteEvent *finished = nullptr) {
		EventSlot_t *slot;

		do {
			slot = queue_.reserve();
			if (slot == nullptr) {
				Error_Handler();
				return false;
			}
		} while (slot != queue_.reserve());

		uint32_t id = 0;

		if (finished != nullptr) {
			id |= (CALLBACK << 16U);
			id |= (static_cast<uint32_t>(finished->index_) << 8U);
		} else {
			id |= (VOID << 16U);
		}

		id |= (event->index_ & 0xFFU);

		slot->event_id = id;

		__DMB();	// ensure payload visible before publish
		queue_.commit();

		next_();
		return true;
	}

	template<typename E>
	bool post(FixedEvent<E> *event, const E &e, ByteEvent *finished = nullptr) {
		EventSlot_t *slot;

		do {
			slot = queue_.reserve();
			if (slot == nullptr) {
				Error_Handler();
				return false;
			}
		} while (slot != queue_.reserve());

		uint32_t id = 0;

		if (finished != nullptr) {
			id |= (CALLBACK << 16U);
			id |= (static_cast<uint32_t>(finished->index_) << 8U);
		} else {
			id |= (VOID << 16U);
		}

		id |= (event->index_ & 0xFFU);

		slot->event_id = id;

		if (sizeof(E) <= sizeof(uint32_t)) {
			slot->payload.u = 0;
			memcpy(&slot->payload.u, &e, sizeof(E));
		} else {
			void *mem = event->allocPayload();

			if (!mem) {
				Error_Handler();
				return false;
			}

			memcpy(mem, &e, sizeof(E));
			slot->payload.p = mem;
		}

		__DMB();	// ensure payload visible before publish
		queue_.commit();

		next_();
		return true;
	}

	bool delay(uint32_t ms) {
		EventSlot_t *slot;

		do {
			slot = queue_.reserve();
			if (slot == nullptr) {
				Error_Handler();
				return false;
			}
		} while (slot != queue_.reserve());

		slot->event_id = (DELAY << 16U);
		slot->payload.u = ms;

		__DMB();	// ensure payload visible before publish
		queue_.commit();

		next_();
		return true;
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
		EventSlot_t slot { };
		if (!queue_.pop(slot)) {
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
