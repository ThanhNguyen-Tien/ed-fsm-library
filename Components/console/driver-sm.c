#include <console/driver.h>
#include <console/controller.h>

void ConsoleDrv_ReceiveHeader(uint8_t data)
{
    if (data == HEADER_INDICATOR) ConsoleDrv.consoleRx = &ConsoleDrv_ReceiveLength;
}

void ConsoleDrv_ReceiveLength(uint8_t data)
{
    ConsoleDrv.rxLength = data;
    if (ConsoleDrv.rxLength < MAX_PACKET_LENGTH)
    {
    	ConsoleDrv.rxIndex = 2;
    	ConsoleDrv.rxType = 0;
    	ConsoleDrv.rxChecksum = HEADER_INDICATOR + ConsoleDrv.rxLength;
    	ConsoleDrv.consoleRx = &ConsoleDrv_ReceiveType;
    }
    else ConsoleDrv.consoleRx = &ConsoleDrv_ReceiveHeader;
}

void ConsoleDrv_ReceiveType(uint8_t data)
{
	ConsoleDrv.rxChecksum += data;
	ConsoleDrv.rxType <<= 8;
	ConsoleDrv.rxType+= data;
    if (--ConsoleDrv.rxIndex == 0)
    {
        if (ConsoleDrv.rxLength > 0)ConsoleDrv.consoleRx = &ConsoleDrv_ReceiveData;
        else ConsoleDrv.consoleRx = &ConsoleDrv_ReceiveChecksum;
    }
}

void ConsoleDrv_ReceiveData(uint8_t data)
{
	ConsoleDrv.rxChecksum += data;
	ConsoleDrv.rxBuffer[ConsoleDrv.rxIndex++] = data;
    if (ConsoleDrv.rxIndex == ConsoleDrv.rxLength) ConsoleDrv.consoleRx = &ConsoleDrv_ReceiveChecksum;
}

void ConsoleDrv_ReceiveChecksum(uint8_t data)
{
    if (data == ConsoleDrv.rxChecksum) ConsoleDrv.consoleRx = &ConsoleDrv_ReceiveFooter;
    else ConsoleDrv.consoleRx = &ConsoleDrv_ReceiveHeader;
}

void ConsoleDrv_ReceiveFooter(uint8_t data)
{
    if (data == FOOTER_INDICATOR)
    {
        ConsoleCtrl_ProcessCommand(ConsoleDrv.rxType, ConsoleDrv.rxLength, ConsoleDrv.rxBuffer);
    }
    ConsoleDrv.consoleRx = &ConsoleDrv_ReceiveHeader;
}
