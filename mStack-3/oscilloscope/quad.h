#ifndef OSC_QUADRUPLE_H
#define OSC_QUADRUPLE_H

#include <core/engine.h>
#include <console/controller.h>
#include <oscilloscope/data.h>
#include <core/event.h>

namespace osc
{

class Quad: public core::Component
{
public:
    Quad(uint8_t c1, uint16_t c2, uint16_t c3, uint16_t c4);
    typedef void (osc::Quad::*State) (uint16_t, uint16_t, uint16_t, uint16_t);

    inline void plot(uint16_t v1, uint16_t v2, uint16_t v3, uint16_t v4)
    {
        (this->*state_)(v1, v2, v3, v4);
        c1_.lastVal = v1;
        c2_.lastVal = v2;
        c3_.lastVal = v3;
        c4_.lastVal = v4;
    }
private:
    void idle_(uint16_t, uint16_t, uint16_t v3, uint16_t v4){}
    void thresholding_(uint16_t v1, uint16_t v2, uint16_t v3, uint16_t v4);
    void probing_(uint16_t v1, uint16_t v2, uint16_t v3, uint16_t v4);
    void first_(uint16_t v1, uint16_t v2, uint16_t v3, uint16_t v4);
    void adding_(uint16_t v1, uint16_t v2, uint16_t v3, uint16_t v4);
private:
    State state_ = & osc::Quad::thresholding_;
    uint16_t min_ = 65535;
    uint16_t max_ = 0;
    int16_t threshold_ = -1;

    Data c1_;
    Data c2_;
    Data c3_;
    Data c4_;
    uint16_t total_ = 0;
private:
    void c1Flush_();
    void c2Flush_();
    void c3Flush_();
    void c4Flush_();
    core::EmptyEvent c1FlushEvent_ = core::EmptyEvent(this, (core::EmptyEvent::Handler)&Quad::c1Flush_);
    core::EmptyEvent c2FlushEvent_ = core::EmptyEvent(this, (core::EmptyEvent::Handler)&Quad::c2Flush_);
    core::EmptyEvent c3FlushEvent_ = core::EmptyEvent(this, (core::EmptyEvent::Handler)&Quad::c3Flush_);
    core::EmptyEvent c4FlushEvent_ = core::EmptyEvent(this, (core::EmptyEvent::Handler)&Quad::c4Flush_);
};

}

#define O_QUAD(name, c1, c2, c3, c4)\
private:\
    osc::Quad name##OSC_ = osc::Quad(c1,c2,c3,c4);\
public:\
    inline void name##Plot(uint16_t v1, uint16_t v2, uint16_t v3, uint16_t v4){name##OSC_.plot(v1,v2,v3,v4);}

#endif // QUADRUPLE_H
