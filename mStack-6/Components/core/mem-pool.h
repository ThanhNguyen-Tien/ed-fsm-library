#ifndef CORE_MEM_POOL_H_
#define CORE_MEM_POOL_H_

#include <cstdlib>
#include <cstring>
#include <core/engine.h>
#include "main.h"

namespace core
{
    template <typename T>
    class MemPool
    {
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
        uint8_t *pMemBlock_;

        /*
         * freeHead layout
         *
         * 15.....8 | 7.....0
         * version  | index
         */
        uint16_t freeHead_;
        uint8_t capacity_;
        uint8_t used_;
        uint8_t peakUsed_;

    private:
        static inline uint8_t atomic_inc_u8(uint8_t *addr)
        {
            uint32_t old;
            uint32_t res;
            do
            {
                old = __LDREXB(addr);
                res = __STREXB((uint8_t)(old + 1), addr);
            } while (res);
            __DMB();
            return (uint8_t)(old + 1);
        }

        static inline uint8_t atomic_dec_u8(uint8_t *addr)
        {
            uint32_t old;
            uint32_t res;
            do
            {
                old = __LDREXB(addr);
                res = __STREXB((uint8_t)(old - 1), addr);
            } while (res);
            __DMB();
            return (uint8_t)(old - 1);
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
        MemPool(void *memory, uint8_t unitNum)
            : pMemBlock_(static_cast<uint8_t *>(memory)),
              freeHead_(0),
              capacity_(unitNum),
              used_(0),
              peakUsed_(0)
        {
            assert(pMemBlock_ != nullptr);

            for (uint8_t i = 0; i < unitNum; ++i)
            {
                uint8_t *addr = this->pMemBlock_ + i * STRIDE;
                Unit *unit = static_cast<Unit *>((void *)addr);

                unit->index = i;
                unit->next =
                    (i + 1 < unitNum) ? (i + 1) : INVALID_INDEX;
            }
            this->freeHead_ = this->makeHead(0, 0);
        }

    public:
        inline void *Alloc()
        {
            uint16_t oldHead, newHead;
            Unit *unit;
            uint32_t retry_count = 0;

            for (;;)
            {
                oldHead = __LDREXH(&this->freeHead_);
                uint8_t index = this->headIndex(oldHead);

                if (index == INVALID_INDEX)
                {
                    __CLREX();
                    return nullptr;
                }

                uint8_t version = this->headVersion(oldHead);
                uint8_t *addr = this->pMemBlock_ + index * STRIDE;
                unit = static_cast<Unit *>((void *)addr);

                newHead = this->makeHead(unit->next, version + 1);

                if (__STREXH(newHead, &freeHead_) == 0)
                {
                    break; // Success
                }
                retry_count++;
            }
            __DMB();

            if (retry_count > 0)
            {
                Telemetry::log(TelemetryType::MEMPOOL_ALLOC_CONTENTION, (uint16_t)retry_count);
            }

            uint8_t u = atomic_inc_u8(&this->used_);
            uint8_t currentPeak = __atomic_load_n(&this->peakUsed_, __ATOMIC_RELAXED);
            if (u > currentPeak)
            {
                __atomic_store_n(&this->peakUsed_, u, __ATOMIC_RELAXED);
            }

            return (uint8_t *)unit + PAYLOAD_OFFSET;
        }

        inline void Free(void *p)
        {
            uint8_t *unitAddr = (uint8_t *)p - PAYLOAD_OFFSET;
            Unit *unit = static_cast<Unit *>((void *)unitAddr);
            uint8_t index = unit->index;

            uint16_t oldHead, newHead;
            uint32_t retry_count = 0;

            for (;;)
            {
                oldHead = __LDREXH(&this->freeHead_);
                uint8_t headIdx = this->headIndex(oldHead);
                uint8_t version = this->headVersion(oldHead);

                unit->next = headIdx;
                __DMB(); // Ensure 'next' is set before updating head

                newHead = this->makeHead(index, version + 1);

                if (__STREXH(newHead, &this->freeHead_) == 0)
                {
                    break; // Success
                }
                retry_count++;
            }

            if (retry_count > 0)
            {
                Telemetry::log(TelemetryType::MEMPOOL_FREE_CONTENTION, (uint16_t)retry_count);
            }

            static_cast<void>(atomic_dec_u8(&this->used_));
        }

    public:
        inline uint8_t used() const
        {
            return __atomic_load_n(&this->used_, __ATOMIC_RELAXED);
        }

        inline uint8_t peak() const
        {
            return __atomic_load_n(&this->peakUsed_, __ATOMIC_RELAXED);
        }
        inline uint8_t capacity() const { return this->capacity_; }
        inline uint8_t free() const { return this->capacity_ - this->used_; }
        inline bool isFull() const { return this->used_ >= this->capacity_; }
    };
}

#endif
