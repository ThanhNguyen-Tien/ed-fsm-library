#include "engine.h"
#include "machine.h"

static void Machine_NullState(machine_t *m)
{
	UNUSED(m);
}

inline void Machine_Execute(void *msg)
{
	ASSERT(msg != NULL);

	machine_event_t *data = (machine_event_t*) msg;
	data->mPtr->nextState = NULL;
	data->mPtr->nextEvent = data->event;
		(*data->mPtr->currentState)(data->mPtr);

	if (data->mPtr->nextState != NULL
			&& data->mPtr->nextState != data->mPtr->currentState)
	{
		data->mPtr->nextEvent = EXIT_CURRENT_STATE;
		(*data->mPtr->currentState)(data->mPtr);
		data->mPtr->currentState = data->mPtr->nextState;
		data->mPtr->nextEvent = ENTER_NEW_STATE;
		(*data->mPtr->currentState)(data->mPtr);
	}
}

static void Machine_Timeout(void *msg)
{
	ASSERT(msg != NULL);

	machine_t *m = (machine_t*) msg;
	Machine_PostEvent(m, (uint8_t) MACHINE_TIMEOUT_EVENT);
}

void Machine_Init(machine_t *m)
{
	ASSERT(m != NULL);

	m->nextState = &Machine_NullState;
	m->currentState = &Machine_NullState;
	m->nextEvent = 0;

	Engine_RegisterTask(&m->timer);
	m->timer.handler = &Machine_Timeout;
	Event_Init(&m->executeEvent, sizeof(machine_event_t),
			(void*) &Machine_Execute);
}

void Machine_PostEvent(machine_t *m, uint8_t event)
{
	ASSERT(m != NULL);

	machine_event_t machineEvent;
	machineEvent.mPtr = m;
	machineEvent.event = event;
	Event_Post(m->executeEvent.index, &machineEvent);
}

void Machine_Start(machine_t *m, machineState s)
{
	ASSERT(m != NULL);

	m->currentState = s;
	m->nextEvent = ENTER_NEW_STATE;
	(*m->currentState)(m);
}

bool Machine_Check(machine_t *m, uint8_t input, machineState state)
{
	ASSERT(m != NULL);

	if (m->nextEvent != input)
		return false;
	m->nextState = state;
	return true;
}

void Machine_StartTimer(machine_t *m, uint32_t interval, int32_t loop)
{
	ASSERT(m != NULL);

	Task_Start(&m->timer, interval, loop, m);
}

void Machine_StopTimer(machine_t *m)
{
	ASSERT(m != NULL);

	Task_Stop(&m->timer);
}

void Machine_Reset(machine_t *m)
{
	m->currentState = &Machine_NullState;
	m->nextState = NULL;
}
