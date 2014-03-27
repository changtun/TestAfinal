/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : pvr_manage.h
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/12/10
  Last Modified :
  Description   : pvr_manage.cpp header file
  Function List :
  History       :
  1.Date        : 2012/12/10
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

#ifndef __PVR_MANAGE_H__
#define __PVR_MANAGE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern bool _register_pvr_msg_callback_methods(JNIEnv *env);
extern bool _logout_pvr_msg_callback_methods(JNIEnv *env);
extern void _PvrPlayMsgEof (JNIEnv *env, jobject obj);
extern void _PvrPlayMsgError (JNIEnv *env, jobject obj);
extern void _PvrPlayMsgReachRec (JNIEnv *env, jobject obj);
extern void _PvrPlayMsgSof (JNIEnv *env, jobject obj);
extern void _PvrRecMsgDiskFull (JNIEnv *env, jobject obj);
extern void _PvrRecMsgDiskSlow (JNIEnv *env, jobject obj);
extern void _PvrRecMsgError (JNIEnv *env, jobject obj);
extern void _PvrRecMsgOverFix (JNIEnv *env, jobject obj);
extern void _PvrRecMsgReachPlay (JNIEnv *env, jobject obj);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PVR_MANAGE_H__ */
