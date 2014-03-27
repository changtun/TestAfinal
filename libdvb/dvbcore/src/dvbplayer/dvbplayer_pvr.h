/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : dvbplayer_pvr.h
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/12/1
  Last Modified :
  Description   : dvbplayer_pvr.c header file
  Function List :
  History       :
  1.Date        : 2012/12/1
    Author      : zxguan
    Modification: Created file

******************************************************************************/
#ifndef __DVBPLAYER_PVR_H__
#define __DVBPLAYER_PVR_H__

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
#include "drv_pvr.h"
#include "dvbplayer.h"
#include "psi_si.h"
typedef enum
{
    PVR_PLAY_POS_TYPE_SIZE,              
    PVR_PLAY_POS_TYPE_TIME,              
    PVR_PLAY_POS_TYPE_FRAME,             
    PVR_PLAY_POS_TYPE_BUTT               
} PVR_PLAY_POS_TYPE_E;


typedef struct Pvr_Info_s
{
   // HI_BOOL   useflag;
    U16     VideoPid;
    U16     AudioPid;
	U16		PCRPid;
    U32     avHandle;
    U32     DemuxID;
    U32     PortID;
    S32     AudioChnNum;
    U32     audiochannelhandle[16];
    U32     videochannelhandle;
	U32		pcrchannelhandle;
    U32     PmtHandle;
    U32     PatHandle;
    U32     recchannelID;
    U32     playchannelID;
    U8      filename[PVR_MAX_NAME_LENGTH];
    U8     pmtInfo[1024];	
    U16    serviceid;
    U16    pmtpid;	
   	
} PVR_INFO_t;

typedef enum
{
    PVR_VCODEC_TYPE_MPEG2,  
    PVR_VCODEC_TYPE_MPEG4,  
    PVR_VCODEC_TYPE_AVS,    
    PVR_VCODEC_TYPE_H263,   
    PVR_VCODEC_TYPE_H264,   
    PVR_VCODEC_TYPE_REAL8,  
    PVR_VCODEC_TYPE_REAL9,  
    PVR_VCODEC_TYPE_VC1,    
    PVR_VCODEC_TYPE_VP6,    
    PVR_VCODEC_TYPE_VP6F,   
    PVR_VCODEC_TYPE_VP6A,   
    PVR_VCODEC_TYPE_MJPEG,  
    PVR_VCODEC_TYPE_SORENSON, 
    PVR_VCODEC_TYPE_DIVX3,  
    PVR_VCODEC_TYPE_RAW,  
    PVR_VCODEC_TYPE_JPEG,  
    PVR_VCODEC_TYPE_BUTT
}PVR_VCODEC_TYPE_E;


typedef struct PVR_USER_DATA_S 
{
    U16 u16VidPid;
    U8  u8AudNum;
    U16 u16AudPid[16];
    U8  u8VidFmt;
    U8  u8AudFmt[16];
	U16 u16PcrPid;
    //U8  u8KeyLen;
    //U8  pKey[16];
    U8  pName[128];
}PVR_USER_DATA_T;
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

extern S32 DVBPlayer_PVR_DVB_ReStart();

extern S32 DVBPlayer_PVR_RecStart(DVBPlayer_PVR_REC_t *pParam);
extern S32 DVBPlayer_PVR_RecStop(U32 VidPid, U32 VidFmt, U32 AudPid, U32 AudFmt);

extern S32 DVBPlayer_PVR_PlayStart(U32 VidPid, U32 VidFmt, U32 AudPid, U32 AudFmt, C8 *pName, U8 u8UseEnctypt);
extern S32 DVBPlayer_PVR_PlayStop(U32 VidPid, U32 VidFmt, U32 AudPid, U32 AudFmt);
extern S32 DVBPlayer_PVR_PlayPause();
extern S32 DVBPlayer_PVR_PlayResume();
extern S32 DVBPlayer_PVR_PlaySpeedCtrl(PVR_SPEED_E ePlaySpeed);
extern S32 DVBPlayer_PVR_GetPlayFileInfo(DVBPlayer_PVR_FILE_ATTR_t *pPvrFileAttr);
extern S32 DVBPlayer_PVR_GetPlayStatus(DVBPlayer_PVR_PLAY_STATUS_t *pPvrPlayStatus);
extern S32 DVBPlayer_PVR_GetFileUserData(C8 * pName, C8 * pData, U32 * pDataLen);
extern S32 DVBPlayer_PVR_SetFileUserData( C8 *pName, C8 *pData, U32 DataLen );
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DVBPLAYER_PVR_H__ */
