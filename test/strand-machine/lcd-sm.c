#include "lcd.h"
#include <console/log.h>
#include "gpio.h"

extern lcd_data_t data_;

STATE_BODY(Lcd_Drv_Ready)
{
	ENTER()
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
	EXIT()
	{
		Lcd_Drv_Write2Nib(data_.data);
	}
}

STATE_BODY(Lcd_Drv_Busy)
{
	ENTER()
	{
		LOG_PRINT("[LCD_DRV]: Busy");
		M_TASK_START(timeout, 2, 1);
	}
	TRANSITION_(LCD_DRV_TIMEOUT, Lcd_Drv_Ready){}
}

