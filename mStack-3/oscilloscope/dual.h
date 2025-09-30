#ifndef OSC_DOUBLE_H
#define OSC_DOUBLE_H

#include <core/engine.h>
#include <core/event.h>
#include <console/controller.h>
#include <oscilloscope/data.h>


namespace osc
{

class Dual: public core::Component
{

public:
    Dual(uint8_t c1, uint16_t c2);
    typedef void (osc::Dual::*State) (uint16_t, uint16_t);

    inline void plot(uint16_t v1, uint16_t v2)
    {
        (this->*state_)(v1, v2);
        c1_.lastVal = v1;
        c2_.lastVal = v2;
    }
private:
    void idle_(uint16_t, uint16_t){}
    void thresholding_(uint16_t v1, uint16_t v2);
    void probing_(uint16_t v1, uint16_t v2);
    void first_(uint16_t v1, uint16_t v2);
    void adding_(uint16_t v1, uint16_t v2);
private:
    State state_ = & osc::Dual::thresholding_;
    uint16_t min_ = 65535;
    uint16_t max_ = 0;
    int16_t threshold_ = -1;

    Data c1_;
    Data c2_;
    uint16_t total_ = 0;
private:
    void c1Flush_();
    void c2Flush_();
    core::EmptyEvent c1FlushEvent_ = core::EmptyEvent(this, (core::EmptyEvent::Handler)&Dual::c1Flush_);
    core::EmptyEvent c2FlushEvent_ = core::EmptyEvent(this, (core::EmptyEvent::Handler)&Dual::c2Flush_);
};

}

#define O_DUAL(name, c1, c2)\
private:\
    osc::Dual name##OSC_ = osc::Dual(c1,c2);\
public:\
    inline void name##Plot(uint16_t v1, uint16_t v2){name##OSC_.plot(v1,v2);}

#endif
