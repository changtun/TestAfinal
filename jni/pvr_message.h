/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : pvr_message.h
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/12/10
  Last Modified :
  Description   : Pvr_message.cpp header file
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

#ifndef __PVR_MESSAGE_H__
#define __PVR_MESSAGE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrMessageInit
         (JNIEnv *env, jobject obj, jobject PvrMsgObj);

extern JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrMessageUnInit
  (JNIEnv *env, jobject obj );

extern void JNI_PvrMsg_Logout(JNIEnv *env);

extern void PvrEventCallBack(signed int  u32ChnID, DVBPlayer_PVR_EVENT_TYPE_E EventType, int s32EventValue, void *args);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PVR_MESSAGE_H__ */
