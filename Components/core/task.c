#include "engine.h"

void Task_Start(task_t *task, uint32_t interval, int32_t loop, void *payload)
{
	ASSERT(task != NULL);

	task->interval = interval;
	task->nextTick = engine.tickCount + interval;
	task->loop = loop;
	task->data = payload;
	Event_Post(engine.startTask.index, &task);
}

void Task_Stop(task_t *task)
{
	ASSERT(task != NULL);

	task->loop = 0;
	task->nextTick = LAST_TICK - 1;
	Event_Post(engine.stopTask.index, &task);
}

bool Task_IsRunning(task_t *task)
{
	ASSERT(task != NULL);

	return (task->loop != 0);
}

void Task_Run(task_t *task)
{
	ASSERT(task != NULL);

	if (--task->loop == 0)
	{
		task->nextTick = LAST_TICK - 1;
		Event_Post(engine.stopTask.index, &task);
	} else
	{
		task->nextTick += task->interval;
		if (task->loop < 0)
			task->loop = -1;
	}

	task->handler(task->data);
}
