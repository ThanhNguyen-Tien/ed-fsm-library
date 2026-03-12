#include "test.h"
#include <hydra/log.h>
#include <telematry/telemetry.h>
#include "tim.h"
using namespace ex;
using namespace core;

STATE_BODY(Test::Idle)
{
	ENTER_()
	{
		LOG_INFO_PRINTF("IDLE - %s", __func__);
		LL_TIM_DisableCounter(TIM3);
		LL_TIM_DisableCounter(TIM4);
		timer_.stop();
		core::Engine::instance().events().resetEventsMeasurements();
	}
	TRANSITION_(Event::START, Running) {}
}

STATE_BODY(Test::Running)
{
	ENTER_()
	{
		LOG_INFO_PRINTF("RUNNING - %s", __func__);
		LL_TIM_EnableCounter(TIM3);
		LL_TIM_EnableCounter(TIM4);
		timer_.start(5);
	}
	TRANSITION_(Event::STOP, Idle) {}
	TIMEOUT_()
	{
		LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_5);
	    static uint32_t seqA = 0;
	    stress_data_t d = { seqA, 0x5555, seqA ^ 0x5555 };

	    if (!Test::instance().stressStrand.post<stress_data_t>(&Test::instance().stressEvent, d)) {
	        LOG_ERROR_PRINT("Failed to post stress event to strand");
	    }

	    if (!stressEvent.post(d)) {
	    	LOG_ERROR_PRINT("Failed to post stress event to event queue");
	    }

	    seqA++;
	}
}
