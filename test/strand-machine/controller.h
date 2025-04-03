#ifndef LCD1602_Lcd_Ctrl_H_
#define LCD1602_Lcd_Ctrl_H_

#include <core/engine.h>
#include "lcd.h"

void Lcd_Ctrl_Init();
void Lcd_Ctrl_Init4Bit();
void Lcd_Ctrl_Init8Bit();

void Lcd_Ctrl_Home();							// cursor goto home position
void Lcd_Ctrl_Gotoxy(uint8_t x, uint8_t y);	// move cursor to x,y
void Lcd_Ctrl_Clear();
void Lcd_Ctrl_Putchar(char data);
void Lcd_Ctrl_printf(char* data);

#endif /* LCD1602_Lcd_Ctrl_H_ */
