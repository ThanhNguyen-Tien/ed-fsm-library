#ifndef CONSOLE_HAL_H
#define CONSOLE_HAL_H
#include <core/base.h>

HAL_DEF(console)
    static void init();
#ifndef USING_DMA
    static bool txReady();
    static void write(uint8_t c);
#endif
HAL_END

#endif // HAL_H
