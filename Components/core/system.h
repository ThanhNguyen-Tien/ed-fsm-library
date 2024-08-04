#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include "main.h"
#include "event.h"

#define EVENT_POOL_SIZE     32
#define EVENT_QUEUE_SIZE    128
#define MAX_EVENT_SIZE		64

extern uint8_t evQueue[EVENT_QUEUE_SIZE];
extern uint8_t tempDataForHandler[MAX_EVENT_SIZE];
extern event_t* evPool[EVENT_POOL_SIZE];

extern void SystemClock_Config();
void System_Init();

#endif // SYSTEM_H
