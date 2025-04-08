#include "controller.h"
#include "lcd.h"
#include <string.h>

void Lcd_Ctrl_Init()
{
	Lcd_Drv_Init();
	Strand_Delay(&lcd_drvStrand, 500);
	Lcd_Ctrl_Init4Bit();
	Lcd_Ctrl_Clear();
}

void Lcd_Ctrl_Init4Bit()
{
	Lcd_Drv_Send(SEND_CMD, 0x20);	// fake Command
	Lcd_Drv_Send(SEND_CMD, M_4_BIT);
	Lcd_Drv_Send(SEND_CMD, DISPLAY_CONTROL);
	Lcd_Drv_Send(SEND_CMD, ENTRY_MODE);
}

void Lcd_Ctrl_Clear()
{
	Lcd_Drv_Send(SEND_CMD, CLEAR_LCD);
}

void Lcd_Ctrl_Home()
{
	Lcd_Drv_Send(SEND_CMD, HOME_CURSOR);
}

void Lcd_Ctrl_Gotoxy(uint8_t col, uint8_t row)
{
	uint16_t address;

    if(row!=0){address=0x40;}
    else{address=0;}

    address += col;

//	pos=64*(x-1)+(y-1)+0x80; // tính mã lệnh
	Lcd_Drv_Send(SEND_CMD, 0x80|address);
}

void Lcd_Ctrl_Putchar(char data)
{
	Lcd_Drv_Send(SEND_DATA, data);
}

void Lcd_Ctrl_Printf(char* data)
{
	uint8_t i, len;
	len = strlen(data);
	for (i=0; i<len; i++)
	{
		if(data[i] > 0) Lcd_Ctrl_Putchar(data[i]);
		else Lcd_Ctrl_Putchar(' ');
	}
}
