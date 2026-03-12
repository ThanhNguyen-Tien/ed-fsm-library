#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include "main.h"

//	C FUNCTION DECLARATIONS
#ifdef __cplusplus
extern "C"
{
#endif

    void systemInit(void);
    void DWT_Init(void);

#ifdef __cplusplus
}
#endif

// ======================================================
//                  INTERRUPT CONTROL MACROS
//          (usable in BOTH C and C++, safe version)
// ======================================================

// Simple IRQ enable/disable
#define DISABLE_INTERRUPT __disable_irq()
#define ENABLE_INTERRUPT __enable_irq()

#define WAIT_FOR_INTERRUPT __WFI()
#define NO_OPERATION __NOP()

//	CRITICAL SECTION FOR C (safe do/while(0) pattern)
#define CRITICAL_SECTION_BEGIN                      \
    do                                              \
    {                                               \
        uint32_t __primask_saved = __get_PRIMASK(); \
        DISABLE_INTERRUPT;

#define CRITICAL_SECTION_END        \
    __set_PRIMASK(__primask_saved); \
    }                               \
    while (0)

#define CRITICAL_SECTION_PRIORITY_BEGIN(prio)       \
    do                                              \
    {                                               \
        uint32_t __basepri_saved = __get_BASEPRI(); \
        uint32_t __shift = (8U - __NVIC_PRIO_BITS); \
        __set_BASEPRI((prio) << __shift);           \
        __DSB();                                    \
        __ISB();

#define CRITICAL_SECTION_PRIORITY_END \
    __set_BASEPRI(__basepri_saved);   \
    }                                 \
    while (0)                         \
        ;

//	CRITICAL SECTION FOR C++ (RAII version)
#ifdef __cplusplus

class GlobalCriticalSection final
{
private:
    uint32_t primask_;

public:
    __attribute__((always_inline)) inline GlobalCriticalSection()
    {
        primask_ = __get_PRIMASK();
        __disable_irq();
        __DSB();
        __ISB();
    }

    __attribute__((always_inline)) inline ~GlobalCriticalSection()
    {
        __set_PRIMASK(primask_);
    }

    GlobalCriticalSection(const GlobalCriticalSection &) = delete;
    GlobalCriticalSection &operator=(const GlobalCriticalSection &) = delete;
};

class PriorityCriticalSection final
{
private:
    uint32_t basepri_;

public:
    __attribute__((always_inline)) inline explicit PriorityCriticalSection(uint32_t priority)
    {
        basepri_ = __get_BASEPRI();

        uint32_t shift = 8U - __NVIC_PRIO_BITS;
        __set_BASEPRI_MAX(priority << shift);

        __DSB();
        __ISB();
    }

    __attribute__((always_inline)) inline ~PriorityCriticalSection()
    {
        __set_BASEPRI(basepri_);
    }

    PriorityCriticalSection(const PriorityCriticalSection &) = delete;
    PriorityCriticalSection &operator=(const PriorityCriticalSection &) = delete;
};

// RAII usage:  CRITICAL_SECTION;
#define CRITICAL_SECTION GlobalCriticalSection __cs;
#define CRITICAL_SECTION_PRIO(x) PriorityCriticalSection __cs(x);

#endif // __cplusplus

//	EVENT CONSTANTS
#define EVENT_POOL_SIZE 24
#define EVENT_QUEUE_SIZE 32

#endif // CORE_SYSTEM_H
