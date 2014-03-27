#include <stdio.h>
#include <limits.h>
#include <jni.h>
#include <android_runtime/AndroidRuntime.h>

#include <logit.h>
#include <installation.h>

#include <dvbplayer.h>

#include "jnistaple.h"
#include "native_installation.h"
#include "installation_message.h"
#include "installation_manage.h"

static JavaVM *g_InstallVM = NULL;
static jobject g_InstallObj = NULL;

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeInstallation_installMessageInit
(JNIEnv *env, jobject obj, jobject InMsgObj)
{
    DeleteGlobalRef( env, g_InstallObj);

	env->GetJavaVM(&g_InstallVM);
	
	if( TRUE != _register_message_callback_methods( env ) )
	{
		return 1;
	}

	g_InstallObj = env->NewGlobalRef(InMsgObj);

	return 0;
}

void JNI_Installation_Logout(JNIEnv *env)
{
	_logout_message_callback_methods(env);
    DeleteGlobalRef( env, g_InstallObj );
	g_InstallObj = NULL;
	if( NULL != g_InstallVM )
	{
		g_InstallVM = NULL;
	}
}

void Installation_Call_Back( Install_Notify_Type_e eNotifyType, void * pData)
{
	JNIEnv *env = NULL;
	
	if( NULL == g_InstallVM || NULL == g_InstallObj )
	{
		DEBUG_MSG(-1,T("Installation_Call_Back-- ---------- mjvm or obj is null--------------------"));
		return;
	}
	if (g_InstallVM->AttachCurrentThread(&env, NULL) != JNI_OK)
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
		case eINSTALL_COMPLETED:     /* 搜索完成，返回搜索结*/
		{
			DEBUG_MSG(-1,T("Installation_Call_Back-----------eINSTALL_COMPLETED--------------------"));	
			_Install_Completed( env, g_InstallObj );
        }
        break;
    	case eINSTALL_INTERRUPT:         /* 搜索中断，返回搜索结果 */
		{
			DEBUG_MSG(-1,T("Installation_Call_Back-----------eINSTALL_INTERRUPT--------------------"));			
			_Install_Interrupt( env, g_InstallObj );
        }
        break;
    	case eINSTALL_FAILED:            /* 搜索失败 */
		{
			DEBUG_MSG(-1,T("Installation_Call_Back-----------eINSTALL_FAILED--------------------"));
			_Install_Failed( env, g_InstallObj );
        }
        break;
    	case eINSTALL_PROGRESS:          /* 显示搜索进度，及当前搜索频点 */
		{
			DEBUG_MSG(-1,T("Installation_Call_Back-----------eINSTALL_PROGRESS--------------------"));
			_Install_Progress( env, g_InstallObj, (Install_Progress_t *)pData );
        }
        break;
    	case eINSTALL_TP_COMPLETED:      /* 完成一个频点，返回完成频点的搜索数据 */
		{
			DEBUG_MSG(-1,T("Installation_Call_Back-----------eINSTALL_TP_COMPLETED--------------------"));
			_Install_TP_Completed( env, g_InstallObj, (Install_Search_Result_t *)pData );
        }
        break;
		case eINSTALL_TP_FAILED:         /* 搜索一个频点失败 */
		{
			DEBUG_MSG(-1,T("Installation_Call_Back-----------eINSTALL_TP_FAILED--------------------"));
			_Install_TP_Failed( env, g_InstallObj );
        }
        break;
		default:
    	{
			DEBUG_MSG(-1,T("Installation_Call_Back-----------default--------------------"));
		}
	}
	
	if(g_InstallVM->DetachCurrentThread() != JNI_OK) 
	{
	 DEBUG_MSG1(-1,T("%s: DetachCurrentThread() failed"), __FUNCTION__); 
	}
	
}


