#include <hydra/controller.h>
#include <hydra/driver.h>

void hydra::Controller::plot(uint8_t channel, int32_t value)
{
    if (channel>7) return;
    plotReg_ |= (1<<channel);
    plotValues_[channel] = value;
}

M_TIMER_HANDLER(hydra::Controller,plot)
{
    if (plotReg_ == 0) return;
    uint16_t plotMask = 0;
    uint8_t buf[34];
    uint8_t index = 2;

    for (int i=0;i<8;i++)
    {
    	if (((plotReg_>>i) & 0x01) == 0) continue;
        int32_t value = plotValues_[i];
        if (value < MAX_ONE && value > -MAX_ONE)
        {
        	plotMask |= (1 << (i*2));
        	buf[index++] = (value & 0xFF);
        }
        else if (value < MAX_TWO && value > -MAX_TWO)
        {
        	plotMask |= (2 << (i*2));
            buf[index++] = ((value >>8) & 0xFF);
            buf[index++] = (value & 0xFF);
        }
        else
        {
        	plotMask |= (3 << (i*2));
            buf[index++] = ((value>>24) & 0xFF);
            buf[index++] = ((value>>16) & 0xFF);
            buf[index++] = ((value>>8) & 0xFF);
            buf[index++] = (value & 0xFF);
        }
    }

    buf[0] = ((plotMask>>8) & 0xFF);
    buf[1] = (plotMask& 0xFF);

    Driver::instance().sendPacket(PlotReceived,index,buf);
}
