/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : drv_pvr.h
  Version       : Initial Draft
  Author        : zshang
  Created       : 2013/11/27
  Last Modified :
  Description   : drv_pvr.c header file
  Function List :
  History       :
  1.Date        : 2013/11/27
    Author      : zshang
    Modification: Created file

******************************************************************************/
#ifndef __DRV_PVR_PLAYER_H__
#define __DRV_PVR_PLAYER_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define PVR_FILELEN 256
#define PLAYER_INJECT 0
#define PLAYER_TIMESHIFT 1
#define PVR_PLAYER_TYPE PLAYER_INJECT
#define COMPUTE_REAT
#if PVR_PLAYER_TYPE == PLAYER_INJECT
#define ONE_FRAME_SIZE(rate) (rate * 1024/8)
#define SKEK_SPEED(rate) (rate * 512/8)
#endif
typedef enum
{
	PLAYER_NO_ERROR = 0,
	PLAYER_ERROR
}Player_Error;

typedef enum
{
    PLAYER_STATE_INVALID = 0,
	PLAYER_STATE_INIT_OK,
    PLAYER_STATE_UNINIT,
    PLAYER_STATE_PLAY,
    PLAYER_STATE_PAUSE,
    PLAYER_STATE_FF,
    PLAYER_STATE_FB,
    PLAYER_STATE_SF,
    PLAYER_STATE_STEPF,
    PLAYER_STATE_STEPB,
    PLAYER_STATE_START,
    PLAYER_STATE_STOP,
    PLAYER_STATE_INJECT_END,
    PLAYER_STATE_END,
    PLAYER_STATE_ERROR,
    PLAYER_STATE_BUTT,
}PLAYER_STATE_E;

typedef struct FileInfo_s
{
	C8  pName[PVR_FILELEN];
	U64 uFileSize;
	U32 ufHandle;
	struct FileInfo_t* next;
	struct FileInfo_t* prev;	
}FileInfo_t;

typedef struct FileInfoList_s
{
	U32 uListLen;
	FileInfo_t* pHead;
	FileInfo_t* pTail;
}FileInfoList_t;


typedef struct pvr_player_info_s
{
	C8  pName[PVR_FILELEN];
	U32 uBitRate;
	U8 isDefRate;
	U64 uFileSize;
	U64 uFileTime;
	U32 uHeadIndex;
	U32 ufHandle;
	U8  isRunning;
	PLAYER_STATE_E player_state;
	pthread_t pThreadId;
#if (PVR_PLAYER_TYPE == PLAYER_TIMESHIFT)
	AM_AV_TimeshiftPara_t PlayerPara;
#elif (PVR_PLAYER_TYPE == PLAYER_INJECT)
	AM_AV_InjectPara_t PlayerPara;
	AM_AV_InjectType_t eInjectType;
	U8 MaxLevel;
	U8 MinLevel;
	struct timeval sCurTime;
	S32 sCorrectTime;
#endif
	U8 hasvid;
	U8 isHD;
	U8 hasaud;
	S8 uSpeed;
	U8  isEnctypt;
	U32 uSemId;
	KeyInfoList_t KeyList;
	FileInfo_t* pCurFile;
	FileInfoList_t FileList;
	S32 uVDscId;
	S32 uADscId;
}pvr_player_info_t;

typedef enum
{      
	/******************************
	* 0x1000x: 
	* player do parse file
	* decoder not running
	******************************/
	PLAYER_INITING  	= 0x10001,
	PLAYER_TYPE_REDY  = 0x10002,
	PLAYER_INITOK   	= 0x10003,	
        
	/******************************
	* 0x2000x: 
	* playback status
	* decoder is running
	******************************/
	PLAYER_RUNNING  	= 0x20001,
	PLAYER_BUFFERING 	= 0x20002,
	PLAYER_PAUSE    	= 0x20003,
	PLAYER_SEARCHING	= 0x20004,
	
	PLAYER_SEARCHOK 	= 0x20005,
	PLAYER_START    	= 0x20006,	
	PLAYER_FF_END   	= 0x20007,
	PLAYER_FB_END   	= 0x20008,

	PLAYER_PLAY_NEXT	= 0x20009,	
	PLAYER_BUFFER_OK	= 0x2000a,	
	PLAYER_FOUND_SUB	= 0x2000b,	
}pvr_player_status;

Player_Error DRV_PVR_PlayerInit(const *pName);
Player_Error DRV_PVR_PlayerUnInit();
Player_Error DRV_PVR_PlayerStart();
Player_Error DRV_PVR_PlayerPlay();
Player_Error DRV_PVR_PlayerPause();
Player_Error DRV_PVR_PlayerResume();
Player_Error DRV_PVR_PlayerStop();
Player_Error DRV_PVR_PlayerGetStatus(PVR_PLAY_STATUS_t *PVRState);
Player_Error DRV_PVR_PlayerGetFileAttr(PVR_FILE_ATTR_t *pPvrFileAttr);
Player_Error DVBPlayer_PVR_PlayerSpeed(PVR_SPEED_E ePlaySpeed);
Player_Error DRV_PVR_PlayerRegisterCallBack(EventCallBack cbf);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRV_PVR_PLAYER_H__ */

