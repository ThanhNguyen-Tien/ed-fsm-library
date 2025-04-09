#ifndef CORE_STRAND_H_
#define CORE_STRAND_H_

#include "queue.h"
#include "event.h"
#include "task.h"

/**
 * @brief Structure representing a Strand.
 *
 * A Strand is a lightweight cooperative multitasking mechanism that manages
 * tasks, events, and commands in a serialized manner. It ensures that only
 * one task or event is executed at a time within the Strand.
 */
typedef struct Strand
{
    task_t timer;                /**< Timer task for managing delays. */
    event_t execute;             /**< Event used to trigger execution. */
    event_t* finished;           /**< Pointer to an event indicating completion. */
    event_queue_t* evQueue;      /**< Pointer to the event queue associated with the Strand. */
    queue_t* command;            /**< Pointer to the command queue for the Strand. */
    uint8_t* payload;            /**< Pointer to the payload data for the Strand. */
    bool busy;                   /**< Flag indicating whether the Strand is busy. */
}strand_t;

/**
 * @brief Enumeration of Strand event types.
 *
 * Defines the types of events that can be handled by a Strand.
 */
typedef enum {
    VOID = 0,    /**< No specific event type. */
    CALLBACK,    /**< Callback event type. */
    DELAY        /**< Delay event type. */
}strand_event_t;

/**
 * @brief Initializes a Strand.
 *
 * This function initializes the given Strand with the specified command queue.
 *
 * @param s Pointer to the Strand to be initialized.
 * @param q Pointer to the command queue to be associated with the Strand.
 */
void Strand_Init(strand_t* s, queue_t* q);

/**
 * @brief Marks a Strand as done.
 *
 * This function signals that the Strand has completed its current task or event.
 *
 * @param s Pointer to the Strand to be marked as done.
 */
void Strand_Done(strand_t* s);

/**
 * @brief Marks a Strand as done with an error.
 *
 * This function signals that the Strand has completed its current task or event
 * with an error.
 *
 * @param s Pointer to the Strand to be marked as done.
 * @param err Error code indicating the type of error.
 */
void Strand_Done_With_Err(strand_t* s, uint8_t err);

/**
 * @brief Posts an event to a Strand.
 *
 * This function posts an event to the Strand for execution. Optionally, a callback
 * event and user-defined data can be provided.
 *
 * @param s Pointer to the Strand to which the event will be posted.
 * @param ev Pointer to the event to be posted.
 * @param cb Pointer to the callback event (optional).
 * @param data Pointer to user-defined data to be passed to the event handler.
 * @return `true` if the event was successfully posted, `false` otherwise.
 */
bool Strand_Post(strand_t* s, event_t* ev, event_t* cb, void* data);

/**
 * @brief Adds a delay to a Strand.
 *
 * This function adds a delay to the Strand, during which no other tasks or events in Strand
 * can be executed.
 *
 * @param s Pointer to the Strand to which the delay will be added.
 * @param ms Delay duration in milliseconds.
 * @return `true` if the delay was successfully added, `false` otherwise.
 */
bool Strand_Delay(strand_t* s, uint32_t ms);

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
