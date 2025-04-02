#ifndef TEST_OBSERVER_OBS_H_
#define TEST_OBSERVER_OBS_H_

#include <console/controller.h>
#include <core/engine.h>
#include <core/task.h>
#include "node-a/node-a.h"
#include "node-b/node-b.h"
#include "node-c/node-c.h"
#include "subject-a/sub-a.h"
#include "subject-b/sub-b.h"
#include "subject-c/sub-c.h"

U_ACTION_DEF(nodeA_attach_subA)
U_ACTION_DEF(nodeA_detach_subA)
U_ACTION_DEF(nodeA_attach_subB)
U_ACTION_DEF(nodeA_detach_subB)
U_ACTION_DEF(nodeA_attach_subC)
U_ACTION_DEF(nodeA_detach_subC)

U_ACTION_DEF(nodeB_attach_subA)
U_ACTION_DEF(nodeB_detach_subA)
U_ACTION_DEF(nodeB_attach_subB)
U_ACTION_DEF(nodeB_detach_subB)
U_ACTION_DEF(nodeB_attach_subC)
U_ACTION_DEF(nodeB_detach_subC)

U_ACTION_DEF(nodeC_attach_subA)
U_ACTION_DEF(nodeC_detach_subA)
U_ACTION_DEF(nodeC_attach_subB)
U_ACTION_DEF(nodeC_detach_subB)
U_ACTION_DEF(nodeC_attach_subC)
U_ACTION_DEF(nodeC_detach_subC)

M_TASK_DEF(subA_notify)
M_TASK_DEF(subB_notify)
M_TASK_DEF(subC_notify)

void Test_Init();

#endif /* TEST_OBSERVER_OBS_H_ */
