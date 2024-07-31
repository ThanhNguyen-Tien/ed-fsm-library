#include <test/test.h>
#include <console/log.h>

U_ACTION_HANDLER(start)
{
	LOG_PRINT("Start");
}

U_ACTION_HANDLER(stop)
{
	LOG_PRINT("Stop");
	SM_EXECUTE(test, STOP);
}

U_TEXT_HANDLER(hello)
{
	LOG_PRINT("Name");
}

