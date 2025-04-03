#ifndef TEST_EVENT_TASK_EVENT_TASK_H_
#define TEST_EVENT_TASK_EVENT_TASK_H_

#include <core/engine.h>
#include <console/controller.h>

M_TASK_DEF(timeout)
M_EVENT_DEF(sendPressed)
M_EVENT_DEF(fake)

U_INTEGER_DEF(interval)
U_INTEGER_DEF(repeat)
U_ACTION_DEF(startTask)
U_ACTION_DEF(stopTask)
U_ACTION_DEF(startForever)
U_ACTION_DEF(send)
U_ACTION_DEF(sync)

typedef struct FakeData
{
	int32_t fake1;
	int16_t fake2;
	int8_t fake3;
	int8_t fake4;
}fake_data_t;

void Test_Init();

#endif /* TEST_EVENT_TASK_EVENT_TASK_H_ */
