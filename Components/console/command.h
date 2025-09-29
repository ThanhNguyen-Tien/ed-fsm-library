#ifndef CONSOLE_COMMAND_H
#define CONSOLE_COMMAND_H
#include <stdint.h>

namespace console
{
    typedef void (*Handler)(uint16_t command, uint8_t length, uint8_t* data);
    struct Command
    {
        uint16_t type;
        Handler handler;
        Command* next;
    };
}

#define U_INTEGER(cmd, name)\
public:\
    static void name##Command(uint16_t command, uint8_t length, uint8_t* data)\
    {\
        int32_t val = ((int32_t)data[0] << 24) | ((int32_t)data[1] << 16) | ((int32_t)data[2] << 8) | ((int32_t)data[3]);\
        instance().name##CommandHandler##_(val);\
        console::Controller::instance().updateIntField(command, val);\
    }\
private:\
    void name##CommandHandler##_(int32_t value);\
    console::Command* name##Command_ = console::Controller::instance().registerCommand((uint16_t)cmd, &name##Command);

#define U_INTEGER_HANDLER(cls,name) void cls::name##CommandHandler##_(int32_t value)

#define U_TEXT(cmd, name)\
public:\
    static void name##Command(uint16_t command, uint8_t length, uint8_t* data)\
    {\
        instance().name##CommandHandler##_(length, data);\
        console::Controller::instance().updateTextField(command, length, data);\
    }\
private:\
    void name##CommandHandler##_(uint8_t length, uint8_t* data);\
    console::Command* name##Command_ = console::Controller::instance().registerCommand((uint16_t)cmd, &name##Command);

#define U_TEXT_HANDLER(cls,name) void cls::name##CommandHandler##_(uint8_t length, uint8_t* data)

#define U_ACTION(cmd, name)\
public:\
    static void name##Command(uint16_t command, uint8_t length, uint8_t* data){instance().name##CommandHandler##_();}\
private:\
    void name##CommandHandler##_();\
    console::Command* name##Command_ = console::Controller::instance().registerCommand((uint16_t)cmd, &name##Command);

#define U_ACTION_HANDLER(cls,name) void cls::name##CommandHandler##_()

#endif // COMMAND_H
