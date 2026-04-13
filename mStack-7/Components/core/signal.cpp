#include <core/signal.h>
#include <core/mem-pool.h>

#define MAX_CONNECTIONS 128UL

namespace core
{
    static constexpr uint32_t size = MemPool<SignalNode>::STRIDE * MAX_CONNECTIONS;
    alignas(4) static uint8_t defaultSignalBuf[size];

    // Default settings for SignalMany if no private pool is specified.
    MemPool<SignalNode> defaultSignalPool(defaultSignalBuf, MAX_CONNECTIONS);
}
