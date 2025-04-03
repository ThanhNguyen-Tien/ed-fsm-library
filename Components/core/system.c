#include "system.h"
#include "engine.h"
#include "gpio.h"

// Global buffers and event pool for the system
uint8_t evQueue[EVENT_QUEUE_SIZE];
uint8_t tempDataForHandler[MAX_EVENT_SIZE];
event_t* evPool[EVENT_POOL_SIZE];

/**
 * @brief System tick interrupt handler (1ms interval).
 *
 * This function is called by the hardware timer interrupt and is responsible
 * for checking tasks in the engine. It ensures periodic task execution.
 */
void SysTick_Handler(void) // 1ms
{
  Engine_CheckTask();
}

/**
 * @brief Initializes the system.
 *
 * This function initializes the hardware abstraction layer (HAL), configures
 * the system clock, and initializes GPIO peripherals.
 */
void System_Init()
{
    HAL_Init();               // Initialize the HAL library
    SystemClock_Config();     // Configure the system clock
    MX_GPIO_Init();           // Initialize GPIO peripherals
}

/**
 * @brief Initializes the cycle measurement system.
 *
 * This function enables the DWT (Data Watchpoint and Trace) unit for cycle
 * counting, which is used for performance monitoring and CPU usage calculation.
 */
void Cycle_Measurement_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable DWT and ITM
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;           // Enable cycle counter
    DWT->CYCCNT = 0;                               // Reset cycle counter
}

/**
 * @brief Retrieves the current CPU cycle count.
 *
 * @return The current value of the DWT cycle counter.
 */
uint32_t Get_Cycle_Count()
{
    return DWT->CYCCNT;
}

/**
 * @brief Retrieves the CPU frequency in Hz.
 *
 * @return The system clock frequency in Hz.
 */
uint32_t Get_CPU_Freq_Hz(void)
{
    return HAL_RCC_GetSysClockFreq();
}