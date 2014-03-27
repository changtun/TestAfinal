/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : DVBPlayer ver 2.0
* File name   : dvbplayer.c
* Description : Implementation of DVBPlayer APIs.
*
* History :
*   2012-05-24 ZYJ
*       Initial Version.
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "pbitrace.h"

#include "dvbplayer.h"
#include "dvbplayer_proc.h"
#include "tmsgqueue.h"
#include "background.h"
#include "drv_avctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern DVBPlayer_Status_t   g_tStatus;

static S32 g_PvrRecFlag = 0;
static PVR_PLAYER_TYPE_E g_PvrPlayerType = PVR_PLYAER_DVB;
static U8 dvbplayer_flag =0;//0:��ǰ�ǲ���״̬��1:player stop DVBPlayer_Stop������ù���Ҫ�ǻ��˵����˵������½�����Ӳ�����Ϊ��ͬһƵ�㣬û����cat��emm����ղ����ˡ� 


S32 DVBPlayer_Set_Attr( DVBPlayer_Attr_e Attr, void * pParam )
{
    DRV_AVCtrl_ErrorCode_t error = AVCTRL_NO_ERROR;

    if( NULL == pParam )
    {
        pbierror("%s param is NULL!!!--%d--\n",__FUNCTION__,__LINE__);
      return  PV_FAILURE;
    }

    switch( Attr )
    {
    case eDVBPLAYER_ATTR_VID_WINDWOS:
    {
        DRV_AVCtrl_VidWindow_t tVidOut_Window; ;

        memcpy( &tVidOut_Window, pParam, sizeof(DRV_AVCtrl_VidWindow_t) );

        error = DRV_AVCtrl_Vid_SetIOWindow( tVidOut_Window );
        if( AVCTRL_NO_ERROR != error )
        {
            pbierror("DRV_AVCtrl_Vid_SetIOWindow error!!!--%d--\n",__LINE__);
            return PV_FAILURE;
        }

    }
    break;
    case eDVBPLAYER_ATTR_SWITCH_AUDIOPID:
    {
        DVBPlayer_SWITCH_AUDIOPID_t  *info=(DVBPlayer_SWITCH_AUDIOPID_t *)pParam;
        DVBPlayer_Event_t tEvent;
        U16   pid[2];
        pbierror("eDVBPLAYER_ATTR_SWITCH_AUDIOPID pid %d  type %d--\n",info->u16AudioPid,info->u16audiotype);
        pid[1] = g_tStatus.tProgStatus.u16AudioPid;
        if (0 != DVBPlayer_Stop_Audio())
        {
            printf("\n-----stop audio error!-----\n");
            return 1;
        }
        g_tStatus.tProgStatus.u8AudioFmt =(U8)info->u16audiotype;
        g_tStatus.tProgStatus.u16AudioPid =info->u16AudioPid;
        if (0 != DVBPlayer_Start_Audio())
        {
            pbierror("\n-----start audio error!-----\n");
            return 1;
        }
        pid[0] = g_tStatus.tProgStatus.u16AudioPid;
        tEvent.eEventType = 3;
        tEvent.pEventData = (void *)&pid;
        pbiinfo("\n player_________________ switch audip pid to cas descriptor ________________\n");
        CA_DVBPlay_CallBack( tEvent );
    }
    break;

	case eDVBPLAYER_ATTR_SET_VPARA:
    {
        U32 *isDvb = pParam;
        return DVBPlayer_Set_VPathPara( *isDvb );
    }
    break;		

    case eDVBPLAYER_ATTR_AUD_SETVOLUME:
    {
        U32 *Volume = pParam;
        error = DVBPlayer_Set_Video_Volume( *Volume );
    }
    break;

    case eDVBPLAYER_ATTR_AUD_MUTE:
    {
        U32 *isMute = pParam;
       error =  DVBPlayer_Video_Mute( *isMute );
    }
    break;

   case	eDVBPLAYER_ATTR_AUD_MODE:
   {
	U32 *mode=	pParam;
	error = DVBPlayer_SetAudioMode(*mode);
   }
   break;

    case eDVBPlayer_ATTR_VID_STOP_MODE:
    {
        U32 *StopMode = (U32 *)pParam;
      error =   DVBPlayer_Set_Video_StopMode(*StopMode);
    }
    break;

    case eDVBPLAYER_ATTR_PVR_GET_FILE_INFO:
    {
        PVR_FILE_ATTR_t *pPvrFileAttr = (PVR_FILE_ATTR_t*)pParam;
      error =   DVBPlayer_PVR_GetPlayFileInfo(pPvrFileAttr);
    }
    break;

    case eDVBPLAYER_ATTR_PVR_GET_STATUS:
    {
        PVR_PLAY_STATUS_t * pPvrPlayStatus = (PVR_PLAY_STATUS_t*)pParam;
     error =    DVBPlayer_PVR_GetPlayStatus(pPvrPlayStatus);
    }
    break;

    default:
    {
    }
    }

    return error;
}

S32 DVBPlayer_Get_Attr( DVBPlayer_Attr_e Attr, void * pParam )
{
    DRV_AVCtrl_ErrorCode_t error = AVCTRL_NO_ERROR;

    if( NULL == pParam )
    {
        pbierror("%s param is NULL!!!--%d--\n",__FUNCTION__,__LINE__);
        return PV_FAILURE;
    }

    switch( Attr )
    {
    case eDVBPLAYER_ATTR_VID_WINDWOS:
    {
        DRV_AVCtrl_VidWindow_t *pVidOut_Window = pParam;

        memset( pVidOut_Window, 0, sizeof(DRV_AVCtrl_VidWindow_t) );

        error = DRV_AVCtrl_Vid_GetIOWindow( pVidOut_Window  );
        if( AVCTRL_NO_ERROR != error )
        {
            pbierror("DRV_AVCtrl_Vid_GetIOWindow error!!!--%d--\n",__LINE__);
            return PV_FAILURE;
        }
    }
    break;
	    case eDVBPLAYER_ATTR_AUD_MODE:
		{
			DRV_AVCtrl_AudStereo_t *mode =pParam;
			
			error = DVBPlayer_GetAudioMode(mode);
			 if( AVCTRL_NO_ERROR != error )
			 {
				    pbierror("eDVBPLAYER_ATTR_AUD_MODE   error!!!--%d--\n",__LINE__);
	          		  return PV_FAILURE;
			 }

		}
		break;
		
    default:
    {
    }
    }
    return PV_SUCCESS;
}

S32 DVBPlayer_Play_Program( DVBPlayer_Play_Param_t * pParam )
{
    DVBPlayer_Action_Param_u    uMessageActionParam;
    U32 u32Freq = 0;
    U8 bState = 0;

    if( NULL == pParam )
    {
        return PV_NULL_PTR;
    }

    pbiinfo("\n------------- DVBPlayer_Play_Program -------------\n");
    pbiinfo("tuner param:\n");
    pbiinfo("  tuner id: %d.\n", pParam->uTunerParam.tCable.u8TunerId);
    pbiinfo("  freq: %d.\n", pParam->uTunerParam.tCable.u32Freq);
    pbiinfo("  srate: %d.\n", pParam->uTunerParam.tCable.u32SymbRate);
    pbiinfo("  QAM: %d.\n", pParam->uTunerParam.tCable.eMod);
	pbiinfo("  Bandwidth: %d.\n", pParam->uTunerParam.tCable.Bandwidth);
    pbiinfo("program param:\n");
    if (eDVBPLAYER_PROG_TYPE_SERVICE == pParam->uProgParam.eProgType)
    {
        pbiinfo("  play service.\n");
        pbiinfo("  lock flag: %d.\n", pParam->uProgParam.tServParam.eServLockFlag);
        pbiinfo("  service id: 0x%04X.\n", pParam->uProgParam.tServParam.u16ServId);
        pbiinfo("  PMT Pid: 0x%04X.\n", pParam->uProgParam.tServParam.u16PMTPid);
        pbiinfo("  TS Id: 0x%04X.\n", pParam->uProgParam.tServParam.u16TsId);
        pbiinfo("  Net Id: 0x%04X.\n", pParam->uProgParam.tServParam.u16NetId);
        pbiinfo("  LogicNum: 0x%04X.\n", pParam->uProgParam.tServParam.u16LogicNum);
        pbiinfo("  u16AudioPid: 0x%04X.\n", pParam->uProgParam.tServParam.u16historyAudioPid);
    }
    else
    {
        if ((pParam->uProgParam.tAVPids.u16VideoPid > MAX_PID)
                && (pParam->uProgParam.tAVPids.u16AudioPid > MAX_PID))
        {
            pbierror("invalid video and audio Pid. VPid = 0x%04X, APid = 0x%04X.\n",
                     pParam->uProgParam.tAVPids.u16VideoPid,
                     pParam->uProgParam.tAVPids.u16AudioPid);
            return PV_INVALID_PARAM;
        }
        pbiinfo("  playe AV pids.\n");
        pbiinfo("  pcr pid: 0x%04X.\n", pParam->uProgParam.tAVPids.u16PCRPid);
        pbiinfo("  video pid: 0x%04X.\n", pParam->uProgParam.tAVPids.u16VideoPid);
        pbiinfo("  audio pid: 0x%04X.\n", pParam->uProgParam.tAVPids.u16AudioPid);
        pbiinfo("  video fmt: %d.\n", pParam->uProgParam.tAVPids.u8VideoFmt);
        pbiinfo("  audio fmt: %d.\n", pParam->uProgParam.tAVPids.u8AudioFmt);
    }
    pbiinfo("============== end.\n");

    memset( &uMessageActionParam, 0, sizeof(DVBPlayer_Action_Param_u) );
    memcpy( &uMessageActionParam, pParam, sizeof(DVBPlayer_Play_Param_t) );
    
    /* modif by zxguan 2013-02-02 {{{*/
    u32Freq = GetTunerFreq();
    /* }}} modif by zxguan 2013-02-02*/
    /* modif by zshang 2013-03-05 {{{*/
    DvbPlayerMessageClean( eDVBPLAYER_ACTION_PLAY );
    /* }}} modif by zshang 2013-03-05*/
    DvbPlayerMessageSend( eDVBPLAYER_ACTION_PLAY, uMessageActionParam, NULL );

    if( u32Freq != pParam->uTunerParam.tCable.u32Freq )
    {
        pbiinfo("DVBPlayer_Play_Program----------------ca 000 ---%d   %d\n",u32Freq,pParam->uTunerParam.tCable.u32Freq);
        bState = 0;
    }
    else
    {
        bState = 1;
    }

    if( 0 == g_PvrRecFlag )
    {
        DVBPlayer_Event_t tEvent;
        memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
        if( 0 == CheckChannel(pParam->uProgParam.tServParam.u16PMTPid, \
                              pParam->uProgParam.tServParam.u16ServId) )
        {
            PostChanChange_t tCaPostChan;
            memset( &tCaPostChan, 0, sizeof(PostChanChange_t));
            tEvent.eEventType = 0;
            tCaPostChan.wChannelId = pParam->uProgParam.tServParam.u16LogicNum;
            tCaPostChan.bState = bState;
			
			if(dvbplayer_flag ==1)
			{
				tCaPostChan.bState =0;
				dvbplayer_flag =0;
			}
            tEvent.pEventData = &tCaPostChan;
            CA_DVBPlay_CallBack( tEvent );
            pbiinfo("DVBPlayer_Play_Program----------------ca  ---%d   %d\n",tCaPostChan.wChannelId,tCaPostChan.bState);
			if(tCaPostChan.bState == 0)
			{
				CatTableReset();
				pbiinfo("DVBPlayer_Play_Program-----cat reset^^^^^^^^^^^^^^^^^");
			}
        }
    }
    else
    {
        DVBPlayer_PVR_DVB_ReStart();
    }

    return PV_SUCCESS;
}

S32 DVBPlayer_Stop()
{
    DVBPlayer_Action_Param_u    uMessageActionParam;
    memset( &uMessageActionParam, 0, sizeof(DVBPlayer_Action_Param_u) );

    DvbPlayerMessageSend( eDVBPLAYER_ACTION_STOP, uMessageActionParam, NULL );

    if( 0 == g_PvrRecFlag )
    {
        DVBPlayer_Event_t tEvent;
        PostChanChange_t tCaPostChan;

        memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
        memset( &tCaPostChan, 0, sizeof(PostChanChange_t));
        tEvent.eEventType = 0;
        tCaPostChan.wChannelId =0xffff;
        tCaPostChan.bState = 0;
        tEvent.pEventData = &tCaPostChan;
        CA_DVBPlay_CallBack( tEvent );
			Reset_ServiceID();
		dvbplayer_flag =1;
        pbiinfo("DVBPlayer_Play_Program----------------ca  ---%d   %d\n",tCaPostChan.wChannelId,tCaPostChan.bState);
    }

    return PV_SUCCESS;
}


S32 DVBPlayer_REC_Play(DVBPlayer_PVR_REC_t *pRecInfo)
{
    DVBPlayer_Action_Param_u    uMessageActionParam;
    S32 sRet = 0;

    if( NULL == pRecInfo )
    {
        return PV_NULL_PTR;
    }

    pbiinfo("[%s %d]=g= FileName %s..\n",DEBUG_LOG,pRecInfo->FileName);

#if 1
    DVBPlayer_PVR_REC_t  tPvrRecParam;
    DVBPlayer_Status_t  tStatus;

    memset( &tStatus, 0, sizeof(DVBPlayer_Status_t) );
    memset( &tPvrRecParam, 0, sizeof(DVBPlayer_PVR_REC_t) );

    tPvrRecParam.u8RecFlag = pRecInfo->u8RecFlag;
    pbiinfo("[%s %d]=g= FileName %s..NameLen= %d..\n",DEBUG_LOG,pRecInfo->FileName, strlen(pRecInfo->FileName));
//        if( 0 == tPvrRecParam.u8RecFlag )
//  {
        tPvrRecParam.u16PmtPid = pRecInfo->u16PmtPid;
    tPvrRecParam.u16ServId = pRecInfo->u16ServId;
        memcpy( tPvrRecParam.FileName, pRecInfo->FileName, strlen(pRecInfo->FileName)+1 );
        tPvrRecParam.u64MaxFileLength = pRecInfo->u64MaxFileLength;

        /* TimeRec */
        GetPlayerStatus( &tStatus );
        tPvrRecParam.tTimeRec.u32VidFmt = tStatus.tProgStatus.u8VideoFmt;
        tPvrRecParam.tTimeRec.u32VidPid = tStatus.tProgStatus.u16VideoPid;
        tPvrRecParam.tTimeRec.u32AudPid = tStatus.tProgStatus.u16AudioPid;
        tPvrRecParam.tTimeRec.u32AudFmt = tStatus.tProgStatus.u8AudioFmt;

        if( 2 == pRecInfo->u8RecFlag )
        {
            DvbPlayerMessageSend( eDVBPLAYER_ACTION_REC_PLAY, uMessageActionParam, NULL );
        }

        /* BackRec */
        tPvrRecParam.u16LogicNum = pRecInfo->u16LogicNum;
        memcpy( &tPvrRecParam.tBackRec.uTunerDesc, &pRecInfo->tBackRec.uTunerDesc, sizeof(DVBCore_Tuner_Desc_u) );
        PBIDEBUG("RecStart\n");

    sRet = DVBPlayer_PVR_RecStart( &tPvrRecParam);
    if( PV_SUCCESS != sRet )
    {
        pbierror("[%s %d] Error = %d.\n",DEBUG_LOG,sRet);
        return PV_FAILURE;
    }
//  }
#endif
#if 0
    //else
    {
        memset( &uMessageActionParam, 0, sizeof(DVBPlayer_Action_Param_u) );
        uMessageActionParam.tPvrData.tPvrRec.u8RecFlag = pRecInfo->u8RecFlag;
        uMessageActionParam.tPvrData.tPvrRec.u16LogicNum = pRecInfo->u16LogicNum;
        uMessageActionParam.tPvrData.tPvrRec.u16PmtPid = pRecInfo->u16PmtPid;
        uMessageActionParam.tPvrData.tPvrRec.u64MaxFileLength = pRecInfo->u64MaxFileLength;

        memcpy( uMessageActionParam.tPvrData.tPvrRec.FileName, pRecInfo->FileName, strlen(pRecInfo->FileName)+1 );
        // uMessageActionParam.tPvrData.tPvrRec.FileName[strlen(pRecInfo->FileName)] = '\0';
        pbiinfo("[%s %d]=g= FileName %s..NameLen= %d..\n",DEBUG_LOG,uMessageActionParam.tPvrData.tPvrRec.FileName, strlen(uMessageActionParam.tPvrData.tPvrRec.FileName));
        DvbPlayerMessageSend( eDVBPLAYER_ACTION_REC_PLAY, uMessageActionParam, NULL );
    }
#endif
    g_PvrRecFlag = 1;
    return PV_SUCCESS;
}

S32 DVBPlayer_REC_Stop()
{
    DVBPlayer_Action_Param_u    uMessageActionParam;
    DVBPlayer_Status_t tStatus;
    U16 u16VidPid = 0x1FFF;
    U16 u16AudPid = 0x1FFF;
    U8 u8VidFmt = 0xFF;
    U8 u8AudFmt = 0xFF;
    PBIDEBUG("RecStop!!\n");

    memset( &tStatus, 0, sizeof(DVBPlayer_Status_t) );
    GetPlayerStatus( &tStatus );
    u16VidPid = tStatus.tProgStatus.u16VideoPid;
    u16AudPid = tStatus.tProgStatus.u16AudioPid;
    u8VidFmt = tStatus.tProgStatus.u8VideoFmt;
    u8AudFmt = tStatus.tProgStatus.u8AudioFmt;
    DVBPlayer_PVR_RecStop( u16VidPid, u8VidFmt, u16AudPid, u8AudFmt );
    DvbPlayerMessageSend( eDVBPLAYER_ACTION_REC_STOP, uMessageActionParam, NULL );
    g_PvrRecFlag = 0;
    return PV_SUCCESS;
}

S32 DVBPlayer_Get_Info( DVBPlayer_Info_t * pInfo )
{
	DVBPlayer_Status_t  tStatus;
	GetPlayerStatus( &tStatus );
	pInfo->u16AudioPid = tStatus.tProgStatus.u16AudioPid;
	pInfo->u16VideoPid = tStatus.tProgStatus.u16VideoPid;
	pInfo->u16PCRPid = tStatus.tProgStatus.u16PCRPid;

	pbiinfo("LinKang:: u16AudioPid = %d.\n", tStatus.tProgStatus.u16AudioPid );
	pbiinfo("LinKang:: u16VideoPid = %d.\n", tStatus.tProgStatus.u16VideoPid );
	pbiinfo("LinKang:: u16PCRPid = %d.\n", tStatus.tProgStatus.u16PCRPid );

    return 0;
}
S32 DVBPlayer_Get_RunningStatus()
{
    return g_tStatus.eRunningStatus;
}

S32 DVBPlayer_PVR_Play(DVBPlayer_PVR_Play_t *pPlayerInfo)
{
#if 0
    DVBPlayer_Action_Param_u    uMessageActionParam;
    memset( &uMessageActionParam, 0, sizeof(DVBPlayer_Action_Param_u) );

    memcpy( &uMessageActionParam.tPvrData.tPvrPlay, pPlayerInfo, sizeof(DVBPlayer_PVR_Play_t) );

    DvbPlayerMessageSend( eDVBPLAYER_ACTION_PVR_PLAY, uMessageActionParam, NULL );
#else
    DVBPlayer_Status_t  tStatus;
    U32 VidPid = 0xFFFFFFFF;
    U32 AudPid = 0xFFFFFFFF;
    U32 VidFmt = 0xFFFFFFFF;
    U32 AudFmt = 0xFFFFFFFF;
    U8 u8UseEnctypt = 1;

    //sprintf(pPlayerInfo->FileName,"/mnt/sda/sda1/pvr/aaa.ts");

    g_PvrPlayerType = pPlayerInfo->PvrPlayType;

    //pPlayerInfo->PvrPlayType = PVR_PLAYER_MenuPlay;
    switch( pPlayerInfo->PvrPlayType )
    {
        case PVR_PLYAER_DVB:
        {
            memset( &tStatus, 0, sizeof(DVBPlayer_Status_t) );
            GetPlayerStatus( &tStatus );
            VidPid = tStatus.tProgStatus.u16VideoPid;
            AudPid = tStatus.tProgStatus.u16AudioPid;
            VidFmt = tStatus.tProgStatus.u8VideoFmt;
            AudFmt = tStatus.tProgStatus.u8AudioFmt;
            u8UseEnctypt = 1;
        }
        break;
        case PVR_PLAYER_Play:
        {
            PVR_USER_DATA_T tUserData;
            U32 u32UserData = sizeof(PVR_USER_DATA_T);
            memset( &tUserData, 0, sizeof(PVR_USER_DATA_T) );

            DVBPlayer_PVR_GetFileUserData( pPlayerInfo->FileName, &tUserData, &u32UserData );

            pbiinfo("[%s %d] -----Get UserData--------------Datelen %d--\n",DEBUG_LOG, u32UserData);
            pbiinfo("Vid Pid 0x%x.\n", tUserData.u16VidPid );
            pbiinfo("Aud Num 0x%x.\n", tUserData.u8AudNum );
            pbiinfo("Aud Pid 0x%x.\n", tUserData.u16AudPid[0] );
            pbiinfo("Vid Fmt 0x%x.\n", tUserData.u8VidFmt );
            pbiinfo("Aud Fmt 0x%x.\n", tUserData.u8AudFmt[0] );
            pbiinfo("Name @%s@.\n",tUserData.pName);
            pbiinfo("[%s %d] -----Get UserData end----------------\n",DEBUG_LOG);

            VidPid = tUserData.u16VidPid;
            AudPid = tUserData.u16AudPid[0];
            VidFmt = tUserData.u8VidFmt;
            AudFmt = tUserData.u8AudFmt[0];
            u8UseEnctypt = 1;
        }
        break;
        case PVR_PLAYER_MenuPlay:
        {
		S32 ret = 0;
		ret = DRV_PVR_PlayerInit(pPlayerInfo->FileName);
		if(ret != 0) {
		pbierror("[%s %d] FileName [%s] ERROR[0x%x].\n", DEBUG_LOG, pPlayerInfo->FileName, ret );
		return ret;
		}
		ret = DRV_PVR_PlayerStart();
		if(ret != 0) {
		pbierror("[%s %d] ERROR[0x%x].\n", DEBUG_LOG, ret );
		return ret;
		}
		return DRV_PVR_PlayerPlay();
        }
        break;
    }
    
#if 0
    if( 0 == pPlayerInfo->PvrPlayType )
    {
        memset( &tStatus, 0, sizeof(DVBPlayer_Status_t) );
        GetPlayerStatus( &tStatus );
        VidPid = tStatus.tProgStatus.u16VideoPid;
        AudPid = tStatus.tProgStatus.u16AudioPid;
        VidFmt = tStatus.tProgStatus.u8VideoFmt;
        AudFmt = tStatus.tProgStatus.u8AudioFmt;
    }
    else
    {
        PVR_USER_DATA_T tUserData;
        U32 u32UserData = sizeof(PVR_USER_DATA_T);
        memset( &tUserData, 0, sizeof(PVR_USER_DATA_T) );

        DVBPlayer_PVR_GetFileUserData( pPlayerInfo->FileName, &tUserData, &u32UserData );

        pbiinfo("[%s %d] -----Get UserData--------------Datelen %d--\n",DEBUG_LOG, u32UserData);
        pbiinfo("Vid Pid 0x%x.\n", tUserData.u16VidPid );
        pbiinfo("Aud Num 0x%x.\n", tUserData.u8AudNum );
        pbiinfo("Aud Pid 0x%x.\n", tUserData.u16AudPid[0] );
        pbiinfo("Vid Fmt 0x%x.\n", tUserData.u8VidFmt );
        pbiinfo("Aud Fmt 0x%x.\n", tUserData.u8AudFmt[0] );
        pbiinfo("Name @%s@.\n",tUserData.pName);
        pbiinfo("[%s %d] -----Get UserData end----------------\n",DEBUG_LOG);

        VidPid = tUserData.u16VidPid;
        AudPid = tUserData.u16AudPid[0];
        VidFmt = tUserData.u8VidFmt;
        AudFmt = tUserData.u8AudFmt[0];
    }
#endif
#endif

    return DVBPlayer_PVR_PlayStart( VidPid, VidFmt, AudPid, AudFmt, pPlayerInfo->FileName, u8UseEnctypt );
}

S32 DVBPlayer_PVR_Stop()
{
#if 0
    DVBPlayer_Action_Param_u    uMessageActionParam;
    memset( &uMessageActionParam, 0, sizeof(DVBPlayer_Action_Param_u) );

    DvbPlayerMessageSend( eDVBPLAYER_ACTION_PVR_STOP, uMessageActionParam, NULL );
#else
    DVBPlayer_Status_t  tStatus;
    U32 VidPid = 0xFFFFFFFF;
    U32 AudPid = 0xFFFFFFFF;
    U32 VidFmt = 0xFFFFFFFF;
    U32 AudFmt = 0xFFFFFFFF;
	if( PVR_PLAYER_MenuPlay == g_PvrPlayerType )
	{
		S32 ret = 0;
        ret = DRV_PVR_PlayerStop();
		if(ret != 0) {
			pbierror("[%s %d] ERROR[0x%x].\n", DEBUG_LOG, ret );
			return ret;
		}			
		return DRV_PVR_PlayerUnInit();
    }

    if( PVR_PLYAER_DVB == g_PvrPlayerType )
    {
        memset( &tStatus, 0, sizeof(DVBPlayer_Status_t) );
        GetPlayerStatus( &tStatus );
        VidPid = tStatus.tProgStatus.u16VideoPid;
        AudPid = tStatus.tProgStatus.u16AudioPid;
        VidFmt = tStatus.tProgStatus.u8VideoFmt;
        AudFmt = tStatus.tProgStatus.u8AudioFmt;
    }
#endif
    return DVBPlayer_PVR_PlayStop( VidPid, VidFmt, AudPid, AudFmt );;
}

S32 DVBPlayer_PVR_Pause()
{
#if 0
    DVBPlayer_Action_Param_u    uMessageActionParam;
    memset( &uMessageActionParam, 0, sizeof(DVBPlayer_Action_Param_u) );

    DvbPlayerMessageSend( eDVBPLAYER_ACTION_PVR_PAUSE, uMessageActionParam, NULL );
#else
	if( PVR_PLAYER_MenuPlay == g_PvrPlayerType )
	{
		return DRV_PVR_PlayerPause();
    }

    DVBPlayer_PVR_PlayPause();
#endif
    return PV_SUCCESS;
}

S32 DVBPlayer_PVR_Resume()
{
#if 0
    DVBPlayer_Action_Param_u    uMessageActionParam;
    memset( &uMessageActionParam, 0, sizeof(DVBPlayer_Action_Param_u) );

    DvbPlayerMessageSend( eDVBPLAYER_ACTION_PVR_RESUME, uMessageActionParam, NULL );
#else
	if( PVR_PLAYER_MenuPlay == g_PvrPlayerType )
	{
		return DRV_PVR_PlayerResume();
    }

    DVBPlayer_PVR_PlayResume();
#endif
    return PV_SUCCESS;
}

S32 DVBPlayer_PVR_Speed(S32 Speed)
{
#if 0
    DVBPlayer_Action_Param_u    uMessageActionParam;
    memset( &uMessageActionParam, 0, sizeof(DVBPlayer_Action_Param_u) );

    uMessageActionParam.tPvrData.ePvrSpeed = Speed;
    DvbPlayerMessageSend( eDVBPLAYER_ACTION_PVR_SPEED, uMessageActionParam, NULL );
#else
	if( PVR_PLAYER_MenuPlay == g_PvrPlayerType )
	{
		//return DRV_PVR_PlayerResume();
		return DRV_PVR_PlayerSpeed((PVR_SPEED_E)Speed);
    }

    DVBPlayer_PVR_PlaySpeedCtrl((PVR_SPEED_E)Speed);
#endif
    return PV_SUCCESS;
}

S32 DVBPlayer_PVR_Seek(S32 pos)
{
#if 0
    DVBPlayer_Action_Param_u    uMessageActionParam;
    memset( &uMessageActionParam, 0, sizeof(DVBPlayer_Action_Param_u) );

    uMessageActionParam.tPvrData.ePvrSpeed = Speed;
    DvbPlayerMessageSend( eDVBPLAYER_ACTION_PVR_SPEED, uMessageActionParam, NULL );
#else
	if( PVR_PLAYER_MenuPlay == g_PvrPlayerType )
	{
		//return DRV_PVR_PlayerResume();
		//return DRV_PVR_PlayerSpeed((PVR_SPEED_E)Speed);
		
    }
else
{
    //DVBPlayer_PVR_PlaySeekCtrl(pos);
    }
#endif
    return PV_SUCCESS;
}

/*****************************************************************************
 Prototype    : CheckEPG
 Description  : EPG��Ϣ �ж� service id��TS id�� Network id
 Input        : U16 u16ServId
                U16 u16TsId
                U16 u16NetId
 Output       : None
 Return Value :
 Calls        :
 Called By    :

  History        :
  1.Date         : 2012/9/24
    Author       : zxguan
    Modification : Created function

*****************************************************************************/
S32 CheckEPG( U16 u16ServId, U16 u16TsId, U16 u16NetId )
{
    S32 Ret = 0;

    DVBPlayer_Status_t  tStatus;

    memset( &tStatus, 0, sizeof(DVBPlayer_Status_t) );

    Ret = GetPlayerStatus( &tStatus );
    if( 0 != Ret )
    {
        return 1;
    }

    if( tStatus.tProgStatus.u16ServiceId != u16ServId )
    {
        return 1;
    }

    if( tStatus.tProgStatus.u16TsId != u16TsId )
    {
        return 1;
    }

    if( tStatus.tProgStatus.u16NetId != u16NetId )
    {
        return 1;
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
