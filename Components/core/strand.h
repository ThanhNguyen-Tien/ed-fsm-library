#ifndef CORE_STRAND_H_
#define CORE_STRAND_H_

#include "queue.h"
#include "event.h"
#include "task.h"

typedef struct Strand
{
	task_t timer;
	event_t execute;
	event_t* finished;
	event_queue_t* evQueue;
	queue_t* command;
	uint8_t* payload;
	bool busy;
}strand_t;

typedef enum {
	VOID=0,
	CALLBACK,
	DELAY
}strand_event_t;

void Strand_Init(strand_t* s, queue_t* q);
void Strand_Done(strand_t* s);
void Strand_Done_With_Err(strand_t* s, uint8_t err);
bool Strand_Post(strand_t* s, event_t* ev, event_t* cb, void* data);
bool Strand_Delay(strand_t* s, uint32_t ms);

#define M_STRAND_DEF(name)	\
	extern strand_t name##Strand;

#define M_STRAND(name, size)\
	strand_t name##Strand;	\
	queue_t name##StrandQueue;\
	uint8_t name##StrandBuffer[size] = {0};

#define M_STRAND_INIT(name)\
	Queue_Init(&name##StrandQueue, name##StrandBuffer, sizeof(name##StrandBuffer)); \
	Strand_Init(&name##Strand, &name##StrandQueue);

#endif /* CORE_STRAND_H_ */
