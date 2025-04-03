#include <console/log.h>
#include "node-c.h"

M_OBS_NODE(c_)

void Node_C_Init()
{
	M_OBS_NODE_INIT(c_, float, CALL_IMMEDIATLY);
}

M_OBS_NODE_HANDLER(c_)
{
	float* payload = (float*)data;
	LOG_PRINTF("Node C Received: %d", (int32_t)((*payload)*100));
}
