#ifndef CORE_EVENT_H
#define CORE_EVENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

/**
 * @brief Function pointer type for event handlers.
 *
 * Event handlers are functions that process events. They take a pointer to
 * user-defined data as their parameter.
 */
typedef void (*EventHandler)(void*);

/**
 * @brief Structure for tracking event execution times.
 *
 * This structure is used to monitor the minimum, maximum, and last execution
 * times of events. It is only included in debug builds.
 */
typedef struct EvTimeExecution {
    uint32_t min_time;       /**< Minimum execution time of the event. */
    uint32_t max_time;       /**< Maximum execution time of the event. */
    uint32_t last_exec_time; /**< Last execution time of the event. */
} event_time_exe_t;

/**
 * @brief Structure representing an event.
 *
 * This structure defines an event, including its handler, size, and index.
 * In debug builds, it also includes execution time tracking.
 */
typedef struct Event {
#ifndef NDEBUG
    event_time_exe_t time;   /**< Execution time tracking (debug builds only). */
#endif
    EventHandler handler;    /**< Function pointer to the event's handler. */
    uint8_t size;            /**< Size of the event's data. */
    uint8_t index;           /**< Index of the event in the event queue. */
} event_t;

/**
 * @brief Structure representing an event queue.
 *
 * This structure manages a queue of events, allowing efficient insertion,
 * removal, and processing of events.
 */
typedef struct EventQueue {
    event_t** events;        /**< events pool */

    uint16_t size;           /**< Total size of the event queue. */
    uint16_t minFree;        /**< Minimum free space in the queue (for monitoring). */
    uint8_t* first;          /**< Pointer to the first event in the queue. */
    uint8_t* last;           /**< Pointer to the last event in the queue. */
    uint8_t* inPtr;          /**< Pointer to the current insertion position. */
    uint8_t* outPtr;         /**< Pointer to the current removal position. */

    uint8_t* dataBuf;        /**< Buffer for storing event data. */
    uint8_t maxEvSize;       /**< Maximum size of an individual event. */
    uint8_t maxPoolSize;     /**< Maximum number of events in the pool. */
    uint8_t poolSize;        /**< Current number of events in the pool. */
} event_queue_t;

/**
 * @brief Initializes an event.
 *
 * This function sets up an event with the specified size and handler.
 *
 * @param ev Pointer to the event to be initialized.
 * @param size Size of the event's data.
 * @param handler Function pointer to the event's handler.
 */
void Event_Init(event_t* ev, uint8_t size, EventHandler handler);

/**
 * @brief Posts an event to the event queue.
 *
 * This function adds an event to the queue for processing. The event's data
 * can optionally be provided.
 *
 * @param index Index of the event in the queue.
 * @param data Pointer to the data associated with the event (optional).
 * @return `true` if the event was successfully posted, `false` otherwise.
 */
bool Event_Post(uint8_t index, void* data);

/**
 * @brief Processes events in the event queue.
 *
 * This function loops through the event queue and executes the handlers
 * for all pending events.
 *
 * @return `true` if there are more events to process, `false` otherwise.
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
