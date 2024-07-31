#include "system.h"
#include "engine.h"
#include "gpio.h"

uint8_t evQueue[EVENT_QUEUE_SIZE];
uint8_t tempDataForHandler[MAX_EVENT_SIZE];
event_t* evPool[EVENT_POOL_SIZE];

void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
  Engine_CheckTask();
  /* USER CODE END SysTick_IRQn 1 */
}

void systemInit()
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
}



