#include "lcd.h"
#include <console/log.h>
#include "gpio.h"

extern lcd_data_t data_;

STATE_BODY(Lcd_Drv_Ready)
{
	ENTER_()
	{
		LOG_PRINT("[LCD_DRV]: Ready");
		Strand_Done(&lcd_drvStrand);
	}
	TRANSITION_(LCD_DRV_SEND_CMD, Lcd_Drv_Busy)
	{
		HAL_GPIO_WritePin(RS_PORT, RS_PIN, GPIO_PIN_RESET);
	}
	TRANSITION_(LCD_DRV_SEND_DATA, Lcd_Drv_Busy)
	{
		HAL_GPIO_WritePin(RS_PORT, RS_PIN, GPIO_PIN_SET);
	}
	EXIT_()
	{
		Lcd_Drv_Write2Nib(data_.data);
	}
}

STATE_BODY(Lcd_Drv_Busy)
{
	ENTER_()
	{
		LOG_PRINT("[LCD_DRV]: Busy");
		SM_TIMEOUT_START(lcd_drvMachine, 5, 1);
	}
	TIMEOUT_()
	{
		SM_SWITCH(Lcd_Drv_Ready);
	}
}

