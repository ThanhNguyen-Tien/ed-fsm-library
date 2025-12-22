#ifndef OSC_SINGLE_H
#define OSC_SINGLE_H
#include <core/engine.h>
#include <core/event.h>
#include <hydra/controller.h>
#include <oscilloscope/data.h>


namespace osc
{

class Single: public core::Component
{
public:
	Single(uint8_t channel);
    typedef void (osc::Single::*State) (uint16_t);

    inline void plot(uint16_t v)
    {
        (this->*state_)(v);
        channel_.lastVal = v;
    }
private:
    void idle_(uint16_t){}
    void thresholding_(uint16_t v);
    void probing_(uint16_t v);
    void first_(uint16_t v);
    void adding_(uint16_t v);
private:
    State state_ = & osc::Single::thresholding_;
    uint16_t min_ = 65535;
    uint16_t max_ = 0;
    int16_t threshold_ = -1;

    Data channel_;
    uint16_t total_ = 0;
private:
    void flush_();
    core::EmptyEvent flushEvent_ = core::EmptyEvent(this, (core::EmptyEvent::Handler)&Single::flush_);
};

}

#define O_SINGLE(name, channel)\
private:\
    osc::Single name##OSC_ = osc::Single(channel);\
public:\
    inline void name##Plot(uint16_t v){name##OSC_.plot(v);}

#endif
