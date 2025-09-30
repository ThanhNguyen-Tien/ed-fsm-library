#include <oscilloscope/single.h>
#include <console/controller.h>

osc::Single::Single(uint8_t channel): channel_(channel)
{
}

void osc::Single::thresholding_(uint16_t v)
{
    if (threshold_ > 0)
    {
        index_ = 0;
        state_ = &osc::Single::probing_;
        return;
    }

    if (v < min_) min_ = v;
    if (v > max_) max_ = v;
    if (index_++ > 1000)
    {
        threshold_ = (min_+max_)/2;
        min_ = 65535;
        max_ = 0;
        index_ = 0;
//        console::Controller::instance().printf("Auto detect threshold:%d", threshold_);
        state_ = &osc::Single::probing_;
    }
}

void osc::Single::probing_(uint16_t v)
{
    if (++index_ > 1000)
    {
        index_ = 0;
        threshold_ = -1;
        state_ = &osc::Single::thresholding_;
    }

    if ((lastVal_< threshold_) && (v >= threshold_))
    {
        state_ = &osc::Single::first_;
    }
}

void osc::Single::first_(uint16_t v)
{
    countIndex_ = 0;
    data_[0] = 1;
    data_[1] = ((v>>8) & 0xFF);
    data_[2] = (v & 0xFF);
    index_ = 3;
    total_ = 1;
    state_ = &osc::Single::adding_;
}

void osc::Single::adding_(uint16_t v)
{
    int16_t d = v - lastVal_;
    if ((d > 127) || (d < -127) || (data_[countIndex_] > 80))
    {
        countIndex_ = index_;
        data_[index_++] = 1;
        data_[index_++] = ((v >> 8)& 0xFF);
        data_[index_++] = (v & 0xFF);
    }
    else
    {
        data_[countIndex_]++;
        data_[index_++] = d;
    }

    total_++;
    if ((total_ > 1000) || (index_ > 1199))
    {
        state_ = &osc::Single::idle_;
        index_ = 0;
        total_ = 0;
        flushEvent_.post();
    }
}

void osc::Single::flush_()
{
    uint8_t count = data_[index_];
    if (console::Controller::instance().sendOSC(channel_, count+1, total_, data_+index_+1))
    {
        total_ += count;
        index_ += count+2;
        if ((total_ > 999) || (index_ > 1198))
        {
            index_ = 0;
            total_ = 0;
            state_ = &osc::Single::probing_;
            return;
        }
    }
    flushEvent_.post();
}
