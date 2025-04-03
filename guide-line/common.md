# SOME NOTES WHEN USING

1. There are three global variables in the `system.c` file. These variables are used exclusively by the Engine. Please do not modify or interfere with these variables.

2. Adjust the values of `EVENT_QUEUE_SIZE`, `EVENT_POOL_SIZE` and `MAX_EVENT_SIZE` to suit with your application. Refer to `system.h` for more details.
  
3. Observe the system's states through the `engine`, `monitor`, and `evPool` components within the Live Expression section of the Debug interface.
    - The variable `engine.eventQueue.minFree` is used to keep track of the number of empty slots remaining in the `EventQueue` and to record the minimum value during operation. This helps users evaluate and adjust `EVENT_QUEUE_SIZE` (defined in `system.h`) to suit the application's needs.
    - The variable `monitor` contains information of CPU_USAGE in debug_mode.
    - The array `evPool` contains information about execution time of each event-handler.