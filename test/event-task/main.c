#include <core/engine.h>
#include "event-task.h"

int main()
{
	// Init Clock, GPIO,...
	System_Init();

	// Init Core Engine
	ENGINE_INIT;

	//Init Components
	Test_Init();

	Engine_Run();
	return 0;
}
