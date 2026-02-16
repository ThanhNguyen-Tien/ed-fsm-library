#ifndef OSC_DATA_H
#define OSC_DATA_H
#include <hydra/controller.h>

namespace osc {

static const uint16_t BUFFER_SIZE = 1200;
static const uint16_t MAX_INDEX_ADDING = BUFFER_SIZE - 1;
static const uint16_t MAX_INDEX_FLUSH = BUFFER_SIZE - 2;
static const uint16_t MAX_TOTAL_SAMPLES_PER_BUF = 1000;
static const uint16_t MAX_COUNT_INDEX_PER_BUF = MAX_TOTAL_SAMPLES_PER_BUF - 1;
static const uint8_t MAX_NUM_OF_SAMPLES_PER_PACK = 120;

struct Data
{
    uint8_t channel;
    uint16_t lastVal;
    uint8_t buffer[BUFFER_SIZE];
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
        if ((d > 127) || (d < -127) || (buffer[countIndex] > MAX_NUM_OF_SAMPLES_PER_PACK))
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
        if(count != 0)
        {
            if(hydra::Controller::instance().sendOSC(channel, count+1, countIndex, buffer+index+1))
            {
        		countIndex += count;
        		index += count+2;
        		if ((countIndex > MAX_COUNT_INDEX_PER_BUF) || (index > MAX_INDEX_FLUSH))
        		{
        			index = 0;
        			countIndex = 0;
        			return true;
        		}
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
