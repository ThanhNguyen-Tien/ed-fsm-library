#include <console/log.h>
#include "sub-a.h"

M_OBS_SUBJECT(a_)

void Sub_A_Init()
{
	M_OBS_SUBJECT_INIT(a_Subject, sizeof(float));
}
