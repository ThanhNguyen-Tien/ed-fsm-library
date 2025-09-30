#include "test.h"
#include <console/log.h>

void test::VendingMachine::init()
{
	COMPONENT_REG(console, Controller);
	SM_START(IDLE);
}

STATE_BODY(test::VendingMachine::IDLE)
{
	ENTER_()
	{
		LOG_PRINT("[SM] WAITING FOR COIN ...");
		return;
	}

	TRANSITION_(Event::INSERT_COIN, HAS_MONEY)
	{
		LOG_PRINT("[SM] COIN INSERTED");
		return;
	}
}

STATE_BODY(test::VendingMachine::HAS_MONEY)
{
	ENTER_()
	{
		LOG_PRINT("[SM] READY FOR SELECTION");
		return;
	}

	TRANSITION_(Event::CANCEL, IDLE)
	{
		LOG_PRINT("[SM] CANCEL -> RETURN TO IDLE");
		return;
	}

	TRANSITION_(Event::SELECT_DRINK, SELECTING)	{return;}
}

STATE_BODY(test::VendingMachine::SELECTING)
{
	ENTER_()
	{
		LOG_PRINT("[SM] USER IS SELECTING DRINK ...");
		return;
	}

	TRANSITION_(Event::CANCEL, IDLE)
	{
		LOG_PRINT("[SM] CANCEL -> RETURN TO IDLE");
		return;
	}

	TRANSITION_(Event::SELECTED, DISPENSING)
	{
		LOG_PRINTF("[SM] DRINK SELECTED at NUMBER: %d", ev_.payload);
		return;
	}
}

STATE_BODY(test::VendingMachine::DISPENSING)
{
	ENTER_()
	{
		LOG_PRINT("[SM] DISPENSING ...");
		timer_.start(2000, 1);
		return;
	}

	TIMEOUT_()
	{
		SM_POST(Event::DISPENSE_DONE, {});
		return;
	}

	TRANSITION_(Event::DISPENSE_DONE, IDLE) {return;}

	EXIT_()
	{
		LOG_PRINT("[SM] DISPENSE DONE");
	}
}
