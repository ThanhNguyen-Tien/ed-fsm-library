#ifndef CORE_TASK_H_
#define CORE_TASK_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct Task task_t;
typedef void (*TaskHandler)();

typedef struct Task
{
	task_t* next ;
	uint32_t interval;
	uint64_t nextTick;
	int32_t loop;
	TaskHandler handler;
}task_t;

void Task_Run();

void Task_Start(task_t* task, uint32_t interval, int32_t loop);
void Task_Stop(task_t* task);
bool Task_IsRunning(task_t* task);

#define M_TASK_DEF(name)\
	extern task_t name##Task;

#define M_TASK(name)\
	task_t name##Task;\
	void name##TaskHandler##_();

#define M_TASK_INIT(name)\
	Engine_RegisterTask(&name##Task);\
	name##Task.handler = &name##TaskHandler_;

#define _M_TASK_START_3(name, interval, loop)	Task_Start(&name##Task, interval, loop)
#define _M_TASK_START_2(name, interval)	Task_Start(&name##Task, interval, 0)

#define _TASK_NARGS3(_1, _2, _3, N, ...) N
#define _TASK_NARGS(...) _TASK_NARGS3(__VA_ARGS__, 3, 2)
#define _TASK_CHOOSER2(count) _M_TASK_START_ ## count
#define _TASK_CHOOSER(count) _TASK_CHOOSER2(count)

#define M_TASK_START(...) _TASK_CHOOSER(_TASK_NARGS(__VA_ARGS__))(__VA_ARGS__)

#define M_TASK_STOP(name)					Task_Stop(&name##Task)
#define M_TASK_HANDLER(name)				void name##TaskHandler##_()


#endif /* CORE_TASK_H_ */
