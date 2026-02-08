#include "event.h"
#include "engine.h"
#include "system.h"
#include <string.h>

#define THRESHOLD_SIZE	8	// bytes

/// @brief STATIC FUNCTION
static inline void EventQueue_Push(uint8_t val)
{
	*(engine.eventQueue.inPtr) = val;
	engine.eventQueue.inPtr++;
	if (engine.eventQueue.inPtr == engine.eventQueue.last)
	{
		engine.eventQueue.inPtr = engine.eventQueue.first;
	}
}

static inline uint8_t EventQueue_Pop()
{
	uint8_t ret = *(engine.eventQueue.outPtr);
	engine.eventQueue.outPtr++;
	if (engine.eventQueue.outPtr == engine.eventQueue.last)
	{
		engine.eventQueue.outPtr = engine.eventQueue.first;
	}
	return ret;
}

static inline void EventQueue_PopData(uint8_t *data, uint8_t size)
{
	if (engine.eventQueue.outPtr + size <= engine.eventQueue.last)
	{
		memcpy(data, engine.eventQueue.outPtr, size);
		engine.eventQueue.outPtr += size;
		if (engine.eventQueue.outPtr == engine.eventQueue.last)
		{
			engine.eventQueue.outPtr = engine.eventQueue.first;
		}
	} else
	{
		uint16_t firstPartSize = engine.eventQueue.last
				- engine.eventQueue.outPtr;
		memcpy(data, engine.eventQueue.outPtr, firstPartSize);
		memcpy(data + firstPartSize, engine.eventQueue.first,
				size - firstPartSize);
		engine.eventQueue.outPtr = engine.eventQueue.first
				+ (size - firstPartSize);
	}
}

static inline void EventQueue_ExecuteEvent(event_t *ev)
{
	if (ev->size == 0)
	{
		ev->handler(NULL);
	} else
	{
		ASSERT(ev->size <= engine.eventQueue.maxEvSize);

		EventQueue_PopData(engine.eventQueue.dataBuf, ev->size);
		ev->handler(engine.eventQueue.dataBuf);
		memset(engine.eventQueue.dataBuf, 0, ev->size);
	}
}

/// @brief PUBLIC FUNCTION
static uint8_t Event_Register_(event_t *event)
{
	ASSERT(event != NULL);
	ASSERT(engine.eventQueue.poolSize - 1 < engine.eventQueue.maxPoolSize);
	engine.eventQueue.events[engine.eventQueue.poolSize - 1] = event;
	return engine.eventQueue.poolSize++;
}

bool Event_Loop()
{
	if (engine.eventQueue.inPtr == engine.eventQueue.outPtr)
		return false;
	uint8_t index = EventQueue_Pop();
	ASSERT(index < engine.eventQueue.poolSize);

#ifndef NDEBUG
	event_t *e = engine.eventQueue.events[index - 1];

	uint32_t exec_start = DWT->CYCCNT;
	EventQueue_ExecuteEvent(e);
	e->time.last_exec_time = DWT->CYCCNT - exec_start;

	if (e->time.last_exec_time > e->time.max_time)
		e->time.max_time = e->time.last_exec_time;
	if (e->time.last_exec_time < e->time.min_time)
		e->time.min_time = e->time.last_exec_time;
#else
	event_t * e = engine.eventQueue.events[index - 1];
	EventQueue_ExecuteEvent(e);
#endif

	return true;
}

void Event_Init(event_t *ev, uint8_t size, EventHandler handler)
{
	ASSERT(ev != NULL);
	ev->size = size;
	ev->handler = handler;
	ev->index = Event_Register_(ev);
#ifndef NDEBUG
	ev->time.last_exec_time = 0;
	ev->time.max_time = 0;
	ev->time.min_time = UINT32_MAX;
#endif
}

inline bool Event_Post(uint8_t index, void *data)
{
	CRITICAL_SECTION_BEGIN
#ifdef NDEBUG
	if(index == 0)
	{
		CRITICAL_SECTION_END
		return false;
	}
#else
	ASSERT(index != 0);
#endif
	uint8_t *ptr = (uint8_t*) data;
	uint8_t size = engine.eventQueue.events[index - 1]->size;
	uint16_t avail = engine.eventQueue.size - 1 + engine.eventQueue.outPtr
			- engine.eventQueue.inPtr;
	if (avail > engine.eventQueue.size)
	{
		avail -= engine.eventQueue.size;
	} else
	{
	}
	if (avail < size + 1)
	{
		CRITICAL_SECTION_END;
		return false;
	} else
	{
	}

#ifndef NDEBUG
	avail -= (size + 1);
	if (avail < engine.eventQueue.minFree)
		engine.eventQueue.minFree = avail;
#endif
	EventQueue_Push(index);
	if (size != 0)
	{
		if (engine.eventQueue.inPtr + size <= engine.eventQueue.last)
		{
			if (size <= THRESHOLD_SIZE)
			{
				for (uint8_t i = 0; i < size; ++i)
					engine.eventQueue.inPtr[i] = ptr[i];
			} else
			{
				memcpy(engine.eventQueue.inPtr, ptr, size);
			}
			engine.eventQueue.inPtr += size;
			if (engine.eventQueue.inPtr == engine.eventQueue.last)
				engine.eventQueue.inPtr = engine.eventQueue.first;
		} else
		{
			uint16_t firstPartSize = engine.eventQueue.last
					- engine.eventQueue.inPtr;
			if (firstPartSize <= 4)
			{
				for (uint8_t i = 0; i < firstPartSize; ++i)
					engine.eventQueue.inPtr[i] = ptr[i];
			} else
			{
				memcpy(engine.eventQueue.inPtr, ptr, firstPartSize);
			}

			uint8_t remaining = size - firstPartSize;
			if (remaining <= THRESHOLD_SIZE)
			{
				for (uint8_t i = 0; i < remaining; ++i)
					engine.eventQueue.first[i] = ptr[firstPartSize + i];
			} else
			{
				memcpy(engine.eventQueue.first, ptr + firstPartSize, remaining);
			}

			engine.eventQueue.inPtr = engine.eventQueue.first + remaining;
		}
	} else
	{
	}

	CRITICAL_SECTION_END;
	return true;
}
