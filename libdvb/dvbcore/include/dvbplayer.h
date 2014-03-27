/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : DVBPlayer ver 2.0
* File name   : dvbplayer.h
* Description : APIs of DVBPlayer.
*
* History :
*   2012-04-27 ZYJ
*       Initial Version.
*******************************************************************************/

#ifndef __DVBPLAYER_H_
#define __DVBPLAYER_H_

#include "pvddefs.h"
#include "dvbcore.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    eDVBPLAYER_ASPECT_RATIO_AUTO = 0,      /* ����Ӧ */
    eDVBPLAYER_ASPECT_RATIO_4TO3,          /* 4 : 3 */
    eDVBPLAYER_ASPECT_RATIO_16TO9,         /* 16 : 9 */
    eDVBPLAYER_ASPECT_RATIO_221TO1,        /* 2.21 : 1 */
    eDVBPLAYER_ASPECT_RATIO_SQUARE         /* 1 : 1 */
} DVBPlayer_Vid_AspectRatio_e;

typedef enum
{
    eDVBPLAYER_VID_AUTO = 0,            /* ����Ӧ */
    eDVBPLAYER_VID_NTSC,                /* NTSC ��ʽ */
    eDVBPLAYER_VID_PAL                  /* PAL ��ʽ */
} DVBPlayer_Vid_System_e;

/*
typedef enum
{
    eDVBPLAYER_VID_RES_1080i = 0,
    eDVBPLAYER_VID_RES_1080P,
    eDVBPLAYER_VID_RES_720P,
} DVBPlayer_Vid_Resolution_e;
*/

typedef enum
{
    eDVBPLAYER_VID_ACVRS_IGNORE=0,
    eDVBPLAYER_VID_ACVRS_LETTERBOX=1,
    eDVBPLAYER_VID_ACVRS_PANANDSCAN
} DVBPlayer_VidOutMode_e;


typedef enum
{
    eDVBPLAYER_SYNC_REF_NONE = 0,        /* ���ɲ��� */
    eDVBPLAYER_SYNC_REF_AUDIO,           /* ͬ������Ƶ */
    eDVBPLAYER_SYNC_REF_VIDEO,           /* ͬ������Ƶ */
    eDVBPLAYER_SYNC_REF_PCR,             /* ͬ���� PCR */
} DVBPlayer_Sync_Type_e;


typedef enum
{
    eDVBPLAYER_SERV_NO_LOCK = 0,         /* ��Ŀδ���� */
    eDVBPLAYER_SERV_LOCK = 1             /* ��Ŀ���� */
} DVBPlayer_Serv_Lock_e;


typedef enum
{
    eDVBPLAYER_DISABLED = 0,
    eDVBPLAYER_ENABLED
} DVBPlayer_Ctrl_Flag_e;


typedef enum DVBPlayer_VidZoomMode_e
{
    eDVBPLAYER_VID_ZOOM_NONE = 0,
    eDVBPLAYER_VID_ZOOM_BY_2,
    eDVBPLAYER_VID_ZOOM_BY_2_5,
    eDVBPLAYER_VID_ZOOM_BY_3,
    eDVBPLAYER_VID_ZOOM_BY_3_5,
    eDVBPLAYER_VID_ZOOM_BY_4
} DVBPlayer_VidZoomMode_e;

/* ͼ�����λ�õĽṹ�� */
typedef struct DVBPlayer_VidOutWindow_s
{
	DVBPlayer_VidZoomMode_e	eZoomMode;
	S32 					s32PositionX;
	S32 					s32PositionY;
	U32 					u32WinWidth;
	U32 					u32WinHight;
} DVBPlayer_VidOutWindow_t;

typedef enum
{
    eDVBPLAYER_ATTR_VID_SYSTEM = 0,
    eDVBPLAYER_ATTR_VID_ASPECT_RATIO,
    eDVBPLAYER_ATTR_VID_WINDWOS,
    eDVBPlayer_ATTR_VID_STOP_MODE,
    eDVBPLAYER_ATTR_SWITCH_AUDIOPID,
    eDVBPLAYER_ATTR_AUD_SETVOLUME,
    eDVBPLAYER_ATTR_AUD_MUTE,
    eDVBPLAYER_ATTR_AUD_MODE,
    eDVBPLAYER_ATTR_PVR_GET_FILE_INFO,
    eDVBPLAYER_ATTR_PVR_GET_STATUS,
    eDVBPLAYER_ATTR_SET_VPARA,
} DVBPlayer_Attr_e;



typedef enum
{
    eDVBPLAYER_PROG_TYPE_SERVICE = 0,
    eDVBPLAYER_PROG_TYPE_AVPID
} DVBPlayer_Prog_Type_e;

typedef struct
{
    DVBPlayer_Prog_Type_e       eProgType;  /* DVBPLAYER_PROG_TYPE_SERVICE */
    DVBPlayer_Serv_Lock_e       eServLockFlag;
    U16                         u16ServId;
    U16                         u16PMTPid;
    U16                         u16TsId;
    U16                         u16NetId;
    U16                        u16LogicNum;
	U16 						u16historyAudioPid;
} DVBPlayer_Serv_Param_t;

typedef struct
{
    DVBPlayer_Prog_Type_e       eProgType;  /* DVBPLAYER_PROG_TYPE_AVPID */
    U16                         u16AudioPid;
    U16                         u16VideoPid;
    U16                         u16PCRPid;
    U8                          u8AudioFmt;
    U8                          u8VideoFmt;
} DVBPlayer_AVPid_t;
typedef struct
{
    U16                         u16AudioPid;
    U16                         u16audiotype;
} DVBPlayer_SWITCH_AUDIOPID_t;

typedef union
{
    DVBPlayer_Prog_Type_e       eProgType;
    DVBPlayer_Serv_Param_t      tServParam;
    DVBPlayer_AVPid_t           tAVPids;
} DVBPlayer_Prog_Param_u;


typedef struct
{
    DVBCore_Tuner_Desc_u        uTunerParam;
    DVBPlayer_Prog_Param_u      uProgParam;
} DVBPlayer_Play_Param_t;



typedef enum
{
    eDVBPLAYER_RUNNING_STATUS_STOP = 0,
    eDVBPLAYER_RUNNING_STATUS_PREPARE,
    eDVBPLAYER_RUNNING_STATUS_PLAY
} DVBPlayer_Running_Status_e;

typedef enum
{
    eDVBPLAYER_TUNER_UNLOCK = 0,
    eDVBPLAYER_TUNER_LOCKED
} DVBPlayer_Tuner_Status_e;

typedef enum
{
    eDVBPLAYER_PMT_STOP = 0,
    eDVBPLAYER_PMT_CHECK,
    eDVBPLAYER_PMT_WAITING,
    eDVBPLAYER_PMT_RECEIVED,
} DVBPlayer_PMT_Status_e;


typedef struct
{
    DVBPlayer_Running_Status_e      eRunningStatus;
    DVBPlayer_Tuner_Status_e        eTunerStatus;
    DVBPlayer_Prog_Type_e           eProgType;
    U16                             u16ServiceId;
    U16                             u16PMTPid;
    DVBPlayer_PMT_Status_e          ePMTStatus;
    U16                             u16VideoPid;
    U16                             u16AudioPid;
    U16                             u16PCRPid;
} DVBPlayer_Info_t;


typedef struct
{
    U16 u16TsId;
    U16 u16ServiceId;
    U16 u16PmtPid;
}DVBPlayer_Update_PMT_t;

typedef enum
{
    eDVBPLAYER_EVENT_INVALID = 0,
    eDVBPLAYER_EVENT_SET_SERV,
    eDVBPLAYER_EVENT_SET_AVPID,
    eDVBPLAYER_EVENT_PLAY_START,
    eDVBPLAYER_EVENT_TUNER_DROPPED,
    eDVBPLAYER_EVENT_TUNER_LOCK,
    eDVBPLAYER_EVENT_PMT_TIMEOUT,
    eDVBPLAYER_EVENT_AV_INTERRUPT, /*����Ƶ�ж�*/
    eDVBPLAYER_EVENT_AUDIO_INFOS,
    eDVBPLAYER_EVENT_CA_MODE,      /*PMT�л�ȡ�Ľ�Ŀ����״̬*/
    eDVBPLAYER_EVENT_UPDATE_PMT,
    eDVBPLAYER_EVENT_CA_CHANNEL_INFO
} DVBPlayer_Event_Type_e;


typedef struct
{
    DVBPlayer_Event_Type_e      eEventType;
    U32                         u32EventLen;
    void *                      pEventData;
} DVBPlayer_Event_t;

typedef struct 
{
    U16 wChannelId;
    U8 bState;
    U8 demuxid;	    
}PostChanChange_t;

typedef struct
{
    U32 u32IdxType;
    U32 u32FrameNum;
    U32 u32StartTimeInMs;
    U32 u32EndTimeInMs;
    U64 u64ValidSizeInByte;
}DVBPlayer_PVR_FILE_ATTR_t;

typedef struct
{
    U32  u32State;
    U32  u32Speed; 
    U64  u64CurPlayPos;
    U32  u32CurPlayFrame; 
    U32  u32CurPlayTimeInMs; 
}DVBPlayer_PVR_PLAY_STATUS_t;

typedef S32 (* DVBPlayer_Callback_pfn)( DVBPlayer_Event_t tEvent );


typedef struct
{
    DVBPlayer_Vid_System_e      *peVideoSystem;         /* ��Ƶ��ʽ */
    DVBPlayer_Vid_AspectRatio_e *peAspectRatio;         /* �����Ļ�Ŀ�߱� */
    DVBPlayer_VidOutWindow_t    *ptVidOutWin;           /* Ҫ����ͼ������λ�ü��ߴ� */
    DVBPlayer_Sync_Type_e       *peSyncType;            /* ����Ƶͬ������ */
    DVBPlayer_Ctrl_Flag_e       *peParentCtrlFlag;      /* �ҳ����ƿ��� */
    U8                          *pu8ParentRating;       /* �ҳ����Ƽ��� */
} DVBPlayer_Config_t;

typedef S32 (* CA_ServiceFunction_t)( U16 pid, U16 service_id);

typedef struct 
{
    CA_ServiceFunction_t   pfServiceStart;
    CA_ServiceFunction_t   pfServiceStop;
}DVBPlayer_CA_Fun_t;

typedef enum
{
    PVR_EVENT_TYPE_PLAY_EOF        = 0x001,
    PVR_EVENT_TYPE_PLAY_SOF        = 0x002,
    PVR_EVENT_TYPE_PLAY_ERROR      = 0x003,
    PVR_EVENT_TYPE_PLAY_REACH_REC  = 0x004,
    PVR_EVENT_TYPE_PLAY_RESV       = 0x00f,
    PVR_EVENT_TYPE_REC_DISKFULL    = 0x010,
    PVR_EVENT_TYPE_REC_ERROR       = 0x011,
    PVR_EVENT_TYPE_REC_OVER_FIX    = 0x012,
    PVR_EVENT_TYPE_REC_REACH_PLAY  = 0x013,
    PVR_EVENT_TYPE_REC_DISK_SLOW   = 0x014,
    PVR_EVENT_TYPE_REC_RESV        = 0x01f,
    PVR_EVENT_TYPE_BUTT            = 0x020,
} DVBPlayer_PVR_EVENT_TYPE_E;

typedef void (*DvbPvrEventCallBack) (U32 u32ChnID, DVBPlayer_PVR_EVENT_TYPE_E eEventType, S32 s32EventValue, void *args);

#define PVR_MAX_NAME_LENGTH   128

typedef struct 
{
   //U16 u16PmtPid;
   //U16 u16LogicNum;
   DVBCore_Tuner_Desc_u uTunerDesc;
}DVB_PVR_BACK_REC_t;

typedef struct
{
    U32 u32AudPid;
    U32 u32AudFmt;
    U32 u32VidPid;
    U32 u32VidFmt;
	U32 u32PCRPid;
    
}DVB_PVR_TIME_REC_t;

typedef struct DVBPlayer_PVR_REC_S
{
    U8 u8RecFlag;
    U16 u16PmtPid;
    U16 u16ServId;
    U16 u16LogicNum;
    DVB_PVR_BACK_REC_t tBackRec;
    DVB_PVR_TIME_REC_t tTimeRec;
    
    C8 FileName[PVR_MAX_NAME_LENGTH]; 
    U64 u64MaxFileLength;
}DVBPlayer_PVR_REC_t;

typedef enum
{
    PVR_PLYAER_DVB = 0,
    PVR_PLAYER_Play = 1,
    PVR_PLAYER_MenuPlay = 2,
}PVR_PLAYER_TYPE_E;

typedef struct
{
    PVR_PLAYER_TYPE_E PvrPlayType;
    C8 FileName[PVR_MAX_NAME_LENGTH];
}DVBPlayer_PVR_Play_t;

typedef enum
{
    DVB_PVR_REC_STATE_INVALID,        
    DVB_PVR_REC_STATE_INIT,           
    DVB_PVR_REC_STATE_RUNNING,        
    DVB_PVR_REC_STATE_PAUSE,          
    DVB_PVR_REC_STATE_STOPPING,       
    DVB_PVR_REC_STATE_STOP,           
    DVB_PVR_REC_STATE_BUTT   
}DVBPlayer_PVR_REC_STATUS_E;

typedef struct
{
    DVBPlayer_PVR_REC_STATUS_E enState;
    U32 u32Reserved;
    U64 u64CurWritePos;
    U32 u32CurWriteFrame;
    U32 u32CurTimeInMs;
    U32 u32StartTimeInMs;
    U32 u32EndTimeInMs;
    U32 u32BufSize;
    U32 u32UsedSize;
}DVBPlayer_PVR_REC_STATUS_t;



S32 DVBTable_Init( void );

S32 DVBPlayer_Init( DVBPlayer_Config_t * pConfig );

S32 DVBPlayer_Deinit( void );

S32 DVBPlayer_Set_Attr( DVBPlayer_Attr_e Attr, void * pParam );

S32 DVBPlayer_Get_Attr( DVBPlayer_Attr_e Attr, void * pParam );

S32 DVBPlayer_RegCallback( DVBPlayer_Callback_pfn pfnCallback );


S32 DVBPlayer_Play_Program( DVBPlayer_Play_Param_t * pParam );

S32 DVBPlayer_Stop();


S32 DVBPlayer_Get_Info( DVBPlayer_Info_t * pInfo );

S32 DVBPlayer_CA_Function_Init(DVBPlayer_CA_Fun_t CaFun);
S32 DVBPlayer_RegCACallback( DVBPlayer_Callback_pfn caCallback );

S32 CheckEPG( U16 u16ServId, U16 u16TsId, U16 u16NetId );
int DVBPlayer_GetCamode();
S32 DVBPlayer_Get_RunningStatus();
void Reset_ServiceID(void);


//extern S32 DVBPlayer_PVR_Init();
//extern S32 DVBPlayer_PVR_UnInit();
extern S32 DVBPlayer_PVR_MessageInit( DvbPvrEventCallBack pCallBack );
extern S32 DVBPlayer_PVR_MessageUnInit();
extern S32 DVBPlayer_REC_Play(DVBPlayer_PVR_REC_t *pRecInfo);
extern S32 DVBPlayer_REC_Stop();
extern S32 DVBPlayer_PVR_RecGetStatus( DVBPlayer_PVR_REC_STATUS_t *ptRecStatus );

extern S32 DVBPlayer_PVR_PvrRemoveFile( C8 *pFileName );

extern S32 DVBPlayer_PVR_Play(DVBPlayer_PVR_Play_t *pPlayerInfo);
extern S32 DVBPlayer_PVR_Stop();
extern S32 DVBPlayer_PVR_Pause();
extern S32 DVBPlayer_PVR_Resume();
extern S32 DVBPlayer_PVR_Speed(S32 Speed);
extern S32 DVBPlayer_PVR_Seek(S32 pos);

U32 dal_http_start(void);

U32 dal_http_stop(void);
U32 dal_http_start2(void);
U32 dal_http_restart(void);



#ifdef __cplusplus
}
#endif

#endif /* __DVBPLAYER_H_ */

