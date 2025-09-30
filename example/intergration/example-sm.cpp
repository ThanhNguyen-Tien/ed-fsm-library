#include <console/log.h>
#include "example.h"

STATE_BODY(ex::Test::StartUp)
{
	ENTER_()
	{
		LOG_PRINTF("[SM] START UP");
		timer_.start(2000);
		commandStrand.done();
		return;
	}

	TIMEOUT_()
	{
		SM_SWITCH(Running);
		return;
	}

	EXIT_()
	{
//		LOG_PRINTF("EXIT START UP");
	}
}

STATE_BODY(ex::Test::Running)
{
	static uint8_t count = 0;
	ENTER_()
	{
		LOG_PRINTF("[SM] RUNNING");
		SM_POST(Event::TEST);
		return;
	}

	TRANSITION_(Event::TEST)
	{
		fixedManySignal.emit(fake_);
		commandStrand.post(&strandEmptyEvent);
		commandStrand.post<fake_t>(&strandFixedEvent, fake_);
		return;
	}

	TIMEOUT_()
	{
		if(count == 0)
		{
			SM_POST(Event::TEST);
			count = 1;
		}
		else
		{
			SM_SWITCH(Pause);
			count = 0;
		}
		return;
	}

	EXIT_()
	{
//		LOG_PRINTF("EXIT RUNNING");
	}
}

STATE_BODY(ex::Test::Pause)
{
	ENTER_()
	{
		LOG_PRINTF("[SM] PAUSE");
		return;
	}

	TIMEOUT_()
	{
		SM_SWITCH(StartUp);
		return;
	}

	EXIT_()
	{
//		LOG_PRINTF("EXIT PAUSE");
	}
}
