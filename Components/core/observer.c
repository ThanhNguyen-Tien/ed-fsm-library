#include "observer.h"
#include "system.h"

void Observer_InitSubject(obs_subject_t *sub, uint8_t sizeOfData)
{
	ASSERT(sub != NULL);

	sub->head = NULL;
	sub->sizeOfData = sizeOfData;
}

void Observer_InitNode(obs_node_t *obs, event_t *ev, obs_handler_type_t type)
{
	ASSERT(obs != NULL);
	ASSERT(ev != NULL);

	obs->ev = ev;
	obs->type = type;
	obs->next = NULL;
}

bool Observer_AttachNode(obs_subject_t *sub, obs_node_t *node)
{
	ASSERT(sub != NULL);
	ASSERT(node != NULL);
	ASSERT(node->ev != NULL);

	if (node->registeredSubject == sub || node->ev->size != sub->sizeOfData)
		return false;
	if (node->registeredSubject != NULL)
	{
		Observer_DetachNode(node->registeredSubject, node);
	}

	node->registeredSubject = sub;
	node->next = sub->head;
	sub->head = node;
	return true;
}

bool Observer_DetachNode(obs_subject_t *sub, obs_node_t *node)
{
	ASSERT(sub != NULL);
	ASSERT(node != NULL);

	obs_node_t **current = &(sub->head);

	while (*current != NULL)
	{
		if (*current == node)
		{
			*current = node->next;
			node->registeredSubject = NULL;
			return true;
		}
		current = &((*current)->next);
	}
	return false;
}

void Observer_Notify(obs_subject_t *sub, void *data)
{
	ASSERT(sub != NULL);

	obs_node_t *current = sub->head;

	while (current != NULL)
	{
		if (current->ev->handler != NULL)
		{
			if (current->type == PUSH_TO_QUEUE)
			{
				Event_Post(current->ev->index, data);
			} else if (current->type == CALL_IMMEDIATLY)
			{
				current->ev->handler(data);
			}
		}
		current = current->next;
	}
}
