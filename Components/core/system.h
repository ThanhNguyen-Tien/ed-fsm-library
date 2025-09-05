#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include "main.h"
#include "event.h"

// Configuration macros for event handling
#define EVENT_POOL_SIZE     32  // Maximum number of reusable events
#define EVENT_QUEUE_SIZE    128 // Size of the event queue
#define MAX_EVENT_SIZE      64  // Maximum size of event data

// Global buffers and event pool declarations
extern uint8_t evQueue[EVENT_QUEUE_SIZE];       // Event queue buffer
extern uint8_t tempDataForHandler[MAX_EVENT_SIZE]; // Temporary buffer for event data
extern event_t* evPool[EVENT_POOL_SIZE];        // Pool of reusable events

/**
 * @brief Configures the system clock.
 *
 * This function is implemented in the main application and is responsible
 * for setting up the system clock.
 */
extern void SystemClock_Config();

/**
 * @brief Initializes the system.
 *
 * This function initializes the HAL library, configures the system clock,
 * and initializes GPIO peripherals.
 */
void System_Init();

/**
 * @brief Initializes the cycle measurement system.
 *
 * This function enables the DWT (Data Watchpoint and Trace) unit for cycle
 * counting, which is used for performance monitoring and CPU usage calculation.
 */
void Cycle_Measurement_Init(void);

/**
 * @brief Retrieves the current CPU cycle count.
 *
 * @return The current value of the DWT cycle counter.
 */
uint32_t Get_Cycle_Count(void);

#endif // CORE_SYSTEM_H
