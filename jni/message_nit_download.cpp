#include <stdio.h>
#include <limits.h>
#include <jni.h>
#include <android_runtime/AndroidRuntime.h>
#include "logit.h"
#include "jnistaple.h"

#include "background.h"
#include "message_nit_download.h"


typedef struct
{
	jclass MsgClass;
	//jmethodID nitMessage;

}DL_Nit_MESSAGE_CALLBACK_t;

DL_Nit_MESSAGE_CALLBACK_t	g_tDL_Msg_Callback;

static JavaVM*	   g_dl_VM = NULL;
static jobject	g_dl_MsgObj = NULL;

bool _register_dl_nit_msg_callback_methods(JNIEnv *env)
{
	jclass ClassTemp = NULL;
	
	ClassTemp = env->FindClass("com/pbi/dvb/dvbinterface/MessageNitDownload");
	if( NULL == ClassTemp )
	{
		DEBUG_MSG(-1, "_register_dl_nit_msg_callback_methods error1!");
		return false;
	}
	g_tDL_Msg_Callback.MsgClass = (jclass)env->NewGlobalRef((jobject)ClassTemp);
	
	if( NULL == g_tDL_Msg_Callback.MsgClass )
	{
		DEBUG_MSG(-1, "_register_dl_nit_msg_callback_methods error2!");
		return false;
	}

	//g_tNit_Msg_Callback.nitMessage = env->GetMethodID(g_tNit_Msg_Callback.MsgClass, "NitUpdate", "(I)V");

	return true;
}

bool _logout_DL_NIT_msg_callback_methods(JNIEnv *env)
{
	DeleteGlobalRef( env, g_tDL_Msg_Callback.MsgClass );
	g_tDL_Msg_Callback.MsgClass = NULL;

	return true;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Nativenit_DLMessageInit
(JNIEnv *env, jobject obj, jobject NitMsgObj)
{
		
	DeleteGlobalRef( env, g_dl_MsgObj );
	g_dl_MsgObj = NULL;
	env->GetJavaVM(&g_dl_VM);
	
	if( true != _register_dl_nit_msg_callback_methods(env) )
	{
		DEBUG_MSG(-1,"_register_dl_nit_msg_callback_methods error register failed");
		return 1;
	}
	
	DL_Nit_RegMessage_Notify_CallBack(DL_Nit_Message_CallBack);
	
	g_dl_MsgObj =  env->NewGlobalRef(NitMsgObj);
	
	DEBUG_MSG(-1,"DLNitMessageInit Ok!!!!!");
	return 0;

}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Nativenit_DLMessageUnint
(JNIEnv *env, jobject obj)
{
	DEBUG_MSG(-1,"------------DLNitMessageUnint----------------\n");
	DL_Nit_UnRegMessage_Notify_CallBack();
	_logout_DL_NIT_msg_callback_methods(env);
	DeleteGlobalRef( env, g_dl_MsgObj );
	
	g_dl_MsgObj = NULL;
	if( NULL != g_dl_VM )
	{
		g_dl_VM = NULL;
	}	
	return 0;
}


void JNI_DLNITMsg_Logout(JNIEnv *env)
{
	DEBUG_MSG(-1,"------------JNI_DLNITMsg_Logout----------------\n");
	DL_Nit_UnRegMessage_Notify_CallBack();
	_logout_DL_NIT_msg_callback_methods(env);
	DeleteGlobalRef( env, g_dl_MsgObj );
	
	g_dl_MsgObj = NULL;
	if( NULL != g_dl_VM )
	{
		g_dl_VM = NULL;
	}
}
void DL_Nit_Message_CallBack(int param1 )
{
	JNIEnv *env = NULL;
	jmethodID nitMessage;

	
	DEBUG_MSG2(-1,T("%s: line %d"), __FUNCTION__,__LINE__); 
	
	if( NULL == g_dl_VM || NULL == g_dl_MsgObj )
	{
		DEBUG_MSG(-1,T("DL_Nit_Message_CallBack-- ---------- mjvm or obj is null--------------------"));
		return;
	}
	

	if (g_dl_VM->AttachCurrentThread(&env, NULL) != JNI_OK)
	{
		 DEBUG_MSG1(-1,T("%s: AttachCurrentThread() failed"), __FUNCTION__);
		 return;
	}
	
	if( NULL == env )
	{
		DEBUG_MSG1(-1,T("%s: new is null "), __FUNCTION__);
		return;
	}
	nitMessage = env->GetMethodID(g_tDL_Msg_Callback.MsgClass, "DLNitUpdate", "(I)V");
	if(nitMessage ==NULL)
	{
		DEBUG_MSG1(-1,T("%s: new is null "), __FUNCTION__);
		return;
	}

	env->CallVoidMethod( g_dl_MsgObj, nitMessage, param1);

	DEBUG_MSG2(-1,T("%s: ver %d"), __FUNCTION__,param1); 

	if(g_dl_VM->DetachCurrentThread() != JNI_OK) 
	{
		 DEBUG_MSG1(-1,T("%s: DetachCurrentThread() failed"), __FUNCTION__); 
	}
	
	DEBUG_MSG2(-1,T("%s: line %d"), __FUNCTION__,__LINE__); 

}

