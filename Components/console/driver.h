#ifndef CONSOLE_DRIVER_H_
#define CONSOLE_DRIVER_H_

#include <core/queue.h>
#include <core/event.h>
#include <console/hal.h>
#include <console/define.h>
#include <stdbool.h>

M_EVENT_DEF(ConsoleDrv_Send)
M_EVENT_DEF(ConsoleDrv_Receive)

typedef void (*ConsoleRx)(uint8_t);
typedef struct ConsoleDriver
{
	ConsoleRx consoleRx;
	uint16_t rxType;

#if USE_DMA == 1
	uint16_t txMinFree;
    uint8_t txBuffer[TX_BUF_SIZE];
    uint8_t txBufferDMA[TX_BUF_SIZE];
    uint8_t* txFirst;
    uint8_t* txLast;
    uint8_t* txIndex;
#else
    queue_t txQueue;
    uint8_t txBuffer[TX_BUF_SIZE];
#endif
    uint8_t rxBuffer[MAX_PACKET_LENGTH];
    uint8_t rxIndex;
    uint8_t rxLength, rxChecksum;
    bool sending;
}console_drv_t;
extern console_drv_t ConsoleDrv;

void ConsoleDrv_ReceiveHeader(uint8_t data);
void ConsoleDrv_ReceiveLength(uint8_t data);
void ConsoleDrv_ReceiveType(uint8_t data);
void ConsoleDrv_ReceiveData(uint8_t data);
void ConsoleDrv_ReceiveChecksum(uint8_t data);
void ConsoleDrv_ReceiveFooter(uint8_t data);

void ConsoleDrv_Init();
bool ConsoleDrv_SendPacket(uint16_t type, uint8_t length, const uint8_t* data);

#if USE_DMA == 1
#include "dma.h"
void ConsoleDrv_HandleRxPackage(uint8_t num_of_pkg);
void ConsoleDrv_TransferDma();
#endif

#endif /* CONSOLE_DRIVER_H_ */
