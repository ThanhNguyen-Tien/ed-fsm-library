#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include <core/engine.h>
#include <core/event.h>
#include <core/timer.h>

typedef struct {
	uint8_t value0;
	uint16_t value1;
	uint32_t value2;
	float value3;
	double value4;
} test_payload_t;

typedef struct {
	uint8_t value0;
	uint8_t value1;
	uint8_t value2;
	uint8_t padding; // Padding to make the struct size a multiple of 4 bytes
} test_payload_small_t;

COMPONENT(ex, Test)

	M_TIMER(blink)

	M_EVENT(empty)
	M_EVENT(fixed, uint32_t)
	M_EVENT(fixedLarge, test_payload_t)
	M_EVENT(fixedSmall, test_payload_small_t)

public:
	void init() override;

private:
	uint16_t count_ = 0;
	test_payload_t payload_ = {1, 2, 3, 4.5f, 6.7};
	test_payload_small_t payloadSmall_ = {1, 2, 3};

COMPONENT_END

#endif /* TEST_TEST_H_ */
