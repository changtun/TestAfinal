#include <stdio.h>
#include <limits.h>
#include <jni.h>
#include <android_runtime/AndroidRuntime.h>

//#include <uti_udrm_type.h>
#include <ca.h>
//#include <Tr_Cas.h>



#include "logit.h"
#include "jnistaple.h"
#include "ca_message.h"
#include "ca_manage.h"
#include "ca.h"
#include "ctiapi.h"

static JavaVM*	   g_caVM = NULL;
static jobject g_CaMsgObj = NULL;

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeCA_CaMessageInit
(JNIEnv *env, jobject obj, jobject CaMsgObj)
{
    DeleteGlobalRef( env, g_CaMsgObj );
	g_CaMsgObj = NULL;
	env->GetJavaVM(&g_caVM);

	if( TRUE != _register_ca_msg_callback_methods(env) )
	{
		DEBUG_MSG(-1,"CaMessageInit error register failed");
		return 1;
	}
	
	CTICA_RegMessage_Notify_CallBack(CA_Message_CallBack);
	
	g_CaMsgObj = env->NewGlobalRef(CaMsgObj);

	DEBUG_MSG(-1,"CaMessageInit Ok!");
	return 0;
}

void JNI_CaMsg_Logout(JNIEnv *env)
{
//	CA_UnRegMessage_Notify_CallBack();
	_logout_ca_msg_callback_methods(env);
    DeleteGlobalRef( env, g_CaMsgObj );
	g_CaMsgObj = NULL;
	if( NULL != g_caVM )
	{
		g_caVM = NULL;
	}
}
#if 1
void CA_Message_CallBack(U32	uEventType, U32 u32EventLen, unsigned char* pEventData, unsigned char* pPrivateData)
{
	JNIEnv *env = NULL;
	
	if( NULL == g_caVM || NULL == g_CaMsgObj )
	{
		DEBUG_MSG(-1,T("ca message-- ---------- mjvm or obj is null--------------------"));
		return;
	}
	
	if (g_caVM->AttachCurrentThread(&env, NULL) != JNI_OK)
	{
		 DEBUG_MSG1(-1,T("%s: AttachCurrentThread() failed"), __FUNCTION__);
		 return;
	}
	if( NULL == env )
	{
		DEBUG_MSG1(-1,T("%s: new is null "), __FUNCTION__);
		return;
	}
	DEBUG_MSG3(-1,"[zshang][%s|%d]type[%d]\n", __FUNCTION__, __LINE__, uEventType);
	switch(uEventType)
	{
		case SET_PARENTAL_QUERY_REPLY:
		case CHECK_PIN_QUERY_REPLY:
		case CHANGE_PIN_QUERY_REPLY:			
		case SUBTITLE_NOTIFY:
		case SC_INFO_NOTIFY:
		case PPID_NOTIFY:
		case PPID_INFO_NOTIFY:
		case USER_VIEW_INFO_NOTIFY:
		case PROG_SKIP_NOTIFY:
		case PPV_LIST_INFO_NOTIFY:
		case IPPV_LIST_INFO_NOTIFY:
		case ANTI_RECORD_QUERY_REPLY:
		case IPPV_QUERY_REPLY:
		{
			if(pEventData != NULL && u32EventLen >= 1)
			_CaStatusMessage(env, g_CaMsgObj, uEventType, pEventData[0]);
		}
		break;
	}
	switch(uEventType)
	{
		case CAT_QUERY_REPLY:
		case PMT_QUERY_REPLY :
		case STOP_PROG_QUERY_REPLY :
		case CAT_INFO_NOTIFY:
		case PMT_INFO_NOTIFY:
		case EMM_STATUS_NOTIFY:		
		case NIT_QUERY_REPLY:
		case NVRAM_OP_REPLY:
			break;
		case IPPV_NOTIFY:
		{
			_CaIppNotifyMessage(env, g_CaMsgObj, (int)pPrivateData);
		}
		break;
		case BMAIL_NOTIFY:		
		case SUBTITLE_NOTIFY:
		{
			_CaMailMessage(env, g_CaMsgObj,(int)pPrivateData);
		}
		break;
		case OSD_DISPLAY_NOTIFY:
		{
			_CaOsdDisplayMessage(env, g_CaMsgObj,(int)pPrivateData);
		}
		break;
		case SC_INFO_NOTIFY:
		{
			if(pEventData != NULL && u32EventLen >= 2 && pEventData[1] == 1)
			{
				DEBUG_MSG3(-1,"[zshang][%s|%d]status[%d]\n", __FUNCTION__, __LINE__, pEventData[1]);
					_CaSCInfoMessage(env, g_CaMsgObj,(int)pPrivateData);
				}
			}
		break;
		case PPID_NOTIFY:
		{
			if(pEventData != NULL && u32EventLen >= 1 && pEventData[0] == 0)
			_CaPPidListMessage(env, g_CaMsgObj,(int)pPrivateData);
		}
		break;
		case PPID_INFO_NOTIFY:
		{
			if(pEventData != NULL && u32EventLen >= 1 && pEventData[0] == 0)
			_CaPPidInfoMessage(env, g_CaMsgObj,(int)pPrivateData);
		}
		break;
		case CA_MESSAGE_CODE:
		{
			if(pEventData != NULL && u32EventLen >= 4)
				_CaServiceMessage(env, g_CaMsgObj,pEventData[0],  (pEventData[1]<<8 | pEventData[2]), pEventData[3]);
		}
		break;
		case FINGERPRINT_NOTIFY:
		{
			if(pEventData != NULL && pEventData[0] == 0  && u32EventLen >= 11)
				_CaFp(env, g_CaMsgObj, pEventData[1], (int)(pEventData+2));
		}
		break;
		case USER_VIEW_INFO_NOTIFY:
		{
			if(pEventData != NULL && u32EventLen >= 1 && pEventData[0] == 1)
				_CaUseViewMessage(env, g_CaMsgObj,(int)pPrivateData);
			}
		break;
		case PROG_SKIP_NOTIFY:
		{
			if(pEventData != NULL && u32EventLen >= 1 && pEventData[0] == 0)
			_caEnhanceChanPlay(env, g_CaMsgObj,(int)pPrivateData);
		}
		break;
		case PPV_LIST_INFO_NOTIFY:
		{
			if(pEventData != NULL && u32EventLen >= 1 && pEventData[0] == 0)
			_CaPPVListMessage(env, g_CaMsgObj,(int)pPrivateData);
		}
		break;
		case IPPV_LIST_INFO_NOTIFY:
		{
			if(pEventData != NULL && u32EventLen >= 1 && pEventData[0] == 0)
			_CaIppvListMessage(env, g_CaMsgObj,(int)pPrivateData);
		}
		break;
		case PROG_PARENTAL_RATE_NOTIFY:
		case SECURITY_INIT_ERROR_NOTIFY:

		case COS_SYSTEM_NOTIFY:
		break;
		default: 
		{
			
		}
		break;
	}
}
#else
void CA_Message_CallBack(int eCaMsg, unsigned short wDemux,  int ulParam, int param2 )
{
	JNIEnv *env = NULL;
	
	if( NULL == g_caVM || NULL == g_CaMsgObj )
	{
		DEBUG_MSG(-1,T("ca message-- ---------- mjvm or obj is null--------------------"));
		return;
	}
	
	if (g_caVM->AttachCurrentThread(&env, NULL) != JNI_OK)
	{
		 DEBUG_MSG1(-1,T("%s: AttachCurrentThread() failed"), __FUNCTION__);
		 return;
	}
	if( NULL == env )
	{
		DEBUG_MSG1(-1,T("%s: new is null "), __FUNCTION__);
		return;
	}
#if 0
	switch( eCaMsg )
	{
		case CAS_MC_DECODER_INFO:/*解扰信息------errmsg  不需要处理*/		
		case CAS_MC_SERVICE_INFO:/*节目信息errmsg不需要处理*/
		{
        }
        break;
		
		case CAS_MC_SC_ACCESS:/*节目信息errmsg对应显示E04或是e07*/
		{
			//_utiCaServiceMessage();
			DEBUG_MSG2(-1,T("ca message- SC- - dfg---------eCaMsg[%d]  wDemuxl[%d]--------------------"),eCaMsg,wDemux);
			_CaServiceMessage(env, g_CaMsgObj,0,0,0,wDemux);
			
        }
        break;
		case CAS_MC_IPP_NOTIFY:/*IPP */
		{
			_CaIppNotifyMessage(env, g_CaMsgObj,param2);
        }
        break;
		case CAS_MC_IPP_INFO_UPDATE:/*IPPV */
		{
			//_CaIppNotifyMessage(env, g_CaMsgObj,param2);
			_CaIppUpdate(env, g_CaMsgObj,param2);
		}
		break;
		case CAS_MC_ENHANCED_SHORT_MESSAGE:/*邮件*/
		{
			
		}
		break;
		case CAS_MC_ENHANCED_FINGER_PRINT:/*强制指纹显示*/
		{
			
        }
		break;
		case CAS_MC_FORCE_CHANNEL:/*强制频道切换*/
		{
			_caEnhanceChanPlay(env, g_CaMsgObj,param2);
		}
		break;
		case CAS_MC_FINGER_PRINT:/*指纹信息*/
		{
			_CaFp(env, g_CaMsgObj,param2);
		}
		break;
		case CAS_MC_EMM_DOWNLOAD:/*升级信息*/
		{
			
        }
		break;
		case CAS_MC_EMM_CHANGEPIN:/*智能卡密码更换 不需要处理*/
		case CAS_MC_PIN_CODE_CHECK:/*智能卡密码更换 不需要处理*/
		case CAS_MC_NOTIFY_EXPIRY_STATE:/*不需要处理*/
		case CAS_MC_MONITOR_ECM:/*不需要处理*/
		case CAS_MC_MONITOR_EMM:/*不需要处理*/
		{
			
        }
		break;
		case CAS_MC_NOTIFY_CURRENT_STATE:/*当前状态errmsg*/
		{
			/*根据wDemux 显示当前的状态 E00~E30,其中e00不用显示*/
			_CaServiceMessage(env, g_CaMsgObj,0,0,0,wDemux);
			
			DEBUG_MSG2(-1,T("ca message- err --sdf--------eCaMsg[%d]  wDemuxl[%d]--------------------"),eCaMsg,wDemux);
        }
        break;
		case CAS_MC_NOTIFY_SHORT_MESSAGE:/*短消息*/
		{
			_CaMailMessage(env, g_CaMsgObj,param2);			
		}
		break;
		default:
		{

		}
		break;
	}


	if(g_caVM->DetachCurrentThread() != JNI_OK) 
	{
		 DEBUG_MSG1(-1,T("%s: DetachCurrentThread() failed"), __FUNCTION__); 
	}
	#endif

}
#endif

