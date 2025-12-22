#include <hydra/driver.h>
#include <hydra/hal.h>
#include "usart.h"

#ifdef USING_DMA
#include "dma.h"
void hydra::HAL::init()
{
	MX_DMA_Init();
	HYDRA_INIT;

	LL_DMA_ConfigAddresses(DMA_MODULE, DMA_TX_CHANNEL,
			(uint32_t) (&Driver::instance().txBuf()),
			LL_USART_DMA_GetRegAddr(HYDRA_PORT),
			LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

	LL_DMA_ConfigAddresses(DMA_MODULE, DMA_RX_CHANNEL,
			LL_USART_DMA_GetRegAddr(HYDRA_PORT),
			(uint32_t) (&Driver::instance().rxBuf()),
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
}

HYDRA_ISR_HANDLER()
{
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
			hydra::Driver::instance().receiveEvent.post(data_length);
		} else
		{
			uint16_t data_length_1 = MAX_PACKET_LENGTH - previous_pos;
			data_length_1 |= (previous_pos << 8);
			uint16_t data_length_2 = current_pos;
			hydra::Driver::instance().receiveEvent.post(data_length_1);
			hydra::Driver::instance().receiveEvent.post(data_length_2);
		}

		previous_pos = current_pos;
	}
}

DMA_TX_ISR()
{
	if (DMA_TX_TC_ACTIVE())
	{
		DMA_TX_TC_CLEAR();
		hydra::Driver::instance().sendEvent.post();
	} else if (DMA_TX_TE_ACTIVE())
	{
		/* Call Error function */
		DMA_TX_TE_CLEAR();
	}
}

DMA_RX_ISR()
{
	if (DMA_RX_TE_ACTIVE())
	{
		/* Call Error function */
		DMA_RX_TE_CLEAR();
	}
}

#else
void hydra::HAL::init()
{
	HYDRA_INIT;

	LL_USART_EnableIT_RXNE(HYDRA_PORT);
	LL_USART_EnableIT_ERROR(HYDRA_PORT);
	LL_USART_DisableIT_TC(HYDRA_PORT);
	LL_USART_DisableIT_TXE(HYDRA_PORT);
}

bool hydra::HAL::txReady()
{
	return (LL_USART_IsActiveFlag_TXE(HYDRA_PORT));
}

void hydra::HAL::write(uint8_t c)
{
	LL_USART_TransmitData8(HYDRA_PORT, c);
}

HYDRA_ISR_HANDLER()
{
	if (LL_USART_IsActiveFlag_RXNE(HYDRA_PORT)
			&& LL_USART_IsEnabledIT_RXNE(HYDRA_PORT))
	{
		uint8_t c = LL_USART_ReceiveData8(HYDRA_PORT);
		hydra::Driver::instance().receiveEvent.post(c);
	}
	else if (LL_USART_IsActiveFlag_TC(HYDRA_PORT))
	{
		LL_USART_DisableIT_TC(HYDRA_PORT);
	}
	else if (LL_USART_IsActiveFlag_TXE(HYDRA_PORT))
	{
		LL_USART_DisableIT_TXE(HYDRA_PORT);
	}
}
#endif
