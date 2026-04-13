#ifndef CORE_QUEUE_SPSC_QUEUE_H_
#define CORE_QUEUE_SPSC_QUEUE_H_

#include "queue.h"

namespace core {

	template<typename T, uint16_t N>
	class SpscQueue : public QueueBase<T, N> {
	public:
		SpscQueue() {}

		bool push(const T& v)
		{
	        uint32_t currentHead = this->head_;
	        uint32_t nextHead = currentHead + 1;

	        if (static_cast<uint8_t>(nextHead - this->tail_) > N)
	        {
	            return false;
	        }

	        this->buffer_[currentHead & this->MASK] = v;

	        __DMB();
	        this->head_ = nextHead;
	        return true;
		}

		bool pop(T& out)
		{
	        if (this->head_ == this->tail_) return false;

	        out = this->buffer_[this->tail_ & this->MASK];

	        __DMB();
	        this->tail_++;
	        return true;
		}
	};
}

#endif /* CORE_QUEUE_SPSC_QUEUE_H_ */
