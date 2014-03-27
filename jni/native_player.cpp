#include <stdio.h>
//#include <jni.h>
#include <nativehelper/jni.h>
#include <dvbplayer.h>
#include <drv_avctrl.h>

#include "logit.h"
#include "native_player.h"

#include "jnistaple.h"
#include "tuner.h"

#include "ca_message.h"
#include "player_message.h"
#include "frontpanel.h"
#include <ca.h>
#include "message_nitupdate.h"
#include "pvr_message.h"

/*
 * Class:     com_pbi_dvb_dvbinterface_NativePlayer
 * Method:    DVBPlayer_Init
 * Signature: (Lcom/pbi/dvb/dvbinterface/NativePlayer/DVBPlayer_Config_t;)I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerInit(
		JNIEnv *env, jobject obj, jobject jConfigObj) {
	jint jRet = 0;

	DVBPlayer_Config_t tConfig;

	memset(&tConfig, 0, sizeof(DVBPlayer_Config_t));

	if (NULL == jConfigObj) {
		jRet = DVBPlayer_Init(NULL);
		DVBPlayer_RegCACallback(CA_CallBack);
	} else {
		/* jConfigObj -> tConfig */

		/*
		 *
		 *
		 *
		 */
		jRet = DVBPlayer_Init(&tConfig);
		DVBPlayer_RegCACallback(CA_CallBack);
	}

	return jRet;
}

/*
 * Class:     com_pbi_dvb_dvbinterface_NativePlayer
 * Method:    DVBPlayer_Deinit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerDeinit(
		JNIEnv *env, jobject obj) {
	jint jRet = 0;

	jRet = DVBPlayer_Deinit();

	JNI_PlayerMsg_Logout(env);
	JNI_CaMsg_Logout(env);
	JNI_NITMsg_Logout(env);

	return jRet;
}

/*
 * Class:     com_pbi_dvb_dvbinterface_NativePlayer
 * Method:    DVBPlayerAVCtrlGetWindows
 * Signature: (Lcom/pbi/dvb/dvbinterface/NativePlayer/DRV_AVCtrl_VidWindow_t;)I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerAVCtrlGetWindows(
		JNIEnv *env, jobject obj, jobject jGetWinObj) {
	jclass jObjCls = NULL;

	int iRet = 0;
	DRV_AVCtrl_VidWindow_t tVidOut_Window;

	memset(&tVidOut_Window, 0, sizeof(DRV_AVCtrl_VidWindow_t));

	iRet = DVBPlayer_Get_Attr(eDVBPLAYER_ATTR_VID_WINDWOS,
			(void *) &tVidOut_Window);
	if (0 != iRet) {

		return -1;
	}

	jObjCls = env->GetObjectClass(jGetWinObj);
	if (NULL == jObjCls) {

		//DeleteLocalRef( env, jObjCls );
		return -1;
	}

	SetIntCtoJava(env, jGetWinObj, jObjCls, "uX_Axis", tVidOut_Window.uX_Axis);
	SetIntCtoJava(env, jGetWinObj, jObjCls, "uY_Axis", tVidOut_Window.uY_Axis);
	SetIntCtoJava(env, jGetWinObj, jObjCls, "uWidth", tVidOut_Window.uWidth);
	SetIntCtoJava(env, jGetWinObj, jObjCls, "uHeight", tVidOut_Window.uHeight);

	return 0;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerSwitchAudio(
		JNIEnv *env, jobject obj, jobject audioobj) {
	jclass jObjCls = NULL;
	int iRet = 0;
	DVBPlayer_SWITCH_AUDIOPID_t t;
	jObjCls = env->GetObjectClass(audioobj);
	if (NULL == jObjCls) {
		return -1;
	}
	t.u16AudioPid = GetIntJavaToC(env, audioobj, jObjCls, "u16AudioPid");
	t.u16audiotype = GetIntJavaToC(env, audioobj, jObjCls, "u8AudeoFmt");
	iRet = DVBPlayer_Set_Attr(eDVBPLAYER_ATTR_SWITCH_AUDIOPID, (void *) &t);
	DEBUG_MSG3(-1,T("%s++++++++++++%d=ret%d"),__FUNCTION__,__LINE__,iRet);

	DeleteLocalRef(env, jObjCls);

	return iRet;
}
/*
 * Class:     com_pbi_dvb_dvbinterface_NativePlayer
 * Method:    DVBPlayerAVCtrlSetWindows
 * Signature: (Lcom/pbi/dvb/dvbinterface/NativePlayer/DRV_AVCtrl_VidWindow_t;)I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerAVCtrlSetWindows(
		JNIEnv *env, jobject obj, jobject jSetWinObj, jint isCheak) {
	jclass jObjCls = NULL;

	int iRet = 0;
	DRV_AVCtrl_VidWindow_t tVidOut_Window;
	DRV_AVCtrl_VidWindow_t tGetVidOut_Window;

	memset(&tVidOut_Window, 0, sizeof(DRV_AVCtrl_VidWindow_t));
	memset(&tGetVidOut_Window, 0, sizeof(DRV_AVCtrl_VidWindow_t));

	if(isCheak) {
		iRet = DVBPlayer_Get_Attr( eDVBPLAYER_ATTR_VID_WINDWOS, (void *)&tGetVidOut_Window );
		if( 0 != iRet )
		{
			return -1;
		}
	}
	else {
		tGetVidOut_Window.uBaseWidth = 0;
		tGetVidOut_Window.uBaseHeight = 0;
	}

	jObjCls = env->GetObjectClass(jSetWinObj);
	if (NULL == jObjCls) {
		return -1;
	}

	tVidOut_Window.uX_Axis = GetIntJavaToC(env, jSetWinObj, jObjCls, "uX_Axis");
	tVidOut_Window.uY_Axis = GetIntJavaToC(env, jSetWinObj, jObjCls, "uY_Axis");
	tVidOut_Window.uWidth = GetIntJavaToC(env, jSetWinObj, jObjCls, "uWidth");
	tVidOut_Window.uHeight = GetIntJavaToC(env, jSetWinObj, jObjCls, "uHeight");

	tVidOut_Window.uX_Axis = tVidOut_Window.uX_Axis; // * tGetVidOut_Window.uWidth / 1920;
	tVidOut_Window.uY_Axis = tVidOut_Window.uY_Axis; // * tGetVidOut_Window.uHeight / 1080;
	tVidOut_Window.uWidth = tVidOut_Window.uWidth; // * tGetVidOut_Window.uWidth / 1920;
	tVidOut_Window.uHeight = tVidOut_Window.uHeight; // * tGetVidOut_Window.uHeight / 1080;
	tVidOut_Window.uBaseWidth = tGetVidOut_Window.uWidth;
	tVidOut_Window.uBaseHeight = tGetVidOut_Window.uHeight;

	iRet = DVBPlayer_Set_Attr(eDVBPLAYER_ATTR_VID_WINDWOS,
			(void *) &tVidOut_Window);

	DeleteLocalRef(env, jObjCls);

	return iRet;
}

/*
 * Class:     com_pbi_dvb_dvbinterface_NativePlayer
 * Method:    DVBPlayerAVCtrlSetVolume
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerAVCtrlSetVolume(
		JNIEnv *env, jobject obj, jint Volueme) {
	int iRet = 0;
	int Volue = Volueme;
	iRet = DVBPlayer_Set_Attr(eDVBPLAYER_ATTR_AUD_SETVOLUME, (void *) &Volue);
	return iRet;
}
/*
 * Class:     com_pbi_dvb_dvbinterface_NativePlayer
 * Method:    DVBPlayerAVCtrlAudMute
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerAVCtrlAudMute(
		JNIEnv *env, jobject obj, jint isMute) {
	int iRet = 0;
	int Mute = isMute;
	iRet = DVBPlayer_Set_Attr(eDVBPLAYER_ATTR_AUD_MUTE, (void *) &Mute);
	return iRet;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_TestPlay1(
		JNIEnv *env, jobject obj) {
	dal_http_start();
	return 0;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_TestPlay2(
		JNIEnv *env, jobject obj) {
	dal_http_restart();
	//dal_http_start2();
	return 0;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_TestStop(
		JNIEnv *env, jobject obj) {
	dal_http_stop();
	return 0;
}
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerAVCtrlAudMode(
		JNIEnv *env, jobject obj, jint mode) {
	int iRet = 0;
	int imode = mode;
	iRet = DVBPlayer_Set_Attr(eDVBPLAYER_ATTR_AUD_MODE, (void *) &imode);
	return iRet;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerAVCtrlGetAudMode(
		JNIEnv *env, jobject obj) {
	int iRet = 0;

	DRV_AVCtrl_AudStereo_t mode;

	iRet = DVBPlayer_Get_Attr(eDVBPLAYER_ATTR_AUD_MODE, &mode);
	DEBUG_MSG3(-1,T("%s++++++++++++%d=ret%d"),__FUNCTION__,__LINE__,iRet);

	if (iRet != 0) {

		return -1;
	}

	return mode;
}

/*
 * Class:     com_pbi_dvb_dvbinterface_NativePlayer
 * Method:    DVBPlayer_Play_Program
 * Signature: (Lcom/pbi/dvb/dvbinterface/NativePlayer/DVBPlayer_Play_Param_t;)I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPlayProgram(
		JNIEnv *env, jobject obj, jobject jPlayProgObj) {
	jclass jObjCls = NULL;
	jclass jClsTemp = NULL;
	jfieldID jFidTemp = NULL;
	jobject jObjTemp = NULL;
	jint jRet = 0;

	int iTunerType = 0;
	int iProgType = 0;
	bool bRet = TRUE;

	DVBPlayer_Play_Param_t tParam;

	memset(&tParam, 0, sizeof(DVBPlayer_Play_Param_t));

	jObjCls = env->GetObjectClass(jPlayProgObj);
	if (NULL == jObjCls) {
		return -1;
	}

	iTunerType = GetIntJavaToC(env, jPlayProgObj, jObjCls, "eSignalSource");
	iProgType = GetIntJavaToC(env, jPlayProgObj, jObjCls, "eProgType");
	switch (iTunerType) {
	case eDVBCORE_SIGNAL_SOURCE_DVBC: {
		jFidTemp = env->GetFieldID(jObjCls, "tCable",
				"Lcom/pbi/dvb/dvbinterface/Tuner$DVBCore_Cab_Desc_t;");
		if (jFidTemp == NULL) {
			DEBUG_MSG2(-1,T("+++++++++++++%s=====obj is null=====%d-------"),__FUNCTION__,__LINE__);
			return -1;
		}
		jObjTemp = env->GetObjectField(jPlayProgObj, jFidTemp);
		if (jObjTemp == NULL) {
			DEBUG_MSG2(-1,T("+++++++++++++%s=====obj is null=====%d-------"),__FUNCTION__,__LINE__);
			return -1;
		}
		bRet = GetDvbCroeCableDescClass(env, jObjTemp,
				&tParam.uTunerParam.tCable);
		if (TRUE != bRet) {

			DeleteLocalRef(env, jObjCls);
			DeleteLocalRef(env, jObjTemp);

			return -1;
		}

	}
		break;
	default: {

	}
		break;
	}

	DeleteLocalRef(env, jObjTemp);

	switch (iProgType) {
	case 0: {
		jFidTemp =
				env->GetFieldID(jObjCls, "tServParam",
						"Lcom/pbi/dvb/dvbinterface/NativePlayer$DVBPlayer_Serv_Param_t;");
		if (jFidTemp == NULL) {
			DEBUG_MSG2(-1,T("%s++++++++++++%d="),__FUNCTION__,__LINE__);
			return -1;
		}
		jObjTemp = env->GetObjectField(jPlayProgObj, jFidTemp);
		if (jObjTemp == NULL) {
			DEBUG_MSG2(-1,T("%s++++++++++++%d="),__FUNCTION__,__LINE__);
			return -1;
		}
		jClsTemp = env->GetObjectClass(jObjTemp);
		if (jClsTemp == NULL) {
			DEBUG_MSG2(-1,T("%s++++++++++++%d="),__FUNCTION__,__LINE__);
			return -1;
		}

		tParam.uProgParam.tServParam.eProgType =
				(DVBPlayer_Prog_Type_e) iProgType; //DVBPLAYER_PROG_TYPE_SERVICE;
		tParam.uProgParam.tServParam.eServLockFlag =
				(DVBPlayer_Serv_Lock_e) GetIntJavaToC(env, jObjTemp, jClsTemp,
						"eServLockFlag");
		tParam.uProgParam.tServParam.u16ServId = GetIntJavaToC(env, jObjTemp,
				jClsTemp, "u16ServId");
		tParam.uProgParam.tServParam.u16PMTPid = GetIntJavaToC(env, jObjTemp,
				jClsTemp, "u16PMTPid");
		tParam.uProgParam.tServParam.u16LogicNum = GetIntJavaToC(env, jObjTemp,
				jClsTemp, "u16LogicNum");
		tParam.uProgParam.tServParam.u16TsId = GetIntJavaToC(env, jObjTemp,
				jClsTemp, "u16TS_Id");
		tParam.uProgParam.tServParam.u16NetId = GetIntJavaToC(env, jObjTemp,
				jClsTemp, "u16NetId");
		tParam.uProgParam.tServParam.u16historyAudioPid = GetIntJavaToC(env,
				jObjTemp, jClsTemp, "u16AudioPID");
		DEBUG_MSG3(-1,T("+++++++++++++%d==========%d-------audioPID%d"),tParam.uProgParam.tServParam.u16ServId,tParam.uProgParam.tServParam.u16PMTPid,
				tParam.uProgParam.tServParam.u16historyAudioPid);
	}
		break;
	case 1: {
		jFidTemp = env->GetFieldID(jObjCls, "tAVPids",
				"Lcom/pbi/dvb/dvbinterface/NativePlayer$DVBPlayer_AVPid_t;");
		jObjTemp = env->GetObjectField(jPlayProgObj, jFidTemp);
		jClsTemp = env->GetObjectClass(jObjTemp);

		tParam.uProgParam.tAVPids.eProgType = (DVBPlayer_Prog_Type_e) iProgType; //DVBPLAYER_PROG_TYPE_AVPID;
		tParam.uProgParam.tAVPids.u16AudioPid = GetIntJavaToC(env, jObjTemp,
				jClsTemp, "u16AudioPid");
		tParam.uProgParam.tAVPids.u16VideoPid = GetIntJavaToC(env, jObjTemp,
				jClsTemp, "u16VideoPid");
		tParam.uProgParam.tAVPids.u16AudioPid = GetIntJavaToC(env, jObjTemp,
				jClsTemp, "u16AudioPid");
		tParam.uProgParam.tAVPids.u16PCRPid = GetIntJavaToC(env, jObjTemp,
				jClsTemp, "u16PCRPid");
		tParam.uProgParam.tAVPids.u8AudioFmt = GetByteJavaToC(env, jObjTemp,
				jClsTemp, "u8AudioFmt");
		tParam.uProgParam.tAVPids.u8VideoFmt = GetByteJavaToC(env, jObjTemp,
				jClsTemp, "u8VideoFmt");

	}
		break;
	default: {

	}
		break;
	}

	Front_Panel_Dispaly(env, tParam.uProgParam.tServParam.u16LogicNum);

	jRet = DVBPlayer_Play_Program(&tParam);

	DeleteLocalRef(env, jObjCls);
	DeleteLocalRef(env, jClsTemp);
	DeleteLocalRef(env, jObjTemp);
	DEBUG_MSG2(-1,T("+++++++++++++%s== END ===%d-------"),__FUNCTION__,__LINE__);
	return jRet;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerSetStopMode(
		JNIEnv *env, jobject obj, jint mode) {
	jint jRet = 0;
	int StopMode = 0;

	StopMode = mode;
	jRet = DVBPlayer_Set_Attr(eDVBPlayer_ATTR_VID_STOP_MODE, (void*) &StopMode);

	return jRet;
}

/*
 * Class:     com_pbi_dvb_dvbinterface_NativePlayer
 * Method:    DVBPlayer_Stop
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerStop(
		JNIEnv *env, jobject obj) {
	jint jRet = 0;

	jRet = DVBPlayer_Stop();

	return jRet;
}

/*
 * Class:     com_pbi_dvb_dvbinterface_NativePlayer
 * Method:    DVBPlayer_Get_Info
 * Signature: (Lcom/pbi/dvb/dvbinterface/NativePlayer/DVBPlayer_Info_t;)I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerGetInfo(
		JNIEnv *env, jobject obj, jobject jInfoObj) {
	jint jRet = 0;
	jclass jCls = NULL;

	DVBPlayer_Info_t tInfo;

	memset(&tInfo, 0, sizeof(DVBPlayer_Info_t));

	jRet = DVBPlayer_Get_Info(&tInfo);

	DEBUG_MSG2(-1,T("LinKang:: tInfo.u16VideoPid = %d, tInfo.u16AudioPid = %d"),tInfo.u16VideoPid ,tInfo.u16AudioPid);

	if (0 != jRet) {
		return jRet;
	}

	jCls = env->GetObjectClass(jInfoObj);

	if (NULL == jCls) {
		DEBUG_MSG2(-1,T("LinKang:: jCls = env->GetObjectClass( jInfoObj ) return null...%s%d"),__FUNCTION__,__LINE__);

		return -1;
	}

	SetIntCtoJava(env, jInfoObj, jCls, "eRunningStatus", tInfo.eRunningStatus);
	SetIntCtoJava(env, jInfoObj, jCls, "eTunerStatus", tInfo.eTunerStatus);
	SetIntCtoJava(env, jInfoObj, jCls, "eProgType", tInfo.eProgType);
	SetIntCtoJava(env, jInfoObj, jCls, "u16ServiceId", tInfo.u16ServiceId);
	SetIntCtoJava(env, jInfoObj, jCls, "u16PMTPid", tInfo.ePMTStatus);
	SetIntCtoJava(env, jInfoObj, jCls, "u16VideoPid", tInfo.u16VideoPid);
	SetIntCtoJava(env, jInfoObj, jCls, "u16AudioPid", tInfo.u16AudioPid);
	SetIntCtoJava(env, jInfoObj, jCls, "u16AudioPid", tInfo.u16AudioPid);
	SetIntCtoJava(env, jInfoObj, jCls, "u16PCRPid", tInfo.u16PCRPid);

	DEBUG_MSG2(-1,T("LinKang:: tInfo.u16VideoPid = %d, tInfo.u16AudioPid = %d"),tInfo.u16VideoPid ,tInfo.u16AudioPid);

	DeleteLocalRef(env, jCls);

	return jRet;
}
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerGetCaMode(
		JNIEnv *env, jobject obj) {
	int mode = DVBPlayer_GetCamode();

	DEBUG_MSG3(-1,T("+++++++++++++%s== END ===%d-------mode %d"),__FUNCTION__,__LINE__,mode);

	return mode;

}

#if 0
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrInit
(JNIEnv *env, jobject obj )
{
	//DVBPlayer_PVR_Init((DvbPvrEventCallBack)PvrEventCallBack);
	return 0;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrDeInit
(JNIEnv *env, jobject obj )
{
	//DVBPlayer_PVR_UnInit();
	JNI_PvrMsg_Logout(env);
	return 0;
}
#endif

/* ---------------------------------------------------------------------------------- */
/* -------------------------------PVR Rec------------------------------------------- */
/* ---------------------------------------------------------------------------------- */

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrRecStart(
		JNIEnv *env, jobject obj, jobject RecObj) {
	jclass RecCls = NULL;
	jobject TunerObj = NULL;
	jstring jName = NULL;
	jfieldID jFidTemp = NULL;
	DVBPlayer_PVR_REC_t tPvrRecParam;

	int iTemp = 0;
	bool bRet = TRUE;

	RecCls = env->GetObjectClass(RecObj);
	if (NULL == RecCls) {
		DEBUG_MSG2(-1, "[%s %d]  class is NULL !!!", __FUNCTION__, __LINE__ );
		return -1;
	}

	memset(&tPvrRecParam, 0, sizeof(DVBPlayer_PVR_REC_t));

	tPvrRecParam.u8RecFlag = GetIntJavaToC(env, RecObj, RecCls, "u8RecFlag");
	tPvrRecParam.u16LogicNum = GetIntJavaToC(env, RecObj, RecCls,
			"u16LogicNum");
	tPvrRecParam.u16ServId = GetIntJavaToC(env, RecObj, RecCls, "u16ServId");
	tPvrRecParam.u16PmtPid = GetIntJavaToC(env, RecObj, RecCls, "u16PmtPid");
	tPvrRecParam.u64MaxFileLength = GetDoubleJavaToC(env, RecObj, RecCls,
			"u64MaxFileLength");

	jName = GetStringJavaToC(env, RecObj, RecCls, "sName");
	Jstringtoc_UTF8(env, jName, tPvrRecParam.FileName);
	DEBUG_MSG2(-1, "FileName =%s...Namelength=%d..\n", tPvrRecParam.FileName,strlen(tPvrRecParam.FileName) );
	tPvrRecParam.FileName[strlen(tPvrRecParam.FileName)] = '\0';
	if (0 == tPvrRecParam.u8RecFlag) {

		iTemp = GetIntJavaToC(env, RecObj, RecCls, "eTunerType");
		switch (iTemp) {
		case eDVBCORE_SIGNAL_SOURCE_DVBC: {
			jFidTemp = env->GetFieldID(RecCls, "tCableInfo",
					"Lcom/pbi/dvb/dvbinterface/Tuner$DVBCore_Cab_Desc_t;");
			if (NULL == jFidTemp) {
				DEBUG_MSG(-1,T("DVBPlayerPvrRecStart DVBCore_Cab_Desc_t error!"));
				DeleteLocalRef(env, RecCls);
				return -1;
			}

			TunerObj = env->GetObjectField(RecObj, jFidTemp);
			bRet = GetDvbCroeCableDescClass(env, TunerObj,
					&tPvrRecParam.tBackRec.uTunerDesc.tCable);
			if (FALSE == bRet) {

				DEBUG_MSG(-1,T("DVBPlayerPvrRecStart DvbCroeCableDescClass error!"));

				DeleteLocalRef(env, RecCls);
				DeleteLocalRef(env, TunerObj);
				return -1;
			}

			DeleteLocalRef(env, TunerObj);

		}
			break;
		default: {
		}
		}
	} else {

	}
	DeleteLocalRef(env, RecCls);

	return DVBPlayer_REC_Play(&tPvrRecParam);
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrRecStop(
		JNIEnv *env, jobject obj) {
	DVBPlayer_REC_Stop();
	return 0;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrRecGetStatus(
		JNIEnv *env, jobject obj, jobject RecStatusObj) {
	int iRet = 0;

	jclass jRecStatusCls = NULL;

	DVBPlayer_PVR_REC_STATUS_t tRecStatus;

	memset(&tRecStatus, 0, sizeof(DVBPlayer_PVR_REC_STATUS_t));

	iRet = DVBPlayer_PVR_RecGetStatus(&tRecStatus);
	if (0 != iRet) {
		DEBUG_MSG2(-1, "[%s %d] DVBPlayer_PVR_RecGetStatus error!", __FUNCTION__, __LINE__ );
		return -1;
	}

	jRecStatusCls = env->GetObjectClass(RecStatusObj);
	if (NULL == jRecStatusCls) {
		DEBUG_MSG2(-1, "[%s %d]  class is NULL !!!", __FUNCTION__, __LINE__ );
		return -1;
	}

	SetIntCtoJava(env, RecStatusObj, jRecStatusCls, "enState",
			tRecStatus.enState);
	SetIntCtoJava(env, RecStatusObj, jRecStatusCls, "u32Reserved",
			tRecStatus.u32Reserved);
	SetIntCtoJava(env, RecStatusObj, jRecStatusCls, "u32CurWriteFrame",
			tRecStatus.u32CurWriteFrame);
	SetIntCtoJava(env, RecStatusObj, jRecStatusCls, "u32CurTimeInMs",
			tRecStatus.u32CurTimeInMs);
	SetIntCtoJava(env, RecStatusObj, jRecStatusCls, "u32StartTimeInMs",
			tRecStatus.u32StartTimeInMs);
	SetIntCtoJava(env, RecStatusObj, jRecStatusCls, "u32EndTimeInMs",
			tRecStatus.u32EndTimeInMs);
	SetIntCtoJava(env, RecStatusObj, jRecStatusCls, "u32BufSize",
			tRecStatus.u32BufSize);
	SetIntCtoJava(env, RecStatusObj, jRecStatusCls, "u32UsedSize",
			tRecStatus.u32UsedSize);
	SetDoubleCtoJava(env, RecStatusObj, jRecStatusCls, "u64CurWritePos",
			tRecStatus.u64CurWritePos);

	return 0;
}

/*
 * Class:     com_pbi_dvb_dvbinterface_NativePlayer
 * Method:    DVBPlayerAVCtrlSetVPathPara
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerAVCtrlSetVPathPara
  (JNIEnv *env, jobject obj, jint dvb )
{
	int iRet = 0;
	int isDvb = dvb;
	iRet = DVBPlayer_Set_Attr( eDVBPLAYER_ATTR_SET_VPARA, (void *)&isDvb );
	return iRet;
}


JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrRemoveFile(
		JNIEnv *env, jobject obj, jstring sFileName) {
	int iRet = 0;
	char Name[128];

	memset(Name, 0, 128);
	iRet = Jstringtoc_UTF8(env, sFileName, Name);
	if (0 != iRet) {
		DEBUG_MSG2(-1, "[%s %d] The Name is String to C error!", __FUNCTION__, __LINE__ );
		return -1;
	}

	DVBPlayer_PVR_PvrRemoveFile(Name);
	return 0;
}

/* ---------------------------------------------------------------------------------- */
/* -------------------------------PVR Player------------------------------------------- */
/* ---------------------------------------------------------------------------------- */

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrPlayStart(
		JNIEnv *env, jobject obj, jobject PvrPlayObj) {
	jclass PvrPlayCls = NULL;
	jstring jName = NULL;
	DVBPlayer_PVR_Play_t tPlayerInfo;

	PvrPlayCls = env->GetObjectClass(PvrPlayObj);
	if (NULL == PvrPlayCls) {
		DEBUG_MSG2(-1, "[%s %d]  class is NULL !!!", __FUNCTION__, __LINE__ );
		return -1;
	}

	memset(&tPlayerInfo, 0, sizeof(DVBPlayer_PVR_Play_t));

	tPlayerInfo.PvrPlayType = (PVR_PLAYER_TYPE_E) GetIntJavaToC(env, PvrPlayObj,
			PvrPlayCls, "PvrPlayerType");
	jName = GetStringJavaToC(env, PvrPlayObj, PvrPlayCls, "sName");
	Jstringtoc_UTF8(env, jName, tPlayerInfo.FileName);

	return DVBPlayer_PVR_Play(&tPlayerInfo);;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrPlayStop(
		JNIEnv *env, jobject obj) {
	return DVBPlayer_PVR_Stop();;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrPlayPause(
		JNIEnv *env, jobject obj) {
	DVBPlayer_PVR_Pause();
	return 0;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrPlayResume(
		JNIEnv *env, jobject obj) {
	DVBPlayer_PVR_Resume();
	return 0;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrPlaySpeedCtrl(
		JNIEnv *env, jobject obj, jint Speed) {
	DVBPlayer_PVR_Speed(Speed);
	return 0;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrPlaySeekCtrl(
		JNIEnv *env, jobject obj, jint Speed) {
	DVBPlayer_PVR_Seek(Speed);
	return 0;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrPlayGetFileInfo(
		JNIEnv *env, jobject obj, jobject jFileInfoObj) {
	jclass jCls = NULL;
	DVBPlayer_PVR_FILE_ATTR_t tPvrFileAttr;

	jCls = env->GetObjectClass(jFileInfoObj);
	if (NULL == jCls) {
		return -1;
	}

	memset(&tPvrFileAttr, 0, sizeof(DVBPlayer_PVR_FILE_ATTR_t));
	DVBPlayer_Set_Attr(eDVBPLAYER_ATTR_PVR_GET_FILE_INFO,
			(void *) &tPvrFileAttr);

	SetIntCtoJava(env, jFileInfoObj, jCls, "eIndxType",
			tPvrFileAttr.u32IdxType);
	SetIntCtoJava(env, jFileInfoObj, jCls, "u32FrameNum",
			tPvrFileAttr.u32FrameNum);
	SetIntCtoJava(env, jFileInfoObj, jCls, "u32StartTimeInMs",
			tPvrFileAttr.u32StartTimeInMs);
	SetIntCtoJava(env, jFileInfoObj, jCls, "u32EndTimeInMs",
			tPvrFileAttr.u32EndTimeInMs);
	SetDoubleCtoJava(env, jFileInfoObj, jCls, "u64ValidSizeInByte",
			tPvrFileAttr.u64ValidSizeInByte);

	DeleteLocalRef(env, jCls);

	return 0;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativePlayer_DVBPlayerPvrPlayGetStatus(
		JNIEnv *env, jobject obj, jobject jPlayStautsObj) {
	jclass jCls = NULL;
	DVBPlayer_PVR_PLAY_STATUS_t tPvrStatusInfo;

	jCls = env->GetObjectClass(jPlayStautsObj);
	if (NULL == jCls) {
		return -1;
	}
	memset(&tPvrStatusInfo, 0, sizeof(DVBPlayer_PVR_PLAY_STATUS_t));
	DVBPlayer_Set_Attr(eDVBPLAYER_ATTR_PVR_GET_STATUS,
			(void *) &tPvrStatusInfo);

	SetIntCtoJava(env, jPlayStautsObj, jCls, "eState", tPvrStatusInfo.u32State);
	SetIntCtoJava(env, jPlayStautsObj, jCls, "eSpeed", tPvrStatusInfo.u32Speed);
	SetIntCtoJava(env, jPlayStautsObj, jCls, "u32CurPlayFrame",
			tPvrStatusInfo.u32CurPlayFrame);
	SetIntCtoJava(env, jPlayStautsObj, jCls, "u32CurPlayTimeInMs",
			tPvrStatusInfo.u32CurPlayTimeInMs);
	SetDoubleCtoJava(env, jPlayStautsObj, jCls, "u64CurPlayPos",
			tPvrStatusInfo.u64CurPlayPos);

	DeleteLocalRef(env, jCls);
	return 0;
}
