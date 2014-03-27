/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : pvr_message.cpp
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/12/10
  Last Modified :
  Description   : pvr message
  Function List :
              Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrMessageInit
              PvrEventCallBack
  History       :
  1.Date        : 2012/12/10
    Author      : zxguan
    Modification: Created file

******************************************************************************/

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
#include <stdio.h>
#include <limits.h>
#include <jni.h>
#include <android_runtime/AndroidRuntime.h>

#include <dvbplayer.h>
#include "logit.h"
#include "jnistaple.h"
#include "pvr_message.h"
#include "pvr_manage.h"
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
static JavaVM*	   g_PvrVM = NULL;
static jobject g_PvrMsgObj = NULL;
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrMessageInit
(JNIEnv *env, jobject obj, jobject PvrMsgObj)
{
	DeleteGlobalRef( env, g_PvrMsgObj);
	g_PvrMsgObj = NULL;

	env->GetJavaVM(&g_PvrVM);
	
	if( TRUE != _register_pvr_msg_callback_methods( env ))
	{
		return 1;
	}

	DVBPlayer_PVR_MessageInit((DvbPvrEventCallBack)PvrEventCallBack);
	
	g_PvrMsgObj = env->NewGlobalRef(PvrMsgObj);
	DEBUG_MSG(-1, "PlayerMessageInit ok");
	return 0;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrMessageUnInit
  (JNIEnv *env, jobject obj )
{	
	DVBPlayer_PVR_MessageUnInit();
	JNI_PvrMsg_Logout(env);
	return 0;
}

void JNI_PvrMsg_Logout(JNIEnv *env)
{	
	_logout_pvr_msg_callback_methods(env);
	DeleteGlobalRef( env, g_PvrMsgObj );
	g_PvrMsgObj = NULL;
	if( NULL != g_PvrVM )
	{
		g_PvrVM = NULL;
	}
}

void PvrEventCallBack(signed int  u32ChnID, DVBPlayer_PVR_EVENT_TYPE_E EventType, int s32EventValue, void *args)
{
	JNIEnv *env = NULL;
	if( NULL == g_PvrVM || NULL == g_PvrMsgObj )
	{
		//DEBUG_MSG(-1,T("Installation_Call_Back-- ---------- mjvm or obj is null--------------------"));
		return;
	}

	if (g_PvrVM->AttachCurrentThread(&env, NULL) != JNI_OK)
	{
		 DEBUG_MSG1(-1,T("%s: AttachCurrentThread() failed"), __FUNCTION__);
		 return;
	}
	if( NULL == env )
	{
		DEBUG_MSG1(-1,T("%s: new is null "), __FUNCTION__);
		return;
	}
	

	DEBUG_MSG3(-1, "PvrEventCallBack : ChnID 0x%x...EventType %d...EventValue %d...", u32ChnID, EventType, s32EventValue );

	switch( EventType )
	{
		case PVR_EVENT_TYPE_PLAY_EOF:
		{
			_PvrPlayMsgEof ( env, g_PvrMsgObj );
		}break;
		case PVR_EVENT_TYPE_PLAY_SOF:
		{
			_PvrPlayMsgSof ( env, g_PvrMsgObj );
		}break;
		case PVR_EVENT_TYPE_PLAY_ERROR:
		{
			_PvrPlayMsgError ( env, g_PvrMsgObj );
		}break;
		case PVR_EVENT_TYPE_PLAY_REACH_REC:
		{
			_PvrPlayMsgReachRec ( env, g_PvrMsgObj );
		}break;
		case PVR_EVENT_TYPE_REC_DISKFULL:
		{
			_PvrRecMsgDiskFull ( env, g_PvrMsgObj );
		}break;
		case PVR_EVENT_TYPE_REC_ERROR:
		{
			_PvrRecMsgError ( env, g_PvrMsgObj );
		}break;
		case PVR_EVENT_TYPE_REC_OVER_FIX:
		{
			_PvrRecMsgOverFix ( env, g_PvrMsgObj );
		}break;
		case PVR_EVENT_TYPE_REC_REACH_PLAY:
		{
			_PvrRecMsgReachPlay ( env, g_PvrMsgObj );
		}break;
		case PVR_EVENT_TYPE_REC_DISK_SLOW:
		{
			_PvrRecMsgDiskSlow ( env, g_PvrMsgObj );
		}break;
		default:
		{

		}break;
	}


	if(g_PvrVM->DetachCurrentThread() != JNI_OK) 
	{
	 	DEBUG_MSG1(-1,T("%s: DetachCurrentThread() failed"), __FUNCTION__); 
	}

}

