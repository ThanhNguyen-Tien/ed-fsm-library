#ifndef CONSOLE_CONTROLLER_H_
#define CONSOLE_CONTROLLER_H_

#include <core/queue.h>
#include <core/engine.h>
#include <core/timer.h>
#include <console/command.h>

COMPONENT(console, Controller)
    M_TIMER(plot)
public:
    void init();
    void print(const char* text);
    void printf(const char* format, ... );

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
    enum CommandType
    {
        MessageReceived = 1,
        IntFieldReceived,
        TextFieldReceived,
        PlotReceived,
        SyncFields,
        Oscilloscope,
    };
COMPONENT_END


#endif /* UART_CONTROLLER_H_ */
