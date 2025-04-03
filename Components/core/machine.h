#ifndef CORE_MACHINE_H_
#define CORE_MACHINE_H_

#include "event.h"

#define ENTER_NEW_STATE         0xFE
#define EXIT_CURRENT_STATE      0xFF

typedef struct Machine machine_t;
typedef void (*machineState)(machine_t*);

/**
 * @brief Structure representing a state machine.
 */
typedef struct Machine
{
    machineState nextState;   ///< Pointer to the next state function.
    machineState currentState; ///< Pointer to the current state function.
    event_t executeEvent;      ///< Event associated with state execution.
    uint8_t nextEvent;         ///< Next event identifier.
} machine_t;

/**
 * @brief Structure representing a machine event.
 */
typedef struct MachineEvent
{
    machine_t* mPtr; ///< Pointer to the associated state machine.
    uint8_t event;   ///< Event identifier.
} machine_event_t;

/**
 * @brief Initializes a state machine.
 * @param m Pointer to the machine structure.
 */
void Machine_Init(machine_t *m);

/**
 * @brief Executes the state machine based on the posted event.
 * @param msg Pointer to the machine event message.
 */
void Machine_Execute(void* msg);

/**
 * @brief Posts an event to a specific state machine.
 * @param m Pointer to the machine structure.
 * @param event Event identifier.
 */
void Machine_PostEvent(machine_t* m, uint8_t event);

/**
 * @brief Starts the state machine from the specified initial state.
 * @param m Pointer to the machine structure.
 * @param s Initial state function pointer.
 */
void Machine_Start(machine_t* m, machineState s);

/**
 * @brief Checks if the given event matches the expected transition condition.
 * @param m Pointer to the machine structure.
 * @param input Event identifier.
 * @param state State function pointer to compare.
 * @return True if the event matches, false otherwise.
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

#define ENTER() if (m->nextEvent == ENTER_NEW_STATE)
#define EXIT() if (m->nextEvent == EXIT_CURRENT_STATE)
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
