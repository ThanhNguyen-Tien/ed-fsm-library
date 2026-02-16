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

    bool push(const T& v) {
        uint16_t head = head_;
        uint16_t tail = tail_;

        uint16_t used = (uint16_t)(head - tail);
        if (used == size_)
            return false;   // full

        buf_[head & mask_] = v;
        head_ = head + 1;

        // ---- track peak ----
        used++; // after push
        if (used > maxUsed_)
            maxUsed_ = used;

        return true;
    }

    bool pop(T& out) {
        uint16_t head = head_;
        uint16_t tail = tail_;

        if (head == tail)
            return false;   // empty

        out = buf_[tail & mask_];
        tail_ = tail + 1;
        return true;
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
    const uint16_t size_;
    const uint16_t mask_;

    volatile uint16_t head_ = 0;
    volatile uint16_t tail_ = 0;

    volatile uint16_t maxUsed_ = 0;
};
}

#define QUEUE_DEF(name, size, type)\
type name##QueueBuffer_[size];\
core::Queue<type> name##_ = core::Queue<type>(name##QueueBuffer_, size);

#endif // QUEUE_H
