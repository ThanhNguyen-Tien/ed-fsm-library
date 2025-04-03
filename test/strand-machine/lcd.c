#include "lcd.h"
#include "gpio.h"
#include <console/log.h>

M_TASK(timeout)
M_EVENT(send)
M_STRAND(lcd_drv, 256)
MACHINE(lcd_drv)

lcd_data_t data_;

void Lcd_Drv_Init()
{
	HAL_GPIO_WritePin(RS_PORT, RS_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(E_PORT, E_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(D4_PORT, D4_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(D5_PORT, D5_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(D6_PORT, D6_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(D7_PORT, D7_PIN, GPIO_PIN_RESET);

	M_TASK_INIT(timeout);
	M_EVENT_INIT(send, sizeof(lcd_data_t));
	M_STRAND_INIT(lcd_drv);

	SM_START(lcd_drv, Lcd_Drv_Ready);
}

void Lcd_Drv_Write2Nib(uint8_t data)
{
	uint8_t hNib, lNib;
	hNib = data >> 4;
	lNib = data & 0x0F;

	HAL_GPIO_WritePin(E_PORT, E_PIN, GPIO_PIN_SET);

	HAL_GPIO_WritePin(D7_PORT, D7_PIN, (GPIO_PinState)((hNib>>3)&0x01));
	HAL_GPIO_WritePin(D6_PORT, D6_PIN, (GPIO_PinState)((hNib>>2)&0x01));
	HAL_GPIO_WritePin(D5_PORT, D5_PIN, (GPIO_PinState)((hNib>>1)&0x01));
	HAL_GPIO_WritePin(D4_PORT, D4_PIN, (GPIO_PinState)((hNib)&0x01));

	HAL_GPIO_WritePin(E_PORT, E_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(E_PORT, E_PIN, GPIO_PIN_SET);

	HAL_GPIO_WritePin(D7_PORT, D7_PIN, (GPIO_PinState)((lNib>>3)&0x01));
	HAL_GPIO_WritePin(D6_PORT, D6_PIN, (GPIO_PinState)((lNib>>2)&0x01));
	HAL_GPIO_WritePin(D5_PORT, D5_PIN, (GPIO_PinState)((lNib>>1)&0x01));
	HAL_GPIO_WritePin(D4_PORT, D4_PIN, (GPIO_PinState)((lNib)&0x01));

	HAL_GPIO_WritePin(E_PORT, E_PIN, GPIO_PIN_RESET);
}

void Lcd_Drv_Write8Bit(uint8_t data)
{
	// TODO: use in 8bit mode
}

void Lcd_Drv_Send(uint8_t type, uint8_t data)
{
	lcd_data_t tmp;
	tmp.data = data; tmp.type = type;
	Strand_Post(&lcd_drvStrand, &sendEvent, NULL, &tmp);
}

M_TASK_HANDLER(timeout)
{
	SM_POST(lcd_drv, LCD_DRV_TIMEOUT);
}

M_EVENT_HANDLER(send)
{
	lcd_data_t* payload = (lcd_data_t*)data;
	data_ = (*payload);
	if(data_.type == SEND_CMD) SM_POST(lcd_drv, LCD_DRV_SEND_CMD);
	else SM_POST(lcd_drv, LCD_DRV_SEND_DATA);
}
