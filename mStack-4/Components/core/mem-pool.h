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
    static const uint8_t INVALID_INDEX = 0xFF;

    struct alignas(4) Unit
    {
        uint8_t next;
        uint8_t index;
    };

public:
    static constexpr uint32_t REQUIRED_ALIGN =
        (alignof(T) > alignof(Unit)) ? alignof(T) : alignof(Unit);

    static constexpr uint32_t FINAL_ALIGN =
        (REQUIRED_ALIGN > 4) ? REQUIRED_ALIGN : 4;

    static constexpr uint32_t PAYLOAD_OFFSET =
        (sizeof(Unit) + (FINAL_ALIGN - 1)) & ~(FINAL_ALIGN - 1);

    static constexpr uint32_t STRIDE =
        (PAYLOAD_OFFSET + sizeof(T) + (FINAL_ALIGN - 1)) & ~(FINAL_ALIGN - 1);

private:

    uint8_t* pMemBlock_;

    /*
     * freeHead layout
     *
     * 15.....8 | 7.....0
     * version  | index
     */
    volatile uint16_t freeHead_;

    uint8_t capacity_;
    volatile uint8_t used_;
    volatile uint8_t peakUsed_;

private:

    static inline uint8_t atomic_inc_u8(volatile uint8_t* addr)
    {
        uint32_t old;
        uint32_t res;

        do
        {
            old = __LDREXB(addr);
            res = __STREXB((uint8_t)(old + 1), addr);
        }
        while (res);

        __DMB();

        return (uint8_t)(old + 1);
    }

    static inline uint8_t headIndex(uint16_t h)
    {
        return (uint8_t)(h & 0xFF);
    }

    static inline uint8_t headVersion(uint16_t h)
    {
        return (uint8_t)(h >> 8);
    }

    static inline uint16_t makeHead(uint8_t idx, uint8_t ver)
    {
        return ((uint16_t)ver << 8) | idx;
    }

public:

    MemPool(void* memory, uint8_t unitNum)
        : pMemBlock_(static_cast<uint8_t*>(memory)),
          freeHead_(0),
          capacity_(unitNum),
          used_(0),
          peakUsed_(0)
    {
    	assert(pMemBlock_ != nullptr);

        for (uint8_t i = 0; i < unitNum; ++i)
        {
            uint8_t* addr = pMemBlock_ + i * STRIDE;
            Unit* unit = static_cast<Unit*>((void*)addr);

            unit->index = i;
            unit->next =
                (i + 1 < unitNum) ? (i + 1) : INVALID_INDEX;
        }
        freeHead_ = makeHead(0, 0);
    }

public:

    __attribute__((always_inline)) inline void* Alloc()
    {
        uint16_t oldHead;
        uint16_t newHead;
        Unit* unit;
        do
        {
            oldHead = __LDREXH(&freeHead_);
            uint8_t index = headIndex(oldHead);

            if (index == INVALID_INDEX)
            {
                __CLREX();
                return nullptr;
            }

            uint8_t version = headVersion(oldHead);
            uint8_t* addr = pMemBlock_ + index * STRIDE;
            unit = static_cast<Unit*>((void*)addr);
            uint8_t next = unit->next;
            newHead = makeHead(next, version + 1);
        }
        while (__STREXH(newHead, &freeHead_) != 0);

        uint8_t u = atomic_inc_u8(&used_);
        if (u > peakUsed_)
        {
            peakUsed_ = u;
        }
        uint8_t* payload =
            (uint8_t*)unit + PAYLOAD_OFFSET;

        return payload;
    }

public:

    __attribute__((always_inline)) inline void Free(void* p)
    {
    	assert(p != nullptr);

        uint8_t* payload = static_cast<uint8_t*>(p);
        uint8_t* unitAddr = payload - PAYLOAD_OFFSET;

        assert(unitAddr >= pMemBlock_);
        assert(unitAddr < pMemBlock_ + capacity_ * STRIDE);

        Unit* unit = static_cast<Unit*>((void*)unitAddr);
        uint8_t index = unit->index;
        uint16_t oldHead;
        uint16_t newHead;
        do
        {
            oldHead = __LDREXH(&freeHead_);

            uint8_t headIdx = headIndex(oldHead);
            uint8_t version = headVersion(oldHead);

            unit->next = headIdx;

            newHead = makeHead(index, version + 1);
        }
        while (__STREXH(newHead, &freeHead_) != 0);

        --used_;
    }

public:

    inline uint8_t capacity() const { return capacity_; }

    inline uint8_t used() const { return used_; }

    inline uint8_t free() const { return capacity_ - used_; }

    inline uint8_t peak() const { return peakUsed_; }

    inline bool isFull() const { return used_ >= capacity_; }
};

}

#endif
