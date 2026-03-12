#ifndef HYDRA_COMMAND_H
#define HYDRA_COMMAND_H
#include <cstdint>

namespace hydra
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
        hydra::Controller::instance().updateIntField(command, val);\
    }\
private:\
    void name##CommandHandler##_(int32_t value);\
    hydra::Command* name##Command_ = hydra::Controller::instance().registerCommand((uint16_t)cmd, &name##Command);

#define U_INTEGER_HANDLER(cls,name) void cls::name##CommandHandler##_(int32_t value)

#define U_TEXT(cmd, name)\
public:\
    static void name##Command(uint16_t command, uint8_t length, uint8_t* data)\
    {\
        instance().name##CommandHandler##_(length, data);\
        hydra::Controller::instance().updateTextField(command, length, data);\
    }\
private:\
    void name##CommandHandler##_(uint8_t length, uint8_t* data);\
    hydra::Command* name##Command_ = hydra::Controller::instance().registerCommand((uint16_t)cmd, &name##Command);

#define U_TEXT_HANDLER(cls,name) void cls::name##CommandHandler##_(uint8_t length, uint8_t* data)

#define U_ACTION(cmd, name)\
public:\
    static void name##Command(uint16_t command, uint8_t length, uint8_t* data){instance().name##CommandHandler##_();}\
private:\
    void name##CommandHandler##_();\
    hydra::Command* name##Command_ = hydra::Controller::instance().registerCommand((uint16_t)cmd, &name##Command);

#define U_ACTION_HANDLER(cls,name) void cls::name##CommandHandler##_()

#endif // COMMAND_H
