#include <oscilloscope/dual.h>
#include <console/controller.h>

osc::Dual::Dual(uint8_t c1, uint16_t c2)
{
    c1_.channel = c1;
    c2_.channel = c2;
}

void osc::Dual::thresholding_(uint16_t v1, uint16_t v2)
{
    if (threshold_ > 0)
    {
        total_ = 0;
        state_ = &osc::Dual::probing_;
        return;
    }

    if (v1 < min_) min_ = v1;
    if (v1 > max_) max_ = v1;
    if (total_++ > 1000)
    {
        threshold_ = (min_+max_)/2;
        min_ = 65535;
        max_ = 0;
        total_ = 0;
//        console::Controller::instance().printf("Auto detect threshold:%d", threshold_);
        state_ = &osc::Dual::probing_;
    }
}

void osc::Dual::probing_(uint16_t v1, uint16_t v2)
{
    if (++total_ > 1000)
    {
        total_ = 0;
        threshold_ = -1;
        state_ = &osc::Dual::thresholding_;
    }

    if ((c1_.lastVal < threshold_) && (v1 >= threshold_))
    {
        state_ = &osc::Dual::first_;
    }
}

void osc::Dual::first_(uint16_t v1, uint16_t v2)
{
    c1_.reset(v1);
    c2_.reset(v2);
    total_ = 1;
    state_ = &osc::Dual::adding_;
}

void osc::Dual::adding_(uint16_t v1, uint16_t v2)
{
    c1_.add(v1);
    c2_.add(v2);

    total_++;
    if ((total_ > 1000) || (c1_.index > 1199) || (c2_.index > 1199))
    {
        state_ = &osc::Dual::idle_;
        total_ = 0;
        c1_.prepareFlush();
        c2_.prepareFlush();
        c1FlushEvent_.post();
    }
}

void osc::Dual::c1Flush_()
{
	static uint32_t count = 0;
    if (c1_.flush())
    {
        c2FlushEvent_.post();
        count = 0;
    }
    else
    {
        c1FlushEvent_.post();
        count++;
    }
}

void osc::Dual::c2Flush_()
{
	static uint32_t count = 0;
    if (c2_.flush())
    {
        state_ = &osc::Dual::probing_;
        count = 0;
        //uart::Controller::instance().print("finish flush c2");
    }
    else
    {
        c2FlushEvent_.post();
        count++;
    }
}

