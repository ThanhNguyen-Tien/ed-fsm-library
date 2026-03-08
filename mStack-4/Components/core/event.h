#ifndef CORE_EVENT_H
#define CORE_EVENT_H

#include <core/engine.h>
#include <core/mem-pool.h>
#include <cstdint>
#include <string.h>
#include <cassert>

namespace core {
class EmptyEvent: public Event {
public:
	using Handler = void (Component::*)();
	EmptyEvent(Component *component, Handler handler) :
			component_(component), handler_(handler) {
	}
	bool post() {
		this->timestamp = DWT->CYCCNT;
		return Engine::instance().events().postSlot(index_, (uint32_t)0U);
	}

private:
	void execute(const EventPayload& payload) override {
		UNUSED(payload);
		(component_->*handler_)();
	}

	inline void execute_() {
		(component_->*handler_)();
	}

	Component *component_ = nullptr;
	Handler handler_ = nullptr;
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

	virtual void* allocPayload() { return nullptr; }	// Only used for BigFixedEvent, SmallFixedEvent will ignore this and directly copy data into payload.u;
	virtual bool post(const E &e) = 0;

protected:
	virtual void execute(const EventPayload& payload) = 0;

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

	bool post(const E &e) override {
		EventPayload p {};
		memcpy(&p.u, &e, sizeof(E));
		this->timestamp = DWT->CYCCNT;
		return Engine::instance().events().postSlot(this->index_, p);
	}

private:
	void execute(const EventPayload& payload) override
	{
		E e;
		memcpy(&e, &payload.u, sizeof(E));
		(this->component_->*this->handler_)(e);
	}
};

template<typename E, uint8_t N>
class BigFixedEvent: public FixedEvent<E> {
	static_assert(sizeof(E) > sizeof(uint32_t), "BigFixedEvent payload must be > 4 bytes");
	static_assert(alignof(E) >= 4, "Event Data Type must be 4-byte aligned! Use 'alignas(4)' on your data.");
public:
	using Base = FixedEvent<E>;
	using Handler = typename Base::Handler;

    static constexpr size_t MEMORY_SIZE =
        N * MemPool<E>::STRIDE;

	BigFixedEvent(Component *component, Handler handler) :
			Base(component, handler), pool_(buffer_, N) {}

    void* allocPayload() override
    {
        return pool_.Alloc();
    }

    bool post(const E& e) override
    {
        void* mem = allocPayload();

        if (!mem)
        {
        	Error_Handler();
			return false;
        }

        memcpy(mem, &e, sizeof(E));

        this->timestamp = DWT->CYCCNT;

        if (!Engine::instance().events().postSlot(this->index_, mem))
        {
            pool_.Free(mem);
            return false;
        }

        return true;
    }

private:
    void execute(const EventPayload& payload) override
    {
        E* payloadPtr = static_cast<E*>(payload.p);
        (this->component_->*this->handler_)(*payloadPtr);

        pool_.Free(payloadPtr);
    }

private:
    alignas(MemPool<E>::FINAL_ALIGN) uint8_t buffer_[N * MemPool<E>::STRIDE];
	MemPool<E> pool_;
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
    core::BigFixedEvent<type, num> name##Event = \
        core::BigFixedEvent<type, num>( \
            this, (typename core::BigFixedEvent<type, num>::Handler)&CLASS::name##Handler##_); \
private: \
    void name##Handler##_(const type& event);

// Handlers
#define _M_EVENT_HANDLER(cls, name) \
    void cls::name##Handler##_()

#define _M_FIXED_EVENT_HANDLER(cls, name, type) \
    void cls::name##Handler##_(const type& event)

#endif // MODEL_H
