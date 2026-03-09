#include <core/timer.h>
#include <core/engine.h>
#include <core/event.h>

using namespace core;

Timer::Timer(Component *component, Handler handler) : component_(component), handler_(handler)
{
	Engine::instance().registerTimer_(this);
}

void Timer::start(uint32_t interval, int32_t loop)
{
	static Engine &engine = Engine::instance();
	this->interval_ = interval;
	this->nextTick_ = engine.tickCount() + interval;
	if (!isRunning())
	{
		this->loop_ = loop;
		engine.pStartTimerEvent_->post(this);
	}
	else
	{
		this->loop_ = loop;
	}
}

void Timer::stop()
{
	loop_ = 0;
	Engine::instance().pStopTimerEvent_->post(this);
	this->nextTick_ = LAST_TICK - 1;
}

void Timer::run_()
{
	if (--loop_ == 0)
	{
		Engine::instance().stopTimer_(this);
		this->nextTick_ = LAST_TICK - 1;
	}
	else
	{
		nextTick_ += interval_;
		if (loop_ < 0)
		{
			loop_ = -1;
		}
	}

	(component_->*handler_)();
}
