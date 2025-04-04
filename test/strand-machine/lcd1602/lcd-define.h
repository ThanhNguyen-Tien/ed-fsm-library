#ifndef LCD1602_LCD_DEFINE_H_
#define LCD1602_LCD_DEFINE_H_

#define RS_PIN	GPIO_PIN_7
#define E_PIN	GPIO_PIN_6
#define D7_PIN	GPIO_PIN_12
#define D6_PIN	GPIO_PIN_13
#define D5_PIN	GPIO_PIN_14
#define D4_PIN	GPIO_PIN_15

#define RS_PORT	GPIOC
#define E_PORT	GPIOC
#define D7_PORT	GPIOB
#define D6_PORT	GPIOB
#define D5_PORT	GPIOB
#define D4_PORT	GPIOB

#define SEND_CMD		0x00
#define SEND_DATA		0x01


#define	M_8_BIT			0x38
#define	M_4_BIT			0x28

#define	DISPLAY_CONTROL	0x0C	// Enable LCD, Disable Cursor, Disable Blinking
#define ENTRY_MODE		0x06	// Display Up, No Shift

#define HOME_CURSOR		0x02
#define CLEAR_LCD		0x01

#endif /* LCD1602_LCD_DEFINE_H_ */
