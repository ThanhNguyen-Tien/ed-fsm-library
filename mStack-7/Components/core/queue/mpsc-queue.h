#ifndef CORE_QUEUE_MPSC_QUEUE_H_
#define CORE_QUEUE_MPSC_QUEUE_H_

#include "queue.h"

template <uint16_t S>
class C_MpscQueue : public QueueBase<uint32_t, S> {
public:
    bool reserveAtomic(uint8_t num, uint32_t& startHead)
    {
        uint32_t curr, next;
        retry_ = 0;
        while (true)
        {
            curr = __LDREXW(&this->head_);
            if (static_cast<uint16_t>(curr + num - this->tail_) > S)
            {
                __CLREX();
                return false;
            }
            next = curr + num;
            if (__STREXW(next, &this->head_) == 0)
            {
                startHead = curr;
                uint16_t usedSize = static_cast<uint16_t>(next - this->tail_);
                if (usedSize > this->maxUsed_) this->maxUsed_ = usedSize;
                return true;
            }
            retry_++;
        }
        //TODO: Add telemetry for retry count if needed
    }

    uint32_t* getBuffer() { return this->buffer; }
protected:
    uint8_t retry_ {0U};
};

class I_MpscQueue {
public:
    virtual uint32_t* getBuffer() = 0;
    virtual void clearSlot(uint32_t idx) = 0;
    virtual void updateTail(uint8_t steps) = 0;
    virtual uint32_t getHead() = 0;
    virtual uint32_t getTail() = 0;
    virtual uint32_t getMask() const = 0;
    virtual bool reserveChunks(uint8_t num, uint32_t& start) = 0;
    virtual bool isEmpty_() = 0;
};

template <uint16_t S>
class C_PriorityQueue : public C_MpscQueue<S>, public I_MpscQueue {
public:
    uint32_t* getBuffer() override { return this->buffer_; }
    void clearSlot(uint32_t idx) override { this->buffer_[idx & (S - 1)] = 0; }
    void updateTail(uint8_t steps) override { this->tail_ += steps; }
    uint32_t getHead() override { return this->head_; }
    uint32_t getTail() override { return this->tail_; }
    uint32_t getMask() const override { return this->MASK; }
    bool reserveChunks(uint8_t num, uint32_t& start) override {
        return C_MpscQueue<S>::reserveAtomic(num, start);
    }
    bool isEmpty_() override { return this->isEmpty(); }
};

#endif /* CORE_QUEUE_MPSC_QUEUE_H_ */
