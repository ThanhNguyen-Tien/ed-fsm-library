#include <console/driver.h>
#include <console/command.h>
#include <core/engine.h>
#include <string.h>

console_drv_t ConsoleDrv;

M_EVENT(ConsoleDrv_Send)
M_EVENT(ConsoleDrv_Receive)

void ConsoleDrv_Init()
{
#if USE_DMA != 1
	ConsoleDrv.rxChecksum = 0;
	ConsoleDrv.rxType = 0;
	ConsoleDrv.rxLength = 0;
	ConsoleDrv.rxIndex = 0;
	Queue_Init(&ConsoleDrv.txQueue, ConsoleDrv.txBuffer, TX_BUF_SIZE);
	M_EVENT_INIT(ConsoleDrv_Receive, sizeof(uint8_t));

#else
	ConsoleDrv.rxType = 0;
	ConsoleDrv.txMinFree = TX_BUF_SIZE;
	ConsoleDrv.rxLength = 0;
	ConsoleDrv.rxChecksum = 0;
	ConsoleDrv.rxIndex = 0;
	ConsoleDrv.txFirst = ConsoleDrv.txBuffer;
	ConsoleDrv.txLast = ConsoleDrv.txBuffer + TX_BUF_SIZE;
	ConsoleDrv.txIndex = ConsoleDrv.txFirst;
	ConsoleDrv.sending = 0;

	M_EVENT_INIT(ConsoleDrv_Receive, sizeof(uint16_t));
#endif
	M_EVENT_INIT(ConsoleDrv_Send);

	ConsoleDrv.consoleRx = &ConsoleDrv_ReceiveHeader;

	ConsoleHal_Init();
}

bool ConsoleDrv_SendPacket(uint16_t type, uint8_t length, const uint8_t* data)
{
#if USE_DMA == 1
    uint8_t checksum = 0u;
    uint16_t avail = ConsoleDrv.txLast - ConsoleDrv.txIndex + 1;
    if(avail < ConsoleDrv.txMinFree) ConsoleDrv.txMinFree = avail;
    if (avail < length + 6)
	{
    	return false;
	}
    (*ConsoleDrv.txIndex) = HEADER_INDICATOR; ConsoleDrv.txIndex++;
    checksum += (uint8_t) HEADER_INDICATOR;
    (*ConsoleDrv.txIndex) = length; ConsoleDrv.txIndex++;
    checksum += length;
    (*ConsoleDrv.txIndex) = (type >> 8) & 0xFF; ConsoleDrv.txIndex++;
    checksum += (uint8_t) ((type >> 8) & 0xFF);
    (*ConsoleDrv.txIndex) = type & 0xFF; ConsoleDrv.txIndex++;
    checksum += (uint8_t) (type & 0xFF);

    for (int i = 0;i < length;i++)
    {
    	(*ConsoleDrv.txIndex) = data[i]; ConsoleDrv.txIndex++;
        checksum += data[i];
    }
    (*ConsoleDrv.txIndex) = checksum; ConsoleDrv.txIndex++;
    (*ConsoleDrv.txIndex) = FOOTER_INDICATOR; ConsoleDrv.txIndex++;

    if(!ConsoleDrv.sending)
    {
    	ConsoleDrv.sending = true;
    	ConsoleDrv_TransferDma();
    }

#else
    uint8_t checksum = 0;
    if (Queue_CheckNumOfFree(&ConsoleDrv.txQueue) < length + 5) return false;
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

#endif

	return true;
}

#if USE_DMA == 1
void ConsoleDrv_TransferDma()
{
	uint16_t len = ConsoleDrv.txIndex - ConsoleDrv.txFirst;
	memcpy(ConsoleDrv.txBufferDMA, ConsoleDrv.txBuffer, len);
    LL_DMA_SetDataLength(DMA_MODULE, DMA_TX_CHANNEL, len);
    LL_DMA_EnableChannel(DMA_MODULE, DMA_TX_CHANNEL);
    ConsoleDrv.txIndex = ConsoleDrv.txFirst;
}
#endif

M_EVENT_HANDLER(ConsoleDrv_Send)
{
#if USE_DMA == 1
	if(ConsoleDrv.txIndex == ConsoleDrv.txFirst){ConsoleDrv.sending = false; return;}
	ConsoleDrv_TransferDma();
#else
    if (Queue_IsEmpty(&ConsoleDrv.txQueue)){ConsoleDrv.sending = false; return;}
    if (ConsoleHal_TxReady())
	{
    	uint8_t data;
    	Queue_Pop(&ConsoleDrv.txQueue, &data);
		ConsoleHal_Write(data);
	}
    M_EVENT_POST(ConsoleDrv_Send);
#endif
}

M_EVENT_HANDLER(ConsoleDrv_Receive)
{
#if USE_DMA == 1
	uint16_t* pkgLen = (uint16_t*)data;
	if(*pkgLen == MAX_PACKET_LENGTH)
	{
		const char* text = "CONSOLE OVER RX";
	    int l = strlen(text) + 1;
	    ConsoleDrv_SendPacket(Console_MessageReceived, l, (uint8_t*)text);
	}
	for(uint16_t i = 0; i < *pkgLen; i++)
	{
		ConsoleDrv.consoleRx(ConsoleDrv.rxBuffer[i]);
	}
#else
	uint8_t* data_ = (uint8_t*)data;
	ConsoleDrv.consoleRx(*data_);
#endif
}
