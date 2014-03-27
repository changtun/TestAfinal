/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : drv_pvr.h
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/12/2
  Last Modified :
  Description   : drv_pvr.c header file
  Function List :
  History       :
  1.Date        : 2012/12/2
    Author      : zxguan
    Modification: Created file

******************************************************************************/
#ifndef __DRV_PVR_H__
#define __DRV_PVR_H__
/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
#include "pvddefs.h"
/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/
#include "am_dvr.h"
#include "drv_avctrl.h"
#define PVR_FILELEN 512
#define PVR_DRIVER_NUM 2
#define TIMESHIFT_DRIVER_NUM 0

#define DVB_PVR_COUNT 3
AM_DVR_StartRecPara_t *Get_Pvr_Data_Para( );
#define PVR_MAX_FILE_LEN   128   /* HISI define */
#define MAX_KEY_LEN 	16
#define CONFIG_FIX		"config"
#define TIMESHIFT_FIX	"timeshift"

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/
typedef struct DRV_PVR_REC_ATTR_S
{
    U32     enIndexType;                           /**<Index type, static attribute.*//**<CNcomment:  索引类型，静态属性。 */
    U32     enIndexVidType;                        /**<Video encoding protocol with which the stream (for which an index is to be created) complies. The protocol (static attribute) needs to be set only when the index type is HI_UNF_PVR_REC_INDEX_TYPE_VIDEO.*//**<CNcomment: 待建立索引的码流的视频编码协议, 索引类型是HI_UNF_PVR_REC_INDEX_TYPE_VIDEO时才需要配置,静态属性  */
    U32     u32IndexPid;                           /**<Index PID, static attribute.*//**<CNcomment:  索引PID，静态属性*/
    U32     u32DemuxID;                            /**<Recording DEMUX ID, static attribute.*//**<CNcomment:  录制DEMUX号，静态属性。*/
    C8      szFileName[PVR_MAX_FILE_LEN];      /**<Name of a stream file, static attribute*//**<CNcomment: 码流文件名，静态属性。*/
    U32     u32FileNameLen;
    U32     u64MaxFileSize;            /* FreeFileSize  (M) */            /**<Size of the file to be recorded, static attribute. If the value is 0, it indicates that there is no limit on the file size. The maximum file size is 5242880 bytes (5 MB). If the disk space is sufficient, it is recommended to set the file size to a value greater than 512 MB. The file size cannot be 0 in rewind mode. *//**<CNcomment: 待录制的文件大小，为0时表示无限制，静态属性。最小值为50M,如果磁盘空间足够的话，推荐配置为512M以上。配置为回绕时不允许配置为0。 */
}DRV_PVR_REC_ATTR_t;


typedef enum
{
    PVR_PLAY_SPEED_32X_FAST_BACKWARD = 0,
    PVR_PLAY_SPEED_16X_FAST_BACKWARD,
    PVR_PLAY_SPEED_8X_FAST_BACKWARD,
    PVR_PLAY_SPEED_4X_FAST_BACKWARD,      
    PVR_PLAY_SPEED_2X_FAST_BACKWARD,
    PVR_PLAY_SPEED_NORMAL=5,                                
    PVR_PLAY_SPEED_2X_FAST_FORWARD,     
    PVR_PLAY_SPEED_4X_FAST_FORWARD,
    PVR_PLAY_SPEED_8X_FAST_FORWARD,
    PVR_PLAY_SPEED_16X_FAST_FORWARD,
    PVR_PLAY_SPEED_32X_FAST_FORWARD,
#if 0
    PVR_PLAY_SPEED_2X_SLOW_FORWARD,
    PVR_PLAY_SPEED_4X_SLOW_FORWARD,
    PVR_PLAY_SPEED_8X_SLOW_FORWARD,
    PVR_PLAY_SPEED_16X_SLOW_FORWARD, 
    PVR_PLAY_SPEED_32X_SLOW_FORWARD,
    PVR_PLAY_SPEED_2X_SLOW_BACKWARD,
    PVR_PLAY_SPEED_4X_SLOW_BACKWARD,
    PVR_PLAY_SPEED_8X_SLOW_BACKWARD,
    PVR_PLAY_SPEED_16X_SLOW_BACKWARD,
    PVR_PLAY_SPEED_32X_SLOW_BACKWARD,
 #endif   
}PVR_SPEED_E;


typedef enum
{
    PVR_PLAY_STATE_INVALID = 0,
    PVR_PLAY_STATE_INIT,
    PVR_PLAY_STATE_PLAY,
    PVR_PLAY_STATE_PAUSE,
    PVR_PLAY_STATE_FF,
    PVR_PLAY_STATE_FB,
    PVR_PLAY_STATE_SF,
    PVR_PLAY_STATE_STEPF,
    PVR_PLAY_STATE_STEPB,
    PVR_PLAY_STATE_STOP,
    PVR_PLAY_STATE_BUTT,
}PVR_PLAY_STATE_E;


typedef struct
{
    PVR_PLAY_STATE_E  enState;
    PVR_SPEED_E  enSpeed; 
    U64  u64CurPlayPos;
    U32  u32CurPlayFrame; 
    U32  u32CurPlayTimeInMs; 
}PVR_PLAY_STATUS_t;


typedef enum
{
    PVR_REC_INDEX_TYPE_NONE = 0,
    PVR_REC_INDEX_TYPE_VIDEO,
    PVR_REC_INDEX_TYPE_AUDIO,
    PVR_REC_INDEX_TYPE_BUTT,
}PVR_REC_INDEX_TYPE_E;

typedef struct
{
    PVR_REC_INDEX_TYPE_E  enIdxType;
    U32 u32FrameNum;
    U32 u32StartTimeInMs;
    U32 u32EndTimeInMs;
    U64 u64ValidSizeInByte;
}PVR_FILE_ATTR_t;

typedef void (*EventCallBack) (U32 u32ChnID, U32 EventType, S32 s32EventValue, void *args);

typedef enum
{
    PVR_REC_STATE_INVALID = 0,        
    PVR_REC_STATE_INIT,           
    PVR_REC_STATE_RUNNING,        
    PVR_REC_STATE_PAUSE,          
    PVR_REC_STATE_STOPPING,       
    PVR_REC_STATE_STOP,           
    PVR_REC_STATE_BUTT   
}PVR_REC_STATUS_E;

typedef struct
{
    PVR_REC_STATUS_E enState;
    U32 u32Reserved;
    U64 u64CurWritePos;
    U32 u32CurWriteFrame;
    U32 u32CurTimeInMs;
    U32 u32StartTimeInMs;
    U32 u32EndTimeInMs;
    U32 u32BufSize;
    U32 u32UsedSize;
}PVR_REC_STATUS_t;


typedef struct KeyInfo_s
{
	U64 uSetIndex; //file index
	U32  uPID;
	U8 uKeyType;
	U8  uKetLen;
	U8  isUser;
	U8  KeyData[MAX_KEY_LEN];
	struct KeyInfo_t* next;
	struct KeyInfo_t* prev;	
}KeyInfo_t;

typedef struct KeyInfoList_s
{
	U32 uListLen;
	KeyInfo_t* pHead;
	KeyInfo_t* pTail;
}KeyInfoList_t;

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/



#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern S32 DRV_PVR_Init();
extern S32 DRV_PVR_UnInit();
extern S32 DRV_PVR_MessageInit( EventCallBack pEventCallBack );
extern S32 DRV_PVR_MessageUnInit();
extern S32 DRV_PVR_RecGetStatus(PVR_REC_STATUS_t *ptRecStatus);

extern S32 DRV_PVR_RemoveFile(C8 *pName );

extern S32 DRV_PVR_PlayInit( U32 *pTsBuffer, U32 *pHandle, C8 *pName, U8 u8UseEnctypt );
extern S32 DRV_PVR_PlayPause();
extern S32 DRV_PVR_PlayResume();
extern S32 DRV_PVR_PlaySeek();
extern S32 DRV_PVR_PlayStart();
//extern S32 DRV_PVR_PlayStartTimeShift();
extern S32 DRV_PVR_PlayStop();
//extern S32 DRV_PVR_PlayStopTimeShift();
extern S32 DRV_PVR_PlayTrickMode(PVR_SPEED_E  ePlaySpeed);
extern S32 DRV_PVR_PlayGetStatus(PVR_PLAY_STATUS_t *pPvrPlayStatus);
extern S32 DRV_PVR_PlayGetFileAttr(PVR_FILE_ATTR_t *pPvrFileAttr);

extern S32 DRV_PVR_PlayUnInit();
extern S32 DRV_PVR_RecInit(  DRV_PVR_REC_ATTR_t *ptRecAttr, U16 LogicNum );
extern S32 DRV_PVR_RecStart();
extern S32 DRV_PVR_RecStop();
extern S32 DRV_PVR_RecUnInit();
extern void DRV_PVR_FreeCurKey();


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRV_PVR_H__ */
