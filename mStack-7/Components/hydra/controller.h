#ifndef HYDRA_CONTROLLER_H_
#define HYDRA_CONTROLLER_H_

#include <core/engine.h>
#include <core/timer.h>
#include <hydra/command.h>

COMPONENT(hydra, Controller)
    M_TIMER(plot)
public:
    void init();
    void print(uint16_t level ,const char* text);
    void printf(uint16_t level ,const char* format, ...);

    Command* registerCommand(uint16_t type, Handler handler);
    void updateIntField(uint16_t type, int32_t value);
    void updateTextField(uint16_t type, char* data);
    void updateTextField(uint16_t type, uint8_t length, uint8_t* data);
    void plot(uint8_t channel, int32_t value);
    void processCommand(uint16_t type, uint8_t length, uint8_t* data);
    bool sendOSC(uint8_t channel, uint8_t length, uint16_t index, const uint8_t* data);
private:
    Command* commands_ = nullptr;
    uint8_t plotReg_ = 0;
    int32_t plotValues_[8];
    uint32_t errorCount_ = 0;
public:
    typedef enum CommandType : uint16_t
    {
        LogDebug = 0,
		LogInfo,
		LogWarning,
		LogError,
		LogCritical,
        IntFieldReceived,
        TextFieldReceived,
        PlotReceived,
        Oscilloscope,
    }command_type_t;
COMPONENT_END


#endif /* HYDRA_CONTROLLER_H_ */
