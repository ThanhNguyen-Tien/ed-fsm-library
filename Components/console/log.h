#ifndef CONSOLE_LOG_H_
#define CONSOLE_LOG_H_

//#ifdef DEBUG
#include <console/controller.h>

#define LOG_PRINT 	console::Controller::instance().print
#define LOG_PRINTF 	console::Controller::instance().printf
#define MC_PLOT 	console::Controller::instance().plot

//#else
//
//#define LOG_PRINT(...)
//#define LOG_PRINTF(...)
//
//#endif


#endif /* LOG_H_ */
