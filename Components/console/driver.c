#include "driver.h"
#include "command.h"
#include <core/engine.h>
#include <string.h>

console_drv_t ConsoleDrv;

M_EVENT(ConsoleDrv_Send)
M_EVENT(ConsoleDrv_Receive)

void ConsoleDrv_Init()
{
	ConsoleDrv.rxChecksum = 0;
	ConsoleDrv.rxType = 0;
	ConsoleDrv.rxLength = 0;
	ConsoleDrv.rxIndex = 0;
	Queue_Init(&ConsoleDrv.txQueue, ConsoleDrv.txBuffer, TX_BUF_SIZE);
	M_EVENT_INIT(ConsoleDrv_Receive, sizeof(uint8_t));
	M_EVENT_INIT(ConsoleDrv_Send);

	ConsoleDrv.consoleRx = &ConsoleDrv_ReceiveHeader;

	ConsoleHal_Init();
}

bool ConsoleDrv_SendPacket(uint16_t type, uint8_t length, const uint8_t* data)
{
    uint8_t checksum = 0;
    if (Queue_CheckNumOfFree(&ConsoleDrv.txQueue) < length + 6) return false;
    Queue_Push(&ConsoleDrv.txQueue, HEADER_INDICATOR);
    checksum += (uint8_t) HEADER_INDICATOR;
    Queue_Push(&ConsoleDrv.txQueue, length);
    checksum += length;
    Queue_Push(&ConsoleDrv.txQueue, (type >> 8) & 0xFF);
    checksum += (uint8_t) ((type >> 8) & 0xFF);
    Queue_Push(&ConsoleDrv.txQueue, type & 0xFF);
    checksum += (uint8_t) (type & 0xFF);

    for (int i = 0;i < length;i++)
    {
    	Queue_Push(&ConsoleDrv.txQueue, data[i]);
        checksum += data[i];
    }
    Queue_Push(&ConsoleDrv.txQueue, checksum);
    Queue_Push(&ConsoleDrv.txQueue, FOOTER_INDICATOR);

    if (!ConsoleDrv.sending)
    {
    	M_EVENT_POST(ConsoleDrv_Send);
    	ConsoleDrv.sending = true;
    }

	return true;
}

M_EVENT_HANDLER(ConsoleDrv_Send)
{
    if (Queue_IsEmpty(&ConsoleDrv.txQueue)){ConsoleDrv.sending = false; return;}
    if (ConsoleHal_TxReady())
	{
    	uint8_t data;
    	Queue_Pop(&ConsoleDrv.txQueue, &data);
		ConsoleHal_Write(data);
	}
    M_EVENT_POST(ConsoleDrv_Send);
}

M_EVENT_HANDLER(ConsoleDrv_Receive)
{
	uint8_t* data_ = (uint8_t*)data;
	ConsoleDrv.consoleRx(*data_);
}
