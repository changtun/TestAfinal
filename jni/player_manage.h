/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : player_manage.h
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/7/26
  Last Modified :
  Description   : player_manage.cpp header file
  Function List :
  History       :
  1.Date        : 2012/7/26
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

#ifndef __PLAYER_MANAGE_H__
#define __PLAYER_MANAGE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern bool _logout_player_msg_callback_methods(JNIEnv *env);
extern void _playerMsgInterrupt(JNIEnv *env, jobject obj);
extern void _playerMsgPmtTimeOut(JNIEnv *env, jobject obj);
extern void _playerMsgAVinterrupt(JNIEnv *env, jobject obj);

extern void _playerMsgTunerLock(JNIEnv *env, jobject obj);
extern void _playerMsgStart(JNIEnv *env, jobject obj);
extern bool _register_player_msg_callback_methods(JNIEnv *env);
extern void _playerMsgAudioInfos(JNIEnv *env, jobject obj,void *temp,int totals );
extern void _playerCaMode(JNIEnv *env, jobject obj,int mode);
extern void _playerMsgUpdatePmtInfo(JNIEnv *env, jobject obj, int TsId, int ServId, int PmtPid );

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PLAYER_MANAGE_H__ */
