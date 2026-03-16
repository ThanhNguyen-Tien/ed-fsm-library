#ifndef CORE_BASE_H
#define CORE_BASE_H

#include <cstdint>
#include "main.h"

#define _M_MACRO_2(_1, _2, NAME, ...) NAME
#define _M_MACRO_3(_1, _2, _3, NAME, ...) NAME
#define _M_MACRO_4(_1, _2, _3, _4, NAME, ...) NAME

namespace core
{
	class Component
	{
	public:
		virtual void init() {}
		virtual ~Component() {}
	};

	union EventPayload
	{
		uint32_t u;
		void *p;

		EventPayload() : u(0) {}
		EventPayload(uint32_t v) : u(v) {}
		EventPayload(void *v) : p(v) {}
	};

	class Event
	{
	public:
		typedef struct EvTime
		{
			uint32_t min_time;
			uint32_t max_time;
			uint32_t value;
		} event_time_t;

		event_time_t timeExecution{
			UINT32_MAX, // min_time
			0,			// max_time
			0U			// value
		};

		event_time_t latency{
			UINT32_MAX, // min_time
			0,			// max_time
			0U			// value
		};

		void setPriority(uint8_t p) { priority_ = p & 0x07; }
		uint8_t getPriority() const { return priority_; }

		Event(uint8_t p = 0);
		virtual ~Event() {}

	protected:
		virtual void execute(const EventPayload &payload) = 0;
		Event(uint8_t index, uint8_t p = 0)
			: index_(index), priority_(p & 0x07) {}
		uint8_t index_;
		uint8_t priority_;
		template <uint8_t>
		friend class EventQueue;
		friend class Strand;
	};
}

#define CLASS(name, ...)             \
	class name : __VA_ARGS__         \
	{                                \
		using CLASS = name;          \
                                     \
	public:                          \
		name() = default;            \
		virtual ~name() = default;   \
                                     \
	private:                         \
		name(const name &) = delete; \
		name &operator=(const name &) = delete;

#define COMPONENT(module, name, ...)                       \
	namespace module                                       \
	{                                                      \
		class name : public core::Component, ##__VA_ARGS__ \
		{                                                  \
			using CLASS = module::name;                    \
                                                           \
		public:                                            \
			static name &instance()                        \
			{                                              \
				static name instance;                      \
				return instance;                           \
			}                                              \
                                                           \
		private:                                           \
			virtual ~name() = default;                     \
			name() = default;                              \
			name(const name &) = delete;                   \
			name &operator=(const name &) = delete;

#define COMPONENT_END \
	}                 \
	;                 \
	}
#define COMPONENT_REG(module, name) module::name::instance().init();

#define HAL_DEF(module) \
	namespace module    \
	{                   \
		class HAL       \
		{               \
		public:

#define HAL_END \
	}           \
	;           \
	}

#endif // COMPONENT_H
