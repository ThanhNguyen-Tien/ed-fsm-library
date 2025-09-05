#include <console/log.h>
#include "sub-c.h"

M_OBS_SUBJECT(c_)

void Sub_C_Init()
{
	M_OBS_SUBJECT_INIT(c_Subject, sizeof(float));
}
