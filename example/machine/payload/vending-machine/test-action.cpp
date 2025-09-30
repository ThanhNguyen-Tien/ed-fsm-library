#include "test.h"
#include <console/log.h>

U_ACTION_HANDLER(test::VendingMachine, insertCoin)
{
	LOG_PRINT("[ACTION]: INSERT COIN");
	SM_POST(Event::INSERT_COIN, {});
}

U_ACTION_HANDLER(test::VendingMachine, selectDrink)
{
	LOG_PRINT("[ACTION]: SELECT DRINK");
	SM_POST(Event::SELECT_DRINK, {});
}

U_ACTION_HANDLER(test::VendingMachine, dispenseDone)
{
	LOG_PRINT("[ACTION]: DISPENSE DONE");
	SM_POST(Event::DISPENSE_DONE, {});
}

U_ACTION_HANDLER(test::VendingMachine, cancel)
{
	LOG_PRINT("[ACTION]: CANCEL");
	SM_POST(Event::CANCEL, {});
}

U_INTEGER_HANDLER(test::VendingMachine, selected)
{
	LOG_PRINTF("[ACTION] SELECT NUMBER %d", value);
	SM_POST(Event::SELECTED, value);
}

