# queue.h

This file declares the definitions for `Queue`.

You can use it to create your own Queue with data type is `uint8_t`.

## FUNCTION DESCRIPTION

### `void Queue_Init(queue_t* q, uint8_t* buf, uint16_t size);`

Initializes a queue structure with the provided buffer and size. This queue can contain at most (size - 1) elements.

- **Parameters:**
  - `q`: Pointer to the queue structure to initialize.
  - `buf`: Pointer to the buffer used for storing queue elements.
  - `size`: Size of the buffer, in bytes.

---

### `void Queue_Reset(queue_t* q);`

Resets the queue, clearing all elements.

- **Parameters:**
  - `q`: Pointer to the queue structure to reset.

---

### `uint16_t Queue_CheckNumOfFree(queue_t* q);`

Checks the number of free slots available in the queue.

- **Parameters:**
  - `q`: Pointer to the queue structure to check.

- **Returns:**
  - Number of free slots in the queue.

---

### `bool Queue_Pop(queue_t* q, uint8_t* val);`

Pops an element from the queue.

- **Parameters:**
  - `q`: Pointer to the queue structure.
  - `val`: Pointer to store the popped value.

- **Returns:**
  - `true` if an element was successfully popped, `false` if the queue is empty.

---

### `bool Queue_Push(queue_t* q, uint8_t val);`

Pushes an element into the queue.

- **Parameters:**
  - `q`: Pointer to the queue structure.
  - `val`: Value to push into the queue.

- **Returns:**
  - `true` if the element was successfully pushed, `false` if the queue is full.

---

### `bool Queue_IsEmpty(queue_t* q);`

Checks if the queue is empty.

- **Parameters:**
  - `q`: Pointer to the queue structure to check.

- **Returns:**
  - `true` if the queue is empty, `false` otherwise.

---

### `bool Queue_IsFull(queue_t* q);`

- **Parameters:**
  - `q`: Pointer to the queue structure to check.

- **Returns:**
  - `true` if the queue is full, `false` otherwise.