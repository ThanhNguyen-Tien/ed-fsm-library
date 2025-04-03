#ifndef CORE_QUEUE_H_
#define CORE_QUEUE_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Structure representing a queue (FIFO).
 */
typedef struct Queue
{
    uint16_t size;    ///< Maximum size of the queue.
    uint8_t* first;   ///< Pointer to the first element.
    uint8_t* last;    ///< Pointer to the last element.
    uint8_t* inPtr;   ///< Pointer to the next insertion position.
    uint8_t* outPtr;  ///< Pointer to the next removal position.
} queue_t;

/**
 * @brief Initializes a queue.
 * @param q Pointer to the queue structure.
 * @param buf Pointer to the buffer that stores queue elements.
 * @param size Maximum number of elements the queue can hold.
 */
void Queue_Init(queue_t* q, uint8_t* buf, uint16_t size);

/**
 * @brief Resets the queue to an empty state.
 * @param q Pointer to the queue structure.
 */
void Queue_Reset(queue_t* q);

/**
 * @brief Checks the number of free slots in the queue.
 * @param q Pointer to the queue structure.
 * @return Number of free slots available.
 */
uint16_t Queue_CheckNumOfFree(queue_t* q);

/**
 * @brief Get an element from the queue.
 * @param q Pointer to the queue structure.
 * @param val Pointer to store the removed element.
 * @return True if successful, false if the queue is empty.
 */
bool Queue_Pop(queue_t* q, uint8_t* val);

/**
 * @brief Adds an element to the queue.
 * @param q Pointer to the queue structure.
 * @param val Value to be added.
 * @return True if successful, false if the queue is full.
 */
bool Queue_Push(queue_t* q, uint8_t val);

/**
 * @brief Checks if the queue is empty.
 * @param q Pointer to the queue structure.
 * @return True if the queue is empty, false otherwise.
 */
bool Queue_IsEmpty(queue_t* q);

/**
 * @brief Checks if the queue is full.
 * @param q Pointer to the queue structure.
 * @return True if the queue is full, false otherwise.
 */
bool Queue_IsFull(queue_t* q);

#endif /* CORE_QUEUE_H_ */