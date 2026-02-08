#include "strand.h"
#include "engine.h"
#include <stdlib.h>

static inline void Strand_Next(strand_t *s)
{
	ASSERT(s != NULL);

	if (s->busy || Queue_IsEmpty(s->command))
		return;
	Event_Post(s->execute.index, &s);
	s->busy = true;
}

static void Strand_Timeout(void *payload)
{
	ASSERT(payload != NULL);

	strand_t *s = (strand_t*) payload;
	s->busy = false;
	Strand_Next(s);
}

static void Strand_Execute(void *msg)
{
	ASSERT(msg != NULL);

	strand_t *s = *(strand_t**) msg;
	uint8_t type = 0;

	(void) Queue_Pop(s->command, &type);
	if (type == DELAY)
	{
		uint32_t time = 0;
		uint8_t temp = 0;
		for (uint8_t i = 0; i < 4; i++)
		{
			(void) Queue_Pop(s->command, &temp);
			time <<= ((4 - i) * 8);
			time += temp;
		}
		Task_Start(&s->timer, time, 1, s);
		s->finished = NULL;
		return;
	} else if (type == VOID)
	{
		s->finished = NULL;
	} else if (type == CALLBACK)
	{
		uint8_t index = 0;
		(void) Queue_Pop(s->command, &index);
		ASSERT(index < engine.eventQueue.poolSize);
		s->finished = s->evQueue->events[index - 1];
	}
	uint8_t index = 0;
	(void) Queue_Pop(s->command, &index);

	ASSERT(index < engine.eventQueue.poolSize);

	event_t *e = s->evQueue->events[index - 1];
	if (e->size == 0)
	{
		e->handler(NULL);
	} else
	{
		ASSERT(e->size <= engine.eventQueue.maxEvSize);

		s->payload = (uint8_t*) malloc(e->size * sizeof(uint8_t));

		ASSERT(s->payload != NULL);

		for (uint8_t i = 0; i < e->size; i++)
		{
			(void) Queue_Pop(s->command, &s->payload[i]);
		}

		e->handler(s->payload);
		free(s->payload);
		s->payload = NULL;
	}
}

void Strand_Init(strand_t *s, queue_t *q)
{
	ASSERT(q != NULL);
	ASSERT(s != NULL);

	Engine_RegisterTask(&s->timer);
	s->timer.handler = &Strand_Timeout;

	Event_Init(&s->execute, sizeof(strand_t*), &Strand_Execute);
	s->finished = NULL;

	s->evQueue = &engine.eventQueue;

	s->command = q;
	s->busy = false;
}

bool Strand_Post(strand_t *s, event_t *ev, event_t *cb, void *data)
{
	ASSERT(s != NULL);
	ASSERT(ev != NULL);
#ifndef NDEBUG
	if (cb != NULL)
		ASSERT(cb->size == 1);
#endif

	if (Queue_CheckNumOfFree(s->command) < ev->size + 3)
		return false;

	CRITICAL_SECTION_BEGIN;

	if (cb != NULL)
	{
		Queue_Push(s->command, CALLBACK);
		Queue_Push(s->command, cb->index);
	} else
	{
		Queue_Push(s->command, VOID);
	}

	Queue_Push(s->command, ev->index);
	if (ev->size > 0)
	{
		uint8_t *ptr = (uint8_t*) data;
		for (size_t i = 0; i < ev->size; i++)
		{
			Queue_Push(s->command, ptr[i]);
		}
	}

	CRITICAL_SECTION_END;
	Strand_Next(s);
	return true;
}

bool Strand_Delay(strand_t *s, uint32_t ms)
{
	ASSERT(s != NULL);

	if (Queue_CheckNumOfFree(s->command) < 5)
		return false;
	CRITICAL_SECTION_BEGIN
	Queue_Push(s->command, DELAY);
	Queue_Push(s->command, (ms >> 24) & 0xFF);
	Queue_Push(s->command, (ms >> 16) & 0xFF);
	Queue_Push(s->command, (ms >> 8) & 0xFF);
	Queue_Push(s->command, ms & 0xFF);
	Strand_Next(s);
	CRITICAL_SECTION_END
	return true;
}

void Strand_Done(strand_t *s)
{
	ASSERT(s != NULL);

	s->busy = false;
	Strand_Next(s);
}

void Strand_Done_With_Err(strand_t *s, uint8_t err)
{
	ASSERT(s != NULL);

	s->busy = false;
	if (s->finished != NULL)
	{
		Event_Post(s->finished->index, &err);
	}
	Strand_Next(s);
}
