#ifndef TEST_HIERARCHICAL_MACHINE_TEST_HI_SM_H_
#define TEST_HIERARCHICAL_MACHINE_TEST_HI_SM_H_

#include <core/machine.h>
#include <console/controller.h>

PAYLOAD_MACHINE(test, VendingMachine, uint8_t)

	U_ACTION(100, insertCoin)
	U_ACTION(101, selectDrink)
	U_ACTION(102, dispenseDone)
	U_ACTION(103, cancel)

	U_INTEGER(104, selected)

public:
	void init();

private:
	enum class Event : uint8_t {
		INSERT_COIN, SELECT_DRINK, SELECTED, DISPENSE_DONE, CANCEL
	};

private:
	STATE_DEF(IDLE)
	STATE_DEF(HAS_MONEY)
	STATE_DEF(SELECTING)
	STATE_DEF(DISPENSING)
};
}

#endif /* TEST_HIERARCHICAL_MACHINE_TEST_HI_SM_H_ */
