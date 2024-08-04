# machine.h

This file declares the definitions for `struct Machine`.

For ease of use, in this file you only need to pay attention to MACROs.

## MACRO DESCRIPTION

- **MACHINE(name)**

    - Declares an instance of the event that you want to use in your system. It declares a variable named `nameMachine`.
    - Typically, you must declare it in a `.c` file, similar to declaring global variables.

    ```c
    // Declare an Machine
    MACHINE(controller)
    ```

---

- **MACHINE_DEF(name)**

    - When you need to interact with `nameMachine` from another `.c` file (postEvent, checkState, etc.), you have to extern it. This MACRO does it for you. Place it in the `.h` file that corresponds with the `.c` file where you declare `MACHINE(name)`
    - Ensure that the `name` in `MACHINE_DEF(name)` and `MACHINE(name)` is the same.

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
  
    This MACRO is used to define the Machine's behavior with an `event`. Only use this MACRO inside the body of `STATE_BODY(name)`
    
    - **TRANSITION_(event, state)**
    
        When `nameMachine` receives the `event`, its state changes to `state` and executes the code in the function body.

    ```c
    TRANSITION_(event, state)
    {
        // fucntion body
    }
    ```

    - **TRANSITION_(event)**

        When `nameMachine` receives the `event`, its state does NOT changes and executes the code in the function body.

    ```c
    TRANSITION_(event)
    {
        // fucntion body
    }
    ```

---

- **ENTER()**

    - Check whether the incoming event is `ENTER_NEW_STATE`.
    - Only use this MACRO inside the body of `STATE_BODY(name)`

---

- **EXIT()**

    - Check whether the incoming event is `EXIT_CURRENT_STATE`.
    - Only use this MACRO inside the body of `STATE_BODY(name)`

---

- **SM_SWITCH(state)**

    - Swith directly to a other state. //TODO:
    Only use this MACRO inside the body of `STATE_BODY(name)`

---

- **SM_START(name, state)**

    - Start a `Machine` with initial as `state`. 
    - An `ENTER NEW_STATE` event will be posted to the machine immediately afterwards.

---

- **SM_POST(name, event)**

    - Post an `event` to `nameMachine`.
    - Event handler of `nameMachine` is pushed to `EventQueue` and then handle the `event`.

---

- **SM_EXECUTE(name, event)**

    - Call immediately to event handler of `nameMachine` instead of push its handler to `EventQueue`. 
    - Please consider carefully before using this MACRO.

## NOTE!!!

**When there is a state transition from state A to state B (from TRANSITION_ or SM_SWITCH):**

- **The system call State A's EXIT()**
- **The system change state from A to B**
- **The system call State B's ENTER()**

---

## EXAMPLE

- For an example of how to use `MACHINE`, refer to the files `test/test.c` and `test/test-sm.c`.