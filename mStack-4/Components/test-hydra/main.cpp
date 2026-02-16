#include "test.h"
#include <hydra/controller.h>

DEBUG_HOOK(core, Engine)
DEBUG_HOOK(ex, Test)

int main()
{
	SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;

	COMPONENT_REG(core, Engine);
	COMPONENT_REG(hydra, Controller);
	COMPONENT_REG(ex, Test);

    DEBUG_ATTACH(core, Engine);
    DEBUG_ATTACH(ex, Test);

	core::Engine::instance().run();
	return 0;
}


