#include "driver.h"
#include "command.h"
#include <core/engine.h>
#include <stdint.h>
#include <string.h>

hydra_drv_t HydraDrv;

M_EVENT(HydraDrv_Send)
M_EVENT(HydraDrv_Receive)

#if USE_DMA == 1
#include "dma.h"
static inline void transferDma_()
{
    // If DMA stream still enabled, another transfer is ongoing - do nothing
    if (LL_DMA_IsEnabledStream(DMA_MODULE, DMA_TX_CHANNEL)) {
        return;
    }

    // Compute contiguous chunk length from tail
    uint16_t len = 0;
    if (HydraDrv.txTail < HydraDrv.txHead) {
        // contiguous region: tail .. head-1
        len = (uint16_t)(HydraDrv.txHead - HydraDrv.txTail);
    } else {
        // wrapped: send from tail to end of buffer
        len = (uint16_t)(TX_BUF_SIZE - HydraDrv.txTail);
    }

    if (len == 0) {
        // No data (caller should have checked already) -> clear sending_ to be safe
        HydraDrv.sending = false;
        return;
    }

    // Remember how many bytes we just scheduled
    HydraDrv.dmaChunkLen = len;

//		LL_DMA_DisableStream(DMA_MODULE, DMA_TX_CHANNEL);
    // Configure DMA memory pointer and length (peripheral address pre-configured)
    LL_DMA_SetMemoryAddress(DMA_MODULE, DMA_TX_CHANNEL, (uint32_t)&HydraDrv.txBuffer[HydraDrv.txTail]);
    LL_DMA_SetDataLength(DMA_MODULE, DMA_TX_CHANNEL, (uint32_t)HydraDrv.dmaChunkLen);

    // Clear any pending DMA flags here if platform requires (optionally).
    // Start DMA transfer
    LL_DMA_EnableStream(DMA_MODULE, DMA_TX_CHANNEL);
}

// push a byte into circular buffer
static inline void pushByte_(uint8_t b)
{
	HydraDrv.txBuffer[HydraDrv.txHead] = b;
	HydraDrv.txHead = (HydraDrv.txHead + 1) % TX_BUF_SIZE;
};
#endif

void HydraDrv_Init()
{
#if USE_DMA != 1
	HydraDrv.rxChecksum = 0;
	HydraDrv.rxType = 0;
	HydraDrv.rxLength = 0;
	HydraDrv.rxIndex = 0;
	Queue_Init(&HydraDrv.txQueue, HydraDrv.txBuffer, TX_BUF_SIZE);
	M_EVENT_INIT(HydraDrv_ReceiveEvent, sizeof(uint8_t));

#else
    HydraDrv.statsDrops = 0;
    HydraDrv.statsHighWatermark = 0;
	HydraDrv.txHead = 0;
	HydraDrv.txTail = 0;
	HydraDrv.dmaChunkLen = 0;
	HydraDrv.rxType = 0;
	HydraDrv.rxLength = 0;
	HydraDrv.rxChecksum = 0;
	HydraDrv.rxIndex = 0;
	HydraDrv.sending = false;

	M_EVENT_INIT(HydraDrv_ReceiveEvent, sizeof(uint16_t));
#endif
	M_EVENT_INIT(HydraDrv_SendEvent);

	HydraDrv.hydraRx = &HydraDrv_ReceiveHeader;

	HydraHal_Init();
}

bool HydraDrv_SendPacket(uint16_t type, uint8_t length, const uint8_t *data)
{
	CRITICAL_SECTION_BEGIN;
#if USE_DMA == 1

    // compute free space in circular buffer
    // free = (tail - head - 1 + SIZE) % SIZE  (we leave 1 byte free to disambiguate full/empty)
    uint16_t free_space = (HydraDrv.txTail + TX_BUF_SIZE - HydraDrv.txHead - 1) % TX_BUF_SIZE;
    uint16_t required = (uint16_t)length + 6; // HEADER + LEN + TYPE(2) + data + checksum + FOOTER

    if (free_space < required) {
    	++HydraDrv.statsDrops;
        CRITICAL_SECTION_END
        return false; // not enough space
    }

    uint8_t checksum = 0;

    // Build packet in circular buffer
    pushByte_(HEADER_INDICATOR);
    pushByte_(length); checksum += length;
    uint8_t type_hi = (uint8_t)(type >> 8);
    uint8_t type_lo = (uint8_t)(type & 0xFF);
    pushByte_(type_hi); checksum += type_hi;
    pushByte_(type_lo); checksum += type_lo;

    for (uint16_t i = 0; i < length; ++i) {
        uint8_t b = data[i];
        pushByte_(b);
        checksum += b;
    }

    pushByte_(checksum);
    pushByte_(FOOTER_INDICATOR);

	uint16_t used = (HydraDrv.txHead + TX_BUF_SIZE - HydraDrv.txTail) % TX_BUF_SIZE;
	if (used > HydraDrv.statsHighWatermark) HydraDrv.statsHighWatermark = used;

    if (!HydraDrv.sending) {
        HydraDrv.sending = true;
        transferDma_();
    }

#else
    if (Queue_CheckNumOfFree(&HydraDrv.txQueue) < length + 6)
	{
    	CRITICAL_SECTION_END
		return false;
	}
    uint8_t checksum = 0;
    Queue_Push(&HydraDrv.txQueue, HEADER_INDICATOR);
    Queue_Push(&HydraDrv.txQueue, length);
    checksum += length;
    Queue_Push(&HydraDrv.txQueue, (type >> 8) & 0xFF);
    checksum += (uint8_t) ((type >> 8) & 0xFF);
    Queue_Push(&HydraDrv.txQueue, type & 0xFF);
    checksum += (uint8_t) (type & 0xFF);

    for (int i = 0;i < length;i++)
    {
    	Queue_Push(&HydraDrv.txQueue, data[i]);
        checksum += data[i];
    }
    Queue_Push(&HydraDrv.txQueue, checksum);
    Queue_Push(&HydraDrv.txQueue, FOOTER_INDICATOR);

    if (!HydraDrv.sending)
    {
    	M_EVENT_POST(HydraDrv_SendEvent);
    	HydraDrv.sending = true;
    }

#endif
    CRITICAL_SECTION_END
	return true;
}

M_EVENT_HANDLER(HydraDrv_Send)
{
#if USE_DMA == 1
    if (HydraDrv.dmaChunkLen != 0) {
        // move tail forward by dmaChunkLen_
    	HydraDrv.txTail = (HydraDrv.txTail + HydraDrv.dmaChunkLen) % TX_BUF_SIZE;
    	HydraDrv.dmaChunkLen = 0;
    }

    // If there's more data, start next chunk
    if (HydraDrv.txHead == HydraDrv.txTail) {
        // buffer empty
    	HydraDrv.sending = false;
        return;
    }

    // start next chunk (will check if DMA stream is free)
    transferDma_();
#else
    if (Queue_IsEmpty(&HydraDrv.txQueue))
    {
    	HydraDrv.sending = false;
    	return;
    }

    if (HydraHal_TxReady())
	{
    	uint8_t data;
    	Queue_Pop(&HydraDrv.txQueue, &data);
		HydraHal_Write(data);
	}
    M_EVENT_POST(HydraDrv_SendEvent);
#endif
}

M_EVENT_HANDLER(HydraDrv_Receive)
{
#if USE_DMA == 1
	uint16_t *pkgLen = (uint16_t*) data;
	if ((*pkgLen & 0xFF) == MAX_PACKET_LENGTH)
	{
		const char *text = "HYDRA OVER RX";
		int l = strlen(text) + 1;
		HydraDrv_SendPacket(Hydra_LogCritical, l, (uint8_t*) text);
	}
    for (uint16_t i = 0; i < ((*pkgLen) & 0xFF); i++)
	{
		uint8_t c = HydraDrv.rxBufferDma_[(((*pkgLen) >> 8) & 0xFF) + i];
		(HydraDrv.hydraRx)(c);
	}
#else
	uint8_t* data_ = (uint8_t*)data;
	HydraDrv.hydraRx(*data_);
#endif
}
