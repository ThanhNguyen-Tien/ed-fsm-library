#include "test.h"
#include <console/log.h>

U_ACTION_HANDLER(test::FileTransfer, start)
{
	LOG_PRINT("[ACTION] START");
	SM_POST(Event::START, fake_);
	for(size_t i = 0; i < numOfBlock_; i++)
	{
		SM_POST(Event::NEXT_BLOCK, fake_);	// Simulation
	}
	SM_POST(Event::FINISH, fake_);
}

U_INTEGER_HANDLER(test::FileTransfer, blockSize)
{
	fake_.blockSize = value;
	fake_.totalSize = fake_.blockSize * numOfBlock_;
	LOG_PRINT("[ACTION]");
	LOG_PRINTF("BLOCK SIZE: %d\nNUM OF BLOCK: %d\n=> TOTAL SIZE: %d",fake_.blockSize, numOfBlock_, fake_.totalSize);
}

U_INTEGER_HANDLER(test::FileTransfer, numOfBlock)
{
	LOG_PRINTF("[ACTION] NUM OF BLOCK: %d", value);
	numOfBlock_ = value;
}

U_TEXT_HANDLER(test::FileTransfer, filename)
{
	memset(fake_.filename, '\0', MAX_LEN);
	if(length > MAX_LEN) Error_Handler();
	for(size_t i = 0; i < length; i++)
	{
		fake_.filename[i] = (char)data[i];
	}

	LOG_PRINTF("[ACTION] FILE NAME: %s", fake_.filename);
}

