#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include <stdint.h>
#include <core/engine.h>
#include <core/machine.h>

M_TASK_DEF(timeout)

void Test_Init();

typedef struct FakeData
{
	uint32_t fake1;
	uint16_t fake2;
	uint8_t fake3;
	uint8_t fake4;
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
