#include "test.h"
#include <hydra/log.h>
using namespace ex;

void Test::init()
{
	blinkTimer_.start(1000);
	emptyEvent.setPriority(3);
	fixedEvent.setPriority(1);
	fixedLargeEvent.setPriority(5);
	fixedSmallEvent.setPriority(4);
}

M_TIMER_HANDLER(Test, blink)
{
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	emptyEvent.post();
	fixedEvent.post(count_++);
	fixedLargeEvent.post(payload_);
	fixedSmallEvent.post(payloadSmall_);
}

M_EVENT_HANDLER(Test, empty)
{
	LOG_INFO_PRINT("Empty Event");
}

M_EVENT_HANDLER(Test, fixed, uint32_t)
{
	LOG_WARNING_PRINTF("Fixed Event: %u", event);
}

M_EVENT_HANDLER(Test, fixedLarge, test_payload_t)
{
	LOG_CRITICAL_PRINTF("Fixed Large Event: %u, %u, %u, %f, %f", event.value0, event.value1, event.value2, event.value3, event.value4);
}

M_EVENT_HANDLER(Test, fixedSmall, test_payload_small_t)
{
	LOG_ERROR_PRINTF("Fixed Small Event: %u, %u, %u", event.value0, event.value1, event.value2);
}
