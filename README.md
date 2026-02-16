# Event-Driven - Finite State Machine Library for ARM Cortex-M Series

Welcome to the event-driven library for ARM Cortex-M microcontrollers. This library is designed to simplify the development of event-driven applications, making your embedded systems more efficient and responsive.

## Introduction to Event-Driven Programming

Event-driven programming is a programming paradigm in which the flow of the program is determined by events—such as user actions (mouse clicks, key presses), sensor outputs, or messages from other programs or threads. In an event-driven application, there is a main loop that listens for events and dispatches them to appropriate event handlers.

   ### Key Concepts
   
   1. **Event**: An occurrence or action that the system can recognize and respond to. Examples include hardware interrupts, timer overflows, or custom user-defined events.
      
   2. **Event Handler**: A function or method that gets called in response to a specific event. Event handlers contain the logic to process events.
   
   3. **Event Queue**: A data structure that holds events until they are processed by the event handler. Events are usually processed in the order they arrive (FIFO - First In, First Out).
   
   4. **Dispatcher**: A component that listens for events in the event queue and calls the appropriate event handler.

   ### How It Works
   
   The following diagram illustrates the basic workflow of an event-driven system:
   
   ![image](https://github.com/user-attachments/assets/89739d51-996a-4b5d-98b2-9eb4bca0734f)

   ### Advantages
   
   - **Modularity**: Event-driven systems promote modularity by separating event detection from event handling.
   - **Responsiveness**: Systems can respond quickly to asynchronous events.
   - **Scalability**: Easy to add new event types and handlers without affecting existing code.

## Library Features

- **Lightweight**: Designed to be minimal in resource usage, suitable for embedded systems with limited memory and processing power.
- **Flexible**: Supports custom event types and handlers.
- **Efficient**: Optimized for ARM Cortex-M microcontrollers.


## Library Note

- **This library is best used for bare-metal systems, which are systems that do not use an OS.**

- **Let's make everythings Asynchronous & non-Blocking.**

- **For MCU has only 1 core, we only have 1 main loop. Please consider carefully if you intend to use any blocking functions such as HAL_Delay(), while(!UART_Ready()),... or similar because it will affect the EventQueue, delaying the whole system.**
