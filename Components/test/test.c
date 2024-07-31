#include "test.h"
#include <core/machine.h>
#include <core/observer.h>
#include <stdbool.h>
//#include <console/log.h>

// Declare M_TASK
M_TASK(timeout)

// Declare M_EVENT
M_EVENT(empty)	// Empty Event
M_EVENT(fixed1)	// Fixed Event
M_EVENT(fixed2) // Fixed Event

// Declare Finite State Machine
MACHINE(test)

// Declare Observer Subject
obs_subject_t encoderData;

//Declare Observer Node
obs_node_t obsEncoder;
M_EVENT(EncoderData)

void Test_Init()
{
	// Init for M_TASK
	M_TASK_INIT(timeout);

	// Init for M_EVENT
	M_EVENT_INIT(empty);
	M_EVENT_INIT(fixed1, sizeof(fake_data_t));
	M_EVENT_INIT(fixed2, sizeof(uint32_t));

	// Init Observer Subject
	Observer_InitSubject(&encoderData, sizeof(uint64_t));

	// Init Observer Node
	M_EVENT_INIT(EncoderData, sizeof(uint64_t));
	Observer_InitNode(&obsEncoder, &EncoderDataEvent, PUSH_TO_QUEUE);

	// Register Node with Subject
	bool ret = false;
	ret = Observer_AttachNode(&encoderData, &obsEncoder);
	if(!ret)
	{
		//TODO: LOG_PRINT("Register ObsEncoder Fail");
	}

	// Start M_TASK
	M_TASK_START(timeout, 1000);

	// Start State Machine
	SM_START(test, StartUp);
}

M_TASK_HANDLER(timeout)
{
	SM_POST(test, TIMEOUT);
	static uint64_t count = 0;
	count++;
	Observer_Notify(&encoderData, &count);
	M_EVENT_POST(empty);
}

M_EVENT_HANDLER(EncoderData)
{
	uint64_t* data_ = (uint64_t*)data;
	uint32_t val = (uint32_t)(*data_);
	//TODO: LOG_PRINTF("Encoder Data: %d", val);
}

M_EVENT_HANDLER(empty)
{
	static uint32_t count = 0;
	fake_data_t fake_;
	fake_.fake1 = count++;
	fake_.fake2 = fake_.fake1++;
	fake_.fake3 = fake_.fake2++;
	fake_.fake4 = sizeof(fake_);

	M_EVENT_POST(fixed1, fake_);
	M_EVENT_POST(fixed2, count);
}

M_EVENT_HANDLER(fixed1)
{
	fake_data_t* data_ = (fake_data_t*) data;
	/*TODO: LOG_PRINTF("Fixed1 Event: %d %d %d",
			(*data_).fake2,
			(*data_).fake3,
			(*data_).fake4);
	MC_PLOT(0, (*data_).fake1);*/
}

M_EVENT_HANDLER(fixed2)
{
	uint32_t* data_ = (uint32_t*) data;
	//TODO: LOG_PRINTF("Fixed2 Event: %d", *data_);
}
