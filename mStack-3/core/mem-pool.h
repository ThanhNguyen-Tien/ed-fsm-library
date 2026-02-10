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
	struct __attribute__((aligned(4))) _Unit {
	    struct _Unit *pNext;
	};

    static constexpr uint32_t REQUIRED_ALIGN =
        (alignof(T) > alignof(_Unit)) ? alignof(T) : alignof(_Unit);

    static constexpr uint32_t FINAL_ALIGN =
        (REQUIRED_ALIGN > 4) ? REQUIRED_ALIGN : 4;

    static constexpr uint32_t STRIDE =
        (sizeof(_Unit) + sizeof(T) + (FINAL_ALIGN - 1)) & ~(FINAL_ALIGN - 1);

	void *pMemBlock_ = nullptr;                // Address of memory pool.

	// Manage all unit with two linkedlist.
	struct _Unit *pAllocatedMemBlock_ = nullptr; // Head pointer to Allocated linkedlist.
	struct _Unit *pFreeMemBlock_ = nullptr;  // Head pointer to Free linkedlist.

	uint8_t capacity_ = 0;
	uint8_t used_ = 0;
	uint8_t peakUsed_ = 0;
public:

	MemPool(uint8_t unitNum) :
			pMemBlock_(nullptr), pAllocatedMemBlock_(nullptr), pFreeMemBlock_(
					nullptr), capacity_(unitNum), used_(0), peakUsed_(0) {

        const uint32_t totalSize = unitNum * STRIDE;
        pMemBlock_ = malloc(totalSize);

        if (!pMemBlock_) {
            Error_Handler();
        }

        uint8_t* base = static_cast<uint8_t*>(pMemBlock_);

        for (uint32_t i = 0; i < unitNum; ++i) {
            uint8_t* unitAddr = base + i * STRIDE;
            _Unit* unit = static_cast<_Unit*>(static_cast<void*>(unitAddr));

            unit->pNext = pFreeMemBlock_;
            pFreeMemBlock_ = unit;
        }

        Engine::instance().events().addNumOfByteHeap(totalSize);
	}

	~MemPool() {
		free(pMemBlock_);
	}

	// Allocate memory unit If memory pool can`t provide proper memory unit,
	// It will call system function.
    inline void* Alloc() {
        CRITICAL_SECTION;

        if (!pFreeMemBlock_) {
            return nullptr;
        }

        _Unit* unit = pFreeMemBlock_;
        pFreeMemBlock_ = unit->pNext;

        unit->pNext = pAllocatedMemBlock_;
        pAllocatedMemBlock_ = unit;

        ++used_;
        if (used_ > peakUsed_) {
            peakUsed_ = used_;
        }

        uint8_t* payload =
            static_cast<uint8_t*>(static_cast<void*>(unit)) + sizeof(_Unit);

        return static_cast<void*>(payload);
    }

	inline void Free(void *p) {
        if (!p) {
            return;
        }

        uint8_t* payload = static_cast<uint8_t*>(p);
        uint8_t* unitAddr = payload - sizeof(_Unit);

        _Unit* unit = static_cast<_Unit*>(static_cast<void*>(unitAddr));
		_Unit *prev = nullptr;
        for (_Unit* it = pAllocatedMemBlock_; it; it = it->pNext) {
            if (it == unit) {
                if (prev) {
                    prev->pNext = it->pNext;
                } else {
                    pAllocatedMemBlock_ = it->pNext;
                }

                unit->pNext = pFreeMemBlock_;
                pFreeMemBlock_ = unit;
                --used_;
                return;
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
