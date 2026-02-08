#include "test.h"
#include "math.h"
#include "hydra/log.h"
using namespace ex;

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

    plotTimer_.start(5); //100Hz
    oscilloscopeTimer_.start(1);
    testLogTimer_.start(1000);

    emptySignal.connect(&emptySignalReceivedEvent);
    fixedSignal.connect(&fixedSignalReceivedEvent);
//
//    fixedManySignal.connect(&fixedManyEvent);
//    fixedManySignal.connect(&fixedMany_1Event);
//
//    SM_START(StartUp);
//    LOG_DEBUG_PRINT("Hello");
}

M_EVENT_HANDLER(Test, strandEmpty)
{
	LOG_INFO_PRINT("STRAND Empty");
}

M_EVENT_HANDLER(::Test, strandFixed, fake_t)
{
//	LOG_PRINTF("STRAND Fixed: %d", event);
	LOG_INFO_PRINTF("STRAND Big Fixed %u %u %u", event.f2, event.f3, event.f4);
}

M_TIMER_HANDLER(Test, testLog)
{
	emptyEvent.post();
	fixedManyEvent.post(fake_);
	fake_.f0 += 1.0;
	fake_.f1 += 1.0;
	fake_.f2 += 10;
	fake_.f3 += 10;
	fake_.f4 += 10;
}

M_TIMER_HANDLER(Test, plot)
{
//	emptySignal.emit();
//	fixedSignal.emit(cosine_[angle_]);
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
}

M_EVENT_HANDLER(Test, fixedMany, struct Fake)
{
	LOG_DEBUG_PRINTF("FixedMany %u %u %u", event.f2, event.f3, (uint32_t)(core::Engine::instance().getCpuLoad()));
}

M_EVENT_HANDLER(::Test, fixedMany_1, struct Fake)
{
	LOG_DEBUG_PRINTF("FixedMany_1 %u %u %u", event.f2, event.f3, event.f4);
}

M_EVENT_HANDLER(Test, emptySignalReceived)
{
	LOG_INFO_PRINT("Empty Signal Received");
}

M_EVENT_HANDLER(Test, fixedSignalReceived, uint16_t)
{
	LOG_INFO_PRINT("Fixed Signal Received");
}

U_ACTION_HANDLER(Test, start)
{
	LOG_DEBUG_PRINT("START");
	LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_5);
}

U_ACTION_HANDLER(Test, stop)
{
	LOG_INFO_PRINT("STOP");
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
