#ifndef CONSOLE_CONTROLLER_H_
#define CONSOLE_CONTROLLER_H_

#include <stdint.h>
#include <stdbool.h>
#include <console/driver.h>
#include <console/command.h>
#include <console/define.h>
#include <core/task.h>

M_TASK_DEF(ConsoleCtrl_Plot)

typedef struct ConsoleController
{
    command_t* commands;
    int32_t plotValues[8];
    uint32_t errorCount;
    uint8_t plotReg;
}console_ctrl_t;
extern console_ctrl_t consoleCtrl;

void ConsoleCtrl_Init();
void ConsoleCtrl_Print(const char* text);
void ConsoleCtrl_Printf(const char* format, ... );

command_t* ConsoleCtrl_RegisterCommand(uint16_t type, ConmmandHandler handler);
void ConsoleCtrl_UpdateIntField(uint16_t type, int32_t value);
void ConsoleCtrl_UpdateTextField(uint16_t type, uint8_t length, uint8_t* data);
void ConsoleCtrl_Plot(uint8_t channel, int32_t value);
void ConsoleCtrl_ProcessCommand(uint16_t type, uint8_t length, uint8_t* data);
bool ConsoleCtrl_SendOSC(uint8_t channel, uint8_t length, uint16_t index, const uint8_t* data);

#endif /* CONSOLE_CONTROLLER_H_ */
