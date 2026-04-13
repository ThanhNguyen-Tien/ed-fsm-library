#include <core/engine.h>
#include <core/system.h>
#include <core/config.h>
#include <core/event.h>
#include <hydra/log.h>

namespace core
{
	template <uint8_t N>
	alignas(32) uint32_t EventQueue<N>::tempData_[32];

	#define X(prio, size) \
		static C_PriorityQueue<size> priorityQueue_##prio;
		CORE_PRIORITY_TABLE
	#undef X

	#define X(prio, size) \
		static_cast<I_MpscQueue*>(&priorityQueue_##prio),

	static I_MpscQueue* const queueInterfaces[] = {
		CORE_PRIORITY_TABLE
	};
	#undef X

	Event::Event(uint8_t p) : priority_(p & 0x07)
	{
		this->index_ = Engine::instance().events().registerEvent_(this);
	}

	Engine::Engine() : Event(0, 0), events_(core::queueInterfaces, CORE_NUM_PRIORITIES)
	{
		this->events().registerEvent_(this);
	}

	void Engine::init()
	{
		this->pStartTimerEvent_ = new FixedEvent<Timer *>(this,
															   static_cast<FixedEvent<Timer *>::Handler>(&Engine::startTimer_));
		this->pStopTimerEvent_ = new FixedEvent<Timer *>(this,
															  static_cast<FixedEvent<Timer *>::Handler>(&Engine::stopTimer_));
		this->pCalculateCpuLoadTimer_ = new Timer(this, static_cast<Timer::Handler>(&Engine::calculateCpuLoadTimerHandler_));

		systemInit();
		this->pCalculateCpuLoadTimer_->start(1000);
	}

	void Engine::run()
	{
		DWT_Init();
		while (true)
		{
			if (this->events_.next())
				continue;
			idle_();
		}
	}

	void Engine::registerTimer_(Timer *const &timer)
	{
		timer->next_ = this->timers_;
		this->timers_ = timer;
	}

	void Engine::startTimer_(Timer *const &timer)
	{
		Timer *prev = nullptr;
		for (Timer *it = this->timers_; it != nullptr; it = it->next_)
		{
			if (it == timer)
			{
				if (prev == nullptr)
				{
					this->timers_ = it->next_;
				}
				else
				{
					prev->next_ = it->next_;
				}
				timer->next_ = this->activeTimers_;
				this->activeTimers_ = timer;
				break;
			}
			else
			{
			}
			prev = it;
		}

		if (this->nextTick_ > timer->nextTick_)
		{
			this->nextTick_ = timer->nextTick_;
		}
		else
		{
		}
	}

	void Engine::stopTimer_(Timer *const &timer)
	{
		Timer *prev = nullptr;
		for (Timer *it = this->activeTimers_; it != nullptr; it = it->next_)
		{
			if (it == timer)
			{
				if (prev == nullptr)
				{
					this->activeTimers_ = it->next_;
				}
				else
				{
					prev->next_ = it->next_;
				}
				timer->next_ = this->timers_;
				this->timers_ = timer;
				break;
			}
			prev = it;
		}
	}

	void Engine::delay(uint32_t t)
	{
		auto timeout = this->tickCount_ + t;
		while (this->tickCount_ < timeout)
		{
			NO_OPERATION;
		}
	}

	void Engine::execute(const void* payload)
	{
		UNUSED(payload);

		uint64_t min = LAST_TICK;
		uint64_t now = this->tickCount_;
		Timer *it = this->activeTimers_;
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
		this->nextTick_ = min;
	}

	void Engine::calculateCpuLoadTimerHandler_()
	{
		uint32_t now = DWT->CYCCNT;
		uint32_t elapsed = now - this->cpu_stats_.window_start;

		this->cpu_stats_.cpu_load =
			((float)((elapsed - this->cpu_stats_.idle_cycles)) / (float)elapsed) * 100;

		this->cpu_stats_.idle_cycles = 0;
		this->cpu_stats_.window_start = now;
	}
}
