#ifndef HYDRA_HAL_H_
#define HYDRA_HAL_H_

#include "usart.h"
#include <stdbool.h>

void HydraHal_Init();

#if USE_DMA != 1
void HydraHal_Write(uint8_t c);
bool HydraHal_TxReady();
#endif

#endif /* HYDRA_HAL_H_ */
