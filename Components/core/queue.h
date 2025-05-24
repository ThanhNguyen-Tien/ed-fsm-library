#ifndef CORE_QUEUE_H_
#define CORE_QUEUE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

/**
 * @brief Structure representing a circular queue.
 *
 * This structure is used to manage a circular queue, which allows efficient
 * insertion and removal of elements in a fixed-size buffer.
 */
typedef struct Queue
{
    uint16_t size;       /**< Total size of the queue in bytes. */
    uint8_t* first;      /**< Pointer to the first element in the queue buffer. */
    uint8_t* last;       /**< Pointer to the last element in the queue buffer. */
    uint8_t* inPtr;      /**< Pointer to the current insertion position in the queue. */
    uint8_t* outPtr;     /**< Pointer to the current removal position in the queue. */
} queue_t;

/**
 * @brief Initializes a circular queue.
 *
 * This function sets up the queue structure with the specified buffer and size.
 *
 * @param q Pointer to the queue structure to be initialized.
 * @param buf Pointer to the buffer to be used for the queue.
 * @param size Size of the buffer in bytes.
 */
static inline void Queue_Init(queue_t* q, uint8_t* buf, uint16_t size)
{
	assert(q != NULL);

	q->first = buf;
	q->size = size;
	q->last = q->first + size;
	q->inPtr = q->first;
	q->outPtr = q->first;
}

/**
 * @brief Resets a circular queue.
 *
 * This function clears the queue, making it empty and ready for reuse.
 *
 * @param q Pointer to the queue structure to be reset.
 */
static inline void Queue_Reset(queue_t* q)
{
	assert(q != NULL);

	q->inPtr = q->first;
	q->outPtr = q->first;
}

/**
 * @brief Checks the number of free bytes in the queue.
 *
 * This function calculates the amount of unused space in the queue.
 *
 * @param q Pointer to the queue structure.
 * @return The number of free bytes in the queue.
 */
static inline uint16_t Queue_CheckNumOfFree(queue_t* q)
{
	assert(q != NULL);

	uint16_t ret = q->size + q->outPtr - q->inPtr;
	if (ret > q->size) ret -= q->size;
	return (ret - 1);
}

/**
 * @brief Checks if the queue is empty.
 *
 * This function determines whether the queue contains any elements.
 *
 * @param q Pointer to the queue structure.
 * @return `true` if the queue is empty, `false` otherwise.
 */
static inline bool Queue_IsEmpty(queue_t* q)
{
	assert(q != NULL);

	return (q->inPtr == q->outPtr);
}

/**
 * @brief Checks if the queue is full.
 *
 * This function determines whether the queue has reached its maximum capacity.
 *
 * @param q Pointer to the queue structure.
 * @return `true` if the queue is full, `false` otherwise.
 */
static inline bool Queue_IsFull(queue_t* q)
{
	assert(q != NULL);

	return ((q->inPtr + 1 == q->outPtr) || (q->inPtr == q->last - 1 && q->outPtr == q->first));
}

/**
 * @brief Removes an element from the queue.
 *
 * This function retrieves and removes the oldest element from the queue.
 *
 * @param q Pointer to the queue structure.
 * @param val Pointer to a variable where the removed value will be stored.
 * @return `true` if an element was successfully removed, `false` if the queue is empty.
 */
static inline bool Queue_Pop(queue_t* q, uint8_t* val)
{
	assert(q != NULL);
	assert(val != NULL);

	if (q->outPtr != q->inPtr)
	{
		*val = *(q->outPtr);
		q->outPtr++;
		if (q->outPtr == q->last) q->outPtr = q->first;
		return true;
	}
    return false;
}

/**
 * @brief Adds an element to the queue.
 *
 * This function inserts a new element into the queue.
 *
 * @param q Pointer to the queue structure.
 * @param val The value to be added to the queue.
 * @return `true` if the element was successfully added, `false` if the queue is full.
 */
static inline bool Queue_Push(queue_t* q, uint8_t val)
{
	assert(q != NULL);

	uint8_t* next = q->inPtr + 1;
	if (next == q->last) next = q->first;
	if (next != q->outPtr)
	{
		*(q->inPtr) = val;
		q->inPtr = next;
		return true;
	}
	else return false;
}

#endif /* CORE_QUEUE_H_ */