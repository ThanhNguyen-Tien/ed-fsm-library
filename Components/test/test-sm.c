#include <test/test.h>
#include <console/log.h>

STATE_BODY(StartUp)
{
	ENTER()
	{
		LOG_PRINT("[SM] StartUp");
		M_TASK_START(timeout, 1000, 1);
	}
	TRANSITION_(TIMEOUT)
	{
		SM_POST(test, START);
	}
	TRANSITION_(START, Running){}
}

STATE_BODY(Running)
{
	ENTER()
	{
		LOG_PRINT("[SM] Running");
		M_TASK_START(timeout, 1000, 5);
	}
	TRANSITION_(TIMEOUT)
	{
		static uint8_t count = 0;
		if(++count == 5)
		{
			SM_SWITCH(Stop);
			count = 0;
		}
	}
	TRANSITION_(STOP, Stop){}
}

STATE_BODY(Stop)
{
	ENTER()
	{
		LOG_PRINT("[SM] Stop");
		M_TASK_START(timeout, 2000, 1);
	}
	TRANSITION_(TIMEOUT, StartUp){}
}
