#ifndef CORE_OBSERVER_H_
#define CORE_OBSERVER_H_

#include "event.h"

/**
 * @brief Enumeration of observer handler types.
 *
 * Defines how an observer node handles events: either by pushing them to a queue
 * or calling the handler immediately.
 */
typedef enum
{
    PUSH_TO_QUEUE = 0, /**< Push the event to a queue for later processing. */
    CALL_IMMEDIATLY    /**< Call the event handler immediately. */
} obs_handler_type_t;

/**
 * @brief Forward declaration of the ObserverNode structure.
 */
typedef struct ObserverNode obs_node_t;

/**
 * @brief Structure representing an observer subject.
 *
 * This structure manages a list of observer nodes and the size of the data
 * associated with the subject.
 */
typedef struct ObserverSubject
{
    obs_node_t* head;      /**< Pointer to the head of the observer node list. */
    uint8_t sizeOfData;    /**< Size of the data associated with the subject. */
} obs_subject_t;

/**
 * @brief Structure representing an observer node.
 *
 * This structure defines an observer node that can handle events and is linked
 * to a subject.
 */
typedef struct ObserverNode
{
    event_t* ev;                     /**< Pointer to the event associated with the node. */
    obs_handler_type_t type;         /**< Type of handler for the node (queue or immediate). */
    struct ObserverNode* next;       /**< Pointer to the next node in the list. */
    struct ObserverSubject* registeredSubject; /**< Pointer to the subject this node is registered with. */
} obs_node_t;

/**
 * @brief Initializes an observer subject.
 *
 * This function sets up an observer subject with the specified data size.
 *
 * @param sub Pointer to the observer subject to be initialized.
 * @param sizeOfData Size of the data associated with the subject.
 */
void Observer_InitSubject(obs_subject_t* sub, uint8_t sizeOfData);

/**
 * @brief Initializes an observer node.
 *
 * This function sets up an observer node with the specified event and handler type.
 *
 * @param obs Pointer to the observer node to be initialized.
 * @param ev Pointer to the event associated with the node.
 * @param type Type of handler for the node (queue or immediate).
 */
void Observer_InitNode(obs_node_t* obs, event_t* ev, obs_handler_type_t type);

/**
 * @brief Attaches an observer node to a subject.
 *
 * This function adds an observer node to the list of nodes managed by the subject.
 *
 * @param sub Pointer to the observer subject.
 * @param node Pointer to the observer node to be attached.
 * @return `true` if the node was successfully attached, `false` otherwise.
 */
bool Observer_AttachNode(obs_subject_t* sub, obs_node_t* node);

/**
 * @brief Detaches an observer node from a subject.
 *
 * This function removes an observer node from the list of nodes managed by the subject.
 *
 * @param sub Pointer to the observer subject.
 * @param node Pointer to the observer node to be detached.
 * @return `true` if the node was successfully detached, `false` otherwise.
 */
bool Observer_DetachNode(obs_subject_t* sub, obs_node_t* node);

/**
 * @brief Notifies all observer nodes of a subject.
 *
 * This function sends a notification to all observer nodes attached to the subject,
 * passing the specified data to their handlers.
 *
 * @param sub Pointer to the observer subject.
 * @param data Pointer to the data to be passed to the observer nodes.
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
