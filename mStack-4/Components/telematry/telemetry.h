/*
 * telemetry.h
 *
 *  Created on: Mar 9, 2026
 *      Author: Admin
 */

#ifndef CORE_TELEMETRY_H_
#define CORE_TELEMETRY_H_

#include <cstdint>
#include <main.h>

namespace core {
    enum class TelemetryType : uint16_t {
    	NONE = 0,
        EV_QUEUE_FULL,
		STRAND_QUEUE_FULL,
        QUEUE_CONTENTION,
        LOCK_FREE_YIELD,
        MEMPOOL_ALLOC_FAIL,
		MEMPOOL_ALLOC_CONTENTION,
		MEMPOOL_FREE_CONTENTION,
		CHECKSUM_ERR
    };

    struct LogEntry {
        uint32_t timestamp; // DWT->CYCCNT
        TelemetryType type;
        uint16_t data;      // Mã lỗi hoặc ID event
    };

    class Telemetry {
    public:
        static void log(TelemetryType type, uint16_t data = 0) {
            uint32_t rawIdx = __atomic_fetch_add(&writeIdx_, 1, __ATOMIC_SEQ_CST);
            uint32_t idx = rawIdx & (LOG_SIZE - 1);

            logs_[idx] = { DWT->CYCCNT, type, data };
        }

        // Dùng Debugger để xem mảng logs_ này
        static const uint16_t LOG_SIZE = 128;
        static LogEntry logs_[LOG_SIZE];
        static volatile uint32_t writeIdx_;
    };
}


#endif /* CORE_TELEMETRY_H_ */
