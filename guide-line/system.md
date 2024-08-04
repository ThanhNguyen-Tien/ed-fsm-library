# system.h & system.c

This file contains configuration parameters for the `engine` as well as declaring the system initialization function.

## MACRO DESCRIPTION

   - `EVENT_QUEUE_SIZE`:  This defines the size of the `EventQueue`. Choose an appropriate value to ensure the `EventQueue` does not overflow during operation.
   - `EVENT_POOL_SIZE`: This defines the total number of events used in your application. You can check the value of `engine.eventQueue.poolsize` after registering all events to determine how many events you are currently using.
   - `MAX_EVENT_SIZE`: This defines the size of the `tempDataForHandler` array used to temporarily store fixed event data. Ensure it is large enough to accommodate the largest `sizeof(dataType)` in your application.

## FUNCTION DESCRIPTION

### `extern void SystemClock_Config();`

- This function is generated from STM32 CubeMx and we use it to initialize the system clock.

---

### `void System_Init()`

- Make sure this function is called right at the start of the main function.
- For an example of how to use this function, refer to the file `test/main.c`.

---

### `void SysTick_Handler(void)`

- This function is Handler function for Systick Interrupt. Normally, its period is 1ms.
- You can use any other TIMER module to call `Engin_CheckTask()`.
- **If `Engine_CheckTask()` is not called periodically then `M_TASK` cannot operate.**
- NOTE: The period in which you call `Engine_CheckTask()` affects the time you configure for `M_TASK()`
    - Example 1:
    ```c
    // Period call Engine_CheckTask() is 1 ms
    M_TASK_START(name, 10, 5);
    // nameTask has period is 10 * 1ms = 10 ms and run 5 times
    // Every 10ms, M_TASK_HANDLER(name) is called
    // After 50ms nameTask automatically stops
    ```

    - Example 2:
    ```c
    // Period call Engine_CheckTask() is 10 ms
    M_TASK_START(name, 20, 3);
    // nameTask has period is 20 * 10ms = 200 ms and run 3 times
    // Every 200ms, M_TASK_HANDLER(name) is called
    // After 600ms nameTask automatically stops
    ```

---

### After generating code from STM32Cube MX, in `Core/src/stm32..._it.c` file, the function `void SysTick_Handler(void)` is automatically generated, leading to a conflict when building project. To avoid that, in STM32Cube MX, at the "**NVIC/Code generation**" tab, disable "**Generate IRQ Handler**" for "**Time base: Systick Timer**". 