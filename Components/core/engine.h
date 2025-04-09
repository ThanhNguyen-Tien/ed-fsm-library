#ifndef CORE_ENGINE_H
#define CORE_ENGINE_H

#include "task.h"
#include "event.h"
#include "system.h"

#define LAST_TICK           0xFFFFFFFFFFFFFFFF /**< Constant representing the last tick value. */

/**
 * @brief Structure representing the Engine.
 *
 * The Engine is responsible for managing tasks, events, and the system tick.
 * It provides functionality for task registration, event handling, and CPU usage monitoring.
 */
typedef struct Engine
{
    task_t* taskLists;          /**< Pointer to the list of registered tasks. */
    task_t* activeTasks;        /**< Pointer to the list of currently active tasks. */
    event_t checkTask;          /**< Event used to check tasks. */
    event_t startTask;          /**< Event used to start tasks. */
    event_t stopTask;           /**< Event used to stop tasks. */
    event_queue_t eventQueue;   /**< Event queue for managing system events. */
    volatile uint64_t tickCount;/**< Current system tick count (volatile for concurrency). */
    uint64_t nextTick;          /**< Tick count for the next scheduled task. */
} engine_t;

/**
 * @brief Global instance of the Engine.
 *
 * This instance is used throughout the system to manage tasks and events.
 */
extern engine_t engine;

/**
 * @brief Initializes the Engine.
 *
 * This function sets up the Engine with the specified buffers and event pool.
 *
 * @param buf Pointer to the buffer for the event queue.
 * @param size Size of the event queue buffer in bytes.
 * @param dataBuf Pointer to the buffer for event data.
 * @param maxEvSize Maximum size of an individual event.
 * @param evPool Pointer to the pool of reusable events.
 * @param evPoolSize Size of the event pool.
 */
void Engine_Init(uint8_t* buf, uint16_t size,
                uint8_t* dataBuf, uint8_t maxEvSize,
                event_t** evPool, uint8_t evPoolSize);

/**
 * @brief Runs the Engine.
 *
 * This function starts the main loop of the Engine, processing tasks and events.
 */
void Engine_Run();

/**
 * @brief Delays execution for a specified time.
 *
 * This function blocks execution for the specified duration. It should only
 * be used in limited contexts where blocking is acceptable.
 *
 * @param t Delay duration in milliseconds.
 *
 * @warning This function is blocking and should be used sparingly.
 */
void Engine_Delay(uint32_t t);

/**
 * @brief Retrieves the CPU usage.
 *
 * This function calculates and returns the current CPU usage as a percentage.
 *
 * @return The CPU usage as a floating-point value (0.0 to 100.0).
 */
float Get_Cpu_Usage();

/**
 * @brief Registers a task with the Engine.
 *
 * This function adds a task to the Engine's task list, allowing it to be executed.
 *
 * @param task Pointer to the task to be registered.
 */
void Engine_RegisterTask(task_t *task);

/**
 * @brief Checks and executes tasks.
 *
 * This function iterates through the list of tasks and executes any tasks
 * that are due for execution based on the current tick count.
 */
void Engine_CheckTask();

#define ENGINE_INIT 	Engine_Init(evQueue, EVENT_QUEUE_SIZE , tempDataForHandler, MAX_EVENT_SIZE, evPool, EVENT_POOL_SIZE)
#endif // ENGINE_H
