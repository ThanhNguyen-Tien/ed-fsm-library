#include <console/controller.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <core/utils.h>
#include <core/engine.h>

console_ctrl_t consoleCtrl;
M_TASK(ConsoleCtrl_Plot)

void ConsoleCtrl_Init()
{
	ConsoleDrv_Init();
	M_TASK_INIT(ConsoleCtrl_Plot);
	M_TASK_START(ConsoleCtrl_Plot, TIME_PLOT);
}

void ConsoleCtrl_Print(const char* text)
{
    int l = strlen(text) + 1;
    ConsoleDrv_SendPacket(Console_MessageReceived, l, (uint8_t*)text);
}

void ConsoleCtrl_Printf(const char* format, ... )
{
    char buf[256];
    va_list args;
    va_start (args, format);
    int l = vsprintf(buf, format ,args) + 1;
    va_end(args);
    ConsoleDrv_SendPacket(Console_MessageReceived, l, (uint8_t*)buf);
}

command_t* ConsoleCtrl_RegisterCommand(uint16_t type, ConmmandHandler handler)
{
    command_t* cmd = (command_t*)malloc(sizeof(command_t));
    cmd->type = type;
    cmd->handler = handler;
    cmd->next = consoleCtrl.commands;
    consoleCtrl.commands = cmd;
    return cmd;
}

void ConsoleCtrl_UpdateIntField(uint16_t type, int32_t value)
{
    uint8_t buf[6];
    ENCODE_16(type,buf,0);
    ENCODE_32(value,buf,2);
    ConsoleDrv_SendPacket(Console_IntFieldReceived, 6, buf);
}

void ConsoleCtrl_UpdateTextField(uint16_t type, uint8_t length, uint8_t* data)
{
    if (length > MAX_PACKET_LENGTH-6) return;
    uint8_t buf[length+2];
    ENCODE_16(type,buf,0);
    for (int i=0;i<length;i++) buf[i+2] = data[i];
    ConsoleDrv_SendPacket(Console_TextFieldReceived, length+2, buf);
}

void ConsoleCtrl_ProcessCommand(uint16_t type, uint8_t length, uint8_t* data)
{
	command_t* it = consoleCtrl.commands;
	while (it!=NULL)
	{
		if (it->type==type)
		{
			(*it->handler)(type, length,data);
			break;
		}
		it=it->next;
	}
}

bool ConsoleCtrl_SendOSC(uint8_t channel, uint8_t length, uint16_t index, const uint8_t* data)
{
    uint8_t buf[256];
    buf[0] = channel;
    buf[1] = (index >> 8) & 0xFF;
    buf[2] = index & 0xFF;

    for (int i =0;i < length;i++)
    {
        buf[i+3] = data[i];
    }

    return ConsoleDrv_SendPacket(Console_Oscilloscope, length+3, buf);
}

void ConsoleCtrl_Plot(uint8_t channel, int32_t value)
{
    if (channel>7) return;
    consoleCtrl.plotReg |= (1<<channel);
    consoleCtrl.plotValues[channel] = value;
}

M_TASK_HANDLER(ConsoleCtrl_Plot)
{
    if (consoleCtrl.plotReg == 0) return;
    uint16_t plotMask = 0;
    uint8_t buf[34];
    uint8_t index = 2;

    for (int i=0;i<8;i++)
    {
    	if (((consoleCtrl.plotReg>>i) & 0x01) == 0) continue;
        int32_t value = consoleCtrl.plotValues[i];
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

    ConsoleDrv_SendPacket(Console_PlotReceived, index, buf);
}

