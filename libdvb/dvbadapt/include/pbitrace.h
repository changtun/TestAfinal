#ifndef __PV_TRACE_H_
#define __PV_TRACE_H_


#define LOG_TAG "DVBCore"
#include <android/log.h>

#if 1
#define pbiinfo(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define pbiwarning(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define pbierror(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define pbifatal(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)
#else
#define pbiinfo(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define pbiwarning printf
#define pbierror printf
#define pbifatal  printf
#endif

#define DEBUG_LOG  __FUNCTION__,__LINE__
#define DEBUG_VER "20131218", "18:41"
#define PBIDEBUG(str) pbiinfo("[%s %d]  %s\n", DEBUG_LOG, str )

#endif /* __PV_TRACE_H_ */
