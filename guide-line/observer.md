# observer.h

This file defines the `ObserverNode` and `ObserverSubject` structures, which implement the *Observer Pattern* to facilitate data exchange between components in the system without introducing direct dependencies.

## Concepts
- `Subject`: The source of data transmission. It does not concern itself with whether any nodes have registered to receive its data. A subject can notify multiple nodes that have subscribed to it.
- `Node`: The entity that receives and processes data. A node can subscribe to multiple subjects over time, as long as those subjects notify using the same data type. However, at any given moment, a node can only be linked to one subject.

## Registration Rules
- Exclusive Subscription: A node can switch to a different subject at any time, but doing so automatically removes its previous subscription from the current subject.

- No Duplicate Registration: A node cannot register multiple times to the same subject.

## Notification Modes

There are two ways in which a subject can notify its registered nodes:

- `PUSH_TO_QUEUE`: The node’s `Handler` is pushed into the `EventQueue`, allowing it to be processed asynchronously.
- `CALL_IMMEDIATLY`: The node’s `Handler` is invoked immediately at the time of notification.

## MACRO DESCRIPTION

- **M_OBS_SUBJECT(name)**  

    - This macro defines an `obs_subject_t` instance named `nameSubject`. Use it in the `.c` file where you want to create a subject.  
    - It should be paired with `M_OBS_SUBJECT_DEF(name)` in the corresponding `.h` file to allow other files to reference the subject.

    ```c
    // Define a subject in the .c file  
    M_OBS_SUBJECT(sensorA)
    ```

---

- **M_OBS_SUBJECT_DEF(name)**  

    - If you need to access `nameSubject` from another `.c` file, you must declare it as `extern`. This macro simplifies that process for you. Place it in the corresponding `.h` file where `M_OBS_SUBJECT(name)` is defined.  
    - Make sure that the `name` used in `M_OBS_SUBJECT_DEF(name)` matches the one in `M_OBS_SUBJECT(name)`.

    ```c
    // Declare a subject for external use  
    M_OBS_SUBJECT_DEF(sensorA)
    ```

---
- **M_OBS_SUBJECT_INIT(name, type)**  

    - Initializes the subject `nameSubject` with the given data type size.  
    - Use this macro in the `.c` file where the subject is defined.  
    - The `type` parameter represents the data structure that will be transmitted through this subject.  

    ```c
    // Initialize a subject with a specific data type  
    M_OBS_SUBJECT_INIT(sensorA, sensor_data_t)
    ```

---

- **M_OBS_NODE_DEF(name)**  

    - Declares an external observer node `nameNode`.  
    - Use this macro in a `.h` file when `nameNode` is defined in another `.c` file.  
    - Ensures that other modules can reference this node without redeclaring it.  

    ```c
    // Declare an observer node  
    M_OBS_NODE_DEF(sensorA)
    ```

---

- **M_OBS_NODE(name)**  

    - Defines an observer node `nameNode` and its corresponding event structure.  
    - Use this macro in the `.c` file where the node is implemented.  
    - Must be paired with `M_OBS_NODE_DEF(name)` in the corresponding `.h` file to make it accessible externally.  

    ```c
    // Define an observer node in the .c file  
    M_OBS_NODE(sensorA)
    ```

---

- **M_OBS_NODE_INIT(name, type, type_enum)**  

    - Initializes the observer node `nameNode` with a specific data type and notification method.  
    - The `type` parameter specifies the size of the data the node will receive.  
    - The `type_enum` parameter determines how the node handles notifications (`PUSH_TO_QUEUE` or `CALL_IMMEDIATELY`).  
    - Ensures at compile time that `type_enum` is a valid `obs_handler_type_t` value.  

    ```c
    // Initialize an observer node with a specific data type and notification type  
    M_OBS_NODE_INIT(sensorA, sensor_data_t, PUSH_TO_QUEUE)
    ```

---

- **M_OBS_NODE_HANDLER(name)**  

    - Declares the event handler function for `nameNode`.  
    - This function is automatically invoked when the node receives data from a subject.  
    - The handler function should be implemented separately in the `.c` file.  

    ```c
    // Define the event handler function for the observer node  
    M_OBS_NODE_HANDLER(sensorA)  
    {  
        sensor_data_t* data = (sensor_data_t*)data;  
        // Process the received data  
    }
    ```
---

- ### `Observer_AttachNode`

    ```c
    void Observer_AttachNode(obs_subject_t* sub, obs_node_t* node);
    ```

    - Attaches a node (node) to a subject (sub), allowing it to receive notifications.
    - A node can only be linked to one subject at a time. If the node is already attached to another subject, it will be detached first.
    - If the node is already attached to the given subject, the function does nothing.
    - Example Usage:

    ```c
    Observer_AttachNode(&sensorASubject, &displayNode);
    ```
---

- ### `Observer_DetachNode`

    ```c
    void Observer_DetachNode(obs_subject_t* sub, obs_node_t* node);
    ```

    - Detaches a node (node) from a subject (sub).
    - After detachment, the node will no longer receive notifications from the subject.
    - Example Usage:

    ```c
    Observer_DetachNode(&sensorASubject, &displayNode);
    ```
---

- ### `Observer_Notify`

    ```c
    void Observer_Notify(obs_subject_t* sub, void* data);
    ```

    - Notifies all attached nodes of the subject (sub), sending them the provided data.
    - The notification method (PUSH_TO_QUEUE or CALL_IMMEDIATELY) determines how each node processes the received data.
    - The size of data must match the type specified when initializing the subject.
    - If no nodes are attached, the function does nothing.
    - Example Usage:

    ```c
    sensor_data_t sensorData = { .temperature = 25, .humidity = 60 };
    Observer_Notify(&sensorASubject, &sensorData);
    ```
---

## EXAMPLE

- The following code snippet illustrates how to use `Observer`
  
    ```h
    //Subject - encoder.h
    extern obs_subject_t encoderData;

    ```

    ```c
    //Subject - encoder.c
    #include "encoder.h"

    //Declare a subject
    obs_subject_t encoderData;

    void Encoder_Init()
    {
	    //Init Observer Subject
	    Observer_InitSubject(&encoderData, sizeof(int16_t));
    }

    void SPI1_IRQHandler(void)  // SPI1 Interrupt Handler for reading data of encoder
    {
        //TODO: LL_GPIO_SetOutput(CS_Port, CS_Pin);
        int16_t position = LL_SPI_ReceiveData16(SPI1); // Get data
        Observer_Notify(&encoderData, &position);  // Notify
    }

    ```

    ```c
    //Node - controller.c
    #include "encoder.h"

    //Declare a Node
    obs_node_t obsEncoder;
    M_EVENT(encoderReceived)

    void Controller_Init()
    {
        //Init Observer Node
        M_EVENT_INIT(encoderReceived, sizeof(uint64_t));
        Observer_InitNode(&obsEncoder, &encoderReceivedEvent, PUSH_TO_QUEUE);

        //Register Node with Subject
        bool ret = false;
        ret = Observer_AttachNode(&encoderData, &obsEncoder);
        if(!ret)
        {
            LOG_PRINT("Register ObsEncoder Fail");
        }
    }

    M_EVENT_HANDLER(encoderReceived)
    {
        int16_t* data_ = (int16_t*)data;
        LOG_PRINTF("Encoder Data: %d", *data_);
    }

    ```