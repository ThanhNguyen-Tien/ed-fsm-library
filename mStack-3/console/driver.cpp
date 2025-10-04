#include <console/controller.h>
#include <console/driver.h>
#include <console/hal.h>

void console::Driver::init() {
	console::HAL::init();
}

void console::Driver::ReceiveHeader_(uint8_t data) {
	if (data == HEADER_INDICATOR)
		rxState_ = &Driver::ReceiveLength_;
}

void console::Driver::ReceiveLength_(uint8_t data) {
	rxLength_ = data;
	if (rxLength_ < MAX_PACKET_LENGTH) {
		rxIndex_ = 2;
		rxType_ = 0;
		checksum_ = HEADER_INDICATOR + rxLength_;
		rxState_ = &Driver::ReceiveType_;
	} else
		rxState_ = &Driver::ReceiveHeader_;
}

void console::Driver::ReceiveType_(uint8_t data) {
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

void console::Driver::ReceiveData_(uint8_t data) {
	checksum_ += data;
	rxBuffer_[rxIndex_++] = data;
	if (rxIndex_ == rxLength_)
		rxState_ = &Driver::ReceiveChecksum_;
}
void console::Driver::ReceiveChecksum_(uint8_t data) {
	if (data == checksum_)
		rxState_ = &Driver::ReceiveFooter_;
	else
		rxState_ = &Driver::ReceiveHeader_;
}

void console::Driver::ReceiveFooter_(uint8_t data) {
	if (data == FOOTER_INDICATOR) {
		Controller::instance().processCommand(rxType_, rxLength_, rxBuffer_);
	}
	rxState_ = &Driver::ReceiveHeader_;
}

#ifdef USING_DMA
M_EVENT_HANDLER(console::Driver, receive, uint16_t) {
	if (event == MAX_PACKET_LENGTH)
	{
		const char *text = "CONSOLE OVER RX";
		int l = strlen(text) + 1;
		sendPacket(Controller::MessageReceived, l, (uint8_t*) text);
	}
	for (uint16_t i = 0; i < (event & 0xFF); i++)
	{
		uint8_t c = rxBufferDma_[((event >> 8) & 0xFF) + i];
		(this->*rxState_)(c);
	}
}

M_EVENT_HANDLER(console::Driver, send) {
	if (txIndex_ == txFirst_) {
		sending_ = false;
		return;
	}
	transferDma_();
}

bool console::Driver::sendPacket(uint16_t type, uint8_t length,
		const uint8_t *data) {
	uint8_t checksum = 0;
	uint16_t avail = txLast_ - txIndex_ + 1;
	if (avail < length + 6)
		return false;

	uint8_t *ptr = txIndex_;
	*ptr++ = HEADER_INDICATOR;
	checksum += HEADER_INDICATOR;
	*ptr++ = length;
	checksum += length;
	*ptr++ = (uint8_t) (type >> 8);
	checksum += (uint8_t) (type >> 8);
	*ptr++ = (uint8_t) (type);
	checksum += (uint8_t) (type);

	memcpy(ptr, data, length);
	for (int i = 0; i < length; ++i)
		checksum += data[i];
	ptr += length;

	*ptr++ = checksum;
	*ptr++ = FOOTER_INDICATOR;

	txIndex_ = ptr;
	if (!sending_) {
		sending_ = true;
		transferDma_();
	}

	return true;
}
#else
M_EVENT_HANDLER(console::Driver, receive, uint8_t) {
	(this->*(rxState_))(event);
}

bool console::Driver::sendPacket(uint16_t type, uint8_t length,
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

M_EVENT_HANDLER(console::Driver, send) {
	if (txQueue_.empty()) {
		sending_ = false;
		return;
	}
	if (console::HAL::txReady()) {
		console::HAL::write(txQueue_.pop());
	}
	sendEvent.post();
}
#endif
