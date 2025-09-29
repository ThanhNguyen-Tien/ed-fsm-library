#ifndef CORE_ENGINE_H
#define CORE_ENGINE_H

#include <core/event-queue.h>
#include <core/timer.h>
#include <assert.h>

#define LAST_TICK       0xFFFFFFFFFFFFFFFF

namespace core {
template<typename E>
class FixedEvent;

class Engine: public Event, public Component {
public:
	// Rule of five
	~Engine() = default;
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine(Engine&&) = delete;
	Engine& operator=(Engine&&) = delete;

	// Singleton access
	static Engine& instance() {
		static Engine engine;
		return engine;
	}

	// Core API
	void init() override;
	void run();
	inline void tick(); /* must be called in timer interrupt or SysTick interrupt, usually 1ms */
	inline uint64_t tickCount() {
		return tickCount_;
	}
	void delay(uint32_t t); // t in ms, WARNING: blocking, use only in limited contexts
	EventQueue& events() {
		return events_;
	}
	uint16_t checkNumOfEvent() {
		return events_.poolSize_;
	}

private:
	Engine();
	void execute(AbstractEventQueue *queue) override;

	// Timer management
	void registerTimer_(Timer *const&timer);
	void startTimer_(Timer *const&timer);
	void stopTimer_(Timer *const&timer);

private:
	// Data members
	FixedEvent<Timer*> *pStartTimerEvent_ = nullptr;
	FixedEvent<Timer*> *pStopTimerEvent_ = nullptr;
	Timer *timers_ = nullptr;
	Timer *activeTimers_ = nullptr;

	EventQueue events_;
	volatile uint64_t tickCount_ = 0;
	uint64_t nextTick_ = LAST_TICK;

	friend class Timer;
};

inline void Engine::tick() {
	if (++tickCount_ >= nextTick_) {
		events_.post(index_);
	}
}
}

#ifdef DEBUG
#define DEBUG_HOOK(ns, cls) \
        ns::cls* cls##Instance = nullptr;
#define DEBUG_ATTACH(ns, cls) \
        cls##Instance = &ns::cls::instance();
#else
    #define DEBUG_HOOK(ns, cls)
    #define DEBUG_ATTACH(ns, cls)
#endif

#endif // ENGINE_H
