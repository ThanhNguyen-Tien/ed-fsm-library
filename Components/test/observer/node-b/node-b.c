#include <console/log.h>
#include "node-b.h"

M_OBS_NODE(b_)

void Node_B_Init()
{
	M_OBS_NODE_INIT(b_, uint16_t, PUSH_TO_QUEUE);
}

M_OBS_NODE_HANDLER(b_)
{
	uint16_t* payload = (uint16_t*)data;
	LOG_PRINTF("Node B Received: %d", (*payload));
}
