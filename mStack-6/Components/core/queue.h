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
        // Default constructor
        Queue() : buf_(nullptr), head_(0), tail_(0), peakUsed_(0), size_(0), mask_(0) {}

        Queue(T *buf, uint16_t size)
            : buf_(buf),
              size_(size),
              mask_(size - 1)
        {
            // enforce power-of-2 runtime:
            assert((size & (size - 1)) == 0); // size must be power of 2
        }

        void init(T *buf, uint16_t size)
        {
            this->buf_ = buf;
            this->size_ = size;
            this->mask_ = size - 1;
            this->reset();
            assert((size & (size - 1)) == 0);
        }

        /**
         * @brief SPSC Reserve (For Strand)
         * Only one producer allowed. No LDREX/STREX needed.
         */
        inline T *reserve()
        {
            uint32_t h = __atomic_load_n(&this->head_, __ATOMIC_RELAXED);
            uint32_t t = __atomic_load_n(&this->tail_, __ATOMIC_ACQUIRE);

            if ((h - t) >= this->size_)
                return nullptr;

            uint32_t used = h - t;
            // Update peakUsed using Relaxed atomic
            uint32_t currentPeak = __atomic_load_n(&this->peakUsed_, __ATOMIC_RELAXED);
            if (used + 1 > currentPeak)
            {
                __atomic_store_n(&this->peakUsed_, used + 1, __ATOMIC_RELAXED);
            }

            return &this->buf_[h & this->mask_];
        }
        /**
         * @brief SPSC Commit (For Strand)
         * Makes the reserved slot visible to the Consumer.
         */
        inline void commit()
        {
            // Ensure payload is written to RAM before updating head
            __atomic_store_n(&this->head_, this->head_ + 1, __ATOMIC_RELEASE);
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
                oldH = __LDREXW(&this->head_);
                uint32_t currentTail = __atomic_load_n(&this->tail_, __ATOMIC_ACQUIRE);
                uint32_t used = oldH - currentTail;

                if (used >= this->size_)
                {
                    __CLREX();
                    return nullptr;
                }

                uint32_t currentPeak = __atomic_load_n(&this->peakUsed_, __ATOMIC_RELAXED);
                if (used + 1 > currentPeak)
                {
                    __atomic_store_n(&this->peakUsed_, used + 1, __ATOMIC_RELAXED);
                }

                newH = oldH + 1;
                if (__STREXW(newH, &this->head_) == 0)
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

            return &this->buf_[oldH & this->mask_];
        }

        inline T *peekTail()
        {
            // Load head with Acquire to see latest updates from Producer
            uint32_t h = __atomic_load_n(&this->head_, __ATOMIC_ACQUIRE);
            if (h == this->tail_)
                return nullptr;
            return &this->buf_[this->tail_ & this->mask_];
        }

        inline void pop()
        {
            // Ensure Consumer finished reading before releasing slot
            __atomic_store_n(&this->tail_, this->tail_ + 1, __ATOMIC_RELEASE);
        }

        inline bool empty() const
        {
            return __atomic_load_n(&this->head_, __ATOMIC_RELAXED) == __atomic_load_n(&this->tail_, __ATOMIC_RELAXED);
        }

        inline uint32_t used() const
        {
            return (__atomic_load_n(&this->head_, __ATOMIC_RELAXED) - __atomic_load_n(&this->tail_, __ATOMIC_RELAXED));
        }

        inline uint32_t peakUsed() const
        {
            return __atomic_load_n(&this->peakUsed_, __ATOMIC_RELAXED);
        }

        inline void reset()
        {
            __atomic_store_n(&this->head_, 0, __ATOMIC_RELAXED);
            __atomic_store_n(&this->tail_, 0, __ATOMIC_RELAXED);
        }

        inline void resetPeak()
        {
            __atomic_store_n(&this->peakUsed_, 0, __ATOMIC_RELAXED);
        }

        inline uint32_t getHead() const { return __atomic_load_n(&this->head_, __ATOMIC_RELAXED); }
        inline uint32_t getTail() const { return __atomic_load_n(&this->tail_, __ATOMIC_RELAXED); }

        inline void decreasePeakOne()
        {
            uint32_t currentPeak = __atomic_load_n(&this->peakUsed_, __ATOMIC_RELAXED);
            if (currentPeak > 0)
            {
                __atomic_store_n(&this->peakUsed_, currentPeak - 1, __ATOMIC_RELAXED);
            }
        }

    private:
        T *buf_;
        // Move the head_ and tail_ values ​​far apart so they don't share a single cache line (32 bytes/ M7, R5)
        // This is extremely important when we later run Core 0 post and Core 1 execute.
        alignas(32) uint32_t head_ = 0;
        alignas(32) uint32_t tail_ = 0;
        uint32_t peakUsed_ = 0;
        uint32_t size_;
        uint32_t mask_;
    };
}

#define QUEUE_DEF(name, size, type) \
    type name##QueueBuffer_[size];  \
    core::Queue<type> name##_ = core::Queue<type>(name##QueueBuffer_, size);

#endif // QUEUE_H
