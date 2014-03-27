/******************************************************************************
  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.
 *******************************************************************************
 File Name     : hi_type.h
Version       : 1.00
Author        : Hisilicon Embedded System Software Group
Created       : 2006/7/18
Last Modified :
Description   : 
Function List :
History       :
 ******************************************************************************/

#ifndef __HI_TYPE_H__
#define __HI_TYPE_H__

#ifdef __cplusplus
extern "C"{
#endif

#define HI_OS_LINUX 0
#define HI_OS_WIN32 1

#ifdef _WIN32
#define HI_OS_TYPE HI_OS_WIN32
#else
#define HI_OS_TYPE HI_OS_LINUX
#endif


typedef unsigned char       HI_U8;
typedef unsigned char       HI_UCHAR;
typedef unsigned short      HI_U16;
typedef unsigned int        HI_U32;
#if HI_OS_TYPE == HI_OS_LINUX
typedef unsigned long long  HI_U64;
#elif HI_OS_TYPE == HI_OS_WIN32
typedef unsigned __int64    HI_U64;
#endif

typedef char                HI_S8;
typedef short               HI_S16;
typedef int                 HI_S32;

typedef long                HI_LONG;
typedef unsigned long       HI_UL;

#if HI_OS_TYPE == HI_OS_LINUX
typedef  long long          HI_S64;
#elif HI_OS_TYPE == HI_OS_WIN32
typedef  __int64            HI_S64;
#endif

typedef char                HI_CHAR;
typedef char*               HI_PCHAR;

typedef float               HI_FLOAT;
typedef double              HI_DOUBLE;
typedef void                HI_VOID;

typedef HI_U64              HI_PTS_TIME;
typedef unsigned long       HI_SIZE_T;
typedef unsigned long       HI_LENGTH_T;

typedef int                 STATUS;

typedef enum 
{
    HI_FALSE    = 0,
    HI_TRUE     = 1,
} HI_BOOL;

typedef HI_U32 HI_HANDLE;

#ifndef NULL
#define NULL                (0U)
#endif

#define HI_NULL             (0U)
#define HI_NULL_PTR         (0U)

#define HI_SUCCESS          (0)
#define HI_FAILURE          (-1)

#define HI_LITTLE_ENDIAN    (1234)
#define HI_BIG_ENDIAN       (4321)

/*#define LOCAL             static */  /* OSR use */

/* return status values */
/* #define OK               0  */      /* OSR use */
/* #define ERROR            (-1) */     /* OSR use */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* __HI_TYPE_H__ */

