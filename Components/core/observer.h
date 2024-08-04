#ifndef CORE_OBSERVER_H_
#define CORE_OBSERVER_H_

#include "event.h"

typedef enum
{
	NONE = 0,
	PUSH_TO_QUEUE,
	CALL_IMMEDIATLY
}obs_handler_type_t;

typedef struct ObserverNode
{
	event_t* ev;
	obs_handler_type_t type;
	struct ObserverNode* next;
}obs_node_t;

typedef struct ObserverSubject
{
	obs_node_t* head;
	uint8_t sizeOfData;
}obs_subject_t;

void Observer_InitSubject(obs_subject_t* sub, uint8_t sizeOfData);
void Observer_InitNode(obs_node_t* obs, event_t* ev, obs_handler_type_t type);
bool Observer_AttachNode(obs_subject_t* sub, obs_node_t* node);
void Observer_DetachNode(obs_subject_t* sub, obs_node_t* node);
void Observer_Notify(obs_subject_t* sub, void* data);

#endif /* CORE_OBSERVER_H_ */
