/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : player_message.h
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/7/26
  Last Modified :
  Description   : player_message.cpp header file
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

#ifndef __PLAYER_MESSAGE_H__
#define __PLAYER_MESSAGE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerMessageInit
         (JNIEnv *env, jobject obj, jobject PlayerMsgObj);
extern void JNI_PlayerMsg_Logout(JNIEnv *env);
extern int Player_Message_CallBack(DVBPlayer_Event_t tEvent);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PLAYER_MESSAGE_H__ */
