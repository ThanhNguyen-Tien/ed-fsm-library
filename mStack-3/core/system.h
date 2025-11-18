#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include "main.h"

// ======================================================
//                  C FUNCTION DECLARATIONS
// ======================================================
#ifdef __cplusplus
extern "C" {
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
#define DISABLE_INTERRUPT      __disable_irq()
#define ENABLE_INTERRUPT       __enable_irq()

#define WAIT_FOR_INTERRUPT     __WFI()
#define NO_OPERATION          __NOP()


// ======================================================
//     CRITICAL SECTION FOR C (safe do/while(0) pattern)
// ======================================================
#ifndef __cplusplus   // Only for pure C code

#define CRITICAL_SECTION_BEGIN                   		\
    do {                                                \
        uint32_t __primask_saved = __get_PRIMASK();     \
        DISABLE_INTERRUPT;

#define CRITICAL_SECTION_END                            \
        __set_PRIMASK(__primask_saved);                 \
    } while (0)

#endif // __cplusplus



// ======================================================
//          CRITICAL SECTION FOR C++ (RAII version)
// ======================================================
#ifdef __cplusplus

class CriticalSection {
private:
    uint32_t primask_;
public:
    CriticalSection()
        : primask_(__get_PRIMASK()) {
        DISABLE_INTERRUPT;
    }

    ~CriticalSection() {
        __set_PRIMASK(primask_);
    }

    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;
};

// RAII usage:  CRITICAL_SECTION;
#define CRITICAL_SECTION   CriticalSection __cs

#endif // __cplusplus


// ======================================================
//                  EVENT CONSTANTS
// ======================================================
#define EVENT_POOL_SIZE      24
#define EVENT_QUEUE_SIZE     128


#endif // CORE_SYSTEM_H
