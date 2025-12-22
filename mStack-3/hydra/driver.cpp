#include <hydra/controller.h>
#include <hydra/driver.h>
#include <hydra/hal.h>

void hydra::Driver::init() {
	hydra::HAL::init();
}

void hydra::Driver::ReceiveHeader_(uint8_t data) {
	if (data == HEADER_INDICATOR)
		rxState_ = &Driver::ReceiveLength_;
}

void hydra::Driver::ReceiveLength_(uint8_t data) {
	rxLength_ = data;
	if (rxLength_ < MAX_PACKET_LENGTH) {
		rxIndex_ = 2;
		rxType_ = 0;
		checksum_ = rxLength_;
		rxState_ = &Driver::ReceiveType_;
	} else
		rxState_ = &Driver::ReceiveHeader_;
}

void hydra::Driver::ReceiveType_(uint8_t data) {
	checksum_ += data;
	rxType_ <<= 8;
	rxType_ += data;
	if (--rxIndex_ == 0) {
		if (rxLength_ > 0)
			rxState_ = &Driver::ReceiveData_;
		else
			rxState_ = &Driver::ReceiveChecksum_;
	}
}

void hydra::Driver::ReceiveData_(uint8_t data) {
	checksum_ += data;
	rxBuffer_[rxIndex_++] = data;
	if (rxIndex_ == rxLength_)
		rxState_ = &Driver::ReceiveChecksum_;
}
void hydra::Driver::ReceiveChecksum_(uint8_t data) {
	if (data == checksum_)
		rxState_ = &Driver::ReceiveFooter_;
	else
		rxState_ = &Driver::ReceiveHeader_;
}

void hydra::Driver::ReceiveFooter_(uint8_t data) {
	if (data == FOOTER_INDICATOR) {
		Controller::instance().processCommand(rxType_, rxLength_, rxBuffer_);
	}
	rxState_ = &Driver::ReceiveHeader_;
}

#ifdef USING_DMA
M_EVENT_HANDLER(hydra::Driver, receive, uint16_t) {
	if (event == MAX_PACKET_LENGTH)
	{
		const char *text = "HYDRA OVER RX";
		int l = strlen(text) + 1;
		sendPacket(Controller::CommandType::LogCritical, l, (uint8_t*) text);
	}
	for (uint16_t i = 0; i < (event & 0xFF); i++)
	{
		uint8_t c = rxBufferDma_[((event >> 8) & 0xFF) + i];
		(this->*rxState_)(c);
	}
}

M_EVENT_HANDLER(hydra::Driver, send) {
    if (dmaChunkLen_ != 0) {
        // move tail forward by dmaChunkLen_
        txTail_ = (txTail_ + dmaChunkLen_) % TX_BUF_SIZE;
        dmaChunkLen_ = 0;
    }

    // If there's more data, start next chunk
    if (txHead_ == txTail_) {
        // buffer empty
        sending_ = false;
        return;
    }

    // start next chunk (will check if DMA stream is free)
    transferDma_();
}

bool hydra::Driver::sendPacket(uint16_t type, uint8_t length, const uint8_t *data) {
    CRITICAL_SECTION;

    // compute free space in circular buffer
    // free = (tail - head - 1 + SIZE) % SIZE  (we leave 1 byte free to disambiguate full/empty)
    uint16_t free_space = (txTail_ + TX_BUF_SIZE - txHead_ - 1) % TX_BUF_SIZE;
    uint16_t required = (uint16_t)length + 6; // HEADER + LEN + TYPE(2) + data + checksum + FOOTER

    if (free_space < required) {
    	++statsDrops;
        return false; // not enough space
    }

    uint8_t checksum = 0;

    // lambda to push a byte into circular buffer
    auto push_byte = [&](uint8_t b) {
        txBuf_[txHead_] = b;
        txHead_ = (txHead_ + 1) % TX_BUF_SIZE;
    };

    // Build packet in circular buffer
    push_byte(HEADER_INDICATOR);
    push_byte(length); checksum += length;
    uint8_t type_hi = static_cast<uint8_t>(type >> 8);
    uint8_t type_lo = static_cast<uint8_t>(type & 0xFF);
    push_byte(type_hi); checksum += type_hi;
    push_byte(type_lo); checksum += type_lo;

    for (uint16_t i = 0; i < length; ++i) {
        uint8_t b = data[i];
        push_byte(b);
        checksum += b;
    }

    push_byte(checksum);
    push_byte(FOOTER_INDICATOR);

	uint16_t used = (txHead_ + TX_BUF_SIZE - txTail_) % TX_BUF_SIZE;
	if (used > statsHighWatermark) statsHighWatermark = used;

    if (!sending_) {
        sending_ = true;
        transferDma_();
    }

    return true;
}
#else
M_EVENT_HANDLER(hydra::Driver, receive, uint8_t) {
	(this->*(rxState_))(event);
}

bool hydra::Driver::sendPacket(uint16_t type, uint8_t length,
		const uint8_t *data) {
	uint8_t checksum = 0;
	if (txQueue_.freeSpace() < length + 6)
		return false;
	txQueue_.push(HEADER_INDICATOR);
	checksum += (uint8_t) HEADER_INDICATOR;
	txQueue_.push(length);
	checksum += length;
	txQueue_.push((type >> 8) & 0xFF);
	checksum += (uint8_t) ((type >> 8) & 0xFF);
	txQueue_.push(type & 0xFF);
	checksum += (uint8_t) (type & 0xFF);

	for (int i = 0; i < length; i++) {
		txQueue_.push(data[i]);
		checksum += data[i];
	}
	txQueue_.push(checksum);
	txQueue_.push(FOOTER_INDICATOR);

	if (!sending_) {
		sendEvent.post();
		sending_ = true;
	}

	return true;
}

M_EVENT_HANDLER(hydra::Driver, send) {
	if (txQueue_.empty()) {
		sending_ = false;
		return;
	}
	if (hydra::HAL::txReady()) {
		hydra::HAL::write(txQueue_.pop());
	}
	sendEvent.post();
}
#endif
