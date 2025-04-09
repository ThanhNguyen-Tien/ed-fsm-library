#ifndef CORE_MACHINE_H_
#define CORE_MACHINE_H_

#include "event.h"
#include "task.h"

#define MACHINE_TIMEOUT_EVENT	0xFD
#define ENTER_NEW_STATE			0xFE
#define EXIT_CURRENT_STATE      0xFF

typedef struct Machine machine_t;
typedef void (*machineState)(machine_t*);

typedef struct Machine
{
	machineState nextState;
	machineState currentState;

	task_t timer;
	event_t executeEvent;

	uint8_t nextEvent;
}machine_t;

typedef struct MachineEvent
{
	machine_t* mPtr;
	uint8_t event;
}machine_event_t;

void Machine_Init(machine_t *m);
void Machine_Execute(void* msg);

void Machine_PostEvent(machine_t* m, uint8_t event);
void Machine_Start(machine_t* m, machineState s);
void Machine_StartTimer(machine_t* m, uint32_t interval, int32_t loop);
void Machine_StopTimer(machine_t* m);
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
