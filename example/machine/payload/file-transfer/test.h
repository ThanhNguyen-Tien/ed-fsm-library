#ifndef EXAMPLE_FILE_TRANSFER_TEST_H_
#define EXAMPLE_FILE_TRANSFER_TEST_H_

#include <core/machine.h>
#include <console/controller.h>

#define MAX_LEN	32U

typedef struct FilePayload {
    char filename[MAX_LEN];
    uint32_t blockSize;
    uint32_t totalSize;
}file_pay_t;

PAYLOAD_MACHINE(test, FileTransfer, file_pay_t, 20)

	U_ACTION(100, start)
	U_TEXT(101, filename)
	U_INTEGER(102, blockSize)
	U_INTEGER(103, numOfBlock)

public:
	void init();

private:
	enum class Event : uint8_t {
	    START,
	    NEXT_BLOCK,
	    FINISH,
	};

private:
	STATE_DEF(IDLE)
	STATE_DEF(RECEIVING)

private:// Data Testing
	file_pay_t fake_;
	uint16_t numOfBlock_ = 0;

MACHINE_END

#endif /* EXAMPLE_FILE_TRANSFER_TEST_H_ */
