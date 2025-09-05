#ifndef CORE_STRAND_H_
#define CORE_STRAND_H_

#include "queue.h"
#include "event.h"
#include "task.h"

/**
 * @brief Structure representing a Strand, which manages asynchronous execution and event handling.
 *
 * Contains timer, event, queues, payload, and busy status for strand management.
 */
typedef struct Strand
{
    task_t timer;             ///< Timer task associated with the strand.
    event_t execute;          ///< Event to trigger execution.
    event_t *finished;        ///< Pointer to event indicating completion.
    event_queue_t *evQueue;   ///< Pointer to the event queue for the strand.
    queue_t *command;         ///< Pointer to the command queue.
    uint8_t *payload;         ///< Pointer to the payload buffer.
    bool busy;                ///< Indicates if the strand is currently busy.
} strand_t;

/**
 * @brief Enumeration for strand event types.
 */
typedef enum
{
    VOID = 0,    ///< No event.
    CALLBACK,    ///< Callback event.
    DELAY        ///< Delay event.
} strand_event_t;

/**
 * @brief Initializes a strand with the given queue.
 *
 * @param s Pointer to the strand structure.
 * @param q Pointer to the command queue.
 */
void Strand_Init(strand_t *s, queue_t *q);

/**
 * @brief Marks the strand as done (completed).
 *
 * @param s Pointer to the strand structure.
 */
void Strand_Done(strand_t *s);

/**
 * @brief Marks the strand as done with an error code.
 *
 * @param s Pointer to the strand structure.
 * @param err Error code to set.
 */
void Strand_Done_With_Err(strand_t *s, uint8_t err);

/**
 * @brief Posts an event to the strand, optionally with a callback and data.
 *
 * @param s Pointer to the strand structure.
 * @param ev Pointer to the event to post.
 * @param cb Pointer to the callback event.
 * @param data Pointer to the event data.
 * @return true if the event was posted successfully, false otherwise.
 */
bool Strand_Post(strand_t *s, event_t *ev, event_t *cb, void *data);

/**
 * @brief Requests a delay for the strand for a specified number of milliseconds.
 *
 * @param s Pointer to the strand structure.
 * @param ms Delay duration in milliseconds.
 * @return true if the delay was set successfully, false otherwise.
 */
bool Strand_Delay(strand_t *s, uint32_t ms);

#define M_STRAND_DEF(name)	\
	extern strand_t name##Strand;

#define M_STRAND(name, size)\
	strand_t name##Strand;	\
	queue_t name##StrandQueue;\
	uint8_t name##StrandBuffer[size] = {0};

#define M_STRAND_INIT(name)\
	Queue_Init(&name##StrandQueue, name##StrandBuffer, sizeof(name##StrandBuffer)); \
	Strand_Init(&name##Strand, &name##StrandQueue);

#endif /* CORE_STRAND_H_ */
