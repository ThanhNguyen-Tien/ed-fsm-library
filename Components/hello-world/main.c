#include <core/engine.h>
#include <hydra/controller.h>
#include <hydra/log.h>
#include "hello.h"

int main()
{
	// Init Clock, GPIO,...
	System_Init();

	// Init Core Engine
	ENGINE_INIT;

	//Init Components
	HydraCtrl_Init();
	hello_Init();

	Engine_Run();
	return 0;
}
