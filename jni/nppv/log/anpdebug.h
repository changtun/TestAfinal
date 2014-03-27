/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : anpdebug.h
* Description : Includes all other STAPI driver header files.
              	This allows a STAPI application to include only this one header
              	file directly.
* Author : LSZ
* History :
*	2006/01/24 : Initial Version
******************************************************************************/
#ifndef __ANP_DEBUG_H
#define __ANP_DEBUG_H

/*******************************************************/
/*              Includes				                   */
/*******************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <android/log.h>


#ifdef __cplusplus
extern "C" {
#endif

    /*******************************************************/
    /*              Exported Defines and Macros            */
    /*******************************************************/

#define LOG_TAG "DVBNPPV"
#define LOGI(a)  __android_log_write(ANDROID_LOG_INFO,LOG_TAG,a)

    typedef int bool_t;
#define TSIZEOF(name)	(sizeof(name)/sizeof(tchar_t))
#define min(a, b)  (((a) < (b)) ? (a) : (b))


#ifdef UNICODE

#if defined(__GNUC__)
#include <wchar.h>
#endif

#if defined(__GNUC__) && defined(__cplusplus)
    typedef __wchar_t tchar_t;
#else
    typedef unsigned short tchar_t;
#endif

#define tcsstr wcsstr
#define tcslen wcslen
#define tcscmp wcscmp
#define tcsncmp wcsncmp
#define tcschr wcschr
#define tcsrchr wcsrchr
#define T(a) L ## a

#ifdef TARGET_SYMBIAN
    extern int wcsncmp(const tchar_t *, const tchar_t *, size_t);
    extern tchar_t *wcschr(const tchar_t *, tchar_t);
    extern tchar_t *wcsrchr(const tchar_t *, tchar_t);
#endif

#else
    typedef char tchar_t;
#define tcsstr strstr
#define tcslen strlen
#define tcscmp strcmp
#define tcsncmp strncmp
#define tcschr strchr
#define tcsrchr strrchr
#define T(a) a
#endif



    /*******************************************************/
    /*              Exported Types			                */
    /*******************************************************/

    /*******************************************************/
    /*              Exported Variables		                */
    /*******************************************************/

    /*******************************************************/
    /*              External				                   */
    /*******************************************************/

    /*******************************************************/
    /*              Exported Functions		                */
    /*******************************************************/

    bool_t IsSpace(int);
    bool_t IsAlpha(int);
    bool_t IsDigit(int);

    int Hex(int);
    void tcscpy_s(tchar_t *Out, size_t OutLen, const tchar_t *In);
    int stscanf(const tchar_t *In, const tchar_t *Mask, ...);
    void tcscat_s(tchar_t *Out, size_t OutLen, const tchar_t *In);
    void vstprintf_s(tchar_t *Out, size_t OutLen, const tchar_t *Mask, va_list Arg);

    void DebugMessage(const tchar_t *, ...);
    void DebugMessage_Switch(int value);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __ANP_DEBUG_H */
/* End of anp_debug.h  --------------------------------------------------------- */
