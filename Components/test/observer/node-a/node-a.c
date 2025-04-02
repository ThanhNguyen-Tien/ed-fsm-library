#include <console/log.h>
#include "node-a.h"

M_OBS_NODE(a_)

void Node_A_Init()
{
	M_OBS_NODE_INIT(a_, float, PUSH_TO_QUEUE);
}

M_OBS_NODE_HANDLER(a_)
{
	float* payload = (float*)data;
	LOG_PRINTF("Node A Received: %d", (int32_t)((*payload)*100));
}
