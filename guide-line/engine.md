# engine.h

This file declares the definitions for `struct Engine`.

In this file you only need to pay attention to the following functions:
- `void Engine_Init(uint8_t* buf, uint16_t size, uint8_t* dataBuf, uint8_t maxEvSize, event_t** evPool, uint8_t evPoolSize);`
- `void Engine_Run();`
- `void Engine_Delay(uint32_t);`

##
### `void Engine_Init(...);`

- This function initializes the Engine. 
- The function call might seem complex, so it is recommended to use the `ENGINE_INIT` MACRO for ease of use instead of calling the function directly and passing arguments. 
- This function must be called immediately after `System_Init()` to ensure the Engine is initialized before any other components in the system.
- For an example of how to use this function, refer to the file `test/main.c`.

---

### `void Engine_Run();`

- This is the main loop for the system to run. You should call this function after initializing all components.
- For an example of how to use this function, refer to the file `test/main.c`

---

### `void Engine_Delay(uint32_t);`

- This function has similar functionality to `HAL_Delay()`. Both functions are used to create a time delay.
- This function is blocking, so please consider before using it.
- **Recommendation**: The only blocking point in the system should be when the system starts and initializes the components.
