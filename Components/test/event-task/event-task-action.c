#include "event-task.h"
#include <console/log.h>

int32_t interval = 1;
int32_t repeat = 1;

U_ACTION_HANDLER(sync)
{
	ConsoleCtrl_UpdateIntField(100, interval);
	ConsoleCtrl_UpdateIntField(101, repeat);
}

U_INTEGER_HANDLER(interval)
{
	LOG_PRINTF("Interval: %d", value);
	interval = value;
}

U_INTEGER_HANDLER(repeat)
{
	LOG_PRINTF("Repeat: %d", value);
	repeat = value;
}

U_ACTION_HANDLER(startTask)
{
	LOG_PRINT("Start Timeout Task With Interval");
	M_TASK_START(timeout, interval, repeat);
}

U_ACTION_HANDLER(stopTask)
{
	LOG_PRINT("Stop Timeout Task");
	M_TASK_STOP(timeout);
}

U_ACTION_HANDLER(startForever)
{
	LOG_PRINT("Start Timeout Task Forever");
	M_TASK_START(timeout, interval);
}

U_ACTION_HANDLER(send)
{
	M_EVENT_POST(sendPressed);
}
