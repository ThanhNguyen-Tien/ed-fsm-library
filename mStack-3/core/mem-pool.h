/*
 * mem-pool.h
 *
 *  Created on: Jul 25, 2023
 *      Author: MSI
 */

#ifndef CORE_MEM_POOL_H_
#define CORE_MEM_POOL_H_

#include <cstdlib>
#include <cstring>
#include <core/engine.h>
#include "main.h"

namespace core {
template<typename T>
class MemPool {
private:
	struct _Unit {
		struct _Unit *pNext = nullptr;
	};

	void *pMemBlock_ = nullptr;                // Address of memory pool.

	// Manage all unit with two linkedlist.
	struct _Unit *pAllocatedMemBlock_ = nullptr; // Head pointer to Allocated linkedlist.
	struct _Unit *pFreeMemBlock_ = nullptr;  // Head pointer to Free linkedlist.

	uint8_t capacity_ = 0;
	uint8_t used_ = 0;
	uint8_t peakUsed_ = 0;
public:

	MemPool(uint32_t ulUnitNum) :
			pMemBlock_(nullptr), pAllocatedMemBlock_(nullptr), pFreeMemBlock_(
					nullptr), capacity_(ulUnitNum), used_(0), peakUsed_(0) {
		uint32_t size = ulUnitNum * (sizeof(T) + sizeof(struct _Unit));
		pMemBlock_ = malloc(size); // Allocate a memory block.
		if (pMemBlock_) {
			for (unsigned long i = 0; i < ulUnitNum; i++) // Link all mem unit . Create linked list.
					{
				struct _Unit *pCurUnit = (struct _Unit*) ((uint8_t*) pMemBlock_
						+ i * (sizeof(T) + sizeof(struct _Unit)));

				pCurUnit->pNext = pFreeMemBlock_; // Insert the new unit at head.

				pFreeMemBlock_ = pCurUnit;
			}
			Engine::instance().events().addNumOfByteHeap(size);
		} else {
			Error_Handler();
		}
	}

	~MemPool() {
		free(pMemBlock_);
	}

	// Allocate memory unit If memory pool can`t provide proper memory unit,
	// It will call system function.
	inline void* Alloc() {
		if (pFreeMemBlock_ == nullptr)	// Pool Full
				{
			return nullptr;
		}

		struct _Unit *pCurUnit = pFreeMemBlock_;
		pFreeMemBlock_ = pCurUnit->pNext;  // Get a unit from free linkedlist.
		pCurUnit->pNext = pAllocatedMemBlock_;
		pAllocatedMemBlock_ = pCurUnit;

		used_++;
		if (used_ > peakUsed_)
			peakUsed_ = used_;

		return (void*) ((char*) pCurUnit + sizeof(struct _Unit));
	}

	inline void Free(void *p) {
		struct _Unit *pCurUnit = (struct _Unit*) ((char*) p
				- sizeof(struct _Unit));
		struct _Unit *prev = nullptr;
		for (struct _Unit *it = pAllocatedMemBlock_; it != nullptr;
				it = it->pNext) {
			if (it == pCurUnit) {
				if (prev == nullptr)
					pAllocatedMemBlock_ = it->pNext;
				else
					prev->pNext = it->pNext;
				pCurUnit->pNext = pFreeMemBlock_;
				pFreeMemBlock_ = pCurUnit;
				used_--;
				break;
			}
			prev = it;
		}
	}

	inline uint8_t capacity() const {
		return capacity_;
	}
	inline uint8_t used() const {
		return used_;
	}
	inline uint8_t free() const {
		return capacity_ - used_;
	}
	inline uint8_t peak() const {
		return peakUsed_;
	}
	inline bool isFull() const {
		return used_ >= capacity_;
	}
};
}

#endif /* CORE_MEM_POOL_H_ */
