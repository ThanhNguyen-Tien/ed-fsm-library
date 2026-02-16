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

    typedef unsigned char  uint8_t;
    typedef unsigned int   uint32_t;

    typedef uint8_t Index;

    static const Index INVALID_INDEX = 0xFF;

    struct alignas(4) Unit {
        Index next;
    };

    static const uint32_t REQUIRED_ALIGN =
        (alignof(T) > alignof(Unit)) ? alignof(T) : alignof(Unit);

    static const uint32_t FINAL_ALIGN =
        (REQUIRED_ALIGN > 4) ? REQUIRED_ALIGN : 4;

    static const uint32_t STRIDE =
        (sizeof(Unit) + sizeof(T) + (FINAL_ALIGN - 1)) & ~(FINAL_ALIGN - 1);

private:

    uint8_t* pMemBlock_;
    Index    freeHead_;

    uint8_t  capacity_;
    uint8_t  used_;
    uint8_t  peakUsed_;

public:

    MemPool(uint8_t unitNum)
        : pMemBlock_(0),
          freeHead_(INVALID_INDEX),
          capacity_(unitNum),
          used_(0),
          peakUsed_(0)
    {
        uint32_t totalSize = unitNum * STRIDE;

        pMemBlock_ = static_cast<uint8_t*>(malloc(totalSize));

        if (!pMemBlock_) {
            Error_Handler();
        }

        // Build free list
        for (Index i = 0; i < unitNum; ++i) {

            uint8_t* addr = pMemBlock_ + i * STRIDE;

            Unit* unit =
                static_cast<Unit*>( static_cast<void*>(addr) );

            unit->next =
                (i + 1 < unitNum) ? (i + 1) : INVALID_INDEX;
        }

        freeHead_ = 0;

        Engine::instance().events().addNumOfByteHeap(totalSize);
    }

    ~MemPool() {
        free(pMemBlock_);
    }

    inline void* Alloc() {

        CRITICAL_SECTION_PRIO(1);

        if (freeHead_ == INVALID_INDEX) { // No free units
        	Error_Handler();
            return 0;
        }

        Index index = freeHead_;

        uint8_t* addr = pMemBlock_ + index * STRIDE;

        Unit* unit =
            static_cast<Unit*>( static_cast<void*>(addr) );

        freeHead_ = unit->next;

        ++used_;
        if (used_ > peakUsed_) {
            peakUsed_ = used_;
        }

        uint8_t* payload = addr + sizeof(Unit);

        return static_cast<void*>(payload);
    }

    inline void Free(void* p) {

        if (!p) {
        	Error_Handler();
            return;
        }

        CRITICAL_SECTION_PRIO(1);

        uint8_t* payload =
            static_cast<uint8_t*>(p);

        uint8_t* unitAddr =
            payload - sizeof(Unit);

        // Optional: range check
        if (unitAddr < pMemBlock_ ||
            unitAddr >= pMemBlock_ + capacity_ * STRIDE) {
        	Error_Handler();
            return;
        }

        Index index =
            (Index)((unitAddr - pMemBlock_) / STRIDE);

        uint8_t* addr =
            pMemBlock_ + index * STRIDE;

        Unit* unit =
            static_cast<Unit*>( static_cast<void*>(addr) );

        unit->next = freeHead_;
        freeHead_  = index;

        --used_;
    }

    inline uint8_t capacity() const { return capacity_; }
    inline uint8_t used() const { return used_; }
    inline uint8_t free() const { return capacity_ - used_; }
    inline uint8_t peak() const { return peakUsed_; }
    inline bool isFull() const { return used_ >= capacity_; }
};

}
#endif /* CORE_MEM_POOL_H_ */
