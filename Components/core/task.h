#ifndef CORE_TASK_H_
#define CORE_TASK_H_

#include "event.h"

// Forward declaration of the Task structure.
typedef struct Task task_t;

/**
 * @brief Structure representing a scheduled task.
 *
 * In this library, concept of task is used to manage periodic or delayed execution of functions.
 * Contains timing, handler, and user data for task management.
 */
typedef struct Task
{
    task_t *next;           ///< Pointer to the next task in the list.
    uint32_t interval;      ///< Interval between task executions (in ticks or ms).
    uint64_t nextTick;      ///< Next scheduled tick for task execution.
    int32_t loop;           ///< Number of times to repeat the task (-1 for infinite).
    EventHandler handler;   ///< Function pointer to the task handler.
    void *data;             ///< User-defined data payload for the task.
} task_t;

/**
 * @brief Runs all scheduled tasks that are due.
 *
 * This function checks all registered tasks and executes those whose scheduled time has arrived.
 */
void Task_Run();

/**
 * @brief Starts a task with the specified interval, loop count, and payload.
 *
 * @param task Pointer to the task structure.
 * @param interval Time interval between executions.
 * @param loop Number of times to repeat (-1 for infinite).
 * @param payload Pointer to user data for the task.
 */
void Task_Start(task_t *task, uint32_t interval, int32_t loop, void *payload);

/**
 * @brief Stops the specified task from running.
 *
 * @param task Pointer to the task structure to stop.
 */
void Task_Stop(task_t *task);

/**
 * @brief Checks if the specified task is currently running.
 *
 * @param task Pointer to the task structure.
 * @return true if the task is running, false otherwise.
 */
bool Task_IsRunning(task_t *task);

#define M_TASK_DEF(name)\
	extern task_t name##Task;

#define M_TASK(name)\
	task_t name##Task;\
	void name##TaskHandler##_();

#define M_TASK_INIT(name)\
	Engine_RegisterTask(&name);\
	name.handler = &name##Handler_;

#define _M_TASK_START_3(name, interval, loop)	Task_Start(&name, interval, loop, NULL)
#define _M_TASK_START_2(name, interval)	Task_Start(&name, interval, 0, NULL)

#define _TASK_NARGS3(_1, _2, _3, N, ...) N
#define _TASK_NARGS(...) _TASK_NARGS3(__VA_ARGS__, 3, 2)
#define _TASK_CHOOSER2(count) _M_TASK_START_ ## count
#define _TASK_CHOOSER(count) _TASK_CHOOSER2(count)

#define M_TASK_START(...) _TASK_CHOOSER(_TASK_NARGS(__VA_ARGS__))(__VA_ARGS__)

#define M_TASK_STOP(name)					Task_Stop(&name)
#define M_TASK_HANDLER(name)				void name##TaskHandler##_(void)

#endif /* CORE_TASK_H_ */
