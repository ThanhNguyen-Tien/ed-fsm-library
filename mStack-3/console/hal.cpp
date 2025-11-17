#include <console/driver.h>
#include <console/hal.h>
#include "usart.h"

#ifdef USING_DMA
#include "dma.h"
void console::HAL::init()
{
	MX_DMA_Init();
	CONSOLE_INIT;

	LL_DMA_ConfigAddresses(DMA_MODULE, DMA_TX_CHANNEL,
			(uint32_t) (&Driver::instance().txBuf()),
			LL_USART_DMA_GetRegAddr(CONSOLE_PORT),
			LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

	LL_DMA_ConfigAddresses(DMA_MODULE, DMA_RX_CHANNEL,
			LL_USART_DMA_GetRegAddr(CONSOLE_PORT),
			(uint32_t) (&Driver::instance().rxBuf()),
			LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

	/* Enable DMA transfer complete/error interrupts  */
	LL_DMA_EnableIT_TC(DMA_MODULE, DMA_TX_CHANNEL);
	LL_DMA_EnableIT_TE(DMA_MODULE, DMA_TX_CHANNEL);
//	LL_DMA_EnableIT_TC(DMA_MODULE, DMA_RX_CHANNEL);
	LL_DMA_EnableIT_TE(DMA_MODULE, DMA_RX_CHANNEL);

	/* Enable DMA Request for TX, RX*/
	LL_USART_EnableDMAReq_TX(CONSOLE_PORT);
	LL_USART_EnableDMAReq_RX(CONSOLE_PORT);

	LL_DMA_SetDataLength(DMA_MODULE, DMA_RX_CHANNEL,
			MAX_PACKET_LENGTH);
	LL_DMA_EnableStream(DMA_MODULE, DMA_RX_CHANNEL);

	int16_t timeout = 100;
	while (!LL_USART_IsActiveFlag_IDLE(CONSOLE_PORT) && timeout > 0)
	{
		timeout--;
	};
	LL_USART_ClearFlag_IDLE(CONSOLE_PORT);

	LL_USART_EnableIT_IDLE(CONSOLE_PORT);
//	LL_USART_EnableIT_RXNE(CONSOLE_PORT);
	LL_USART_EnableIT_ERROR(CONSOLE_PORT);
	LL_USART_EnableIT_PE(CONSOLE_PORT);
}

CONSOLE_ISR_HANDLER()
{
    if (LL_USART_IsActiveFlag_FE(CONSOLE_PORT) ||
        LL_USART_IsActiveFlag_NE(CONSOLE_PORT) ||
        LL_USART_IsActiveFlag_ORE(CONSOLE_PORT) ||
        LL_USART_IsActiveFlag_PE(CONSOLE_PORT))
    {
        volatile uint8_t dummy = LL_USART_ReceiveData8(CONSOLE_PORT);
        UNUSED(dummy);

        if (LL_USART_IsActiveFlag_FE(CONSOLE_PORT)) {
            LL_USART_ClearFlag_FE(CONSOLE_PORT);
        }
        if (LL_USART_IsActiveFlag_NE(CONSOLE_PORT)) {
            LL_USART_ClearFlag_NE(CONSOLE_PORT);
        }
        if (LL_USART_IsActiveFlag_ORE(CONSOLE_PORT)) {
            LL_USART_ClearFlag_ORE(CONSOLE_PORT);
        }
        if (LL_USART_IsActiveFlag_PE(CONSOLE_PORT)) {
            LL_USART_ClearFlag_PE(CONSOLE_PORT);
        }
//        Comm_Drv_Err_Rx();
    }

	if (LL_USART_IsActiveFlag_IDLE(CONSOLE_PORT))
	{
		LL_USART_ClearFlag_IDLE(CONSOLE_PORT);
		static uint16_t previous_pos = 0;
		uint16_t current_pos = MAX_PACKET_LENGTH
				- LL_DMA_GetDataLength(DMA_MODULE, DMA_RX_CHANNEL);

		if (current_pos >= previous_pos)
		{
			uint16_t data_length = current_pos - previous_pos;
			data_length |= (previous_pos << 8);
			console::Driver::instance().receiveEvent.post(data_length);
		} else
		{
			uint16_t data_length_1 = MAX_PACKET_LENGTH - previous_pos;
			data_length_1 |= (previous_pos << 8);
			uint16_t data_length_2 = current_pos;
			console::Driver::instance().receiveEvent.post(data_length_1);
			console::Driver::instance().receiveEvent.post(data_length_2);
		}

		previous_pos = current_pos;
	}
}

DMA_TX_ISR()
{
	if (DMA_CHECK_TC_FLAG(6))
	{
		DMA_CLEAR_TC_FLAG(6);
		console::Driver::instance().sendEvent.post();
	} else if (DMA_CHECK_TE_FLAG(6))
	{
		/* Call Error function */
		DMA_CLEAR_TE_FLAG(6);
	}
}

DMA_RX_ISR()
{
	if (DMA_CHECK_TE_FLAG(5))
	{
		/* Call Error function */
		DMA_CLEAR_TE_FLAG(5);
	}
}

#else
void console::HAL::init()
{
	CONSOLE_INIT;

	LL_USART_EnableIT_RXNE(CONSOLE_PORT);
	LL_USART_EnableIT_ERROR(CONSOLE_PORT);
	LL_USART_DisableIT_TC(CONSOLE_PORT);
	LL_USART_DisableIT_TXE(CONSOLE_PORT);
}

bool console::HAL::txReady()
{
	return (LL_USART_IsActiveFlag_TXE(CONSOLE_PORT));
}

void console::HAL::write(uint8_t c)
{
	LL_USART_TransmitData8(CONSOLE_PORT, c);
}

CONSOLE_ISR_HANDLER()
{
	if (LL_USART_IsActiveFlag_RXNE(CONSOLE_PORT)
			&& LL_USART_IsEnabledIT_RXNE(CONSOLE_PORT))
	{
		uint8_t c = LL_USART_ReceiveData8(CONSOLE_PORT);
		console::Driver::instance().receiveEvent.post(c);
	}
	else if (LL_USART_IsActiveFlag_TC(CONSOLE_PORT))
	{
		LL_USART_DisableIT_TC(CONSOLE_PORT);
	}
	else if (LL_USART_IsActiveFlag_TXE(CONSOLE_PORT))
	{
		LL_USART_DisableIT_TXE(CONSOLE_PORT);
	}
}
#endif
