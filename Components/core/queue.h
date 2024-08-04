#ifndef CORE_QUEUE_H_
#define CORE_QUEUE_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct Queue
{
	uint16_t size;
	uint8_t* first;
	uint8_t* last;
	uint8_t* inPtr;
	uint8_t* outPtr;
}queue_t;

void Queue_Init(queue_t* q, uint8_t* buf, uint16_t size);
void Queue_Reset(queue_t* q);
uint16_t Queue_CheckNumOfFree(queue_t* q);
bool Queue_Pop(queue_t* q, uint8_t* val);
bool Queue_Push(queue_t* q, uint8_t val);
bool Queue_IsEmpty(queue_t* q);
bool Queue_IsFull(queue_t* q);

#endif /* CORE_QUEUE_H_ */
