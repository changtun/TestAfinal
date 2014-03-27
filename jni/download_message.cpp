#include <stdio.h>
#include <limits.h>
#include <jni.h>
#include <android_runtime/AndroidRuntime.h>

#include <logit.h>


#include <dvbplayer.h>

#include "jnistaple.h"
#include "native_download.h"
#include "dvb_download.h"
#include "download_message.h"

typedef struct 
{
	//JNIEnv 	  *env;
	jclass	  Clazz;
	//jmethodID Constructor;
	//jmethodID DL_Completed;
	//jmethodID DL_Interrupt;
   	//jmethodID DL_Failed;
	//jmethodID DL_Progress;
	
}DL_MESSAGE_CALLBACK_t;

DL_MESSAGE_CALLBACK_t g_DownloadMsgCallback;
static JavaVM *g_DLVM = NULL;
static jobject g_DLObj = NULL;

bool _register_dl_msg_callback_methods(JNIEnv *env)
{
	jclass jClass = NULL;
	jmethodID jMethodID = NULL;

	DEBUG_MSG(-1,T("_register_dl_msg_callback_methods"));
	
	jClass = env->FindClass("com/pbi/dvb/dvbinterface/MessageDownload");//env->GetObjectClass(MsgObj);
	if( NULL == jClass )
	{
		DEBUG_MSG(-1, "_register_dl_msg_callback_methods error!");
		return false;
	}
	DEBUG_MSG1(-1,"_register_dl_msg_callback_methods %d",__LINE__);
	g_DownloadMsgCallback.Clazz = (jclass)env->NewGlobalRef((jobject)jClass);
	if( NULL == g_DownloadMsgCallback.Clazz )
	{
		DEBUG_MSG(-1,T("_register_dl_msg_callback_methods class is null!-2----------"));
		return FALSE;
	}
	DEBUG_MSG1(-1,"_register_dl_msg_callback_methods %d",__LINE__);

	DeleteLocalRef( env, jClass );

	DEBUG_MSG(-1,T("_register_dl_msg_callback_methods ok____ok"));

	return TRUE;
}

bool _logout_dl_msg_callback_methods(JNIEnv *env)
{
	DeleteGlobalRef( env, g_DownloadMsgCallback.Clazz );
	g_DownloadMsgCallback.Clazz = NULL;

	return true;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeDownload_downloadMessageInit
(JNIEnv *env, jobject obj, jobject InMsgObj)
{
	DeleteGlobalRef( env, g_DLObj);

	env->GetJavaVM(&g_DLVM);

	if( TRUE != _register_dl_msg_callback_methods( env ) )
	{
		return 1;
	}
	DL_Info_RegMessage_Notify_CallBack(Download_Call_Back);
	g_DLObj = env->NewGlobalRef(InMsgObj);

	return 0;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeDownload_downloadMessageUnInit
(JNIEnv *env, jobject obj)
{
	DEBUG_MSG(-1,"------------downloadMessageUnInit----------------\n");
	DL_Info_UnRegMessage_Notify_CallBack();
	_logout_dl_msg_callback_methods(env);
	DeleteGlobalRef( env, g_DLObj );
	
	g_DLObj = NULL;
	if( NULL != g_DLVM )
	{
		g_DLVM = NULL;
	}	
	return 0;
}

void JNI_Download_Logout(JNIEnv *env)
{
	DL_Info_UnRegMessage_Notify_CallBack();
	_logout_dl_msg_callback_methods(env);
	DeleteGlobalRef( env, g_DLObj );
	g_DLObj = NULL;
	if( NULL != g_DLVM )
	{
		g_DLVM = NULL;
	}
}

void Download_Call_Back( int eNotifyType, int pData )
{
	JNIEnv *env = NULL;

	if( NULL == g_DLVM || NULL == g_DLObj )
	{
		DEBUG_MSG(-1,T("Download_Call_Back-- ---------- mjvm or obj is null--------------------"));
		return;
	}
	if(g_DLVM->AttachCurrentThread(&env, NULL) != JNI_OK)
	{
		DEBUG_MSG1(-1,T("%s: AttachCurrentThread() failed"), __FUNCTION__);
		return;
	}
	if( NULL == env )
	{
		DEBUG_MSG1(-1,T("%s: new is null "), __FUNCTION__);
		return;
	}


	switch( eNotifyType )
	{
		case eDOWNLOAD_COMPLETED:     /* 下载完成*/
		{
			jclass jClass = NULL; 
			jmethodID   Method = NULL;
			DEBUG_MSG(-1,T("Download_Call_Back-----------eDOWNLOAD_COMPLETED--------------------"));
			//_Install_Completed( env, g_DLObj );
		
			Dvb_DownLoad_SetStatus(0);
			Method = env->GetMethodID( g_DownloadMsgCallback.Clazz, "downloadCompleted", "(I)V");
			if( NULL == Method )
			{
				DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
			}
			
			env->CallVoidMethod( g_DLObj, Method, pData );
		}
		break;
	
		case eDOWNLOAD_FAILED:            /*下载失败 */
		{
			jclass jClass = NULL; 
			jmethodID   Method = NULL;
			DEBUG_MSG(-1,T("Download_Call_Back-----------eDOWNLOAD_FAILED--------------------"));
			//_Install_Failed( env, g_DLObj );
			Dvb_DownLoad_SetStatus(0);
			Method = env->GetMethodID( g_DownloadMsgCallback.Clazz, "downloadFailed", "(I)V");
			if( NULL == Method )
			{
				DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
			}
			
			env->CallVoidMethod( g_DLObj, Method, pData );
		}
		break;
		case eDOWNLOAD_PROGRESS:          /* 显示下载进度*/
		{
			jclass jClass = NULL; 
			jmethodID   Method = NULL;
			DEBUG_MSG(-1,T("Download_Call_Back-----------eDOWNLOAD_PROGRESS--------------------"));
			//_Install_Progress( env, g_DLObj, pData );
			Method = env->GetMethodID( g_DownloadMsgCallback.Clazz, "downloadProgress", "(I)V");
			if( NULL == Method )
			{
				DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
			}
			
			env->CallVoidMethod( g_DLObj, Method, pData );
		}
		break;

		default:
		{
			DEBUG_MSG(-1,T("Download_Call_Back-----------default--------------------"));
		}
	}

	if(g_DLVM->DetachCurrentThread() != JNI_OK)
	{
		DEBUG_MSG1(-1,T("%s: DetachCurrentThread() failed"), __FUNCTION__);
	}

}
 
