#include <hydra/controller.h>
#include <hydra/driver.h>
#include <hydra/utils.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

void hydra::Controller::init()
{
    Driver::instance().init();
    plotTimer_.start(TIME_PLOT);
}

void hydra::Controller::print(uint16_t level ,const char* text)
{
    int l = strlen(text);
    Driver::instance().sendPacket(level, l, (uint8_t*)text);
}

bool hydra::Controller::sendOSC(uint8_t channel, uint8_t length, uint16_t index, const uint8_t* data)
{
	if(length > 125) return false;
    uint8_t buf[128];
    buf[0] = channel;
    buf[1] = (index >> 8) & 0xFF;
    buf[2] = index & 0xFF;

    for (int i =0;i < length;i++)
    {
        buf[i+3] = data[i];
    }
    Driver::instance().sendPacket(Controller::Oscilloscope, length+3, buf);
    return true;
}

void hydra::Controller::printf(uint16_t level ,const char* format, ...)
{
    char buf[256];
    va_list args;
    va_start (args, format);
    int l = vsprintf(buf, format ,args);
    va_end(args);
    Driver::instance().sendPacket(level, l, (uint8_t*)buf);
}

hydra::Command* hydra::Controller::registerCommand(uint16_t type, Handler handler)
{
    Command* cmd = new Command();
    cmd->type = type;
    cmd->handler = handler;
    cmd->next = commands_;
    commands_ = cmd;
    return cmd;
}

void hydra::Controller::updateIntField(uint16_t command, int32_t value)
{
    uint8_t buf[6];
    ENCODE_16(command,buf,0);
    ENCODE_32(value,buf,2);
    Driver::instance().sendPacket(Controller::IntFieldReceived, 6, buf);
}

void hydra::Controller::processCommand(uint16_t type, uint8_t length, uint8_t* data)
{
	Command* it = commands_;
	while (it!=nullptr)
	{
		if (it->type==type)
		{
			(*it->handler)(type, length,data);
			break;
		}
		it=it->next;
	}
}

void hydra::Controller::updateTextField(uint16_t command, char* data)
{
    int8_t length = strlen(data);
    updateTextField(command, length, (uint8_t*)data);
}

void hydra::Controller::updateTextField(uint16_t command, uint8_t length, uint8_t* data)
{
    if (length > MAX_PACKET_LENGTH-6) return;
    uint8_t buf[length+2];
    ENCODE_16(command,buf,0);
    for (int i=0;i<length;i++) buf[i+2] = data[i];
    Driver::instance().sendPacket(Controller::TextFieldReceived, length+2, buf);
}
