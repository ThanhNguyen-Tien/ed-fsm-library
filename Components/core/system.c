#include "system.h"
#include "engine.h"
#include "gpio.h"

uint8_t evQueue[EVENT_QUEUE_SIZE];
uint8_t tempDataForHandler[MAX_EVENT_SIZE];
event_t* evPool[EVENT_POOL_SIZE];

void SysTick_Handler(void)	//1ms
{
  Engine_CheckTask();
}

void System_Init()
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
}

void Cycle_Measurement_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
}

uint32_t Get_Cycle_Count()
{
	return DWT->CYCCNT;
}

uint32_t Get_CPU_Freq_Hz(void)
{
	return HAL_RCC_GetSysClockFreq();
}



