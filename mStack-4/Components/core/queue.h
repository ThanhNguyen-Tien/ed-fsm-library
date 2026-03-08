#ifndef CORE_QUEUE_H
#define CORE_QUEUE_H

#include <cstdint>
#include <cassert>

namespace core {

template<typename T>
class Queue {
public:
    Queue(T* buf, uint16_t size)
        : buf_(buf),
          size_(size),
          mask_(size - 1)
    {
        // enforce power-of-2 runtime:
         assert((size & (size - 1)) == 0); // size must be power of 2
    }

    __attribute__((always_inline)) inline T* reserve() {
        uint16_t head = head_;
        uint16_t tail = tail_;

        uint16_t used = (uint16_t)(head - tail);
        if (used == size_)
            return nullptr; // full

        return &buf_[head & mask_];
    }

    __attribute__((always_inline)) inline void commit() {
        uint16_t head = head_;
        uint16_t tail = tail_;

        head_ = head + 1;

        uint16_t used = (uint16_t)(head + 1 - tail);
        if (used > maxUsed_)
            maxUsed_ = used;
    }

    __attribute__((always_inline)) inline bool push(const T& v) {
        T* s = reserve();
        if (!s)
            return false;

        *s = v;
        commit();
        return true;
    }

    __attribute__((always_inline)) inline bool pop(T& out) {
        uint16_t head = head_;
        uint16_t tail = tail_;

        if (head == tail)
            return false;   // empty

        out = buf_[tail & mask_];
        tail_ = tail + 1;
        return true;
    }

    void pop() {
        tail_ = tail_ + 1;
    }

    inline bool empty() const {
        return head_ == tail_;
    }

    inline uint16_t size() const {
        return size_;
    }

    inline uint16_t used() const {
        return (uint16_t)(head_ - tail_);
    }

    inline uint16_t freeSpace() const {
        return size_ - used();
    }

    inline uint16_t peakUsed() const {
		return maxUsed_;
	}

    void reset() {
        head_ = 0;
        tail_ = 0;
    }

    void resetPeak() {
		maxUsed_ = 0;
    }

private:
    T* const buf_;
//	alignas(32) volatile uint32_t head_ = 0;	// align to cache line to avoid false sharing between head and tail, M7 has 32-byte cache line size
//	alignas(32) volatile uint32_t tail_ = 0;
    volatile uint32_t head_ = 0;
	volatile uint32_t tail_ = 0;
    const uint16_t size_;
    const uint16_t mask_;
    volatile uint16_t maxUsed_ = 0;
};
}

#define QUEUE_DEF(name, size, type)\
type name##QueueBuffer_[size];\
core::Queue<type> name##_ = core::Queue<type>(name##QueueBuffer_, size);

#endif // QUEUE_H
