#ifndef CORE_ENGINE_H
#define CORE_ENGINE_H

#include <core/event-queue.h>
#include <core/timer.h>

#define LAST_TICK 0xFFFFFFFFFFFFFFFF

namespace core
{
	template <typename E>
	class FixedEvent;

	class Engine : public Event, public Component
	{
	public:
		// Rule of five
		~Engine() = default;
		Engine(const Engine &) = delete;
		Engine &operator=(const Engine &) = delete;
		Engine(Engine &&) = delete;
		Engine &operator=(Engine &&) = delete;

		// Singleton access
		static Engine &instance()
		{
			static Engine engine;
			return engine;
		}

		// Core API
		void init() override;
		void run();

		inline void isrEnter(void)
		{
			if (cpu_stats_.in_idle)
			{
				uint32_t now = DWT->CYCCNT;
				cpu_stats_.idle_cycles += (now - cpu_stats_.last_idle_start);
				cpu_stats_.in_idle = 0;
			}
		}

		inline void tick() /* must be called in timer interrupt or SysTick interrupt, usually 1ms */
		{
			if (++tickCount_ >= nextTick_)
			{
				events_.post(index_);
			}
		}

		inline uint64_t tickCount()
		{
			return tickCount_;
		}
		void delay(uint32_t t); // t in ms, WARNING: blocking, use only in limited contexts
		EventQueue &events()
		{
			return events_;
		}
		float getCpuLoad()
		{
			return cpu_stats_.cpu_load;
		}
		uint16_t checkNumOfEvent()
		{
			return events_.poolSize_;
		}

	private:
		Engine();
		inline void idle_(void)
		{
			cpu_stats_.last_idle_start = DWT->CYCCNT;
			cpu_stats_.in_idle = 1;

			WAIT_FOR_INTERRUPT;
		}

		typedef struct
		{
			float cpu_load;
			uint32_t window_start;

			uint32_t idle_cycles;
			uint32_t last_idle_start;
			uint8_t in_idle;
		} cpu_stats_t;

		volatile cpu_stats_t cpu_stats_;
		void execute(const EventPayload &payload) override;

		// Timer management
		void registerTimer_(Timer *const &timer);
		void startTimer_(Timer *const &timer);
		void stopTimer_(Timer *const &timer);
		void calculateCpuLoadTimerHandler_();

	private:
		// Data members
		FixedEvent<Timer *> *pStartTimerEvent_ = nullptr;
		FixedEvent<Timer *> *pStopTimerEvent_ = nullptr;
		Timer *pCalculateCpuLoadTimer_ = nullptr;
		Timer *timers_ = nullptr;
		Timer *activeTimers_ = nullptr;

		EventQueue events_;
		volatile uint64_t tickCount_ = 0;
		uint64_t nextTick_ = LAST_TICK;

		friend class Timer;
	};

}

#ifdef DEBUG
#define DEBUG_HOOK(module, cls) \
	module::cls *cls##Instance = nullptr;
#define DEBUG_ATTACH(module, cls) \
	cls##Instance = &module::cls::instance();
#else
#define DEBUG_HOOK(module, cls)
#define DEBUG_ATTACH(module, cls)
#endif

#endif // ENGINE_H
