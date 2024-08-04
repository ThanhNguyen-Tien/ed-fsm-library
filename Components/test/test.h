#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include <stdint.h>
#include <core/engine.h>
#include <core/machine.h>
#include <console/controller.h>

M_TASK_DEF(timeout)

U_ACTION_DEF(sayHello)
U_ACTION_DEF(start)
U_ACTION_DEF(stop)
U_TEXT_DEF(name)
U_INTEGER_DEF(data)

void Test_Init();

typedef struct FakeData
{
	int32_t fake1;
	int16_t fake2;
	int8_t fake3;
	int8_t fake4;
}fake_data_t;

MACHINE_DEF(test)
STATE_DEF(StartUp)
STATE_DEF(Running)
STATE_DEF(Stop)

enum TestEvent {
	START,
	TIMEOUT,
	STOP,
};

#endif /* TEST_TEST_H_ */
