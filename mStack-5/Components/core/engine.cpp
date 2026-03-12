#include <core/engine.h>
#include <core/system.h>
#include <core/event.h>
#include <hydra/log.h>

using namespace core;

Event::Event()
{
	index_ = Engine::instance().events().registerEvent_(this);
}

Engine::Engine() : Event(0)
{
	events_.registerEvent_(this);
}

void Engine::init()
{
	pStartTimerEvent_ = new SmallFixedEvent<Timer *>(this,
													 static_cast<SmallFixedEvent<Timer *>::Handler>(&Engine::startTimer_));
	pStopTimerEvent_ = new SmallFixedEvent<Timer *>(this,
													static_cast<SmallFixedEvent<Timer *>::Handler>(&Engine::stopTimer_));
	pCalculateCpuLoadTimer_ = new Timer(this, static_cast<Timer::Handler>(&Engine::calculateCpuLoadTimerHandler_));

	systemInit();
	pCalculateCpuLoadTimer_->start(1000);
}

void Engine::run()
{
	DWT_Init();
	while (true)
	{
		if (events_.next())
			continue;
		idle_();
	}
}

void Engine::registerTimer_(Timer *const &timer)
{
	timer->next_ = timers_;
	timers_ = timer;
}

void Engine::startTimer_(Timer *const &timer)
{
	Timer *prev = nullptr;
	for (Timer *it = timers_; it != nullptr; it = it->next_)
	{
		if (it == timer)
		{
			if (prev == nullptr)
			{
				timers_ = it->next_;
			}
			else
			{
				prev->next_ = it->next_;
			}
			timer->next_ = activeTimers_;
			activeTimers_ = timer;
			break;
		}
		else
		{
		}
		prev = it;
	}

	if (nextTick_ > timer->nextTick_)
	{
		nextTick_ = timer->nextTick_;
	}
	else
	{
	}
}

void Engine::stopTimer_(Timer *const &timer)
{
	Timer *prev = nullptr;
	for (Timer *it = activeTimers_; it != nullptr; it = it->next_)
	{
		if (it == timer)
		{
			if (prev == nullptr)
			{
				activeTimers_ = it->next_;
			}
			else
			{
				prev->next_ = it->next_;
			}
			timer->next_ = timers_;
			timers_ = timer;
			break;
		}
		prev = it;
	}
}

void Engine::delay(uint32_t t)
{
	auto timeout = tickCount_ + t;
	while (tickCount_ < timeout)
	{
		NO_OPERATION;
	}
}

void Engine::execute(const EventPayload &payload)
{
	UNUSED(payload);

	uint64_t min = LAST_TICK;
	uint64_t now = tickCount_;
	Timer *it = activeTimers_;
	Timer *next;
	while (it != nullptr)
	{
		next = it->next_;
		if (now >= it->nextTick_)
		{
			it->run_();
		}
		else
		{
		}
		if (min > it->nextTick_)
		{
			min = it->nextTick_;
		}
		else
		{
		}
		it = next;
	}
	nextTick_ = min;
}

void Engine::calculateCpuLoadTimerHandler_()
{
	uint32_t now = DWT->CYCCNT;
	uint32_t elapsed = now - cpu_stats_.window_start;

	cpu_stats_.cpu_load =
		((float)((elapsed - cpu_stats_.idle_cycles)) / (float)elapsed) * 100;

	cpu_stats_.idle_cycles = 0;
	cpu_stats_.window_start = now;
}
