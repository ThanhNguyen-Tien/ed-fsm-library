#ifndef CONSOLE_DEFINE_H_
#define CONSOLE_DEFINE_H_

//CONTROLLER
#define TIME_PLOT	2	//ms
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
