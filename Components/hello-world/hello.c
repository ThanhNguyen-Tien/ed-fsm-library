#include "hello.h"
#include <hydra/log.h>
#include "tim.h"

M_TASK(blink)

U_ACTION(start)
U_ACTION(stop)

void hello_Init()
{
    M_TASK_INIT(blinkTask);
    M_TASK_START(blinkTask, 10);

    HYDRA_REGISTER_COMMAND(100, start);
    HYDRA_REGISTER_COMMAND(101, stop);

    MX_TIM4_Init();
    HAL_TIM_Base_Start_IT(&htim4);
}

M_TASK_HANDLER(blink)
{
	static uint32_t count;
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

    MC_PLOT(0, count++);
    MC_PLOT(1, count++);
    MC_PLOT(2, count++);
    MC_PLOT(3, count++);
    MC_PLOT(4, count++);
    MC_PLOT(5, count++);
    for (int32_t i = 0; i < 1000; i++) {
    
    }
}

U_ACTION_HANDLER(start)
{
    LOG_INFO_PRINT("START");
}

U_ACTION_HANDLER(stop)
{
    LOG_INFO_PRINT("STOP");
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	Engine_ISR_Enter();
	if(htim->Instance == htim4.Instance)
	{
		for(uint16_t i = 0; i < 700; i++)
		{
			NO_OPERATION;
		}
	}
}
