#include <stdio.h>
#include <jni.h>
#include <installation.h>

#include "installation_manage.h"
#include "logit.h"
#include "jnistaple.h"
#include "pbi_coding.h"

typedef struct 
{
	//JNIEnv 	  *env;
	jclass	  Clazz;
	jmethodID Constructor;
	jmethodID Install_Completed;
	jmethodID Install_Interrupt;
   	jmethodID Install_Failed;
	jmethodID Install_Progress;
	jmethodID Install_TP_Completed;
	jmethodID Install_TP_Failed;
	
}INSTALLATION_MESSAGE_CALLBACK_t;

INSTALLATION_MESSAGE_CALLBACK_t g_InstallMsgCallback;


bool _register_message_callback_methods(JNIEnv *env)
{
	jclass jClass = NULL;
	jmethodID jMethodID = NULL;

	DEBUG_MSG(-1,T("register_message_callback_methods"));
	
	jClass = env->FindClass("com/pbi/dvb/dvbinterface/MessageInstallation");//env->GetObjectClass(MsgObj);
	if( NULL == jClass )
	{
		DEBUG_MSG(-1, "_register_player_msg_callback_methods error!");
		return false;
	}
	DEBUG_MSG1(-1,"register_message_callback_methods %d",__LINE__);
	g_InstallMsgCallback.Clazz = (jclass)env->NewGlobalRef((jobject)jClass);
	if( NULL == g_InstallMsgCallback.Clazz )
	{
		DEBUG_MSG(-1,T("register_message_callback_methods class is null!-2----------"));
		return FALSE;
	}
	DEBUG_MSG1(-1,"register_message_callback_methods %d",__LINE__);

	DeleteLocalRef( env, jClass );

#if 0
	g_InstallMsgCallback.Clazz 					= env->FindClass("com/pbi/dvb/dvbinterface/MessageInstallation");
	if( NULL == g_InstallMsgCallback.Clazz )
	{
		DEBUG_MSG(-1,T("register_message_callback_methods class is null!-2----------"));
	}
	
	g_InstallMsgCallback.Constructor 			= env->GetMethodID(g_InstallMsgCallback.Clazz, "<init>", "()V");
	g_InstallMsgCallback.Install_Completed		= env->GetMethodID(g_InstallMsgCallback.Clazz, "installCompleted", "()V");
	g_InstallMsgCallback.Install_Failed			= env->GetMethodID(g_InstallMsgCallback.Clazz, "installFailed", "()V");
	g_InstallMsgCallback.Install_Interrupt		= env->GetMethodID(g_InstallMsgCallback.Clazz, "installInterrupt", "()V");
	g_InstallMsgCallback.Install_Progress		= env->GetMethodID(g_InstallMsgCallback.Clazz, "installProgress", "(II)V");
	g_InstallMsgCallback.Install_TP_Completed	= env->GetMethodID(g_InstallMsgCallback.Clazz, "installTpCompleted", "(ILjava/lang/String;C)V");
	g_InstallMsgCallback.Install_TP_Failed		= env->GetMethodID(g_InstallMsgCallback.Clazz, "installTpFailed", "()V");
#endif
	DEBUG_MSG(-1,T("register_message_callback_methods ok____ok"));

	return TRUE;
}

bool _logout_message_callback_methods(JNIEnv *env)
{
	DeleteGlobalRef( env, g_InstallMsgCallback.Clazz);
	g_InstallMsgCallback.Clazz = NULL;
	DEBUG_MSG(-1,T("Logout message callback methods"));
	
	return TRUE;
}


void _Install_Completed(JNIEnv *env, jobject obj )
{
	DEBUG_MSG(-1,T("_Install_Completed *******************************************"));
//	env->CallVoidMethod( obj, g_InstallMsgCallback.Install_Completed, NULL );
	jclass jClass = NULL; 
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_InstallMsgCallback.Clazz, "installCompleted", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
	}
	
	env->CallVoidMethod( obj, Method, NULL );
}

void _Install_Interrupt(JNIEnv *env, jobject obj )
{
	DEBUG_MSG(-1,T("_Install_Interrupt *******************************************"));	
///	env->CallVoidMethod( obj, g_InstallMsgCallback.Install_Interrupt, NULL );
	jclass jClass = NULL; 
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_InstallMsgCallback.Clazz, "installInterrupt", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
	}
	
	env->CallVoidMethod( obj, Method, NULL );
}

void _Install_Failed(JNIEnv *env, jobject obj )
{

	DEBUG_MSG(-1,T("_Install_Failed *******************************************"));		
//	env->CallVoidMethod( obj, g_InstallMsgCallback.Install_Failed, NULL );
	jclass jClass = NULL; 
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_InstallMsgCallback.Clazz, "installFailed", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
	}
	
	env->CallVoidMethod( obj, Method, NULL );
}

void _Install_Progress( JNIEnv *env, jobject obj, Install_Progress_t *progress )
{
	jint jiProgress;
	jint jiFreq;
	
	jiProgress = progress->u32Progress;
	jiFreq = progress->puCurrTuner->tCable.u32Freq;
	
	DEBUG_MSG2(-1,T("_Install_Progress ***************progress   %d*********freq %d*******************"), \
		jiProgress, jiFreq );		
//	env->CallVoidMethod( obj, g_InstallMsgCallback.Install_Progress, jiProgress, jiFreq );
	jclass jClass = NULL; 
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_InstallMsgCallback.Clazz, "installProgress", "(II)V" );
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
	}
	
	env->CallVoidMethod( obj, Method, jiProgress, jiFreq );
}

void _Install_TP_Completed( JNIEnv *env, jobject obj, Install_Search_Result_t *ptIsr )
{
	unsigned int ii = 0;
	jclass jClass = NULL; 
	jmethodID   Method = NULL;
	jint jiFreq = 0;
	jint jiMod = 0;
	jint jiSymbRate = 0;
	jchar jcServType = 0;
	jstring jsName;
	int len = 0;
	int cmp = 0;

	char name[20];
//	int iRet = 0;
//	int iNameLen = 0;
//	char *pOutName = NULL;
	
	if( NULL != ptIsr->ptTpList )
	{
		if( eDVBCORE_SIGNAL_SOURCE_DVBC == ptIsr->ptTpList[0].uTunerDesc.tCable.eSignalSource )
		{
			jiFreq = ptIsr->ptTpList[0].uTunerDesc.tCable.u32Freq;
			jiSymbRate = ptIsr->ptTpList[0].uTunerDesc.tCable.u32SymbRate;
			jiMod = ptIsr->ptTpList[0].uTunerDesc.tCable.eMod;
		}
	}
	for( ii = 0; ii < ptIsr->u32ServNum; ii++ )
	{
		if( NULL == ptIsr->ptServList[ii].sServName)
		{
			cmp = 1;
		}
		else
		{
			cmp = strncmp( ptIsr->ptServList[ii].sServName, DEFAULT_SERVICE_NAME, ptIsr->ptServList[ii].u32ServNameLen );
		}
		
		if( 0 != cmp )
		{
			jsName = CodingToJstring( env, ptIsr->ptServList[ii].uServNameType, ptIsr->ptServList[ii].sServName, ptIsr->ptServList[ii].u32ServNameLen-1 );
		}
		else
		{
			memset( name, 0, 20 );
			sprintf( name, "SID:%d", ptIsr->ptServList[ii].u16ServId );
			ptIsr->ptServList[ii].uServNameType = CHARACTER_TYPE_UTF8;
			jsName = CodingToJstring( env, ptIsr->ptServList[ii].uServNameType, name, strlen(name) );
			
		}
		jcServType = ptIsr->ptServList[ii].u8ServType;
		DEBUG_MSG2(-1,T("_Install_TP_Completed *******************Freq : %d*********** Name: %s*************"), \
			jiFreq, ptIsr->ptServList[ii].sServName);

		if( NULL == g_InstallMsgCallback.Install_TP_Completed )
		{
			DEBUG_MSG(-1,T("g_InstallMsgCallback.Install_TP_Completed is NULL*************************************"));
		}
		
//		env->CallVoidMethod( obj, g_InstallMsgCallback.Install_TP_Completed, jiFreq, jsName, jcServType);
	
		Method = env->GetMethodID( g_InstallMsgCallback.Clazz, "installTpCompleted", "(IIILjava/lang/String;C)V");
		if( NULL == Method )
		{
			DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
		}

		env->CallVoidMethod( obj, Method, jiFreq, jiSymbRate, jiMod, jsName, jcServType );

	}
}

void _Install_TP_Failed( JNIEnv *env, jobject obj )
{	
	DEBUG_MSG(-1,T("_Install_TP_Failed *******************************************"));		
//	env->CallVoidMethod( obj, g_InstallMsgCallback.Install_TP_Failed, NULL );
	jclass jClass = NULL; 
	jmethodID   Method = NULL;

	Method = env->GetMethodID( g_InstallMsgCallback.Clazz, "installTpFailed", "()V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
	}
	
	env->CallVoidMethod( obj, Method, NULL );
}

