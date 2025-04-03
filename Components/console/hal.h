#ifndef CONSOLE_HAL_H_
#define CONSOLE_HAL_H_

#include "usart.h"
#include <stdbool.h>

void ConsoleHal_Init();
void ConsoleHal_Write(uint8_t c);
bool ConsoleHal_TxReady();

#endif /* CONSOLE_HAL_H_ */
