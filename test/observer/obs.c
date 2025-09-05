#include "obs.h"
#include <console/log.h>

U_ACTION(nodeA_attach_subA)
U_ACTION(nodeA_detach_subA)
U_ACTION(nodeA_attach_subB)
U_ACTION(nodeA_detach_subB)
U_ACTION(nodeA_attach_subC)
U_ACTION(nodeA_detach_subC)

U_ACTION(nodeB_attach_subA)
U_ACTION(nodeB_detach_subA)
U_ACTION(nodeB_attach_subB)
U_ACTION(nodeB_detach_subB)
U_ACTION(nodeB_attach_subC)
U_ACTION(nodeB_detach_subC)

U_ACTION(nodeC_attach_subA)
U_ACTION(nodeC_detach_subA)
U_ACTION(nodeC_attach_subB)
U_ACTION(nodeC_detach_subB)
U_ACTION(nodeC_attach_subC)
U_ACTION(nodeC_detach_subC)

M_TASK(subA_notify)
M_TASK(subB_notify)
M_TASK(subC_notify)

void Test_Init()
{
	ConsoleCtrl_Init();

	CONSOLE_REGISTER_COMMAND(100, nodeA_attach_subA);
	CONSOLE_REGISTER_COMMAND(101, nodeA_detach_subA);
	CONSOLE_REGISTER_COMMAND(102, nodeA_attach_subB);
	CONSOLE_REGISTER_COMMAND(103, nodeA_detach_subB);
	CONSOLE_REGISTER_COMMAND(104, nodeA_attach_subC);
	CONSOLE_REGISTER_COMMAND(105, nodeA_detach_subC);

	CONSOLE_REGISTER_COMMAND(106, nodeB_attach_subA);
	CONSOLE_REGISTER_COMMAND(107, nodeB_detach_subA);
	CONSOLE_REGISTER_COMMAND(108, nodeB_attach_subB);
	CONSOLE_REGISTER_COMMAND(109, nodeB_detach_subB);
	CONSOLE_REGISTER_COMMAND(110, nodeB_attach_subC);
	CONSOLE_REGISTER_COMMAND(111, nodeB_detach_subC);

	CONSOLE_REGISTER_COMMAND(112, nodeC_attach_subA);
	CONSOLE_REGISTER_COMMAND(113, nodeC_detach_subA);
	CONSOLE_REGISTER_COMMAND(114, nodeC_attach_subB);
	CONSOLE_REGISTER_COMMAND(115, nodeC_detach_subB);
	CONSOLE_REGISTER_COMMAND(116, nodeC_attach_subC);
	CONSOLE_REGISTER_COMMAND(117, nodeC_detach_subC);

	Sub_A_Init();	// float
	Sub_B_Init();	// uint16_t
	Sub_C_Init();	// float

	Node_A_Init();	// float
	Node_B_Init();	// uint16_t
	Node_C_Init();	// float

	M_TASK_INIT(subA_notifyTask);
	M_TASK_INIT(subB_notifyTask);
	M_TASK_INIT(subC_notifyTask);

	M_TASK_START(subA_notifyTask, 2000);
	M_TASK_START(subB_notifyTask, 2000);
	M_TASK_START(subC_notifyTask, 2000);

	LOG_PRINT("TEST OBSERVERS");
}

M_TASK_HANDLER(subA_notify)
{
	static float fake = 10.0f;
	Observer_Notify(&a_Subject, &fake);
	fake+= 0.1f;
	if(fake > 10.5f)fake = 10.0f;
}

M_TASK_HANDLER(subB_notify)
{
	static uint16_t fake = 50;
	Observer_Notify(&b_Subject, &fake);
	fake+= 1;
	if(fake > 55)fake = 50;
}

M_TASK_HANDLER(subC_notify)
{
	static float fake = 100.0f;
	Observer_Notify(&c_Subject, &fake);
	fake+= 0.1f;
	if(fake > 100.5f)fake = 100.0f;
}
