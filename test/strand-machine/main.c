#include <core/engine.h>
#include <test/strand-machine/controller.h>

int main()
{
	// Init Clock, GPIO,...
	System_Init();

	// Init Core Engine
	ENGINE_INIT;

	//Init Components
	Lcd_Ctrl_Init();

	Engine_Run();
	return 0;
}
