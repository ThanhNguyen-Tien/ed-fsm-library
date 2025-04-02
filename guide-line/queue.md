# queue.h

This file defines the `Queue` structure and its associated functions.

The queue operates on `uint8_t` data types and provides a simple way to manage buffered data efficiently.

## Function Descriptions

### `void Queue_Init(queue_t* q, uint8_t* buf, uint16_t size);`

Initializes a queue with a provided buffer and size. The maximum number of elements the queue can hold is `(size - 1)`.

- **Parameters:**
  - `q`: Pointer to the queue structure to initialize.
  - `buf`: Pointer to the buffer used for storing queue elements.
  - `size`: Size of the buffer in bytes.

---

### `void Queue_Reset(queue_t* q);`

Clears all elements in the queue, resetting it to an empty state.

- **Parameters:**
  - `q`: Pointer to the queue structure.

---

### `uint16_t Queue_CheckNumOfFree(queue_t* q);`

Returns the number of available slots in the queue.

- **Parameters:**
  - `q`: Pointer to the queue structure.

- **Returns:**
  - The number of free slots in the queue.

---

### `bool Queue_Pop(queue_t* q, uint8_t* val);`

Retrieves and removes the oldest element from the queue.

- **Parameters:**
  - `q`: Pointer to the queue structure.
  - `val`: Pointer to a variable where the popped value will be stored.

- **Returns:**
  - `true` if an element was successfully popped.
  - `false` if the queue is empty.

---

### `bool Queue_Push(queue_t* q, uint8_t val);`

Adds a new element to the queue.

- **Parameters:**
  - `q`: Pointer to the queue structure.
  - `val`: The value to be added.

- **Returns:**
  - `true` if the element was successfully pushed.
  - `false` if the queue is full.

---

### `bool Queue_IsEmpty(queue_t* q);`

Checks whether the queue is empty.

- **Parameters:**
  - `q`: Pointer to the queue structure.

- **Returns:**
  - `true` if the queue contains no elements.
  - `false` if it has at least one element.

---

### `bool Queue_IsFull(queue_t* q);`

Checks whether the queue is full.

- **Parameters:**
  - `q`: Pointer to the queue structure.

- **Returns:**
  - `true` if the queue has no available space.
  - `false` if there is room for more elements.
