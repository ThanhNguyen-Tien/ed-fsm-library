#ifndef HYDRA_HAL_C_
#define HYDRA_HAL_C_

#include "hal.h"
#include "driver.h"
#include "define.h"
#include <core/engine.h>
#include "usart.h"

HYDRA_ISR_HANDLER()
{
	Engine_ISR_Enter();
#if USE_DMA == 1
	if (LL_USART_IsActiveFlag_IDLE(HYDRA_PORT))
	{
		LL_USART_ClearFlag_IDLE(HYDRA_PORT);
		static uint16_t previous_pos = 0;
		uint16_t current_pos = MAX_PACKET_LENGTH
				- LL_DMA_GetDataLength(DMA_MODULE, DMA_RX_CHANNEL);

		if (current_pos >= previous_pos)
		{
			uint16_t data_length = current_pos - previous_pos;
			data_length |= (previous_pos << 8);
			Event_Post(HydraDrv_ReceiveEvent.index, &data_length);
		} else
		{
			uint16_t data_length_1 = MAX_PACKET_LENGTH - previous_pos;
			data_length_1 |= (previous_pos << 8);
			uint16_t data_length_2 = current_pos;
			Event_Post(HydraDrv_ReceiveEvent.index, &data_length_1);
			Event_Post(HydraDrv_ReceiveEvent.index, &data_length_2);
		}

		previous_pos = current_pos;
	}
#else
	if(LL_USART_IsActiveFlag_RXNE(HYDRA_PORT))
	{
		uint8_t c = LL_USART_ReceiveData8(HYDRA_PORT);
		M_EVENT_POST(HydraDrv_ReceiveEvent, c);
	}
#endif
    if (LL_USART_IsActiveFlag_FE(HYDRA_PORT) ||
        LL_USART_IsActiveFlag_NE(HYDRA_PORT) ||
        LL_USART_IsActiveFlag_ORE(HYDRA_PORT) ||
        LL_USART_IsActiveFlag_PE(HYDRA_PORT))
    {
        volatile uint8_t dummy = LL_USART_ReceiveData8(HYDRA_PORT);
        UNUSED(dummy);

        if (LL_USART_IsActiveFlag_FE(HYDRA_PORT)) {
            LL_USART_ClearFlag_FE(HYDRA_PORT);
        }
        if (LL_USART_IsActiveFlag_NE(HYDRA_PORT)) {
            LL_USART_ClearFlag_NE(HYDRA_PORT);
        }
        if (LL_USART_IsActiveFlag_ORE(HYDRA_PORT)) {
            LL_USART_ClearFlag_ORE(HYDRA_PORT);
        }
        if (LL_USART_IsActiveFlag_PE(HYDRA_PORT)) {
            LL_USART_ClearFlag_PE(HYDRA_PORT);
        }
//        Comm_Drv_Err_Rx();
    }
}

#if USE_DMA == 1
#include "dma.h"
DMA_TX_ISR()
{
	Engine_ISR_Enter();
	if (DMA_TX_TC_ACTIVE())
	{
		DMA_TX_TC_CLEAR();
		Event_Post(HydraDrv_SendEvent.index, NULL);
	} else if (DMA_TX_TE_ACTIVE())
	{
		/* Call Error function */
		DMA_TX_TE_CLEAR();
	}
}

DMA_RX_ISR()
{
	Engine_ISR_Enter();
	if (DMA_RX_TE_ACTIVE())
	{
		/* Call Error function */
		DMA_RX_TE_CLEAR();
	}
}

#else
void HydraHal_Write(uint8_t c)
{
	LL_USART_TransmitData8(HYDRA_PORT, c);
}

bool HydraHal_TxReady()
{
	return (LL_USART_IsActiveFlag_TXE(HYDRA_PORT));
}
#endif

void HydraHal_Init()
{
#if USE_DMA == 1
	MX_DMA_Init();
#endif

	HYDRA_INIT();

#if USE_DMA == 1
	LL_DMA_ConfigAddresses(DMA_MODULE, DMA_TX_CHANNEL,
			(uint32_t) (&HydraDrv.txBuffer[0]),
			LL_USART_DMA_GetRegAddr(HYDRA_PORT),
			LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

	LL_DMA_ConfigAddresses(DMA_MODULE, DMA_RX_CHANNEL,
			LL_USART_DMA_GetRegAddr(HYDRA_PORT),
			(uint32_t) (&HydraDrv.rxBufferDma_[0]),
			LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

	/* Enable DMA transfer complete/error interrupts  */
	LL_DMA_EnableIT_TC(DMA_MODULE, DMA_TX_CHANNEL);
	LL_DMA_EnableIT_TE(DMA_MODULE, DMA_TX_CHANNEL);
//	LL_DMA_EnableIT_TC(DMA_MODULE, DMA_RX_CHANNEL);
	LL_DMA_EnableIT_TE(DMA_MODULE, DMA_RX_CHANNEL);

	/* Enable DMA Request for TX, RX*/
	LL_USART_EnableDMAReq_TX(HYDRA_PORT);
	LL_USART_EnableDMAReq_RX(HYDRA_PORT);

	LL_DMA_SetDataLength(DMA_MODULE, DMA_RX_CHANNEL,
			MAX_PACKET_LENGTH);
	LL_DMA_EnableStream(DMA_MODULE, DMA_RX_CHANNEL);

	int16_t timeout = 100;
	while (!LL_USART_IsActiveFlag_IDLE(HYDRA_PORT) && timeout > 0)
	{
		timeout--;
	};
	LL_USART_ClearFlag_IDLE(HYDRA_PORT);

	LL_USART_EnableIT_IDLE(HYDRA_PORT);
//	LL_USART_EnableIT_RXNE(HYDRA_PORT);
	LL_USART_EnableIT_ERROR(HYDRA_PORT);
	LL_USART_EnableIT_PE(HYDRA_PORT);

#else
	LL_USART_EnableIT_RXNE(HYDRA_PORT);
	LL_USART_EnableIT_ERROR(HYDRA_PORT);
	LL_USART_DisableIT_TC(HYDRA_PORT);
	LL_USART_DisableIT_TXE(HYDRA_PORT);
#endif
}

#endif /* HYDRA_HAL_C_ */
