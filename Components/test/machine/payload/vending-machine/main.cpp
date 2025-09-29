#include "test.h"
#include <core/engine.h>
#include <core/machine.h>

DEBUG_HOOK(core, Engine)
DEBUG_HOOK(test, VendingMachine)

int main()
{
	COMPONENT_REG(core, Engine);
	COMPONENT_REG(test, VendingMachine);

    DEBUG_ATTACH(core, Engine);
    DEBUG_ATTACH(test, VendingMachine);

	core::Engine::instance().run();

	return 0;
}

