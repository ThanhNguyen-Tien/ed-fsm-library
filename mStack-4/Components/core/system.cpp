#include <core/system.h>
#include <core/engine.h>
#include "gpio.h"

// ======================
// C API
// ======================
extern "C" {

void SystemClock_Config(void);

void SysTick_Handler(void) {
	core::Engine& engine = core::Engine::instance();
	engine.isrEnter();
    engine.tick();
}

void systemInit(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
}

void DWT_Init(void) {
    // Enable DWT counter for cycle measurement
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable Trace
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            // Enable counter
    DWT->CYCCNT = 0;                                // Reset
}

} // extern "C"
