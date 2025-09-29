#include "test.h"
#include <console/log.h>

void test::FileTransfer::init()
{
	COMPONENT_REG(console, Controller);
	SM_START(IDLE);
}

STATE_BODY(test::FileTransfer::IDLE)
{
	ENTER_()
	{
		LOG_PRINT("[SM] IDLE");
		return;
	}

	TRANSITION_(Event::START, RECEIVING)
	{
		LOG_PRINTF("[SM] START FILE: %s, size: %u", ev_.payload.filename, ev_.payload.totalSize);
		return;
	}
}

STATE_BODY(test::FileTransfer::RECEIVING)
{
	ENTER_()
	{
		LOG_PRINT("[SM] RECEIVING");
		return;
	}

	TRANSITION_(Event::NEXT_BLOCK)
	{
		LOG_PRINTF("SM] NEXT BLOCK SIZE: %u", ev_.payload.blockSize);
		return;
	}

	TRANSITION_(Event::FINISH, IDLE)
	{
		LOG_PRINTF("[SM] FINISH FILE: %s", ev_.payload.filename);
		return;
	}
}
