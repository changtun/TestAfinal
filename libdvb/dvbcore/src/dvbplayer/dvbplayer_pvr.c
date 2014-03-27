/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : dvbplayer_pvr.c
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/12/1
  Last Modified :
  Description   : dvbplayer pvr module
  Function List :
  History       :
  1.Date        : 2012/12/1
    Author      : zxguan
    Modification: Created file

******************************************************************************/

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
#include <string.h>
#include "osapi.h"
#include "pvddefs.h"
#include "pbitrace.h"
#include "dvbplayer.h"
#include "dvbplayer_pvr.h"
#include "drv_filter.h"
//#include "hi_unf_demux.h"
#include "drv_pvr.h"
#include "drv_avctrl.h"
#include "psi_si.h"
#include "drv_tuner.h"
#include "background.h"
/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/
static U32 *g_TsBuffer = 0xFFFFFFFF;
static PVR_INFO_t g_tPvrInfo[5];
static U32 g_TmsPlayFlag = 0;
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

U16 Get_pvr_sid()
{
	pbiinfo("%s -----%d ",__FUNCTION__,g_tPvrInfo[DEMUX_2].serviceid);
	return g_tPvrInfo[DEMUX_2].serviceid;
}
void  send_pvr_pmt_to_cas(void)
{
		DVBPlayer_Event_t tEvent;
		tEvent.eEventType = 2;

#if 1
		int iii =0;
		g_tPvrInfo[DEMUX_2].pmtInfo[1]=2;
		for(iii=0;iii<50;iii++)
		{
			pbiinfo("@@ %02x  ",g_tPvrInfo[DEMUX_2].pmtInfo[iii]);
		}
#endif
		tEvent.pEventData = 	g_tPvrInfo[DEMUX_2].pmtInfo;
		CA_DVBPlay_CallBack( tEvent );
}

U32 Check_pvr_sid_pmtpid(U16 sid)
{
	
	if( g_tPvrInfo[DEMUX_2].serviceid ==sid)
	{
		pbiinfo("%s %d =====sid%x",__FUNCTION__,__LINE__, g_tPvrInfo[DEMUX_2].serviceid );
		return 1;
	}
	else
	{
		pbiinfo("%s %d =====sid%x",__FUNCTION__,__LINE__, g_tPvrInfo[DEMUX_2].serviceid );
		return 0;
	}

}

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
S32 _Start_Avdio(U32 AudPid, U32 AudFmt)
{
    S32 ErrorCode = PV_SUCCESS;

    switch(AudFmt)
    {
        case STREAM_TYPE_MPEG1_AUDIO:
        {
            AudFmt = AVCTRL_AUD_FORMAT_MPEG;
        }
        break;

        case STREAM_TYPE_MPEG2_AUDIO:
        {
            AudFmt = AVCTRL_AUD_FORMAT_MPEG;
        }
        break;

        case STREAM_TYPE_MPEG4_AAC:
        {
            AudFmt = AVCTRL_AUD_FORMAT_AAC_RAW;
        }
        break;
        case STREAM_TYPE_MPEG_AAC:
        {
            AudFmt = AVCTRL_AUD_FORMAT_AAC;
        }
        break;

        case STREAM_TYPE_AC3:
        {
            AudFmt = AVCTRL_AUD_FORMAT_AC3;
        }
        break;
        case STREASM_TYPE_EAC3:
        {
            AudFmt = AVCTRL_AUD_FORMAT_EAC3;
        }
        break;

        default:
        {
            AudFmt = AVCTRL_AUD_FORMAT_MPEG;
        }
        break;
    }

    ErrorCode = DRV_AVCtrl_Aud_SetPID(AudPid);
    ErrorCode |= DRV_AVCtrl_Aud_Start(AudFmt);
	ErrorCode |= DRV_AVCtrl_StartTs();
    pbiinfo("DVBPlayer_Start_Audio----------------ret%x\n",ErrorCode);

    if( PV_SUCCESS == ErrorCode )
    {
        return PV_SUCCESS;
    }

    return PV_FAILURE;
}

S32 _Start_Video(U32 VidPid, U32 VidFmt)
{
    S32 ErrorCode = PV_SUCCESS;

    if((VidPid > 0 ) && (VidPid < 0x1fff ))
    {
        switch(VidFmt)
        {
            case STREAM_TYPE_MPEG1_VIDEO:
            case STREAM_TYPE_MPEG2_VIDEO:
            {
                VidFmt = AVCTRL_VID_FORMAT_MPEG2;
            }
            break;

            case STREAM_TYPE_MPEG4P2:
            {
                VidFmt = AVCTRL_VID_FORMAT_MPEG4;
            }
            break;

            case STREAM_TYPE_H264:
            {
                VidFmt = AVCTRL_VID_FORMAT_H264;
            }
            break;

            default:
            {
                VidFmt = AVCTRL_VID_FORMAT_MPEG2;
            }
            break;
        }

        ErrorCode |= DRV_AVCtrl_Vid_SetPID(VidPid);
        ErrorCode |= DRV_AVCtrl_Vid_Start(VidFmt);
		ErrorCode |= DRV_AVCtrl_StartTs();
        if( PV_SUCCESS == ErrorCode )
        {
            return PV_SUCCESS;
        }
    }

    return PV_FAILURE;
}

PVR_VCODEC_TYPE_E  _ChangeVideoFmt(U32 Fmt)
{
    PVR_VCODEC_TYPE_E RetFmt = PVR_VCODEC_TYPE_BUTT;
    switch( Fmt )
    {
        case STREAM_TYPE_MPEG1_VIDEO:
        case STREAM_TYPE_MPEG2_VIDEO:
        {
            RetFmt = PVR_VCODEC_TYPE_MPEG2;
        }
        break;
        case STREAM_TYPE_MPEG4P2:
        {
            RetFmt = PVR_VCODEC_TYPE_MPEG4;
        }
        break;
        case STREAM_TYPE_H264:
        {
            RetFmt = PVR_VCODEC_TYPE_H264;
        }
        break;
        case 0xFFFF:
        {
            RetFmt = PVR_VCODEC_TYPE_BUTT;
        }
        break;
        default:
        {
            RetFmt = PVR_VCODEC_TYPE_MPEG2;
        }
        break;
    }
    pbiinfo("[%s %d]==IN Fmt 0x%x===Video Fmt 0x%x===\n",DEBUG_LOG,Fmt,RetFmt);
    return RetFmt;
}


S32 _GetPvrRecStatus()
{
    S32 RetErr = 0;
    PVR_REC_STATUS_t tRecStatus;

    memset( &tRecStatus, 0, sizeof(PVR_REC_STATUS_t) );
    RetErr = DRV_PVR_RecGetStatus( &tRecStatus);
    if( 0 != RetErr )
    {
        return -1;
    }
	pbiinfo("[%s %d]==tRecStatus.enState[%d]===\n",DEBUG_LOG, tRecStatus.enState);
    switch ( tRecStatus.enState)
    {
        case PVR_REC_STATE_RUNNING:
        case PVR_REC_STATE_PAUSE:
        {
            return 1;
        }
        break;
        default:
        {}
    }

    return 0;
}

S32 _GetPvrPlayStatus()
{
    S32 RetErr = 0;
    PVR_PLAY_STATUS_t tPvrPlayStatus;

    memset( &tPvrPlayStatus, 0, sizeof(PVR_PLAY_STATUS_t) );

    RetErr = DRV_PVR_PlayGetStatus( &tPvrPlayStatus );
    if( 0 != RetErr )
    {
        return -1;
    }
	pbiinfo("[%s %d]==tPvrPlayStatus.enState[%d]===\n",DEBUG_LOG, tPvrPlayStatus.enState);
    switch( tPvrPlayStatus.enState )
    {
        case PVR_PLAY_STATE_PLAY:
        case PVR_PLAY_STATE_PAUSE:
        case PVR_PLAY_STATE_FF:
        case PVR_PLAY_STATE_FB:
        case PVR_PLAY_STATE_SF:
        case PVR_PLAY_STATE_STEPF:
        case PVR_PLAY_STATE_STEPB:
        {
            return 1;
        }
        break;
        default:
        {}
    }

    return 0;
}

S32 PvrFilter_PmtCallback( U8 *pData, U16 Len, void * pParam )
{
    PSISI_Table_t *  pTable = NULL;
    U32 *pValue = (U32 *)pParam;
    S32  ret =0;
    if( NULL == pData || NULL == pParam )
    {
        PBIDEBUG("PvrFilter_PmtCallback Param is NULL\n");
        return -1;
    }

    pTable = PSISI_Alloc_Table(PMT_TABLE_ID, pData, Len);
    if (NULL == pTable)
    {
        PBIDEBUG("Alloc Table is Null !\n");
        return -1;
    }

    *pValue = pTable;
	 pbiinfo("@@@@@@@@@@@---%d ",__LINE__);	
    ret = PSISI_Parse_PMT_Section((PSISI_Table_t * )pTable, pData, Len);
	if(ret != 0) {
		PSISI_Free_Table(pTable);
	}
     if(ret ==0)
    {
    	
       DVBPlayer_Event_t tEvent;
        memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
        tEvent.eEventType = 8;
        tEvent.pEventData = pData;
        CA_DVBPlay_CallBack( tEvent );
        pbiinfo("PvrFilter_PmtCallback-----------send pmt to cas");
    }
	pbiinfo("PvrFilter_PmtCallback Param is ret %d\n",ret);
    return ret;
}

S32 PvrFilter_PatCallback( U8 *pData, U16 Len, void *pParam )
{
    PSISI_Table_t *  pTable = NULL;
    U32 *pValue = (U32 *)pParam;
    S32  ret =0;
    if( NULL == pData || NULL == pParam )
    {
        PBIDEBUG("PvrFilter_PmtCallback Param is NULL\n");
        return -1;
    }

    pTable = PSISI_Alloc_Table(PAT_TABLE_ID, pData, Len);
    if (NULL == pTable)
    {
        PBIDEBUG("Alloc Table is Null !\n");
        return -1;
    }

    *pValue = pTable;
    ret = PSISI_Parse_PAT_Section((PSISI_Table_t * )pTable, pData, Len);
	if(ret != 0) {
		PSISI_Free_Table(pTable);
	}
    return ret;
}

S32 DVBPlayer_PVR_Init()
{
    SFILTER_PVR_Init();

    DRV_PVR_Init();

	 memset( &g_tPvrInfo[DEMUX_2], 0xFF, sizeof(PVR_INFO_t) );
	//g_tPvrInfo[DEMUX_2].pmtInfo =NULL;

    return PV_SUCCESS;
}
S32 DVBPlayer_PVR_UnInit()
{
    S32 ErrRet = PV_SUCCESS;
    ErrRet = DRV_PVR_UnInit();
    return ErrRet;
}

S32 DVBPlayer_PVR_DVB_ReStart()
{

    DVBPlayer_Event_t tEvent;
    U32 data[2]= {0,0};

    memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
    data[0] = g_tPvrInfo[DEMUX_2].VideoPid;
    data[1] = g_tPvrInfo[DEMUX_2].AudioPid;
    tEvent.eEventType = 6;
    tEvent.pEventData = &data;
    CA_DVBPlay_CallBack( tEvent );
    pbiinfo("rec stop============DVBPlayer_PVR_RecStart----attach play ch-----------\n");

    return 0;
}

/*CA加扰节目 打开PAT， 清流不能打开PAT*/
U8 DVBPlayer_PVR_CheakCaMode(DVBPlayer_PVR_REC_t *pPvrParam)
{
	C8 *FileName = pPvrParam->FileName;
	C8 *pTemp = NULL;
	if(FileName && (NULL != (pTemp = strstr(FileName, "/pvr/[$"))))
		return 0;
	return 1;
}
#if 0
/*  返回值说明	0  正确，-1错误，1 表示其它录制在运行中*/
S32 DVBPlayer_PVR_RecStartExt(DVBPlayer_PVR_REC_t *pPvrParam)
{
    DRV_PVR_REC_ATTR_t stRecAttr;
    TUNER_ErrorCode_t rTunerError = 0;
    ts_src_info_t tTunerInfo;
    PSISI_Table_t *pPmtTable = NULL;
    PSISI_Table_t *pPatTable = NULL;
    PMT_Section_t *pPmtSection = NULL;
    PAT_Section_t *pPatSection = NULL;
    U32 u32PmtChannelHandle = 0xFFFFFFFF;
    U32 u32PatChannelHandle = 0xFFFFFFFF;
    U32 u32StreamType = 0xFF;
    U32 u32TableAddr = 0;
    U32 u32PmtAddr = 0;
    U32 u32PatAddr = 0;
    U32 ii = 0;
    U32 u32Ret = 0;
    S32 s32RecStatus = 0;
    PVR_USER_DATA_T tUserData;
    U8 is_trcas_encrypt =0,ecm_count =0;	//1: 是腾锐ca加扰
    U16  ecm_pid[8] ={0,0,0,0,0,0,0,0};
    U8 *temp_pmtdata=NULL;	
   
    s32RecStatus = _GetPvrRecStatus();
    if( -1 == s32RecStatus )
    {
        return PV_FAILURE;
    }
    else if( 1 == s32RecStatus )
    {
        PBIDEBUG("[INFO] DVBPlayer_PVR_RecStart is Run!!!");
        return 1;
    }
    
    SFILTER_PVR_UnInit();
    SFILTER_PVR_InitExt();
    
    if(DVBPlayer_PVR_CheakCaMode(pPvrParam))
    {
    	PBIDEBUG("[INFO] SYS_BackGround_TablePause SYS_BG_PAT!!!");
	SYS_BackGround_TablePause(SYS_BG_PAT);
	}
     //if( g_tPvrInfo[DEMUX_2].pmtInfo)
	 //	PSISI_Free_Table( g_tPvrInfo[DEMUX_2].pmtInfo);
    memset( &g_tPvrInfo[DEMUX_2], 0xFF, sizeof(PVR_INFO_t) );
    memset(g_tPvrInfo[DEMUX_2].pmtInfo,0,1024);
    memset( &stRecAttr, 0, sizeof(DRV_PVR_REC_ATTR_t) );
    memset( &tTunerInfo, 0, sizeof(ts_src_info_t) );
    memset( &tUserData, 0, sizeof(PVR_USER_DATA_T) );
	
 //   pPvrParam->u8RecFlag =0;

   // pPvrParam->u16PmtPid =34;//for test
   // pPvrParam->u16ServId =6;//for test

    g_tPvrInfo[DEMUX_2].serviceid =pPvrParam->u16ServId;
   g_tPvrInfo[DEMUX_2].pmtpid =pPvrParam->u16PmtPid ;
	
	
	
 //   if( pPvrParam->u8RecFlag==0)//
    {

        DVBPlayer_Event_t tEvent;
        memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
        if(1)
        {
            PostChanChange_t tCaPostChan;
            memset( &tCaPostChan, 0, sizeof(PostChanChange_t));
            tEvent.eEventType = eDVBPLAYER_EVENT_CA_PVR_REC_START;
            tCaPostChan.wChannelId = pPvrParam->u16LogicNum;
            tCaPostChan.bState = 0;//
            tEvent.pEventData = &tCaPostChan;
            CA_DVBPlay_CallBack( tEvent );
            pbiinfo("DVBPlayer_PVR_RecStart  9900----------------ca  ---%d   %d\n",tCaPostChan.wChannelId,tCaPostChan.bState);
        }

    }

    g_tPvrInfo[DEMUX_2].AudioPid = 0x1FFF;
    g_tPvrInfo[DEMUX_2].VideoPid = 0x1FFF;
    tUserData.u16VidPid = 0x1FFF;
    tUserData.u16AudPid[0] = 0x1FFF;
	tUserData.u16PcrPid = 0x1FFF;

 //   if( 0 == pPvrParam->u8RecFlag ) /* BackRec */
    {
        /* luck Tuner */
        PBIDEBUG("BackRec PVR Lock Tuner\n");
        if( eDVBCORE_SIGNAL_SOURCE_DVBC == pPvrParam->tBackRec.uTunerDesc.eSignalSource )
        {
            int retry = 0;
            TUNER_LOCKSTATUS_t tunerState = eTun_unLock;

            tTunerInfo.MediumType = MEDIUM_TYPE_DVBC;
            tTunerInfo.u.Cable.FrequencyKHertz = pPvrParam->tBackRec.uTunerDesc.tCable.u32Freq;
            tTunerInfo.u.Cable.SymbolRateSPS = pPvrParam->tBackRec.uTunerDesc.tCable.u32SymbRate;
            tTunerInfo.u.Cable.Modulation = pPvrParam->tBackRec.uTunerDesc.tCable.eMod + 5;
			tTunerInfo.u.Cable.Bandwidth = pPvrParam->tBackRec.uTunerDesc.tCable.eBandWidth;
		
            //rTunerError = DRV_Tuner_SetFrequency(0, &tTunerInfo, 0 );
			rTunerError = DRV_Tuner_SetFrequency(0, &tTunerInfo, 0 );
            do
            { 
                if( retry >= 30)
                {
                    PBIDEBUG("lock tuner failed.\n");
                    return -1;   /* NO LOCK*/
                }
                OS_TaskDelay(100);
                //tunerState = DRV_Tuner_GetLockStatus(0);
				tunerState = DRV_Tuner_GetLockStatus(2);
                retry++;
            }
            while (tunerState != eTun_Lock);
        }
        else
        {

        }
        PBIDEBUG("PVR Search Table.\n");
        /* search Pat */
        SFILTER_PVR_AddPid( DEMUX_2, PAT_TABLE_ID, 0, &u32PatChannelHandle);
        SFILTER_PVR_AddFilter( DEMUX_2, u32PatChannelHandle, 5000, PAT_TABLE_ID, \
                              0,0, PvrFilter_PatCallback, (void *)&u32PatAddr);
        SFILTER_PVR_DelPid( DEMUX_2, &u32PatChannelHandle); /* 关闭后 breakground PAT 停止 */
        
        pPatTable = (PSISI_Table_t *)u32PatAddr;
        if( NULL != pPatTable )
        {
            pPatSection = (PAT_Section_t *)(pPatTable->ppSection[0]);
            if( NULL == pPatSection)
            {
                if( NULL != pPatTable )
                {
                    psisi_section_free( pPatTable );
                }
            }
            else
            {
                for( ii = 0; ii< pPatSection->u16ProgNum; ii++ )
                {
                    if( pPatSection->ptProgram[ii].u16ProgramNumber == pPvrParam->u16ServId )
                    {
                        if( pPatSection->ptProgram[ii].u16PMTPid != pPvrParam->u16PmtPid )
                        {
                            pbiinfo("[%s %d] Pvr ServId Changed. 0x%x V 0x%x.", \
                                DEBUG_LOG, pPvrParam->u16PmtPid ,pPatSection->ptProgram[ii].u16PMTPid);
                            pPvrParam->u16PmtPid = pPatSection->ptProgram[ii].u16PMTPid; 
                        }
                    }
                }

                if( NULL != pPatTable )
                {
                    PSISI_Free_Table( pPatTable );
                }
                
            }
        }
		else
		{
			PBIDEBUG("-----------pat is null------------\n");
            return -1;
		}
	    g_tPvrInfo[DEMUX_2].serviceid =pPvrParam->u16ServId;	
          g_tPvrInfo[DEMUX_2].pmtpid  =pPvrParam->u16PmtPid;
        /* search Pmt */
        SFILTER_PVR_AddPid( DEMUX_2, PMT_TABLE_ID, 0, &u32PmtChannelHandle);
        SFILTER_PVR_AddFilter( DEMUX_2, u32PmtChannelHandle, 5000, PMT_TABLE_ID, \
                               pPvrParam->u16ServId ,pPvrParam->u16PmtPid, PvrFilter_PmtCallback, (void *)&u32PmtAddr);
        SFILTER_PVR_DelPid( DEMUX_2, &u32PmtChannelHandle);

        pPmtTable = (PSISI_Table_t *)u32PmtAddr;
        if( NULL == pPmtTable )
        {
            PBIDEBUG("-----------2222-3333------------\n");
            return -1;
        }

        pPmtSection = (PMT_Section_t *)(pPmtTable->ppSection[0]);
        if( NULL == pPmtSection )
        {
            PBIDEBUG("PVR Search Pmt PmtSection is NULL\n");
            if( NULL != pPmtTable )
            {
                psisi_section_free( pPmtTable );
            }
            return -1;
        }
		if(pPvrParam->tTimeRec.u32PCRPid <= 0x1FFF){
			g_tPvrInfo[DEMUX_2].PCRPid = pPmtSection->u16PCRPid;
			SFILTER_PVR_AddPid( DEMUX_2, g_tPvrInfo[DEMUX_2].PCRPid, 0, &g_tPvrInfo[DEMUX_2].pcrchannelhandle);
		}
        pbiinfo("table id 0x%x.\n",pPmtSection->u8TableId);
        pbiinfo("Pmt pid 0x%x.\n",pPmtSection->u16ProgramNum);
		pbiinfo("PCR pid 0x%x.\n",pPmtSection->u16PCRPid);

        PBIDEBUG("PVR Open Channel\n");
	  
        /* open channel */
        if( NULL != pPmtSection->ptVideo )
        {
            pbiinfo("set Video \n");
            stRecAttr.enIndexType = 1;
            stRecAttr.enIndexVidType = _ChangeVideoFmt(pPmtSection->ptVideo[0].u8StreamType);
            stRecAttr.u32IndexPid = pPmtSection->ptVideo[0].u16ElementaryPid;
            pbiinfo("videoPid 0x%x  Fmt 0x%x.\n",pPmtSection->ptVideo[0].u16ElementaryPid, stRecAttr.enIndexVidType);
            SFILTER_PVR_AddPid( DEMUX_2, pPmtSection->ptVideo[0].u16ElementaryPid, 0, &g_tPvrInfo[DEMUX_2].videochannelhandle);
            g_tPvrInfo[DEMUX_2].VideoPid = pPmtSection->ptVideo[0].u16ElementaryPid;
            tUserData.u8VidFmt = pPmtSection->ptVideo[0].u8StreamType;
			
        }

        g_tPvrInfo[DEMUX_2].AudioChnNum = 0;
        if( NULL != pPmtSection->ptAudio )
        {
            if( (NULL == pPmtSection->ptVideo) && ( 0 < pPmtSection->u16AudioNum ) )
            {
                stRecAttr.enIndexType = 2;
                stRecAttr.enIndexVidType = _ChangeVideoFmt(0xFFFF);
                stRecAttr.u32IndexPid = pPmtSection->ptAudio[0].u16ElementaryPid;
            }
            pbiinfo("set Audio \n");
            g_tPvrInfo[DEMUX_2].AudioChnNum = pPmtSection->u16AudioNum;
            for( ii = 0; ii < pPmtSection->u16AudioNum; ii++ )
            {
                pbiinfo("audioPid 0x%x .\n",pPmtSection->ptAudio[ii].u16ElementaryPid);
                SFILTER_PVR_AddPid( DEMUX_2, pPmtSection->ptAudio[ii].u16ElementaryPid, 0, &g_tPvrInfo[DEMUX_2].audiochannelhandle[ii]);
                tUserData.u16AudPid[ii] = pPmtSection->ptAudio[ii].u16ElementaryPid;
                tUserData.u8AudFmt[ii] = pPmtSection->ptAudio[ii].u8StreamType;
                g_tPvrInfo[DEMUX_2].AudioChnNum++;
            }
            g_tPvrInfo[DEMUX_2].AudioPid = pPmtSection->ptAudio[0].u16ElementaryPid;
		

			
#if 0 /* CA */
            DVBPlayer_Event_t tEvent;
            U32		data[2]= {0,0};

            memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
            tEvent.eEventType = 4;
            data[0] = pPmtSection->ptAudio[0].u16ElementaryPid;
            data[1] = g_tPvrInfo[DEMUX_2].audiochannelhandle[0];
            tEvent.pEventData = &data;
            CA_DVBPlay_CallBack( tEvent );
            pbiinfo("DVBPlayer_PVR_RecStart----audio------------ca  ---%d   %d\n",data[0],data[1]);
#endif
        }
    }
    
    if(2 == pPvrParam->u8RecFlag)
    stRecAttr.u32DemuxID = DEMUX_0;
	else
    stRecAttr.u32DemuxID = DEMUX_2;
    stRecAttr.u64MaxFileSize = pPvrParam->u64MaxFileLength;
    stRecAttr.u32FileNameLen = strlen(pPvrParam->FileName);

    memcpy( stRecAttr.szFileName, pPvrParam->FileName, stRecAttr.u32FileNameLen+1 );
    stRecAttr.szFileName[stRecAttr.u32FileNameLen] = '\0';

    pbiinfo("FileLen = %d..\n",stRecAttr.u32FileNameLen);
    pbiinfo("MaxLen = 0x%x.\n",stRecAttr.u64MaxFileSize);

    PBIDEBUG("PVR DRV_PVR_RecInit");
    u32Ret = DRV_PVR_RecInit( &stRecAttr, pPvrParam->u16LogicNum );
    if( 0 != u32Ret )
    {
        PBIDEBUG("[ERROR] DRV_PVR_RecInit");
        if( NULL != pPmtTable )
        {
            PSISI_Free_Table( pPmtTable );
        }
        return PV_FAILURE;
    }
    
	tUserData.u16PcrPid = g_tPvrInfo[DEMUX_2].PCRPid;
    tUserData.u16VidPid = g_tPvrInfo[DEMUX_2].VideoPid;
    tUserData.u8AudNum = g_tPvrInfo[DEMUX_2].AudioChnNum;
    memcpy( tUserData.pName, stRecAttr.szFileName, stRecAttr.u32FileNameLen );
    tUserData.pName[stRecAttr.u32FileNameLen] = '\0';


    pbiinfo("[%s %d] -----Set UserData----------------\n",DEBUG_LOG);
    pbiinfo("Vid Pid 0x%x.\n", tUserData.u16VidPid );
    pbiinfo("Aud Num 0x%x.\n", tUserData.u8AudNum );
    pbiinfo("Aud Pid 0x%x.\n", tUserData.u16AudPid[0] );
    pbiinfo("Vid Fmt 0x%x.\n", tUserData.u8VidFmt );
    pbiinfo("Aud Fmt 0x%x.\n", tUserData.u8AudFmt[0] );
	pbiinfo("Pcr Pid 0x%x.\n", tUserData.u16PcrPid );
    pbiinfo("Name =%s.\n",tUserData.pName);
    pbiinfo("[%s %d] -----Set UserData end----------------\n",DEBUG_LOG);

    DRV_PVR_SetUserData( stRecAttr.szFileName, &tUserData, sizeof(PVR_USER_DATA_T) );

	u32Ret = DRV_PVR_RecStart();
    if( 0 != u32Ret )
    {
        PBIDEBUG("[ERROR] DRV_PVR_RecStart");
        if( NULL != pPmtTable )
        {
            PSISI_Free_Table( pPmtTable );
        }
        return PV_FAILURE;
    }

/*    if( NULL != pPmtTable )
    {
        PSISI_Free_Table( pPmtTable );
    }*/ //liqian 20131108

	DRV_PVR_Start_Phone_Sending();
    PBIDEBUG("DVBPlayer_PVR_RecStart OK!!!");
    
    return PV_SUCCESS;
}
#endif

S32 DVBPlayer_PVR_RecStart(DVBPlayer_PVR_REC_t *pPvrParam)
{
    DRV_PVR_REC_ATTR_t stRecAttr;
    TUNER_ErrorCode_t rTunerError = 0;
    ts_src_info_t tTunerInfo;
    PSISI_Table_t *pPmtTable = NULL;
    PSISI_Table_t *pPatTable = NULL;
    PMT_Section_t *pPmtSection = NULL;
    PAT_Section_t *pPatSection = NULL;
    U32 u32PmtChannelHandle = 0xFFFFFFFF;
    U32 u32PatChannelHandle = 0xFFFFFFFF;
    U32 u32StreamType = 0xFF;
    U32 u32TableAddr = 0;
    U32 u32PmtAddr = 0;
    U32 u32PatAddr = 0;
    U32 ii = 0;
    U32 u32Ret = 0;
    S32 s32RecStatus = 0;
    PVR_USER_DATA_T tUserData;
    U8 is_trcas_encrypt =0,ecm_count =0;	//1: 是腾锐ca加扰
    U16  ecm_pid[8] ={0,0,0,0,0,0,0,0};
    U8 *temp_pmtdata=NULL;	
   
    s32RecStatus = _GetPvrRecStatus();
    if( -1 == s32RecStatus )
    {
        return PV_FAILURE;
    }
    else if( 1 == s32RecStatus )
    {
        PBIDEBUG("[INFO] DVBPlayer_PVR_RecStart is Run!!!");
        return PV_SUCCESS;
    }
     //SFILTER_PVR_UnInit();
    //SFILTER_PVR_Init();
    if(DVBPlayer_PVR_CheakCaMode(pPvrParam))
    {
    	PBIDEBUG("[INFO] SYS_BackGround_TablePause SYS_BG_PAT!!!");
		//SYS_BackGround_TablePause(SYS_BG_PAT);
	}
   //  if( g_tPvrInfo[DEMUX_2].pmtInfo)
	// 	PSISI_Free_Table( g_tPvrInfo[DEMUX_2].pmtInfo);
    memset( &g_tPvrInfo[DEMUX_2], 0xFF, sizeof(PVR_INFO_t) );
  //  g_tPvrInfo[DEMUX_2].pmtInfo	=NULL;
    memset( g_tPvrInfo[DEMUX_2].pmtInfo,0,1024);
    memset( &stRecAttr, 0, sizeof(DRV_PVR_REC_ATTR_t) );
    memset( &tTunerInfo, 0, sizeof(ts_src_info_t) );
    memset( &tUserData, 0, sizeof(PVR_USER_DATA_T) );
	
    g_tPvrInfo[DEMUX_2].serviceid =pPvrParam->u16ServId;
   g_tPvrInfo[DEMUX_2].pmtpid =pPvrParam->u16PmtPid ;
	
	

    g_tPvrInfo[DEMUX_2].AudioPid = 0x1FFF;
    g_tPvrInfo[DEMUX_2].VideoPid = 0x1FFF;
    tUserData.u16VidPid = 0x1FFF;
    tUserData.u16AudPid[0] = 0x1FFF;
	tUserData.u16PcrPid = 0x1FFF;

    if( 0 == pPvrParam->u8RecFlag ) /* BackRec */
    {
        /* luck Tuner */
        PBIDEBUG("BackRec PVR Lock Tuner\n");
        if( eDVBCORE_SIGNAL_SOURCE_DVBC == pPvrParam->tBackRec.uTunerDesc.eSignalSource )
        {
            int retry = 0;
            TUNER_LOCKSTATUS_t tunerState = eTun_unLock;

            tTunerInfo.MediumType = MEDIUM_TYPE_DVBC;
            tTunerInfo.u.Cable.FrequencyKHertz = pPvrParam->tBackRec.uTunerDesc.tCable.u32Freq;
            tTunerInfo.u.Cable.SymbolRateSPS = pPvrParam->tBackRec.uTunerDesc.tCable.u32SymbRate;
            tTunerInfo.u.Cable.Modulation = pPvrParam->tBackRec.uTunerDesc.tCable.eMod + 5;
		tTunerInfo.u.Cable.Bandwidth = GetTunerBandWidth();
		
            //rTunerError = DRV_Tuner_SetFrequency(0, &tTunerInfo, 0 );
			rTunerError = DRV_Tuner_SetFrequency(0, &tTunerInfo, 0 );
            do
            { 
                if( retry >= 30)
                {
                    PBIDEBUG("lock tuner failed.\n");
                    return -1;   /* NO LOCK*/
                }
                OS_TaskDelay(100);
                //tunerState = DRV_Tuner_GetLockStatus(0);
				tunerState = DRV_Tuner_GetLockStatus(0);
                retry++;
            }
            while (tunerState != eTun_Lock);
        }
        else
        {

        }
        PBIDEBUG("PVR Search Table.\n");
        /* search Pat */
        SFILTER_PVR_AddPid( DEMUX_2, PAT_TABLE_ID, 0, &u32PatChannelHandle);
        SFILTER_PVR_AddFilter( DEMUX_2, u32PatChannelHandle, 5000, PAT_TABLE_ID, \
                              0,0, PvrFilter_PatCallback, (void *)&u32PatAddr);
        SFILTER_PVR_DelPid( DEMUX_2, &u32PatChannelHandle); /* 关闭后 breakground PAT 停止 */
        
        pPatTable = (PSISI_Table_t *)u32PatAddr;
        if( NULL != pPatTable )
        {
            pPatSection = (PAT_Section_t *)(pPatTable->ppSection[0]);
            if( NULL == pPatSection)
            {
                if( NULL != pPatTable )
                {
                    psisi_section_free( pPatTable );
                }
            }
            else
            {
                for( ii = 0; ii< pPatSection->u16ProgNum; ii++ )
                {
                    if( pPatSection->ptProgram[ii].u16ProgramNumber == pPvrParam->u16ServId )
                    {
                        if( pPatSection->ptProgram[ii].u16PMTPid != pPvrParam->u16PmtPid )
                        {
                            pbiinfo("[%s %d] Pvr ServId Changed. 0x%x V 0x%x.", \
                                DEBUG_LOG, pPvrParam->u16PmtPid ,pPatSection->ptProgram[ii].u16PMTPid);
                            pPvrParam->u16PmtPid = pPatSection->ptProgram[ii].u16PMTPid; 
                        }
                    }
                }

                if( NULL != pPatTable )
                {
                    PSISI_Free_Table( pPatTable );
                }
                
            }
        }
		else
		{
			PBIDEBUG("-----------pat is null------------\n");
            return -1;
		}
	    g_tPvrInfo[DEMUX_2].serviceid =pPvrParam->u16ServId;	
          g_tPvrInfo[DEMUX_2].pmtpid  =pPvrParam->u16PmtPid;
        /* search Pmt */
        SFILTER_PVR_AddPid( DEMUX_2, PMT_TABLE_ID, 0, &u32PmtChannelHandle);
        SFILTER_PVR_AddFilter( DEMUX_2, u32PmtChannelHandle, 5000, PMT_TABLE_ID, \
                               pPvrParam->u16ServId ,pPvrParam->u16PmtPid, PvrFilter_PmtCallback, (void *)&u32PmtAddr);
        SFILTER_PVR_DelPid( DEMUX_2, &u32PmtChannelHandle);

        pPmtTable = (PSISI_Table_t *)u32PmtAddr;
        if( NULL == pPmtTable )
        {
            PBIDEBUG("-----------2222-3333------------\n");
            return -1;
        }

        pPmtSection = (PMT_Section_t *)(pPmtTable->ppSection[0]);
        if( NULL == pPmtSection )
        {
            PBIDEBUG("PVR Search Pmt PmtSection is NULL\n");
            if( NULL != pPmtTable )
            {
                psisi_section_free( pPmtTable );
            }
            return -1;
        }
		if(pPvrParam->tTimeRec.u32PCRPid <= 0x1FFF){
			g_tPvrInfo[DEMUX_2].PCRPid = pPmtSection->u16PCRPid;
			SFILTER_PVR_AddPid( DEMUX_2, g_tPvrInfo[DEMUX_2].PCRPid, 0, &g_tPvrInfo[DEMUX_2].pcrchannelhandle);
		}
        pbiinfo("table id 0x%x.\n",pPmtSection->u8TableId);
        pbiinfo("Pmt pid 0x%x.\n",pPmtSection->u16ProgramNum);
		pbiinfo("PCR pid 0x%x.\n",pPmtSection->u16PCRPid);

        PBIDEBUG("PVR Open Channel\n");
        /* open channel */
        if( NULL != pPmtSection->ptVideo )
        {
            pbiinfo("set Video \n");
            stRecAttr.enIndexType = 1;
            stRecAttr.enIndexVidType = _ChangeVideoFmt(pPmtSection->ptVideo[0].u8StreamType);
            stRecAttr.u32IndexPid = pPmtSection->ptVideo[0].u16ElementaryPid;
            pbiinfo("videoPid 0x%x  Fmt 0x%x.\n",pPmtSection->ptVideo[0].u16ElementaryPid, stRecAttr.enIndexVidType);
            SFILTER_PVR_AddPid( DEMUX_2, pPmtSection->ptVideo[0].u16ElementaryPid, 0, &g_tPvrInfo[DEMUX_2].videochannelhandle);
            g_tPvrInfo[DEMUX_2].VideoPid = pPmtSection->ptVideo[0].u16ElementaryPid;
            tUserData.u8VidFmt = pPmtSection->ptVideo[0].u8StreamType;
        }

        g_tPvrInfo[DEMUX_2].AudioChnNum = 0;
        if( NULL != pPmtSection->ptAudio )
        {
            if( (NULL == pPmtSection->ptVideo) && ( 0 < pPmtSection->u16AudioNum ) )
            {
                stRecAttr.enIndexType = 2;
                stRecAttr.enIndexVidType = _ChangeVideoFmt(0xFFFF);
                stRecAttr.u32IndexPid = pPmtSection->ptAudio[0].u16ElementaryPid;
            }
            pbiinfo("set Audio \n");
            g_tPvrInfo[DEMUX_2].AudioChnNum = pPmtSection->u16AudioNum;
            for( ii = 0; ii < pPmtSection->u16AudioNum; ii++ )
            {
                pbiinfo("audioPid 0x%x .\n",pPmtSection->ptAudio[ii].u16ElementaryPid);
                SFILTER_PVR_AddPid( DEMUX_2, pPmtSection->ptAudio[ii].u16ElementaryPid, 0, &g_tPvrInfo[DEMUX_2].audiochannelhandle[ii]);
                tUserData.u16AudPid[ii] = pPmtSection->ptAudio[ii].u16ElementaryPid;
                tUserData.u8AudFmt[ii] = pPmtSection->ptAudio[ii].u8StreamType;
                g_tPvrInfo[DEMUX_2].AudioChnNum++;
            }
            g_tPvrInfo[DEMUX_2].AudioPid = pPmtSection->ptAudio[0].u16ElementaryPid;

#if 0
	U16   ss =0,pp =0;
	U8     *pmt_send_ca_buf =NULL,total_len =0,len1 =0,s_stream_count =0;
	PMT_Section_t * ptPMTSect = pPmtSection;
	U8    stream_sum =0;
	pmt_send_ca_buf =OS_MemAllocate(system_p,5);
	if(pmt_send_ca_buf ==NULL)
	{
		pbiinfo("%s   %d  error ",__FUNCTION__,__LINE__);
	}
	else
	{
		pmt_send_ca_buf[0]=0;
		pmt_send_ca_buf[1]=2;
		pmt_send_ca_buf[2]=0xff;
		pmt_send_ca_buf[3]=0xff;
		pmt_send_ca_buf[4] =0;
		total_len =5;
		
		if((ptPMTSect->u16CA_DescNum>0)&&(ptPMTSect->ptCA_Desc!=NULL))
		{
		     pmt_send_ca_buf =OS_MemReallocate(system_p,pmt_send_ca_buf,total_len +6*(ptPMTSect->u16CA_DescNum));	
		     if( pmt_send_ca_buf ==NULL)
		     {
				pbiinfo("%s   %d  error ",__FUNCTION__,__LINE__);
			}
			 else
			 {
				for(ss=0;ss< ptPMTSect->u16CA_DescNum;ss++)
				{							
					pmt_send_ca_buf[5+ss*6+0] =0x09;
					pmt_send_ca_buf[5+ss*6+1] =4;
					pmt_send_ca_buf[5+ss*6+2] =((ptPMTSect->ptCA_Desc[ss].u16CASystemId)&0xff00)>>8;
					pmt_send_ca_buf[5+ss*6+3] =(ptPMTSect->ptCA_Desc[ss].u16CASystemId)&0x00ff;
					pmt_send_ca_buf[5+ss*6+4] =((ptPMTSect->ptCA_Desc[ss].u16CAPid)&0xff00)>>8;
					pmt_send_ca_buf[5+ss*6+5] =(ptPMTSect->ptCA_Desc[ss].u16CAPid)&0x00ff;					
				}
				pmt_send_ca_buf[4] =ptPMTSect->u16CA_DescNum;
				total_len +=(ptPMTSect->u16CA_DescNum)*6;
				pbiinfo("%s   %d   totallen %d ",__FUNCTION__,__LINE__,total_len);
			 }
		}
		s_stream_count =total_len+0;//stream count;
	 if ((NULL == ptPMTSect->ptVideo)
            && ((NULL == ptPMTSect->ptAudio) || (0 == ptPMTSect->u16AudioNum)) )
	    {
	    	
	    }
	 else
	 {
		if (ptPMTSect->ptVideo != NULL)
		{
			stream_sum =(1 +ptPMTSect->u16AudioNum);
		}
		else
		{
			stream_sum =ptPMTSect->u16AudioNum;
		}
	 	pmt_send_ca_buf[total_len] =stream_sum;
		pbiinfo("%s   %d   total[%d] pes %d   ",__FUNCTION__,__LINE__,total_len,pmt_send_ca_buf[total_len] );
		if (ptPMTSect->ptVideo != NULL)
		{
			
			len1 =6*(ptPMTSect->ptVideo->u16CA_DescNum ) +4;
			//len1 +=1;
			pmt_send_ca_buf =OS_MemReallocate(system_p,pmt_send_ca_buf,total_len + len1);	
		     if( pmt_send_ca_buf ==NULL)
		     {
				pbiinfo("%s   %d  error ",__FUNCTION__,__LINE__);
			}
			 else
			 {
				
				pmt_send_ca_buf[total_len +1] =ptPMTSect->ptVideo->u8StreamType;
				pmt_send_ca_buf[total_len +2] =((ptPMTSect->ptVideo->u16ElementaryPid)&0xff00)>>8;
				pmt_send_ca_buf[total_len +3] =((ptPMTSect->ptVideo->u16ElementaryPid)&0xff);
				pmt_send_ca_buf[total_len +4] =ptPMTSect->ptVideo->u16CA_DescNum;
				for(ss=0;ss<ptPMTSect->ptVideo->u16CA_DescNum;ss++)
				{
					pmt_send_ca_buf[total_len +5+ss*6+0] =0x9;
					pmt_send_ca_buf[total_len +5+ss*6+1] =4;
					pmt_send_ca_buf[total_len +5+ss*6+2] =((ptPMTSect->ptVideo->ptCA_Desc[ss].u16CASystemId)&0xff00)>>8;
					pmt_send_ca_buf[total_len +5+ss*6+3] =((ptPMTSect->ptVideo->ptCA_Desc[ss].u16CASystemId)&0xff);
					pmt_send_ca_buf[total_len +5+ss*6+4] =((ptPMTSect->ptVideo->ptCA_Desc[ss].u16CAPid)&0xff00)>>8;
					pmt_send_ca_buf[total_len +5+ss*6+5] =((ptPMTSect->ptVideo->ptCA_Desc[ss].u16CAPid)&0xff);
				}
				//s_stream_count =total_len+0;//stream count;
				//pmt_send_ca_buf[total_len+0] = 1;//stream count;
				total_len+=len1;
				pbiinfo("%s   %d  len1 %d  totallen %d ",__FUNCTION__,__LINE__,len1,total_len);
			 }
			
		}
		if ((ptPMTSect->ptAudio != NULL) && (ptPMTSect->u16AudioNum != 0))
		{
			//stream_sum +=ptPMTSect->u16AudioNum ;
			for(ss=0;ss<ptPMTSect->u16AudioNum;ss++)
			{
				len1= (4+( ptPMTSect->ptAudio[ss].u16CA_DescNum*6));
				pbiinfo("%s   %d  len1 %d  totallen %d ",__FUNCTION__,__LINE__,len1,total_len);
				pmt_send_ca_buf =OS_MemReallocate(system_p,pmt_send_ca_buf,total_len + len1);	
			      if( pmt_send_ca_buf ==NULL)
			      {
					pbiinfo("%s   %d  error ",__FUNCTION__,__LINE__);
				}
				else
				{
					pmt_send_ca_buf[total_len +1] =ptPMTSect->ptAudio[ss].u8StreamType;
					pmt_send_ca_buf[total_len +2] =((ptPMTSect->ptAudio[ss].u16ElementaryPid)&0xff00)>>8;
					pmt_send_ca_buf[total_len +3] =((ptPMTSect->ptAudio[ss].u16ElementaryPid)&0xff);
					pmt_send_ca_buf[total_len +4] =ptPMTSect->ptAudio[ss].u16CA_DescNum;
					for(pp =0;pp<ptPMTSect->ptAudio[ss].u16CA_DescNum;pp++)
					{
						pmt_send_ca_buf[total_len +5+pp*6 +0] = 0x9;
						pmt_send_ca_buf[total_len +5+pp*6 +1] = 4;
						pmt_send_ca_buf[total_len +5+pp*6 +2] = ((ptPMTSect->ptAudio[ss].ptCA_Desc[pp].u16CASystemId)&0xff00)>>8;
						pmt_send_ca_buf[total_len +5+pp*6 +3] =(ptPMTSect->ptAudio[ss].ptCA_Desc[pp].u16CASystemId)&0xff;
						pmt_send_ca_buf[total_len +5+pp*6 +4] = ((ptPMTSect->ptAudio[ss].ptCA_Desc[pp].u16CAPid)&0xff00)>>8;
						pmt_send_ca_buf[total_len +5+pp*6 +5] =(ptPMTSect->ptAudio[ss].ptCA_Desc[pp].u16CAPid)&0xff;
					}

					total_len +=len1;
					pbiinfo("%s   %d  len1 %d  totallen %d ",__FUNCTION__,__LINE__,len1,total_len);
				}
			}
			//pmt_send_ca_buf[s_stream_count] =stream_sum;
			//pbiinfo("####count %d  ---count %d",s_stream_count,pmt_send_ca_buf[s_stream_count] );
		}


	 }
            DVBPlayer_Event_t tEvent;
	tEvent.eEventType = 2;
#if 1
	int iii =0;
	for(iii=0;iii<total_len;iii++)
	{
		pbiinfo("@@ %02x  ",pmt_send_ca_buf[iii]);
	}

#endif

	tEvent.pEventData = 	pmt_send_ca_buf;
            CA_DVBPlay_CallBack( tEvent );

	}




#endif

			
        }
    }
    else /* TMS And TVRec */
    {
        if( 2 == pPvrParam->u8RecFlag )
        {
            g_TmsPlayFlag = 1;
            DRV_AVCtrl_PVR_Stop();
        }

        PBIDEBUG("TMS and TVRec PVR Lock Tuner\n");
		pbiinfo("eSignalSource:[%d]\n", pPvrParam->tBackRec.uTunerDesc.eSignalSource);
        //if( eDVBCORE_SIGNAL_SOURCE_DVBC == pPvrParam->tBackRec.uTunerDesc.eSignalSource )

        if(2 != pPvrParam->u8RecFlag)
        {
#if 0        
            int retry = 0;
            TUNER_LOCKSTATUS_t tunerState = eTun_unLock;

            tTunerInfo.MediumType = MEDIUM_TYPE_DVBC;
            tTunerInfo.u.Cable.FrequencyKHertz = pPvrParam->tBackRec.uTunerDesc.tCable.u32Freq;
            tTunerInfo.u.Cable.SymbolRateSPS = pPvrParam->tBackRec.uTunerDesc.tCable.u32SymbRate;
            tTunerInfo.u.Cable.Modulation = pPvrParam->tBackRec.uTunerDesc.tCable.eMod + 5;
			tTunerInfo.u.Cable.Bandwidth = GetTunerBandWidth();

			pbiinfo("FrequencyKHertz:[%d]\n", pPvrParam->tBackRec.uTunerDesc.tCable.u32Freq);
			pbiinfo("SymbolRateSPS:[%d]\n", pPvrParam->tBackRec.uTunerDesc.tCable.u32SymbRate);
			pbiinfo("Modulation:[%d]\n", pPvrParam->tBackRec.uTunerDesc.tCable.eMod);
//			pbiinfo("Bandwidth:[%d]\n", pPvrParam->tBackRec.uTunerDesc.tCable.eBandWidth);
			/*tTunerInfo.MediumType = MEDIUM_TYPE_DVBC;
            tTunerInfo.u.Cable.FrequencyKHertz = 698000;
            tTunerInfo.u.Cable.SymbolRateSPS = 6875;
            tTunerInfo.u.Cable.Modulation = 7;
			tTunerInfo.u.Cable.Bandwidth = 0;*/

            //rTunerError = DRV_Tuner_SetFrequency(0, &tTunerInfo, 0 );
			rTunerError = DRV_Tuner_SetFrequency(2, &tTunerInfo, 0 );
            do
            {
                if( retry >= 30)
                {
                    PBIDEBUG("lock tuner failed.\n");
                    return -1;   /* NO LOCK*/
                }
                OS_TaskDelay(100);
                //tunerState = DRV_Tuner_GetLockStatus(0);
				tunerState = DRV_Tuner_GetLockStatus(2);
                retry++;
            }
            while (tunerState != eTun_Lock);
#endif            
        }
        //else
        {

        }
		if(pPvrParam->tTimeRec.u32PCRPid <= 0x1FFF){
			g_tPvrInfo[DEMUX_2].PCRPid = pPvrParam->tTimeRec.u32PCRPid;
			SFILTER_PVR_AddPid( DEMUX_2, g_tPvrInfo[DEMUX_2].PCRPid, 0, &g_tPvrInfo[DEMUX_2].pcrchannelhandle);
		}
		pbiinfo("RecPMT u32VidPid[%x] u32AudPid[%x] u32PCRPid[%x]\n",pPvrParam->tTimeRec.u32VidPid, pPvrParam->tTimeRec.u32AudPid, pPvrParam->tTimeRec.u32PCRPid );
        if( pPvrParam->tTimeRec.u32VidPid < 0x1FFF )
        {
            stRecAttr.enIndexType = 1;
            stRecAttr.u32IndexPid = pPvrParam->tTimeRec.u32VidPid;
            stRecAttr.enIndexVidType = _ChangeVideoFmt(pPvrParam->tTimeRec.u32VidFmt);
            SFILTER_PVR_AddPid( DEMUX_2, pPvrParam->tTimeRec.u32VidPid, 0, &g_tPvrInfo[DEMUX_2].videochannelhandle);
            g_tPvrInfo[DEMUX_2].VideoPid = pPvrParam->tTimeRec.u32VidPid;
            tUserData.u8VidFmt = pPvrParam->tTimeRec.u32VidFmt;
#if 0/* CA */
            DVBPlayer_Event_t tEvent;
            U32		data[2]= {0,0};

            memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
            tEvent.eEventType = 4;
            data[0] = pPvrParam->tTimeRec.u32VidPid;
            data[1] = g_tPvrInfo[DEMUX_2].videochannelhandle;
            tEvent.pEventData = &data;
            CA_DVBPlay_CallBack( tEvent );
            pbiinfo("DVBPlayer_PVR_RecStart  line%d----------------ca  ---%d   %d   \n",__LINE__,data[0],data[1]);
#endif
        }

        if( pPvrParam->tTimeRec.u32AudPid < 0x1FFF )
        {
            if( pPvrParam->tTimeRec.u32VidPid >= 0x1FFF )
            {
                stRecAttr.enIndexType = 2;
                stRecAttr.u32IndexPid = pPvrParam->tTimeRec.u32AudPid;
                stRecAttr.enIndexVidType = _ChangeVideoFmt(0xFFFF);
            }
            SFILTER_PVR_AddPid( DEMUX_2, pPvrParam->tTimeRec.u32AudPid, 0, &g_tPvrInfo[DEMUX_2].audiochannelhandle[0]);

            g_tPvrInfo[DEMUX_2].AudioPid = pPvrParam->tTimeRec.u32AudPid;
			g_tPvrInfo[DEMUX_2].AudioChnNum = 1;
            tUserData.u16AudPid[0] = pPvrParam->tTimeRec.u32AudPid;
            tUserData.u8AudFmt[0] = pPvrParam->tTimeRec.u32AudFmt;

#if 0
            {
                DVBPlayer_Event_t tEvent;
                U32    data[2]= {0,0};

                memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
                tEvent.eEventType = 4;
                data[0] = pPvrParam->tTimeRec.u32AudPid;
                data[1] = g_tPvrInfo[DEMUX_2].audiochannelhandle[0];
                g_tPvrInfo[DEMUX_2].AudioChnNum = 1;
                tEvent.pEventData = &data;
                CA_DVBPlay_CallBack( tEvent );
                pbiinfo("DVBPlayer_PVR_RecStart  line%d----------------ca  ---%d   %d   \n",__LINE__,data[0],data[1]);
            }
#endif
        }
	pbiinfo("@@@@@@@@@@@---start ");		
	 if( 1 == pPvrParam->u8RecFlag )
	{
#if 0	
	U16   ss =0,pp =0;
	U8     *pmt_send_ca_buf =NULL,total_len =0,len1 =0,s_stream_count =0;
	PMT_Section_t * ptPMTSect = NULL;

	pmt_send_ca_buf =Get_player_pmt_info();
	if(pmt_send_ca_buf!=NULL)
		{
		DVBPlayer_Event_t tEvent;
		tEvent.eEventType = 2;
		memset(g_tPvrInfo[DEMUX_2].pmtInfo,0,1024);
		memcpy(g_tPvrInfo[DEMUX_2].pmtInfo,pmt_send_ca_buf,1024);
#if 1
		int iii =0;
		g_tPvrInfo[DEMUX_2].pmtInfo[1]=2;
		g_tPvrInfo[DEMUX_2].pmtInfo[2] =0xff;
		g_tPvrInfo[DEMUX_2].pmtInfo[3] =0xff;
		for(iii=0;iii<50;iii++)
			 {
			pbiinfo("@@ %02x  ",g_tPvrInfo[DEMUX_2].pmtInfo[iii]);
			 }
#endif
		tEvent.pEventData = 	g_tPvrInfo[DEMUX_2].pmtInfo;
			CA_DVBPlay_CallBack( tEvent );	


		
     }
#endif     
 	}	

    }
    	
     
	
	if(2 == pPvrParam->u8RecFlag)
    stRecAttr.u32DemuxID = DEMUX_0;
	else
    stRecAttr.u32DemuxID = DEMUX_2;
	
    stRecAttr.u64MaxFileSize = pPvrParam->u64MaxFileLength;
    stRecAttr.u32FileNameLen = strlen(pPvrParam->FileName);

    memcpy( stRecAttr.szFileName, pPvrParam->FileName, stRecAttr.u32FileNameLen+1 );
    stRecAttr.szFileName[stRecAttr.u32FileNameLen] = '\0';

    pbiinfo("FileLen = %d..\n",stRecAttr.u32FileNameLen);
    pbiinfo("MaxLen = 0x%x.\n",stRecAttr.u64MaxFileSize);

    PBIDEBUG("PVR DRV_PVR_RecInit");
    u32Ret = DRV_PVR_RecInit( &stRecAttr, pPvrParam->u16LogicNum );
    if( 0 != u32Ret )
    {
        PBIDEBUG("[ERROR] DRV_PVR_RecInit");
        if( NULL != pPmtTable )
        {
            PSISI_Free_Table( pPmtTable );
        }
        return PV_FAILURE;
    }
    
	tUserData.u16PcrPid = g_tPvrInfo[DEMUX_2].PCRPid;
    tUserData.u16VidPid = g_tPvrInfo[DEMUX_2].VideoPid;
    tUserData.u8AudNum = g_tPvrInfo[DEMUX_2].AudioChnNum;
    memcpy( tUserData.pName, stRecAttr.szFileName, stRecAttr.u32FileNameLen );
    tUserData.pName[stRecAttr.u32FileNameLen] = '\0';


    pbiinfo("[%s %d] -----Set UserData----------------\n",DEBUG_LOG);
    pbiinfo("Vid Pid 0x%x.\n", tUserData.u16VidPid );
    pbiinfo("Aud Num 0x%x.\n", tUserData.u8AudNum );
    pbiinfo("Aud Pid 0x%x.\n", tUserData.u16AudPid[0] );
    pbiinfo("Vid Fmt 0x%x.\n", tUserData.u8VidFmt );
    pbiinfo("Aud Fmt 0x%x.\n", tUserData.u8AudFmt[0] );
	pbiinfo("Pcr Pid 0x%x.\n", tUserData.u16PcrPid );
    pbiinfo("Name =%s.\n",tUserData.pName);
    pbiinfo("[%s %d] -----Set UserData end----------------\n",DEBUG_LOG);

    DRV_PVR_SetUserData( stRecAttr.szFileName, &tUserData, sizeof(PVR_USER_DATA_T) );

	u32Ret = DRV_PVR_RecStart();
    if( 0 != u32Ret )
    {
        PBIDEBUG("[ERROR] DRV_PVR_RecStart");
        if( NULL != pPmtTable )
        {
            PSISI_Free_Table( pPmtTable );
        }
        return PV_FAILURE;
    }

/*    if( NULL != pPmtTable )
    {
        PSISI_Free_Table( pPmtTable );
    }*/ //liqian 20131108
	PSISI_Free_Table( pPmtTable );
    PBIDEBUG("DVBPlayer_PVR_RecStart OK!!!");
    return PV_SUCCESS;
}

#if 0
S32 DVBPlayer_PVR_RecStopExt()
{
    S32 ErrRet = PV_SUCCESS;
    S32 iI = 0;
    S32 s32RecStatus = 0;
    S32 s32PlayStatus = 0;

    s32RecStatus = _GetPvrRecStatus();
    if( -1 == s32RecStatus )
    {
        return PV_FAILURE;
    }
    else if( 1 != s32RecStatus )
    {
        PBIDEBUG("[INFO] DVBPlayer_PVR_RecStart is Run!!!");
        return PV_SUCCESS;
    }
 pbiinfo("%s---%d-------- -- ---- \n",__FUNCTION__,__LINE__);
	SYS_BackGround_TableResume(SYS_BG_PAT);

	pbiinfo("[%s %d]-------AudioChnNum[%d]- -- ---- \n",__FUNCTION__,__LINE__, g_tPvrInfo[DEMUX_2].AudioChnNum);
    SFILTER_PVR_DelPid( DEMUX_2, &g_tPvrInfo[DEMUX_2].videochannelhandle );
    for( iI = 0; iI < g_tPvrInfo[DEMUX_2].AudioChnNum; iI++ )
    {
        SFILTER_PVR_DelPid( DEMUX_2, &g_tPvrInfo[DEMUX_2].audiochannelhandle[iI] );
    }
	 pbiinfo("%s---%d-------- -- ---- \n",__FUNCTION__,__LINE__);

    ErrRet = DRV_PVR_RecStop();

    ErrRet = DRV_PVR_RecUnInit();


    memset( &g_tPvrInfo[DEMUX_2], 0xFF, sizeof(PVR_INFO_t) );

    g_tPvrInfo[DEMUX_2].AudioPid = 0x1FFF;
    g_tPvrInfo[DEMUX_2].VideoPid = 0x1FFF;
    if( PV_SUCCESS != ErrRet )
    {
        PBIDEBUG("[error] DRV_PVR_RecStop || DRV_PVR_RecUnInit");
        return ErrRet;
    }
	DRV_PVR_Stop_Phone_Sending();
    PBIDEBUG(" DVBPlayer_PVR_RecStop OK!!!!");
    return PV_SUCCESS;
}
#endif

S32 DVBPlayer_PVR_RecStop(U32 VidPid, U32 VidFmt, U32 AudPid, U32 AudFmt)
{
    S32 ErrRet = PV_SUCCESS;
    S32 iI = 0;
    S32 s32RecStatus = 0;
    S32 s32PlayStatus = 0;

    s32RecStatus = _GetPvrRecStatus();
    if( -1 == s32RecStatus )
    {
        return PV_FAILURE;
    }
    else if( 1 != s32RecStatus )
    {
        PBIDEBUG("[INFO] DVBPlayer_PVR_RecStart is Run!!!");
        return PV_SUCCESS;
    }
 pbiinfo("%s---%d-------- -- ---- \n",__FUNCTION__,__LINE__);
	//SYS_BackGround_TableResume(SYS_BG_PAT);
#if 1
    if ( 1 == g_TmsPlayFlag )
    {
        PBIDEBUG("Start AV And CA!!");
        if( AudPid < 0x1FFF )
        {
            ErrRet |=  _Start_Avdio( AudPid, AudFmt);
        }
        if( VidPid < 0x1FFF )
        {
            ErrRet |=  _Start_Video( VidPid, VidFmt);
        }
        if( PV_SUCCESS != ErrRet )
        {
            PBIDEBUG("[error] _Start_Avdio || _Start_Video");
            return ErrRet;
        }
#if 0
        {
            DVBPlayer_Event_t tEvent;
            U32 	data[2]= {0,0};

            memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
            data[0] =VidPid;
            data[1]=AudPid;
            tEvent.eEventType = 6;
            tEvent.pEventData = &data;
            CA_DVBPlay_CallBack( tEvent );
            pbiinfo("rec stop============DVBPlayer_PVR_RecStop----attach play ch-----------\n");
        }
#endif
	
        g_TmsPlayFlag = 0;
    }
    else //liqian 20131108
    {
		DVBPlayer_Event_t tEvent;
       PostChanChange_t tCaPostChan;

		memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
        tEvent.eEventType = 0; 
	tCaPostChan.demuxid =2;
	tCaPostChan.wChannelId =0xffff;
	tCaPostChan.bState =0;
        tEvent.pEventData = &tCaPostChan;
		CA_DVBPlay_CallBack( tEvent );
        pbiinfo("rec stop============DVBPlayer_PVR_RecStop----detach-----------\n");

    }
#endif

#if 0
    {
        DVBPlayer_Event_t tEvent;
        U32 	data[2]= {0,0};

        memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
        tEvent.eEventType = 5;
        tEvent.pEventData = &data;
        CA_DVBPlay_CallBack( tEvent );
        pbiinfo("rec stop============DVBPlayer_PVR_RecStop----detach-----------\n");

    }
#endif
	pbiinfo("[%s %d]-------AudioChnNum[%d]- -- ---- \n",__FUNCTION__,__LINE__, g_tPvrInfo[DEMUX_2].AudioChnNum);
    SFILTER_PVR_DelPid( DEMUX_2, &g_tPvrInfo[DEMUX_2].videochannelhandle );
    for( iI = 0; iI < g_tPvrInfo[DEMUX_2].AudioChnNum; iI++ )
    {
        SFILTER_PVR_DelPid( DEMUX_2, &g_tPvrInfo[DEMUX_2].audiochannelhandle[iI] );
    }
	 pbiinfo("%s---%d-------- -- ---- \n",__FUNCTION__,__LINE__);

    ErrRet = DRV_PVR_RecStop();

    ErrRet = DRV_PVR_RecUnInit();
 pbiinfo("%s---%d-------- -- ---- \n",__FUNCTION__,__LINE__);

 //   if(g_tPvrInfo[DEMUX_2].pmtInfo !=NULL)
	//	PSISI_Free_Table( g_tPvrInfo[DEMUX_2].pmtInfo);
// pbiinfo("%s---%d-------- -- ---- \n",__FUNCTION__,__LINE__);

    memset( &g_tPvrInfo[DEMUX_2], 0xFF, sizeof(PVR_INFO_t) );
//    g_tPvrInfo[DEMUX_2].pmtInfo =NULL;
//	 pbiinfo("%s---%d-------- -- ---- \n",__FUNCTION__,__LINE__);

    g_tPvrInfo[DEMUX_2].AudioPid = 0x1FFF;
    g_tPvrInfo[DEMUX_2].VideoPid = 0x1FFF;
    if( PV_SUCCESS != ErrRet )
    {
        PBIDEBUG("[error] DRV_PVR_RecStop || DRV_PVR_RecUnInit");
        return ErrRet;
    }

    PBIDEBUG(" DVBPlayer_PVR_RecStop OK!!!!");
    return PV_SUCCESS;
}

S32 DVBPlayer_PVR_PvrRemoveFile( C8 *pFileName )
{
    if( NULL == pFileName )
    {
        return PV_NULL_PTR;
    }

    DRV_PVR_RemoveFile(pFileName);

    return PV_SUCCESS;
}

S32 DVBPlayer_PVR_RecGetStatus( DVBPlayer_PVR_REC_STATUS_t *pRecStatus )
{
    S32 ErrRet = PV_SUCCESS;

    ErrRet = DRV_PVR_RecGetStatus((PVR_REC_STATUS_t*)pRecStatus);
    if( PV_SUCCESS != ErrRet )
    {
        PBIDEBUG("[error] DRV_PVR_RecGetStatus");
        return ErrRet;
    }

    return PV_SUCCESS;
}

S32 DVBPlayer_PVR_MessageInit( DvbPvrEventCallBack pCallBack )
{
    S32 ErrRet = PV_SUCCESS;

    DRV_PVR_MessageInit( (EventCallBack)pCallBack );
	DRV_PVR_PlayerRegisterCallBack(pCallBack);
    return ErrRet;
}

S32 DVBPlayer_PVR_MessageUnInit()
{
    S32 ErrRet = PV_SUCCESS;

    DRV_PVR_MessageUnInit();
    return ErrRet;
}

S32 DVBPlayer_PVR_PlayStart(U32 VidPid, U32 VidFmt, U32 AudPid, U32 AudFmt, C8 *pName, U8 u8UseEnctypt )
{
    U32 *pPlayerHandle = NULL;
    S32 ErrRet = PV_SUCCESS;
    S32 s32PlayStatus = 0;

    s32PlayStatus = _GetPvrPlayStatus();
    if( -1 == s32PlayStatus )
    {
        return PV_FAILURE;
    }
    else if( 1 == s32PlayStatus )
    {
        PBIDEBUG("[INFO]  PVR Play was Run!!!!!");
        return PV_SUCCESS;
    }
#if 0
    ErrRet = DRV_AVCtrl_UnInit();
    if( PV_SUCCESS != ErrRet )
    {
        PBIDEBUG("[error] DRV_AVCtrl_UnInit");
        return ErrRet;
    }

    ErrRet = SFILTER_PVR_Player_Init( &g_TsBuffer );
    if( PV_SUCCESS != ErrRet )
    {
        PBIDEBUG("[error] SFILTER_PVR_Player_Init");
        return ErrRet;
    }

    ErrRet = DRV_AVCtrl_Init(0);
    if( PV_SUCCESS != ErrRet )
    {
        PBIDEBUG("[error] DRV_AVCtrl_Init");
        return ErrRet;
    }
#endif
    pbiinfo("PlayerPMT %x %x %x %x\n",VidPid, VidFmt, AudPid, AudFmt );
    if( AudPid < 0x1FFF )
    {
        ErrRet |= _Start_Avdio( AudPid, AudFmt);
    }
    if( VidPid < 0x1FFF )
    {
        ErrRet |= _Start_Video( VidPid, VidFmt);
    }
    if( PV_SUCCESS != ErrRet )
    {
        PBIDEBUG("[error] _Start_Avdio || _Start_Video ");
        return ErrRet;
    }

    pPlayerHandle = DRV_AVCtrl_GetAVHandle();
    ErrRet = DRV_PVR_PlayInit( &g_TsBuffer, pPlayerHandle, pName, u8UseEnctypt );
    if( PV_SUCCESS != ErrRet )
    {
        PBIDEBUG("[error] DRV_AVCtrl_GetAVHandle");
        return ErrRet;
    }

    ErrRet = DRV_PVR_PlayStart();
    if( PV_SUCCESS != ErrRet )
    {
        PBIDEBUG("[error] DRV_PVR_PlayStart");
        return ErrRet;
    }
    g_TmsPlayFlag = 2;
    PBIDEBUG("DVBPlayer_PVR_PlayStart Ok!!!");
    return PV_SUCCESS;
}

S32 DVBPlayer_PVR_PlayStop(U32 VidPid, U32 VidFmt, U32 AudPid, U32 AudFmt)
{
    S32 ErrRet = PV_SUCCESS;
    S32 s32PlayStatus = 0;

    s32PlayStatus = _GetPvrPlayStatus();
    if( -1 == s32PlayStatus )
    {
        return PV_FAILURE;
    }
    else if( 0 == s32PlayStatus )
    {
        PBIDEBUG("[INFO] PVR was Stop!!!!");
        return PV_SUCCESS;
    }

    ErrRet = DRV_PVR_PlayStop();
    if( PV_SUCCESS != ErrRet )
    {
        PBIDEBUG("[error] DRV_PVR_PlayStop");
        // return ErrRet;
    }
#if 0//Timeshift 时序问题
    ErrRet = DRV_AVCtrl_UnInit();
    if( PV_SUCCESS != ErrRet )
    {
        PBIDEBUG("[error] DRV_AVCtrl_UnInit");
        return ErrRet;
    }
    ErrRet =  SFILTER_PVR_Player_UnInit( &g_TsBuffer );
    if( PV_SUCCESS != ErrRet )
    {
        PBIDEBUG("[error] SFILTER_PVR_Player_UnInit");
        return ErrRet;
    }
    ErrRet =  DRV_AVCtrl_Init(DEMUX_0);
    if( PV_SUCCESS != ErrRet )
    {
        PBIDEBUG("[error] DRV_AVCtrl_Init");
        return ErrRet;
    }
#endif
#if 1
    if ( 2 == g_TmsPlayFlag )
    {
        if( AudPid < 0x1FFF )
        {
            ErrRet |=  _Start_Avdio( AudPid, AudFmt);
        }
        if( VidPid < 0x1FFF )
        {
            ErrRet |=  _Start_Video( VidPid, VidFmt);
        }
        if( PV_SUCCESS != ErrRet )
        {
            PBIDEBUG("[error] _Start_Avdio || _Start_Video");
            return ErrRet;
        }
        g_TmsPlayFlag = 0;
    }
#endif
    DRV_PVR_PlayUnInit();
    PBIDEBUG("DVBPlayer_PVR_PlayStop Ok!!!");
    return PV_SUCCESS;
}

S32 DVBPlayer_PVR_PlayPause()
{
    S32 ErrRet = PV_SUCCESS;
    S32 s32PlayStatus = 0;

    s32PlayStatus = _GetPvrPlayStatus();
    if( -1 == s32PlayStatus )
    {
        return PV_FAILURE;
    }
    else if( 0 == s32PlayStatus )
    {
        PBIDEBUG("[INFO] PVR was Stop!!!!");
        return PV_SUCCESS;
    }
    ErrRet = DRV_PVR_PlayPause();

    return ErrRet;
}

S32 DVBPlayer_PVR_PlayResume()
{
    S32 ErrRet = PV_SUCCESS;
    S32 s32PlayStatus = 0;

    s32PlayStatus = _GetPvrPlayStatus();
    if( -1 == s32PlayStatus )
    {
        return PV_FAILURE;
    }
    else if( 0 == s32PlayStatus )
    {
        PBIDEBUG("[INFO] PVR was Stop!!!!");
        return PV_SUCCESS;
    }
    ErrRet = DRV_PVR_PlayResume();

    return ErrRet;
}

S32 DVBPlayer_PVR_PlaySpeedCtrl(PVR_SPEED_E ePlaySpeed)
{
    S32 ErrRet = PV_SUCCESS;
    S32 s32PlayStatus = 0;

    s32PlayStatus = _GetPvrPlayStatus();
    if( -1 == s32PlayStatus )
    {
        return PV_FAILURE;
    }
    else if( 0 == s32PlayStatus )
    {
        PBIDEBUG("[INFO] PVR was Stop!!!!");
        return PV_SUCCESS;
    }
    ErrRet = DRV_PVR_PlayTrickMode(ePlaySpeed);

    return ErrRet;
}

S32 DVBPlayer_PVR_GetPlayFileInfo(DVBPlayer_PVR_FILE_ATTR_t *pPvrFileAttr)
{
    S32 ErrRet = PV_SUCCESS;
    PVR_FILE_ATTR_t tPvrFileAttr;

    memset( &tPvrFileAttr, 0, sizeof(PVR_FILE_ATTR_t) );
    ErrRet = DRV_PVR_PlayGetFileAttr(&tPvrFileAttr);
    if( PV_SUCCESS == ErrRet )
    {
        pPvrFileAttr->u32IdxType = tPvrFileAttr.enIdxType;
        pPvrFileAttr->u32FrameNum = tPvrFileAttr.u32FrameNum;
        pPvrFileAttr->u32StartTimeInMs = tPvrFileAttr.u32StartTimeInMs;
        pPvrFileAttr->u32EndTimeInMs = tPvrFileAttr.u32EndTimeInMs;
        pPvrFileAttr->u64ValidSizeInByte = tPvrFileAttr.u64ValidSizeInByte;
    }
    return ErrRet;
}

S32 DVBPlayer_PVR_GetPlayStatus(DVBPlayer_PVR_PLAY_STATUS_t * pPvrPlayStatus)
{
    S32 ErrRet = PV_SUCCESS;
    PVR_PLAY_STATUS_t tPvrPlayStatus;

    memset( &tPvrPlayStatus, 0, sizeof(PVR_PLAY_STATUS_t) );
    ErrRet = DRV_PVR_PlayGetStatus(&tPvrPlayStatus);
    if( PV_SUCCESS == ErrRet )
    {
        pPvrPlayStatus->u32State = tPvrPlayStatus.enState;
        pPvrPlayStatus->u32Speed = tPvrPlayStatus.enSpeed;
        pPvrPlayStatus->u32CurPlayFrame = tPvrPlayStatus.u32CurPlayFrame;
        pPvrPlayStatus->u32CurPlayTimeInMs = tPvrPlayStatus.u32CurPlayTimeInMs;
        pPvrPlayStatus->u64CurPlayPos = tPvrPlayStatus.u64CurPlayPos;
    }

    return ErrRet;
}

S32 DVBPlayer_PVR_GetFileUserData( C8 *pName, C8 *pData, U32 *pDataLen )
{
    S32 ErrRet = PV_SUCCESS;

    ErrRet = DRV_PVR_GetUserData( pName,  pData, pDataLen );
    if( PV_SUCCESS != ErrRet )
    {
        PBIDEBUG("[Error]");
    }

    return ErrRet;
}

S32 DVBPlayer_PVR_SetFileUserData( C8 *pName, C8 *pData, U32 DataLen )
{
    S32 ErrRet = PV_SUCCESS;

    ErrRet = DRV_PVR_SetUserData( pName, pData, DataLen );
    if( PV_SUCCESS != ErrRet )
    {
        PBIDEBUG("[Error]");
    }
    return ErrRet;
}
