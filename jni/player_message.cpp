#include <stdio.h>
#include <limits.h>
#include <jni.h>
#include <android_runtime/AndroidRuntime.h>
#include <dvbplayer.h>

#include "logit.h"
#include "jnistaple.h"
#include "player_manage.h"
#include "player_message.h"


static JavaVM*	   g_playerVM = NULL;
static jobject g_PlayerMsgObj = NULL;


JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerMessageInit
(JNIEnv *env, jobject obj, jobject PlayerMsgObj)
{
	DeleteGlobalRef( env, g_PlayerMsgObj);
	g_PlayerMsgObj = NULL;

	env->GetJavaVM(&g_playerVM);
	
	if( TRUE != _register_player_msg_callback_methods( env ))
	{
		return 1;
	}

	DVBPlayer_RegCallback(Player_Message_CallBack);
	
	g_PlayerMsgObj = env->NewGlobalRef(PlayerMsgObj);

	DEBUG_MSG(-1, "PlayerMessageInit ok");
	return 0;
}


void JNI_PlayerMsg_Logout(JNIEnv *env)
{
	_logout_player_msg_callback_methods(env);
	DeleteGlobalRef( env, g_PlayerMsgObj );
	g_PlayerMsgObj = NULL;
	if( NULL != g_playerVM )
	{
		g_playerVM = NULL;
	}
}

int Player_Message_CallBack(DVBPlayer_Event_t tEvent)
{
	JNIEnv *env = NULL;

	if( NULL == g_playerVM || NULL == g_PlayerMsgObj )
	{
		//DEBUG_MSG(-1,T("Installation_Call_Back-- ---------- mjvm or obj is null--------------------"));
		return -1;
	}

	if (g_playerVM->AttachCurrentThread(&env, NULL) != JNI_OK)
	{
		 DEBUG_MSG1(-1,T("%s: AttachCurrentThread() failed"), __FUNCTION__);
		 return -1;
	}
	if( NULL == env )
	{
		DEBUG_MSG1(-1,T("%s: new is null "), __FUNCTION__);
		return -1;
	}
	
	switch(tEvent.eEventType)
	{
		case eDVBPLAYER_EVENT_INVALID:
		case eDVBPLAYER_EVENT_TUNER_DROPPED:
		{
			_playerMsgInterrupt( env, g_PlayerMsgObj );
		}break;
		case eDVBPLAYER_EVENT_PLAY_START:
		{
			_playerMsgStart( env, g_PlayerMsgObj );
		}break;
		case eDVBPLAYER_EVENT_PMT_TIMEOUT:
		{
			_playerMsgPmtTimeOut( env, g_PlayerMsgObj );
		}break;
		case eDVBPLAYER_EVENT_AV_INTERRUPT:
		{
			_playerMsgAVinterrupt( env, g_PlayerMsgObj );
		}
		break;
		case eDVBPLAYER_EVENT_TUNER_LOCK:
		{
			_playerMsgTunerLock( env, g_PlayerMsgObj );
		}break;
		case eDVBPLAYER_EVENT_AUDIO_INFOS:
		{
			DEBUG_MSG1(-1,"Player_Message_CallBack	line=%d.",__LINE__);
			if(tEvent.u32EventLen >0)	
			{
				_playerMsgAudioInfos( env, g_PlayerMsgObj,(void*)(tEvent.pEventData),(int)(tEvent.u32EventLen));
			}
		}
		break;
		case eDVBPLAYER_EVENT_CA_MODE:
		{
			DEBUG_MSG1(-1,"Player_Message_CallBack	line=%d.",__LINE__);
			_playerCaMode(env, g_PlayerMsgObj,(int)(tEvent.u32EventLen));
		}
		break;
		case eDVBPLAYER_EVENT_UPDATE_PMT:
		{
			DVBPlayer_Update_PMT_t *pNewPmtInfo = (DVBPlayer_Update_PMT_t *)tEvent.pEventData;
			_playerMsgUpdatePmtInfo( env, g_PlayerMsgObj, \
				pNewPmtInfo->u16TsId, pNewPmtInfo->u16ServiceId, pNewPmtInfo->u16PmtPid );
		}
		break;
		default:
		{
			DEBUG_MSG1(-1,"Player Message type is %d.",(int)tEvent.eEventType);
			//eDVBPLAYER_EVENT_SET_SERV,
			//eDVBPLAYER_EVENT_SET_AVPID,
		}break;
	}


	if(g_playerVM->DetachCurrentThread() != JNI_OK) 
	{
	 DEBUG_MSG1(-1,T("%s: DetachCurrentThread() failed"), __FUNCTION__); 
	}

	return 0;
}

