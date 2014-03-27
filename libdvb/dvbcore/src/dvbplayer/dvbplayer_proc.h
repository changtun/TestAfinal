/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : DVBPlayer ver 2.0
* File name   : dvbplayer_proc.h
* Description : Definitions of action message, status.
*
* History :
*   2012-05-23 ZYJ
*       Initial Version.
*******************************************************************************/
#ifndef __DVBPLAYER_PROC_H_
#define __DVBPLAYER_PROC_H_

#include "dvbplayer.h"
#include "psi_si.h"
#include "dvbplayer_pvr.h"

#ifdef __cplusplus
extern "C" {
#endif




typedef enum
{
    eDVBPLAYER_ACTION_INVALID = 0,
    eDVBPLAYER_ACTION_PLAY,
    eDVBPLAYER_ACTION_STOP,
    eDVBPLAYER_ACTION_SET_ATTR,
    eDVBPLAYER_ACTION_REC_PLAY,
    eDVBPLAYER_ACTION_REC_STOP,
    eDVBPLAYER_ACTION_PVR_PLAY,
    eDVBPLAYER_ACTION_PVR_STOP,
    eDVBPLAYER_ACTION_PVR_PAUSE,
    eDVBPLAYER_ACTION_PVR_RESUME,
    eDVBPLAYER_ACTION_PVR_SPEED,
} DVBPlayer_Action_Type_e;


typedef struct
{
    DVBPlayer_Attr_e            eAttrType;
    union
    {
        DVBPlayer_Vid_System_e      eVideoSystem;       /* 视频制式 */
        DVBPlayer_Vid_AspectRatio_e eAspectRatio;       /* 输出屏幕的宽高比 */
        DVBPlayer_VidOutWindow_t    tVidOutWin;         /* 要设置图像的输出位置及尺寸 */
        DVBPlayer_Sync_Type_e       eSyncType;          /* 音视频同步类型 */
        DVBPlayer_Ctrl_Flag_e       eParentCtrlFlag;    /* 家长控制开关 */
    } attr;
} DVBPlayer_Attr_Param_t;

typedef enum
{
	ePROGRAM_FREE =0,
	ePROGRAM_PAY

}DVBPlayer_CaStatus;

typedef struct
{
    DVBPlayer_PVR_REC_t tPvrRec;
    DVBPlayer_PVR_Play_t tPvrPlay;
    PVR_SPEED_E ePvrSpeed;
}DVBPlayer_PVR_Param_t;

typedef union
{
    DVBPlayer_Play_Param_t      tPlayData;
    DVBPlayer_Attr_Param_t      tAttrData;
  //  DVBPlayer_PVR_Param_t     tPvrData;
} DVBPlayer_Action_Param_u;



typedef enum
{
    eDVBPLAYER_STOP = 0,
    eDVBPLAYER_PLAY
} DVBPlayer_Component_Status_e;


typedef struct
{
    U16                             u16ServiceId;
    U16 					u16LogicId;
    U16                             u16PMTPid;
    U16                             u16NetId;
    U16                             u16TsId;
    DVBPlayer_Serv_Lock_e           eServLockFlag;
    DVBPlayer_PMT_Status_e          ePMTStatus;
    U16                             u16PCRPid;
    U16                             u16VideoPid;
    U16                             u16AudioPid;
    U8                              u8VideoFmt;
    U8                              u8AudioFmt;
    DVBPlayer_Component_Status_e    eVideoStatus;
    DVBPlayer_Component_Status_e    eAudioStatus;
    DVBPlayer_Component_Status_e    ePCRStatus;
    DVBPlayer_Component_Status_e    eStillStatus;
    DVBPlayer_Component_Status_e    eTTXStatus;
    DVBPlayer_Component_Status_e    eSubtitleStatus;
	DVBPlayer_CaStatus				ecamode;/*liqian*/
	U16                  			u16History_audioPID;
    PSISI_Table_t *                 ptPMTTable;
} DVBPlayer_Prog_Status_t;


typedef struct
{
    DVBPlayer_Callback_pfn          pfnNotifyFunc;
	DVBPlayer_Callback_pfn			caCallback;//liqian 
    DVBPlayer_Running_Status_e      eRunningStatus;
    DVBPlayer_Tuner_Status_e        eTunerStatus;
    DVBCore_Tuner_Desc_u            uTunerParam;
    DVBPlayer_Prog_Status_t         tProgStatus;
    DVBPlayer_Vid_System_e          eVideoSystem;       /* 视频制式 */
    DVBPlayer_Vid_AspectRatio_e     eAspectRatio;       /* 输出屏幕的宽高比 */
    DVBPlayer_VidOutWindow_t        tVidOutWin;         /* 要设置图像的输出位置及尺寸 */
    DVBPlayer_Sync_Type_e           eSyncType;          /* 音视频同步类型 */
    DVBPlayer_Ctrl_Flag_e           eParentCtrlFlag;    /* 家长控制开关 */
    DVBPlayer_Ctrl_Flag_e           eVideoCtrlFlag;
    DVBPlayer_Ctrl_Flag_e           eAudioCtrlFlag;
    DVBPlayer_Ctrl_Flag_e           eStillCtrlFlag;
    DVBPlayer_Ctrl_Flag_e           eTTXCtrlFlag;
    DVBPlayer_Ctrl_Flag_e           eSubtitleCtrlFlag;
    U8	  pmtbuf[1024];	
    U8      change_flag;//0,跨频点，1同频点	
    
} DVBPlayer_Status_t;

void DvbPlayerMessageSend(DVBPlayer_Action_Type_e type, DVBPlayer_Action_Param_u param1, void* param2);
U32 GetTunerFreq();
U32 CheckChannel(U16 u16PmtPid, U16 u16ServId);
void CA_DVBPlay_CallBack( DVBPlayer_Event_t tEvent);

S32 GetPlayerStatus(DVBPlayer_Status_t  *pStatus );

#ifdef __cplusplus
}
#endif

#endif /* __DVBPLAYER_PROC_H_ */

