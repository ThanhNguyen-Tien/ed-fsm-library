#ifndef CORE_TASK_H_
#define CORE_TASK_H_

#include <stdint.h>
#include <stdbool.h>
#include "event.h"

/**
 * @brief Forward declaration of the Task structure.
 */
typedef struct Task task_t;

/**
 * @brief Structure representing a task in the system.
 *
 * This structure holds information about a task, including its execution interval,
 * the next tick for execution, the number of loops, the handler function, and any
 * associated user data.
 */
typedef struct Task
{
    task_t* next;              /**< Pointer to the next task in the task list. */
    uint32_t interval;         /**< Interval for task execution. */
    uint64_t nextTick;         /**< Tick count when the task should execute next. */
    int32_t loop;              /**< Number of times the task should loop (-1 for infinite). */
    EventHandler handler;      /**< Function pointer to the task's handler. */
    void* data;                /**< Pointer to data for the task. */
}task_t;

/**
 * @brief Executes a task and updates its state.
 *
 * This function processes a given task by decrementing its loop counter,
 * updating its next execution tick, and invoking its handler function.
 * If the task's loop counter reaches zero, it is marked for stopping.
 *
 * @param task Pointer to the task to be executed. Must not be NULL.
 *
 * @note This function assumes that the task's handler and data are properly
 * initialized. It is the caller's responsibility to ensure the validity
 * of the task structure.
 */
void Task_Run(task_t* task);

/**
 * @brief Starts a task with the specified parameters.
 *
 * @param task Pointer to the task to be started.
 * @param interval Interval for task executions.
 * @param loop Number of times the task should loop (-1 for infinite).
 * @param payload Pointer to a context to be passed to the task handler.
 *
 * @note If the task is already running, its parameters will be updated.
 */
void Task_Start(task_t* task, uint32_t interval, int32_t loop, void* payload);

/**
 * @brief Stops a running task.
 *
 * @param task Pointer to the task to be stopped.
 *
 * @note Once stopped, the task will no longer be executed until it is started again.
 */
void Task_Stop(task_t* task);

/**
 * @brief Checks if a task is currently running.
 *
 * @param task Pointer to the task to be checked.
 * @return `true` if the task is running, `false` otherwise.
 */
bool Task_IsRunning(task_t* task);

#define M_TASK_DEF(name)\
	extern task_t name##Task;

#define M_TASK(name)\
	task_t name##Task;\
	void name##TaskHandler##_();

#define M_TASK_INIT(name)\
	Engine_RegisterTask(&name##Task);\
	name##Task.handler = &name##TaskHandler_;

#define _M_TASK_START_3(name, interval, loop)	Task_Start(&name##Task, interval, loop, NULL)
#define _M_TASK_START_2(name, interval)	Task_Start(&name##Task, interval, 0, NULL)

#define _TASK_NARGS3(_1, _2, _3, N, ...) N
#define _TASK_NARGS(...) _TASK_NARGS3(__VA_ARGS__, 3, 2)
#define _TASK_CHOOSER2(count) _M_TASK_START_ ## count
#define _TASK_CHOOSER(count) _TASK_CHOOSER2(count)

#define M_TASK_START(...) _TASK_CHOOSER(_TASK_NARGS(__VA_ARGS__))(__VA_ARGS__)

#define M_TASK_STOP(name)					Task_Stop(&name##Task)
#define M_TASK_HANDLER(name)				void name##TaskHandler##_(void *data)


#endif /* CORE_TASK_H_ */
