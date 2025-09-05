#ifndef CORE_QUEUE_H_
#define CORE_QUEUE_H_

#include <stddef.h>
#include "event.h"

/**
 * @brief Structure representing a circular queue.
 *
 * Contains pointers and size information for managing a circular buffer.
 */
typedef struct Queue
{
    uint16_t size;      ///< Total size of the queue buffer.
    uint8_t *first;     ///< Pointer to the first element in the buffer.
    uint8_t *last;      ///< Pointer to one past the last element in the buffer.
    uint8_t *inPtr;     ///< Pointer to the next insertion position.
    uint8_t *outPtr;    ///< Pointer to the next removal position.
} queue_t;

/**
 * @brief Initializes the queue with a buffer and size.
 *
 * Sets up the queue structure to use the provided buffer and size.
 *
 * @param q Pointer to the queue structure.
 * @param buf Pointer to the buffer to use for the queue.
 * @param size Size of the buffer.
 */
static inline void Queue_Init(queue_t *q, uint8_t *buf, uint16_t size)
{
    ASSERT(q != NULL);

    q->first = buf;             // Set the start of the buffer.
    q->size = size;             // Set the buffer size.
    q->last = q->first + size;  // Set the end of the buffer.
    q->inPtr = q->first;        // Initialize insertion pointer.
    q->outPtr = q->first;       // Initialize removal pointer.
}

/**
 * @brief Resets the queue pointers to the start of the buffer.
 *
 * Sets both the insertion and removal pointers to the start of the buffer.
 *
 * @param q Pointer to the queue structure.
 */
static inline void Queue_Reset(queue_t *q)
{
    ASSERT(q != NULL);

    q->inPtr = q->first;        // Reset insertion pointer.
    q->outPtr = q->first;       // Reset removal pointer.
}

/**
 * @brief Returns the number of free slots available in the queue.
 *
 * Calculates the available space in the queue for new elements.
 *
 * @param q Pointer to the queue structure.
 * @return Number of free slots in the queue.
 */
static inline uint16_t Queue_CheckNumOfFree(queue_t *q)
{
    ASSERT(q != NULL);

    uint16_t ret = q->size + q->outPtr - q->inPtr;
    if (ret > q->size)
        ret -= q->size;
    return (ret - 1);           // Subtract 1 to avoid ambiguity between full and empty.
}

/**
 * @brief Checks if the queue is empty.
 *
 * @param q Pointer to the queue structure.
 * @return true if the queue is empty, false otherwise.
 */
static inline bool Queue_IsEmpty(queue_t *q)
{
    ASSERT(q != NULL);

    return (q->inPtr == q->outPtr);
}

/**
 * @brief Checks if the queue is full.
 *
 * @param q Pointer to the queue structure.
 * @return true if the queue is full, false otherwise.
 */
static inline bool Queue_IsFull(queue_t *q)
{
    ASSERT(q != NULL);

    return ((q->inPtr + 1 == q->outPtr)
            || (q->inPtr == q->last - 1 && q->outPtr == q->first));
}

/**
 * @brief Pushes a value into the queue.
 *
 * Adds a value to the queue if there is space available.
 *
 * @param q Pointer to the queue structure.
 * @param val Value to push into the queue.
 * @return true if successful, false if the queue is full.
 */
static inline bool Queue_Push(queue_t *q, uint8_t val)
{
    ASSERT(q != NULL);

    uint8_t *next = q->inPtr + 1;
    if (next == q->last)
        next = q->first;
    if (next != q->outPtr)
    {
        *(q->inPtr) = val;      // Store value at insertion pointer.
        q->inPtr = next;        // Advance insertion pointer.
        return true;
    } else
        return false;           // Queue is full.
}

/**
 * @brief Pops a value from the queue.
 *
 * Removes a value from the queue if it is not empty.
 *
 * @param q Pointer to the queue structure.
 * @param val Pointer to store the popped value.
 * @return true if successful, false if the queue is empty.
 */
static inline bool Queue_Pop(queue_t *q, uint8_t *val)
{
    ASSERT(q != NULL);
    ASSERT(val != NULL);

    if (q->outPtr != q->inPtr)
    {
        *val = *(q->outPtr);    // Retrieve value at removal pointer.
        q->outPtr++;            // Advance removal pointer.
        if (q->outPtr == q->last)
            q->outPtr = q->first;
        return true;
    }
    return false;               // Queue is empty.
}

#endif /* CORE_QUEUE_H_ */
