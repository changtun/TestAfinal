/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : Installation
* File name   : installation.c
* Description : Include implementations of APIs.
*
* History :
*   2012-05-23 ZYJ
*       Initial Version.
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "pvddefs.h"
#include "pbitrace.h"
#include "osapi.h"
#include "drv_tuner.h"
#include "drv_filter.h"

#include "dvbcore.h"
#include "psi_si.h"
#include "crc.h"
#include "installation.h"
#include "background.h"
//#include "client.h"
//#include "dvb_socket_message.h"


/*******************************************************/
/*               Private Defines and Macros			   */
/*******************************************************/
#define MAX_SECTION_LENGTH              (4096)
#define SERVICE_MEM_CHUNK_SIZE          (8 * 1024)
#define FILTER_DEPTH_SIZE               (16)
#define	ALL_SEARCH_MAX_TP			(256)
#define LW_BAT 1
/*******************************************************/
/*               Private Types						   */
/*******************************************************/
//#if LW_BAT

//#else if
static int g_count = 0;
//#endif

typedef struct
{
    void *          pNext;              /* ָ����һ���ڴ� */
    U32             u32TotalSize;       /* �ڴ��ܴ�С */
    U32             u32RemainSize;      /* �ڴ�ʣ���С */
    U32             u32ServNum;         /* �ڴ��б���� service ��Ŀ */
    Install_Serv_t  ptService[0];       /* service �б� */
} _Install_Serv_Mem_t;


typedef enum
{
	eSearchStop = 0,
	eSearchRun,
}SearchStatus_e;

typedef struct
{
    /* �������� */
    Install_Search_Type_e   eSearchType;
	SearchStatus_e			eSearchStatus;  /*  add by zxguan used Search exit */
    Install_Timeout_t       tTimeout;
    U32                     u32MainTpNum;
    DVBCore_Tuner_Desc_u *  puMainTpDescList;
    U32                     u32NormalTpNum;
    DVBCore_Tuner_Desc_u *  puNormalTpDescList;
    Install_Notify_pfn      pfnNotifyFunc;

    /* ����״̬����� */
    Install_Progress_t      tProgressInfo;  /* ������� */
    U32                     u32TpNum;       /* ����Ƶ����Ŀ */
    Install_Tp_t *          ptTpList;       /* ����Ƶ���б����� */
    U32                     u32ServNum;     /* �������� Service ��Ŀ */
    _Install_Serv_Mem_t *   ptServMem;      /* ���� service list ���ڴ�ָ�� */
    PSISI_Table_t *         ptNIT;          /* ��Ƶ�� NIT ��, NULL ��ʾû����Ƶ�� NIT */
    PSISI_Table_t *         ptBAT;          /* ��Ƶ�� BAT ��, NULL ��ʾû����Ƶ�� BAT */
    U8                      u8SearchRsltFlag;   /* ��¼�������״̬. 0 - ���ͷ�; 1 - ���� */
    U8                      u8SearchLimitFlag; /* ���������_�P��0�������ƣ� 1���o���� */
} _Install_Status_t;


typedef enum
{
    eINSTALL_MSG_INVALID = 0,
    eINSTALL_MSG_NEW_SECTION,
    eINSTALL_MSG_STOP
} _Install_Msg_Type_e;

typedef struct
{
    _Install_Msg_Type_e eMsgType;
    U16                 u16Pid;
    U16                 u16SectLen;
    U8                  u8TableId;
    U8                  u8Version;
    U16                 u16ExtId;
    U8 *                pu8Buffer;
} _Table_Msg_t;



typedef enum
{
    eSEARCH_TABLE_SUCCESS = 0,
    eSEARCH_TABLE_TIMEOUT,
    eSEARCH_TABLE_FAILED,
    eSEARCH_TABLE_STOP,
    eSEARCH_TABLE_PARAM_ERROR,
    eSEARCH_TABLE_OTHER_ERROR
} _Search_Tbl_Err_e;


typedef struct
{
    U16     u16Pid;
    U8      u8TableId;
    U8      reserved1;
    U16     u16ExtendId;
    U32     u32TimeoutMs;
} _Search_Tbl_Param_t;

/*******************************************************/
/*               Global Variables					   */
/*******************************************************/
static _Install_Status_t    g_tInstallStatus;


static U32 guTable_Msg_Queue = 0xFFFFFFFF;
static U32 guInstall_Start_Sem = 0xFFFFFFFF;
static U32 g_InstallFinish_Sem = 0xFFFFFFFF;


static U32 guInstall_Task = 0xFFFFFFFF;

static const U32 guInstall_Stack_Size = 18 * 1024;		/* 18K */

static const U32 guInstall_Priority = 4;

static  U16 g_nitsearch_nitversion=0xffff;
/*******************************************************/
/*               Global Function prototypes					   */
/*******************************************************/

/*******************************************************/
/*               Private Function prototypes		   */
/*******************************************************/


/*******************************************************/
/*               Functions							   */
/*******************************************************/
U16 _Get_install_Nit_Version(void)
{
    return g_nitsearch_nitversion;
}
void Set_Nit_Version(U16 value)
{
    g_nitsearch_nitversion =value;
}

static U8 *_install_filter_get_buffer(tSFILTER_SlotId  slot)
{
    U8	*pbuf = NULL;

    (void)slot;
    pbuf = (U8 *)OS_MemAllocate(system_p, MAX_SECTION_LENGTH);
    if( pbuf == NULL )
    {
        printf("_install_filter_get_buffer allocate memory failed.\n");
    }

    return pbuf;
}

static void _install_free_section_buffer(void * ptr)
{
    if( ptr != NULL )
    {
        OS_MemDeallocate(system_p, ptr);
        ptr = NULL;
    }
}

static void _install_filter_free_buffer(tSFILTER_SlotId slotid, void *ptr)
{
    (void)slotid;
    _install_free_section_buffer(ptr);
}


static void _install_filter_callback
(
    tSFILTER_SlotId         slot,
    tSFILTER_FilterId       filter,
    tSFILTER_FreeBufferFct  free_func,
    U8 *                    buffer,
    U32                     length,
    U16                     pid
)
{
    S32				os_err = OS_SUCCESS;
    U8				*section_buffer = NULL;
    U16				section_length = 0;
    U16				extend_id = 0xFFFF;
    U8				table_id = 0xFF;
    U8				version = 0xFF;
    U8				sect_num = 0;
    U8				last_sect_num = 0;
    _Table_Msg_t	msg;

    if( NULL == buffer )
    {
        return;
    }

    section_buffer = buffer;
    section_length = (U16)length;

    table_id = section_buffer[0];
    extend_id = (section_buffer[3] << 8) + section_buffer[4];
    version = (section_buffer[5] & 0x3e) >> 1;
    sect_num = section_buffer[6];
    last_sect_num = section_buffer[7];

    if( pv_crc32(buffer, length, 1) != 0)
    {
        printf("section CRC error. PID = 0x%04X, tabid = 0x%02X, extid = 0x%04X, sect_num = %d, last_sect_num = %d.\n",
               pid, table_id, extend_id, sect_num, last_sect_num);
        if( NULL != free_func )
        {
            free_func(slot, buffer);
        }
        return;
    }

    msg.eMsgType = eINSTALL_MSG_NEW_SECTION;
    msg.u16Pid = pid;
    msg.u8TableId	= table_id;
    msg.u16SectLen	= section_length;
    msg.u16ExtId = extend_id;
    msg.u8Version	= version;
    msg.pu8Buffer	= section_buffer;

    os_err = OS_QueueSend(guTable_Msg_Queue, &msg, sizeof(_Table_Msg_t),
                          OS_PEND, 0);
    if(os_err != OS_SUCCESS)
    {
        printf("_install_filter_callback: send message to queue failed.\n");
        if((NULL != free_func) && (NULL != buffer))
        {
            free_func(slot, buffer);
        }
    }
}



static _Search_Tbl_Err_e _install_Search_Table
(
    _Search_Tbl_Param_t tParam,
    PSISI_Table_t **    pptTable
)
{
    S32                 os_err = OS_SUCCESS;
    tSFILTER_SlotId     slot_id = -1;
    tSFILTER_FilterId   filter_id = -1;
    U8                  filter_data[FILTER_DEPTH_SIZE] = {0};
    U8                  filter_mask[FILTER_DEPTH_SIZE] = {0};
    U8                  filter_negate[FILTER_DEPTH_SIZE] = {0};
    U8                  filter_err_flag = 0;
    U8                  stop_recv_flag = 0;
    _Search_Tbl_Err_e   ret = eSEARCH_TABLE_SUCCESS;

    if( pptTable == NULL )
    {
        return eSEARCH_TABLE_PARAM_ERROR;
    }

    /* create message queue */
    os_err = OS_QueueCreate(&guTable_Msg_Queue, "Install_Table_Queue",
                            50, sizeof(_Table_Msg_t), 0);
    if(OS_SUCCESS != os_err)
    {
        pbierror("_install_Search_Table, create message queue failed! Errcode = %d.\n", os_err);
        return eSEARCH_TABLE_FAILED;
    }

    memset(filter_data, 0, FILTER_DEPTH_SIZE);
    memset(filter_mask, 0xFF, FILTER_DEPTH_SIZE);
    //memset(filter_negate, 0, FILTER_DEPTH_SIZE);


    /* set and start filter */
    /* allocate channel */
    slot_id = SFILTER_Check_DataPID(tParam.u16Pid,DEMUX_0);
    if(slot_id >= 0)
    {
        pbiinfo("found exist channel, pid = 0x%04X. try to disable it.\n", tParam.u16Pid);
        //xhren0713,����Ѿ��к͸�pid�������channel, �Ƚ���channel�ص�
        if (SFILTER_Free_Channel(slot_id) < 0)
        {
            pbierror("warning: diable channel failed.\n");
            return eSEARCH_TABLE_FAILED;
        }
    }

    slot_id = SFILTER_Allocate_Channel(DEMUX_0);
    if(slot_id < 0)
    {
        //xhren0713,����channelʧ�ܣ��˳�
        pbierror("_install_Search_Table allocate channel failed.\n");
        filter_err_flag = 1;
        ret = eSEARCH_TABLE_FAILED;
    }
    else
    {

        SFilterCallback_t callbacks;
        callbacks.pFGetBuffer = (tSFILTER_GetBufferFct)_install_filter_get_buffer;
        callbacks.pFFreeBuffer =(tSFILTER_FreeBufferFct)_install_filter_free_buffer;
        callbacks.pFCallBack = (tSFILTER_SlotCallbackFct)_install_filter_callback;
        SFILTER_RegisterCallBack_Function(slot_id, &callbacks);
    }


    /* allocate filter */
    if( filter_err_flag == 0)
    {
        S8 scDemuxError;
        //filter_id = SFILTER_Allocate_Filter(slot_id, eSFILTER_MODE_HW);
        filter_id = SFILTER_Allocate_Filter(DEMUX_0);
        if(filter_id < 0)
        {
            if(SFILTER_NumFilterAssociate_Slot(slot_id) == 0)
                SFILTER_Free_Channel(slot_id);

            filter_err_flag = 1;
            ret = eSEARCH_TABLE_FAILED;
        }
        else
        {
            filter_data[0] = tParam.u8TableId;
            filter_mask[0] = 0;
            if( (tParam.u8TableId == PMT_TABLE_ID)
                    || (tParam.u8TableId == SDT_OTHER_TABLE_ID)
                    || (tParam.u8TableId == BAT_TABLE_ID) )
            {
                filter_data[1] = (tParam.u16ExtendId >> 8) & 0xFF;
                filter_data[2] = tParam.u16ExtendId & 0xFF;
                filter_mask[1] = 0;
                filter_mask[2] = 0;
            }
            scDemuxError = SFILTER_SetFilter(slot_id, filter_id, FILTER_DEPTH_SIZE, filter_mask, filter_data, filter_negate);
            if(scDemuxError<0)
            {
                SFILTER_Free_Filter(filter_id);
                if(SFILTER_NumFilterAssociate_Slot(slot_id) == 0)
                    SFILTER_Free_Channel(slot_id);
                filter_err_flag = 1;
                ret = eSEARCH_TABLE_FAILED;
            }
            else
            {
                scDemuxError = SFILTER_SetPid(slot_id, tParam.u16Pid);
                if(scDemuxError<0)
                {
                    SFILTER_Free_Filter(filter_id);
                    if(SFILTER_NumFilterAssociate_Slot(slot_id)==0)
                        SFILTER_Free_Channel(slot_id);
                    filter_err_flag = 1;
                    ret = eSEARCH_TABLE_FAILED;
                }
                else
                {
                    scDemuxError = SFILTER_Open_Channel(slot_id);
                    if(scDemuxError<0)
                    {
                        SFILTER_Free_Filter(filter_id);
                        if(SFILTER_NumFilterAssociate_Slot(slot_id)==0)
                            SFILTER_Free_Channel(slot_id);
                        filter_err_flag = 1;
                        ret = eSEARCH_TABLE_FAILED;
                    }
                }
            }
        }
    }


    /* receive and parse table */
    stop_recv_flag = filter_err_flag;
    while(stop_recv_flag == 0)
    {
        _Table_Msg_t	msg;
        U32				actual_size = 0;
        S32				recv_ret = OS_SUCCESS;

        /* receive section */
        memset(&msg, 0, sizeof(_Table_Msg_t));

        recv_ret = OS_QueueReceive(guTable_Msg_Queue, &msg, sizeof(_Table_Msg_t),
                                   &actual_size, (tParam.u32TimeoutMs+2000) );
        if( recv_ret < 0 )
        {
            ret = eSEARCH_TABLE_TIMEOUT;
            break;
        }

        if (eINSTALL_MSG_STOP == msg.eMsgType)
        {
            ret = eSEARCH_TABLE_STOP;
            PBIDEBUG("====================INSTALL_MSG_STOP====================");
            break;
        }
        /* parse section��ע�� parse ��ɺ��ͷ� section buffer ռ�õ��ڴ� */
        else if (eINSTALL_MSG_NEW_SECTION == msg.eMsgType)
        {
            if ((msg.u16Pid != tParam.u16Pid)
                    || (msg.u8TableId != tParam.u8TableId)
                    || ((tParam.u16ExtendId != 0) && (msg.u16ExtId != tParam.u16ExtendId)))
            {
                _install_free_section_buffer(msg.pu8Buffer);
                continue;
            }

            if ((PAT_TABLE_ID != msg.u8TableId)
                    && (PMT_TABLE_ID != msg.u8TableId)
                    && (NIT_TABLE_ID != msg.u8TableId)
                    && (SDT_TABLE_ID != msg.u8TableId)
                    &&(BAT_TABLE_ID != msg.u8TableId))
            {
                _install_free_section_buffer(msg.pu8Buffer);
                continue;
            }
            if (NULL == *pptTable)
            {
                *pptTable = PSISI_Alloc_Table(msg.u16Pid, msg.pu8Buffer, msg.u16SectLen);
                if (NULL == *pptTable)
                {
                    pbierror("allocate memory for table failed.\n");
                    _install_free_section_buffer(msg.pu8Buffer);
                    continue;
                }
            }

            if (PAT_TABLE_ID == msg.u8TableId)
            {
                PSISI_Parse_PAT_Section(*pptTable, msg.pu8Buffer, msg.u16SectLen);
            }
            else if (PMT_TABLE_ID == msg.u8TableId)
            {
                PSISI_Parse_PMT_Section(*pptTable, msg.pu8Buffer, msg.u16SectLen);
            }
            else if (NIT_TABLE_ID == msg.u8TableId)
            {
                PSISI_Parse_NIT_Section(*pptTable, msg.pu8Buffer, msg.u16SectLen);
            }
            else if (SDT_TABLE_ID == msg.u8TableId)
            {
                PSISI_Parse_SDT_Section(*pptTable, msg.pu8Buffer, msg.u16SectLen);
            }
            else if (BAT_TABLE_ID == msg.u8TableId)
            {
                pbiinfo("Parse_BAT_Section____\n");
                PSISI_Parse_BAT_Section(*pptTable, msg.pu8Buffer, msg.u16SectLen);
            }
            else
            {}

            if (PSISI_Is_Table_Complete(*pptTable) != 0)
            {
                ret = eSEARCH_TABLE_SUCCESS;
                stop_recv_flag = 1;
            }
        }
        else
        {}

        _install_free_section_buffer(msg.pu8Buffer);
    }

    /* stop and free filter */
    if( filter_err_flag == 0)
    {
        SFILTER_Free_Filter(filter_id);
        /*modify lsz 2010.05.9*/
        if(SFILTER_NumFilterAssociate_Slot(slot_id)==0)
        {
            if(SFILTER_Free_Channel(slot_id)==0)
            {
                //ret = eSEARCH_TABLE_SUCCESS;
            }
            else
            {
                ret = eSEARCH_TABLE_OTHER_ERROR;
            }
        }
    }

    /* clean message queue */
    {
        _Table_Msg_t	msg;
        U32				actual_size = 0;

        memset(&msg, 0, sizeof(_Table_Msg_t));
        while( OS_QueueReceive(guTable_Msg_Queue, &msg, sizeof(_Table_Msg_t),
                               &actual_size, OS_CHECK) > 0 )
        {
            _install_free_section_buffer(msg.pu8Buffer);
            memset(&msg, 0, sizeof(_Table_Msg_t));
        }
    }

    /* delete message queue */
    os_err = OS_QueueDelete(guTable_Msg_Queue);
    guTable_Msg_Queue = 0xFFFFFFFF;

    return ret;
}


static void _convert_tuner_param
(
    DVBCore_Tuner_Desc_u *  puTunerDesc,
    U8 *                    pu8TunerId,
    ts_src_info_t *         ptTsSrc
)
{
    if ((NULL == puTunerDesc) || (NULL == pu8TunerId) || (NULL == ptTsSrc))
    {
        return;
    }

    if (eDVBCORE_SIGNAL_SOURCE_DVBC == puTunerDesc->eSignalSource)
    {
        *pu8TunerId = puTunerDesc->tCable.u8TunerId;
        ptTsSrc->MediumType = MEDIUM_TYPE_DVBC;
        ptTsSrc->u.Cable.FrequencyKHertz = puTunerDesc->tCable.u32Freq;
        ptTsSrc->u.Cable.SymbolRateSPS = puTunerDesc->tCable.u32SymbRate;
        ptTsSrc->u.Cable.Bandwidth = puTunerDesc ->tCable.Bandwidth;
        switch (puTunerDesc->tCable.eMod)
        {
        case eDVBCORE_MOD_16QAM:
            ptTsSrc->u.Cable.Modulation = ePI_16QAM;
            break;
        case eDVBCORE_MOD_32QAM:
            ptTsSrc->u.Cable.Modulation = ePI_32QAM;
            break;
        case eDVBCORE_MOD_64QAM:
            ptTsSrc->u.Cable.Modulation = ePI_64QAM;
            break;
        case eDVBCORE_MOD_128QAM:
            ptTsSrc->u.Cable.Modulation = ePI_128QAM;
            break;
        case eDVBCORE_MOD_256QAM:
            ptTsSrc->u.Cable.Modulation = ePI_256QAM;
            break;
        case eDVBCORE_MOD_512QAM:
            ptTsSrc->u.Cable.Modulation = ePI_512QAM;
            break;
        default:
            ptTsSrc->u.Cable.Modulation = ePI_64QAM;
            break;
        }

        switch (puTunerDesc->tCable.eFEC_Inner)
        {
        case eDVBCORE_CONV_CODE_RATE_1_2:
            ptTsSrc->u.Cable.FecInner = ePI_FECIN_DVBS_1_2;
            break;
        case eDVBCORE_CONV_CODE_RATE_2_3:
            ptTsSrc->u.Cable.FecInner = ePI_FECIN_DVBS_2_3;
            break;
        case eDVBCORE_CONV_CODE_RATE_3_4:
            ptTsSrc->u.Cable.FecInner = ePI_FECIN_DVBS_3_4;
            break;
        case eDVBCORE_CONV_CODE_RATE_5_6:
            ptTsSrc->u.Cable.FecInner = ePI_FECIN_DVBS_5_6;
            break;
        case eDVBCORE_CONV_CODE_RATE_7_8:
            ptTsSrc->u.Cable.FecInner = ePI_FECIN_DVBS_7_8;
            break;
        default:
            ptTsSrc->u.Cable.FecInner = ePI_FECIN_DVBS_7_8;
            break;
        }

        switch (puTunerDesc->tCable.eFEC_Outer)
        {
        case eDVBCORE_FEC_OUTER_RS_204_188:
            ptTsSrc->u.Cable.FecOuter = ePI_FECOUT_RS_204_188;
            break;
        default:
            ptTsSrc->u.Cable.FecOuter = ePI_FECOUT_NOT_CODING;
            break;
        }
    }
    else if (eDVBCORE_SIGNAL_SOURCE_DVBS == puTunerDesc->eSignalSource)
    {
    }
    else if (eDVBCORE_SIGNAL_SOURCE_DVBS2 == puTunerDesc->eSignalSource)
    {
    }
    else if (eDVBCORE_SIGNAL_SOURCE_DVBT == puTunerDesc->eSignalSource)
    {
    }
    else if (eDVBCORE_SIGNAL_SOURCE_DMBT == puTunerDesc->eSignalSource)
    {
    }
    else if (eDVBCORE_SIGNAL_SOURCE_ANALOG == puTunerDesc->eSignalSource)
    {
    }
    else
    {}

    return;
}


static U8 _install_set_frequency(DVBCore_Tuner_Desc_u * puTunerDesc)
{
    U8                  u8TunerId;
    ts_src_info_t       tTsSrc;
    TUNER_ErrorCode_t   tunerError = eTUNER_NOT_DONE;/*δ��*/
    TUNER_LOCKSTATUS_t  tunerState = eTun_unLock;
    U32 retry = 0;

    _convert_tuner_param(puTunerDesc, &u8TunerId, &tTsSrc);
        tTsSrc.u.Cable.Bandwidth = GetTunerBandWidth();
    tunerError = DRV_Tuner_SetFrequency(u8TunerId, &tTsSrc, 0);
	pbiinfo("DRV_Tuner_SetFrequency = %d,width = %d\n",tTsSrc.u.Cable.FrequencyKHertz,tTsSrc.u.Cable.Bandwidth);
    do
    {
        if( retry >= 20)
        {
            pbiinfo("lock tuner failed.\n");
            return 0;   /* NO LOCK*/
        }
        OS_TaskDelay(100);
        tunerState = DRV_Tuner_GetLockStatus(0);
        retry++;
    }
    while (tunerState != eTun_Lock);

    pbiinfo("lock tuner success.\n");
    return 1;       /* lock */
}


static SDT_Service_t * _find_program_in_sdt
(
    PSISI_Table_t * ptSDT,
    U16             u16ProgNum
)
{
    SDT_Section_t *     ptSDTSect = NULL;
    U32     i, j;

    if (NULL == ptSDT)
    {
        pbiinfo("[%s %d]-----\n",__FUNCTION__,__LINE__);
        return NULL;
    }

    for (i=0; i<ptSDT->u16SectionNum; i++)
    {
        ptSDTSect = (SDT_Section_t *)(ptSDT->ppSection[i]);
        if ((NULL == ptSDTSect) || (NULL == ptSDTSect->ptService))
        {
            continue;
        }

        for (j=0; j<ptSDTSect->u16ServiceNum; j++)
        {
            if (ptSDTSect->ptService[j].u16ServId == u16ProgNum)
            {
                return &(ptSDTSect->ptService[j]);
            }
        }
    }
    pbiinfo("[%s %d]-----\n",__FUNCTION__,__LINE__);
    return NULL;
}


static U8 _Bat_Find_Logical_Channel_Num( U16 u16TsId, U16 u16ServiceId, U16 *pLogicalNum )
{
    U16 ii = 0;
    U16 jj = 0;
    U16 kk =0;
    U16 ll = 0;

    BAT_Section_t *pBatSection = NULL;
//#if LW_BAT
    if( 0 == g_tInstallStatus.u8SearchLimitFlag )
    {
    if( NULL == g_tInstallStatus.ptBAT )
    {
        *pLogicalNum = 0xFFFF;
        return 0;
    }

    for(ii = 0; ii < g_tInstallStatus.ptBAT->u16RecievedNum; ii++)
    {
        pBatSection = g_tInstallStatus.ptBAT->ppSection[ii];

        for( jj = 0; jj < pBatSection->u16BouquetComNum; jj++ )
        {
            for(kk = 0; kk < pBatSection->pBouquetCom[jj].u16LwListNum; kk++ )
            {
                if( u16TsId != pBatSection->pBouquetCom[jj].u16TS_Id )
                {
                    //pbiinfo("[%s %d]----Tsid 0x%x V 0x%x------\n",DEBUG_LOG,u16TsId,pBatSection->pBouquetCom[jj].u16TS_Id);
                    continue;
                }

                for( ll = 0; ll < pBatSection->pBouquetCom[jj].pLwList[kk].u16ListNum; ll++ )
                {
                    if( u16ServiceId == pBatSection->pBouquetCom[jj].pLwList[kk].pLogicalInfo[ll].u16ServiceId )
                    {
                        if( 1 == pBatSection->pBouquetCom[jj].pLwList[kk].pLogicalInfo[ll].u8VisibleServicesFlag )
                        {
                            *pLogicalNum = pBatSection->pBouquetCom[jj].pLwList[kk].pLogicalInfo[ll].u16LogicalNum;
                            pbiinfo("TsId 0x%x ServiceId 0x%x  LogicalNum 0x%x\n",u16TsId,u16ServiceId,*pLogicalNum);
                            return 0;
                        }
                        else
                        {
                            *pLogicalNum = 0xFFFF;
                            pbiinfo("[%s %d]--VisibleServices is False!",__FUNCTION__,__LINE__);
                            return -1;
                        }
                    }
                }
            }
        }
    }

    *pLogicalNum = 0xFFFF;
    pbiinfo("[%s %d]--TsID 0x%x--ServID 0x%x not Find Logcal Num!",__FUNCTION__,__LINE__,u16TsId,u16ServiceId);
//#else
    }
    else
    {
    *pLogicalNum = u16ServiceId;
    g_count++;
    }
    return 0;
//#endif

    return -1;
}


static void _parse_service_of_tp
(
    Install_Tp_t *      ptTp,
    U32 *               pu32ServNum,
    Install_Serv_t **   pptServ
)
{
    PAT_Section_t *     ptPATSect = NULL;
    SDT_Section_t *     ptSDTSect = NULL;
    SDT_Service_t *     ptSDTServ = NULL;
    U32                 u32PAT_ProgNum = 0;
    U32                 u32NvodRefServNum = 0;

    _Install_Serv_Mem_t *   ptServMem = NULL;
    Install_Serv_t *        ptServ = NULL;
    U32     u32ServiceNum = 0;
    U32     u32ServMemSize = 0;
    U32     u32ServMemChunkSize = 0;

    U32     i, j;

    if (NULL == ptTp)
    {
        *pu32ServNum = 0;
        *pptServ = NULL;
        return;
    }

    /* ͳ�� PAT �еĽ�Ŀ����. ������ program_number Ϊ 0 �Ľ�Ŀ */
    if (ptTp->ptPAT != NULL)
    {
        for (i=0; i<ptTp->ptPAT->u16SectionNum; i++)
        {
            ptPATSect = (PAT_Section_t *)(ptTp->ptPAT->ppSection[i]);
            if ((NULL == ptPATSect) || (NULL == ptPATSect->ptProgram))
            {
                continue;
            }

            for (j=0; j<ptPATSect->u16ProgNum; j++)
            {
                if (ptPATSect->ptProgram[j].u16ProgramNumber != 0)
                {
                    u32PAT_ProgNum++;
                }
            }
        }
    }

    /* ͳ�� SDT �� NVOD �ο�ҵ������� */
    if (ptTp->ptSDT != NULL)
    {
        for (i=0; i<ptTp->ptSDT->u16SectionNum; i++)
        {
            ptSDTSect = (SDT_Section_t *)(ptTp->ptSDT->ppSection[i]);
            if ((NULL == ptSDTSect) || (NULL == ptSDTSect->ptService))
            {
                continue;
            }
            ptTp->u16TS_Id =ptSDTSect->u16TS_Id;//liqian 20120910
            ptTp->u16ON_Id =ptSDTSect->u16ON_Id;

            pbiinfo("-----------------%s=============tsid %d  ORNID %d\n",__FUNCTION__,ptTp->u16TS_Id,ptTp->u16ON_Id);
            for (j=0; j<ptSDTSect->u16ServiceNum; j++)
            {
                if (NULL == ptSDTSect->ptService[j].ptServDesc)
                {
                    continue;
                }

                if (SERVICE_TYPE_NVOD_REFER == ptSDTSect->ptService[j].ptServDesc->u8ServType)
                {
                    u32NvodRefServNum++;
                }
            }
        }
    }

    /* ��Ƶ���Ŀ����Ϊ PAT �еĽ�Ŀ�������� NVOD �ο�ҵ�������. �� NVOD �ο�
       ҵ����, ��� SDT �е�ҵ���� PAT ��û�ж�Ӧ����Ŀ, �򲻼����Ŀ���� */
    u32ServiceNum = u32PAT_ProgNum + u32NvodRefServNum;
    if (0 == u32ServiceNum)
    {
        *pu32ServNum = 0;
        *pptServ = NULL;
        return;
    }

    /* ����洢��Ŀ��Ϣ��Ҫ���ڴ�ռ�, Ϊ��Ŀ�б�����ڴ� */
    u32ServMemSize = u32ServiceNum * sizeof(Install_Serv_t);
    if ((NULL == g_tInstallStatus.ptServMem)
            || (g_tInstallStatus.ptServMem->u32RemainSize < u32ServMemSize))
    {
        u32ServMemChunkSize = u32ServMemSize + sizeof(_Install_Serv_Mem_t);
        if (u32ServMemChunkSize < SERVICE_MEM_CHUNK_SIZE)
        {
            u32ServMemChunkSize = SERVICE_MEM_CHUNK_SIZE;
        }
        ptServMem = (_Install_Serv_Mem_t *)OS_MemAllocate(system_p, u32ServMemChunkSize);
        if (NULL == ptServMem)
        {
            pbierror("allocate memory for service list failed.\n");
            *pu32ServNum = 0;
            *pptServ = NULL;
            return;
        }
        memset(ptServMem, 0, u32ServMemChunkSize);
        ptServMem->pNext = g_tInstallStatus.ptServMem;
        ptServMem->u32TotalSize = u32ServMemChunkSize;
        ptServMem->u32RemainSize = u32ServMemChunkSize - sizeof(_Install_Serv_Mem_t);
        ptServMem->u32ServNum = 0;
        g_tInstallStatus.ptServMem = ptServMem;
    }

    ptServ = g_tInstallStatus.ptServMem->ptService;
    ptServ += g_tInstallStatus.ptServMem->u32ServNum;

    /* ���� PAT �еĽ�Ŀ����Ŀ�б� */
    if (ptTp->ptPAT != NULL)
    {
        for (i=0; i<ptTp->ptPAT->u16SectionNum; i++)
        {
            ptPATSect = (PAT_Section_t *)(ptTp->ptPAT->ppSection[i]);
            if ((NULL == ptPATSect) || (NULL == ptPATSect->ptProgram))
            {
                continue;
            }

            for (j=0; j<ptPATSect->u16ProgNum; j++)
            {
                if (0 == ptPATSect->ptProgram[j].u16ProgramNumber)
                {
                    continue;
                }

                ptServ->u16ServId = ptPATSect->ptProgram[j].u16ProgramNumber;
                ptServ->u16PMTPid = ptPATSect->ptProgram[j].u16PMTPid;

                _Bat_Find_Logical_Channel_Num( ptPATSect->u16TS_Id, ptServ->u16ServId, &ptServ->u16LogicalNum );
                /* �� SDT ��Ѱ�Ҷ�Ӧ��ҵ��, ���û��, �� service_type,
                   free_ca_mode, service_name �ֶν���ΪĬ��ֵ */
                ptSDTServ = _find_program_in_sdt(ptTp->ptSDT, ptServ->u16ServId);
                if (NULL == ptSDTServ)
                {
                    ptServ->u8ServType = SERVICE_TYPE_NONE;
                    ptServ->u8FreeCA_Mode = 0;

                    //pbiinfo("zxguan[%s %d]---SDT Set Default Serv Name---\n",__FUNCTION__,__LINE__);

                    ptServ->sServName = DEFAULT_SERVICE_NAME;
                    ptServ->u32ServNameLen = strlen(DEFAULT_SERVICE_NAME);

                }
                else /* NULL != ptSDTServ */
                {
                    if (NULL == ptSDTServ->ptServDesc)
                    {
                        if (NULL == ptSDTServ->ptNvodTimeShiftDesc)
                        {
                            ptServ->u8ServType = SERVICE_TYPE_NONE;
                        }
                        else
                        {
                            ptServ->u8ServType = SERVICE_TYPE_NVOD_TIMESHIFT;
                        }

                        //pbiinfo("zxguan[%s %d]---SDT Set Default Serv Name---\n",__FUNCTION__,__LINE__);
                        ptServ->sServName = DEFAULT_SERVICE_NAME;
                        ptServ->u32ServNameLen = strlen(DEFAULT_SERVICE_NAME);

                    }
                    else /* NULL != ptSDTServ->ptServDesc */
                    {
                        ptServ->u8ServType = ptSDTServ->ptServDesc->u8ServType;
                        if ((NULL == ptSDTServ->ptServDesc->ptServName)
                                || (NULL == ptSDTServ->ptServDesc->ptServName->sText))
                        {

                            //pbiinfo("zxguan[%s %d]---SDT Set Default Serv Name---\n",__FUNCTION__,__LINE__);
                            ptServ->sServName = DEFAULT_SERVICE_NAME;
                            ptServ->u32ServNameLen = strlen(DEFAULT_SERVICE_NAME);

                        }
                        else
                        {
                            ptServ->uServNameType = (U8)ptSDTServ->ptServDesc->ptServName->u32CharSet;
                            ptServ->sServName = ptSDTServ->ptServDesc->ptServName->sText;
                            ptServ->u32ServNameLen = (U32)ptSDTServ->ptServDesc->ptServName->u8Len;
                        }
                    }
                    ptServ->u8FreeCA_Mode = ptSDTServ->u8FreeCA_Mode;
                }
                ptServ->ptTpDesc = ptTp;
                // pbiinfo("%s %d-----TsID 0x%x-----\n",DEBUG_LOG,ptTp->u16TS_Id);
                ptServ->ptServInfo = ptSDTServ;
                ptServ->ptPMT = NULL;
                ptServ++;
            }
        }
    }

    /* ���� NVOD �ο�ҵ�񵽽�Ŀ�б� */
    if (u32NvodRefServNum != 0)
    {
        for (i=0; i<ptTp->ptSDT->u16SectionNum; i++)
        {
            ptSDTSect = (SDT_Section_t *)(ptTp->ptSDT->ppSection[i]);
            if ((NULL == ptSDTSect) || (NULL == ptSDTSect->ptService))
            {
                continue;
            }

            for (j=0; j<ptSDTSect->u16ServiceNum; j++)
            {
                ptSDTServ = &(ptSDTSect->ptService[j]);
                if (NULL == ptSDTServ->ptServDesc)
                {
                    continue;
                }

                if (SERVICE_TYPE_NVOD_REFER == ptSDTServ->ptServDesc->u8ServType)
                {
                    ptServ->u16ServId = ptSDTServ->u16ServId;
                    ptServ->u16PMTPid = INVALID_PID;
                    ptServ->u8ServType = SERVICE_TYPE_NVOD_REFER;
                    ptServ->u8FreeCA_Mode = ptSDTServ->u8FreeCA_Mode;
                    if ((NULL == ptSDTServ->ptServDesc->ptServName)
                            || (NULL == ptSDTServ->ptServDesc->ptServName->sText))
                    {
                        ptServ->sServName = DEFAULT_SERVICE_NAME;
                        ptServ->u32ServNameLen = strlen(DEFAULT_SERVICE_NAME);
                    }
                    else
                    {
                        ptServ->sServName = ptSDTServ->ptServDesc->ptServName->sText;
                        ptServ->u32ServNameLen = ptSDTServ->ptServDesc->ptServName->u8Len;
                    }
                    ptServ->ptTpDesc = ptTp;
                    ptServ->ptServInfo = ptSDTServ;
                    ptServ->ptPMT = NULL;
                    ptServ++;
                }
            }
        }
    }

    ptServ = g_tInstallStatus.ptServMem->ptService;
    ptServ += g_tInstallStatus.ptServMem->u32ServNum;
    g_tInstallStatus.u32ServNum += u32ServiceNum;
    g_tInstallStatus.ptServMem->u32ServNum += u32ServiceNum;
    g_tInstallStatus.ptServMem->u32RemainSize -= u32ServMemSize;
    *pu32ServNum = u32ServiceNum;
    *pptServ = ptServ;

    return;
}



static void _convert_cab_desc
(
    DVBCore_Tuner_Desc_u *  puTunerDesc,
    Delivery_System_Desc_Cab_t *    ptCabDesc
)
{
    if ((NULL == puTunerDesc) || (NULL == ptCabDesc))
    {
        return;
    }

    puTunerDesc->tCable.u32Freq = ptCabDesc->u32Freq;
    puTunerDesc->tCable.u32SymbRate= ptCabDesc->u32SymRate;
    switch (ptCabDesc->u8Mod)
    {
    case CABLE_MOD_16QAM:
        puTunerDesc->tCable.eMod = eDVBCORE_MOD_16QAM;
        break;
    case CABLE_MOD_32QAM:
        puTunerDesc->tCable.eMod = eDVBCORE_MOD_32QAM;
        break;
    case CABLE_MOD_64QAM:
        puTunerDesc->tCable.eMod = eDVBCORE_MOD_64QAM;
        break;
    case CABLE_MOD_128QAM:
        puTunerDesc->tCable.eMod = eDVBCORE_MOD_128QAM;
        break;
    case CABLE_MOD_256QAM:
        puTunerDesc->tCable.eMod = eDVBCORE_MOD_256QAM;
        break;
    default:
        puTunerDesc->tCable.eMod = eDVBCORE_MOD_64QAM;
        break;
    }

    switch (ptCabDesc->u8FEC_Inner)
    {
    case FEC_INNER_CODE_RATE_1_2:
        puTunerDesc->tCable.eFEC_Inner = eDVBCORE_CONV_CODE_RATE_1_2;
        break;
    case FEC_INNER_CODE_RATE_2_3:
        puTunerDesc->tCable.eFEC_Inner = eDVBCORE_CONV_CODE_RATE_2_3;
        break;
    case FEC_INNER_CODE_RATE_3_4:
        puTunerDesc->tCable.eFEC_Inner = eDVBCORE_CONV_CODE_RATE_3_4;
        break;
    case FEC_INNER_CODE_RATE_5_6:
        puTunerDesc->tCable.eFEC_Inner = eDVBCORE_CONV_CODE_RATE_5_6;
        break;
    case FEC_INNER_CODE_RATE_7_8:
        puTunerDesc->tCable.eFEC_Inner = eDVBCORE_CONV_CODE_RATE_7_8;
        break;
    case FEC_INNER_NO_CONV_CODING:
        puTunerDesc->tCable.eFEC_Inner = eDVBCORE_NO_CONV_CODING;
        break;
    default:
        puTunerDesc->tCable.eFEC_Inner = eDVBCORE_CONV_CODE_RATE_AUTO;
        break;
    }

    switch (ptCabDesc->u8FEC_Outer)
    {
    case FEC_OUTER_NOT_DEFINED:
        puTunerDesc->tCable.eFEC_Outer = eDVBCORE_FEC_OUTER_NOT_DEFINED;
        break;
    case FEC_OUTER_NO_CODING:
        puTunerDesc->tCable.eFEC_Outer = eDVBCORE_FEC_OUTER_NO_CODING;
        break;
    case FEC_OUTER_RS_204_188:
        puTunerDesc->tCable.eFEC_Outer = eDVBCORE_FEC_OUTER_RS_204_188;
        break;
    default:
        puTunerDesc->tCable.eFEC_Outer = eDVBCORE_FEC_OUTER_NOT_DEFINED;
        break;
    }
}


static void _convert_sat_desc
(
    DVBCore_Tuner_Desc_u *  puTunerDesc,
    Delivery_System_Desc_Sat_t *    ptSatDesc
)
{
    if ((NULL == puTunerDesc) || (NULL == ptSatDesc))
    {
        return;
    }
}


static void _convert_ter_desc
(
    DVBCore_Tuner_Desc_u *  puTunerDesc,
    Delivery_System_Desc_Ter_t *    ptTerDesc
)
{
    if ((NULL == puTunerDesc) || (NULL == ptTerDesc))
    {
        return;
    }
}


static U8 _compare_tuner_desc
(
    DVBCore_Tuner_Desc_u *  puDesc1,
    DVBCore_Tuner_Desc_u *  puDesc2
)
{
    if (puDesc1->eSignalSource != puDesc2->eSignalSource)
    {
        return 1;
    }

    if (eDVBCORE_SIGNAL_SOURCE_DVBC == puDesc1->eSignalSource)
    {
        if ((puDesc1->tCable.u32Freq != puDesc2->tCable.u32Freq)
                || (puDesc1->tCable.u32SymbRate != puDesc2->tCable.u32SymbRate)
                || (puDesc1->tCable.eMod != puDesc2->tCable.eMod)
                || (puDesc1->tCable.u8TunerId != puDesc2->tCable.u8TunerId))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if ((eDVBCORE_SIGNAL_SOURCE_DVBS == puDesc1->eSignalSource)
             && (eDVBCORE_SIGNAL_SOURCE_DVBS2 == puDesc1->eSignalSource))
    {
    }
    else if ((eDVBCORE_SIGNAL_SOURCE_DVBT == puDesc1->eSignalSource)
             && (eDVBCORE_SIGNAL_SOURCE_DMBT == puDesc1->eSignalSource))
    {
    }
    else if (eDVBCORE_SIGNAL_SOURCE_ANALOG == puDesc1->eSignalSource)
    {
    }
    else
    {
    }

    return 1;
}


static U8 _search_main_tp(void)
{
    Install_Progress_t *    ptProgress = NULL;
    _Search_Tbl_Err_e		eErrCode = eSEARCH_TABLE_SUCCESS;
    _Search_Tbl_Param_t		tParam;

    U32     i;

    if (0 == g_tInstallStatus.u32MainTpNum)
    {
        return 0;
    }

    pbiinfo("\nsearch nit in main tp ...\n");
    ptProgress = &(g_tInstallStatus.tProgressInfo);
    for (i=0; i<g_tInstallStatus.u32MainTpNum; i++)
    {
        ptProgress->u32Progress = 0;
        ptProgress->puCurrTuner = &(g_tInstallStatus.puMainTpDescList[i]);
        if (g_tInstallStatus.pfnNotifyFunc != NULL)
        {
            g_tInstallStatus.pfnNotifyFunc(eINSTALL_PROGRESS, ptProgress);
        }

        if (0 == _install_set_frequency(&(g_tInstallStatus.puMainTpDescList[i])))
        {
            eErrCode = eSEARCH_TABLE_FAILED;
            continue;
        }

        tParam.u16Pid = NIT_PID;
        tParam.u8TableId = NIT_TABLE_ID;
        tParam.u16ExtendId = 0;
        tParam.u32TimeoutMs = g_tInstallStatus.tTimeout.u32NITActualTimeout;

        eErrCode = _install_Search_Table(tParam, &(g_tInstallStatus.ptNIT));

        if ( (eErrCode == eSEARCH_TABLE_SUCCESS)
                && (g_tInstallStatus.ptNIT != NULL) )
        {
            break;
        }
    }

    if ( (eErrCode != eSEARCH_TABLE_SUCCESS)
            || (NULL == g_tInstallStatus.ptNIT))
    {
        pbierror("NIT search, can not get NIT table of main tp.\n");
        return 0;
    }

    /* search NIT of main tp success. */
    return 1;
}


static U8 _search_BAT()
{
    _Search_Tbl_Err_e		eErrCode = eSEARCH_TABLE_SUCCESS;
    _Search_Tbl_Param_t		tParam;


    if (g_tInstallStatus.pfnNotifyFunc != NULL)
    {
//        g_tInstallStatus.pfnNotifyFunc(eINSTALL_PROGRESS, 0);
    }

    DVBCore_Tuner_Desc_u * puTunerDesc = NULL;
    if( 0 != g_tInstallStatus.u32MainTpNum )
    {
        puTunerDesc = &g_tInstallStatus.puMainTpDescList[0];
    }
    else
    {
        if( 0 != g_tInstallStatus.u32NormalTpNum )
        {
            puTunerDesc = &g_tInstallStatus.puNormalTpDescList[0];
        }
        else
        {
            pbierror("[%s %d] TunerParam is NULL!",__FUNCTION__,__LINE__);
            return 0;
        }
    }


    if (0 == _install_set_frequency(puTunerDesc) )
    {
        // eErrCode = eSEARCH_TABLE_FAILED;
        return 0;
    }

    tParam.u16Pid = BAT_PID;
    tParam.u8TableId = BAT_TABLE_ID;
    tParam.u16ExtendId = 192;
    tParam.u32TimeoutMs = g_tInstallStatus.tTimeout.u32BATTimeout;

    pbiinfo( "install_search_table\n");
    eErrCode = _install_Search_Table(tParam, &(g_tInstallStatus.ptBAT));

    if ( (eErrCode == eSEARCH_TABLE_SUCCESS)
            && (g_tInstallStatus.ptBAT != NULL) )
    {
        pbierror("BAT search success.  line %d.\n",__LINE__);
        return 1;
    }

    if ( (eErrCode != eSEARCH_TABLE_SUCCESS)
            || (NULL == g_tInstallStatus.ptBAT))
    {
        pbierror("BAT search error 0x%x. line %d.\n",eErrCode,__LINE__);
        return 0;
    }

    /* search BAT success. */
    return 1;
}
#if 1
static short  All_Search_TP_List[ ] = { 57,63,69,79,85,93,99,
105,141,147,153,159,165,171,177,183,189,195,
201,207,213,219,225,231,237,243,249,255,261,267,273,279,285,291,297,
303,309,315,321,327,333,339,345,351,357,363,369,375,381,387,393,399,
405,411,423,429,435,441,447,453,459,465,471,477,483,489,495,
501,507,513,519,525,531,537,543,549,555,561,567,573,579,585,591,597,
603,609,615,621,627,633,639,645,651,657,663,669,675,681,687,693,699,
705,711,723,729,735,741,747,753,759,765,771,777,783,789,795,
801,807,813,819,825,831,837,843,849,855,861
};
#else
static short  All_Search_TP_List[ ] = { 
783,789,795,801,807,813,819,825,831,837,843,849,855,861
};

#endif
static U32 _build_tp_list(void)
{
    PSISI_Table_t *         ptNIT = NULL;
    NIT_Section_t *         ptNitSect = NULL;
    NIT_TpStream_t *        ptNitTp = NULL;
    Install_Tp_t *          ptTpList = NULL;
    DVBCore_Signal_Source_e eCurrSigSource = eDVBCORE_SIGNAL_SOURCE_DVBC;
    U8                      u8CurrTunerId = 0;

    Delivery_System_Desc_Cab_t *    ptCabDesc = NULL;
    Delivery_System_Desc_Sat_t *    ptSatDesc = NULL;
    Delivery_System_Desc_Ter_t *    ptTerDesc = NULL;

    U32 u32MaxTpNum = 0;
    U32 u32TpNum = 0;
    U32 i, j;
if(g_tInstallStatus.eSearchType == eALL_SEARCH)
{
	u32TpNum = sizeof(All_Search_TP_List)/2;
	pbiinfo("all search total frequeycn  = %d\n",u32TpNum);
    /* ΪƵ���б�����ڴ� */
    ptTpList = (Install_Tp_t *)OS_MemAllocate(system_p, u32TpNum * sizeof(Install_Tp_t));
    if (NULL == ptTpList)
    {
        printf("allocate memory for tp_list failed.\n");
        return 0;
    }
    memset(ptTpList, 0, u32TpNum * sizeof(Install_Tp_t));
	for(i=0;i<u32TpNum;i++)
	{
		ptTpList[i].uTunerDesc.tCable.eSignalSource  = eDVBCORE_SIGNAL_SOURCE_DVBC;
		ptTpList[i].uTunerDesc.tCable.u32Freq = All_Search_TP_List[i]*1000;
		ptTpList[i].uTunerDesc.tCable.u32SymbRate = 5210;
		ptTpList[i].uTunerDesc.tCable.eMod = eDVBCORE_MOD_256QAM;
		ptTpList[i].uTunerDesc.tCable.Bandwidth = 0;	//6M
		ptTpList[i].uTunerDesc.tCable.u8TunerId = 0;
	}
	
}
else
{
    ptNIT = g_tInstallStatus.ptNIT;
    if (ptNIT != NULL)
    {
        eCurrSigSource = g_tInstallStatus.tProgressInfo.puCurrTuner->eSignalSource;
        u8CurrTunerId = g_tInstallStatus.tProgressInfo.puCurrTuner->tCable.u8TunerId;

        /* ���� NIT �е�Ƶ����� */
        for (i=0; i<ptNIT->u16SectionNum; i++)
        {
            ptNitSect = (NIT_Section_t *)(ptNIT->ppSection[i]);
            if ((ptNitSect != NULL) && (ptNitSect->ptTpStream != NULL))
            {
                u32MaxTpNum += ptNitSect->u16TpStreamNum;
            }
        }
        Set_Nit_Version(ptNitSect->u8Version);
        pbiinfo("!!!!!!!!!!!!!!!!!!!!!nit ver %x!!!!!!!!!!!!!!!!!!!!!!!!!!\n",ptNitSect->u8Version);
    }

    /* ��������ߴ������ͨƵ����� */
    u32MaxTpNum += g_tInstallStatus.u32NormalTpNum;

    if (0 == u32MaxTpNum)
    {
        return 0;
    }

    /* ΪƵ���б�����ڴ� */
    ptTpList = (Install_Tp_t *)OS_MemAllocate(system_p, u32MaxTpNum * sizeof(Install_Tp_t));
    if (NULL == ptTpList)
    {
        printf("allocate memory for tp_list failed.\n");
        return 0;
    }
    memset(ptTpList, 0, u32MaxTpNum * sizeof(Install_Tp_t));

    /* ���� NIT �е�Ƶ����Ϣ��Ƶ���б� */
    u32TpNum = 0;
    for (i=0; (ptNIT != NULL)&&(i<ptNIT->u16SectionNum); i++)
    {
        ptNitSect = (NIT_Section_t *)(ptNIT->ppSection[i]);
        if ((NULL == ptNitSect) || (NULL == ptNitSect->ptTpStream))
        {
            continue;
        }

        for (j=0; j<ptNitSect->u16TpStreamNum; j++)
        {
            ptNitTp = &(ptNitSect->ptTpStream[j]);
            if (NULL == ptNitTp->pDeliveryDesc)
            {
                continue;
            }

            ptCabDesc = (Delivery_System_Desc_Cab_t *)ptNitTp->pDeliveryDesc;
            ptSatDesc = (Delivery_System_Desc_Sat_t *)ptNitTp->pDeliveryDesc;
            ptTerDesc = (Delivery_System_Desc_Ter_t *)ptNitTp->pDeliveryDesc;

            switch (ptNitTp->u8DeliveryDescTag)
            {
            case CABLE_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
            {
                if (eCurrSigSource != eDVBCORE_SIGNAL_SOURCE_DVBC)
                {
                    continue;
                }

                ptTpList[u32TpNum].u16NetId = ptNitSect->u16NetworkId;
                ptTpList[u32TpNum].u16TS_Id = ptNitTp->u16TS_Id;
                ptTpList[u32TpNum].u16ON_Id = ptNitTp->u16ON_Id;
                _convert_cab_desc(&(ptTpList[u32TpNum].uTunerDesc), ptCabDesc);
                ptTpList[u32TpNum].uTunerDesc.tCable.eSignalSource = eCurrSigSource;
                ptTpList[u32TpNum].uTunerDesc.tCable.u8TunerId = u8CurrTunerId;
                u32TpNum++;
            }
            break;

            case SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
            {
                if ((eCurrSigSource != eDVBCORE_SIGNAL_SOURCE_DVBS)
                        && (eCurrSigSource != eDVBCORE_SIGNAL_SOURCE_DVBS2))
                {
                    continue;
                }

                ptTpList[u32TpNum].u16NetId = ptNitSect->u16NetworkId;
                ptTpList[u32TpNum].u16TS_Id = ptNitTp->u16TS_Id;
                ptTpList[u32TpNum].u16ON_Id = ptNitTp->u16ON_Id;
                _convert_sat_desc(&(ptTpList[u32TpNum].uTunerDesc), ptSatDesc);
                ptTpList[u32TpNum].uTunerDesc.tSatellite.eSignalSource = eCurrSigSource;
                ptTpList[u32TpNum].uTunerDesc.tSatellite.u8TunerId = u8CurrTunerId;
                u32TpNum++;
            }
            break;

            case TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
            {
                if ((eCurrSigSource != eDVBCORE_SIGNAL_SOURCE_DVBT)
                        && (eCurrSigSource != eDVBCORE_SIGNAL_SOURCE_DMBT))
                {
                    continue;
                }

                ptTpList[u32TpNum].u16NetId = ptNitSect->u16NetworkId;
                ptTpList[u32TpNum].u16TS_Id = ptNitTp->u16TS_Id;
                ptTpList[u32TpNum].u16ON_Id = ptNitTp->u16ON_Id;
                _convert_ter_desc(&(ptTpList[u32TpNum].uTunerDesc), ptTerDesc);
                ptTpList[u32TpNum].uTunerDesc.tTerrestrial.eSignalSource = eCurrSigSource;
                ptTpList[u32TpNum].uTunerDesc.tTerrestrial.u8TunerId = u8CurrTunerId;
                u32TpNum++;
            }
            break;

            default:
                break;
            }
        }
    }

    /* ���������ߴ������ͨƵ�㵽Ƶ���б� */
    for (i=0; i<g_tInstallStatus.u32NormalTpNum; i++)
    {
        memcpy(&(ptTpList[u32TpNum].uTunerDesc),
               &(g_tInstallStatus.puNormalTpDescList[i]),
               sizeof(DVBCore_Tuner_Desc_u));
        u32TpNum++;
    }

    if (0 == u32TpNum)
    {
        OS_MemDeallocate(system_p, ptTpList);
        return 0;
    }

    /* ȥ���ظ���Ƶ�� */
    u32MaxTpNum = u32TpNum;
    u32TpNum = 1;
    for (i=1; i<u32MaxTpNum; i++)
    {
        for (j=0; j<u32TpNum; j++)
        {
            if ( 0 == _compare_tuner_desc(&(ptTpList[j].uTunerDesc),
                                          &(ptTpList[i].uTunerDesc)) )
            {
                break;
            }
        }

        /* �����ظ�Ƶ�� */
        if (j == u32TpNum)
        {
            if (j < i)
            {
                memcpy(&(ptTpList[j]), &(ptTpList[i]), sizeof(Install_Tp_t));
            }
            u32TpNum++;
        }
    }
}
    g_tInstallStatus.u32TpNum = u32TpNum;
    g_tInstallStatus.ptTpList = ptTpList;

    return u32TpNum;
}

static U8 _search_each_tp(void)
{
    _Search_Tbl_Err_e       eErrCode = eSEARCH_TABLE_SUCCESS;
    _Search_Tbl_Param_t     tParam;
    Install_Tp_t *          ptTpList = NULL;
    U32                     u32TpNum = 0;
    Install_Progress_t *    ptProgress = NULL;

    U32                     u32ServNum = 0;
    Install_Serv_t *        ptServList = NULL;

    Install_Search_Result_t tTpResult;

    U32 i = 0, j = 0;

    ptTpList = g_tInstallStatus.ptTpList;
    u32TpNum = g_tInstallStatus.u32TpNum;
    ptProgress = &(g_tInstallStatus.tProgressInfo);
    if ((NULL == ptTpList) || (0 == u32TpNum))
    {
        return 0;
    }
	pbiinfo("_search_each_tp  = %d\n",u32TpNum);

    /* �������ÿ��Ƶ��Ľ�Ŀ */
    for (i=0; i<u32TpNum; i++)
    {
    	/* add by zxguan  */
		if( eSearchStop == g_tInstallStatus.eSearchStatus )
		{
			g_tInstallStatus.pfnNotifyFunc(eINSTALL_INTERRUPT, NULL);
			break;
		}
	
        ptProgress->u32Progress = (i * 100) / u32TpNum;
        ptProgress->puCurrTuner = &(ptTpList[i].uTunerDesc);
        if (g_tInstallStatus.pfnNotifyFunc != NULL)
        {
            g_tInstallStatus.pfnNotifyFunc(eINSTALL_PROGRESS, ptProgress);
        }

        if (0 == _install_set_frequency(ptProgress->puCurrTuner))
        {
            eErrCode = eSEARCH_TABLE_FAILED;
            g_tInstallStatus.pfnNotifyFunc(eINSTALL_TP_FAILED, NULL);
            continue;
        }

        /* Search PAT */
        tParam.u16Pid = PAT_PID;
        tParam.u8TableId = PAT_TABLE_ID;
        tParam.u16ExtendId = 0;
        tParam.u32TimeoutMs = g_tInstallStatus.tTimeout.u32PATTimeout;

        eErrCode = _install_Search_Table(tParam, &(ptTpList[i].ptPAT));
        if (eErrCode == eSEARCH_TABLE_STOP)
        {
            g_tInstallStatus.pfnNotifyFunc(eINSTALL_INTERRUPT, NULL);
            break;
        }

        if (eErrCode != eSEARCH_TABLE_SUCCESS)
        {
            g_tInstallStatus.pfnNotifyFunc(eINSTALL_TP_FAILED, NULL);
            continue;
        }

        /* Search SDT */
        tParam.u16Pid = SDT_PID;
        tParam.u8TableId = SDT_TABLE_ID;
        tParam.u16ExtendId = 0;
        tParam.u32TimeoutMs = g_tInstallStatus.tTimeout.u32SDTActualTimeout;

        eErrCode = _install_Search_Table(tParam, &(ptTpList[i].ptSDT));
        if (eErrCode == eSEARCH_TABLE_STOP)
        {
            g_tInstallStatus.pfnNotifyFunc(eINSTALL_INTERRUPT, NULL);
            break;
        }

        _parse_service_of_tp(&(ptTpList[i]), &u32ServNum, &ptServList);

#if USE_PMT
        /* Search PMT */
        for (j=0; j<u32ServNum; j++)
        {
            if (INVALID_PID == ptServList[j].u16PMTPid)
            {
                continue;
            }
            tParam.u16Pid = ptServList[j].u16PMTPid;
            tParam.u8TableId = PMT_TABLE_ID;
            tParam.u16ExtendId = ptServList[j].u16ServId;
            tParam.u32TimeoutMs = g_tInstallStatus.tTimeout.u32PMTTimeout;

            eErrCode = _install_Search_Table(tParam, &(ptServList[j].ptPMT));
            if (eErrCode == eSEARCH_TABLE_SUCCESS)
            {
            }
        }
#endif
#if 0//USE_BAT
        if( NULL == tTpResult.ptBAT )
        {
            `   tParam.u16Pid = BAT_PID;
            tParam.u8TableId = BAT_TABLE_ID;
            tParam.u16ExtendId = 0xC0; /* LianWei ����*/
            tParam.u32TimeoutMs = g_tInstallStatus.tTimeout.u32BATTimeout;

            eErrCode = _install_Search_Table(tParam, );
        }
#endif


        tTpResult.u32TpNum = 1;
        tTpResult.ptTpList = &(ptTpList[i]);
        tTpResult.u32ServNum = u32ServNum;
        tTpResult.ptServList = ptServList;
        tTpResult.ptNIT = NULL;
        tTpResult.ptBAT = NULL;
        g_tInstallStatus.pfnNotifyFunc(eINSTALL_TP_COMPLETED, &tTpResult);

    }

    ptProgress->u32Progress = 100;
    /* ptProgress->puCurrTuner Ϊ���һ��Ƶ�����Ϣ. �˴����ٸ�ֵ */
    if (g_tInstallStatus.pfnNotifyFunc != NULL)
    {
        g_tInstallStatus.pfnNotifyFunc(eINSTALL_PROGRESS, ptProgress);
    }

    return 1;
}


static U8 _collect_search_result(void)
{

    U32 ii = 0, jj = 0;

    _Install_Serv_Mem_t *pInstallServMem = NULL;



    if( g_tInstallStatus.u32ServNum > 0 )
    {
#if 0
        for( ii = 0; ii < g_tInstallStatus.u32ServNum; ii++ )
        {
            for( jj = 0; jj < g_tInstallStatus.ptServMem[ii].u32ServNum; jj++ )
            {
                pInstallServMem = &g_tInstallStatus.ptServMem[ii];
                _Bat_Find_Logical_Channel_Num( \
                                               pInstallServMem->ptService[jj].u16ServId, \
                                               &pInstallServMem->ptService[jj].u16LogicalNum);
            }
        }
#endif

        g_tInstallStatus.u8SearchRsltFlag = 1;
        return 1;
    }

    pbiinfo("Search Result is Null![%s--%d]\n",__FUNCTION__,__LINE__);
    return 0;
}




static U8 _search_all_tp(void)
{
    Install_Progress_t *    ptProgress = NULL;
    _Search_Tbl_Err_e		eErrCode = eSEARCH_TABLE_SUCCESS;
    _Search_Tbl_Param_t		tParam;

    U32     i;

    if (0 == g_tInstallStatus.u32MainTpNum)
    {
        return 0;
    }

    pbiinfo("\nsearch nit in main tp ...\n");
    ptProgress = &(g_tInstallStatus.tProgressInfo);
    for (i=0; i<g_tInstallStatus.u32MainTpNum; i++)
    {
        ptProgress->u32Progress = 0;
        ptProgress->puCurrTuner = &(g_tInstallStatus.puMainTpDescList[i]);
        if (g_tInstallStatus.pfnNotifyFunc != NULL)
        {
            g_tInstallStatus.pfnNotifyFunc(eINSTALL_PROGRESS, ptProgress);
        }

        if (0 == _install_set_frequency(&(g_tInstallStatus.puMainTpDescList[i])))
        {
            eErrCode = eSEARCH_TABLE_FAILED;
            continue;
        }

        tParam.u16Pid = NIT_PID;
        tParam.u8TableId = NIT_TABLE_ID;
        tParam.u16ExtendId = 0;
        tParam.u32TimeoutMs = g_tInstallStatus.tTimeout.u32NITActualTimeout;

        eErrCode = _install_Search_Table(tParam, &(g_tInstallStatus.ptNIT));

        if ( (eErrCode == eSEARCH_TABLE_SUCCESS)
                && (g_tInstallStatus.ptNIT != NULL) )
        {
            break;
        }
    }

    if ( (eErrCode != eSEARCH_TABLE_SUCCESS)
            || (NULL == g_tInstallStatus.ptNIT))
    {
        pbierror("NIT search, can not get NIT table of main tp.\n");
        return 0;
    }

    /* search NIT of main tp success. */
    return 1;
}



static U8 _process_all_search( void )
{
    Install_Search_Result_t tTpResult;

 //   if (0 == _search_all_tp())
    {
  //      pbiinfo("search NIT failed.\n");
        if (g_tInstallStatus.eSearchType != eALL_SEARCH)
        {
            return 0;
        }
    }

//#if LW_BAT
    if( 0 == g_tInstallStatus.u8SearchLimitFlag )
    {
    pbiinfo("\nsearch bat ...\n");
    if( 0 == _search_BAT () )
    {
        pbierror("\nsearch bat error!\n");
        return 0;
    }
    }
//#endif
    pbiinfo("\nbuild tp list ...\n");
    if (0 == _build_tp_list())
    {
        pbierror("build tp list failed.\n");
        return 0;
    }

    pbiinfo("\nsearch each tp ...\n");
    if (0 == _search_each_tp())
    {
        return 0;
    }

    pbiinfo("\ncollect search result ...\n");
    if (0 == _collect_search_result())
    {
        return 0;
    }

    tTpResult.u32TpNum = g_tInstallStatus.u32TpNum;
    tTpResult.ptTpList = g_tInstallStatus.ptTpList;
    tTpResult.u32ServNum = g_tInstallStatus.u32ServNum;
    tTpResult.ptServList = g_tInstallStatus.ptServMem->ptService;
    tTpResult.ptNIT = g_tInstallStatus.ptNIT;
    tTpResult.ptBAT = g_tInstallStatus.ptBAT;
    g_tInstallStatus.pfnNotifyFunc(eINSTALL_COMPLETED, &tTpResult);

    return 1;
}



static U8 _process_nit_search( void )
{
    Install_Search_Result_t tTpResult;

    if (0 == _search_main_tp())
    {
        pbiinfo("search NIT failed.\n");
        if (g_tInstallStatus.eSearchType != eALL_SEARCH)
        {
            return 0;
        }
    }

//#if LW_BAT
    if( 0 == g_tInstallStatus.u8SearchLimitFlag )
    {
    pbiinfo("\nsearch bat ...\n");
    if( 0 == _search_BAT () )
    {
        pbierror("\nsearch bat error!\n");
        return 0;
    }
    }
//#endif
    pbiinfo("\nbuild tp list ...\n");
    if (0 == _build_tp_list())
    {
        pbierror("build tp list failed.\n");
        return 0;
    }

    pbiinfo("\nsearch each tp ...\n");
    if (0 == _search_each_tp())
    {
        return 0;
    }

    pbiinfo("\ncollect search result ...\n");
    if (0 == _collect_search_result())
    {
        return 0;
    }

    tTpResult.u32TpNum = g_tInstallStatus.u32TpNum;
    tTpResult.ptTpList = g_tInstallStatus.ptTpList;
    tTpResult.u32ServNum = g_tInstallStatus.u32ServNum;
    tTpResult.ptServList = g_tInstallStatus.ptServMem->ptService;
    tTpResult.ptNIT = g_tInstallStatus.ptNIT;
    tTpResult.ptBAT = g_tInstallStatus.ptBAT;
    g_tInstallStatus.pfnNotifyFunc(eINSTALL_COMPLETED, &tTpResult);

    return 1;
}


static U8 _process_tp_search( void )
{
    Install_Search_Result_t tTpResult;

    pbiinfo("\nbuild tp list ...\n");
    if (0 == _build_tp_list())
    {
        printf("build tp list failed.\n");
        return 0;
    }
//#if LW_BAT
    if( 0 == g_tInstallStatus.u8SearchLimitFlag )
    {
    pbiinfo("\nsearch bat ...\n");
    if( 0 == _search_BAT () )
    {
        pbierror("\nsearch bat error!\n");
        return 0;
    }
    pbiinfo("search bat ok!!!!!!!!!!\n");
    }
//#endif
    pbiinfo("\nsearch each tp ...\n");
    if (0 == _search_each_tp())
    {
        return 0;
    }

    pbiinfo("\ncollect search result ...\n");
    if (0 == _collect_search_result())
    {
        return 0;
    }

    tTpResult.u32TpNum = g_tInstallStatus.u32TpNum;
    tTpResult.ptTpList = g_tInstallStatus.ptTpList;
    tTpResult.u32ServNum = g_tInstallStatus.u32ServNum;
    tTpResult.ptServList = g_tInstallStatus.ptServMem->ptService;
    tTpResult.ptNIT = g_tInstallStatus.ptNIT;
    tTpResult.ptBAT = g_tInstallStatus.ptBAT;
    g_tInstallStatus.pfnNotifyFunc(eINSTALL_COMPLETED, &tTpResult);

    return 1;
}


static void _installation_task(void* param)
{
    int ret = 0;
    while (1)
    {
        if (OS_SUCCESS == OS_SemWait(guInstall_Start_Sem))
        {
            if ( (eAUTO_SEARCH == g_tInstallStatus.eSearchType)
                    || (eQUICK_SEARCH == g_tInstallStatus.eSearchType)
                    || (eNIT_SEARCH == g_tInstallStatus.eSearchType))
            {

                ret = _process_nit_search();
                if( 0 == ret )
                {
                    g_tInstallStatus.pfnNotifyFunc(eINSTALL_FAILED, NULL);
                    goto OUT_INSTALLATION_TASK;
                }
            }   
             else if((eALL_SEARCH == g_tInstallStatus.eSearchType) )
             {
            	ret =   _process_all_search();
                if( 0 == ret )
                {
                    g_tInstallStatus.pfnNotifyFunc(eINSTALL_FAILED, NULL);
                    goto OUT_INSTALLATION_TASK;
                }
             
             }
            
            else if (eTP_SEARCH == g_tInstallStatus.eSearchType)
            {
                ret = _process_tp_search();
                if( 0 == ret )
                {
                    g_tInstallStatus.pfnNotifyFunc(eINSTALL_FAILED, NULL);
                    goto OUT_INSTALLATION_TASK;
                }
            }
            else
            {
                pbiinfo("Unknown search type.\n");
            }
OUT_INSTALLATION_TASK:
			g_tInstallStatus.eSearchStatus = eSearchStop;
            OS_SemSignal(g_InstallFinish_Sem);
        }
    }
}


static void _install_free_search_result(void)
{
    U32 i;

    g_tInstallStatus.u8SearchRsltFlag = 0;
    if (g_tInstallStatus.puMainTpDescList != NULL)
    {
        OS_MemDeallocate(system_p, g_tInstallStatus.puMainTpDescList);
        g_tInstallStatus.puMainTpDescList = NULL;
    }

    if (g_tInstallStatus.puNormalTpDescList != NULL)
    {
        OS_MemDeallocate(system_p, g_tInstallStatus.puNormalTpDescList);
        g_tInstallStatus.puNormalTpDescList = NULL;
    }
    while (g_tInstallStatus.ptServMem != NULL)
    {
        void * pTmp;

        for (i=0; i<g_tInstallStatus.ptServMem->u32ServNum; i++)
        {
            PSISI_Free_Table(g_tInstallStatus.ptServMem->ptService[i].ptPMT);
        }

        pTmp = g_tInstallStatus.ptServMem;
        g_tInstallStatus.ptServMem = (_Install_Serv_Mem_t *)(g_tInstallStatus.ptServMem->pNext);
        OS_MemDeallocate(system_p, pTmp);
    }

    if (g_tInstallStatus.ptTpList != NULL)
    {
        for (i=0; i<g_tInstallStatus.u32TpNum; i++)
        {
            PSISI_Free_Table(g_tInstallStatus.ptTpList[i].ptPAT);
             PSISI_Free_Table(g_tInstallStatus.ptTpList[i].ptSDT);
        }
        OS_MemDeallocate(system_p, g_tInstallStatus.ptTpList);
        g_tInstallStatus.ptTpList = NULL;
    }

    PSISI_Free_Table(g_tInstallStatus.ptNIT);
    while (g_tInstallStatus.ptBAT != NULL)
    {
        void * pTmp = g_tInstallStatus.ptBAT->pNextTable;
        PSISI_Free_Table(g_tInstallStatus.ptBAT);
        g_tInstallStatus.ptBAT = (PSISI_Table_t *)pTmp;
    }

    memset(&g_tInstallStatus, 0, sizeof(g_tInstallStatus));
}

void Installation_Null_Call_Back( Install_Notify_Type_e eNotifyType, void * pData)
{
    PBIDEBUG("Installation_Call_Back-------------------void");
}

/******************************************************************************
* Function:
*   Installation_Init
* Description:
*   �ú������ڳ�ʼ������ģ��.
* Parameter:
*   NULL
* Return:
*   PV_SUCCESS: success
*   PV_FAILURE: failure
* History :
*   2012-04-27 ZYJ
*       Initial.
*
*******************************************************************************/
static int g_InstallationInitFlag = 0;
S32 Installation_Init( void )
{
    S32 s32Err;
    if( 1 == g_InstallationInitFlag )
    {
        PBIDEBUG("Installation_Init again|!!!");
        return PV_SUCCESS;
    }
    //Msg_Socket_Client_Init();

    memset(&g_tInstallStatus, 0, sizeof(g_tInstallStatus));

    s32Err = OS_SemCreate(&guInstall_Start_Sem, "Install_Start_Sem", 0, 0);
    if(OS_SUCCESS != s32Err)
    {
        pbierror("Installation_Init, created semaphore guInstall_Start_Sem failed!\n");
        return PV_FAILURE;
    }

    s32Err = OS_SemCreate(&g_InstallFinish_Sem, "g_InstallFinish_Sem", 0, 0);
    if(OS_SUCCESS != s32Err)
    {
        pbierror("Installation_Init, created semaphore g_InstallFinish_Sem failed!\n");
        return PV_FAILURE;
    }

    s32Err = OS_TaskCreate(&guInstall_Task, "Install_Task", (void *(*)(void *))_installation_task,
                           NULL, NULL, guInstall_Stack_Size, guInstall_Priority, 0);
    if(OS_SUCCESS != s32Err)
    {
        pbierror("Installation_Init, created installation task failed!\n");
        OS_SemDelete(guInstall_Start_Sem);
        return PV_FAILURE;
    }

    g_InstallationInitFlag = 1;
    return PV_SUCCESS;
}


/******************************************************************************
* Function:
*   Installation_Start
* Description:
*   �ú�����������һ����������.
* Parameter:
*   tCfg: (I) installation ���ò���. ������ж��ƵĲ����������.
* Return:
*   PV_SUCCESS:       success
*   PV_INVALID_PARAM: ����Ƿ�
*   PV_NO_MEMORY:     �ڴ治��
* History :
*   2012-04-27 ZYJ
*       Initial.
*
*******************************************************************************/
S32 Installation_Start( Install_Cfg_t tCfg )
{
    DVBCore_Tuner_Desc_u *  puTunerDesc = NULL;
    U32 i = 0;

    SYS_BackGround_TableStop(SYS_BG_NIT);
    SYS_BackGround_TableStop(SYS_BG_PAT);
    _install_free_search_result();

    if (tCfg.eSearchType >= eOTHER_SEARCH)
    {
        return PV_INVALID_PARAM;
    }
    g_tInstallStatus.eSearchType = tCfg.eSearchType;

    if (tCfg.ptTimeout != NULL)
    {
        memcpy(&(g_tInstallStatus.tTimeout), tCfg.ptTimeout, sizeof(Install_Timeout_t));
    }
    else
    {
        g_tInstallStatus.tTimeout.u32TunerTimeout = 1000;
        g_tInstallStatus.tTimeout.u32NITActualTimeout = 10000;
        g_tInstallStatus.tTimeout.u32NITOtherTimeout = 10000;
        g_tInstallStatus.tTimeout.u32BATTimeout = 10000;
        g_tInstallStatus.tTimeout.u32PATTimeout = 5000;
        g_tInstallStatus.tTimeout.u32PMTTimeout = 5000;
        g_tInstallStatus.tTimeout.u32SDTActualTimeout = 10000;
        g_tInstallStatus.tTimeout.u32SDTOtherTimeout = 10000;
    }

    if ((tCfg.u32MainTpNum != 0) && (tCfg.puMainTpDescList != NULL))
    {
        puTunerDesc = OS_MemAllocate(system_p, tCfg.u32MainTpNum * sizeof(DVBCore_Tuner_Desc_u));
        if (NULL == puTunerDesc)
        {
            return PV_NO_MEMORY;
        }

        g_tInstallStatus.puMainTpDescList = puTunerDesc;
        for (i=0; i<tCfg.u32MainTpNum; i++)
        {
            memcpy(puTunerDesc, &(tCfg.puMainTpDescList[i]),
                   sizeof(DVBCore_Tuner_Desc_u));
            puTunerDesc++;
        }
        g_tInstallStatus.u32MainTpNum = tCfg.u32MainTpNum;
    }

    if ((tCfg.u32NormalTpNum != 0) && (tCfg.puNormalTpDescList != NULL))
    {
        puTunerDesc = OS_MemAllocate(system_p, tCfg.u32NormalTpNum * sizeof(DVBCore_Tuner_Desc_u));
        if (NULL == puTunerDesc)
        {
            return PV_NO_MEMORY;
        }

        g_tInstallStatus.puNormalTpDescList = puTunerDesc;
        for (i=0; i<tCfg.u32NormalTpNum; i++)
        {
            memcpy(puTunerDesc, &(tCfg.puNormalTpDescList[i]),
                   sizeof(DVBCore_Tuner_Desc_u));
            puTunerDesc++;
        }
        g_tInstallStatus.u32NormalTpNum = tCfg.u32NormalTpNum;
    }

    g_tInstallStatus.pfnNotifyFunc = tCfg.pfnNotifyFunc;
    g_tInstallStatus.u8SearchLimitFlag = GetSearchBatSwitch();//tCfg.u8SearchLimitFlag;
    //g_tInstallStatus.pfnNotifyFunc = Installation_Call_Back;

	g_tInstallStatus.eSearchStatus = eSearchRun;
    OS_SemSignal(guInstall_Start_Sem);
	
    if( 0 == g_tInstallStatus.u8SearchLimitFlag )
    {
        pbiinfo("\nThe search is limited.\n");
    }
    else
    {
        pbiinfo("\nWithout limiting the search.\n");
    }
    pbiinfo(" %s =============%d==========ok========",__FUNCTION__,__LINE__);
    return PV_SUCCESS;
}


/******************************************************************************
* Function:
*   Installation_Stop
* Description:
*   �ú��������ж�����.
* Parameter:
*   NULL
* Return:
*   PV_SUCCESS: success
*   PV_FAILURE: failure
* History :
*   2012-04-27 ZYJ
*       Initial.
*
*******************************************************************************/
S32 Installation_Stop( void )
{
    _Table_Msg_t	msg;
    memset( &msg, 0, sizeof(_Table_Msg_t) );
    msg.eMsgType = eINSTALL_MSG_STOP;

    int count = 0;
    g_tInstallStatus.pfnNotifyFunc = Installation_Null_Call_Back;
	g_tInstallStatus.eSearchStatus = eSearchStop;
#if 0 /* close by zxguan*/
	while(0xFFFFFFFF == guTable_Msg_Queue )
    {
        usleep(100);
        count++;
        if( count > 15)
        {
            return PV_FAILURE;
        }
    }
 #endif   
    OS_QueueSend(guTable_Msg_Queue, &msg, sizeof(_Table_Msg_t),
                 OS_PEND, 0);

    return PV_SUCCESS;
}


/******************************************************************************
* Function:
*   Installation_Get_SearchRet
* Description:
*   �ú������ڻ�ȡ�������.
* Parameter:
*   ptRet: (O) �����������.
* Return:
*   PV_SUCCESS: success
*   PV_FAILURE: û�п��õ��������
*   PV_NULL_PTR: null pointer err
* History :
*   2012-05-23 ZYJ
*       Initial.
*
*******************************************************************************/
S32 Installation_Get_SearchRet( Install_Search_Result_t *ptRet )
{
    if (NULL == ptRet)
    {
        return PV_NULL_PTR;
    }

    if (0 == g_tInstallStatus.u8SearchRsltFlag)
    {
        return PV_FAILURE;
    }

    pbiinfo("__%s____%d______servnum %d_____\n",__FUNCTION__,__LINE__,g_tInstallStatus.u32ServNum);
    ptRet->u32TpNum = g_tInstallStatus.u32TpNum;
    ptRet->ptTpList = g_tInstallStatus.ptTpList;
    ptRet->u32ServNum = g_tInstallStatus.u32ServNum;
    ptRet->ptServList = g_tInstallStatus.ptServMem->ptService;
    ptRet->ptNIT = g_tInstallStatus.ptNIT;
    ptRet->ptBAT = g_tInstallStatus.ptBAT;

    return PV_SUCCESS;
}


/******************************************************************************
* Function:
*   Installation_Free_SearchRet
* Description:
*   �ú��������ͷ��������.
* Parameter:
*   NULL
* Return:
*   NULL
* History :
*   2012-05-03 ZYJ
*       Initial.
*
*******************************************************************************/
void Installation_Free_SearchRet( void )
{
    _install_free_search_result();
}


/******************************************************************************
* Function:
*   Installation_Deinit
* Description:
*   �ú�������ȥ��ʼ������ģ��, �ͷ�����ģ��ռ�õ���Դ.
* Parameter:
*   NULL
* Return:
*   PV_SUCCESS: success
*   PV_FAILURE: failure
* History :
*   2012-04-27 ZYJ
*       Initial.
*
*******************************************************************************/
S32 Installation_Deinit( void )
{
    SYS_Table_Param_t bgfilterdata;
    S32 sRet  = 0;

    OS_SemWait_Timeout(g_InstallFinish_Sem,3500);

    _install_free_search_result();
    sRet = SYS_BackGround_TableStart(SYS_BG_NIT, bgfilterdata);
    if( 0 != sRet )
    {
        PBIDEBUG("SYS_BackGround_TableStart NIT error----------------");
    }
    sRet = SYS_BackGround_TableStart(SYS_BG_PAT, bgfilterdata);
    if( 0 != sRet )
    {
        PBIDEBUG("SYS_BackGround_TableStart PAT error----------------");
    }
    return PV_SUCCESS;
}

