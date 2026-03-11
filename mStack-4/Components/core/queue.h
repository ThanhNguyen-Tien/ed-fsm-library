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

        inline T* reserveAtomic() {
            uint32_t oldHead, newHead;
            uint32_t retry_count = 0;

            for (;;) {
                oldHead = __LDREXW(&head_);
                uint32_t currentUsed = oldHead - tail_;
                if (currentUsed >= size_) {
                    __CLREX();
                    return nullptr;
                }

                if (currentUsed + 1 > maxUsed_) maxUsed_ = currentUsed + 1;

                newHead = oldHead + 1;
                if (__STREXW(newHead, &head_) == 0) {
                    break; // Thành công
                }
                retry_count++; // Bắt được tranh chấp tại Queue!
            }
            __DMB();

            if (retry_count > 0) {
                Telemetry::log(TelemetryType::QUEUE_CONTENTION, (uint16_t)retry_count);
            }
            return &buf_[oldHead & mask_];
        }

        inline T *peekTail()
        {
            if (head_ == tail_)
                return nullptr;
            return &buf_[tail_ & mask_];
        }

        inline void pop()
        {
            __DMB();
            tail_ = tail_ + 1;
        }

        inline bool empty() const { return head_ == tail_; }
        inline uint32_t used() const { return (head_ - tail_); }
        inline uint32_t getHead() const { return head_; }
        inline uint32_t getTail() const { return tail_; }
        inline uint32_t peakUsed() const { return maxUsed_; }

        void reset()
        {
            head_ = 0;
            tail_ = 0;
        }

        void resetPeak()
        {
            maxUsed_ = 0;
        }

    private:
        T *const buf_;
        //	alignas(32) volatile uint32_t head_ = 0;	// align to cache line to avoid false sharing between head and tail, M7 has 32-byte cache line size
        //	alignas(32) volatile uint32_t tail_ = 0;
        volatile uint32_t head_ = 0;
        volatile uint32_t tail_ = 0;
        volatile uint32_t maxUsed_ = 0;
        const uint32_t size_;
        const uint32_t mask_;
    };
}

#define QUEUE_DEF(name, size, type) \
    type name##QueueBuffer_[size];  \
    core::Queue<type> name##_ = core::Queue<type>(name##QueueBuffer_, size);

#endif // QUEUE_H
