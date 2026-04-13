#ifndef CORE_EVENT_H
#define CORE_EVENT_H

#include <core/engine.h>
#include <core/mem-pool.h>
#include <cstdint>
#include <string.h>
#include <cassert>

namespace core
{
	class EmptyEvent : public Event
	{
	public:
		using Handler = void (Component::*)();

		EmptyEvent(Component* comp, Handler h) : component_(comp), handler_(h) {}

		bool post()
		{
			return Engine::instance().events().post(this->index_, nullptr, 0);
		}

		void execute(const void* payload) override
		{
			UNUSED(payload);
			(component_->*handler_)();
		}

	private:
		Component* component_;
		Handler handler_;
	};

	template <typename T>
	class FixedEvent : public Event
	{
	    static_assert(sizeof(T) % 4 == 0, "Payload size must be multiple of 4 bytes (uint32_t)");

	public:
	    using Handler = void (Component::*)(const T&);

	    FixedEvent(Component* comp, Handler h) : component_(comp), handler_(h) {}
	    static constexpr uint8_t NUM_OF_CHUNKS = (sizeof(T) / 4); // Num of 4-byte chunks in payload

	    bool post(const T& data)
	    {
	        const uint32_t* pData = static_cast<const uint32_t*>(static_cast<const void*>(&data));
	        return Engine::instance().events().post(this->index_, pData, NUM_OF_CHUNKS);
	    }

	    void execute(const void* payload) override
	    {
	    	assert(payload != nullptr); // Payload must not be null for FixedEvent
			const T& data = *static_cast<const T*>(payload);
			(component_->*handler_)(data);
	    }

	private:
	    Component* component_;
	    Handler handler_;
	};

	typedef FixedEvent<uint32_t> WordEvent;
}

#define M_EVENT(...) _M_MACRO_2(__VA_ARGS__, _M_FIXED_EVENT, _M_EVENT_EMPTY)(__VA_ARGS__)
#define M_EVENT_HANDLER(...) _M_MACRO_3(__VA_ARGS__, _M_FIXED_EVENT_HANDLER, _M_EVENT_HANDLER)(__VA_ARGS__)

// EmptyEvent
#define _M_EVENT_EMPTY(name) \
public: \
    core::EmptyEvent name##Event = core::EmptyEvent( \
        this, static_cast<core::EmptyEvent::Handler>(&CLASS::name##Handler##_ )); \
private: \
    void name##Handler##_();

// FixedEvent
#define _M_FIXED_EVENT(name, type) \
public: \
    core::FixedEvent<type> name##Event = core::FixedEvent<type>( \
        this, static_cast<typename core::FixedEvent<type>::Handler>(&CLASS::name##Handler##_ )); \
private: \
    void name##Handler##_(const type& event);

// Handlers
#define _M_EVENT_HANDLER(cls, name) \
    void cls::name##Handler##_()

#define _M_FIXED_EVENT_HANDLER(cls, name, type) \
    void cls::name##Handler##_(const type& event)

#endif // MODEL_H
