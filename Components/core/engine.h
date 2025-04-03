#ifndef CORE_ENGINE_H
#define CORE_ENGINE_H

#include "task.h"
#include "event.h"
#include "system.h"

#define LAST_TICK           0xFFFFFFFFFFFFFFFF

/**
 * @brief Main engine structure.
 */
typedef struct Engine
{
    task_t* taskLists;        ///< List of all tasks in the system.
    task_t* activeTasks;      ///< List of active tasks.
    event_t checkTask;        ///< Event to check tasks.
    event_queue_t eventQueue; ///< Event queue.
    volatile uint64_t tickCount; ///< Real-time tick counter.
    uint64_t nextTick;        ///< Next scheduled tick.
} engine_t;

extern engine_t engine; ///< Global engine instance.

/**
 * @brief Initializes the engine.
 * @param buf Memory buffer for the event queue.
 * @param size Size of the event queue.
 * @param dataBuf Memory buffer for event data.
 * @param maxEvSize Maximum event size.
 * @param evPool Pool of reusable events.
 * @param evPoolSize Number of elements in the event pool.
 *
 * This function must be called first before using the engine.
 */
void Engine_Init(uint8_t* buf, uint16_t size,
                 uint8_t* dataBuf, uint8_t maxEvSize,
                 event_t** evPool, uint8_t evPoolSize);

/**
 * @brief Runs the main engine loop.
 *
 * This function executes tasks and processes events in an infinite loop.
 */
void Engine_Run();

/**
 * @brief Creates a blocking delay.
 * @param t Delay time in milliseconds.
 *
 * Warning: This function blocks CPU execution and should be used only in limited contexts.
 */
void Engine_Delay(uint32_t t);

/**
 * @brief Gets the current CPU usage.
 * @return Value of the CPU usage.
 *
 * Warning: This function blocks CPU execution and should be used only in limited contexts.
 */
float Get_Cpu_Usage();

// The following functions are internal and should not be used by the user.

/**
 * @brief Registers a task in the system.
 */
void Engine_RegisterTask(task_t *task);

/**
 * @brief Starts a task.
 */
void Engine_StartTask(task_t *task);

/**
 * @brief Stops a running task.
 */
void Engine_StopTask(task_t *task);

/**
 * @brief Checks the status of tasks and handles related events.
 */
void Engine_CheckTask();

// The following functions can be overridden by the user in system.h

/**
 * @brief Initializes the cycle measurement system.
 *
 * The user can override this function to customize cycle measurement initialization.
 */
__attribute__((weak)) void Cycle_Measurement_Init(void);

/**
 * @brief Retrieves the CPU cycle count.
 *
 * The user can override this function if a different method is needed.
 */
__attribute__((weak)) uint32_t Get_Cycle_Count(void);

/**
 * @brief Retrieves the CPU frequency in Hz.
 *
 * The user can override this function if a different method is needed.
 */
__attribute__((weak)) uint32_t Get_CPU_Freq_Hz(void);

/**
 * @brief Macro for initializing the engine with specified parameters.
 */
#define ENGINE_INIT     Engine_Init(evQueue, EVENT_QUEUE_SIZE , tempDataForHandler, MAX_EVENT_SIZE, evPool, EVENT_POOL_SIZE)

#endif // CORE_ENGINE_H
