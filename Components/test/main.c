#include <core/engine.h>
#include "test.h"

int main()
{
	systemInit();
	
	ENGINE_INIT;

	Test_Init();

	Engine_Run();
	return 0;
}
