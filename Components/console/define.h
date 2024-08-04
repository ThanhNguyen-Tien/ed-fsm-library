#ifndef CONSOLE_DEFINE_H_
#define CONSOLE_DEFINE_H_

#define USE_DMA		0

#if USE_DMA == 1
#define DMA_MODULE					DMA1
#define DMA_TX_CHANNEL				LL_DMA_CHANNEL_5
#define DMA_RX_CHANNEL				LL_DMA_CHANNEL_6
#define DMA_TX_ISR()				void DMA1_Channel5_IRQHandler(void)
#define DMA_RX_ISR()				void DMA1_Channel6_IRQHandler(void)
#define DMA_CHECK_TC_FLAG(num)		LL_DMA_IsActiveFlag_TC##num(DMA_MODULE)
#define DMA_CHECK_TE_FLAG(num)		LL_DMA_IsActiveFlag_TE##num(DMA_MODULE)
#define DMA_CLEAR_TC_FLAG(num)		LL_DMA_ClearFlag_TC##num(DMA_MODULE)
#define DMA_CLEAR_TE_FLAG(num)		LL_DMA_ClearFlag_TE##num(DMA_MODULE)
#endif

//CONTROLLER
#define TIME_PLOT	5	//ms
#define MAX_ONE (1<<7)
#define MAX_TWO (1<<15)

//DRIVER
#define TX_BUF_SIZE   		512
#define HEADER_INDICATOR	0xFE
#define FOOTER_INDICATOR	0xFD
#define MAX_PACKET_LENGTH	32

//HAL
#define CONSOLE_PORT			USART2
#define CONSOLE_INIT()			MX_USART2_UART_Init()
#define CONSOLE_ISR_HANDLER()	void USART2_IRQHandler(void)

#endif /* CONSOLE_DEFINE_H_ */
