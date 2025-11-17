#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include <main.h>

//#define DISABLE_INTERRUPT   __disable_irq()
//#define ENABLE_INTERRUPT    __enable_irq()
#define WAIT_FOR_INTERUPT   __WFI()
#define NO_OPERATION		__NOP()

#define EVENT_POOL_SIZE     24
#define EVENT_QUEUE_SIZE	128

void systemInit();
void DWT_Init(void);

class CriticalSection {
private:
    uint32_t primask_;
public:
    CriticalSection() : primask_(__get_PRIMASK()) {
        __disable_irq();
    }
    ~CriticalSection() {
        __set_PRIMASK(primask_);
    }
    // Prevent copy
    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;
};
#define CRITICAL_SECTION() CriticalSection __cs

#endif // SYSTEM_H
