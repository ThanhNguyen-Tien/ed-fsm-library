#include "test.h"
#include "math.h"
#include "hydra/log.h"
#include "tim.h"
#include <telematry/telemetry.h>
using namespace ex;
using namespace core;

void Test::init()
{
	for (int i = 0; i < N_SAMPLES; i++)
	{
	    float phase = (2.0f * M_PI * i) / N_SAMPLES;

	    sine_[i]   = (int16_t)(sinf(phase) * AMP) + OFFSET;
	    cosine_[i]= (int16_t)(cosf(phase) * AMP) + OFFSET;
	}

	for (int i = 0; i < N_SAMPLES; i++)
	{
	    float t = (float)i / N_SAMPLES;   // 0 → 1
	    float tri;

	    if (t < 0.25f)
	        tri = 4.0f * t;
	    else if (t < 0.75f)
	        tri = 2.0f - 4.0f * t;
	    else
	        tri = -4.0f + 4.0f * t;

	    triangle_[i] = (int16_t)(tri * AMP) + OFFSET;
	}

	for (int i = 0; i < N_SAMPLES; i++)
	{
	    float t = (float)i / N_SAMPLES; // 0 → 1
	    float saw = 2.0f * t - 1.0f;    // -1 → +1

	    sawtooth_up_[i] = (int16_t)(saw * AMP) + OFFSET;
	}

	for (int i = 0; i < N_SAMPLES; i++)
	{
	    float t = (float)i / N_SAMPLES;
	    float saw = 1.0f - 2.0f * t;

	    sawtooth_down_[i] = (int16_t)(saw * AMP) + OFFSET;
	}

	for (int i = 0; i < N_SAMPLES; i++)
	{
	    float t = (float)i / N_SAMPLES;

	    square_[i] = (t < 0.5f) ? (OFFSET + AMP) : (OFFSET - AMP);
	}

	for (int i = 0; i < N_SAMPLES; i++)
	{
	    float phase = (2.0f * M_PI * i) / N_SAMPLES;
	    float s = sinf(phase);

	    if (s < 0) s = 0;

	    half_sine_[i] = (int16_t)(s * AMP) + OFFSET;
	}

	for (int i = 0; i < N_SAMPLES; i++)
	{
	    float t = (float)i / N_SAMPLES;
	    float phase = 2.0f * M_PI * t * t * 5.0f; // freq tăng dần

	    chirp_[i] = (int16_t)(sinf(phase) * AMP) + OFFSET;
	}

	MX_TIM3_Init();
	MX_TIM4_Init();

	LL_TIM_EnableIT_UPDATE(TIM3);
	LL_TIM_EnableIT_UPDATE(TIM4);

    plotTimer_.start(10); //100Hz
    oscilloscopeTimer_.start(1);
    testLogTimer_.start(1000);

    emptySignal.connect(&emptySignalReceivedEvent);
    fixedSignal.connect(&fixedSignalReceivedEvent);
    fixedManySignal.connect(&fixedManyEvent);
    fixedManySignal.connect(&fixedMany_1Event);

//
    INIT_STATE(IDLE, Idle);
    INIT_STATE(RUNNING, Running);
    SM_START(IDLE);

    LOG_DEBUG_PRINT("Hello Thanh Neymar");
}

M_EVENT_HANDLER(Test, strandEmpty)
{
	LOG_INFO_PRINT("STRAND Empty");
	commandStrand.done();
}

M_EVENT_HANDLER(::Test, strandFixed, fake_t)
{
	LOG_INFO_PRINTF("STRAND Fixed %u %u %u", event.f2, event.f3, event.f4);
	commandStrand.done();
}

M_TIMER_HANDLER(Test, testLog)
{
	fixedManyEvent.setPriority(2);
	emptyEvent.setPriority(fake_.f4 % 3);
	emptyEvent.post();
	fixedManyEvent.post(fake_);
	emptySignal.emit();
	fixedSignal.emit(cosine_[angle_]);
	fixedManySignal.emit(fake_);
	fake_.f0 += 1.0;
	fake_.f1 += 1.0;
	fake_.f2 += 10;
	fake_.f3 += 10;
	fake_.f4 += 1;
	commandStrand.post(&strandEmptyEvent);
	commandStrand.delay(500);
	commandStrand.post<fake_t>(&strandFixedEvent, fake_);
	commandStrand.delay(200);
	commandStrand.delay(100);
}

M_TIMER_HANDLER(Test, plot)
{
	MC_PLOT(4, triangle_[angle_]);
	MC_PLOT(5, half_sine_[angle_]);
	MC_PLOT(6, chirp_[angle_]);
	MC_PLOT(7, square_[angle_]);
	if (++angle_>=400) angle_=0;
}

M_TIMER_HANDLER(Test, oscilloscope)
{
	static uint16_t angle;
//    singlePlot(sine_[angle] + rand()%10);
//    singlePlot(fake_.f3);
//    dualPlot(sine_[angle_] + rand()%10, cosine_[angle_] + rand()%10);
    //triplePlot(sine_[angle] + rand()%10, cosine_[angle] + rand()%10, cosine_[angle]/2 + rand()%5);
//    quadPlot(sine_[angle] + rand()%10, cosine_[angle] + rand()%10, cosine_[angle]/2 + rand()%5, sine_[angle]/2 + rand()%5);
    quadPlot(sine_[angle] + rand()%10, cosine_[angle] + rand()%10, sawtooth_up_[angle] + rand()%5, sawtooth_down_[angle] + rand()%5);
    if (++angle>=400) angle=0;
}

M_EVENT_HANDLER(Test, empty)
{
//	LOG_INFO_PRINT("INFO");
//	LOG_WARNING_PRINT("WARNING");
//	LOG_ERROR_PRINT("ERROR");
//	LOG_CRITICAL_PRINT("CRITICAL");
	LOG_CRITICAL_PRINT("Empty Event");
}

M_EVENT_HANDLER(Test, fixedMany, struct Fake)
{
	LOG_CRITICAL_PRINTF("FixedMany %u %u %u", event.f2, event.f3, (uint32_t)(core::Engine::instance().getCpuLoad()));
}

M_EVENT_HANDLER(::Test, fixedMany_1, struct Fake)
{
//	LOG_DEBUG_PRINTF("FixedMany_1 %u %u %u", event.f2, event.f3, event.f4);
}

M_EVENT_HANDLER(Test, emptySignalReceived)
{
	LOG_INFO_PRINT("Empty Signal Received");
}

M_EVENT_HANDLER(Test, fixedSignalReceived, uint16_t)
{
	LOG_INFO_PRINTF("Fixed Signal Received %u", event);
}

U_ACTION_HANDLER(Test, start)
{
	LOG_DEBUG_PRINT("START");
	SM_POST(Event::START);
}

U_ACTION_HANDLER(Test, stop)
{
	LOG_INFO_PRINT("STOP");
	SM_POST(Event::STOP);
}

U_ACTION_HANDLER(Test, left)
{
	LOG_WARNING_PRINT("LEFT");
}

U_ACTION_HANDLER(Test, right)
{
	LOG_ERROR_PRINT("RIGHT");
}

U_INTEGER_HANDLER(Test, integer)
{
	LOG_INFO_PRINTF("Integer: %d", value);
}

U_TEXT_HANDLER(Test, name)
{
	int i = 0;
	for(; i < length; i++)
	{
		name_[i] = data[i];
	}
	name_[i] = 0;
	LOG_INFO_PRINTF("%s", name_);
}

M_EVENT_HANDLER(Test, stress, stress_data_t)
{
    // Verify checksum to detect if LDREX/STREX in MemPool is working correctly
    if (event.checksum != (event.seq ^ event.producer_id)) {
        // Error detected, log telemetry with producer_id for debugging
        Telemetry::log(TelemetryType::CHECKSUM_ERR, (uint16_t)event.producer_id);
    }

    // Simulate some processing delay to increase chance of contention and stress test the system
    for(volatile int i=0; i<1200; i++);

    // Unlock Strand slot after processing
    stressStrand.done();
}

extern "C" void TIM3_IRQHandler(void)
{
	core::Engine::instance().isrEnter();
	if(LL_TIM_IsActiveFlag_UPDATE(TIM3))
	{
		LL_TIM_ClearFlag_UPDATE(TIM3);
	    static uint32_t seqA = 0;
	    stress_data_t d = { seqA, 0xAABB, seqA ^ 0xAABB };

	    if (!Test::instance().stressEvent.post(d)) {
	    	LOG_ERROR_PRINT("TIM3 - Failed to post stress event to event queue");
	    }
	    seqA++;
	}
}

extern "C" void TIM4_IRQHandler(void)
{
	core::Engine::instance().isrEnter();
	if(LL_TIM_IsActiveFlag_UPDATE(TIM4))
	{
		LL_TIM_ClearFlag_UPDATE(TIM4);
	    static uint32_t seqB = 0;
	    stress_data_t d = { seqB, 0xCCDD, seqB ^ 0xCCDD };

	    if (!Test::instance().stressEvent.post(d)) {
	    	LOG_ERROR_PRINT("TIM4 - Failed to post stress event to event queue");
	    }
	    seqB++;
	}
}
