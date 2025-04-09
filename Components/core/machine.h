#ifndef CORE_MACHINE_H_
#define CORE_MACHINE_H_

#include "event.h"
#include "task.h"

// Special event codes for state machine transitions
#define MACHINE_TIMEOUT_EVENT	0xFD /**< Event code for timeout events. */
#define ENTER_NEW_STATE			0xFE /**< Event code for entering a new state. */
#define EXIT_CURRENT_STATE      0xFF /**< Event code for exiting the current state. */

/**
 * @brief Forward declaration of the Machine structure.
 */
typedef struct Machine machine_t;

/**
 * @brief Function pointer type for state functions.
 *
 * Each state function takes a pointer to the machine as its parameter.
 */
typedef void (*machineState)(machine_t*);

/**
 * @brief Structure representing a state machine.
 *
 * This structure holds the current and next states, a timer for managing timeouts,
 * an event for execution, and the next event to be processed.
 */
typedef struct Machine
{
    machineState nextState;      /**< Pointer to the next state function. */
    machineState currentState;   /**< Pointer to the current state function. */

    task_t timer;                /**< Timer task for managing timeouts. */
    event_t executeEvent;        /**< Event used to trigger state execution. */

    uint8_t nextEvent;           /**< Code of the next event to be processed. */
} machine_t;

/**
 * @brief Structure representing a machine event.
 *
 * This structure is used to pass events to the state machine for processing.
 */
typedef struct MachineEvent
{
    machine_t* mPtr; /**< Pointer to the machine associated with the event. */
    uint8_t event;   /**< Event code to be processed by the machine. */
} machine_event_t;

/**
 * @brief Initializes a state machine.
 *
 * This function sets up the state machine, preparing it for operation.
 *
 * @param m Pointer to the machine to be initialized.
 */
void Machine_Init(machine_t *m);

/**
 * @brief Executes the current state of the machine.
 *
 * This function processes the given event and executes the appropriate state function.
 *
 * @param msg Pointer to the machine event to be processed.
 */
void Machine_Execute(void* msg);

/**
 * @brief Posts an event to the state machine.
 *
 * This function queues an event for the machine to process.
 *
 * @param m Pointer to the machine.
 * @param event Event code to be posted.
 */
void Machine_PostEvent(machine_t* m, uint8_t event);

/**
 * @brief Starts the state machine with an initial state.
 *
 * This function sets the initial state of the machine and begins its operation.
 *
 * @param m Pointer to the machine.
 * @param s Pointer to the initial state function.
 */
void Machine_Start(machine_t* m, machineState s);

/**
 * @brief Starts a timer for the state machine.
 *
 * This function sets up a timer for the machine, which can be used for timeout events.
 *
 * @param m Pointer to the machine.
 * @param interval Timer interval in milliseconds.
 * @param loop Number of times the timer should loop (-1 for infinite).
 */
void Machine_StartTimer(machine_t* m, uint32_t interval, int32_t loop);

/**
 * @brief Stops the timer for the state machine.
 *
 * This function stops the timer associated with the machine.
 *
 * @param m Pointer to the machine.
 */
void Machine_StopTimer(machine_t* m);

/**
 * @brief Checks if a specific event matches the current state.
 *
 * This function verifies if the given event and state match the machine's current state.
 *
 * @param m Pointer to the machine.
 * @param input Event code to check.
 * @param state Pointer to the state function to check.
 * @return `true` if the event and state match, `false` otherwise.
 */
bool Machine_Check(machine_t* m, uint8_t input, machineState state);

#define MACHINE_DEF(name)\
    extern machine_t name##Machine;

#define MACHINE(name)\
    machine_t name##Machine;\

#define STATE_DEF(name) void name(machine_t* m);
#define STATE_BODY(name) void name(machine_t* m)

#define _TRANSITION_2(ev,st) if (Machine_Check(m, (uint8_t)ev, st))
#define _TRANSITION_1(ev) if ((uint8_t)ev == m->nextEvent)

#define _TRANSITION_NARGS2(_1, _2, N, ...) N
#define _TRANSITION_NARGS(...) _TRANSITION_NARGS2(__VA_ARGS__, 2, 1)
#define _TRANSITION_CHOOSER2(count) _TRANSITION_ ## count
#define _TRANSITION_CHOOSER(count) _TRANSITION_CHOOSER2(count)

#define TRANSITION_(...) _TRANSITION_CHOOSER(_TRANSITION_NARGS(__VA_ARGS__))(__VA_ARGS__)

#define _SM_TIMEOUT_START_3(name, interval, loop) Machine_StartTimer(&name##Machine, interval, loop)
#define _SM_TIMEOUT_START_2(name, interval) Machine_StartTimer(&name##Machine, interval, 0)
#define _SM_TIMEOUT_START_NARGS3(_1, _2, _3, N, ...) N
#define _SM_TIMEOUT_START_NARGS(...) _SM_TIMEOUT_START_NARGS3(__VA_ARGS__, 3, 2)

#define _SM_TIMEOUT_START_CHOOSER2(count) _SM_TIMEOUT_START_ ## count
#define _SM_TIMEOUT_START_CHOOSER(count) _SM_TIMEOUT_START_CHOOSER2(count)
#define SM_TIMEOUT_START(...) _SM_TIMEOUT_START_CHOOSER(_SM_TIMEOUT_START_NARGS(__VA_ARGS__))(__VA_ARGS__)


#define SM_TIMEOUT_STOP(name) Machine_StopTimer(&name##Machine)

#define ENTER_() if (m->nextEvent == ENTER_NEW_STATE)
#define EXIT_() if (m->nextEvent == EXIT_CURRENT_STATE)
#define TIMEOUT_() if (m->nextEvent == MACHINE_TIMEOUT_EVENT)
#define SM_SWITCH(state) m->nextState = &state
#define SM_START(name, state)\
    Machine_Init(&name##Machine);\
    Machine_Start(&name##Machine, (machineState)&state)
#define SM_POST(name, event) Machine_PostEvent(&name##Machine, (uint8_t)event)
#define SM_EXECUTE(name, event_)\
{machine_event_t e = {.mPtr=&name##Machine, .event = (uint8_t)event_};\
Machine_Execute(&e);}

#define SM_CHECK(name, state) (name##Machine.currentState == &state)
#endif /* CORE_MACHINE_H_ */
