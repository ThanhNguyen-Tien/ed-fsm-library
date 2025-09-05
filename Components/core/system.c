#include "system.h"
#include "engine.h"
#include "gpio.h"

// Global buffers and event pool for the system
uint8_t evQueue[EVENT_QUEUE_SIZE];       // Event queue buffer
uint8_t tempDataForHandler[MAX_EVENT_SIZE]; // Temporary buffer for event data
event_t* evPool[EVENT_POOL_SIZE];        // Pool of reusable events

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
 * 
 * Note: This implementation is for Arm Cortex-M devices. 
 * The function can be redefined to match the target hardware platform.
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
 *
 * Note: This implementation is for Arm Cortex-M devices.
 * The function can be redefined to match the target hardware platform.
 */
uint32_t Get_Cycle_Count()
{
    return DWT->CYCCNT;
}

/**
 * @brief Waits for an interrupt.
 *
 * Executes a "wait for interrupt" instruction to reduce power consumption
 * until the next interrupt occurs.
 *
 * Note: This implementation uses __WFI() for Arm Cortex-M.
 * The function can be redefined to match the target hardware platform.
 */
void Wait_For_Interrupt(void)
{
    __WFI();
}

/**
 * @brief Executes a no-operation instruction.
 *
 * Useful for timing or debugging purposes.
 *
 * Note: This implementation uses __NOP() for Arm Cortex-M.
 * The function can be redefined to match the target hardware platform.
 */
void No_Operation(void)
{
    __NOP();
}

/**
 * @brief Enables global interrupts.
 *
 * Allows the processor to respond to interrupt requests.
 *
 * Note: This implementation uses __enable_irq() for Arm Cortex-M.
 * The function can be redefined to match the target hardware platform.
 */
void Enable_IRQ(void)
{
    __enable_irq();
}

/**
 * @brief Disables global interrupts.
 *
 * Prevents the processor from responding to interrupt requests.
 *
 * Note: This implementation uses __disable_irq() for Arm Cortex-M.
 * The function can be redefined to match the target hardware platform.
 */
void Disable_IRQ(void)
{
    __disable_irq();
}