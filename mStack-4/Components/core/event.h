#ifndef CORE_EVENT_H
#define CORE_EVENT_H

#include <cstdint>
#include <core/engine.h>
#include <string.h>
#include <core/mem-pool.h>
#include <cassert>

namespace core {
class EmptyEvent: public Event {
public:
	using Handler = void (Component::*)();
	EmptyEvent(Component *component, Handler handler) :
			component_(component), handler_(handler) {
	}
	void post() {
		EventPayload p; p.u = 0; // Empty Payload
		Engine::instance().events().postSlot(this->index_, p);
		this->timestamp = DWT->CYCCNT;
	}

private:
	void execute(EventPayload payload) override {
		UNUSED(payload);
		(component_->*handler_)();
	}

	inline void execute_() {
		(component_->*handler_)();
	}

	Component *component_ = nullptr;
	Handler handler_ = nullptr;
	friend class Strand;
    friend class EmptySignalOne;
    friend class EmptySignalMany;
};

template<typename E>
class FixedEvent: public Event {
public:
	using Handler = void (Component::*)(const E&);
	FixedEvent(Component *component, Handler handler) :
			component_(component), handler_(handler) {
	}

	virtual void post(const E &e) = 0;

protected:
	virtual void execute(EventPayload payload) = 0;

	inline void execute_(const E &e)
	{
		(this->component_->*this->handler_)(e);
	}

	Component *component_ = nullptr;
	Handler handler_ = nullptr;
    template <typename EV, typename EventE>
    friend class SignalOne;
    template <typename EV, typename EventE>
    friend class SignalMany;
};

template<typename E>
class SmallFixedEvent: public FixedEvent<E> {
	// Force user only use small datasize to optimize Slot 8-bytes
	static_assert(sizeof(E) <= sizeof(uint32_t), "SmallFixedEvent payload must be <= 4 bytes");

public:
	using Base = FixedEvent<E>;
	using Handler = typename Base::Handler;
	SmallFixedEvent(Component *component, Handler handler) :
			Base(component, handler) {}

	void post(const E &e) override {
		EventPayload p;
		p.u = 0;
		memcpy(&p.u, &e, sizeof(E));
		Engine::instance().events().postSlot(this->index_, p);
		this->timestamp = DWT->CYCCNT;
	}

private:
	void execute(EventPayload payload) override
	{
		E e;
		memcpy(&e, &payload.u, sizeof(E));
		(this->component_->*this->handler_)(e);
	}
};

template<typename E>
class BigFixedEvent: public FixedEvent<E> {
	static_assert(sizeof(E) > sizeof(uint32_t), "BigFixedEvent payload must be > 4 bytes");
	static_assert(alignof(E) >= 4, "Event Data Type must be 4-byte aligned! Use 'alignas(4)' on your data.");
public:
	using Base = FixedEvent<E>;
	using Handler = typename Base::Handler;

	BigFixedEvent(Component *component, Handler handler, uint8_t numOfMem) :
			Base(component, handler) {
		if (numOfMem == 0)
			Error_Handler();

		pool_ = new MemPool<E>(numOfMem);
		if (pool_ == nullptr)
			Error_Handler();
	}

	void post(const E &e) override
	{
		if (pool_ == nullptr)
			Error_Handler();

		void *mem = pool_->Alloc();
		if (!mem) {
#ifdef RELEASE
				return;
#else
			Error_Handler();
#endif
		}

		memcpy(mem, &e, sizeof(E));
		EventPayload payload;
		payload.p = mem; // Payload
		Engine::instance().events().postSlot(this->index_, payload);
		this->timestamp = DWT->CYCCNT;
	}

private:
	void execute(EventPayload payload) override
	{
	    E* payloadPtr = static_cast<E*>(payload.p);
	    (this->component_->*this->handler_)(*payloadPtr);

		pool_->Free(payloadPtr);
	}

private:
	MemPool<E> *pool_;
	friend class Strand;
};

typedef SmallFixedEvent<uint8_t> ByteEvent;
}

#define M_EVENT(...) _M_MACRO_3(__VA_ARGS__, _M_BIG_EVENT, _M_SMALL_EVENT, _M_EVENT_EMPTY)(__VA_ARGS__)
#define M_EVENT_HANDLER(...) _M_MACRO_3(__VA_ARGS__, _M_FIXED_EVENT_HANDLER, _M_EVENT_HANDLER)(__VA_ARGS__)

// EmptyEvent
#define _M_EVENT_EMPTY(name) \
public: \
    core::EmptyEvent name##Event = core::EmptyEvent( \
        this, (core::EmptyEvent::Handler)&CLASS::name##Handler##_ ); \
private: \
    void name##Handler##_();

// SmallFixedEvent
#define _M_SMALL_EVENT(name, type) \
public: \
    core::SmallFixedEvent<type> name##Event = core::SmallFixedEvent<type>( \
        this, (typename core::SmallFixedEvent<type>::Handler)&CLASS::name##Handler##_ ); \
private: \
    void name##Handler##_(const type& event);

// BigFixedEvent
#define _M_BIG_EVENT(name, type, num) \
public: \
    core::BigFixedEvent<type> name##Event = core::BigFixedEvent<type>( \
        this, (typename core::BigFixedEvent<type>::Handler)&CLASS::name##Handler##_, num ); \
private: \
    void name##Handler##_(const type& event);

// Handlers
#define _M_EVENT_HANDLER(cls, name) \
    void cls::name##Handler##_()

#define _M_FIXED_EVENT_HANDLER(cls, name, type) \
    void cls::name##Handler##_(const type& event)

#endif // MODEL_H
