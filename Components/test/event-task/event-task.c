#include "event-task.h"
#include <console/log.h>

M_TASK(timeout)
M_EVENT(sendPressed)
M_EVENT(fake)

U_INTEGER(interval)
U_INTEGER(repeat)
U_ACTION(startTask)
U_ACTION(stopTask)
U_ACTION(startForever)
U_ACTION(send)
U_ACTION(sync)

void Test_Init()
{
	ConsoleCtrl_Init();

	// Register Console Commands
	CONSOLE_REGISTER_COMMAND(Console_SyncFields, sync);
	CONSOLE_REGISTER_COMMAND(100, interval);
	CONSOLE_REGISTER_COMMAND(101, repeat);
	CONSOLE_REGISTER_COMMAND(102, startTask);
	CONSOLE_REGISTER_COMMAND(103, stopTask);
	CONSOLE_REGISTER_COMMAND(104, startForever);
	CONSOLE_REGISTER_COMMAND(105, send);

	// Init for M_TASK
	M_TASK_INIT(timeout);

	// Init for M_EVENT
	M_EVENT_INIT(sendPressed);
	M_EVENT_INIT(fake, sizeof(fake_data_t));

	LOG_PRINT("TEST EVENTS - TASKS");
}

M_EVENT_HANDLER(sendPressed)
{
	LOG_PRINT("Button Send Pressed");
}

M_EVENT_HANDLER(fake)
{
	fake_data_t* payload = (fake_data_t*)data;
	LOG_PRINTF("Received Fake Event: %d %d %d %d",
			(*payload).fake1,
			(*payload).fake2,
			(*payload).fake3,
			(*payload).fake4);
}

M_TASK_HANDLER(timeout)
{
	static fake_data_t value = {0};
	value.fake2 = value.fake1 + 1;
	value.fake3 = value.fake2 + 1;
	value.fake4 = value.fake3 + 1;

	M_EVENT_POST(fake, value);
	value.fake1++;
}


