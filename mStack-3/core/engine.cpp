#include <core/engine.h>
#include <core/system.h>
#include <core/event.h>
#include <console/log.h>

using namespace core;

Event::Event() {
	index_ = Engine::instance().events().registerEvent_(this);
}

Engine::Engine() : Event(0)
{
	events_.registerEvent_(this);
}

void Engine::init() {
	pStartTimerEvent_ = new SmallFixedEvent<Timer*>(this,
					static_cast<SmallFixedEvent<Timer*>::Handler>(&Engine::startTimer_));
	pStopTimerEvent_ = new SmallFixedEvent<Timer*>(this,
			static_cast<SmallFixedEvent<Timer*>::Handler>(&Engine::stopTimer_));
	systemInit();
}

void Engine::run() {
#ifdef MONITOR_EVENT_TIME_EXECUTION
	DWT_Init();
#endif
	while (true) {
		if (events_.next())
			continue;
		WAIT_FOR_INTERRUPT;
	}
}

void Engine::registerTimer_(Timer *const&timer) {
	timer->next_ = timers_;
	timers_ = timer;
}

void Engine::startTimer_(Timer *const&timer) {
	Timer *prev = nullptr;
	for (Timer *it = timers_; it != nullptr; it = it->next_) {
		if (it == timer) {
			if (prev == nullptr) {
				timers_ = it->next_;
			} else {
				prev->next_ = it->next_;
			}
			timer->next_ = activeTimers_;
			activeTimers_ = timer;
			break;
		}
		prev = it;
	}

	if (nextTick_ > timer->nextTick_) {
		nextTick_ = timer->nextTick_;
	}
}

void Engine::stopTimer_(Timer *const&timer) {
	Timer *prev = nullptr;
	for (Timer *it = activeTimers_; it != nullptr; it = it->next_) {
		if (it == timer) {
			if (prev == nullptr) {
				activeTimers_ = it->next_;
			} else {
				prev->next_ = it->next_;
			}
			timer->next_ = timers_;
			timers_ = timer;
			break;
		}
		prev = it;
	}
}

void Engine::delay(uint32_t t) {
	auto timeout = tickCount_ + t;
	while (tickCount_ < timeout) {
		NO_OPERATION;
	}
}

void Engine::execute(AbstractEventQueue *queue) {
	uint64_t min = LAST_TICK;
	Timer *it = activeTimers_;
	Timer *next;
	while (it != nullptr) {
		next = it->next_;
		if (tickCount_ >= it->nextTick_) {
			it->run_();
		}
		if (min > it->nextTick_) {
			min = it->nextTick_;
		}
		it = next;
	}
	nextTick_ = min;
}
