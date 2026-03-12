#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include <core/engine.h>
#include <core/event.h>
#include <core/timer.h>
#include <core/machine.h>
#include <core/signal.h>
#include <core/strand.h>
#include <hydra/controller.h>
#include <oscilloscope/quad.h>

#define N_SAMPLES 400
#define AMP       512.0f
#define OFFSET    512

typedef struct Fake
{
	double 	f0;
	float	f1;
	uint32_t f2;
	uint16_t f3;
	uint8_t f4;
}fake_t;

typedef struct StressData {
    uint32_t seq;
    uint32_t producer_id;
    uint32_t checksum;
} stress_data_t;

SIMPLE_MACHINE(ex, Test)
	M_TIMER(testLog)
	M_TIMER(plot)
	M_TIMER(oscilloscope)
	M_EVENT(empty)
	M_EVENT(fixedMany, fake_t, 3)
	M_EVENT(fixedMany_1, fake_t, 3)
	M_EVENT(stress, stress_data_t, 8)

	O_QUAD(quad, 0,1,2,3)

	M_SIGNAL(empty)
	M_SIGNAL(fixed, uint16_t)

	M_SIGNAL_MANY(emptyMany);
	M_SIGNAL_MANY(fixedMany, fake_t);

	M_EVENT(emptySignalReceived)
	M_EVENT(fixedSignalReceived, uint16_t)

	U_ACTION(100, start)
	U_ACTION(101, stop)
	U_ACTION(102, left)
	U_ACTION(103, right)
	U_INTEGER(104, integer)
	U_TEXT(105, name)

	M_EVENT(strandEmpty)
	M_EVENT(strandFixed, fake_t, 5)
	M_STRAND(command, 8)
	M_STRAND(stress, 32)

public:
	void init();

private:
    STATE_MAP_BEGIN
        IDLE,
        RUNNING,
    STATE_MAP_END

	STATE_DEF(Idle)
	STATE_DEF(Running)

	enum class Event : uint8_t
	{
		START,
		STOP
	};

private:
	fake_t fake_ = {.f0 = 100.123f,
						.f1 = 200.123f,
						.f2 = 300,
						.f3 = 400,
						.f4 = 10};
	uint32_t strandCount_;

	uint32_t angle_ = 0;
    uint16_t sine_[N_SAMPLES];
    uint16_t cosine_[N_SAMPLES];
    uint16_t triangle_[N_SAMPLES];
    uint16_t sawtooth_up_[N_SAMPLES];
    uint16_t sawtooth_down_[N_SAMPLES];
    uint16_t square_[N_SAMPLES];
    uint16_t half_sine_[N_SAMPLES];
    uint16_t chirp_[N_SAMPLES];
    char name_[32];

COMPONENT_END

#endif /* TEST_TEST_H_ */
