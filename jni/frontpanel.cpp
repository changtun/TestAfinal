#include <stdio.h>
#include <jni.h>

#include "logit.h"
#include "jnistaple.h"
#include "frontpanel.h"


static jclass g_FpClass;

bool  _register_front_panel_Class(JNIEnv *env )
{

	jclass jClass = NULL;
	
	jClass = env->FindClass("com/pbi/dvb/dvbinterface/NativeDrive");//env->GetObjectClass(PlayerMsgObj);
	if( NULL == jClass )
	{
		DEBUG_MSG1(-1, "%s error!",__FUNCTION__);
		return false;
	}
	g_FpClass = (jclass)env->NewGlobalRef((jobject)jClass);
	if( NULL == g_FpClass )
	{
		DEBUG_MSG1(-1, "%s Fp error! ", __FUNCTION__);
		return false;
	}
	
	return true;
}

int Front_Panel_Dispaly(JNIEnv *env, int LogNum )
{
	jobject Object = NULL;
	jmethodID   Method = NULL;
	jmethodID   Constructor = NULL;
	jstring String = NULL;
	char str[6];
	int temp = 0;

	Object = NewClsToObject( env, g_FpClass );
	if( NULL == Object )
	{
		return -1;
	}
	
	Method = env->GetMethodID( g_FpClass, "DisPlay", "(Ljava/lang/String;)V");
	if( NULL == Method )
	{
		DEBUG_MSG1(-1,"Player Message Method  is NULL  line=%d.",__LINE__);
	}

	memset( str, 0, 6 );
	
	if( -1 == LogNum )
	{
		sprintf(str,"--X--");
	}
	else if( -2 == LogNum )
	{
		sprintf(str,"BOXOT"); /* 显示boot */
	}
	else if( -3 == LogNum )
	{
		sprintf(str,"XXXXX"); /* 清除前面板 */
	}
	else
	{
		temp= LogNum / 100;
		temp %= 10;
		
		sprintf(str,"C%dX%02d", temp, LogNum % 100 );
	}

	DEBUG_MSG1(-1,"Front_Panel_Dispaly  str %s-----",str);
	
	String = stoJstring( env, str );
	if( NULL != String )
	{
		env->CallVoidMethod( Object, Method, String );
	}
	

	DeleteLocalRef(  env, Object );

	return 0;
	
}

