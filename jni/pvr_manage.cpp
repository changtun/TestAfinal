/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : pvr_manage.cpp
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/12/10
  Last Modified :
  Description   : pvr call message function
  Function List :
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

#include "logit.h"
#include "jnistaple.h"
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
static jclass g_PVRCls = NULL;
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/

bool _register_pvr_msg_callback_methods(JNIEnv *env)
{
	jclass jClass = NULL;
	jmethodID jMethodID = NULL;

	jClass = env->FindClass("com/pbi/dvb/dvbinterface/MessagePVR");//env->GetObjectClass(PlayerMsgObj);
	if( NULL == jClass )
	{
		DEBUG_MSG(-1, "_register_player_msg_callback_methods error3!");
		return false;
	}
	
	g_PVRCls = (jclass)env->NewGlobalRef((jobject)jClass);
	if( NULL == g_PVRCls )
	{
		DEBUG_MSG(-1, "_register_player_msg_callback_methods Msg error4!");
		return false;
	}

	DeleteLocalRef( env, jClass );

	DEBUG_MSG(-1,"_register_player_msg_callback_methods ok");
	return true;
}

bool _logout_pvr_msg_callback_methods(JNIEnv *env)
{
	DeleteGlobalRef( env, g_PVRCls );
	g_PVRCls = NULL;

	return true;
}

void _PvrPlayMsgEof (JNIEnv *env, jobject obj)
{
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_PVRCls, "PvrPlayMsgEof", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"PvrPlayMsgEof Method  is NULL  line=%d.",__LINE__);
	}

	env->CallVoidMethod( obj, Method, NULL );
	Method = NULL;
	DEBUG_MSG1(-1,"PvrPlayMsgEof end %d.",__LINE__);
}

void _PvrPlayMsgSof (JNIEnv *env, jobject obj)
{
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_PVRCls, "PvrPlayMsgSof", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"_PvrPlayMsgSof Method  is NULL  line=%d.",__LINE__);
	}

	env->CallVoidMethod( obj, Method, NULL );
	DEBUG_MSG1(-1,"_PvrPlayMsgSof end %d.",__LINE__);
}

void _PvrPlayMsgError (JNIEnv *env, jobject obj)
{
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_PVRCls, "PvrPlayMsgError", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"_PvrPlayMsgError Method  is NULL  line=%d.",__LINE__);
	}

	env->CallVoidMethod( obj, Method, NULL );
	DEBUG_MSG1(-1,"_PvrPlayMsgError end %d.",__LINE__);
}

void _PvrPlayMsgReachRec (JNIEnv *env, jobject obj)
{
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_PVRCls, "PvrPlayMsgReachRec", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"_PvrPlayMsgReachRec Method  is NULL  line=%d.",__LINE__);
	}

	env->CallVoidMethod( obj, Method, NULL );
	Method = NULL;
	DEBUG_MSG1(-1,"_PvrPlayMsgReachRec end %d.",__LINE__);
}

void _PvrRecMsgDiskFull (JNIEnv *env, jobject obj)
{
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_PVRCls, "PvrRecMsgDiskFull", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"_PvrRecMsgDiskFull Method  is NULL  line=%d.",__LINE__);
	}

	env->CallVoidMethod( obj, Method, NULL );
	Method = NULL;
	DEBUG_MSG1(-1,"_PvrRecMsgDiskFull end %d.",__LINE__);
}

void _PvrRecMsgError (JNIEnv *env, jobject obj)
{
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_PVRCls, "PvrRecMsgError", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"_PvrRecMsgError Method  is NULL  line=%d.",__LINE__);
	}

	env->CallVoidMethod( obj, Method, NULL );
	Method = NULL;
	DEBUG_MSG1(-1,"_PvrRecMsgError end %d.",__LINE__);
}

void _PvrRecMsgOverFix (JNIEnv *env, jobject obj)
{
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_PVRCls, "PvrRecMsgOverFix", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"_PvrRecMsgOverFix Method  is NULL  line=%d.",__LINE__);
	}

	env->CallVoidMethod( obj, Method, NULL );
	Method = NULL;
	DEBUG_MSG1(-1,"_PvrRecMsgOverFix end %d.",__LINE__);
}

void _PvrRecMsgReachPlay (JNIEnv *env, jobject obj)
{
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_PVRCls, "PvrRecMsgReachPlay", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"_PvrRecMsgReachPlay Method  is NULL  line=%d.",__LINE__);
	}

	env->CallVoidMethod( obj, Method, NULL );
	Method = NULL;
	DEBUG_MSG1(-1,"_PvrRecMsgReachPlay end %d.",__LINE__);
}
void _PvrRecMsgDiskSlow (JNIEnv *env, jobject obj)
{
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_PVRCls, "PvrRecMsgDiskSlow", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"_PvrRecMsgDiskSlow Method  is NULL  line=%d.",__LINE__);
	}

	env->CallVoidMethod( obj, Method, NULL );
	Method = NULL;
	DEBUG_MSG1(-1,"_PvrRecMsgDiskSlow end %d.",__LINE__);
}

