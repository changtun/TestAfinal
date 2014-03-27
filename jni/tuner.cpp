#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include <osapi.h>
#include <dvbcore.h>
#include <drv_tuner.h>
#include <file_config.h>

#include "tuner.h"
#include "logit.h"
#include "jnistaple.h"

bool GetDvbCroeCableDescClass(JNIEnv *env, jobject ObjCable, DVBCore_Cab_Desc_t *ptCable )
{
	jclass CableCls;
	bool ret = TRUE;
	
	CableCls = env->GetObjectClass( ObjCable );
	if( NULL == CableCls || NULL == ptCable )
	{
		DEBUG_MSG1(-1,T("==================address 0x%x..."),ptCable);
		ret = FALSE;
		goto OUT_GET_DVB_CROE_CABLE_CLASS;
	}
	
	ptCable->eSignalSource	=	eDVBCORE_SIGNAL_SOURCE_DVBC;
	ptCable->u8TunerId		=	GetCharJavaToC( env, ObjCable, CableCls, "u8TunerId");
	ptCable->u32Freq		=	GetIntJavaToC( env, ObjCable, CableCls, "u32Freq");
	ptCable->u32SymbRate	=	GetIntJavaToC( env, ObjCable, CableCls, "u32SymbRate");
	ptCable->eMod			=	(DVBCore_Modulation_e)GetIntJavaToC( env, ObjCable, CableCls, "eMod");
	ptCable->eFEC_Inner		=	(DVBCore_Conv_Code_Rate_e)GetIntJavaToC( env, ObjCable, CableCls, "eFEC_Inner");
	ptCable->eFEC_Outer		=	(DVBCore_FEC_Outer_e)GetIntJavaToC( env, ObjCable, CableCls, "eFEC_Outer");
	
	DEBUG_MSG1( -1,"======eSignalSource=============%d=============", ptCable->eSignalSource);
	DEBUG_MSG1( -1,"======u8TunerId=============%d=============", ptCable->u8TunerId);
	DEBUG_MSG1( -1,"======u32Freq=============%d=============", ptCable->u32Freq);
	DEBUG_MSG1( -1,"======u32SymbRate=============%d=============", ptCable->u32SymbRate);
	DEBUG_MSG1( -1,"======eMod=============%d=============", ptCable->eMod);
	DEBUG_MSG1( -1,"======eFEC_Inner=============%d=============", ptCable->eFEC_Inner);
	DEBUG_MSG1( -1,"======eFEC_Outer=============%d=============", ptCable->eFEC_Outer);

OUT_GET_DVB_CROE_CABLE_CLASS:

	DeleteLocalRef(env, CableCls);

	return ret;
}


bool SetDvbCroeCableDescClass(JNIEnv *env, jobject ObjCable, DVBCore_Cab_Desc_t tCable )
{
	jclass CableCls;
	bool ret = TRUE;
	
	CableCls = env->GetObjectClass( ObjCable );
	if( NULL == CableCls )
	{
		ret = FALSE;
		goto OUT_SET_DVB_CROE_CABLE_CLASS;
	}
	
	SetCharCtoJava( env, ObjCable, CableCls, "u8TunerId", tCable.u8TunerId );
	SetIntCtoJava( env, ObjCable, CableCls, "u32Freq", tCable.u32Freq );
	SetIntCtoJava( env, ObjCable, CableCls, "u32SymbRate", tCable.u32SymbRate );
	SetIntCtoJava( env, ObjCable, CableCls, "eMod", (jint)tCable.eMod );
	SetIntCtoJava( env, ObjCable, CableCls, "eFEC_Inner", (jint)tCable.eFEC_Inner );
	SetIntCtoJava( env, ObjCable, CableCls, "eFEC_Outer", (jint)tCable.eFEC_Outer );
	
OUT_SET_DVB_CROE_CABLE_CLASS:
	DeleteLocalRef(env, CableCls);
	return ret;

}

/*
	getTunerStrengthPercent
*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Tuner_getTunerStrengthPercent
  (JNIEnv *env, jobject obj, jint tunerId)
{
	unsigned int percent = 0;
	jint jRet = 0;
	TUNER_LOCKSTATUS_t  tunerState = eTun_unLock;

	tunerState = DRV_Tuner_GetLockStatus((unsigned char)tunerId);
	if( eTun_Lock != tunerState )
	{
		return 0;
	}
	
	jRet = DRV_Tuner_GetTunInfoStrengthPercent( (unsigned char)tunerId, &percent );
	if( eTUNER_NO_ERROR != jRet )
	{
		return 0;
	}
	
	return percent;
}

/*
	getTunerQualityPercent
*/
JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_Tuner_getTunerQualityPercent
  (JNIEnv *env, jobject obj, jint tunerId)
{
	unsigned int percent = 0;
	jint jRet = 0;
	TUNER_LOCKSTATUS_t  tunerState = eTun_unLock;

	tunerState = DRV_Tuner_GetLockStatus((unsigned char)tunerId);
	if( eTun_Lock != tunerState )
	{
		return 0;
	}
	
	jRet = DRV_Tuner_GetTunInfoQualityPercent( (unsigned char)tunerId, &percent );
	if( eTUNER_NO_ERROR != jRet )
	{
		return 0;
	}
	
	return percent;
}
/*
	getTunerCablePercent
*/
JNIEXPORT void JNICALL Java_com_pbi_dvb_dvbinterface_Tuner_getTunerCablePercent
  (JNIEnv *env, jobject obj, jobject TunerObj, jobject PercentObj)
{
	jclass TunerCls = NULL;
	jclass PercentCls = NULL;
	int iRet = 0;
	unsigned int retry = 0;
	unsigned char ucTunerId = 0;
	unsigned int uiPercent = 0;
	

	TUNER_LOCKSTATUS_t  tunerState = eTun_unLock;
	ts_src_info_t tSrcInfo;
	memset( &tSrcInfo, 0, sizeof(ts_src_info_t) );
	
	TunerCls = env->GetObjectClass( TunerObj );
	if( NULL == TunerCls )
	{
		DEBUG_MSG(-1,"-------------GetTunerCablePercent getClass is NULL----------------");
		goto OUT_CablePercent;
	}

	ucTunerId = GetCharJavaToC( env, TunerObj, TunerCls, "u8TunerId");
    tSrcInfo.MediumType = MEDIUM_TYPE_DVBC;
    tSrcInfo.u.Cable.FrequencyKHertz = GetIntJavaToC( env, TunerObj, TunerCls, "u32Freq");
    tSrcInfo.u.Cable.SymbolRateSPS = GetIntJavaToC( env, TunerObj, TunerCls, "u32SymbRate");
    tSrcInfo.u.Cable.Modulation = (tPI_Modulation)(GetIntJavaToC( env, TunerObj, TunerCls, "eMod") + 5);

 tSrcInfo.u.Cable.Bandwidth = GetTunerBandWidth();
	iRet = DRV_Tuner_SetFrequency( ucTunerId, &tSrcInfo, 0 );
	if( eTUNER_NO_ERROR != iRet )
	{
		DEBUG_MSG1(-1,"--------------GetTunerCablePercent SetFrequency error-----%d-------",iRet);
		goto OUT_CablePercent;
	}
	
	do
    {
        if( retry >= 15)
        {
        	DEBUG_MSG(-1,"--------------GetTunerCablePercent GetLockStatus error------------");
            goto OUT_CablePercent;
        }
        OS_TaskDelay(100);
        tunerState = DRV_Tuner_GetLockStatus(ucTunerId);
        retry++;
    } while (tunerState != eTun_Lock);


	PercentCls = env->GetObjectClass( PercentObj);
	if( NULL == PercentCls )
	{
		goto OUT_CablePercent;
	}
		
	
	
	iRet = DRV_Tuner_GetTunInfoStrengthPercent( ucTunerId, &uiPercent );
	if( eTUNER_NO_ERROR == iRet )
	{
		SetIntCtoJava( env, PercentObj, PercentCls, "Strength", uiPercent );
	}
	
	uiPercent = 0;
	
	iRet = DRV_Tuner_GetTunInfoQualityPercent( ucTunerId, &uiPercent );
	if( eTUNER_NO_ERROR == iRet )
	{
		SetIntCtoJava( env, PercentObj, PercentCls, "Quality", uiPercent );
	}

OUT_CablePercent:
	DeleteLocalRef( env, TunerCls );
	DeleteLocalRef( env, PercentCls );

}


