#include <stdio.h>
#include <jni.h>
#include <android_runtime/AndroidRuntime.h>
#include "logit.h"
#include "jnistaple.h"

#include <native_epg.h>
#include <epg.h>
#include <epg_common.h>
#include <dvbplayer.h>

static JavaVM*	   g_epgVM = NULL;
static jobject 	   g_epgMsgObj = NULL;


typedef struct
{
	jclass 	MsgClass;
	jmethodID Constructor;	
	jmethodID epgMessage;
	
}EPG_MESSAGE_CALLBACK_t;

EPG_MESSAGE_CALLBACK_t g_Epg_Msg_Callback;


bool _register_epg_msg_callback_methods(JNIEnv *env)
{
	jclass jClass = NULL;

	jClass = env->FindClass("com/pbi/dvb/dvbinterface/NativeEpgMsg");
	if( NULL == jClass )
	{
		DEBUG_MSG1( -1, "%s class is NULL!", __FUNCTION__ );
		return false;
	}
	
	g_Epg_Msg_Callback.MsgClass = (jclass)env->NewGlobalRef( (jobject)jClass );
	if( NULL == g_Epg_Msg_Callback.MsgClass )
	{
		DEBUG_MSG(-1, "_register_epg_msg_callback_methods error!");
		return false;
	}

	//g_Epg_Msg_Callback.Constructor = env->GetMethodID(g_Epg_Msg_Callback.MsgClass, "<init>", "()V");
	//g_Epg_Msg_Callback.epgMessage = env->GetMethodID(g_Epg_Msg_Callback.MsgClass, "epgmsgNotify", "(SSS)V");

	return true;
}

bool _logout_epg_msg_callback_methods(JNIEnv *env)
{
	DeleteGlobalRef( env, g_Epg_Msg_Callback.MsgClass );
	g_Epg_Msg_Callback.MsgClass = NULL;

	return true;
}




void _epgMessage(JNIEnv *env, jobject obj, short serviceid, short tsid, short onid)
{
	DEBUG_MSG3( -1, "epg Message   servid %d tsid %d onid %d ", \
		serviceid, tsid, onid);

	jmethodID epgMessage = NULL;
	
	epgMessage = env->GetMethodID(g_Epg_Msg_Callback.MsgClass, "epgmsgNotify", "(SSS)V");
	if( NULL != epgMessage )
	{
		env->CallVoidMethod( obj, epgMessage, serviceid, tsid, onid);
	}
}




void EPG_Message_CallBack(unsigned short serviceid,unsigned short tsid,unsigned short onid)
{
	JNIEnv *env = NULL;

	if(CheckEPG(serviceid,tsid,onid)!=0)
	{
		return;
	}
	
	if( NULL == g_epgVM || NULL == g_epgMsgObj )
	{
		DEBUG_MSG(-1,T("epg message-- ---------- mjvm or obj is null--------------------"));
		return;
	}
	if (g_epgVM->AttachCurrentThread(&env, NULL) != JNI_OK)
	{
		 DEBUG_MSG1(-1,T("%s: AttachCurrentThread() failed"), __FUNCTION__);
		 return;
	}
	if( NULL == env )
	{
		DEBUG_MSG1(-1,T("%s: new is null "), __FUNCTION__);
		return;
	}

	_epgMessage(env,g_epgMsgObj,serviceid,tsid,onid);
	if(g_epgVM->DetachCurrentThread() != JNI_OK) 
	{
		 DEBUG_MSG1(-1,T("%s: DetachCurrentThread() failed"), __FUNCTION__); 
	}


}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeEpg_EpgMessageInit
(JNIEnv *env, jobject obj, jobject MsgObj)
{
	DeleteGlobalRef( env, g_epgMsgObj );
	g_epgMsgObj = NULL;
	env->GetJavaVM(&g_epgVM);

	if( TRUE != _register_epg_msg_callback_methods(env) )
	{
		DEBUG_MSG(-1,"EPG MessageInit error register failed");
		return 1;
	}
	
	App_Register_cb(EPG_Message_CallBack);
	
	g_epgMsgObj = env->NewGlobalRef( MsgObj );

	DEBUG_MSG(-1,"EPG MessageInit Ok!");
	return 0;
}



