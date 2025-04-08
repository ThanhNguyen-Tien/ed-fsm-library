#ifndef CORE_EVENT_H
#define CORE_EVENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

typedef void (*EventHandler)(void*);

typedef struct EvTimeExecution{
    uint32_t min_time;
    uint32_t max_time;
    uint32_t last_exec_time;
} event_time_exe_t;

typedef struct Event
{
#ifndef NDEBUG
	event_time_exe_t time;
#endif
	EventHandler handler;
	uint8_t size;
	uint8_t index;
}event_t;

typedef struct EventQueue
{
	event_t** events;

	uint16_t size;
	uint16_t minFree;
	uint8_t* first;
	uint8_t* last;
	uint8_t* inPtr;
	uint8_t* outPtr;

	uint8_t* dataBuf;
	uint8_t maxEvSize;
	uint8_t maxPoolSize;
	uint8_t poolSize;
}event_queue_t;

void Event_Init(event_t* ev, uint8_t size, EventHandler handler);
bool Event_Post(uint8_t index, void* data);
bool Event_Loop();

#define M_EVENT_DEF(name)\
	extern event_t name##Event;

#define M_EVENT(name)\
	event_t name##Event;\
	void name##EventHandler_(void * data);

//Fixed Event
#define _M_EVENT_POST_2(name, data)\
	Event_Post(name##Event.index, &data)

// Empty Event
#define _M_EVENT_POST_1(name)\
	Event_Post(name##Event.index, NULL)

#define M_EVENT_HANDLER(name) void name##EventHandler_(void *data)

#define _EVENT_NARGS2(_1, _2, N, ...) N
#define _EVENT_NARGS(...) _EVENT_NARGS2(__VA_ARGS__, 2, 1)
#define _EVENT_CHOOSER2(count) _M_EVENT_POST_ ## count
#define _EVENT_CHOOSER(count) _EVENT_CHOOSER2(count)

#define M_EVENT_POST(...) _EVENT_CHOOSER(_EVENT_NARGS(__VA_ARGS__))(__VA_ARGS__)

#define _M_EVENT_INIT_1(name)\
	Event_Init(&name##Event, 0, &name##EventHandler_)

#define _M_EVENT_INIT_2(name, size)\
	Event_Init(&name##Event, size, &name##EventHandler_)

#define _EVENT_INIT_NARGS2(_1, _2, N, ...) N
#define _EVENT_INIT_NARGS(...) _EVENT_INIT_NARGS2(__VA_ARGS__, 2, 1)
#define _EVENT_INIT_CHOOSER2(count) _M_EVENT_INIT_ ## count
#define _EVENT_INIT_CHOOSER(count) _EVENT_INIT_CHOOSER2(count)

#define M_EVENT_INIT(...) _EVENT_INIT_CHOOSER(_EVENT_NARGS(__VA_ARGS__))(__VA_ARGS__)

#endif /*CORE_EVENT_H*/
