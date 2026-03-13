# Embedded Event Framework -- Introduction

## 1. Overview

This framework is a lightweight **event-driven runtime for MCU systems**
designed for deterministic execution, minimal ISR work, and zero dynamic
memory allocation.

The architecture follows a **single consumer event loop** model:

-   Multiple producers (ISRs, Event Handlers - main loop) can post events.
-   A single consumer (Engine Dispatcher - main loop) executes all logic.

This approach eliminates most race conditions and simplifies reasoning
about system behavior.

Core design goals:

-   Deterministic memory usage
-   Minimal ISR execution time
-   No heap allocation
-   Event-driven system architecture
-   Easy debugging and tracing

------------------------------------------------------------------------

# 2. High Level Architecture

    ISR / Producers
          │
          ▼
    Lock-Free EventQueue (MPSC)
          │
          ▼
    Engine Main Loop
          │
          ├─ Machine (state machines)
          ├─ Strand (serialized execution)
          ├─ Signal (observer pattern)
          ├─ Timer
          └─ Events

Key principle:

**ISR never runs application logic. ISR only posts events.**

All system behavior is executed inside the Engine main loop.

------------------------------------------------------------------------

# 3. Engine

The **Engine** is the runtime scheduler of the framework.
   ![image](https://github.com/user-attachments/assets/89739d51-996a-4b5d-98b2-9eb4bca0734f)

Responsibilities:

-   Executes events from the EventQueue
-   Runs timers
-   Dispatches events to components
-   Measures CPU load

Typical execution loop:

    while(true)
    {
        if(events.next())
            continue;

        idle();
    }

The engine is the **single execution context** for all framework logic.

------------------------------------------------------------------------

# 4. Event System

Events are the fundamental communication mechanism in the framework.

Instead of calling functions directly between modules, components
communicate by **posting events**.

Benefits:

-   Decoupled modules
-   Deterministic execution order
-   Safe communication with ISR

Each event is automatically registered inside the Engine during
construction.

Event posting pushes a slot into the EventQueue.

------------------------------------------------------------------------

# 5. EventQueue

The EventQueue is the **only lock-free structure in the framework**.

It is designed as a:

**MPSC queue (Multi Producer Single Consumer)**

Producers:

-   ISR
-   Application code
-   Framework components

Consumer:

-   Engine main loop

Characteristics:

-   Lock-free implementation
-   Uses atomic operations
-   Supports ISR posting

This queue is the **boundary between concurrent contexts and the
single-threaded runtime**.

------------------------------------------------------------------------

# 6. Event Types

To optimize memory and performance, events are divided into three
categories.

### EmptyEvent

Event without payload.

Used for simple signals.

### SmallFixedEvent

Payload up to 4 bytes.

Stored directly inside the EventQueue slot.

### BigFixedEvent

Payload larger than 4 bytes.

Allocated from a **MemPool** and referenced by pointer.

This avoids copying large data inside the queue.

------------------------------------------------------------------------

# 7. Memory Management (MemPool)

The framework does not use dynamic heap allocation.

Instead it provides a **fixed size memory pool allocator**.

Characteristics:

-   Lock-free free-list
-   Deterministic allocation time
-   No fragmentation

MemPool is mainly used by:

-   BigFixedEvent
-   Signal subscribers

------------------------------------------------------------------------

# 8. State Machines (Machine)

The framework provides a **state machine base class** for building
deterministic logic.

Features:

-   Table-driven state transitions
-   ENTER / EXIT handling
-   Optional timeout support

Events are typically delivered to state machines using:

    SM_POST()

This ensures execution inside the Engine context.

`SM_EXECUTE()` exists for rare situations where immediate execution is
required and must be used carefully.

------------------------------------------------------------------------

# 9. Strand

A **Strand** guarantees serialized execution of events.

It ensures that events posted to the same strand are executed **one at a
time in order**, even if multiple producers submit work.

This is useful when several components must interact with a resource
sequentially.

------------------------------------------------------------------------

# 10. Signal System

Signals implement a lightweight **observer pattern**.

Types:

-   SignalOne -- single subscriber
-   SignalMany -- multiple subscribers

Subscribers receive events when a signal is emitted.

Signals internally post events to ensure execution occurs in the Engine
context.

------------------------------------------------------------------------

# 11. Timer System

Timers provide periodic or one-shot callbacks.

Features:

-   millisecond resolution
-   periodic or single-shot mode
-   integrated with Engine scheduler

Timer callbacks run as part of the event loop.

------------------------------------------------------------------------

# 12. Concurrency Model

The framework follows a strict concurrency model.

  Context       Allowed Actions
  ------------- ------------------
  ISR           Post events only
  Engine Loop   Execute logic
  Components    Post events

Important rule:

Application logic should run only inside the Engine loop.

This avoids race conditions and keeps the system deterministic.

------------------------------------------------------------------------

# 13. Advantages of the Architecture

### Deterministic behavior

No uncontrolled thread interaction.

### Minimal ISR load

Interrupt handlers only post events.

### No dynamic memory

All allocations use fixed pools.

### Easy debugging

System behavior can be traced through events.

------------------------------------------------------------------------

# 14. Typical Usage Pattern

Typical component workflow:

1.  ISR or component posts an event
2.  Event enters EventQueue
3.  Engine main loop pops event
4.  Event handler executes
5.  Handler may post new events

This forms a predictable event-driven system.

------------------------------------------------------------------------

# 15. Summary

This framework provides a structured way to build complex embedded
systems using an **event-driven architecture**.

Key ideas:

-   Everything communicates via events
-   Only EventQueue handles concurrency
-   Engine executes all logic

Following these rules keeps the system safe, predictable, and easy to
maintain.
