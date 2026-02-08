#ifndef CORE_QUEUE_H_
#define CORE_QUEUE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "system.h"

typedef struct Queue
{
	uint16_t size;
	uint8_t *first;
	uint8_t *last;
	volatile uint8_t *inPtr;
	volatile uint8_t *outPtr;
} queue_t;

static inline void Queue_Init(queue_t *q, uint8_t *buf, uint16_t size)
{
	ASSERT(q != NULL);

	q->first = buf;
	q->size = size;
	q->last = q->first + size;
	q->inPtr = q->first;
	q->outPtr = q->first;
}

static inline void Queue_Reset(queue_t *q)
{
	ASSERT(q != NULL);

	q->inPtr = q->first;
	q->outPtr = q->first;
}

static inline uint16_t Queue_CheckNumOfFree(queue_t *q)
{
	ASSERT(q != NULL);

	uint16_t ret = q->size + q->outPtr - q->inPtr;
	if (ret > q->size)
		ret -= q->size;
	return (ret - 1);
}

static inline bool Queue_IsEmpty(queue_t *q)
{
	ASSERT(q != NULL);

	return (q->inPtr == q->outPtr);
}

static inline bool Queue_IsFull(queue_t *q)
{
	ASSERT(q != NULL);

	return ((q->inPtr + 1 == q->outPtr)
			|| (q->inPtr == q->last - 1 && q->outPtr == q->first));
}

static inline bool Queue_Push(queue_t *q, uint8_t val)
{
	ASSERT(q != NULL);

	volatile uint8_t *next = q->inPtr + 1;
	if (next == q->last)
		next = q->first;
	if (next != q->outPtr)
	{
		*(q->inPtr) = val;
		q->inPtr = next;
		return true;
	} else
		return false;
}

static inline bool Queue_Pop(queue_t *q, uint8_t *val)
{
	ASSERT(q != NULL);
	ASSERT(val != NULL);

	if (q->outPtr != q->inPtr)
	{
		*val = *(q->outPtr);
		q->outPtr++;
		if (q->outPtr == q->last)
			q->outPtr = q->first;
		return true;
	}
	return false;
}

#endif /* CORE_QUEUE_H_ */
