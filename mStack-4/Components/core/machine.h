#ifndef CORE_MACHINE_H
#define CORE_MACHINE_H

#include <core/event.h>
#include <core/timer.h>

namespace core {

class Machine: public Component {
public:
    typedef void (Machine::*State)();

protected:
    void NullState() {}

    virtual void onTimeout_() {}

    virtual void start_(State s) {
        currentState_ = s;
        nextEvent_ = ENTER;
        (this->*currentState_)();
    }

    virtual bool check_(uint8_t event, State state) {
        if (this->nextEvent_ == event) {
            nextState_ = state;
            return true;
        }
        return false;
    }

protected:
    volatile State nextState_ = nullptr;
    volatile State currentState_ = &Machine::NullState;

    Timer timer_ = Timer(
        this,
        static_cast<Timer::Handler>(&Machine::onTimeout_)
    );

    volatile uint8_t nextEvent_ = 0U;

    const static uint8_t TIMEOUT = 0xFD;
    const static uint8_t ENTER   = 0xFE;
    const static uint8_t EXIT    = 0xFF;
};

class SimpleMachine: public Machine {
public:

    void postEvent(uint8_t event) {
        postEvent_.post(event);
    }

    void execute(uint8_t &event) {
        nextState_ = nullptr;
        this->nextEvent_ = event;
        (this->*currentState_)();
        if (nextState_ != nullptr && nextState_ != currentState_) {
            this->nextEvent_ = EXIT;
            (this->*currentState_)();
            currentState_ = nextState_;
            this->nextEvent_ = ENTER;
            (this->*currentState_)();
        }
    }

protected:
    void onTimeout_() override {
        postEvent(TIMEOUT);
    }

protected:
    ByteEvent postEvent_ =
        ByteEvent(this,(ByteEvent::Handler)&SimpleMachine::execute);
};

template<typename EventT, bool IsBig, size_t N>
struct EventHolder;

/* SMALL EVENT */
template<typename EventT, size_t N>
struct EventHolder<EventT, false, N> {
    using Type = SmallFixedEvent<EventT>;
    EventHolder(Component *owner,
                typename Type::Handler h)
        : obj(owner, h) {}
    void post(const EventT &e) {
        obj.post(e);
    }
    Type obj;
};

/* BIG EVENT */
template<typename EventT, size_t N>
struct EventHolder<EventT, true, N> {
    using Type = BigFixedEvent<EventT, N>;
    EventHolder(Component *owner,
                typename Type::Handler h)
        : obj(owner, h) {}

    void post(const EventT &e) {
        obj.post(e);
    }
    Type obj;
};

template<typename Payload, size_t N = 5>
class PayloadMachine: public Machine {
public:

    struct PM_Event {

        Payload payload{};
        uint8_t event{};
    };

    static constexpr bool IsBig =
        (sizeof(PM_Event) > sizeof(uint32_t));

    using Holder = EventHolder<PM_Event, IsBig, N>;
    using EventType = typename Holder::Type;

public:
    PayloadMachine()
        : payloadEvent_(
            this,
            (typename EventType::Handler)&PayloadMachine::execute
        ) {}

    void postEvent(uint8_t ev, Payload payload) {
        PM_Event temp{payload, ev};
        payloadEvent_.post(temp);
    }

    void execute(const PM_Event &ev) {
        nextState_ = nullptr;
        this->nextEvent_ = ev.event;
        ev_.payload = ev.payload;
        (this->*currentState_)();
        if (nextState_ != nullptr && nextState_ != currentState_) {
            this->nextEvent_ = EXIT;
            (this->*currentState_)();
            currentState_ = nextState_;
            this->nextEvent_ = ENTER;
            (this->*currentState_)();
        }
    }

protected:
    void onTimeout_() override {
        postEvent(TIMEOUT, {});
    }

protected:
    PM_Event ev_;
    Holder payloadEvent_;
};
}

/* MACROS */
#define SIMPLE_MACHINE(module, name, ...) \
namespace module { \
class name : public core::SimpleMachine, ##__VA_ARGS__ { \
    using CLASS = module::name; \
public: \
    static name& instance() { static name instance; return instance; } \
private: \
    virtual ~name() = default; \
    name() = default; \
    name(const name&) = delete; \
    name& operator=(const name&) = delete;

#define _PAYLOAD_MACHINE_3(module, name, type, ...) \
namespace module { \
class name : public core::PayloadMachine<type>, ##__VA_ARGS__ { \
    using CLASS = module::name; \
public: \
    static name& instance() { static name instance; return instance; } \
private: \
    virtual ~name() = default; \
    name() = default; \
    name(const name&) = delete; \
    name& operator=(const name&) = delete;

#define _PAYLOAD_MACHINE_4(module, name, type, numOfBlock, ...) \
namespace module { \
class name : public core::PayloadMachine<type, numOfBlock>, ##__VA_ARGS__ { \
    using CLASS = module::name; \
public: \
    static name& instance() { static name instance; return instance; } \
private: \
    virtual ~name() = default; \
    name() = default; \
    name(const name&) = delete; \
    name& operator=(const name&) = delete;

#define PAYLOAD_MACHINE(...) \
_M_MACRO_4(__VA_ARGS__, _PAYLOAD_MACHINE_4, _PAYLOAD_MACHINE_3)(__VA_ARGS__)

#define STATE_DEF(name) void name();
#define STATE_BODY(name) void name()

#define _TRANSITION_1(ev) \
if (check_((uint8_t)ev, currentState_))

#define _TRANSITION_2(ev,st) \
if (check_((uint8_t)ev, (core::Machine::State)&CLASS::st))

#define TRANSITION_(...) \
_M_MACRO_2(__VA_ARGS__, _TRANSITION_2, _TRANSITION_1)(__VA_ARGS__)

#define _SM_POST_SIMPLE(event) \
this->postEvent((uint8_t)event)

#define _SM_POST_PAYLOAD(ev, payload) \
this->postEvent((uint8_t)ev, payload)

#define SM_POST(...) \
_M_MACRO_2(__VA_ARGS__, _SM_POST_PAYLOAD, _SM_POST_SIMPLE)(__VA_ARGS__)


#define _SM_EXECUTE_SIMPLE(ev) \
{uint8_t e = (uint8_t)ev; this->execute(e);}

#define _SM_EXECUTE_PAYLOAD(ev, pay) \
{ pm_event_t e = {.payload = pay, .event = (uint8_t)ev}; this->execute(e); }

#define SM_EXECUTE(...) \
_M_MACRO_2(__VA_ARGS__, _SM_EXECUTE_PAYLOAD, _SM_EXECUTE_SIMPLE)(__VA_ARGS__)

#define ENTER_() if (nextEvent_ == ENTER)
#define EXIT_() if (nextEvent_ == EXIT)
#define TIMEOUT_() if (nextEvent_ == TIMEOUT)

#define SM_SWITCH(state) \
nextState_ = (core::Machine::State)&CLASS::state

#define SM_START(state) \
this->start_((core::Machine::State)&CLASS::state)

#define SM_CHECK(state) \
(currentState_ == (core::Machine::State)&CLASS::state)

#define MACHINE_END };}

#endif
