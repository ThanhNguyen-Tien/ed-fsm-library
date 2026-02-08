#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include "main.h"
#include "event.h"

#ifndef NDEBUG
#include "assert.h"
#define ASSERT(e)	assert(e)
#else
#define ASSERT(e)
#endif

//#define DISABLE_INTERRUPT   __disable_irq()
//#define ENABLE_INTERRUPT    __enable_irq()
#define WAIT_FOR_INTERUPT   __WFI()
#define NO_OPERATION		__NOP()

#define CRITICAL_SECTION_BEGIN  \
    uint32_t __primask = __get_PRIMASK(); \
    __disable_irq();

#define CRITICAL_SECTION_END     \
    if (!(__primask)) __enable_irq();

#define EVENT_POOL_SIZE     	32
#define EVENT_QUEUE_SIZE    	128
#define MAX_EVENT_SIZE			32

extern uint8_t evQueue[EVENT_QUEUE_SIZE];
extern uint8_t tempDataForHandler[MAX_EVENT_SIZE];
extern event_t *evPool[EVENT_POOL_SIZE];

extern void SystemClock_Config();
void System_Init();
void DWT_Init(void);

#endif // SYSTEM_H
