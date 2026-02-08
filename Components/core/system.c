#include "system.h"
#include "engine.h"
#include "gpio.h"
#include <stdint.h>

uint8_t evQueue[EVENT_QUEUE_SIZE];
uint8_t tempDataForHandler[MAX_EVENT_SIZE];
event_t *evPool[EVENT_POOL_SIZE];

void SysTick_Handler(void)	//1ms
{
	Engine_ISR_Enter();
	Engine_Check_Task();
}

void System_Init()
{
	HAL_Init();
	MX_GPIO_Init();
	SystemClock_Config();
}

void DWT_Init(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable Debug Exception and Monitor Control
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;           // Enable Cycle Counter
	DWT->CYCCNT = 0;                               // Reset Counter
}

