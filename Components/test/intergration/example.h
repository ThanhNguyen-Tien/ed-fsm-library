#ifndef TEST_EXAMPLE_EXAMPLE_H_
#define TEST_EXAMPLE_EXAMPLE_H_

#include <core/engine.h>
#include <core/event.h>
#include <core/timer.h>
#include <core/machine.h>
#include <core/signal.h>
#include <core/strand.h>
#include <console/controller.h>
//#include <oscilloscope/quad.h>
#include <oscilloscope/single.h>

typedef struct Fake
{
	double 	f0;
	float	f1;
	uint32_t f2;
	uint16_t f3;
	uint8_t f4;
}fake_t;

SIMPLE_MACHINE(ex, Test)

	M_TIMER(plot)
	M_TIMER(oscilloscope)
	M_EVENT(empty)
	M_EVENT(fixedMany, fake_t, 3)
	M_EVENT(fixedMany_1, fake_t, 3)

//	O_QUAD(quad, 2,3,4,5)
	O_SINGLE(single, 2)

	M_SIGNAL(empty)
	M_SIGNAL(fixed, uint16_t)

	M_SIGNAL_MANY(emptyMany);
	M_SIGNAL_MANY(fixedMany, fake_t);

	M_EVENT(emptySignalReceived)
	M_EVENT(fixedSignalReceived, uint16_t)

	U_ACTION(101, start)
	U_ACTION(102, stop)
	U_INTEGER(100, integer)
	U_TEXT(103, name)

	M_EVENT(strandEmpty)
	M_EVENT(strandFixed, fake_t, 5)
	M_STRAND(command, 64)

public:
	void init();

private:
	enum class Event{TEST = 0};

private:
	STATE_DEF(StartUp)
	STATE_DEF(Running)
	STATE_DEF(Pause)

private:
	fake_t fake_ = {.f0 = 100.123f,
						.f1 = 200.123f,
						.f2 = 300,
						.f3 = 400,
						.f4 = 10};
	uint32_t strandCount_;

	uint32_t angle_ = 0;
    uint16_t sine_[400];
    uint16_t cosine_[400];
    char name_[32];
    friend struct __Test_DebugHelper;

MACHINE_END

#endif /* TEST_EXAMPLE_EXAMPLE_H_ */
