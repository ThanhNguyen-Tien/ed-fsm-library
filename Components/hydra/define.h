#ifndef HYDRA_DEFINE_H_
#define HYDRA_DEFINE_H_

#define USE_DMA		1

#if USE_DMA == 1
/* ==========================================================
 * 1. USER CONFIG
 * ========================================================== */
#define DMA_MODULE            DMA1

#define DMA_TX_CHANNEL_NUM     6
#define DMA_RX_CHANNEL_NUM     5

/* ==========================================================
 * 2. INTERNAL TOKEN-PASTE HELPERS (DO NOT TOUCH)
 * ========================================================== */

/* ---- Stream ---- */
#define __DMA_STREAM(num)              LL_DMA_STREAM_##num
#define _DMA_STREAM(num)               __DMA_STREAM(num)

/* ---- IRQ handler ---- */
#define __DMA_IRQ_HANDLER(num)         DMA1_Stream##num##_IRQHandler
#define _DMA_IRQ_HANDLER(num)          __DMA_IRQ_HANDLER(num)

/* ---- TC flag ---- */
#define __DMA_TC(num)                  LL_DMA_IsActiveFlag_TC##num
#define _DMA_TC(num)                   __DMA_TC(num)(DMA_MODULE)

#define __DMA_TC_CLR(num)              LL_DMA_ClearFlag_TC##num
#define _DMA_TC_CLR(num)               __DMA_TC_CLR(num)(DMA_MODULE)

/* ---- TE flag ---- */
#define __DMA_TE(num)                  LL_DMA_IsActiveFlag_TE##num
#define _DMA_TE(num)                   __DMA_TE(num)(DMA_MODULE)

#define __DMA_TE_CLR(num)              LL_DMA_ClearFlag_TE##num
#define _DMA_TE_CLR(num)               __DMA_TE_CLR(num)(DMA_MODULE)

/* ==========================================================
 * 3. STREAM HANDLE
 * ========================================================== */

#define DMA_TX_CHANNEL        _DMA_STREAM(DMA_TX_CHANNEL_NUM)
#define DMA_RX_CHANNEL        _DMA_STREAM(DMA_RX_CHANNEL_NUM)

/* ==========================================================
 * 4. ISR DECLARATION (C++ SAFE)
 * ========================================================== */

#define DMA_TX_ISR()   void _DMA_IRQ_HANDLER(DMA_TX_CHANNEL_NUM)(void)
#define DMA_RX_ISR()   void _DMA_IRQ_HANDLER(DMA_RX_CHANNEL_NUM)(void)

/* ==========================================================
 * 5. TX DMA SHORTCUT
 * ========================================================== */

#define DMA_TX_TC_ACTIVE()    _DMA_TC(DMA_TX_CHANNEL_NUM)
#define DMA_TX_TC_CLEAR()     _DMA_TC_CLR(DMA_TX_CHANNEL_NUM)

#define DMA_TX_TE_ACTIVE()    _DMA_TE(DMA_TX_CHANNEL_NUM)
#define DMA_TX_TE_CLEAR()     _DMA_TE_CLR(DMA_TX_CHANNEL_NUM)

/* ==========================================================
 * 6. RX DMA SHORTCUT
 * ========================================================== */

#define DMA_RX_TC_ACTIVE()    _DMA_TC(DMA_RX_CHANNEL_NUM)
#define DMA_RX_TC_CLEAR()     _DMA_TC_CLR(DMA_RX_CHANNEL_NUM)

#define DMA_RX_TE_ACTIVE()    _DMA_TE(DMA_RX_CHANNEL_NUM)
#define DMA_RX_TE_CLEAR()     _DMA_TE_CLR(DMA_RX_CHANNEL_NUM)
#endif

//OSCILLOSCOPE
#define NUM_OF_SAMPLES	4096

//CONTROLLER
#define TIME_PLOT	3	//ms
#define MAX_ONE (1<<7)
#define MAX_TWO (1<<15)

//DRIVER
#define TX_BUF_SIZE   		4096

#define HEADER_INDICATOR	0xFEU
#define FOOTER_INDICATOR	0xFDU
#define MAX_PACKET_LENGTH	32

//HAL
#define HYDRA_PORT				USART2
#define HYDRA_INIT()			MX_USART2_UART_Init()
#define HYDRA_ISR_HANDLER()		void USART2_IRQHandler(void)

#endif /* HYDRA_DEFINE_H_ */
