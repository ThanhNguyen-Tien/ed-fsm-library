# task.h

This file defines macros for working with `struct Task`.

## Concepts

- In this framework, a **Task** acts as a **SoftTimer**.
- It serves as the **Event Timer source** among the three Event sources in the system.
- This allows periodic event-driven execution without hardware timers.

---

## MACRO DESCRIPTION

- **M_TASK(name)**

    - Declares an instance of an task that you want to use in your system. It declares a variable named `nameTask`.
    - Typically, you must declare it in a `.c` file, similar to declaring global variables.

    ```c
    // Declare a Task
    M_TASK(blink)
    ```

---

- **M_TASK_DEF(name)**

    - When you need to control (start/stop) `nameTask` from another `.c` file, you have to extern it. This MACRO does it for you. Place it in the `.h` file that corresponds with the `.c` file where you declare `M_TASK(name)`
    - Ensure that the `name` in `M_TASK_DEF(name)` and `M_TASK(name)` is the same.

    ```h
    M_TASK_DEF(blink)
    ```

---

- **M_TASK_INIT(name)**

    - This MACRO initializes `nameTask`. Ensure that `nameTask` is initialized before use it.

    ```c
    M_TASK_INIT(blink);
    ```

---

- **M_TASK_START(name)**

    - This MACRO starts a `nameTask` with a period time and repetition count

    ```c
    M_TASK_START(name, 10); // Start `nameTask` with period = 10 and it runs with unlimited number of repetitions
    M_TASK_START(name, 100, 5); // Start `nameTask` with period = 100 and it runs 10 times 
    ```

    - To convert a period to a time (in millisSecond), let's say the period you call `Engine_CheckTask()` is x (ms):
    > **periodTime(ms)** = **period** * **x** (ms)

    - Refer to `system.md` for more details. Normarlly, we call `Engine_CheckTask()` in `Systick_Handler()` so here x = 1ms. Therefore `M_TASK_START(name, 10);` starts `nameTask` with Period = 10ms.

---

- **M_TASK_STOP(name)**

    - This MACRO stops a `nameTask`.

    ```c
    M_STOP_START(name); // Stop M_TASK
    ```

---

- **M_TASK_HANDLER(name)**

    - This is the event handler for `nameTask`. When a task expires, the corresponding handler is called.

    **NOTE: Do NOT Start/Stop other M_TASK in M_TASK_HANDLER**

    ```c
    M_EVENT_HANDLER(blink)  // Task Handler
    {
        //TODO: Toggle GPIO Pin
    }
    ```

## EXAMPLE

- The following code snippet illustrates how to use M_TASK

    ```c
    "Example`.c`"

    //Declare Task    
    M_TASK(blink)
    M_TASK(readSensor)

    void init()
    {
        //Initialization
        M_TASK_INIT(blink); // blinkTask runs an unlimited number of times
        M_TASK_INIT(readSensor, 5); // readSensorTask runs 5 times
    }

    //blinkTask Handler
    M_TASK_HANDLER(blink)
    {
        //TODO: Doing somethings
        /*NOTE: Do NOT start/stop other tasks here
            M_TASK_START(readSensor)    : Do NOT do that!
            M_TASK_STOP(readSensor)     : Do NOT do that!
        */
    }

    //readEncoderTask Handler
    M_TASK_HANDLER(readSensor)
    {
        //TODO: Doing somethings
        /*NOTE: Do NOT start/stop other tasks here
            M_TASK_START(blink)    : Do NOT do that!
            M_TASK_STOP(blink)     : Do NOT do that!
        */
    }
    ```