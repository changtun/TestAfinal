/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : DVBPlayer ver 2.0
* File name   : dvbplayer_proc.c
* Description : Implementataion of action process and status monitor.
*
* History :
*   2012-05-23 ZYJ
*       Initial Version.
*******************************************************************************/

#include <string.h>
#include "pvddefs.h"
#include "pbitrace.h"

#include "osapi.h"
#include "drv_tuner.h"
#include "background.h"

#include "dvbplayer.h"
#include "dvbplayer_proc.h"
#include "dvbplayer_av.h"
#include "dvbplayer_pvr.h"
#include "dvbplayer_callbackandpmt.h"
#include "tmsgqueue.h"
#include "tlock.h"

#include "systime.h"
#include "epg.h"
#ifdef __cplusplus
extern "C" {
#endif


#if 1 /* 临时原来DVBPlayer_Param_t 结构体中  */
    U32     semPMT_CallBack = 0xFFFFFFFF;
#endif
static U32 g_semPatCallBack = 0xFFFFFFFF;

    DVBPlayer_Status_t  g_tStatus;
    TLock               g_tPlayer_Lock;

    static MsgQueue     g_tMsg_Queue_Switch;

static U32          g_uDVBPlayerTask = 0xFFFFFFFF;
static U32 g_PlayerTaskFlag = 0;
    static const int    DVBPlayer_Priority = 5;
    static const int    DVBPlayer_Size = 1024 * 16;

    static U32 g_PlayerCountFlag = 0;
    static DVBPlayer_CA_Fun_t g_tCaFun;

    static int g_PlayerInitFlag = 0;
    static U32 g_AudioFrameErrorCount = 0;
    static U32 g_VideoFrameErrorCount = 0;
static U32 g_PmtTimeOutErrorCount = 0;
static U32 g_PmtCheckTimeOutCount = 0;


    static DVBPlayer_Event_Type_e g_tsignal_event =eDVBPLAYER_EVENT_TUNER_LOCK;
    static DVBPlayer_Event_Type_e g_program_event =eDVBPLAYER_EVENT_PMT_TIMEOUT;
    static int g_tuner_retry = 0;
    static int g_pmt_retry = 0;

    static U32 g_TimePvrFlag = 0; /* 1: Time Pvr mode  0: dvb mode */
static int g_tuner_times =0;

typedef struct CAPMT_QueryData_tag
{
	unsigned short	frequency;
	unsigned short	network_id;
	unsigned short	ts_id;
	unsigned short	service_id;
}CAPMT_QueryData;

static CAPMT_QueryData QueryData;

    void Reset_Signal_Status(void)
    {
        g_tsignal_event =eDVBPLAYER_EVENT_TUNER_LOCK;
        g_program_event =eDVBPLAYER_EVENT_PMT_TIMEOUT;
    }
    /*****************************************************************************
     Prototype    : GetPlayerStatus
     Description  : Get g_tStatus
     Input        : DVBPlayer_Status_t  *pStatus
     Output       : None
     Return Value :
     Calls        :
     Called By    :

      History        :
      1.Date         : 2012/9/24
        Author       : zxguan
        Modification : Created function

    *****************************************************************************/
    S32 GetPlayerStatus(DVBPlayer_Status_t  *pStatus )
    {
        if( NULL == pStatus )
        {
            return 1;
        }

        memcpy( pStatus, &g_tStatus, sizeof(DVBPlayer_Status_t) );

        return 0;
    }


    /******************************************************************************
    * Function : DvbPlayerMessageSend
    * parameters :
    *		IN DVBPlayer_Action_Type_e, 消息类型
    *       IN DVBPlayer_Action_Param_u, 消息数据
    *       OUT void *
    * Return :
    *		void
    * Description :
    *		给 g_tMsg_Queue_Switch 发送消息
    *
    * Author : zxguan 2012/05/28
    ******************************************************************************/
    void DvbPlayerMessageSend(DVBPlayer_Action_Type_e type,DVBPlayer_Action_Param_u param1, void* param2 )
    {
        MsgQueue_post( &g_tMsg_Queue_Switch, type, param1, param2 );
    }

	/******************************************************************************
    * Function : DvbPlayerMessageClean
    * parameters :
    *		IN DVBPlayer_Action_Type_e, 消息类型
    *       OUT void *
    * Return :
    *		void
    * Description :
    *		清除 g_tMsg_Queue_Switch 的消息
    *
    * Author : zshang 2013/03/5
    ******************************************************************************/
    void DvbPlayerMessageClean(DVBPlayer_Action_Type_e type )
    {
        MsgQueue_clean( &g_tMsg_Queue_Switch, type );
    }    

    /******************************************************************************
    * Function : PmtCallbackFunc
    * parameters :
    *		IN SYS_Table_Msg_t,新的PMT数据
    * Return :
    *		void
    * Description :
    *		在backgroud_pmt.c中判断pmt的version有更新 则将更新的数据callback
    *
    * Author : zbl 2007/12/13
    ******************************************************************************/
    void _pmt_callback_func(SYS_Table_Msg_t message)
    {
        PSISI_Table_t * ptNewPMT = NULL;
        PMT_Section_t * ptPMTSect = NULL;
        int				pes_flag =0,find_history_flag =0,set_audio_flag =0;
        S32			ret =0;
    DVBPlayer_Event_t caEvent,caPidEvent;
    U16   	 temp_avpid[2];
        ptNewPMT = PSISI_Alloc_Table(message.pid, message.buffer, message.section_length);
        if (NULL == ptNewPMT)
        {
            if((NULL != message.buffer) && (NULL != message.pfFreeBuffer))
            {
                message.pfFreeBuffer(message.buffer);
            }
            OS_SemSignal(semPMT_CallBack);
            return;
        }

        ret = PSISI_Parse_PMT_Section(ptNewPMT, message.buffer, message.section_length);
        if(ret !=0)
        {
            if((NULL != message.buffer) && (NULL != message.pfFreeBuffer))
            {
                message.pfFreeBuffer(message.buffer);
            }
            OS_SemSignal(semPMT_CallBack);
            return;
        }
        if(g_tStatus.tProgStatus.u16ServiceId != ptNewPMT->u16ExtendId)
        {
            PSISI_Free_Table(ptNewPMT);
            if((NULL != message.buffer) && (NULL != message.pfFreeBuffer))
            {
                message.pfFreeBuffer(message.buffer);
            }
            OS_SemSignal(semPMT_CallBack);
            return;
        }

        if ((0 == ptNewPMT->u16SectionNum) || (NULL == ptNewPMT->ppSection))
        {
        pbiinfo("invalid pmt table.\n");
            PSISI_Free_Table(ptNewPMT);
            if((NULL != message.buffer) && (NULL != message.pfFreeBuffer))
            {
                message.pfFreeBuffer(message.buffer);
            }
            OS_SemSignal(semPMT_CallBack);

            /* 无效的 PMT 表 */
            return;
        }

        ptPMTSect = (PMT_Section_t *)(ptNewPMT->ppSection[0]);
        /* 既无视频又无音频, 不是可播放的节目. 等同于没有 PMT */
        if ((NULL == ptPMTSect->ptVideo)
                && ((NULL == ptPMTSect->ptAudio) || (0 == ptPMTSect->u16AudioNum)) )
        {
            pbiinfo("no video & audio component, can not play.\n");
            PSISI_Free_Table(ptNewPMT);
            if((NULL != message.buffer) && (NULL != message.pfFreeBuffer))
            {
                message.pfFreeBuffer(message.buffer);
            }
            OS_SemSignal(semPMT_CallBack);
            return;
        }

        if (ptPMTSect->ptVideo != NULL)
        {
            g_tStatus.eVideoCtrlFlag = eDVBPLAYER_ENABLED;
            g_tStatus.tProgStatus.u16VideoPid = ptPMTSect->ptVideo->u16ElementaryPid;
            g_tStatus.tProgStatus.u8VideoFmt = ptPMTSect->ptVideo->u8StreamType;
        pbiinfo("%s %d videopid%x\n",__FUNCTION__,__LINE__,g_tStatus.tProgStatus.u16VideoPid);
        }
        else
        {
            g_tStatus.eVideoCtrlFlag = eDVBPLAYER_DISABLED;
        }
        pbiinfo("%s   u16AudioNum %d-------COMPONIT num %d \n",__FUNCTION__,ptPMTSect->u16AudioNum ,ptPMTSect->u16ComponentNum);
        if ((ptPMTSect->ptAudio != NULL) && (ptPMTSect->u16AudioNum != 0))
        {
            g_tStatus.eAudioCtrlFlag = eDVBPLAYER_ENABLED;
            g_tStatus.tProgStatus.u16AudioPid = ptPMTSect->ptAudio[0].u16ElementaryPid;
            g_tStatus.tProgStatus.u8AudioFmt = ptPMTSect->ptAudio[0].u8StreamType;
            set_audio_flag =1;
        pbiinfo("%s %d AUDIOpid%x\n",__FUNCTION__,__LINE__,g_tStatus.tProgStatus.u16AudioPid);
        }
        else
        {
            if((ptPMTSect->u16ComponentNum!=0 )&&(ptPMTSect->ptComponent!=NULL))/*解析audio部分*/
            {
                int  mm =0,nn=0;
                for(mm =0; mm<ptPMTSect->u16ComponentNum; mm++ )
                {
                    pbiinfo("------------------  STREAM_TYPE_PRIVATE_PES_DATA  pid%x ---------------",
                            ptPMTSect->ptComponent[mm].u16ElementaryPid);
                    if(ptPMTSect->ptComponent[mm].u8StreamType ==STREAM_TYPE_PRIVATE_PES_DATA)
                    {
                        pbiinfo("---------------- DescNum%d   --------------------",ptPMTSect->ptComponent[mm].u16OtherDescNum);
                        if((ptPMTSect->ptComponent[mm].u16OtherDescNum >0)
                                &&(ptPMTSect->ptComponent[mm].ptDesc!=NULL))
                        {
                            for(nn=0; nn<ptPMTSect->ptComponent[mm].u16OtherDescNum; nn++)
                            {
                                pbiinfo("--------------- OtherDescNum tage%x ---  nn%d   mm%d------------------- ",ptPMTSect->ptComponent[mm].ptDesc[nn].u8Tag,nn,mm);
                                if(ptPMTSect->ptComponent[mm].ptDesc[nn].u8Tag ==ENHANCE_AC3_DESCRIPTOR_TAG)
                                {
                                    g_tStatus.eAudioCtrlFlag = eDVBPLAYER_ENABLED;
                                    g_tStatus.tProgStatus.u16AudioPid = ptPMTSect->ptComponent[mm].u16ElementaryPid;
                                    g_tStatus.tProgStatus.u8AudioFmt = ENHANCE_AC3_DESCRIPTOR_TAG;
                                    set_audio_flag =1;
                                    pbiinfo("%s  %d ====find AC3+=+++  %d  type %d\n",__FILE__,__LINE__,
                                            g_tStatus.tProgStatus.u16AudioPid,g_tStatus.tProgStatus.u8AudioFmt);
                                    break;
                                }
                            }
                        }
                    }
                }
                if(set_audio_flag ==0)
                {
                    g_tStatus.eAudioCtrlFlag= eDVBPLAYER_DISABLED;
                    pbiinfo("++++++++++++++  Not find audio  ++++++++++++++++++++++++++++++");
                }
            }
            else
            {
                g_tStatus.eAudioCtrlFlag= eDVBPLAYER_DISABLED;
                pbiinfo("++++++++++++++  Not find audio  ++++++++++++++++++++++++++++++");
            }
        }
        if((g_tStatus.tProgStatus.u16History_audioPID >0)&&(g_tStatus.tProgStatus.u16History_audioPID<0x1fff))
        {
            if ((ptPMTSect->ptAudio != NULL) && (ptPMTSect->u16AudioNum != 0))
            {
                int kk =0;
                for(kk=0; kk<ptPMTSect->u16AudioNum; kk++)
                {
                    if( ptPMTSect->ptAudio[kk].u16ElementaryPid == g_tStatus.tProgStatus.u16History_audioPID)
                    {
                        pbiinfo("%s-------play audio in history pid %d ",__FUNCTION__,ptPMTSect->ptAudio[kk].u16ElementaryPid );
                        g_tStatus.tProgStatus.u16AudioPid =ptPMTSect->ptAudio[kk].u16ElementaryPid;
                        g_tStatus.tProgStatus.u8AudioFmt = ptPMTSect->ptAudio[kk].u8StreamType;
                        find_history_flag =1;
                        break;
                    }
                }
            }
            if(find_history_flag ==0)/*继续在其他compont中寻找*/
            {
                if((ptPMTSect->u16ComponentNum!=0 )&&(ptPMTSect->ptComponent!=NULL))/*解析audio部分*/
                {
                    int  mm =0,nn=0;
                    for(mm =0; mm<ptPMTSect->u16ComponentNum; mm++ )
                    {
                        if(ptPMTSect->ptComponent[mm].u8StreamType ==STREAM_TYPE_PRIVATE_PES_DATA)
                        {
                            if((ptPMTSect->ptComponent[mm].u16OtherDescNum >0)
                                    &&(ptPMTSect->ptComponent[mm].ptDesc!=NULL))
                            {
                                for(nn=0; nn<ptPMTSect->ptComponent[mm].u16OtherDescNum; nn++)
                                {
                                    if(ptPMTSect->ptComponent[mm].ptDesc[nn].u8Tag ==ENHANCE_AC3_DESCRIPTOR_TAG)
                                    {
                                        pbiinfo("line %d +++++++++++ ac3+  pid %d  ",__LINE__,ptPMTSect->ptComponent[mm].u16ElementaryPid);
                                        if( ptPMTSect->ptComponent[mm].u16ElementaryPid== g_tStatus.tProgStatus.u16History_audioPID)
                                        {
                                            pbiinfo("%s-------play audio in history pid %d ",__FUNCTION__,ptPMTSect->ptComponent[mm].u16ElementaryPid );
                                            g_tStatus.tProgStatus.u16AudioPid =ptPMTSect->ptComponent[mm].u16ElementaryPid;
                                            g_tStatus.tProgStatus.u8AudioFmt =ENHANCE_AC3_DESCRIPTOR_TAG;
                                            find_history_flag =1;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        pbiinfo("ptPMTSect->u16AudioNum------[%d]\n",ptPMTSect->u16AudioNum);
#if 1
        if(g_tStatus.pfnNotifyFunc!=NULL)
        {
            DVBPlayer_Event_t tEvent;
            memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
            tEvent.eEventType = eDVBPLAYER_EVENT_AUDIO_INFOS;
            tEvent.u32EventLen =ptPMTSect->u16AudioNum+ptPMTSect->u16ComponentNum;
            tEvent.pEventData = (void *)(ptPMTSect);
            g_tStatus.pfnNotifyFunc( tEvent );
            pbiinfo("----------------------------------------send pmt infos to jni-----------------\n");
        }
#endif
		g_tStatus.tProgStatus.ecamode =ePROGRAM_FREE;

        if(ptPMTSect->u16CA_DescNum>0)
        {
            g_tStatus.tProgStatus.ecamode =ePROGRAM_PAY;
        }
        else
        {
            if(ptPMTSect->ptVideo != NULL)
            {
                if(ptPMTSect->ptVideo->u16CA_DescNum>0)
                {
                    g_tStatus.tProgStatus.ecamode =ePROGRAM_PAY;
                }
            }

			if(g_tStatus.tProgStatus.ecamode ==ePROGRAM_FREE)
            {
                int jj=0;
                if ((ptPMTSect->ptAudio != NULL) && (ptPMTSect->u16AudioNum != 0))
                {
                    for(jj=0; jj<ptPMTSect->u16AudioNum; jj++)
                    {
                        if(ptPMTSect->ptAudio[jj].u16CA_DescNum>0 )
                        {
                            g_tStatus.tProgStatus.ecamode =ePROGRAM_PAY;
                            break;
                        }
                    }
                    if(jj ==ptPMTSect->u16AudioNum)
                    {
                        g_tStatus.tProgStatus.ecamode =ePROGRAM_FREE;
                    }
                }
                else
                {
                    g_tStatus.tProgStatus.ecamode =ePROGRAM_FREE;
                }
            }
        }

//    if( 0 != ptPMTSect->u16CA_DescNum )
//    {
//        ptPMTSect->ptCA_Desc[0].u16CASystemId;
//    }
		memset( &caEvent, 0, sizeof(DVBPlayer_Event_t) );
        caEvent.eEventType = eDVBPLAYER_EVENT_CA_MODE;
		caEvent.u32EventLen = g_tStatus.tProgStatus.ecamode;
        if( NULL != g_tStatus.pfnNotifyFunc )
        {
            g_tStatus.pfnNotifyFunc( caEvent );
            pbiinfo("\n_pmt_callback_func\n\n", __FUNCTION__,__LINE__);
        }
        else
        {
            pbiinfo("\n n_pmt_callback_func is null\n\n");
        }


        g_tStatus.tProgStatus.u16PCRPid = ptPMTSect->u16PCRPid;

        PSISI_Free_Table(g_tStatus.tProgStatus.ptPMTTable);
        g_tStatus.tProgStatus.ptPMTTable = ptNewPMT;
        g_tStatus.tProgStatus.ePMTStatus = eDVBPLAYER_PMT_RECEIVED;

//AMLOGIC 不用传递这组参数
/*    memset( &caPidEvent, 0, sizeof(DVBPlayer_Event_t) );
    caPidEvent.eEventType = 7;
    temp_avpid[0] =g_tStatus.tProgStatus.u16VideoPid;//liqian 用这个变量传递给ca videopid
    temp_avpid[1] = g_tStatus.tProgStatus.u16AudioPid;//liqian 用这个变量传递给ca videopid
    caPidEvent.pEventData  =&temp_avpid;
    pbiinfo("send PID to cas vidiopid[%x] audiopid[%x] ================\n",temp_avpid[0],temp_avpid[1]);
    CA_DVBPlay_CallBack( caPidEvent );
 */

        pbiinfo("\n send pmt to trcas ================\n");
  #if 1
        DVBPlayer_Event_t tEvent;
        QueryData.frequency = g_tStatus.uTunerParam.tCable.u32Freq/1000;
        QueryData.network_id = g_tStatus.tProgStatus.u16NetId;
        QueryData.service_id = g_tStatus.tProgStatus.u16ServiceId;
        QueryData.ts_id = g_tStatus.tProgStatus.u16TsId;
       pbiinfo("\n send data fre = %d,network_id = %d,service_id = %d,ts_id = %d\n",QueryData.frequency,QueryData.network_id,QueryData.service_id,QueryData.ts_id); 
       memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
        tEvent.eEventType = eDVBPLAYER_EVENT_CA_CHANNEL_INFO;
        tEvent.pEventData = &QueryData;
        CA_DVBPlay_CallBack( tEvent );
  #endif
#if 1
  //      DVBPlayer_Event_t tEvent;
        memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
        tEvent.eEventType = 2;
        tEvent.pEventData = message.buffer;
        CA_DVBPlay_CallBack( tEvent );
#endif
        pbiinfo("\n send pmt to trcas ================\n");
        if((NULL != message.buffer) && (NULL != message.pfFreeBuffer))
        {
            message.pfFreeBuffer(message.buffer);
        }
        OS_SemSignal(semPMT_CallBack);
    }

static void _Pat_CheckPmtCallBack(SYS_Table_Msg_t message)
{
#if 1
    S32 ret = 0;
    U32 iFor = 0;
    PSISI_Table_t * ptNewPAT = NULL;
    PAT_Section_t * ptPATSect = NULL;
    DVBPlayer_Event_t playerEvent;
    DVBPlayer_Update_PMT_t tNewPmtInfo;
    
    if( eDVBPLAYER_PMT_CHECK != g_tStatus.tProgStatus.ePMTStatus )
    {
        goto OUT_PAT_CHECK_PMT_FREE_MSG;
    }

    PBIDEBUG("-----PAT Start----------------");
    ptNewPAT = PSISI_Alloc_Table(message.pid, message.buffer, message.section_length);
    if (NULL == ptNewPAT)
    {
        pbiinfo("[%s %d] ptNewPat is NULL.\n",DEBUG_LOG);
        goto OUT_PAT_CHECK_PMT_FREE_MSG;
    }

    ret = PSISI_Parse_PAT_Section(ptNewPAT, message.buffer, message.section_length);
    if(ret !=0)
    {
        pbiinfo("[%s %d] parse error.ret = %d.\n",DEBUG_LOG, ret);
        goto OUT_PAT_CHECK_PMT_FREE_MSG;
    }

    if ((0 == ptNewPAT->u16SectionNum) || (NULL == ptNewPAT->ppSection))
    {
        pbiinfo("invalid pat table.\n");
        goto OUT_PAT_CHECK_PMT_FREE_TABLE;
    }

    ptPATSect = (PAT_Section_t *)(ptNewPAT->ppSection[0]);

    if( g_tStatus.tProgStatus.u16TsId != ptPATSect->u16TS_Id )
    {
        pbiinfo("[%s %d] Pat TsId is error. Tsid 0x%x V 0x%x.\n", \
            DEBUG_LOG, g_tStatus.tProgStatus.u16TsId, ptPATSect->u16TS_Id );
        goto OUT_PAT_CHECK_PMT_FREE_TABLE;
    }

    for( iFor = 0; iFor < ptPATSect->u16ProgNum; iFor++ )
    {
    	pbiinfo(" service id = %d,program number = %d\n",g_tStatus.tProgStatus.u16ServiceId,ptPATSect->ptProgram[iFor].u16ProgramNumber);
        if( g_tStatus.tProgStatus.u16ServiceId == ptPATSect->ptProgram[iFor].u16ProgramNumber )
        {
            if( g_tStatus.tProgStatus.u16PMTPid == ptPATSect->ptProgram[iFor].u16PMTPid )
            {
                /* The Pmt Pid not change */
                DVBPlayer_Start_PMT(g_tStatus.tProgStatus.u16PMTPid, g_tStatus.tProgStatus.u16ServiceId);
                g_tStatus.tProgStatus.ePMTStatus = eDVBPLAYER_PMT_WAITING;
                pbiinfo("[%s %d] Pmt Pid Not Change.\n",DEBUG_LOG);
                goto OUT_PAT_CHECK_PMT_FREE_TABLE;
            }
            else
            {
                /* The PMT Pid was Changed */
                g_tStatus.tProgStatus.u16PMTPid = ptPATSect->ptProgram[iFor].u16PMTPid;
                DVBPlayer_Start_PMT(g_tStatus.tProgStatus.u16PMTPid, g_tStatus.tProgStatus.u16ServiceId);
                g_tStatus.tProgStatus.ePMTStatus = eDVBPLAYER_PMT_WAITING;
                pbiinfo("[%s %d] The ServId = 0x%x. Pmt Pid Changed.\n",DEBUG_LOG, ptPATSect->ptProgram[iFor].u16ProgramNumber);

                /* Update Pmt Info In Databases */
                if( NULL != g_tStatus.pfnNotifyFunc )
                {
                    memset( &playerEvent, 0, sizeof(DVBPlayer_Event_t) );
                    memset( &tNewPmtInfo, 0, sizeof(DVBPlayer_Update_PMT_t) );

                    tNewPmtInfo.u16TsId = g_tStatus.tProgStatus.u16TsId;
                    tNewPmtInfo.u16ServiceId = g_tStatus.tProgStatus.u16ServiceId;
                    tNewPmtInfo.u16PmtPid = g_tStatus.tProgStatus.u16PMTPid;
                    
                    playerEvent.eEventType = eDVBPLAYER_EVENT_UPDATE_PMT;
                    playerEvent.u32EventLen = sizeof(DVBPlayer_Update_PMT_t);
                    playerEvent.pEventData = &tNewPmtInfo;
                    g_tStatus.pfnNotifyFunc( playerEvent );

                    pbiinfo("\nPlayer video OK [%s--%d]\n\n", __FUNCTION__,__LINE__);
                }
                
                goto OUT_PAT_CHECK_PMT_FREE_TABLE;
            }
        }
    }

    pbiinfo("[%s %d] Pat error.\n", DEBUG_LOG);
OUT_PAT_CHECK_PMT_FREE_TABLE:
    //PBIDEBUG("Free Table.");
    PSISI_Free_Table(ptNewPAT);
OUT_PAT_CHECK_PMT_FREE_MSG:    
    if((NULL != message.buffer) && (NULL != message.pfFreeBuffer))
    {
        //PBIDEBUG("Free Buffer.");
        message.pfFreeBuffer(message.buffer);
    }
    OS_SemSignal(g_semPatCallBack);
 #endif   
}

void BackGroundPatInit()
{
    int error = 0;
    SYS_Table_InitParam_t init_parameter;
   // SYS_Table_Param_t bgfilterdata;
        
    OS_SemCreate( &g_semPatCallBack, "g_semPatCallBack", 0, 0 );
    init_parameter.p_semaphore = g_semPatCallBack;
    init_parameter.callback = _Pat_CheckPmtCallBack;

    error = SYS_BackGround_TableInit(SYS_BG_PAT,init_parameter);
	if (error)
    {   
		pbierror("Background_Pat_Init error !!!!!!!\n");
    }
    else
	{	
	    pbiinfo("Background_Pat_Init ok !!!!!!!\n");
    }
   // bgfilterdata.timeout = ONESEC;
   // SYS_BackGround_TableStart(SYS_BG_PAT, bgfilterdata);
   // SYS_BackGround_TablePause(SYS_BG_PAT);
}

static void TunerReset(void)
{
    ts_src_info_t ptTunerInfo;
    DVBCore_Cab_Desc_t *ptCable=NULL;
    if(g_tuner_times > 20)
    {
        ptCable = (DVBCore_Cab_Desc_t *)(&(g_tStatus.uTunerParam.tCable));
        ptTunerInfo.MediumType = MEDIUM_TYPE_DVBC;
        ptTunerInfo.u.Cable.FrequencyKHertz = ptCable->u32Freq;
        ptTunerInfo.u.Cable.SymbolRateSPS = ptCable->u32SymbRate;
        ptTunerInfo.u.Cable.Modulation = ptCable->eMod +5;		
	 ptTunerInfo.u.Cable.Bandwidth = ptCable->Bandwidth;
	 ptTunerInfo.u.Cable.Bandwidth = GetTunerBandWidth();
        DRV_Tuner_SetFrequency(0, &ptTunerInfo, 0xFFFFFFFF);
        PBIDEBUG("TunerReset.");
        g_tuner_times =0;
    }
    g_tuner_times++;
}

    /*
    0 - 新的 tuner 参数与当前相同, 不需要重新设置 tuner.
    1 - 新的 tuner 参数与当前不同, 需要重新设置 tuner.
    */
    static int _prepare_tuner_param(DVBCore_Tuner_Desc_u *puNewParam,
                                    U8 * pu8TunerId, ts_src_info_t * ptTunerInfo)
    {

#ifdef DVBS_SUPPORT
        if (eDVBCORE_SIGNAL_SOURCE_DVBS == puNewParam->eSignalSource)
        {
            DVBCore_Sat_Desc_t  *ptSatellite = NULL;

            if (0 == memcmp(puNewParam, &(g_tStatus.uTunerParam), sizeof(DVBCore_Sat_Desc_t)))
            {
                pbierror("[%d %s] memcmp error!\n", DEBUG_LOG);
                return 0;
            }

            ptSatellite = (DVBCore_Sat_Desc_t *)(puNewParam);
            *pu8TunerId = ptSatellite->u8TunerId;

            return 1;
        }
#endif

#ifdef DVBS2_SUPPORT
        if (eDVBCORE_SIGNAL_SOURCE_DVBS2 == puNewParam->eSignalSource)
        {
        }
#endif

#ifdef DVBC_SUPPORT
        if (eDVBCORE_SIGNAL_SOURCE_DVBC == puNewParam->eSignalSource)
        {
            DVBCore_Cab_Desc_t  *ptCable = NULL;

            if (0 == memcmp(puNewParam, &(g_tStatus.uTunerParam), sizeof(DVBCore_Cab_Desc_t)))
            {
                pbierror("[%s %d] memcmp error!\n", DEBUG_LOG);
                return 0;
            }

            ptCable = (DVBCore_Cab_Desc_t *)(puNewParam);
            *pu8TunerId = ptCable->u8TunerId;
            ptTunerInfo->MediumType = MEDIUM_TYPE_DVBC;
            ptTunerInfo->u.Cable.FrequencyKHertz = ptCable->u32Freq;
            ptTunerInfo->u.Cable.SymbolRateSPS = ptCable->u32SymbRate;
            ptTunerInfo->u.Cable.Modulation = ptCable->eMod + 5;
	     ptTunerInfo->u.Cable.Bandwidth = ptCable->Bandwidth;
            return 1;
        }
#endif

#ifdef DVBT_SUPPORT
        if (eDVBCORE_SIGNAL_SOURCE_DVBT == puNewParam->eSignalSource)
        {
            DVBCore_Ter_Desc_t  *ptTerrestrial = NULL;

            if (0 == memcmp(puNewParam, &(g_tStatus.uTunerParam), sizeof(DVBCore_Ter_Desc_t)))
            {
                pbierror("[%d %s] memcmp error!\n", DEBUG_LOG);
                return 0;
            }

            ptTerrestrial = (DVBCore_Ter_Desc_t *)(puNewParam);
            *pu8TunerId = ptTerrestrial->u8TunerId;

            return 1;
        }
#endif

#ifdef DMBT_SUPPORT
        if (eDVBCORE_SIGNAL_SOURCE_DMBT == puNewParam->eSignalSource)
        {
        }
#endif

#ifdef ANALOGUE_SUPPORT
        if (eDVBCORE_SIGNAL_SOURCE_ANALOG == puNewParam->eSignalSource)
        {
            DVBCore_Ana_Desc_t  *ptAnalogue = NULL;

            if (0 == memcmp(puNewParam, &(g_tStatus.uTunerParam), sizeof(DVBCore_Ana_Desc_t)))
            {
                pbierror("[%d %s] memcmp error!\n", DEBUG_LOG);
                return 0;
            }

            ptAnalogue = (DVBCore_Ana_Desc_t *)(puNewParam);
            *pu8TunerId = ptAnalogue->u8TunerId;

            return 1;
        }
#endif

        /* unknown signal source type */
        return -1;
    }

int g_FastTunerLockFlag = 0;
    static S32 _prepare_play( DVBPlayer_Play_Param_t * ptPlayParam )
    {
        U8              u8TunerId;
        ts_src_info_t   tTuner_Info;

        if (NULL == ptPlayParam)
        {
            pbierror("[%s %d] _prepare_play param is NULL return.\n", DEBUG_LOG);
            return 0;
        }

        g_PlayerCountFlag = 0;
        /*
            if (0 != DVBPlayer_Stop_TTX())
            {
                pbiinfo("\n-----stop teletext error!-----\n");
            }

            if (0 != DVBPlayer_Stop_Subtitle())
            {
                pbiinfo("\n-----stop subtitle error!-----\n");
            }
        */

        if ((eDVBPLAYER_ENABLED == g_tStatus.eVideoCtrlFlag)
                && (eDVBPLAYER_PLAY == g_tStatus.tProgStatus.eVideoStatus))
        {

            if (0 != DVBPlayer_Stop_Video())
            {
                pbierror("\n-----stop video error!-----\n");
                return 1;
            }
            g_tStatus.tProgStatus.eVideoStatus = eDVBPLAYER_STOP;
        }
		else
		{
			DVBPlayer_Clear_BroadcastBG();
		}

        if ((eDVBPLAYER_ENABLED == g_tStatus.eAudioCtrlFlag)
                && (eDVBPLAYER_PLAY == g_tStatus.tProgStatus.eAudioStatus))
        {
            if (0 != DVBPlayer_Stop_Audio())
            {
                pbierror("\n-----stop audio error!-----\n");
                return 1;
            }
            g_tStatus.tProgStatus.eAudioStatus = eDVBPLAYER_STOP;
        }

        if ((eDVBPLAYER_SYNC_REF_PCR == g_tStatus.eSyncType)
                && (eDVBPLAYER_PLAY == g_tStatus.tProgStatus.ePCRStatus))
        {
            if (0 != DVBPlayer_Stop_PCR())
            {
                pbierror("\n-----stop pcr error!-----\n");
                return 1;
            }
            g_tStatus.tProgStatus.ePCRStatus = eDVBPLAYER_STOP;
        }

        if (g_tStatus.tProgStatus.ePMTStatus != eDVBPLAYER_PMT_STOP)
        {
            if (0 != DVBPlayer_Stop_PMT())
            {
                pbierror("\n-----stop pmt error!-----\n");
                return 1;
            }
            g_tStatus.tProgStatus.ePMTStatus = eDVBPLAYER_PMT_STOP;
        }

        g_tStatus.eRunningStatus = eDVBPLAYER_RUNNING_STATUS_PREPARE;
        g_tStatus.tProgStatus.ecamode =ePROGRAM_PAY;
        if (eDVBPLAYER_PROG_TYPE_SERVICE == ptPlayParam->uProgParam.eProgType)
        {
            DVBPlayer_Serv_Param_t  *ptServ = NULL;

            ptServ = (DVBPlayer_Serv_Param_t *)(&(ptPlayParam->uProgParam));
            g_tStatus.tProgStatus.u16ServiceId = ptServ->u16ServId;
            g_tStatus.tProgStatus.u16PMTPid = ptServ->u16PMTPid;
            g_tStatus.tProgStatus.u16TsId = ptServ->u16TsId;
            g_tStatus.tProgStatus.u16NetId = ptServ->u16NetId;
            g_tStatus.tProgStatus.eServLockFlag = ptServ->eServLockFlag;
            g_tStatus.tProgStatus.u16PCRPid = INVALID_PID;
            g_tStatus.tProgStatus.u16VideoPid = INVALID_PID;
            g_tStatus.tProgStatus.u16AudioPid = INVALID_PID;
            g_tStatus.tProgStatus.u8VideoFmt = 0xFF;
            g_tStatus.tProgStatus.u8AudioFmt = 0xFF;
            g_tStatus.tProgStatus.u16History_audioPID =ptServ->u16historyAudioPid;
        pbiinfo("[%s %d] historyaudiopid%d",DEBUG_LOG,g_tStatus.tProgStatus.u16History_audioPID);
        }
        else
        {
            DVBPlayer_AVPid_t * ptAVPids = NULL;

            ptAVPids = (DVBPlayer_AVPid_t *)(&(ptPlayParam->uProgParam));
            g_tStatus.tProgStatus.u16PCRPid = ptAVPids->u16PCRPid;
            g_tStatus.tProgStatus.u16VideoPid = ptAVPids->u16VideoPid;
            g_tStatus.tProgStatus.u16AudioPid = ptAVPids->u16AudioPid;
            g_tStatus.tProgStatus.u8VideoFmt = ptAVPids->u8VideoFmt;
            g_tStatus.tProgStatus.u8AudioFmt = ptAVPids->u8AudioFmt;
            if (INVALID_PID != ptAVPids->u16VideoPid)
            {
                g_tStatus.eVideoCtrlFlag = eDVBPLAYER_ENABLED;
            }
            else
            {
                g_tStatus.eVideoCtrlFlag = eDVBPLAYER_DISABLED;
            }

            if (INVALID_PID != ptAVPids->u16AudioPid)
            {
                g_tStatus.eAudioCtrlFlag = eDVBPLAYER_ENABLED;
            }
            else
            {
                g_tStatus.eAudioCtrlFlag = eDVBPLAYER_DISABLED;
            }
        }

        /*
            if (0 != DVBPlayer_Stop_STILL(g_tState.tRadioBackGround))
            {
                pbiinfo("\n-----stop still error!-----\n");
            }
        */
        Reset_Signal_Status();

        memset(&tTuner_Info, 0, sizeof(tTuner_Info));

        if (1 == _prepare_tuner_param(&(ptPlayParam->uTunerParam), &u8TunerId, &tTuner_Info))
        {
        	tTuner_Info.u.Cable.Bandwidth = GetTunerBandWidth();
		DRV_Tuner_SetFrequency(u8TunerId, &tTuner_Info, 0);
		g_tStatus.eTunerStatus = eDVBPLAYER_TUNER_UNLOCK;
		g_tStatus.uTunerParam.tCable.u32Freq =tTuner_Info.u.Cable.FrequencyKHertz;
		g_tStatus.uTunerParam.tCable.u32SymbRate =tTuner_Info.u.Cable.SymbolRateSPS;
		g_tStatus.uTunerParam.tCable.eMod =tTuner_Info.u.Cable.Modulation -5;
		g_tStatus.uTunerParam.tCable.Bandwidth = tTuner_Info.u.Cable.Bandwidth;
            {
                //g_PlayerCountFlag = 1;
                U8 ucCount = 0;
                if( 0 == g_FastTunerLockFlag )
                {
                    
                    while( ucCount < 20 )
                    {
                        if (DRV_Tuner_GetLockStatus(0) != eTun_Lock)
                        {
                            ucCount++;
                            OS_TaskDelay( 100 );
                            // pbiinfo("----------sdfsdf--------------------_prepare_play ------time%d\n",ucCount);
                        }
                        else
                        {
                            pbiinfo("[%s %d] GetLockStatus is UnLock.\n",DEBUG_LOG);
                            break;
                        }
                    }
                    g_FastTunerLockFlag = 1;
                }
            else
            {
                while( ucCount < 10 )
                {
                    if (DRV_Tuner_GetLockStatus(0) != eTun_Lock)
                    {
                        ucCount++;
                        OS_TaskDelay( 100 );
                        // pbiinfo("----------sdfsdf--------------------_prepare_play ------time%d\n",ucCount);
                    }
                    else
                    {
                        break;
                    }
                }
            }
                //pbiinfo("------------------------------_prepare_play ------time%d\n",ucCount);
            }
        }

        if (eDVBPLAYER_PROG_TYPE_SERVICE == ptPlayParam->uProgParam.eProgType)
        {
    #if 0
            DVBPlayer_Serv_Param_t  *ptServ = NULL;
            pbiinfo("[%s %d] DvbPlayer Start Pmt.\n",DEBUG_LOG);
            ptServ = (DVBPlayer_Serv_Param_t *)(&(ptPlayParam->uProgParam));
            if (0 != DVBPlayer_Start_PMT(ptServ->u16PMTPid, ptServ->u16ServId))
            {
                pbierror("\n-----start pmt failed!-----\n");
                return 1;
            }
            g_pmt_retry = 0;
        g_PmtTimeOutErrorCount = 0;
    #endif
        if( 0 != DVBPlayer_Check_PMT() )
        {
            pbierror("[%s %d] Check Pmt error.\n",DEBUG_LOG);
        }
        g_tStatus.tProgStatus.ePMTStatus = eDVBPLAYER_PMT_CHECK;
        //g_tStatus.tProgStatus.ePMTStatus = eDVBPLAYER_PMT_WAITING;
        }

        return 0;
    }

    static S32 _start_play( void )
    {
        pbiinfo("[%s %d] VideoCtrl = %d. VideoStatus = %d.\n", \
                DEBUG_LOG,g_tStatus.eVideoCtrlFlag, g_tStatus.tProgStatus.eVideoStatus );
        if ((eDVBPLAYER_ENABLED == g_tStatus.eVideoCtrlFlag)
                && (eDVBPLAYER_STOP == g_tStatus.tProgStatus.eVideoStatus))
        {
            if (0 != DVBPlayer_Enable_Screen())
            {
                pbierror("\n-----enable video out error!-----\n");
                return 1;
            }
			if (0 != DVBPlayer_Start_Video())
            {
                pbierror("\n-----start video error!-----\n");
                return 1;
            }
            
            g_tStatus.tProgStatus.eVideoStatus = eDVBPLAYER_PLAY;
        }
		else
		{
			DVBPlayer_Show_BroadcastBG();
		}

        pbiinfo("[%s %d] AudioCtrl = %d. AudioStatus = %d.\n", \
                DEBUG_LOG,g_tStatus.eAudioCtrlFlag, g_tStatus.tProgStatus.eAudioStatus );
        if ((eDVBPLAYER_ENABLED == g_tStatus.eAudioCtrlFlag)
                && (eDVBPLAYER_STOP == g_tStatus.tProgStatus.eAudioStatus))
        {
            if (0 != DVBPlayer_Start_Audio())
            {
                pbierror("\n-----start audio error!-----\n");
                // return 1;
            }
            else
            {
                g_tStatus.tProgStatus.eAudioStatus = eDVBPLAYER_PLAY;
            }
        }
        /*
            if (0 != DVBPlayer_Start_Pcr(g_tState.tChannel_Param[1].tPmt_Param.pcrpid))
            {
                pbiinfo("\n-----start pcr error!-----\n");
                return 1;
            }
        */
    g_AudioFrameErrorCount = 0;
    g_VideoFrameErrorCount = 0;
        g_tStatus.eRunningStatus = eDVBPLAYER_RUNNING_STATUS_PLAY;

        return 0;
    }

    static S32 _stop_play( void )
    {
        if ((eDVBPLAYER_ENABLED == g_tStatus.eVideoCtrlFlag)
                && (eDVBPLAYER_PLAY == g_tStatus.tProgStatus.eVideoStatus))
        {
            if (0 != DVBPlayer_Stop_Video())
            {
                pbierror("\n-----stop video error!-----\n");
            }
            g_tStatus.tProgStatus.eVideoStatus = eDVBPLAYER_STOP;
        }

        if ((eDVBPLAYER_ENABLED == g_tStatus.eAudioCtrlFlag)
                && (eDVBPLAYER_PLAY == g_tStatus.tProgStatus.eAudioStatus))
        {
            if (0 != DVBPlayer_Stop_Audio())
            {
                pbierror("\n-----stop audio error!-----\n");
            }
            g_tStatus.tProgStatus.eAudioStatus = eDVBPLAYER_STOP;
        }

        if ((eDVBPLAYER_SYNC_REF_PCR == g_tStatus.eSyncType)
                && (eDVBPLAYER_PLAY == g_tStatus.tProgStatus.ePCRStatus))
        {
            if (0 != DVBPlayer_Stop_PCR())
            {
                pbierror("\n-----stop pcr error!-----\n");
            }
            g_tStatus.tProgStatus.ePCRStatus = eDVBPLAYER_STOP;
        }

        if (g_tStatus.tProgStatus.ePMTStatus != eDVBPLAYER_PMT_STOP)
        {
            if (0 != DVBPlayer_Stop_PMT())
            {
                pbierror("\n-----stop pmt error!-----\n");
            }
            g_tStatus.tProgStatus.ePMTStatus = eDVBPLAYER_PMT_STOP;
        }

        g_tStatus.tProgStatus.ePMTStatus = eDVBPLAYER_PMT_STOP;
        g_tStatus.eRunningStatus = eDVBPLAYER_RUNNING_STATUS_STOP;

        return 0;
    }

    static void _set_attr( DVBPlayer_Attr_Param_t *pAttrParam )
    {
        switch( pAttrParam->eAttrType )
        {
        case eDVBPLAYER_ATTR_VID_SYSTEM:
        case eDVBPLAYER_ATTR_VID_ASPECT_RATIO:
        case eDVBPLAYER_ATTR_VID_WINDWOS:
        case eDVBPLAYER_ATTR_SWITCH_AUDIOPID:
        default:
            break;
        }
    }

    static void _process_message( void )
    {
        DVBPlayer_Action_Type_e  eActionType = eDVBPLAYER_ACTION_INVALID;
        DVBPlayer_Action_Param_u uActionParam;
        int count = 0;
    
        while (1)
        {
            if (MsgQueue_try_take(&g_tMsg_Queue_Switch, 0,
                                  &eActionType, &uActionParam, NULL, NULL))
            {
                count++;
            	/*if(MsgQueue_CompareMsgType(&g_tMsg_Queue_Switch,(int)eActionType))
                {
                    pbiinfo("[%s %d] CompareMsgType.\n",DEBUG_LOG);
                    continue;
                } */
                
                if ( 0 == DRV_AVCtrl_InitFlag())
                {
                    pbiinfo("[%s %d] _Process_message continue.\n", DEBUG_LOG );
                    continue;
                }
                
                switch( eActionType )
                {
                case eDVBPLAYER_ACTION_PLAY:
                {
                    _prepare_play((DVBPlayer_Play_Param_t *)(&uActionParam));
                }
                break;

                case eDVBPLAYER_ACTION_STOP:
                {
                    _stop_play();
                }
                break;

                case eDVBPLAYER_ACTION_SET_ATTR:
                {
                    _set_attr( (DVBPlayer_Attr_Param_t*)(&uActionParam));
                }
                break;

                case eDVBPLAYER_ACTION_REC_PLAY:
                {
                    g_TimePvrFlag = 1;
                }
                break;

                case eDVBPLAYER_ACTION_REC_STOP:
                {
                    g_TimePvrFlag = 0;
                }
                break;
#if 0
                case eDVBPLAYER_ACTION_PVR_PLAY:
                {
                    DVBPlayer_PVR_Param_t *pPvrParam = NULL;
                    pPvrParam = (DVBPlayer_PVR_Param_t *)(&uActionParam);

                    U32 VidPid = g_tStatus.tProgStatus.u16VideoPid;
                    U32 AudPid = g_tStatus.tProgStatus.u16AudioPid;
                    U32 VidFmt = g_tStatus.tProgStatus.u8VideoFmt;
                    U32 AudFmt = g_tStatus.tProgStatus.u8AudioFmt;

                    DVBPlayer_PVR_PlayStart( VidPid, VidFmt, AudPid, AudFmt, pPvrParam->tPvrPlay.FileName );
                }
                break;

                case eDVBPLAYER_ACTION_PVR_STOP:
                {
                    U32 VidPid = g_tStatus.tProgStatus.u16VideoPid;
                    U32 AudPid = g_tStatus.tProgStatus.u16AudioPid;
                    U32 VidFmt = g_tStatus.tProgStatus.u8VideoFmt;
                    U32 AudFmt = g_tStatus.tProgStatus.u8AudioFmt;
                    DVBPlayer_PVR_PlayStop( VidPid, VidFmt, AudPid, AudFmt );
                }
                break;
                case eDVBPLAYER_ACTION_PVR_PAUSE:
                {
                    DVBPlayer_PVR_PlayPause();
                }
                break;

                case eDVBPLAYER_ACTION_PVR_RESUME:
                {
                    DVBPlayer_PVR_PlayResume();
                }
                break;

                case eDVBPLAYER_ACTION_PVR_SPEED:
                {
                    DVBPlayer_PVR_Param_t *pPvrParam = NULL;

                    pPvrParam = (DVBPlayer_PVR_Param_t *)&uActionParam;
                    DVBPlayer_PVR_PlaySpeedCtrl(pPvrParam->ePvrSpeed);
                }
                break;
#endif
                default:
                {
                }
                break;
                }

            if( count >= 5 )
            {
                break;
            }

            }
            else
            {
                break;
            }
        }
    }


U8 ProcessPrintCount = 0;
    static void _process_monitor( void )
    {
        DVBPlayer_Event_t playerEvent;

        if ( ( 1 == Drv_AVCtrl_ModeType() ) ||( 1 == g_TimePvrFlag ) )
        {
            /* The Run of Plyer is Not DVB Player */
            if( ProcessPrintCount >= 5 )
            {
                ProcessPrintCount = 0;
                pbiinfo("[%s %d] _process_monitor return! ModeType %d, TimePVR %d.\n", \
                    DEBUG_LOG,Drv_AVCtrl_ModeType(), g_TimePvrFlag );
            }
            else
            {
                ProcessPrintCount++;
            }
        	//pbiinfo("[%s %d] return.\n",DEBUG_LOG);
            return;
        }

        if (eDVBPLAYER_RUNNING_STATUS_STOP == g_tStatus.eRunningStatus)
        {
            /* dvbplayer stopped, needn't monitor. */
            if( ProcessPrintCount >= 5 )
            {
                ProcessPrintCount = 0;
                pbiinfo("[%s %d] _process_monitor return! Status %d.\n", \
                    DEBUG_LOG, g_tStatus.eRunningStatus );
            }
            else
            {
                ProcessPrintCount++;
            }
        //pbiinfo("[%s %d] return.\n",DEBUG_LOG);
            return;
        }

        if (DRV_Tuner_GetLockStatus(0) != eTun_Lock)
        {
        if (g_tuner_retry > 30)
            {
                g_tStatus.eTunerStatus = eDVBPLAYER_TUNER_UNLOCK;
                g_program_event =eDVBPLAYER_EVENT_PMT_TIMEOUT;
                /* send dvbplayer tuner dropped message to user */
                if(g_tsignal_event ==eDVBPLAYER_EVENT_TUNER_LOCK)
                {
                    memset( &playerEvent, 0, sizeof(DVBPlayer_Event_t) );
                    playerEvent.eEventType = eDVBPLAYER_EVENT_TUNER_DROPPED;
                    if( NULL != g_tStatus.pfnNotifyFunc )
                    {
                        g_tStatus.pfnNotifyFunc( playerEvent );
                        pbiinfo("\nPlayer Tuner DrOpped Message [%s--%d]\n\n", __FUNCTION__,__LINE__);
                    }
                    g_tuner_retry = 0;
                    g_tsignal_event =eDVBPLAYER_EVENT_TUNER_DROPPED;
                }
                if (eDVBPLAYER_RUNNING_STATUS_PREPARE == g_tStatus.eRunningStatus)
                {
                    TunerReset();
                }
            }
            else
            {
                g_tuner_retry++;
            }

        	//pbiinfo("[%s %d] return.\n",DEBUG_LOG);
            return;
        }
        else
        {
            g_tStatus.eTunerStatus = eDVBPLAYER_TUNER_LOCKED;
            g_tuner_retry = 0;

            /* send dvbplayer tuner locked message to user */
            if(g_tsignal_event ==eDVBPLAYER_EVENT_TUNER_DROPPED)
            {
                memset( &playerEvent, 0, sizeof(DVBPlayer_Event_t) );
                playerEvent.eEventType = eDVBPLAYER_EVENT_TUNER_LOCK;
                if( NULL != g_tStatus.pfnNotifyFunc )
                {
                    g_tStatus.pfnNotifyFunc( playerEvent );
                    pbiinfo("\nPlayer Tuner LOCK Message [%s--%d]\n\n", __FUNCTION__,__LINE__);
                }
                else
                {
                    pbiinfo("\nplayer notify function is null\n\n");
                }
                g_tsignal_event =eDVBPLAYER_EVENT_TUNER_LOCK;
            }
        }

        if (eDVBPLAYER_RUNNING_STATUS_PREPARE == g_tStatus.eRunningStatus)
        {
            pbiinfo("[%s %d] PmtStatus = %d.\n",DEBUG_LOG,g_tStatus.tProgStatus.ePMTStatus);
        if( eDVBPLAYER_PMT_CHECK == g_tStatus.tProgStatus.ePMTStatus )
        {
            g_PmtCheckTimeOutCount++;
            if( g_PmtCheckTimeOutCount > 30 )
            {
                pbiinfo("[%s %d] Pat count out.\n",DEBUG_LOG);
                DVBPlayer_Start_PMT(g_tStatus.tProgStatus.u16PMTPid, g_tStatus.tProgStatus.u16ServiceId);
                g_tStatus.tProgStatus.ePMTStatus = eDVBPLAYER_PMT_WAITING;
            }
        }
        else if (eDVBPLAYER_PMT_WAITING == g_tStatus.tProgStatus.ePMTStatus)
            {

                    TUNER_LOCKSTATUS_t lock = 0;
                    lock = DRV_Tuner_GetLockStatus(0);
            g_PmtCheckTimeOutCount = 0;
                    /* send dvbplayer pmt timeout message to user */
                    if( eTun_Lock == lock )
                    {
                        memset( &playerEvent, 0, sizeof(DVBPlayer_Event_t) );
                        playerEvent.eEventType = eDVBPLAYER_EVENT_PMT_TIMEOUT;

                g_PmtTimeOutErrorCount++;
                if( g_PmtTimeOutErrorCount > 30 )
                    {
                        if( NULL != g_tStatus.pfnNotifyFunc )
                        {
                            g_tStatus.pfnNotifyFunc( playerEvent );
                        pbiinfo("\nPlayer PMT Time Out Message [%s--%d]\n\n", __FUNCTION__,__LINE__);
                        }
                        else
                        {
                            pbiinfo("\nplayer notify function is null\n\n");
                        }
                    g_tStatus.tProgStatus.ePMTStatus = eDVBPLAYER_PMT_CHECK;
                    g_PmtTimeOutErrorCount = 0;
                    }
                TunerReset();
                g_program_event =eDVBPLAYER_EVENT_PMT_TIMEOUT;
                }
            	//pbiinfo("[%s %d] return.\n",DEBUG_LOG);
                return;
            }
            else if (eDVBPLAYER_PMT_RECEIVED == g_tStatus.tProgStatus.ePMTStatus)
            {
                g_pmt_retry = 0;
            g_PmtTimeOutErrorCount = 0;
                _start_play();
                /* send dvbplayer play start message to user */
                if(g_program_event ==eDVBPLAYER_EVENT_PMT_TIMEOUT)
                {
                    memset( &playerEvent, 0, sizeof(DVBPlayer_Event_t) );
                    playerEvent.eEventType = eDVBPLAYER_EVENT_PLAY_START;
                    if( NULL != g_tStatus.pfnNotifyFunc )
                    {
                        g_tStatus.pfnNotifyFunc( playerEvent );
                        pbiinfo("\nPlayer Start Message [%s--%d]\n\n", __FUNCTION__,__LINE__);
                    }
                    else
                    {
                        pbiinfo("\nplayer notify function is null\n\n");
                    }
                    g_program_event =eDVBPLAYER_EVENT_PLAY_START;
                }
            }
            else /* eDVBPLAYER_PMT_STOP == g_tStatus.tProgStatus.ePMTStatus */
            {
                /* play AV Pids */
                _start_play();
                /* send dvbplayer play start message to user */
                memset( &playerEvent, 0, sizeof(DVBPlayer_Event_t) );
                playerEvent.eEventType = eDVBPLAYER_EVENT_PLAY_START;
                if( NULL != g_tStatus.pfnNotifyFunc )
                {
                    g_tStatus.pfnNotifyFunc( playerEvent );
                    pbiinfo("\nPlayer Start Message [%s--%d]\n", __FUNCTION__,__LINE__);
                }
                else
                {
                    pbiinfo("\nplayer notify function is null\n\n");
                }
            }
        }
        else if( eDVBPLAYER_RUNNING_STATUS_PLAY == g_tStatus.eRunningStatus )
        {
            if( eDVBPLAYER_PLAY == g_tStatus.tProgStatus.eVideoStatus )
            {
                if( 1 == DVBPlayer_Check_Video_Status() )
                {
                    g_VideoFrameErrorCount++;
                //if( 40 == g_VideoFrameErrorCount )
                if( 80 == g_VideoFrameErrorCount )
                    {
                        if(g_program_event !=eDVBPLAYER_EVENT_AV_INTERRUPT)
                        {
                            memset( &playerEvent, 0, sizeof(DVBPlayer_Event_t) );
                            playerEvent.eEventType = eDVBPLAYER_EVENT_AV_INTERRUPT;
                            if( NULL != g_tStatus.pfnNotifyFunc )
                            {
                            pbiinfo("[%s %d] VideoFrameErrorCount = %d.\n",DEBUG_LOG,g_VideoFrameErrorCount);
                                g_tStatus.pfnNotifyFunc( playerEvent );
                                pbiinfo("\neDVBPLAYER_EVENT_PROGRAM_INTERRUPT Message [%s--%d]\n\n", __FUNCTION__,__LINE__);
                            }
                            else
                            {
                                pbiinfo("\n eDVBPLAYER_EVENT_PROGRAM_INTERRUPT player notify function is null\n\n");
                            }
                            g_program_event =eDVBPLAYER_EVENT_AV_INTERRUPT;
                        }
                        g_VideoFrameErrorCount =0;

                    }
                }
                else
                {
                    if( g_VideoFrameErrorCount > 0 )
                    {
                    g_VideoFrameErrorCount = 0;
                    }
                    if(g_program_event ==eDVBPLAYER_EVENT_AV_INTERRUPT)
                    {
                        playerEvent.eEventType = eDVBPLAYER_EVENT_PLAY_START;
                        if( NULL != g_tStatus.pfnNotifyFunc )
                        {
                            g_tStatus.pfnNotifyFunc( playerEvent );
                            pbiinfo("\nPlayer video OK [%s--%d]\n\n", __FUNCTION__,__LINE__);
                        }
                        g_program_event =eDVBPLAYER_EVENT_PLAY_START;
                    }

                }
            }
            if( eDVBPLAYER_PLAY == g_tStatus.tProgStatus.eAudioStatus )
            {
                if( 1 == DVBPlayer_Check_Audio_Status() )
                {
                    //	pbiinfo("\nAudio Frame Error!!!!!!!!! coutn %d\n\n",g_AudioFrameErrorCount);
                    g_AudioFrameErrorCount++;
                //if( 40 == g_AudioFrameErrorCount )
                if( 80 == g_AudioFrameErrorCount )
                    {
                        if(g_program_event !=eDVBPLAYER_EVENT_AV_INTERRUPT)
                        {
                            memset( &playerEvent, 0, sizeof(DVBPlayer_Event_t) );
                            playerEvent.eEventType = eDVBPLAYER_EVENT_AV_INTERRUPT;
                            if( NULL != g_tStatus.pfnNotifyFunc )
                            {
                            pbiinfo("[%s %d] AudioFrameErrorCount = %d.\n",DEBUG_LOG,g_AudioFrameErrorCount);
                                g_tStatus.pfnNotifyFunc( playerEvent );
                                pbiinfo("\neDVBPLAYER_EVENT_PROGRAM_INTERRUPT Message [%s--%d]\n\n", __FUNCTION__,__LINE__);
                            }
                            else
                            {
                                pbiinfo("\n eDVBPLAYER_EVENT_PROGRAM_INTERRUPT player notify function is null\n\n");
                            }
                            g_program_event =eDVBPLAYER_EVENT_AV_INTERRUPT;
                        }
                        g_AudioFrameErrorCount =0;
                    }
                }
                else
                {
                    if( g_AudioFrameErrorCount > 0 )
                    {
                    g_AudioFrameErrorCount = 0;
                    }

                    if(g_program_event ==eDVBPLAYER_EVENT_AV_INTERRUPT)
                    {
                        playerEvent.eEventType = eDVBPLAYER_EVENT_PLAY_START;
                        if( NULL != g_tStatus.pfnNotifyFunc )
                        {
                            g_tStatus.pfnNotifyFunc( playerEvent );

                            pbiinfo("\nPlayer video OK [%s--%d]\n\n", __FUNCTION__,__LINE__);
                        }
                        g_program_event =eDVBPLAYER_EVENT_PLAY_START;
                    }
                }
            }
        }
    	//pbiinfo("[%s %d] return.\n",DEBUG_LOG);
    }

    static void _dvbplayer_task( void )
    {
        while(g_PlayerTaskFlag)
        {
            _process_message();
            _process_monitor();
            OS_TaskDelay(100);
        }
    }
    DRV_AVCTRL_EVENT_CALLBACK_FUN dvbcallback( DRV_AVCTRL_FRAME_INFO_t tVideoFrameInfo )
    {
        return AVCTRL_NO_ERROR;
    }

    S32 DVBTable_Init()
    {

        pbiinfo("\n DVBPlayer_Init init ,  init~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~!\n");
        SYS_BackGround_Init();
        System_Time_Init(NULL);
        EPG_Init(NULL);
        background_cat_init();
        background_nit_init();
    	BackGroundPatInit();
        return 0;
    }


    S32 DVBPlayer_Init( DVBPlayer_Config_t * pConfig )
    {
        S32 sRet  = 0;

        SYS_Table_InitParam_t pmt_init_param;
        SYS_Table_InitParam_t cat_init_param;

        if( 1 == g_PlayerInitFlag )
        {
            pbiinfo("\nDvbplayer init again, can't init!+++++++++++++++++++++++++++++++++++++++++\n");
            if(g_PlayerTaskFlag == 0 && g_uDVBPlayerTask == 0xFFFFFFFF)//amlogic DVB onStop时有几率会挂掉这个Task
            {
            		g_PlayerTaskFlag = 1;
	            if(OS_SUCCESS != OS_TaskCreate(&g_uDVBPlayerTask, "DVBPlayerTask",
	                                       (void *(*)(void*))_dvbplayer_task, NULL, NULL,
	                                       DVBPlayer_Size, DVBPlayer_Priority, 0))
		        {
		            pbierror("----DVBPlayerTask create error!----\n");
				g_PlayerTaskFlag = 0;
		            return PV_FAILURE;
		        }
	        }
            return PV_SUCCESS;
        }

        //  EPG_Resume();
        EPG_Start();
        g_AudioFrameErrorCount = 0;
        g_VideoFrameErrorCount = 0;

        memset(&g_tStatus, 0, sizeof(g_tStatus));
        g_tStatus.eRunningStatus = eDVBPLAYER_RUNNING_STATUS_STOP;
        g_tStatus.eTunerStatus = eDVBPLAYER_TUNER_UNLOCK;
        g_tStatus.tProgStatus.u16PMTPid = INVALID_PID;
        g_tStatus.tProgStatus.ePMTStatus = eDVBPLAYER_PMT_STOP;

        OS_SemCreate(&(semPMT_CallBack), (const char *)"semPMT_CallBack", 0, 0);

        pmt_init_param.p_semaphore = semPMT_CallBack;
        pmt_init_param.callback = _pmt_callback_func;
        sRet = SYS_BackGround_TableInit(SYS_BG_PMT, pmt_init_param);
        if( 0 != sRet )
        {
            PBIDEBUG("SYS_BackGround_TableInit error");
        }

        SYS_Table_Param_t bgfilterdata;
        sRet = SYS_BackGround_TableStart(SYS_BG_CAT, bgfilterdata);
        if( 0 != sRet )
        {
            PBIDEBUG("SYS_BackGround_TableStart error");
    }
    
    sRet = SYS_BackGround_TableStart(SYS_BG_PAT, bgfilterdata);
    if( 0 != sRet )
    {
        PBIDEBUG("SYS_BackGround_PAT_Start error");
    }
    

        sRet = SYS_BackGround_PMT_Start();
        if( 0 != sRet )
        {
            PBIDEBUG("SYS_BackGround_PMT_Start error");
        }

        sRet = SYS_BackGround_TableStart(SYS_BG_NIT, bgfilterdata);
        if( 0 != sRet )
        {
            PBIDEBUG("SYS_BackGround_TableStart error");
        }

        MsgQueue_create(&g_tMsg_Queue_Switch, 100);
        TLock_create(&g_tPlayer_Lock);

        DRV_AVCtrl_Regist_CallBackFun( dvbcallback );
	g_PlayerTaskFlag = 1;
        if(OS_SUCCESS != OS_TaskCreate(&g_uDVBPlayerTask, "DVBPlayerTask",
                                       (void *(*)(void*))_dvbplayer_task, NULL, NULL,
                                       DVBPlayer_Size, DVBPlayer_Priority, 0))
        {
            pbierror("----DVBPlayerTask create error!----\n");
g_PlayerTaskFlag = 0;
            return PV_FAILURE;
        }

        g_PlayerInitFlag = 1;

        PBIDEBUG("DVBPlayer_Init .End..\n");
        return PV_SUCCESS;
    }


    S32 DVBPlayer_Deinit( void )
    {
        S32 sRet = 0;

        PBIDEBUG("_stop_play....");
	g_PlayerTaskFlag = 0;
	OS_TaskDelete(g_uDVBPlayerTask);
        g_uDVBPlayerTask = 0xFFFFFFFF;

        //DVBPlayer_PVR_UnInit();
        _stop_play();
       
#if 0
        PBIDEBUG("Stop_Background");
        //  Stop_Background();
        PBIDEBUG("EPG_Pause");
        EPG_Pause();
        PBIDEBUG("MsgQueue_destroy");
        MsgQueue_destroy(&g_tMsg_Queue_Switch);
        PBIDEBUG("TLock_destroy");
        TLock_destroy(&g_tPlayer_Lock);
        PBIDEBUG("OS_TaskDelete_Android");
        OS_TaskDelete_Android(g_uDVBPlayerTask);
        g_uDVBPlayerTask = 0xFFFFFFFF;
#endif

//   PBIDEBUG("g_PlayerInitFlag");
//    g_PlayerInitFlag = 0;
        return PV_SUCCESS;
    }


    S32 DVBPlayer_RegCallback( DVBPlayer_Callback_pfn pfnCallback )
    {
        if (NULL == pfnCallback)
        {
            return PV_NULL_PTR;
        }

        g_tStatus.pfnNotifyFunc = pfnCallback;

        return PV_SUCCESS;
    }
    S32 DVBPlayer_RegCACallback( DVBPlayer_Callback_pfn caCallback )
    {
        pbiinfo("\n gzx--------DVBPlayer_RegCACallback is start  !!!!!\n\n\n");
        if (NULL == caCallback)
        {
            return PV_NULL_PTR;
        }
        g_tStatus.caCallback= caCallback;
        pbiinfo("\n gzx--------DVBPlayer_RegCACallback is ok!!!!!!\n\n\n");
        return PV_SUCCESS;
    }
    void CA_DVBPlay_CallBack( DVBPlayer_Event_t tEvent)
    {
        if( NULL != g_tStatus.caCallback)
        {
            g_tStatus.caCallback(tEvent);
           // pbiinfo("\nCA_CallBack do!!!!!!!!\n\n");
        }
        else
        {
            CatTableVersionClaer();
            pbiinfo("\n gzx--------CA_CallBack is NULL!!!!!!\n\n\n");
        }
    }
    U32 GetTunerFreq()
    {
        return g_tStatus.uTunerParam.tCable.u32Freq;
    }
    U32 CheckChannel(U16 u16PmtPid, U16 u16ServId)
    {
        if( ( g_tStatus.tProgStatus.u16PMTPid == u16PmtPid )
                &&( g_tStatus.tProgStatus.u16ServiceId == u16ServId ) )
        {
            return 1;
        }
        return 0;
    }

	void Reset_ServiceID(void)
	{
		g_tStatus.tProgStatus.u16ServiceId =0xffff;
	}
		
    S32 DVBPlayer_CA_Function_Init(DVBPlayer_CA_Fun_t CaFun)
    {
        if( (NULL != CaFun.pfServiceStart) && (NULL != CaFun.pfServiceStop) )
        {
            g_tCaFun.pfServiceStart = CaFun.pfServiceStart;
            g_tCaFun.pfServiceStop = CaFun.pfServiceStop;
        }
        else
        {
            g_tCaFun.pfServiceStart = NULL;
            g_tCaFun.pfServiceStop = NULL;
            pbiinfo("g_tCaFun is null!\n");
        }

        return 0;
    }


    int DVBPlayer_GetCamode()
    {
        return g_tStatus.tProgStatus.ecamode;
    }



#ifdef __cplusplus
}
#endif

