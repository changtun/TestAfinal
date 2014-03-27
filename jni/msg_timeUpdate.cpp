#include <stdio.h>
#include <limits.h>
#include <jni.h>
#include <android_runtime/AndroidRuntime.h>
#include "logit.h"
#include "jnistaple.h"
#include "systime.h"
#include "msgtimeupdate.h"

static JavaVM*	   g_timeVM = NULL;
static jobject g_timeMsgObj = NULL;


typedef struct
{
	jclass MsgClass;
	//jmethodID nitMessage;

}Time_MESSAGE_CALLBACK_t;

Time_MESSAGE_CALLBACK_t g_time_Msg_Callback;

bool _register_time_msg_callback_methods(JNIEnv *env)
{
	jclass ClassTemp = NULL;
	
	ClassTemp = env->FindClass("com/pbi/dvb/dvbinterface/MsgTimeUpdate");
	if( NULL == ClassTemp )
	{
		DEBUG_MSG(-1, "_register_time_msg_callback_methods error1!");
		return false;
	}
	g_time_Msg_Callback.MsgClass = (jclass)env->NewGlobalRef((jobject)ClassTemp);
	
	if( NULL == g_time_Msg_Callback.MsgClass )
	{
		DEBUG_MSG(-1, "_register_time_msg_callback_methods error2!");
		return false;
	}
	//g_tNit_Msg_Callback.nitMessage = env->GetMethodID(g_tNit_Msg_Callback.MsgClass, "NitUpdate", "(I)V");

	return true;
}

bool _logout_time_msg_callback_methods(JNIEnv *env)
{
	DeleteGlobalRef( env, g_time_Msg_Callback.MsgClass );
	g_time_Msg_Callback.MsgClass = NULL;

	return true;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Nativetime_TimeUpdateInit
(JNIEnv *env, jobject obj, jobject NitMsgObj)
{
		
	DeleteGlobalRef( env, g_timeMsgObj );
	g_timeMsgObj = NULL;
	env->GetJavaVM(&g_timeVM);
	
	if( true != _register_time_msg_callback_methods(env) )
	{
		DEBUG_MSG(-1,"Java_com_pbi_dvb_dvbinterface_Nativetime_TimeUpdateInit error register failed");
		return 1;
	}
	
	Time_RegMessage_Notify_CallBack(Time_Message_CallBack);
	
	g_timeMsgObj =  env->NewGlobalRef(NitMsgObj);
	
	DEBUG_MSG(-1,"Java_com_pbi_dvb_dvbinterface_Nativetime_TimeUpdateInit!!!!!");
	return 0;

}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Nativetime_TimeMessageUnint
(JNIEnv *env, jobject obj)
{
	DEBUG_MSG(-1,"------------TimeMessageUnint----------------\n");
	Time_UnRegMessage_Notify_CallBack();
	_logout_time_msg_callback_methods(env);
	DeleteGlobalRef( env, g_timeMsgObj );
	
	g_timeMsgObj = NULL;
	if( NULL != g_timeVM )
	{
		g_timeVM = NULL;
	}	
	return 0;
}


void JNI_TimeMsg_Logout(JNIEnv *env)
{
	DEBUG_MSG(-1,"------------JNI_NITMsg_Logout----------------\n");
	Time_UnRegMessage_Notify_CallBack();
	_logout_time_msg_callback_methods(env);
	DeleteGlobalRef( env, g_timeMsgObj );
	
	g_timeMsgObj = NULL;
	if( NULL != g_timeVM )
	{
		g_timeVM = NULL;
	}
}
void Time_Message_CallBack( )
{
	JNIEnv *env = NULL;
	jmethodID timeMessage;

	
	DEBUG_MSG2(-1,T("%s: line %d"), __FUNCTION__,__LINE__); 
	
	if( NULL == g_timeVM || NULL == g_timeMsgObj )
	{
		DEBUG_MSG(-1,T("Time_Message_CallBack-- ---------- mjvm or obj is null--------------------"));
		return;
	}
	

	if (g_timeVM->AttachCurrentThread(&env, NULL) != JNI_OK)
	{
		 DEBUG_MSG1(-1,T("%s: AttachCurrentThread() failed"), __FUNCTION__);
		 return;
	}
	
	if( NULL == env )
	{
		DEBUG_MSG1(-1,T("%s: new is null "), __FUNCTION__);
		return;
	}
	timeMessage = env->GetMethodID(g_time_Msg_Callback.MsgClass, "TimeUpdate", "()V");
	if(timeMessage ==NULL)
	{
		DEBUG_MSG1(-1,T("%s: new is null "), __FUNCTION__);
		return;
	}

	env->CallVoidMethod( g_timeMsgObj, timeMessage,0);

	DEBUG_MSG2(-1,T("%s: line%d"), __FUNCTION__,__LINE__); 

	if(g_timeVM->DetachCurrentThread() != JNI_OK) 
	{
		 DEBUG_MSG1(-1,T("%s: DetachCurrentThread() failed"), __FUNCTION__); 
	}
	
	DEBUG_MSG2(-1,T("%s: line %d"), __FUNCTION__,__LINE__); 

}



