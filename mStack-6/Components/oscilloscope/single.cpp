#include <oscilloscope/Single.h>
#include <hydra/log.h>

osc::Single::Single(uint8_t channel)
{
    channel_.channel = channel;
}

void osc::Single::thresholding_(uint16_t v)
{
    if (threshold_ > 0)
    {
        total_ = 0;
        state_ = &osc::Single::probing_;
        return;
    }

    if (v < min_) min_ = v;
    if (v > max_) max_ = v;
    if (total_++ > MAX_TOTAL_SAMPLES_PER_BUF)
    {
        threshold_ = (min_+max_)/2;
        min_ = 65535;
        max_ = 0;
        total_ = 0;
        LOG_INFO_PRINTF("Auto detect threshold:%d", threshold_);
        state_ = &osc::Single::probing_;
    }
}

void osc::Single::probing_(uint16_t v)
{
    if (++total_ > MAX_TOTAL_SAMPLES_PER_BUF)
    {
        total_ = 0;
        threshold_ = -1;
        state_ = &osc::Single::thresholding_;
    }

    if ((channel_.lastVal < threshold_) && (v >= threshold_))
    {
        state_ = &osc::Single::first_;
    }
}

void osc::Single::first_(uint16_t v)
{
    channel_.reset(v);
    total_ = 1;
    state_ = &osc::Single::adding_;
}

void osc::Single::adding_(uint16_t v)
{
    channel_.add(v);

    total_++;
    if ((total_ > MAX_TOTAL_SAMPLES_PER_BUF) || (channel_.index > MAX_INDEX_ADDING))
    {
        state_ = &osc::Single::idle_;
        total_ = 0;
        channel_.prepareFlush();
        flushEvent_.post();
    }
}

void osc::Single::flush_()
{
    if(channel_.flush())
    {
    	state_ = &osc::Single::probing_;
    }
    else
    {
    	flushEvent_.post();
    }
}
