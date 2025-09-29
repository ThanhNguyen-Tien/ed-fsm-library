#include <console/log.h>
#include "gpio.h"
#include <core/machine.h>
#include <math.h>
#include "example.h"
#define PI 3.14159265358979323846

using namespace ex;

void Test::init()
{
    for (int i=0;i<400;i++)
    {
        double v = ((double)i/200) * PI;
        int16_t sine = (int16_t)(sin(v)*512.0);
        int16_t cosine = (int16_t)(cos(v)*512.0);
        sine_[i] = sine + 512;
        cosine_[i] = cosine + 512;
    }

    plotTimer_.start(1); //100Hz
    oscilloscopeTimer_.start(1);

    emptySignal.connect(&emptySignalReceivedEvent);
    fixedSignal.connect(&fixedSignalReceivedEvent);

    fixedManySignal.connect(&fixedManyEvent);
    fixedManySignal.connect(&fixedMany_1Event);

    SM_START(StartUp);
}

M_EVENT_HANDLER(Test, strandEmpty)
{
	LOG_PRINT("STRAND Empty");
}

M_EVENT_HANDLER(::Test, strandFixed, fake_t)
{
//	LOG_PRINTF("STRAND Fixed: %d", event);
	LOG_PRINTF("STRAND Big Fixed %u %u %u", event.f2, event.f3, event.f4);
}

M_TIMER_HANDLER(Test, plot)
{

	emptySignal.emit();
	fixedSignal.emit(cosine_[angle_]);

	if (++angle_>=400) angle_=0;
}

M_TIMER_HANDLER(Test, oscilloscope)
{
	static uint16_t angle;
    singlePlot(sine_[angle] + rand()%10);
//    dualPlot(sine_[angle_] + rand()%10, cosine_[angle_] + rand()%10);
    //triplePlot(sine_[angle] + rand()%10, cosine_[angle] + rand()%10, cosine_[angle]/2 + rand()%5);
//    quadPlot(sine_[angle] + rand()%10, cosine_[angle] + rand()%10, cosine_[angle]/2 + rand()%5, sine_[angle]/2 + rand()%5);
    if (++angle>=400) angle=0;
}

M_EVENT_HANDLER(Test, empty)
{
	LOG_PRINTF("EmptyEvent");
}

M_EVENT_HANDLER(Test, fixedMany, struct Fake)
{
	LOG_PRINTF("FixedMany %u %u %u", event.f2, event.f3, event.f4);
}

M_EVENT_HANDLER(::Test, fixedMany_1, struct Fake)
{
	LOG_PRINTF("FixedMany_1 %u %u %u", event.f2, event.f3, event.f4);
}

M_EVENT_HANDLER(Test, emptySignalReceived)
{
    console::Controller::instance().plot(0, sine_[angle_]);
}

M_EVENT_HANDLER(Test, fixedSignalReceived, uint16_t)
{
	console::Controller::instance().plot(1, event);
}

U_ACTION_HANDLER(Test, start)
{
	LOG_PRINTF("START");
	LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_5);
}

U_ACTION_HANDLER(Test, stop)
{
	LOG_PRINTF("STOP");
}

U_INTEGER_HANDLER(Test, integer)
{
	LOG_PRINTF("Integer: %d", value);
}

U_TEXT_HANDLER(Test, name)
{
	int i = 0;
	for(; i < length; i++)
	{
		name_[i] = data[i];
	}
	name_[i] = 0;
	LOG_PRINTF("%s", name_);
}
