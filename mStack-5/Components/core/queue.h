#ifndef CORE_QUEUE_H
#define CORE_QUEUE_H

#include <cstdint>
#include <cassert>
#include <main.h>
#include <telematry/telemetry.h>

namespace core
{

    template <typename T>
    class Queue
    {
    public:
        Queue(T *buf, uint16_t size)
            : buf_(buf),
              size_(size),
              mask_(size - 1)
        {
            // enforce power-of-2 runtime:
            assert((size & (size - 1)) == 0); // size must be power of 2
        }

        /**
         * @brief SPSC Reserve (For Strand)
         * Only one producer allowed. No LDREX/STREX needed.
         */
        inline T *reserve()
        {
            uint32_t h = __atomic_load_n(&head_, __ATOMIC_RELAXED);
            uint32_t t = __atomic_load_n(&tail_, __ATOMIC_ACQUIRE);

            if ((h - t) >= size_) return nullptr;

            uint32_t used = h - t;
            // Update peakUsed using Relaxed atomic
            uint32_t currentPeak = __atomic_load_n(&peakUsed_, __ATOMIC_RELAXED);
            if (used + 1 > currentPeak) {
                __atomic_store_n(&peakUsed_, used + 1, __ATOMIC_RELAXED);
            }

            return &buf_[h & mask_];
        }
        /**
         * @brief SPSC Commit (For Strand)
         * Makes the reserved slot visible to the Consumer.
         */
        inline void commit()
        {
            // Ensure payload is written to RAM before updating head
            __atomic_store_n(&head_, head_ + 1, __ATOMIC_RELEASE);
        }

        /**
         * @brief MPSC Reserve (For EventQueue)
         * Safe for multiple producers (Interrupts/Threads).
         */
        inline T *reserveAtomic()
        {
            uint32_t oldH, newH;
            uint32_t retry_count = 0;
            for (;;)
            {
                oldH = __LDREXW(&head_);
                uint32_t currentTail = __atomic_load_n(&tail_, __ATOMIC_ACQUIRE);
                uint32_t used = oldH - currentTail;

                if (used >= size_)
                {
                    __CLREX();
                    return nullptr;
                }

                uint32_t currentPeak = __atomic_load_n(&peakUsed_, __ATOMIC_RELAXED);
                if (used + 1 > currentPeak) {
                    __atomic_store_n(&peakUsed_, used + 1, __ATOMIC_RELAXED);
                }

                newH = oldH + 1;
                if (__STREXW(newH, &head_) == 0)
                {
                    break; // Success
                }
                retry_count++;
            }
            __DMB();

            if (retry_count > 0)
            {
                Telemetry::log(TelemetryType::QUEUE_CONTENTION, (uint16_t)retry_count);
            }

            return &buf_[oldH & mask_];
        }

        inline T *peekTail()
        {
            // Load head with Acquire to see latest updates from Producer
            uint32_t h = __atomic_load_n(&head_, __ATOMIC_ACQUIRE);
            if (h == tail_)
                return nullptr;
            return &buf_[tail_ & mask_];
        }

        inline void pop()
        {
            // Ensure Consumer finished reading before releasing slot
            __atomic_store_n(&tail_, tail_ + 1, __ATOMIC_RELEASE);
        }

        inline bool empty() const { 
            return __atomic_load_n(&head_, __ATOMIC_RELAXED) == __atomic_load_n(&tail_, __ATOMIC_RELAXED); 
        }

        inline uint32_t used() const { 
            return (__atomic_load_n(&head_, __ATOMIC_RELAXED) - __atomic_load_n(&tail_, __ATOMIC_RELAXED)); 
        }

        inline uint32_t peakUsed() const { 
            return __atomic_load_n(&peakUsed_, __ATOMIC_RELAXED); 
        }

        inline void reset() {
            __atomic_store_n(&head_, 0, __ATOMIC_RELAXED);
            __atomic_store_n(&tail_, 0, __ATOMIC_RELAXED);
        }

        inline void resetPeak() {
            __atomic_store_n(&peakUsed_, 0, __ATOMIC_RELAXED);
        }

        inline uint32_t getHead() const { return __atomic_load_n(&head_, __ATOMIC_RELAXED); }
        inline uint32_t getTail() const { return __atomic_load_n(&tail_, __ATOMIC_RELAXED); }

        inline void decreasePeakOne()
        {
            uint32_t currentPeak = __atomic_load_n(&peakUsed_, __ATOMIC_RELAXED);
            if (currentPeak > 0)
            {
                __atomic_store_n(&peakUsed_, currentPeak - 1, __ATOMIC_RELAXED);
            }
        }

    private:
        T *const buf_;
        // Move the head_ and tail_ values ​​far apart so they don't share a single cache line (32 bytes/ M7, R5)
        // This is extremely important when we later run Core 0 post and Core 1 execute.
        alignas(32) uint32_t head_ = 0;
        alignas(32) uint32_t tail_ = 0;
        uint32_t peakUsed_ = 0; 
        const uint32_t size_;
        const uint32_t mask_;
    };
}

#define QUEUE_DEF(name, size, type) \
    type name##QueueBuffer_[size];  \
    core::Queue<type> name##_ = core::Queue<type>(name##QueueBuffer_, size);

#endif // QUEUE_H
