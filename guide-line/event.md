# event.h

This file declares the definitions for `struct Event` and `struct EventQueue`. On other platforms, you might encounter the concepts of `Message` & `MessageQueue`, which are similar to `Event` & `EventQueue` here.

In this library, there are two key concepts to understand:
- `Empty Event` - Event with no required data
- `Fixed Event` - Event with required data  

For ease of use, in this file you only need to pay attention to MACROs.

## MACRO DESCRIPTION

- **M_EVENT(name)**
    
    - Declares an instance of the event that you want to use in your system. It declares a variable named `nameEvent`.
    - Typically, you must declare it in a `.c` file, similar to declaring global variables.

    ```c
    // Declare an Event
    M_EVENT(buttonPressed)
    ```

---

- **M_EVENT_DEF(name)**

    - When you need to post `nameEvent` from another `.c` file, you have to extern it. This MACRO does it for you. Place it in the `.h` file that corresponds with the `.c` file where you declare `M_EVENT(name)`
    - Ensure that the `name` in `M_EVENT_DEF(name)` and `M_EVENT(name)` is the same.

    ```h
    M_EVENT_DEF(buttonPressed)
    ```

---

- **M_EVENT_INIT(...)**

    - This MACRO initializes `nameEvent`. Ensure that `nameEvent` is initialized before use it.
    
    - There are two types of use:
    - **M_EVENT_INIT(name)**
    
        This MACRO initializes `Empty Events`.

    ```c
    M_EVENT_INIT(buttonPressed); // Empty Event
    ```

    - **M_EVENT_INIT(name, sizeof(type))**

        This MACRO initializes `Fixed Events`. The type here can be any data type such as `char`, `int`, `long`, `float`, `double`, `struct`, etc.

    ```c
    M_EVENT_INIT(imu, sizeof(struct ImuData)); // Fixed Event
    ```

---- 

- **M_EVENT_POST(...)**

    - This MACRO posts `nameEvent` to `EventQueue`. There are two types of use:
    - **M_EVENT_POST(name)**
    
        This MACRO push an Empty Events to `EventQueue`.

    ```c
    M_EVENT_POST(buttonPressed); // Empty Event
    ```

    - **M_EVENT_POST(name, data)**

        This MACRO push an Fixed Events to `EventQueue`.

    ```c
    M_EVENT_POST(imu, imuData_);    // Fixed Event
    ```
    
    - You can check the return value to know whether the event was pushed to the `EventQueue` successfully or failed

    ```c
    bool ret;
    ret = M_EVENT_POST(imu, imuData_);
    if(!ret)
    {
        // Push Fail
        // TODO: Handle error
    }
    ```

    **#Note: You must use the correct MACRO for the event type you initiated in `M_EVENT_INIT`. Misuse, whether intentional or unintentional, can lead to unwanted system behavior.**

---

- **M_EVENT_HANDLER(name)**
    
    - This is the event handler for `nameEvent`. When an event is popped out from `EventQueue`, the corresponding handler is called.
    ```c
    M_EVENT_HANDLER(buttonPressed)  // Empty Event Handler
    {
        //TODO: Toggle GPIO Pin
    }
    ```

    - For a Fixed Event, you can cast the data to use it. For an Empty Event, no casting is necessary.

## EXAMPLE

- The following code snippet illustrates how to use M_EVENT

    ```c
    "Example`.c`"

    //Declare Event    
    M_EVENT(encoderDataReceived)
    M_EVENT(buttonPressed)

    void init()
    {
        //Initialization
        M_EVENT_INIT(encoderDataReceived, int16_t);
        M_EVENT_INIT(buttonPressed);
    }

    void SPI1_IRQHandler(void)  // SPI1 Interrupt Handler for reading data of encoder
    {
        //TODO: LL_GPIO_SetOutput(CS_Port, CS_Pin);
        int16_t position = LL_SPI_ReceiveData16(SPI1); // Get data
        M_EVENT_POST(encoderDataReceived, &position);  // Post Fixed Event with data is "position"
    }

    void GPIO_IRQHandler(void)  // EXT GPIO Interrupt Handler
    {
        M_EVENT_POST(buttonPressed);    // Post Empty Event
    }

    //Event Handler
    M_EVENT_HANDLER(encoderData)    // Fixed Event Handler
    {
        // Cast data (if event has data) to use
        int16_t* data_ = (int16_t*) data;   // Only do this with Fixed Event
        // (* data_) is a copied of position and you can use it for your work

        //TODO: LOG_PRINTF("Encoder: %d", (*data));
    }

    M_EVENT_HANDLER(buttonPressed)    // Empty Event Handler
    {
        //TODO: LOG_PRINT("Button Pressed");
    }
    ```