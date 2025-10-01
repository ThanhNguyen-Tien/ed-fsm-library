#include <core/system.h>
#include <core/engine.h>
#include "gpio.h"

extern "C" void SystemClock_Config(void);

extern "C" void SysTick_Handler(void) {
	core::Engine::instance().tick();
}

void systemInit() {
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
}

void DWT_Init(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable Debug Exception and Monitor Control
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;           // Enable Cycle Counter
	DWT->CYCCNT = 0;                               // Reset Counter
}
