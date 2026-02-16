#ifndef HYDRA_DRIVER_H_
#define HYDRA_DRIVER_H_
#include <hydra/define.h>
#include <core/event.h>
#include <core/queue.h>
#include <string.h>

#ifdef USING_DMA
COMPONENT(hydra, Driver)
    M_EVENT(send);
	M_EVENT(receive, uint16_t)
public:
	void init();
	bool sendPacket(uint16_t type, uint8_t length, const uint8_t* data);

	uint8_t& txBuf() {return txBuf_[txTail_];}
	uint8_t& rxBuf() {return rxBufferDma_[0];}

    volatile uint32_t statsDrops = 0;
    volatile uint32_t statsHighWatermark = 0;

private:
	typedef void (Driver::*RxState) (uint8_t);
	void ReceiveHeader_(uint8_t data);
	void ReceiveLength_(uint8_t data);
	void ReceiveType_(uint8_t data);
	void ReceiveData_(uint8_t data);
	void ReceiveChecksum_(uint8_t data);
	void ReceiveFooter_(uint8_t data);
	RxState rxState_ = &Driver::ReceiveHeader_;

private:
	inline void transferDma_() {
	    // If DMA stream still enabled, another transfer is ongoing - do nothing
	    if (LL_DMA_IsEnabledStream(DMA_MODULE, DMA_TX_CHANNEL)) {
	        return;
	    }

	    // Compute contiguous chunk length from tail
	    uint16_t len = 0;
	    if (txTail_ < txHead_) {
	        // contiguous region: tail .. head-1
	        len = (uint16_t)(txHead_ - txTail_);
	    } else {
	        // wrapped: send from tail to end of buffer
	        len = (uint16_t)(TX_BUF_SIZE - txTail_);
	    }

	    if (len == 0) {
	        // No data (caller should have checked already) -> clear sending_ to be safe
	        sending_ = false;
	        return;
	    }

	    // Remember how many bytes we just scheduled
	    dmaChunkLen_ = len;

//		LL_DMA_DisableStream(DMA_MODULE, DMA_TX_CHANNEL);
	    // Configure DMA memory pointer and length (peripheral address pre-configured)
	    LL_DMA_SetMemoryAddress(DMA_MODULE, DMA_TX_CHANNEL, (uint32_t)&txBuf_[txTail_]);
	    LL_DMA_SetDataLength(DMA_MODULE, DMA_TX_CHANNEL, (uint32_t)dmaChunkLen_);

	    // Clear any pending DMA flags here if platform requires (optionally).
	    // Start DMA transfer
	    LL_DMA_EnableStream(DMA_MODULE, DMA_TX_CHANNEL);
	}

private:
	uint16_t rxType_;
    volatile uint16_t txHead_ = 0;
    volatile uint16_t txTail_ = 0;
    volatile uint16_t dmaChunkLen_ = 0;

    uint8_t txBuf_[TX_BUF_SIZE];
    static_assert((TX_BUF_SIZE & (TX_BUF_SIZE - 1)) == 0, "TX_BUF_SIZE must be a power of 2");
    static constexpr uint16_t MASK_ = TX_BUF_SIZE - 1;

    uint8_t rxBuffer_[MAX_PACKET_LENGTH];
    uint8_t rxBufferDma_[MAX_PACKET_LENGTH];
    uint8_t rxLength_, checksum_;
    uint8_t rxIndex_;

    volatile bool sending_ = false;
COMPONENT_END
#else
COMPONENT(hydra, Driver)
    M_EVENT(send);
	M_EVENT(receive, uint8_t)
public:
	void init();
    uint16_t getMinAvail(){return txMinAvail_;}
	bool sendPacket(uint16_t type, uint8_t length, const uint8_t* data);

private:
	typedef void (Driver::*RxState) (uint8_t);
	void ReceiveHeader_(uint8_t data);
	void ReceiveLength_(uint8_t data);
	void ReceiveType_(uint8_t data);
	void ReceiveData_(uint8_t data);
	void ReceiveChecksum_(uint8_t data);
	void ReceiveFooter_(uint8_t data);
	RxState rxState_ = &Driver::ReceiveHeader_;

private:
	uint16_t rxType_;
	uint16_t txMinAvail_= TX_BUF_SIZE;

    uint8_t rxBuffer_[MAX_PACKET_LENGTH];
    uint8_t rxLength_, checksum_;
    uint8_t rxIndex_;

    core::Queue<uint8_t> txQueue_ = core::Queue<uint8_t>(txBuffer_, TX_BUF_SIZE);
    uint8_t txBuffer_[TX_BUF_SIZE];

    bool sending_ = false;
COMPONENT_END
#endif
#endif
