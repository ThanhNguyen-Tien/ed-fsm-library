#ifndef CORE_STRAND_H_
#define CORE_STRAND_H_

#include "queue.h"
#include "event.h"
#include "task.h"

/**
 * @brief Structure representing a strand.
 */
typedef struct Strand
{
    task_t timer;          ///< Timer associated with the strand.
    event_t execute;       ///< Execution event.
    event_t* finished;     ///< Pointer to event callback.
    event_queue_t* evQueue; ///< Event queue for managing events.
    queue_t* command;      ///< Command queue.
    uint8_t* payload;      ///< Data payload.
    bool busy;             ///< Status flag indicating busy state.
} strand_t;

/**
 * @brief Enumeration for different strand event types.
 */
typedef enum {
    VOID=0,    ///< No callback event.
    CALLBACK,  ///< Has callback event.
    DELAY      ///< Delay event.
} strand_event_t;

/**
 * @brief Initializes a strand.
 * @param s Pointer to the strand structure.
 * @param q Pointer to the queue associated with the strand.
 */
void Strand_Init(strand_t* s, queue_t* q);

/**
 * @brief Marks a strand as completed without callback.
 * @param s Pointer to the strand.
 */
void Strand_Done(strand_t* s);

/**
 * @brief Marks a strand as completed and send callback with an error code.
 * @param s Pointer to the strand.
 * @param err Error code.
 */
void Strand_Done_With_Err(strand_t* s, uint8_t err);

/**
 * @brief Posts an event to the strand for execution.
 * @param s Pointer to the strand structure.
 * @param ev Pointer to the event to be posted.
 * @param cb Pointer to the callback event to be triggered upon completion.
 * @param data Pointer to the data associated with the event.
 * @return true if the event was successfully posted, false otherwise.
 */
bool Strand_Post(strand_t* s, event_t* ev, event_t* cb, void* data);

/**
 * @brief Introduces a delay in the execution of the strand.
 * @param s Pointer to the strand structure.
 * @param ms Delay duration in milliseconds.
 * @return true if the delay was successfully set, false otherwise.
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
