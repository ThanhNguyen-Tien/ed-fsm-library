#ifndef LCD1602_LCD_H_
#define LCD1602_LCD_H_

#include <core/engine.h>
#include <core/machine.h>
#include <core/strand.h>
#include <core/event.h>
#include <core/task.h>
#include "lcd-define.h"

M_TASK_DEF(timeout)
M_EVENT_DEF(send)
M_STRAND_DEF(lcd_drv)

void Lcd_Drv_Init();
void Lcd_Drv_Write2Nib(uint8_t data);
void Lcd_Drv_Write8Bit(uint8_t data);
void Lcd_Drv_Send(uint8_t type, uint8_t data);

typedef struct LcdData
{
	uint8_t type;
	uint8_t data;
}lcd_data_t;

enum LCD_EVENT {LCD_DRV_TIMEOUT, LCD_DRV_SEND_CMD, LCD_DRV_SEND_DATA};
enum LCD_TYPE {Command = 0, Data};

MACHINE_DEF(lcd_drv)
STATE_DEF(Lcd_Drv_Ready)
STATE_DEF(Lcd_Drv_Busy)

#endif /* LCD1602_LCD_H_ */
