#include "test.h"
#include "lcd1602/controller.h"
#include <console/log.h>

U_ACTION(clear)
U_ACTION(show_line_1)
U_ACTION(show_line_2)
U_TEXT(text)

char lcdBuf[16] = {0};

void Test_Init()
{
	ConsoleCtrl_Init();

	CONSOLE_REGISTER_COMMAND(100, text);
	CONSOLE_REGISTER_COMMAND(101, show_line_1);
	CONSOLE_REGISTER_COMMAND(102, show_line_2);
	CONSOLE_REGISTER_COMMAND(103, clear);

	Lcd_Ctrl_Init();
	LOG_PRINT("TEST STRAND MACHINE");
}

U_ACTION_HANDLER(clear)
{
	LOG_PRINT("Clear LCD");
	Lcd_Ctrl_Clear();
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
}

U_ACTION_HANDLER(show_line_1)
{
	LOG_PRINT("Show Line 1");
	Lcd_Ctrl_Gotoxy(1, 0);
	Lcd_Ctrl_Printf(lcdBuf);
}

U_ACTION_HANDLER(show_line_2)
{
	LOG_PRINT("Show Line 2");
	Lcd_Ctrl_Gotoxy(3, 1);
	Lcd_Ctrl_Printf(lcdBuf);
}

U_TEXT_HANDLER(text)
{
	size_t i = 0;
	for(; i < length; i++)
	{
		lcdBuf[i] = data[i];
	}
	lcdBuf[i] = '\0';
	LOG_PRINTF("%s", lcdBuf);
}
