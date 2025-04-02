# machine.h

This file defines the `Machine` structure and provides a set of macros to facilitate state machine implementation.

## MACRO DESCRIPTION

- **MACHINE(name)**
    - Declares an instance of a state machine named `nameMachine`. 
    - Typically, this macro is used in a `.c` file to define the state machine.

    ```c
    // Declare an Machine
    MACHINE(controller)
    ```
---

- **MACHINE_DEF(name)**
    - When you need to interact with `nameMachine` from another `.c` file (e.g., posting events or checking states), you must extern it. This macro does it for you.
    - Place this macro in the corresponding .h file where MACHINE(name) is declared.
    - Make sure that the `name` in `MACHINE_DEF(name)` and `MACHINE(name)` is the same.

    ```h
    MACHINE_DEF(controller)
    ```
---

- **STATE_DEF(name)**

    - Declare a function that represents a state in `nameMachine`. 
    - Using this MACRO in `.h` file.
---

- **STATE_BODY(name)**

    - Implementation of a function that represents a state in `nameMachine`. 
    - Using this MACRO in `.c` file.
---

- **TRANSITION_(...)**
  
    - Defines how the machine responds to an event. Only use this macro inside STATE_BODY(name).
    
    - *TRANSITION_(event, state)*
    
        - When `nameMachine` receives `event`, it transitions to `state` and executes the function body.

    ```c
    TRANSITION_(event, state)
    {
        // fucntion body
    }
    ```

    - *TRANSITION_(event)*

        - When `nameMachine` receives `event`, it remains in the current state and executes the function body.

    ```c
    TRANSITION_(event)
    {
        // fucntion body
    }
    ```
---

- **ENTER()**
    - Check if the incoming event is `ENTER_NEW_STATE`.
    - Only use this MACRO inside the body of `STATE_BODY(name)`
---

- **EXIT()**
    - Check if the incoming event is `EXIT_CURRENT_STATE`.
    - Only use this MACRO inside the body of `STATE_BODY(name)`
---

- **SM_SWITCH(state)**
    - Switches immediately to another state.
    - Only use this MACRO inside the body of `STATE_BODY(name)`
---

- **SM_START(name, state)**
    - Starts the state machine `nameMachine` in the specified initial `state`.
    - Immediately after initialization, an `ENTER_NEW_STATE` event is posted to the `machine`.
---

- **SM_POST(name, event)**
    - Post an `event` to `nameMachine`.
    - The event handler of `nameMachine` is pushed to the `EventQueue` and processed later.
---

- **SM_EXECUTE(name, event)**
    - Immediately executes the event handler of `nameMachine`, instead of pushing it to the `EventQueue`. 
    - Use with caution, as it bypasses the normal event queue mechanism.

## State Transition Behavior

**When transitioning from state A to state B (via TRANSITION_ or SM_SWITCH):**

- **The system calls EXIT() of state A.**
- **The system updates the state from A to B.**
- **The system calls ENTER() of state B.**
---
## EXAMPLE
- For an example of how to use `MACHINE`, refer to the files `test/test.c` and `test/test-sm.c`.