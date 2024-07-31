#ifndef CONSOLE_HAL_C_
#define CONSOLE_HAL_C_

#include <console/hal.h>
#include <console/driver.h>
#include <console/define.h>
#include "usart.h"

void ConsoleHal_Init()
{
#if USE_DMA == 1
	MX_DMA_Init();
#endif

	CONSOLE_INIT();

#if USE_DMA == 1
	LL_DMA_ConfigAddresses(DMA_MODULE, DMA_TX_CHANNEL,
						 (uint32_t)(&ConsoleDrv.txBufferDMA[0]),
						 LL_USART_DMA_GetRegAddr(CONSOLE_PORT, LL_USART_DMA_REG_DATA_TRANSMIT),
						 LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

	LL_DMA_ConfigAddresses(DMA_MODULE, DMA_RX_CHANNEL,
						 LL_USART_DMA_GetRegAddr(CONSOLE_PORT, LL_USART_DMA_REG_DATA_RECEIVE),
						 (uint32_t)(&ConsoleDrv.rxBuffer[0]),
						 LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

	/* Enable DMA transfer complete/error interrupts  */
	LL_DMA_EnableIT_TC(DMA_MODULE, DMA_TX_CHANNEL);
	LL_DMA_EnableIT_TE(DMA_MODULE, DMA_TX_CHANNEL);
//	LL_DMA_EnableIT_TC(DMA_MODULE, DMA_RX_CHANNEL);
	LL_DMA_EnableIT_TE(DMA_MODULE, DMA_RX_CHANNEL);

	/* Enable DMA Request for TX, RX*/
	LL_USART_EnableDMAReq_TX(CONSOLE_PORT);
	LL_USART_EnableDMAReq_RX(CONSOLE_PORT);

    LL_DMA_SetDataLength(DMA_MODULE, DMA_RX_CHANNEL, MAX_PACKET_LENGTH);
    LL_DMA_EnableChannel(DMA_MODULE, DMA_RX_CHANNEL);

	HAL_Delay(1);
	LL_USART_ClearFlag_IDLE(CONSOLE_PORT);

	LL_USART_EnableIT_IDLE(CONSOLE_PORT);
	LL_USART_EnableIT_RXNE(CONSOLE_PORT);
	LL_USART_EnableIT_ERROR(CONSOLE_PORT);

#else
	LL_USART_EnableIT_RXNE(CONSOLE_PORT);
	LL_USART_EnableIT_ERROR(CONSOLE_PORT);
	LL_USART_DisableIT_TC(CONSOLE_PORT);
	LL_USART_DisableIT_TXE(CONSOLE_PORT);
#endif
}

CONSOLE_ISR_HANDLER()
{
#if USE_DMA == 1
	if(LL_USART_IsActiveFlag_IDLE(CONSOLE_PORT) && LL_USART_IsEnabledIT_IDLE(CONSOLE_PORT))
	{
		LL_USART_ClearFlag_IDLE(CONSOLE_PORT);
		LL_DMA_DisableChannel(DMA_MODULE, DMA_RX_CHANNEL);
		DMA_CLEAR_TC_FLAG(6);

		uint16_t num_received = MAX_PACKET_LENGTH - LL_DMA_GetDataLength(DMA_MODULE, DMA_RX_CHANNEL);
		if(num_received != 0)
		{
			M_EVENT_POST(ConsoleDrv_Receive, num_received);
		}
        // Reset DMA for next reception
        LL_DMA_SetDataLength(DMA_MODULE, DMA_RX_CHANNEL, MAX_PACKET_LENGTH);
        LL_DMA_EnableChannel(DMA_MODULE, DMA_RX_CHANNEL);
	}
	else	// Error
	{
		if(LL_USART_IsActiveFlag_FE(CONSOLE_PORT))
		{
			LL_USART_ClearFlag_FE(CONSOLE_PORT);
		}
		else if(LL_USART_IsActiveFlag_ORE(CONSOLE_PORT))
		{
			LL_USART_ClearFlag_ORE(CONSOLE_PORT);
		}
		else if(LL_USART_IsActiveFlag_NE(CONSOLE_PORT))
		{
			LL_USART_ClearFlag_NE(CONSOLE_PORT);
		}
//		console_.errorRx();
	}
#else
	if(LL_USART_IsActiveFlag_RXNE(CONSOLE_PORT))
	{
		uint8_t c = LL_USART_ReceiveData8(CONSOLE_PORT);
		M_EVENT_POST(ConsoleDrv_Receive, c);
	}
	else if(LL_USART_IsActiveFlag_TC(CONSOLE_PORT))
	{
		LL_USART_DisableIT_TC(CONSOLE_PORT);
	}
	else if(LL_USART_IsActiveFlag_TXE(CONSOLE_PORT))
	{
		LL_USART_DisableIT_TXE(CONSOLE_PORT);
	}
#endif
}

#if USE_DMA == 1
DMA_TX_ISR()
{
	if (DMA_CHECK_TC_FLAG(5))
	{
		DMA_CLEAR_TC_FLAG(5);
		LL_DMA_DisableChannel(DMA_MODULE, DMA_TX_CHANNEL);
		M_EVENT_POST(ConsoleDrv_Send);
	}
	else if (DMA_CHECK_TE_FLAG(5))
	{
		/* Call Error function */
		DMA_CLEAR_TE_FLAG(5);
	}
}

DMA_RX_ISR()
{
	if (DMA_CHECK_TC_FLAG(6))
	{
		DMA_CLEAR_TC_FLAG(6);
		uint16_t num_received = MAX_PACKET_LENGTH;
		M_EVENT_POST(ConsoleDrv_Receive, num_received);
	}
	else if (DMA_CHECK_TE_FLAG(6))
	{
		/* Call Error function */
		DMA_CLEAR_TE_FLAG(6);
	}
}

#else
void ConsoleHal_Write(uint8_t c)
{
	LL_USART_TransmitData8(CONSOLE_PORT, c);
}

bool ConsoleHal_TxReady()
{
	return (LL_USART_IsActiveFlag_TXE(CONSOLE_PORT));
}
#endif

#endif /* CONSOLE_HAL_C_ */
