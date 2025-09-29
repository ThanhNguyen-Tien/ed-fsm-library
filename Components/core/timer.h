#ifndef CORE_TIMER_H
#define CORE_TIMER_H

#include <core/base.h>

namespace core {
class Timer {
public:
	using Handler = void (Component::*)();
	Timer(Component *component, Handler handler);
	void start(uint32_t interval, int32_t loop = -1);
	void stop();
	inline bool isRunning() {
		return (loop_ != 0);
	}
private:
	void run_();
	Timer *next_ = nullptr;
	uint32_t interval_ = 0U;
	uint64_t nextTick_ = 0U;
	int32_t loop_ = 0;
	Component *component_ = nullptr;
	Handler handler_ = nullptr;
	friend class Engine;
};
}

#define _M_TIMER(name)\
private:\
    core::Timer name##Timer_ = core::Timer(this, (core::Timer::Handler)&CLASS::name##Handler##_);\
    void name##Handler##_();\

#define _M_TIMER_2(name,code)\
private:\
    core::Timer name##Timer_ = core::Timer(this, (core::Timer::Handler)&CLASS::name##Handler##_);\
    void name##Handler##_() code
#define M_TIMER(...) _M_MACRO_2(__VA_ARGS__, _M_TIMER_2, _M_TIMER)(__VA_ARGS__)

#define M_TIMER_HANDLER(cls,name) void cls::name##Handler##_()

#endif // TIMER_H
