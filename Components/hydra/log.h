#ifndef HYDRA_LOG_H_
#define HYDRA_LOG_H_

//#ifdef DEBUG
#include <hydra/controller.h>

#define HYDRA_LOG_SIMPLE_CALL(level, text) \
    HydraCtrl_Print(level, text)

#define HYDRA_LOG_FORMAT_CALL(level, format, ...) \
    HydraCtrl_Printf(level, format, ##__VA_ARGS__)

#define LOG_DEBUG_PRINT(text)       HYDRA_LOG_SIMPLE_CALL(Hydra_LogDebug, text)
#define LOG_INFO_PRINT(text)        HYDRA_LOG_SIMPLE_CALL(Hydra_LogInfo, text)
#define LOG_WARNING_PRINT(text)     HYDRA_LOG_SIMPLE_CALL(Hydra_LogWarning, text)
#define LOG_ERROR_PRINT(text)       HYDRA_LOG_SIMPLE_CALL(Hydra_LogError, text)
#define LOG_CRITICAL_PRINT(text)    HYDRA_LOG_SIMPLE_CALL(Hydra_LogCritical, text)
#define LOG_DEBUG_PRINTF(format, ...)       HYDRA_LOG_FORMAT_CALL(Hydra_LogDebug, format, ##__VA_ARGS__)
#define LOG_INFO_PRINTF(format, ...)        HYDRA_LOG_FORMAT_CALL(Hydra_LogInfo, format, ##__VA_ARGS__)
#define LOG_WARNING_PRINTF(format, ...)     HYDRA_LOG_FORMAT_CALL(Hydra_LogWarning, format, ##__VA_ARGS__)
#define LOG_ERROR_PRINTF(format, ...)       HYDRA_LOG_FORMAT_CALL(Hydra_LogError, format, ##__VA_ARGS__)
#define LOG_CRITICAL_PRINTF(format, ...)    HYDRA_LOG_FORMAT_CALL(Hydra_LogCritical, format, ##__VA_ARGS__)

#define MC_PLOT 	HydraCtrl_Plot

//#else
//
//#define LOG_DEBUG_PRINT(...)
//#define LOG_DEBUG_PRINTF(...)
//
//#endif


#endif /* LOG_H_ */
