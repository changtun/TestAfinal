
/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. 
*
* File name  : pvddefs.h
* Description : Include all the defs used for the other layers except platform.
              	This allows a application to include only this one header file directly. 
* Author : zheng dong sheng
* History :
*	2010/06/05 : Initial Version
******************************************************************************/
#ifndef _PVDDEFS_H
#define _PVDDEFS_H
#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************/
/*              Exported Defines and Macros                                                                           */
/*****************************************************************************/
/* Boolean type (values should be among TRUE and FALSE constants only) */
/*!!!BOOL is not allowed to use!!!*/
#ifndef DEFINED_BOOL
#define DEFINED_BOOL
typedef int BOOL;
#endif
/*!!!BOOL is not allowed to use!!!*/

/* BOOL type constant values */
#ifndef TRUE
    #define TRUE (1 == 1)
#endif
#ifndef FALSE
    #define FALSE (!TRUE)
#endif

/*****************************************************************************/
/*              Exported Types			                                                                               */
/*****************************************************************************/
/* Common unsigned types */
#ifndef DEFINED_U8
#define DEFINED_U8
typedef unsigned char  U8;
#endif

#ifndef DEFINED_U16
#define DEFINED_U16
typedef unsigned short U16;
#endif

#ifndef DEFINED_U32
#define DEFINED_U32
typedef unsigned int U32;
//typedef unsigned int HI_U32;
typedef unsigned int HI_Handle_t;
#endif

#ifndef DEFINED_U64
#define DEFINED_U64
typedef unsigned long long U64;
#endif

/* Common signed types */
#ifndef DEFINED_S8
#define DEFINED_S8
typedef signed char  S8;
#endif

#ifndef DEFINED_C8
#define DEFINED_C8
typedef char  C8;
#endif

#ifndef DEFINED_I32
#define DEFINED_I32
typedef int  I32;
#endif

#ifndef DEFINED_S16
#define DEFINED_S16
typedef signed short S16;
#endif

#ifndef DEFINED_S32
#define DEFINED_S32
typedef signed int   S32;
typedef S32 HI_ErrorCode_t;
#endif

#ifndef DEFINED_S64
#define DEFINED_S64
typedef signed long long S64;
#endif

/* General purpose string type */
typedef char* PV_String_t;

/* Revision structure */
typedef const char * PV_Revision_t;

/*****************************************************************************/
/*              Exported Variables		                                                                               */
/*****************************************************************************/

/*****************************************************************************/
/*              External				                                                                               */
/*****************************************************************************/

/*****************************************************************************/
/*              Exported Functions		                                                                               */
/*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef _PVDDEFS_H */
/* End of pvddefs.h  --------------------------------------------------------- */

