#ifndef CORE_QUEUE_QUEUE_H_
#define CORE_QUEUE_QUEUE_H_

#include <cstdint>

template <typename T, uint16_t N>
class QueueBase
{
    static_assert((N > 0) && ((N & (N - 1)) == 0), "N must be a power of 2!");

public:
	inline bool isEmpty() const
	{
		return head_ == tail_;
	}

	inline uint32_t used() const
	{
		return (head_ - tail_);
	}

	inline uint32_t freeSpace() const
	{
		return N - used();
	}

	inline uint32_t peakUsed() const
	{
		return maxUsed_;
	}

	void reset() {
		head_ = 0;
		tail_ = 0;
		maxUsed_ = 0;
	}

protected:
    T buffer_[N];
    static constexpr uint16_t MASK = N - 1;

    QueueBase() : head_(0), tail_(0) {}

    volatile uint32_t head_;
    volatile uint32_t tail_;
	volatile uint32_t maxUsed_ = 0;
};

#endif /* CORE_QUEUE_QUEUE_H_ */
