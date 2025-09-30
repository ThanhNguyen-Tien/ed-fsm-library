#ifndef OSC_TRIPLE_H
#define OSC_TRIPLE_H

#include <core/engine.h>
#include <core/event.h>
#include <console/controller.h>
#include <oscilloscope/data.h>

namespace osc
{

class Triple: public core::Component
{
public:
    Triple(uint8_t c1, uint16_t c2, uint16_t c3);
    typedef void (osc::Triple::*State) (uint16_t, uint16_t, uint16_t);

    inline void plot(uint16_t v1, uint16_t v2, uint16_t v3)
    {
        (this->*state_)(v1, v2, v3);
        c1_.lastVal = v1;
        c2_.lastVal = v2;
        c3_.lastVal = v3;
    }
private:
    void idle_(uint16_t, uint16_t, uint16_t v3){}
    void thresholding_(uint16_t v1, uint16_t v2, uint16_t v3);
    void probing_(uint16_t v1, uint16_t v2, uint16_t v3);
    void first_(uint16_t v1, uint16_t v2, uint16_t v3);
    void adding_(uint16_t v1, uint16_t v2, uint16_t v3);
private:
    State state_ = & osc::Triple::thresholding_;
    uint16_t min_ = 65535;
    uint16_t max_ = 0;
    int16_t threshold_ = -1;

    Data c1_;
    Data c2_;
    Data c3_;
    uint16_t total_ = 0;
private:
    void c1Flush_();
    void c2Flush_();
    void c3Flush_();
    core::EmptyEvent c1FlushEvent_ = core::EmptyEvent(this, (core::EmptyEvent::Handler)&Triple::c1Flush_);
    core::EmptyEvent c2FlushEvent_ = core::EmptyEvent(this, (core::EmptyEvent::Handler)&Triple::c2Flush_);
    core::EmptyEvent c3FlushEvent_ = core::EmptyEvent(this, (core::EmptyEvent::Handler)&Triple::c3Flush_);
};

}

#define O_TRIPLE(name, c1, c2, c3)\
private:\
    osc::Triple name##OSC_ = osc::Triple(c1,c2,c3);\
public:\
    inline void name##Plot(uint16_t v1, uint16_t v2, uint16_t v3){name##OSC_.plot(v1,v2,v3);}

#endif // OSC2_H
