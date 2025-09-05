#include <console/log.h>
#include "sub-b.h"

M_OBS_SUBJECT(b_)

void Sub_B_Init()
{
	M_OBS_SUBJECT_INIT(b_Subject, sizeof(uint16_t));
}
