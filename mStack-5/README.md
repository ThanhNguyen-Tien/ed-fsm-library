# Event-Driven Stack & FSM Framework

## 1. Design Philosophy

This framework is designed for:

- Deterministic real-time behavior
- Zero dynamic memory allocation
- Strict ownership model
- Constant-time ISR operations
- Clear separation of responsibilities
- Cooperative scheduling (non-preemptive)

This is **NOT an RTOS**.  
This is a minimal message-driven execution engine for MCU systems.

---

## 2. System Architecture Overview
```yaml
+--------------------------------------------------+
| Application Layer |
| (FSMs / Event Handlers) |
+--------------------------+-----------------------+
                           |
                           v
+--------------------------------------------------+
| Engine Core |
| - Event Dispatch |
| - Main Loop (Cooperative) |
+--------------------------+-----------------------+
                           |
                           v
+--------------------------------------------------+
| EventQueue |
| Fixed Slot (2 x 32-bit words) |
+--------------------------+-----------------------+
                           |
                           v
+--------------------------------------------------+
| MemoryPool |
| (Used only for BigFixedEvent) |
+--------------------------------------------------+
```

---

## 3. Execution Model (Cooperative)

The entire system runs in a **single main loop**:

```cpp
while (true)
{
    if (eventQueue.not_empty())
    {
        pop_event();
        dispatch();
    }
    else
    {
        WAIT_FOR_INTERRUPT;
    }
}
```
- No preemption

- No context switching

- No scheduler

- No time slicing

- Handlers must: 
    - Execute fast
    - Never block
    - Never spin-wait

All concurrency comes from posting events.

---

## 4. Event Sources
Events may originate from:

- External ISR: UART, SPI, I2C, EXTI
- Internal ISR: TIMER, ADC, DMA
- Main Loop / Other Handlers: Event chaining, FSM transitions, Deferred processing

All sources converge into the same EventQueue.

---

## 5. Event Type System
Events are classified along two axes:
- Presence of data
- Data size

---

### 5.1 Event Classification Diagram
```pgsql
                         +----------------+
                         |     Event      |
                         +----------------+
                                 |
                 ---------------------------------
                 |                               |
         +---------------+               +----------------+
         |  EmptyEvent   |               |   FixedEvent   |
         |  (no data)    |               |     (data)     |
         +---------------+               +----------------+
                                                 |
                                  --------------------------------
                                  |                              |
                         +------------------+           +------------------+
                         | SmallFixedEvent  |           |  BigFixedEvent   |
                         | sizeof(data)<=4  |           | sizeof(data)>4   |
                         +------------------+           +------------------+
```
---

### 5.2 EmptyEvent
- Contains only event_id
- Payload = nullptr
- Fastest possible event

Used for:
- State triggers
- Simple notifications
- Software signals

---

### 5.3 SmallFixedEvent
Condition:
```csharp
sizeof(data) <= 4 bytes
```
Mechanism:
- Data copied directly into 32-bit payload
- No memory allocation
- No indirection

Advantages:
- Fully deterministic
- Extremely fast
- Ideal for ISR usage

---

### 5.4 BigFixedEvent
Condition:
```csharp
sizeof(data) > 4 bytes
```
Mechanism:
- Request slot from MemoryPool
- Copy data into allocated slot
- Store pointer in 32-bit payload
- Push to EventQueue

Engine will:
- Extract pointer
- Call handler
- Free memory after execution

Important:
The EventQueue still processes only:
```cpp
(uint32_t id, uint32_t payload)
```
So queue operations remain constant-time.

---

## 6. EventQueue Design (Fixed Slot – 2 Words)
Each slot:
```cpp
struct EventSlot
{
    uint32_t id;
    uint32_t payload;
};
```
Properties:
- 4-byte aligned
- Single atomic write per field
- Cache-friendly
- O(1) push
- O(1) pop

Even BigFixedEvent does NOT increase queue cost.

---

## 7. Ownership Model (Critical Design Principle)
This is the most important concept of the system.

---

## 7.1 Ownership Lifecycle

| Phase                      | Owner  |
|----------------------------|--------|
| Before `post()`            | Caller |
| After successful `post()`  | Engine |
| During handler execution   | Engine |
| After handler completes    | Engine (frees memory if BigFixedEvent) |

---

### 7.2 Ownership Flow (BigFixedEvent)
```yaml
Caller
   |
   |  alloc()
   v
MemoryPool
   |
   |  copy data
   v
EventQueue (payload = pointer)
   |
   v
Engine pop
   |
   v
Handler executes
   |
   v
Engine free()
```
Key rule:

> Once post() returns successfully, caller must NEVER touch the data again.

This eliminates:
- Double free
- Use-after-free
- Memory corruption
- Ownership ambiguity

---

## 8. Critical Section Strategy (BASEPRI)
The system is cooperative, but ISR can interrupt main loop.
We must protect queue slot modification.
The framework uses:
```nginx
BASEPRI masking
```
Instead of:
- Global interrupt disable (PRIMASK)
- Full critical lock

---

### 8.1 Why BASEPRI?
BASEPRI allows:
- Blocking lower-priority interrupts
- Allowing higher-priority interrupts to still run

This ensures:
- Deterministic latency
- Fine-grained interrupt control
- Better real-time behavior

---

### 8.2 Critical Section Scope
Critical section is minimal:
- Only protect:
    - Head index update
    - Tail index update
    - Slot write

Example conceptual flow:
```pgsql
Enter Critical Section
    write slot
    update head
Exit Critical Section
```
Critical section duration is extremely short.

---

## 9. Real-Time Characteristics
### Deterministic Behavior
- No malloc
- No heap fragmentation
- Bounded memory usage
- Constant-time queue ops

---

### ISR Safety
- SmallFixedEvent → direct payload copy
- BigFixedEvent → allocation outside queue
- Queue always pushes 2 words only

---

### Cooperative Execution
- No preemption inside handlers
- Predictable execution order
- Fully controlled scheduling

---

## 10. Memory Model Summary
Memory usage is fully static:
- EventQueue: fixed array
- MemoryPool: fixed block pool, allocation occurs only once at startup.
- No runtime growth

Worst-case RAM usage is known at compile time.

---

## 11. Conceptual Summary
This framework behaves like a:
> Mini message-driven cooperative kernel for microcontrollers.
```csharp
    Interrupts
        ↓
    post()
        ↓
    EventQueue
        ↓
    Engine (main loop)
        ↓
    FSM / Handlers
        ↓
    State Transitions
```
All behavior is:
- Explicit
- Deterministic
- Ownership-safe
- RT-friendly

---

## 12. Design Strengths
- Strict ownership model
- Clear event hierarchy
- Separation of memory and scheduling
- Minimal ISR latency
- Zero dynamic allocation (at runtime)
- Portable across Cortex-M devices
- Easy to reason about

---