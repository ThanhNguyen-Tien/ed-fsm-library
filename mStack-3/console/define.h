#ifndef CONSOLE_DEFINE_H_
#define CONSOLE_DEFINE_H_

/*CONTROLLER*/
#define TIME_PLOT	2	//ms

/*DRIVER*/
#define TX_BUF_SIZE   		2048
#define HEADER_INDICATOR	0xFE
#define FOOTER_INDICATOR	0xFD
#define MAX_PACKET_LENGTH	64

/*HAL*/
#define CONSOLE_PORT			USART2
#define CONSOLE_INIT			MX_USART2_UART_Init()
#define CONSOLE_ISR_HANDLER()	extern "C" void USART2_IRQHandler(void)

#define USING_DMA

#ifdef USING_DMA
#define DMA_MODULE					DMA1
#define DMA_TX_CHANNEL				LL_DMA_STREAM_6
#define DMA_RX_CHANNEL				LL_DMA_STREAM_5
#define DMA_TX_ISR()				extern "C" void DMA1_Stream6_IRQHandler(void)
#define DMA_RX_ISR()				extern "C" void DMA1_Stream5_IRQHandler(void)
#define DMA_CHECK_TC_FLAG(num)		LL_DMA_IsActiveFlag_TC##num(DMA_MODULE)
#define DMA_CHECK_TE_FLAG(num)		LL_DMA_IsActiveFlag_TE##num(DMA_MODULE)
#define DMA_CLEAR_TC_FLAG(num)		LL_DMA_ClearFlag_TC##num(DMA_MODULE)
#define DMA_CLEAR_TE_FLAG(num)		LL_DMA_ClearFlag_TE##num(DMA_MODULE)
#endif

#endif /* CONSOLE_DEFINE_H_ */
