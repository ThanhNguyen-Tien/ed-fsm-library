#ifndef CORE_MACHINE_H
#define CORE_MACHINE_H

#include <core/event.h>
#include <core/timer.h>
#include <cstdint>

namespace core
{
    class Machine : public Component
    {
    public:
        using StateHandler = void (Machine::*)();

    protected:
        Machine() = default;
        void NullState() {}
        virtual void onTimeout_() {}

        inline void callState_(uint8_t &s)
        {
            if (this->stateTable_ && s < this->maxStates_ && this->stateTable_[s] != nullptr)
            {
                (this->*stateTable_[s])();
            }
            else
            {
                this->NullState();
                // Optional: Log error or assert for invalid state
            }
        }

        void start_(uint8_t state)
        {
            __atomic_store_n(&this->currentState_, state, __ATOMIC_RELEASE);
            __atomic_store_n(&this->nextEvent_, ENTER, __ATOMIC_RELEASE);

            uint8_t s = __atomic_load_n(&this->currentState_, __ATOMIC_ACQUIRE);
            this->callState_(s);
        }

        bool check_(uint8_t event, uint8_t state)
        {
            uint8_t ev = __atomic_load_n(&this->nextEvent_, __ATOMIC_ACQUIRE);
            if (ev == event)
            {
                __atomic_store_n(&this->nextState_, state, __ATOMIC_RELEASE);
                return true;
            }
            return false;
        }

    protected:
        const StateHandler *stateTable_ = nullptr;
        uint8_t maxStates_ = 0;

        volatile uint8_t currentState_ = 0;
        volatile uint8_t nextState_ = 0xFF;
        volatile uint8_t nextEvent_ = 0;

        Timer timer_ = Timer(this, static_cast<Timer::Handler>(&Machine::onTimeout_));

        static constexpr uint8_t TIMEOUT = 0xFD;
        static constexpr uint8_t ENTER = 0xFE;
        static constexpr uint8_t EXIT = 0xFF;
    };

    class SimpleMachine : public Machine
    {
    public:
        void postEvent(uint8_t event) { this->postEvent_.post(event); }

        void execute(uint8_t &event)
        {
            __atomic_store_n(&this->nextState_, 0xFF, __ATOMIC_RELEASE);
            __atomic_store_n(&this->nextEvent_, event, __ATOMIC_RELEASE);

            uint8_t state = __atomic_load_n(&this->currentState_, __ATOMIC_ACQUIRE);
            this->callState_(state);

            uint8_t target = __atomic_load_n(&this->nextState_, __ATOMIC_ACQUIRE);
            if (target != 0xFF && target != state)
            {
                __atomic_store_n(&this->nextEvent_, EXIT, __ATOMIC_RELEASE);
                this->callState_(state);

                __atomic_store_n(&this->currentState_, target, __ATOMIC_RELEASE);
                __atomic_store_n(&this->nextEvent_, ENTER, __ATOMIC_RELEASE);
                this->callState_(target);
            }
        }

    protected:
        void onTimeout_() override { this->postEvent(TIMEOUT); }
        ByteEvent postEvent_ = ByteEvent(this, (ByteEvent::Handler)&SimpleMachine::execute);
    };

    template <typename EventT, bool IsBig, size_t N>
    struct EventHolder;

    /* SMALL EVENT */
    template <typename EventT, size_t N>
    struct EventHolder<EventT, false, N>
    {
        using Type = SmallFixedEvent<EventT>;
        EventHolder(Component *owner,
                    typename Type::Handler h)
            : obj(owner, h) {}

        void post(const EventT &e)
        {
            obj.post(e);
        }
        Type obj;
    };

    /* BIG EVENT */
    template <typename EventT, size_t N>
    struct EventHolder<EventT, true, N>
    {
        using Type = BigFixedEvent<EventT, N>;
        EventHolder(Component *owner,
                    typename Type::Handler h)
            : obj(owner, h) {}

        void post(const EventT &e)
        {
            obj.post(e);
        }
        Type obj;
    };

    template <typename Payload, size_t N = 5>
    class PayloadMachine : public Machine
    {
    public:
        struct PM_Event
        {
            Payload payload{};
            uint8_t event{};
        };
        static constexpr bool IsBig = (sizeof(PM_Event) > sizeof(uint32_t));
        using Holder = EventHolder<PM_Event, IsBig, N>;
        using EventType = typename Holder::Type;

    public:
        PayloadMachine()
            : payloadEvent_(this, (typename EventType::Handler) & PayloadMachine::execute) {}

        void postEvent(uint8_t ev, Payload payload)
        {
            PM_Event temp{payload, ev};
            this->payloadEvent_.post(temp);
        }

        void execute(const PM_Event &ev)
        {
            __atomic_store_n(&this->nextState_, 0xFF, __ATOMIC_RELEASE);
            __atomic_store_n(&this->nextEvent_, ev.event, __ATOMIC_RELEASE);

            this->ev_.payload = ev.payload;

            uint8_t state = __atomic_load_n(&this->currentState_, __ATOMIC_ACQUIRE);
            this->callState_(state);

            uint8_t target = __atomic_load_n(&this->nextState_, __ATOMIC_ACQUIRE);
            if (target != 0xFF && target != state)
            {
                __atomic_store_n(&this->nextEvent_, EXIT, __ATOMIC_RELEASE);
                this->callState_(state);

                __atomic_store_n(&this->currentState_, target, __ATOMIC_RELEASE);
                __atomic_store_n(&this->nextEvent_, ENTER, __ATOMIC_RELEASE);
                this->callState_(target);
            }
        }

    protected:
        void onTimeout_() override { this->postEvent(TIMEOUT, {}); }
        PM_Event ev_;
        Holder payloadEvent_;
    };
}

/* ===================== STATE GENERATOR ===================== */
#define STATE_MAP_BEGIN    \
public:                    \
    enum StateID : uint8_t \
    {

#define STATE_MAP_END \
    ST_MAX            \
    }                 \
    ;                 \
                      \
private:              \
    core::Machine::StateHandler _table[ST_MAX] = {nullptr};

#define INIT_STATE(id, func) \
    _table[id] = static_cast<core::Machine::StateHandler>(&CLASS::func)

/* ========================= MACROS ========================= */

#define SIMPLE_MACHINE(module, name, ...)                      \
    namespace module                                           \
    {                                                          \
        class name : public core::SimpleMachine, ##__VA_ARGS__ \
        {                                                      \
            using CLASS = module::name;                        \
                                                               \
        public:                                                \
            static name &instance()                            \
            {                                                  \
                static name instance;                          \
                return instance;                               \
            }                                                  \
                                                               \
        private:                                               \
            virtual ~name() = default;                         \
            name() = default;                                  \
            name(const name &) = delete;                       \
            name &operator=(const name &) = delete;

#define _PAYLOAD_MACHINE_3(module, name, type, ...)                   \
    namespace module                                                  \
    {                                                                 \
        class name : public core::PayloadMachine<type>, ##__VA_ARGS__ \
        {                                                             \
            using CLASS = module::name;                               \
                                                                      \
        public:                                                       \
            static name &instance()                                   \
            {                                                         \
                static name instance;                                 \
                return instance;                                      \
            }                                                         \
                                                                      \
        private:                                                      \
            virtual ~name() = default;                                \
            name() = default;                                         \
            name(const name &) = delete;                              \
            name &operator=(const name &) = delete;

#define _PAYLOAD_MACHINE_4(module, name, type, numOfBlock, ...)                   \
    namespace module                                                              \
    {                                                                             \
        class name : public core::PayloadMachine<type, numOfBlock>, ##__VA_ARGS__ \
        {                                                                         \
            using CLASS = module::name;                                           \
                                                                                  \
        public:                                                                   \
            static name &instance()                                               \
            {                                                                     \
                static name instance;                                             \
                return instance;                                                  \
            }                                                                     \
                                                                                  \
        private:                                                                  \
            virtual ~name() = default;                                            \
            name() = default;                                                     \
            name(const name &) = delete;                                          \
            name &operator=(const name &) = delete;

#define PAYLOAD_MACHINE(...) \
    _M_MACRO_4(__VA_ARGS__, _PAYLOAD_MACHINE_4, _PAYLOAD_MACHINE_3)(__VA_ARGS__)

#define STATE_DEF(name) void name();
#define STATE_BODY(name) void name()

#define _TRANSITION_1(ev) \
    if (check_((uint8_t)ev, currentState_))

#define _TRANSITION_2(ev, st) \
    if (check_((uint8_t)ev, st))

#define TRANSITION_(...) \
    _M_MACRO_2(__VA_ARGS__, _TRANSITION_2, _TRANSITION_1)(__VA_ARGS__)

#define SM_SWITCH(stateID) \
    __atomic_store_n(&nextState_, (uint8_t)stateID, __ATOMIC_RELEASE)

#define SM_START(stateID)             \
    this->stateTable_ = this->_table; \
    this->maxStates_ = ST_MAX;        \
    this->start_((uint8_t)stateID)

#define ENTER_() if (nextEvent_ == ENTER)
#define EXIT_() if (nextEvent_ == EXIT)
#define TIMEOUT_() if (nextEvent_ == TIMEOUT)

#define _SM_POST_SIMPLE(event) this->postEvent((uint8_t)event)
#define _SM_POST_PAYLOAD(ev, payload) this->postEvent((uint8_t)ev, payload)
#define SM_POST(...) _M_MACRO_2(__VA_ARGS__, _SM_POST_PAYLOAD, _SM_POST_SIMPLE)(__VA_ARGS__)

#define _SM_EXECUTE_SIMPLE(ev)   \
    {                            \
        uint8_t e = (uint8_t)ev; \
        this->execute(e);        \
    }
#define _SM_EXECUTE_PAYLOAD(ev, pay)                           \
    {                                                          \
        pm_event_t e = {.payload = pay, .event = (uint8_t)ev}; \
        this->execute(e);                                      \
    }
#define SM_EXECUTE(...) _M_MACRO_2(__VA_ARGS__, _SM_EXECUTE_PAYLOAD, _SM_EXECUTE_SIMPLE)(__VA_ARGS__)

#define MACHINE_END \
    }               \
    ;               \
    }

#endif
