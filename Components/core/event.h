#ifndef CORE_EVENT_H
#define CORE_EVENT_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Function pointer type for event handlers.
 */
typedef void (*EventHandler)(void*);

/**
 * @brief Structure for tracking event execution time.
 */
typedef struct EvTimeExecution{
    uint32_t min_time;       ///< Minimum execution time.
    uint32_t max_time;       ///< Maximum execution time.
    uint32_t last_exec_time; ///< Last recorded execution time.
} event_time_exe_t;

/**
 * @brief Structure representing an event.
 */
typedef struct Event
{
#ifndef NDEBUG
    event_time_exe_t time; ///< Execution time tracking (enabled in debug mode).
#endif
    EventHandler handler; ///< Function pointer to the event handler.
    uint8_t size;         ///< Size of the event data.
    uint8_t index;        ///< Event index.
} event_t;

/**
 * @brief Event queue structure.
 */
typedef struct EventQueue
{
    event_t** events;      ///< event pool.
    uint16_t size;         ///< Total queue size.
    uint16_t minFree;      ///< Minimum free space recorded.
    uint8_t* first;        ///< Pointer to the first event in the queue.
    uint8_t* last;         ///< Pointer to the last event in the queue.
    uint8_t* inPtr;        ///< Pointer for inserting new events.
    uint8_t* outPtr;       ///< Pointer for extracting events.
    uint8_t* dataBuf;      ///< Buffer for event data storage.
    uint8_t maxEvSize;     ///< Maximum size of an event.
    uint8_t maxPoolSize;   ///< Maximum event pool size.
    uint8_t poolSize;      ///< Current pool size.
} event_queue_t;

/**
 * @brief Initializes an event.
 * @param ev Pointer to the event structure.
 * @param size Size of the event data.
 * @param handler Function pointer to the event handler.
 */
void Event_Init(event_t* ev, uint8_t size, EventHandler handler);

/**
 * @brief Posts an event to the queue.
 * @param index Event index.
 * @param data Pointer to the event data.
 * @return True if the event was posted successfully, false otherwise.
 */
bool Event_Post(uint8_t index, void* data);

/**
 * @brief Processes events in the event loop.
 * @return True if EventQueue has data, false if the EventQueue empty.
 */
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
