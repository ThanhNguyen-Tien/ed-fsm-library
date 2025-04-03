#ifndef CONSOLE_HAL_C_
#define CONSOLE_HAL_C_

#include "hal.h"
#include "driver.h"
#include "define.h"
#include <core/engine.h>
#include "usart.h"

void ConsoleHal_Init()
{
	CONSOLE_INIT();

	LL_USART_EnableIT_RXNE(CONSOLE_PORT);
	LL_USART_EnableIT_ERROR(CONSOLE_PORT);
	LL_USART_DisableIT_TC(CONSOLE_PORT);
	LL_USART_DisableIT_TXE(CONSOLE_PORT);
}

CONSOLE_ISR_HANDLER()
{
	if(LL_USART_IsActiveFlag_RXNE(CONSOLE_PORT))
	{
		uint8_t c = LL_USART_ReceiveData8(CONSOLE_PORT);
		M_EVENT_POST(ConsoleDrv_Receive, c);
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
	}
}

void ConsoleHal_Write(uint8_t c)
{
	LL_USART_TransmitData8(CONSOLE_PORT, c);
}

bool ConsoleHal_TxReady()
{
	return (LL_USART_IsActiveFlag_TXE(CONSOLE_PORT));
}

#endif /* CONSOLE_HAL_C_ */
