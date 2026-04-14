#ifndef CORE_MEM_POOL_H_
#define CORE_MEM_POOL_H_

#include <core/engine.h>

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
        static constexpr uint32_t FINAL_ALIGN = alignof(T) > 4 ? alignof(T) : 4;
		static constexpr uint32_t PAYLOAD_OFFSET = (sizeof(Unit) + (FINAL_ALIGN - 1)) & ~(FINAL_ALIGN - 1);
		static constexpr uint32_t STRIDE = (PAYLOAD_OFFSET + sizeof(T) + (FINAL_ALIGN - 1)) & ~(FINAL_ALIGN - 1);

    private:
        uint8_t *pMemBlock_;

        /*
         * freeHead_ layout (32-bit):
         * [31...16: Version] | [7...0: Index]
         */
        volatile uint16_t freeHead_;
        uint8_t capacity_;
        uint8_t used_;
        uint8_t peakUsed_;

    public:
        MemPool(void* memory, uint8_t unitNum)
            : pMemBlock_(static_cast<uint8_t*>(memory)),
              freeHead_(0),
              capacity_(unitNum),
              used_(0),
              peakUsed_(0)
        {
            for (uint8_t i = 0; i < unitNum; ++i)
            {
                Unit* unit = static_cast<Unit*>(static_cast<void*>(pMemBlock_ + (i * STRIDE)));
                unit->index = i;
                unit->next = (i + 1 < unitNum) ? (i + 1) : INVALID_INDEX;
            }
            // version 0, index 0
            freeHead_ = 0;
        }

    public:
        inline void* Alloc()
        {
            uint32_t oldHead, newHead;
            uint32_t retry = 0;
            uint8_t index;

            for (;;) {
                oldHead = __LDREXH(&freeHead_);
                index = static_cast<uint8_t>(oldHead & 0xFF);

                if (index == INVALID_INDEX)
                {
                    __CLREX();
                    return nullptr;
                }

                Unit* unit = static_cast<Unit*>(static_cast<void*>(pMemBlock_ + (index * STRIDE)));

                uint32_t version = (oldHead >> 16) + 1;
                newHead = (version << 16) | unit->next;

                if (__STREXH(newHead, &freeHead_) == 0) {
                    uint32_t u = __atomic_add_fetch(&used_, 1, __ATOMIC_RELAXED);
                    if (u > peakUsed_)
                    {
                        peakUsed_ = u;
                    }
                    break;
                }
                retry++;
            }
            __DMB(); // Ensure memory operations are completed before returning the pointer

            if (retry > 0) Telemetry::log(TelemetryType::MEMPOOL_ALLOC_CONTENTION, (uint16_t)retry);

            return static_cast<void*>(pMemBlock_ + (index * STRIDE) + PAYLOAD_OFFSET);
        }

        inline void Free(void* p)
        {
            if (p == nullptr) return;

            uint8_t* unitAddr = static_cast<uint8_t*>(p) - PAYLOAD_OFFSET;
            Unit* unit = static_cast<Unit*>(static_cast<void*>(unitAddr));
            uint8_t index = unit->index;

            uint16_t oldHead, newHead;
            for (;;)
            {
                oldHead = __LDREXH(&freeHead_);
                unit->next = static_cast<uint8_t>(oldHead & 0xFF);

                __DMB(); // Ensure next update is visible before updating head

                uint32_t version = (oldHead >> 16) + 1;
                newHead = (version << 16) | index;

                if (__STREXH(newHead, &freeHead_) == 0)
				{
                	__atomic_sub_fetch(&used_, 1, __ATOMIC_RELAXED);
                	break;
				}
            }
        }

    public:
        uint8_t used() const { return static_cast<uint8_t>(used_); }
        uint8_t peak() const { return static_cast<uint8_t>(peakUsed_); }
        uint8_t capacity() const { return capacity_; }
        inline uint8_t free() const { return this->capacity_ - this->used_; }
        inline bool isFull() const { return this->used_ >= this->capacity_; }
    };
}

#endif
