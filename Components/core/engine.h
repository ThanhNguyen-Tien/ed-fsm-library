#ifndef CORE_ENGINE_H
#define CORE_ENGINE_H

#include "task.h"
#include "event.h"
#include "system.h"

#define LAST_TICK           0xFFFFFFFFFFFFFFFF

typedef struct Engine
{
	task_t* taskLists;
	task_t* activeTasks;
	event_t checkTask;
	event_queue_t eventQueue;
	volatile uint64_t tickCount;
	uint64_t nextTick;
}engine_t;

extern engine_t engine;

void Engine_Init(uint8_t* buf, uint16_t size,
				uint8_t* dataBuf, uint8_t maxEvSize,
				event_t** evPool, uint8_t evPoolSize);

void Engine_Run();
void Engine_Delay(uint32_t t);    // WARNING:  this function is blocking, use in some limited context only

void Engine_RegisterTask(task_t *task);
void Engine_StartTask(task_t *task);
void Engine_StopTask(task_t *task);
void Engine_CheckTask();

#define ENGINE_INIT 	Engine_Init(evQueue, EVENT_QUEUE_SIZE , tempDataForHandler, MAX_EVENT_SIZE, evPool, EVENT_POOL_SIZE)
#endif // ENGINE_H
