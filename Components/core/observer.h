#ifndef CORE_OBSERVER_H_
#define CORE_OBSERVER_H_

#include "event.h"

typedef enum
{
	PUSH_TO_QUEUE = 0, CALL_IMMEDIATLY
} obs_handler_type_t;

typedef struct ObserverNode obs_node_t;
typedef struct ObserverSubject
{
	obs_node_t *head;
	uint8_t sizeOfData;
} obs_subject_t;

typedef struct ObserverNode
{
	event_t *ev;
	obs_handler_type_t type;
	struct ObserverNode *next;
	struct ObserverSubject *registeredSubject;
} obs_node_t;

void Observer_InitSubject(obs_subject_t *sub, uint8_t sizeOfData);
void Observer_InitNode(obs_node_t *obs, event_t *ev, obs_handler_type_t type);
bool Observer_AttachNode(obs_subject_t *sub, obs_node_t *node);
bool Observer_DetachNode(obs_subject_t *sub, obs_node_t *node);
void Observer_Notify(obs_subject_t *sub, void *data);

#define M_OBS_SUBJECT_DEF(name)\
	extern obs_subject_t name##Subject;

#define M_OBS_SUBJECT(name)\
	obs_subject_t name##Subject;

#define M_OBS_SUBJECT_INIT(name, size)\
	Observer_InitSubject(&name, size);

#define M_OBS_NODE_DEF(name)\
	extern obs_node_t name##Node;\
	M_EVENT_DEF(name##Node)

#define M_OBS_NODE(name)\
	obs_node_t name##Node;\
	M_EVENT(name##Node)

#define M_OBS_NODE_INIT(name, size, type_enum)\
	_Static_assert((type_enum) == PUSH_TO_QUEUE || (type_enum) == CALL_IMMEDIATLY, "Invalid obs_handler_type_t value"); \
	M_EVENT_INIT(name##Event, size);\
	Observer_InitNode(&name, &name##Event, (obs_handler_type_t)(type_enum));

#define M_OBS_NODE_HANDLER(name)	void name##NodeEventHandler_(void *data)

#endif /* CORE_OBSERVER_H_ */
