#ifndef CONSOLE_COMMAND_H
#define CONSOLE_COMMAND_H

#include <stdint.h>

typedef void (*ConmmandHandler)(uint16_t command, uint8_t length, uint8_t* data);
typedef struct Command
{
	uint16_t type;
	ConmmandHandler handler;
	struct Command* next;
}command_t;

typedef enum ConsoleCommandType
{
	Console_MessageReceived = 1,
	Console_IntFieldReceived,
	Console_TextFieldReceived,
	Console_PlotReceived,
	Console_SyncFields,
	Console_Oscilloscope,
}console_command_type_t;

#define CONSOLE_REGISTER_COMMAND(cmd, name)	name##Command = ConsoleCtrl_RegisterCommand((uint16_t)cmd, &name##CommandHandler)

//**********************************************************************************
#define U_INTEGER_DEF(name)\
	void name##CommandHandler##_(int32_t value);\
	extern command_t* name##Command;

#define U_INTEGER(name)\
	command_t* name##Command = NULL;\
    static void name##CommandHandler(uint16_t command, uint8_t length, uint8_t* data)\
    {\
        int32_t val = ((int32_t)data[0] << 24) | ((int32_t)data[1] << 16) | ((int32_t)data[2] << 8) | ((int32_t)data[3]);\
        name##CommandHandler##_(val);\
        ConsoleCtrl_UpdateIntField(command, val);\
    }

#define U_INTEGER_HANDLER(name) void name##CommandHandler##_(int32_t value)

//**********************************************************************************
#define U_TEXT_DEF(name)\
	void name##CommandHandler##_(uint8_t length, uint8_t* data);\
	extern command_t* name##Command;

#define U_TEXT(name)\
	command_t* name##Command = NULL;\
    static void name##CommandHandler(uint16_t command, uint8_t length, uint8_t* data)\
    {\
        name##CommandHandler##_(length, data);\
        ConsoleCtrl_UpdateTextField(command, length, data);\
    }

#define U_TEXT_HANDLER(name) void name##CommandHandler##_(uint8_t length, uint8_t* data)

//**********************************************************************************
#define U_ACTION_DEF(name)\
	void name##CommandHandler##_();\
	extern command_t* name##Command;

#define U_ACTION(name)\
	command_t* name##Command = NULL;\
    static void name##CommandHandler(uint16_t command, uint8_t length, uint8_t* data)\
	{\
		UNUSED(command);\
		UNUSED(length);\
		UNUSED(data);\
		name##CommandHandler##_();\
	}

#define U_ACTION_HANDLER(name) void name##CommandHandler##_()

#endif // COMMAND_H
