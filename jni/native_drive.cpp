#include <stdio.h>

#include <jni.h>
#include <drv_api.h>
#include <ca.h>
#include <dvbplayer.h>

#include "logit.h"
#include "native_drive.h"
#include "frontpanel.h"
#include "jnistaple.h"

#include "dal_common.h"
#include "dal.h"

static int g_DrvCAInitFlag = 0;
static int g_DrvApiInitFlag = 0;

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeDrive_pvwareDRVInit
  (JNIEnv *env, jobject obj)
{
	jint jRet = 0;

	if( 0 == g_DrvApiInitFlag )
	{
		_register_front_panel_Class( env );
		
		Front_Panel_Dispaly( env, -1 );
		
		jRet = Pvware_DRV_Init();
		if( 0 != jRet )
		{
			DEBUG_MSG( -1, T("Pvware Drv Init error!") );
		}

		DVBTable_Init();
		
		//DAL_Init();
		g_DrvApiInitFlag = 1;
	}
	else
	{
		DEBUG_MSG(-1, "pvwareDrvInit was Run!!!");
	}
	
	
	return jRet;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeDrive_CAInit
  (JNIEnv *env, jobject obj)
{
#if 0
	DVBPlayer_CA_Fun_t tCaFun;

	CAS_Init();
	DEBUG_MSG(-1, "CA Init!");
	
	tCaFun.pfServiceStart = CAS_Start_Service_CB;
	tCaFun.pfServiceStop = CAS_Stop_Service_CB;	
	
	DVBPlayer_CA_Function_Init(tCaFun);
#else
	if (0 == g_DrvCAInitFlag)
	{
		CA_Init();
		g_DrvCAInitFlag = 1;
	}
	else
	{
		DEBUG_MSG(-1, "CA was Run!!!");
	}
#endif
	
	return 0;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeDrive_DrvPlayerInit
  (JNIEnv *env, jobject obj)
{
	int iRet = 10;
	int iCount = 0;
	while( 10 == iRet )
	{
		iRet = DrvPlayerInit( g_DrvCAInitFlag );
		
		if( iCount > 50 )
		{
			return -1;
		}
		iCount++;
	}
	return 0;
}

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeDrive_DrvPlayerUnInit
  (JNIEnv *env, jobject obj)
{
	DrvPlayerUnInit();
	return 0;
}

/*
JNIEXPORT void JNICALL Java_com_pbi_dvb_dvbinterface_NativeDrive_DrvFPClear
  (JNIEnv *env, jobject obj)
{
	DEBUG_MSG(-1, "FrontPanelDisplay Clear!");
//	Front_Panel_Dispaly( env, -3 );
}
*/

JNIEXPORT jint JNICALL Java_com_pbi_dvb_dvbinterface_NativeDrive_DrvPmocSetWakeUp
  (JNIEnv *env, jobject obj, jobject WakeUpObj)
{
	jclass WakeCls = NULL;

	SYS_DRV_ErrorCode_t tErrRet = SYS_DRV_NO_ERROR;
	SYS_DRV_PMOC_WKUP_T tPmocWkup;

	memset( &tPmocWkup, 0, sizeof(SYS_DRV_PMOC_WKUP_T) );

	WakeCls = env->GetObjectClass(WakeUpObj);
	if( NULL == WakeCls )
	{
		DEBUG_MSG(-1, "PMOC SetWakeUp GetClass Error!");
		return -1;
	}

	tPmocWkup.u32WakeUpTime = GetIntJavaToC( env, WakeUpObj, WakeCls, "u32WakeUpTime");
	
	tErrRet = DrvPmSetWakeUp(tPmocWkup);
	if( SYS_DRV_NO_ERROR != tErrRet )
	{
		DEBUG_MSG(-1, "PMOC SetWakeUp DrvPmSetWakeUp Error!");
	}

	DeleteLocalRef( env, WakeCls );

	return tErrRet;
}