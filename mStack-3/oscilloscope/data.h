#ifndef OSC_DATA_H
#define OSC_DATA_H
#include <console/controller.h>

namespace osc {

struct Data
{
    uint8_t channel;
    uint16_t lastVal;
    uint8_t buffer[1200];
    uint16_t index;
    uint16_t countIndex;

    void reset(uint16_t v)
    {
        countIndex = 0;
        buffer[0] = 1;
        buffer[1] = ((v>>8) & 0xFF);
        buffer[2] = (v & 0xFF);
        index = 3;
    }

    void add(uint16_t v)
    {
        int16_t d = v - lastVal;
        if ((d > 127) || (d < -127) || (buffer[countIndex] > 80))
        {
            countIndex = index;
            buffer[index++] = 1;
            buffer[index++] = ((v >> 8)& 0xFF);
            buffer[index++] = (v & 0xFF);
        }
        else
        {
            buffer[countIndex]++;
            buffer[index++] = d;
        }
    }

    bool flush()
    {
        uint8_t count = buffer[index];
        if (console::Controller::instance().sendOSC(channel, count+1, countIndex, buffer+index+1))
        {
            countIndex += count;
            index += count+2;
            if ((countIndex > 999) || (index > 1198))
            {
                index = 0;
                countIndex = 0;
                return true;
            }
        }
        return false;
    }
    void prepareFlush()
    {
        index = 0;
        countIndex = 0;
    }
};

}

#endif // DATA_H
