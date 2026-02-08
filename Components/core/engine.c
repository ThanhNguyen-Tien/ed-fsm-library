#include "engine.h"
#include "system.h"

engine_t engine;
M_TASK(calculateCpuLoad)

static inline void Engine_StartTask_(task_t *task)
{
	ASSERT(task != NULL);
	task_t *prev = NULL;
	task_t *it;
	for (it = engine.taskLists; it != NULL; it = it->next)
	{
		if (it == task)
		{
			if (prev == NULL)
				engine.taskLists = it->next;
			else
				prev->next = it->next;
			task->next = engine.activeTasks;
			engine.activeTasks = task;
			break;
		}
		prev = it;
	}
	if (engine.nextTick > task->nextTick)
		engine.nextTick = task->nextTick;
}

static inline void Engine_StopTask_(task_t *task)
{
	ASSERT(task != NULL);
	task_t *prev = NULL;
	task_t *it;
	for (it = engine.activeTasks; it != NULL; it = it->next)
	{
		if (it == task)
		{
			if (prev == NULL)
				engine.activeTasks = it->next;
			else
				prev->next = it->next;
			task->next = engine.taskLists;
			engine.taskLists = task;
			break;
		}
		prev = it;
	}
}

static inline void Engine_Idle_(void)
{
    engine.cpu_stats.last_idle_start = DWT->CYCCNT;
    engine.cpu_stats.in_idle = 1;

	WAIT_FOR_INTERUPT;
}

inline void Engine_ISR_Enter(void)
{
    if (engine.cpu_stats.in_idle)
    {
        uint32_t now = DWT->CYCCNT;
        engine.cpu_stats.idle_cycles += (now - engine.cpu_stats.last_idle_start);
        engine.cpu_stats.in_idle = 0;
    }
}


static void Engine_StartTask(void *data)
{
	task_t *task = *(task_t**) data;
	ASSERT(task != NULL);

	Engine_StartTask_(task);
}

static void Engine_StopTask(void *data)
{
	task_t *task = *(task_t**) data;
	ASSERT(task != NULL);

	Engine_StopTask_(task);
}

static void Engine_ExecuteTask()	// Event Handler
{
	uint64_t min = LAST_TICK;
	task_t *it = engine.activeTasks;
	task_t *next;
	while (it != NULL)
	{
		next = it->next;
		if (engine.tickCount >= it->nextTick)
		{
			Task_Run(it);
		}
		if (min > it->nextTick)
			min = it->nextTick;
		it = next;
	}
	engine.nextTick = min;
}

void Engine_Init(uint8_t *buf, uint16_t size, uint8_t *dataBuf,
		uint8_t maxEvSize, event_t **evPool, uint8_t evPoolSize)
{
	engine.eventQueue.size = size;
	engine.eventQueue.minFree = size;
	engine.eventQueue.first = buf;
	engine.eventQueue.last = engine.eventQueue.first + size;
	engine.eventQueue.inPtr = engine.eventQueue.first;
	engine.eventQueue.outPtr = engine.eventQueue.first;

	engine.eventQueue.dataBuf = dataBuf;
	engine.eventQueue.maxEvSize = maxEvSize;
	engine.eventQueue.events = evPool;
	engine.eventQueue.maxPoolSize = evPoolSize;
	engine.eventQueue.poolSize = 1;

	Event_Init(&engine.checkTask, 0, &Engine_ExecuteTask);
	Event_Init(&engine.startTask, sizeof(task_t*), &Engine_StartTask);
	Event_Init(&engine.stopTask, sizeof(task_t*), &Engine_StopTask);

	engine.tickCount = 0;
	engine.nextTick = LAST_TICK;

	M_TASK_INIT(calculateCpuLoadTask);

#ifndef NDEBUG
	DWT_Init();
	M_TASK_START(calculateCpuLoadTask, 1000);
#endif
}

void Engine_Run()
{
	while (1)
	{
		if (Event_Loop())
		{
			continue;
		}
		Engine_Idle_();
	}
}

void Engine_Delay(uint32_t t)
{
	uint64_t timeout = engine.tickCount + t;
	while (engine.tickCount < timeout)
	{
		NO_OPERATION;
	}
}

void Engine_RegisterTask(task_t *task)
{
	ASSERT(task != NULL);
	task->nextTick = 0;
	task->interval = 0;
	task->loop = -1;
	task->data = NULL;

	task->next = engine.taskLists;
	engine.taskLists = task;
}

uint16_t Get_Min_Free_Ev_Queue()
{
	return engine.eventQueue.minFree;
}

inline void Engine_Check_Task()
{
	if (++engine.tickCount >= engine.nextTick)
	{
		Event_Post(engine.checkTask.index, NULL);
	}
}

M_TASK_HANDLER(calculateCpuLoad)
{
    uint32_t now = DWT->CYCCNT;
    uint32_t elapsed = now - engine.cpu_stats.window_start;

	engine.cpu_stats.cpu_load =
		((float)((elapsed - engine.cpu_stats.idle_cycles)) / (float)elapsed) * 100;

	engine.cpu_stats.idle_cycles = 0;
	engine.cpu_stats.window_start = now;
}
