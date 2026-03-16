# Embedded Event Framework -- Detailed README

This document describes the **internal design and usage rules** of the
Embedded Event Framework.

The goal of this framework is to provide a **deterministic, event‑driven
execution environment** for MCU firmware without requiring an RTOS.

This file is intended for developers working **inside the framework or
building components on top of it**.

------------------------------------------------------------------------

# 1. Design Philosophy

The framework is built around several strict principles:

-   Deterministic real‑time behavior
-   No dynamic heap allocation
-   Event‑driven architecture
-   Minimal ISR work
-   Single execution context for logic
-   Lock‑free communication boundary
-   Clear ownership rules

This framework is **NOT an RTOS**.

It is a **message‑driven cooperative execution engine** designed
specifically for MCU systems.

Key idea:

> All application logic runs inside a single Engine event loop.

Concurrency only exists at the **event injection boundary**.

------------------------------------------------------------------------

# 2. High Level Architecture

    ISR / Producers
          │
          ▼
    Lock‑Free EventQueue (MPSC)
          │
          ▼
    Engine Main Loop
          │
          ├── Event Dispatch
          ├── Timer Scheduler
          ├── Machine (State Machines)
          ├── Strand (Serialized Execution)
          └── Signal System

Important rule:

**ISR never executes application logic. ISR only posts events.**

All logic runs inside the Engine loop.

------------------------------------------------------------------------

# 3. Execution Model

The system uses **cooperative scheduling**.

Main loop:

    while (true)
    {
        if (events.next())
            continue;

        idle();   // WFI
    }

Properties:

-   No preemption
-   No context switching
-   No task scheduler
-   No time slicing

Handlers must:

-   Execute quickly
-   Never block
-   Never busy‑wait
-   Avoid long computations

If long processing is needed, split work across multiple events.

------------------------------------------------------------------------

# 4. Event System

Events are the **primary communication mechanism** in the framework.

Components communicate by **posting events instead of calling functions
directly**.

Benefits:

-   Decoupled components
-   Deterministic execution order
-   Safe ISR communication
-   Easy debugging and tracing

Every event is registered automatically with the Engine during
construction.

Event posting pushes a slot into the EventQueue.

------------------------------------------------------------------------

# 5. Event Classification

Events are categorized based on payload size.

                         Event
                           │
               ┌───────────┴───────────┐
               │                       │
          EmptyEvent              FixedEvent
          (no data)                 (data)
                                       │
                           ┌───────────┴───────────┐
                           │                       │
                    SmallFixedEvent         BigFixedEvent
                     sizeof(data)<=4        sizeof(data)>4

------------------------------------------------------------------------

## 5.1 EmptyEvent

Contains only an event ID.

Used for:

-   Simple notifications
-   State triggers
-   Software signals

Fastest possible event type.

------------------------------------------------------------------------

## 5.2 SmallFixedEvent

Condition:

    sizeof(payload) <= 4 bytes

Mechanism:

-   Data copied directly into queue slot
-   No allocation
-   No pointer indirection

Advantages:

-   Deterministic
-   Extremely fast
-   Safe to post from ISR

------------------------------------------------------------------------

## 5.3 BigFixedEvent

Condition:

    sizeof(payload) > 4 bytes

Mechanism:

1.  Allocate memory from MemPool
2.  Copy payload into allocated block
3.  Store pointer in queue slot
4.  Push event to EventQueue

When Engine processes the event:

1.  Pointer extracted
2.  Handler executed
3.  Memory returned to pool

Queue structure remains constant size.

------------------------------------------------------------------------

# 6. EventQueue Design

The EventQueue is the **only lock‑free structure in the framework**.

Queue model:

**MPSC -- Multi Producer Single Consumer**

Producers:

-   ISR
-   Application code
-   Framework modules

Consumer:

-   Engine main loop

Each slot:

    struct EventSlot
    {
        uint32_t event_id;
        uint32_t payload;
        uint32_t timestamp;
    };

Properties:

-   Fixed slot size
-   O(1) push
-   O(1) pop
-   ISR safe
-   Lock‑free producer operations

This queue forms the **synchronization boundary** between concurrent
producers and the single‑threaded runtime.

------------------------------------------------------------------------

# 7. MemoryPool (MemPool)

Dynamic heap allocation is not used in the framework.

Instead a **fixed‑size memory pool allocator** is provided.

Characteristics:

-   Lock‑free free list
-   Constant allocation time
-   No fragmentation
-   Deterministic memory usage

Used by:

-   BigFixedEvent
-   SignalMany subscribers
-   Other optional internal allocations

------------------------------------------------------------------------

# 8. State Machine System (Machine)

The framework provides a **table‑driven finite state machine base
class**.

Features:

-   Explicit state transition tables
-   ENTER / EXIT handlers
-   Optional timeout events
-   Payload support

Typical event delivery:

    SM_POST()

This schedules the event through the Engine loop.

`SM_EXECUTE()` exists for rare situations requiring immediate execution
and should be used with caution.

------------------------------------------------------------------------

# 9. Strand

A **Strand** ensures serialized execution of tasks.

It guarantees:

-   Events execute **one at a time**
-   Execution order is preserved
-   No concurrent access to shared resources

Use Strand when multiple producers need to interact with the same
component safely.

------------------------------------------------------------------------

# 10. Signal System

Signals implement a lightweight **observer pattern**.

Types:

SignalOne -- single subscriber

SignalMany -- multiple subscribers

When a signal is emitted, events are posted to the subscribers.

Execution still occurs inside the Engine loop.

------------------------------------------------------------------------

# 11. Timer System

Timers provide delayed or periodic callbacks.

Features:

-   Millisecond resolution
-   Periodic or one‑shot
-   Integrated with Engine scheduler

Timer callbacks run as part of the event system.

------------------------------------------------------------------------

# 12. Concurrency Model

The framework enforces a strict concurrency boundary.

  Context       Allowed Actions
  ------------- -----------------
  ISR           Post events
  Engine loop   Execute logic
  Components    Post events

Key rule:

Application logic must run only inside the Engine loop.

This eliminates most race conditions.

------------------------------------------------------------------------

# 13. Ownership Model

Ownership rules are critical for system safety.

  Phase                      Owner
  -------------------------- ----------------------------------------
  Before post()              Caller
  After successful post()    Engine
  During handler execution   Engine
  After handler completes    Engine frees memory (if BigFixedEvent)

Rule:

Once `post()` returns successfully, the caller **must not access the
payload again**.

------------------------------------------------------------------------

# 14. Real‑Time Characteristics

Deterministic behavior:

-   No malloc
-   Fixed memory usage
-   Constant‑time queue operations

ISR safety:

-   Minimal ISR work
-   Only event posting

Cooperative execution:

-   Predictable execution order

------------------------------------------------------------------------

# 15. Typical Event Flow

    Interrupt / Component
            │
            ▼
         post()
            │
            ▼
       EventQueue
            │
            ▼
      Engine pops event
            │
            ▼
     Handler executes
            │
            ▼
     Optional new events

------------------------------------------------------------------------

# 16. Framework Strengths

-   Deterministic execution model
-   Minimal ISR latency
-   No runtime heap allocation
-   Clear ownership rules
-   Simple concurrency model
-   Easy debugging through event tracing
-   Portable across Cortex‑M MCUs

------------------------------------------------------------------------

# 17. Conceptual Summary

The framework behaves like a **message‑driven cooperative kernel** for
embedded systems.

    Interrupts
         ↓
       post()
         ↓
     EventQueue
         ↓
       Engine
         ↓
     FSM / Handlers
         ↓
     State changes

Everything in the system is driven by **events**.
