#ifndef CORE_BASE_H
#define CORE_BASE_H

#include <cstdint>
#include "main.h"

#define _M_MACRO_2(_1,_2,NAME,...) NAME
#define _M_MACRO_3(_1,_2,_3,NAME,...) NAME
#define _M_MACRO_4(_1,_2,_3,_4,NAME,...) NAME

namespace core {
class AbstractEventQueue {
public:
	virtual void pushFixed(uint8_t index, uint8_t *data, size_t size) = 0;
	virtual void popFixed(uint8_t *data, size_t size) = 0;
};

class Component {
public:
	virtual void init() {}
	virtual ~Component() {}
};

class Event {
public:
#ifdef MONITOR_EVENT_TIME_EXECUTION
	typedef struct EvTimeExecution
	{
		uint32_t min_time;
		uint32_t max_time;
		uint32_t last_exec_time;
	} event_time_exe_t;

	event_time_exe_t timeExecution {
		UINT32_MAX, // min_time
		0,          // max_time
		0U          // last_exec_time
	};

#endif
	Event();
	virtual ~Event() {}

protected:
	virtual void execute(core::AbstractEventQueue *queue) = 0;
	Event(uint8_t index) : index_(index) {}
	uint8_t index_;
	friend class EventQueue;
	friend class Strand;
};
}

#define COMPONENT(module, name, ...) \
namespace module { \
class name : public core::Component, ##__VA_ARGS__ { \
    using CLASS = module::name; \
public: \
    static name& instance() { static name instance; return instance; } \
private: \
    virtual ~name() = default; \
    name() = default;

#define COMPONENT_END };}
#define COMPONENT_REG(module, name) module::name::instance().init();

#define HAL_DEF(module)\
namespace module{\
class HAL{\
public:

#define HAL_END };}

#endif // COMPONENT_H
