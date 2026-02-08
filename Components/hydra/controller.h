#ifndef HYDRA_CONTROLLER_H_
#define HYDRA_CONTROLLER_H_

#include <stdint.h>
#include <stdbool.h>
#include "driver.h"
#include "command.h"
#include "define.h"
#include <core/task.h>

M_TASK_DEF(HydraCtrl_Plot)

typedef struct HydraController
{
	command_t *commands;
	int32_t plotValues[8];
	uint32_t errorCount;
	uint8_t plotReg;
} hydra_ctrl_t;
extern hydra_ctrl_t hydraCtrl;

void HydraCtrl_Init(void);
void HydraCtrl_Print(uint16_t level ,const char *text);
void HydraCtrl_Printf(uint16_t level ,const char *format, ...);

command_t* HydraCtrl_RegisterCommand(uint16_t type, ConmmandHandler handler);
void HydraCtrl_UpdateIntField(uint16_t type, int32_t value);
void HydraCtrl_UpdateTextField(uint16_t type, uint8_t length, uint8_t *data);
void HydraCtrl_Plot(uint8_t channel, int32_t value);
void HydraCtrl_ProcessCommand(uint16_t type, uint8_t length, uint8_t *data);
bool HydraCtrl_SendOSC(uint8_t channel, uint8_t length, uint16_t index,
		const uint8_t *data);

#endif /* HYDRA_CONTROLLER_H_ */
