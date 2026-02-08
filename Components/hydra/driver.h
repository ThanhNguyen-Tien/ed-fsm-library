#ifndef HYDRA_DRIVER_H_
#define HYDRA_DRIVER_H_

#include <core/queue.h>
#include <core/event.h>
#include <hydra/hal.h>
#include <hydra/define.h>
#include <stdbool.h>

M_EVENT_DEF(HydraDrv_Send)
M_EVENT_DEF(HydraDrv_Receive)

typedef void (*HydraRx)(uint8_t);
typedef struct HydraDriver
{
	HydraRx hydraRx;
	uint16_t rxType;

#if USE_DMA == 1
    volatile uint32_t statsDrops;
    volatile uint32_t statsHighWatermark;
    uint16_t txHead;
    uint16_t txTail;
    volatile uint16_t dmaChunkLen;
	uint8_t txBuffer[TX_BUF_SIZE];
#else
    queue_t txQueue;
    uint8_t txBuffer[TX_BUF_SIZE];
#endif
	uint8_t rxBuffer[MAX_PACKET_LENGTH];
	uint8_t rxBufferDma_[MAX_PACKET_LENGTH];
	uint8_t rxIndex;
	uint8_t rxLength, rxChecksum;
	volatile bool sending;
} hydra_drv_t;
extern hydra_drv_t HydraDrv;

void HydraDrv_ReceiveHeader(uint8_t data);
void HydraDrv_ReceiveLength(uint8_t data);
void HydraDrv_ReceiveType(uint8_t data);
void HydraDrv_ReceiveData(uint8_t data);
void HydraDrv_ReceiveChecksum(uint8_t data);
void HydraDrv_ReceiveFooter(uint8_t data);

void HydraDrv_Init();
bool HydraDrv_SendPacket(uint16_t type, uint8_t length, const uint8_t *data);

#endif /* HYDRA_DRIVER_H_ */
