#ifndef HYDRA_LOG_H_
#define HYDRA_LOG_H_

//#ifdef DEBUG
#include <hydra/controller.h>

#define HYDRA_LOG_SIMPLE_CALL(level, text) \
    hydra::Controller::instance().print(hydra::Controller::level, text)

#define HYDRA_LOG_FORMAT_CALL(level, format, ...) \
    hydra::Controller::instance().printf(hydra::Controller::level, format, ##__VA_ARGS__)

#define LOG_DEBUG_PRINT(text)       HYDRA_LOG_SIMPLE_CALL(LogDebug, text)
#define LOG_INFO_PRINT(text)        HYDRA_LOG_SIMPLE_CALL(LogInfo, text)
#define LOG_WARNING_PRINT(text)     HYDRA_LOG_SIMPLE_CALL(LogWarning, text)
#define LOG_ERROR_PRINT(text)       HYDRA_LOG_SIMPLE_CALL(LogError, text)
#define LOG_CRITICAL_PRINT(text)    HYDRA_LOG_SIMPLE_CALL(LogCritical, text)

#define LOG_DEBUG_PRINTF(format, ...)       HYDRA_LOG_FORMAT_CALL(LogDebug, format, ##__VA_ARGS__)
#define LOG_INFO_PRINTF(format, ...)        HYDRA_LOG_FORMAT_CALL(LogInfo, format, ##__VA_ARGS__)
#define LOG_WARNING_PRINTF(format, ...)     HYDRA_LOG_FORMAT_CALL(LogWarning, format, ##__VA_ARGS__)
#define LOG_ERROR_PRINTF(format, ...)       HYDRA_LOG_FORMAT_CALL(LogError, format, ##__VA_ARGS__)
#define LOG_CRITICAL_PRINTF(format, ...)    HYDRA_LOG_FORMAT_CALL(LogCritical, format, ##__VA_ARGS__)

#define MC_PLOT 	hydra::Controller::instance().plot

//#else
//
//#define LOG_DEBUG_PRINT(...)
//#define LOG_DEBUG_PRINTF(...)
//
//#endif


#endif /* LOG_H_ */
