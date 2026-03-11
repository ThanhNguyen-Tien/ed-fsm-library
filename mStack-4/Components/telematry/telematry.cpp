#include "telemetry.h"

namespace core {
    LogEntry Telemetry::logs_[Telemetry::LOG_SIZE];
    volatile uint32_t Telemetry::writeIdx_ = 0;
}
