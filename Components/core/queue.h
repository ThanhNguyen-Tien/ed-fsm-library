#ifndef CORE_QUEUE_H
#define CORE_QUEUE_H

#include <cstdint>

namespace core {

template<typename T>
class Queue {
public:
	Queue(T *buf, uint16_t size) {
		first_ = buf;
		size_ = size;
		last_ = first_ + size;
		inPtr_ = first_;
		outPtr_ = first_;
	}

	inline uint16_t freeSpace() const {
		uint16_t ret = size_ + (outPtr_ - inPtr_);
		if (ret > size_) {
			ret -= size_;
		}
		return ret - 1;
	}

	bool notEmpty() const {
		return (inPtr_ != outPtr_);
	}
	bool empty() const {
		return (inPtr_ == outPtr_);
	}

	void reset() {
		inPtr_ = first_;
		outPtr_ = first_;
	}

	void push(const T &val) {
		T *next = inPtr_ + 1;
		if (next == last_) {
			next = first_;
		}
		if (next != outPtr_) // queue not full
				{
			*inPtr_ = val;
			inPtr_ = next;
		}
	}

	T pop() {
		if (outPtr_ != inPtr_) {
			T ret = *outPtr_;
			outPtr_++;
			if (outPtr_ == last_)
				outPtr_ = first_;
			return ret;
		}
		return T { }; // default value if empty
	}

private:
	uint16_t size_;
	T *first_;
	T *last_;
	T *inPtr_;
	T *outPtr_;
};
}

#define QUEUE_DEF(name, size, type)\
type name##QueueBuffer_[size];\
core::Queue<type> name##_ = core::Queue<type>(name##QueueBuffer_, size);

#endif // QUEUE_H
