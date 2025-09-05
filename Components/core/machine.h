#ifndef CORE_MACHINE_H_
#define CORE_MACHINE_H_

#include "event.h"
#include "task.h"

// Special event codes for state machine control.
#define MACHINE_TIMEOUT_EVENT	0xFD
#define ENTER_NEW_STATE			0xFE
#define EXIT_CURRENT_STATE      0xFF

// Forward declaration of the Machine structure.
typedef struct Machine machine_t;

// Function pointer type for state functions.
typedef void (*machineState)(machine_t*);

/**
 * @brief Structure representing a finite state machine.
 *
 * Contains pointers to current and next state functions, a timer for state transitions,
 * an event for execution, and the next event to process.
 */
typedef struct Machine
{
    machineState nextState;      ///< Pointer to the next state function.
    machineState currentState;   ///< Pointer to the current state function.
    task_t timer;                ///< Timer task for state transitions or timeouts.
    event_t executeEvent;        ///< Event to trigger state execution.
    uint8_t nextEvent;           ///< Next event to process.
} machine_t;

/**
 * @brief Structure representing a machine event.
 *
 * Used for posting events to the state machine.
 */
typedef struct MachineEvent
{
    machine_t *mPtr;             ///< Pointer to the associated machine.
    uint8_t event;               ///< Event code.
} machine_event_t;

/**
 * @brief Initializes the state machine.
 *
 * Sets up the machine structure for operation.
 *
 * @param m Pointer to the machine structure.
 */
void Machine_Init(machine_t *m);

/**
 * @brief Sets the state machine to a null state.
 *
 * Used to reset or disable the state machine.
 *
 * @param m Pointer to the machine structure.
 */
void Machine_NullState(machine_t *m);

/**
 * @brief Handles a timeout event for the state machine.
 *
 * Called when the state machine's timer expires.
 *
 * @param msg Pointer to the message or event data.
 */
void Machine_Timeout(void *msg);

/**
 * @brief Executes the state machine with the provided message.
 *
 * Processes the current event and transitions states as needed.
 *
 * @param msg Pointer to the message or event data.
 */
void Machine_Execute(void *msg);

/**
 * @brief Posts an event to the state machine.
 *
 * Queues an event for the state machine to process.
 *
 * @param m Pointer to the machine structure.
 * @param event Event code to post.
 */
void Machine_PostEvent(machine_t *m, uint8_t event);

/**
 * @brief Starts the state machine with the specified initial state.
 *
 * Sets the initial state and begins execution.
 *
 * @param m Pointer to the machine structure.
 * @param s Initial state function.
 */
void Machine_Start(machine_t *m, machineState s);

/**
 * @brief Starts the timer for the state machine.
 *
 * Begins timing for state transitions or timeouts.
 *
 * @param m Pointer to the machine structure.
 * @param interval Time interval for the timer.
 * @param loop Number of times to repeat (-1 for infinite).
 */
void Machine_StartTimer(machine_t *m, uint32_t interval, int32_t loop);

/**
 * @brief Stops the timer for the state machine.
 *
 * Cancels any ongoing timing for state transitions.
 *
 * @param m Pointer to the machine structure.
 */
void Machine_StopTimer(machine_t *m);

/**
 * @brief Checks if the state machine is in a specific state and received a specific event.
 *
 * Used for conditional state transitions.
 *
 * @param m Pointer to the machine structure.
 * @param input Event code to check.
 * @param state State function to check.
 * @return true if the machine is in the specified state and event, false otherwise.
 */
bool Machine_Check(machine_t *m, uint8_t input, machineState state);

#define MACHINE_DEF(name)\
    extern machine_t name##Machine;

#define MACHINE(name)\
    machine_t name##Machine = {										\
								.currentState = &Machine_NullState,	\
								.nextState = &Machine_NullState,	\
								.nextEvent = 0,						\
								.timer.handler = &Machine_Timeout	\
};	\

#define STATE_DEF(name) void name(machine_t* m);
#define STATE_BODY(name) void name(machine_t* m)

#define _TRANSITION_2(ev,st) if (Machine_Check(m, (uint8_t)ev, st))
#define _TRANSITION_1(ev) if ((uint8_t)ev == m->nextEvent)

#define _TRANSITION_NARGS2(_1, _2, N, ...) N
#define _TRANSITION_NARGS(...) _TRANSITION_NARGS2(__VA_ARGS__, 2, 1)
#define _TRANSITION_CHOOSER2(count) _TRANSITION_ ## count
#define _TRANSITION_CHOOSER(count) _TRANSITION_CHOOSER2(count)

#define TRANSITION_(...) _TRANSITION_CHOOSER(_TRANSITION_NARGS(__VA_ARGS__))(__VA_ARGS__)

#define _SM_TIMEOUT_START_3(name, interval, loop) Machine_StartTimer(&name, interval, loop)
#define _SM_TIMEOUT_START_2(name, interval) Machine_StartTimer(&name, interval, 0)
#define _SM_TIMEOUT_START_NARGS3(_1, _2, _3, N, ...) N
#define _SM_TIMEOUT_START_NARGS(...) _SM_TIMEOUT_START_NARGS3(__VA_ARGS__, 3, 2)

#define _SM_TIMEOUT_START_CHOOSER2(count) _SM_TIMEOUT_START_ ## count
#define _SM_TIMEOUT_START_CHOOSER(count) _SM_TIMEOUT_START_CHOOSER2(count)
#define SM_TIMEOUT_START(...) _SM_TIMEOUT_START_CHOOSER(_SM_TIMEOUT_START_NARGS(__VA_ARGS__))(__VA_ARGS__)

#define SM_TIMEOUT_STOP(name) Machine_StopTimer(&name)

#define ENTER_() if (m->nextEvent == ENTER_NEW_STATE)
#define EXIT_() if (m->nextEvent == EXIT_CURRENT_STATE)
#define TIMEOUT_() if (m->nextEvent == MACHINE_TIMEOUT_EVENT)
#define SM_SWITCH(state) m->nextState = &state
#define SM_START(name, state)\
    Machine_Init(&name);\
    Machine_Start(&name, (machineState)&state)
#define SM_POST(name, event) Machine_PostEvent(&name, (uint8_t)event)
#define SM_EXECUTE(name, event_)\
{machine_event_t e = {.mPtr=&name, .event = (uint8_t)event_};\
Machine_Execute(&e);}

#define SM_CHECK(name, state) (name.currentState == &state)
#endif /* CORE_MACHINE_H_ */