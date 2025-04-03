#ifndef CORE_TASK_H_
#define CORE_TASK_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct Task task_t;
typedef void (*TaskHandler)();

/**
 * @brief Task structure.
 */
typedef struct Task
{
	task_t* next;            ///< Pointer to the next task in the list.
	uint32_t interval;       ///< Time interval for the task.
	uint64_t nextTick;       ///< Next tick time for the task.
	int32_t loop;            ///< Loop count for the task, -1 for infinite.
	TaskHandler handler;     ///< Handler function for the task.
} task_t;

/**
 * @brief Handles task's information. Only used in engine.
 */
void Task_Run();

/**
 * @brief Starts a task with a given interval and loop count.
 * @param task Pointer to the task structure.
 * @param interval Interval for task execution (in milliseconds).
 * @param loop Number of times the task should run, -1 for infinite.
 */
void Task_Start(task_t* task, uint32_t interval, int32_t loop);

/**
 * @brief Stops a running task.
 * @param task Pointer to the task structure.
 */
void Task_Stop(task_t* task);

/**
 * @brief Checks if a task is currently running.
 * @param task Pointer to the task structure.
 * @return True if the task is running, false otherwise.
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

#define _M_TASK_START_3(name, interval, loop)	Task_Start(&name##Task, interval, loop)
#define _M_TASK_START_2(name, interval)	Task_Start(&name##Task, interval, 0)

#define _TASK_NARGS3(_1, _2, _3, N, ...) N
#define _TASK_NARGS(...) _TASK_NARGS3(__VA_ARGS__, 3, 2)
#define _TASK_CHOOSER2(count) _M_TASK_START_ ## count
#define _TASK_CHOOSER(count) _TASK_CHOOSER2(count)

#define M_TASK_START(...) _TASK_CHOOSER(_TASK_NARGS(__VA_ARGS__))(__VA_ARGS__)

#define M_TASK_STOP(name)					Task_Stop(&name##Task)
#define M_TASK_HANDLER(name)				void name##TaskHandler##_()


#endif /* CORE_TASK_H_ */
