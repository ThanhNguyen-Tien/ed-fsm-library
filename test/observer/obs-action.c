#include "obs.h"
#include <console/log.h>

U_ACTION_HANDLER(nodeA_attach_subA)
{
	bool ret = Observer_AttachNode(&a_Subject, &a_Node);
	if(ret) LOG_PRINT("Successful Node A - Sub A");
	else LOG_PRINT("Fail to Attach Node A - Sub A");
}

U_ACTION_HANDLER(nodeA_detach_subA)
{
	bool ret = Observer_DetachNode(&a_Subject, &a_Node);
	if(ret) LOG_PRINT("Detach Done Node A - Sub A");
	else LOG_PRINT("Node A not exist in Sub A");
}

U_ACTION_HANDLER(nodeA_attach_subB)
{
	bool ret = Observer_AttachNode(&b_Subject, &a_Node);
	if(ret) LOG_PRINT("Successful Node A - Sub B");
	else LOG_PRINT("Fail to Attach Node A - Sub B");
}

U_ACTION_HANDLER(nodeA_detach_subB)
{
	bool ret = Observer_DetachNode(&b_Subject, &a_Node);
	if(ret) LOG_PRINT("Detach Done Node A - Sub B");
	else LOG_PRINT("Node A not exist in Sub B");
}

U_ACTION_HANDLER(nodeA_attach_subC)
{
	bool ret = Observer_AttachNode(&c_Subject, &a_Node);
	if(ret) LOG_PRINT("Successful Node A - Sub C");
	else LOG_PRINT("Fail to Attach Node A - Sub C");
}

U_ACTION_HANDLER(nodeA_detach_subC)
{
	bool ret = Observer_DetachNode(&c_Subject, &a_Node);
	if(ret) LOG_PRINT("Detach Done Node A - Sub C");
	else LOG_PRINT("Node A not exist in Sub C");
}

U_ACTION_HANDLER(nodeB_attach_subA)
{
	bool ret = Observer_AttachNode(&a_Subject, &b_Node);
	if(ret) LOG_PRINT("Successful Node B - Sub A");
	else LOG_PRINT("Fail to Attach Node B - Sub A");
}

U_ACTION_HANDLER(nodeB_detach_subA)
{
	bool ret = Observer_DetachNode(&a_Subject, &b_Node);
	if(ret) LOG_PRINT("Detach Done Node B - Sub A");
	else LOG_PRINT("Node B not exist in Sub A");
}

U_ACTION_HANDLER(nodeB_attach_subB)
{
	bool ret = Observer_AttachNode(&b_Subject, &b_Node);
	if(ret) LOG_PRINT("Successful Node B - Sub B");
	else LOG_PRINT("Fail to Attach Node B - Sub B");
}

U_ACTION_HANDLER(nodeB_detach_subB)
{
	bool ret = Observer_DetachNode(&b_Subject, &b_Node);
	if(ret) LOG_PRINT("Detach Done Node B - Sub B");
	else LOG_PRINT("Node B not exist in Sub B");
}

U_ACTION_HANDLER(nodeB_attach_subC)
{
	bool ret = Observer_AttachNode(&c_Subject, &b_Node);
	if(ret) LOG_PRINT("Successful Node B - Sub C");
	else LOG_PRINT("Fail to Attach Node B - Sub C");
}

U_ACTION_HANDLER(nodeB_detach_subC)
{
	bool ret = Observer_DetachNode(&c_Subject, &b_Node);
	if(ret) LOG_PRINT("Detach Done Node B - Sub C");
	else LOG_PRINT("Node B not exist in Sub C");
}

U_ACTION_HANDLER(nodeC_attach_subA)
{
	bool ret = Observer_AttachNode(&a_Subject, &c_Node);
	if(ret) LOG_PRINT("Successful Node C - Sub A");
	else LOG_PRINT("Fail to Attach Node C - Sub A");
}

U_ACTION_HANDLER(nodeC_detach_subA)
{
	bool ret = Observer_DetachNode(&a_Subject, &c_Node);
	if(ret) LOG_PRINT("Detach Done Node C - Sub A");
	else LOG_PRINT("Node C not exist in Sub A");
}

U_ACTION_HANDLER(nodeC_attach_subB)
{
	bool ret = Observer_AttachNode(&b_Subject, &c_Node);
	if(ret) LOG_PRINT("Successful Node C - Sub B");
	else LOG_PRINT("Fail to Attach Node C - Sub B");
}

U_ACTION_HANDLER(nodeC_detach_subB)
{
	bool ret = Observer_DetachNode(&b_Subject, &c_Node);
	if(ret) LOG_PRINT("Detach Done Node C - Sub B");
	else LOG_PRINT("Node C not exist in Sub B");
}

U_ACTION_HANDLER(nodeC_attach_subC)
{
	bool ret = Observer_AttachNode(&c_Subject, &c_Node);
	if(ret) LOG_PRINT("Successful Node C - Sub C");
	else LOG_PRINT("Fail to Attach Node C - Sub C");
}

U_ACTION_HANDLER(nodeC_detach_subC)
{
	bool ret = Observer_DetachNode(&c_Subject, &c_Node);
	if(ret) LOG_PRINT("Detach Done Node C - Sub C");
	else LOG_PRINT("Node C not exist in Sub C");
}
