/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : installation_manage.h
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/7/16
  Last Modified :
  Description   : installation_manage.cpp header file
  Function List :
  History       :
  1.Date        : 2012/7/16
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

#ifndef __INSTALLATION_MANAGE_H__
#define __INSTALLATION_MANAGE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern bool _register_message_callback_methods(JNIEnv *env);
extern bool _logout_message_callback_methods(JNIEnv *env);
extern void _Install_Completed(JNIEnv *env, jobject obj );
extern void _Install_Failed(JNIEnv *env, jobject obj );
extern void _Install_Interrupt(JNIEnv *env, jobject obj );
extern void _Install_Progress( JNIEnv *env, jobject obj, Install_Progress_t *progress );
extern void _Install_TP_Completed( JNIEnv *env, jobject obj, Install_Search_Result_t *ptIsr );
extern void _Install_TP_Failed( JNIEnv *env, jobject obj );

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __INSTALLATION_MANAGE_H__ */
