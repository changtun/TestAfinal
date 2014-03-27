#include <stdio.h>
#include <limits.h>
#include <jni.h>
#include <android_runtime/AndroidRuntime.h>
#include "logit.h"
#include "jnistaple.h"
#include "message_nitupdate.h"

#include "background.h"
#include <installation.h>
#include "message_nitupdate.h"

static JavaVM*	   g_nitVM = NULL;
static jobject g_nitMsgObj = NULL;


typedef struct
{
	jclass MsgClass;
	//jmethodID nitMessage;

}Nit_MESSAGE_CALLBACK_t;

Nit_MESSAGE_CALLBACK_t g_tNit_Msg_Callback;

bool _register_nit_msg_callback_methods(JNIEnv *env)
{
	jclass ClassTemp = NULL;
	
	ClassTemp = env->FindClass("com/pbi/dvb/dvbinterface/MessageNit");
	if( NULL == ClassTemp )
	{
		DEBUG_MSG(-1, "_register_nit_msg_callback_methods error1!");
		return false;
	}
	g_tNit_Msg_Callback.MsgClass = (jclass)env->NewGlobalRef((jobject)ClassTemp);
	
	if( NULL == g_tNit_Msg_Callback.MsgClass )
	{
		DEBUG_MSG(-1, "_register_nit_msg_callback_methods error2!");
		return false;
	}




	//g_tNit_Msg_Callback.nitMessage = env->GetMethodID(g_tNit_Msg_Callback.MsgClass, "NitUpdate", "(I)V");

	return true;
}

bool _logout_NIT_msg_callback_methods(JNIEnv *env)
{
	DeleteGlobalRef( env, g_tNit_Msg_Callback.MsgClass );
	g_tNit_Msg_Callback.MsgClass = NULL;

	return true;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Nativenit_NitMessageInit
(JNIEnv *env, jobject obj, jobject NitMsgObj)
{
		
	DeleteGlobalRef( env, g_nitMsgObj );
	g_nitMsgObj = NULL;
	env->GetJavaVM(&g_nitVM);
	
	if( true != _register_nit_msg_callback_methods(env) )
	{
		DEBUG_MSG(-1,"_register_nit_msg_callback_methods error register failed");
		return 1;
	}
	
	Nit_RegMessage_Notify_CallBack(Nit_Message_CallBack);
	
	g_nitMsgObj =  env->NewGlobalRef(NitMsgObj);
	
	DEBUG_MSG(-1,"NitMessageInit Ok!!!!!");
	return 0;

}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Nativenit_NitMessageUnint
(JNIEnv *env, jobject obj)
{
	DEBUG_MSG(-1,"------------NitMessageUnint----------------\n");
	Nit_UnRegMessage_Notify_CallBack();
	_logout_NIT_msg_callback_methods(env);
	DeleteGlobalRef( env, g_nitMsgObj );
	
	g_nitMsgObj = NULL;
	if( NULL != g_nitVM )
	{
		g_nitVM = NULL;
	}	
	return 0;
}


void JNI_NITMsg_Logout(JNIEnv *env)
{
	DEBUG_MSG(-1,"------------JNI_NITMsg_Logout----------------\n");
	Nit_UnRegMessage_Notify_CallBack();
	_logout_NIT_msg_callback_methods(env);
	DeleteGlobalRef( env, g_nitMsgObj );
	
	g_nitMsgObj = NULL;
	if( NULL != g_nitVM )
	{
		g_nitVM = NULL;
	}
}
void Nit_Message_CallBack(int param1 )
{
	JNIEnv *env = NULL;
	jmethodID nitMessage;

	
	DEBUG_MSG2(-1,T("%s: line %d"), __FUNCTION__,__LINE__); 
	
	if( NULL == g_nitVM || NULL == g_nitMsgObj )
	{
		DEBUG_MSG(-1,T("Nit_Message_CallBack-- ---------- mjvm or obj is null--------------------"));
		return;
	}
	

	if (g_nitVM->AttachCurrentThread(&env, NULL) != JNI_OK)
	{
		 DEBUG_MSG1(-1,T("%s: AttachCurrentThread() failed"), __FUNCTION__);
		 return;
	}
	
	if( NULL == env )
	{
		DEBUG_MSG1(-1,T("%s: new is null "), __FUNCTION__);
		return;
	}
	nitMessage = env->GetMethodID(g_tNit_Msg_Callback.MsgClass, "NitUpdate", "(I)V");
	if(nitMessage ==NULL)
	{
		DEBUG_MSG1(-1,T("%s: new is null "), __FUNCTION__);
		return;
	}

	env->CallVoidMethod( g_nitMsgObj, nitMessage,param1);

	DEBUG_MSG2(-1,T("%s: ver %d"), __FUNCTION__,param1); 

	if(g_nitVM->DetachCurrentThread() != JNI_OK) 
	{
		 DEBUG_MSG1(-1,T("%s: DetachCurrentThread() failed"), __FUNCTION__); 
	}
	
	DEBUG_MSG2(-1,T("%s: line %d"), __FUNCTION__,__LINE__); 

}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_MessageNit_nitSearchVer
(JNIEnv *env, jobject obj)
{
	short int ver =0;
	
	DEBUG_MSG2(-1,"%s +++++++++++++++++++++=%d==enter",__FUNCTION__,__LINE__);	
	
	ver =_Get_install_Nit_Version();

	
	DEBUG_MSG3(-1,"%s +++++++++++++++++++++=%d==ver%d",__FUNCTION__,__LINE__,ver);


	return ver;

}

