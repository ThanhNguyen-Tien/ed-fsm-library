#ifndef CORE_TASK_H_
#define CORE_TASK_H_

#include <stdint.h>
#include <stdbool.h>
#include "event.h"

typedef struct Task task_t;

typedef struct Task
{
	task_t *next;
	uint32_t interval;
	volatile uint64_t nextTick;
	volatile int32_t loop;
	EventHandler handler;
	void *data;
} task_t;

void Task_Run(task_t *task);

void Task_Start(task_t *task, uint32_t interval, int32_t loop, void *payload);
void Task_Stop(task_t *task);
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
