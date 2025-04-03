#ifndef CORE_OBSERVER_H_
#define CORE_OBSERVER_H_

#include "event.h"

/**
 * @brief Enumeration for observer handler types.
 */
typedef enum
{
    PUSH_TO_QUEUE = 0, ///< Push event data to the EventQueue.
    CALL_IMMEDIATELY    ///< Call the event handler immediately.
} obs_handler_type_t;

/**
 * @brief Structure representing an observer node.
 */
typedef struct ObserverNode
{
    event_t* ev;                        ///< Associated event.
    obs_handler_type_t type;             ///< Handler type.
    struct ObserverNode* next;           ///< Pointer to the next observer node.
    struct ObserverSubject* registeredSubject; ///< Pointer to the subject this node is registered to.
} obs_node_t;

/**
 * @brief Structure representing an observer subject.
 */
typedef struct ObserverSubject
{
    obs_node_t* head;   ///< Head of the observer node list.
    uint8_t sizeOfData; ///< Size of the event data.
} obs_subject_t;

/**
 * @brief Initializes an observer subject.
 * @param sub Pointer to the observer subject structure.
 * @param sizeOfData Size of the event data.
 */
void Observer_InitSubject(obs_subject_t* sub, uint8_t sizeOfData);

/**
 * @brief Initializes an observer node.
 * @param obs Pointer to the observer node structure.
 * @param ev Pointer to the associated event.
 * @param type Observer handler type.
 */
void Observer_InitNode(obs_node_t* obs, event_t* ev, obs_handler_type_t type);

/**
 * @brief Attaches an observer node to an observer subject.
 * @param sub Pointer to the observer subject.
 * @param node Pointer to the observer node.
 * @return True if successfully attached, false otherwise.
 */
bool Observer_AttachNode(obs_subject_t* sub, obs_node_t* node);

/**
 * @brief Detaches an observer node from an observer subject.
 * @param sub Pointer to the observer subject.
 * @param node Pointer to the observer node.
 * @return True if successfully detached, false otherwise.
 */
bool Observer_DetachNode(obs_subject_t* sub, obs_node_t* node);

/**
 * @brief Notifies all attached observer nodes with event data.
 * @param sub Pointer to the observer subject.
 * @param data Pointer to the event data.
 */
void Observer_Notify(obs_subject_t* sub, void* data);

#define M_OBS_SUBJECT_DEF(name)\
	extern obs_subject_t name##Subject;

#define M_OBS_SUBJECT(name)\
	obs_subject_t name##Subject;

#define M_OBS_SUBJECT_INIT(name, type)\
	Observer_InitSubject(&name##Subject, sizeof(type));

#define M_OBS_NODE_DEF(name)\
	extern obs_node_t name##Node;

#define M_OBS_NODE(name)\
	obs_node_t name##Node;\
	M_EVENT(name##Node)

#define M_OBS_NODE_INIT(name, type, type_enum)\
	_Static_assert((type_enum) == PUSH_TO_QUEUE || (type_enum) == CALL_IMMEDIATLY, "Invalid obs_handler_type_t value"); \
	M_EVENT_INIT(name##Node, sizeof(type));\
	Observer_InitNode(&name##Node, &name##NodeEvent, (obs_handler_type_t)(type_enum));

#define M_OBS_NODE_HANDLER(name)	void name##NodeEventHandler_(void *data)

#endif /* CORE_OBSERVER_H_ */
