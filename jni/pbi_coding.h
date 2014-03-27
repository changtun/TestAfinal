/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : pbi_coding.h
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/9/14
  Last Modified :
  Description   : pbi_coding.cpp header file
  Function List :
  History       :
  1.Date        : 2012/9/14
    Author      : zxguan
    Modification: Created file

******************************************************************************/

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

#ifndef __PBI_CODING_H__
#define __PBI_CODING_H__

#define UTF8	"UTF-8"
#define UTF16	"UTF-16"
#define GBK  	"GBK"
#define BIG5	"BIG-5"
#define GB2312	"GB2312"
#define UNICODE "UNICODE-1-1"

#define CHARACTER_TYPE_IEC10646 	0x11
#define CHARACTER_TYPE_KSX		0x12
#define CHARACTER_TYPE_GB2312 	0x13
#define	CHARACTER_TYPE_BIG5		0x14
#define	CHARACTER_TYPE_UTF8		0x15
#define	CHARACTER_TYPE_DEFAULT	0xFF


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern int Big5ToUtf8(char* inbuf, int inlen, char* outbuf, int outlen);
extern int GbkToUtf8(char* inbuf, int inlen, char* outbuf, int outlen);
extern int Utf16ToUtf8(char* inbuf, int inlen, char* outbuf, int outlen);

extern jstring CodingToJstring( JNIEnv *env, int CodeType, char *str, int strLen );
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PBI_CODING_H__ */
