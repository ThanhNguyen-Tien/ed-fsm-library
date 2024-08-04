# observer.h

This file declares the definitions for `struct ObserverNode` and `struct ObserverSubject`.

In this library, the observer pattern is chosen to perform the task of connecting data between components in the system without being dependent on each other.

There are two concepts here:
- `Subject`: This is the source of data transmission. It doesn't care whether anyone signed up to receive this data or not. A `Subject` can can notify data to many `Nodes` that register to receive.
- `Node`: This is where data is received and processed. A `Node` can register to receive data from many `Subjects`.

There are two types of Notify:
- `PUSH_TO_QUEUE`: The `Handler` of `Node` is pushed to `EventQueue`.
- `CALL_IMMEDIATLY`: The `Handler` of `Node` is called immediately.

## FUNCTION DESCRIPTION

### `void Observer_InitSubject(obs_subject_t* sub, uint8_t sizeOfData);`

Initializes a subject for observation.

- **Parameters:**
  - `sub`: Pointer to the subject structure to initialize.
  - `sizeOfData`: Size of data to be stored in the subject.

---
  
### `void Observer_InitNode(obs_node_t* obs, event_t* ev, obs_handler_type_t type);`

Initializes an observer node with an event and handler type.

- **Parameters:**
  - `obs`: Pointer to the observer node structure to initialize.
  - `ev`: Pointer to the event structure associated with this observer node.
  - `type`: Type of event handler.

---

### `bool Observer_AttachNode(obs_subject_t* sub, obs_node_t* node);`

Attaches an observer node to a subject for receiving notifications.

- **Parameters:**
  - `sub`: Pointer to the subject structure.
  - `node`: Pointer to the observer node structure to attach.

- **Returns:**
  - `true` if attachment was successful, `false` otherwise.

---

### `void Observer_DetachNode(obs_subject_t* sub, obs_node_t* node);`

Detaches an observer node from a subject.

- **Parameters:**
  - `sub`: Pointer to the subject structure.
  - `node`: Pointer to the observer node structure to detach.

---

### `void Observer_Notify(obs_subject_t* sub, void* data);`

Notifies all attached observer nodes about an event.

- **Parameters:**
  - `sub`: Pointer to the subject structure.
  - `data`: Pointer to data to pass along with the notification.

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