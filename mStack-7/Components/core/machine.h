#ifndef CORE_MACHINE_H
#define CORE_MACHINE_H

#include <core/event.h>
#include <core/timer.h>

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

        inline void callState_(uint32_t s)
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

        void start_(uint32_t state)
        {
            currentState_ = state;
            nextEvent_ = ENTER;
            callState_(currentState_);
        }

        bool check_(uint32_t event, uint32_t state)
        {
            if (nextEvent_ == event)
            {
                nextState_ = state;
                return true;
            }
            return false;
        }

    protected:
        const StateHandler *stateTable_ = nullptr;
        uint32_t maxStates_ = 0;

        uint32_t currentState_ = 0;
        uint32_t nextState_ = 0xFF;
        uint32_t nextEvent_ = 0;

        Timer timer_ = Timer(this, static_cast<Timer::Handler>(&Machine::onTimeout_));

        static constexpr uint32_t TIMEOUT = 0xFFFFFFFD;
        static constexpr uint32_t ENTER = 0xFFFFFFFE;
        static constexpr uint32_t EXIT = 0xFFFFFFFF;
    };

    class SimpleMachine : public Machine
    {
    public:
        void postEvent(uint32_t event) { this->postEvent_.post(event); }

        void execute(const uint32_t &event)
        {
            nextState_ = 0xFF;
            nextEvent_ = event;

            uint32_t activeState = currentState_;
            callState_(activeState);

            if (nextState_ != 0xFF && nextState_ != activeState)
            {
                nextEvent_ = EXIT;
                callState_(activeState);
                currentState_ = nextState_;
                nextEvent_ = ENTER;
                callState_(currentState_);
            }
        }

    protected:
        void onTimeout_() override { this->postEvent(TIMEOUT); }
        WordEvent postEvent_ = WordEvent(this, static_cast<WordEvent::Handler>(&SimpleMachine::execute));
    };
}

/* ===================== STATE GENERATOR ===================== */
#define STATE_MAP_BEGIN    \
public:                    \
    enum StateID : uint32_t \
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

#define MACHINE(module, name, ...)                      \
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

#define STATE_DEF(name) void name();
#define STATE_BODY(name) void name()

#define _TRANSITION_1(ev) \
    if (check_((uint32_t)ev, currentState_))

#define _TRANSITION_2(ev, st) \
    if (check_((uint32_t)ev, st))

#define TRANSITION_(...) \
    _M_MACRO_2(__VA_ARGS__, _TRANSITION_2, _TRANSITION_1)(__VA_ARGS__)

#define SM_SWITCH(stateID) nextState_ = static_cast<core::Machine::State>(&CLASS::stateID)

#define SM_START(stateID)             \
    this->stateTable_ = this->_table; \
    this->maxStates_ = ST_MAX;        \
    this->start_((uint32_t)stateID)

#define ENTER_() if (nextEvent_ == ENTER)
#define EXIT_() if (nextEvent_ == EXIT)
#define TIMEOUT_() if (nextEvent_ == TIMEOUT)

#define SM_POST(event) this->postEvent((uint32_t)event)

#endif
