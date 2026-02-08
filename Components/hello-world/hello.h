#ifndef HELLO_WORLD_TEST_H
#define HELLO_WORLD_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <core/engine.h>
#include <core/task.h>
#include <hydra/controller.h>

M_TASK_DEF(blink)
U_ACTION_DEF(start)
U_ACTION_DEF(stop)

void hello_Init();

#ifdef __cplusplus
}
#endif

#endif /* HELLO_WORLD_TEST_H */
