#ifndef CORE_OBSERVER_H_
#define CORE_OBSERVER_H_

#include "event.h"

/**
 * @brief Enumeration for observer handler types.
 *
 * Specifies how an observer node handles events: either by pushing to a queue or calling immediately.
 */
typedef enum
{
    PUSH_TO_QUEUE = 0,    ///< Handler pushes event to queue.
    CALL_IMMEDIATLY       ///< Handler calls event immediately.
} obs_handler_type_t;

// Forward declaration of ObserverNode structure.
typedef struct ObserverNode obs_node_t;

/**
 * @brief Structure representing an observer subject.
 *
 * Manages a list of observer nodes and the size of data passed to observers.
 */
typedef struct ObserverSubject
{
    obs_node_t *head;        ///< Pointer to the head of the observer node list.
    uint8_t sizeOfData;      ///< Size of the data passed to observers.
} obs_subject_t;

/**
 * @brief Structure representing an observer node.
 *
 * Contains event and handler type information, and links to other nodes and the registered subject.
 */
typedef struct ObserverNode
{
    event_t *ev;                     ///< Pointer to the event associated with the node.
    obs_handler_type_t type;         ///< Type of handler (queue or immediate).
    struct ObserverNode *next;       ///< Pointer to the next observer node in the list.
    struct ObserverSubject *registeredSubject; ///< Pointer to the subject this node is registered to.
} obs_node_t;

/**
 * @brief Initializes an observer subject with the specified data size.
 *
 * Sets up the subject to manage observer nodes and defines the size of data passed to observers.
 *
 * @param sub Pointer to the observer subject structure.
 * @param sizeOfData Size of the data passed to observers.
 */
void Observer_InitSubject(obs_subject_t *sub, uint8_t sizeOfData);

/**
 * @brief Initializes an observer node with the specified event and handler type.
 *
 * Sets up the observer node to handle events in the specified manner.
 *
 * @param obs Pointer to the observer node structure.
 * @param ev Pointer to the event associated with the node.
 * @param type Handler type (queue or immediate).
 */
void Observer_InitNode(obs_node_t *obs, event_t *ev, obs_handler_type_t type);

/**
 * @brief Attaches an observer node to a subject.
 *
 * Links the observer node to the subject's list of nodes.
 *
 * @param sub Pointer to the observer subject structure.
 * @param node Pointer to the observer node to attach.
 * @return true if successful, false otherwise.
 */
bool Observer_AttachNode(obs_subject_t *sub, obs_node_t *node);

/**
 * @brief Detaches an observer node from a subject.
 *
 * Removes the observer node from the subject's list of nodes.
 *
 * @param sub Pointer to the observer subject structure.
 * @param node Pointer to the observer node to detach.
 * @return true if successful, false otherwise.
 */
bool Observer_DetachNode(obs_subject_t *sub, obs_node_t *node);

/**
 * @brief Notifies all observer nodes attached to the subject with the provided data.
 *
 * Sends the specified data to all nodes registered to the subject.
 *
 * @param sub Pointer to the observer subject structure.
 * @param data Pointer to the data to notify observers with.
 */
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
