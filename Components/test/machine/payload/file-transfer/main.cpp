#include "test.h"

DEBUG_HOOK(core, Engine)
DEBUG_HOOK(test, FileTransfer)

int main()
{
	COMPONENT_REG(core, Engine);
	COMPONENT_REG(test, FileTransfer);

    DEBUG_ATTACH(core, Engine);
    DEBUG_ATTACH(test, FileTransfer);

	core::Engine::instance().run();

	return 0;
}

