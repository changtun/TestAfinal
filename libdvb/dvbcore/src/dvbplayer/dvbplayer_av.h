/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : dvbplayer_av.h
* Description :player 的av控制部分
* Author : LSZ
* History :
*	2007/12/28 : Initial Version
******************************************************************************/
#ifndef __DVBPLAYER_AV_H
#define __DVBPLAYER_AV_H
#include "drv_avctrl.h"

/*******************************************************/
/*              Includes				                   */
/*******************************************************/
/* include ANSI C .h file */
/* include OS, STAPI, Driver .h file */
/* include CA .h file */
/* include Public .h file */
/* include Local .h file */

#ifdef __cplusplus
extern "C" { 
#endif

/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/

/*******************************************************/
/*              Exported Types			                */
/*******************************************************/

/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/
U32 DVBPlayer_Start_Audio(void);
U32 DVBPlayer_Start_Video(void);
U32 DVBPlayer_Stop_Video( void);
U32 DVBPlayer_Stop_Audio( void);
U32 DVBPlayer_Enable_Screen(void);
U32 DVBPlayer_Blank_Screen(void);
U32 DVBPlayer_Start_Pcr(U16 uPcrPID);
U32 DVBPlayer_Stop_PCR( void);
U32 DVBPlayer_Start_TTX(U32 teletextpid, U32 magazine_number, U32 page_number);
U32 DVBPlayer_Stop_TTX(void);
/* xhren0710, 该接口暂时未实现*/
//U32 DVBPlayer_PAL_NTSC(DRV_AVCtrl_SysColorMode_t Mode );
U32 DVBPlayer_VID_SetIOWindows(DVBPlayer_VidOutWindow_t window_param);
U32 DVBPlayer_Set_ScreenRatio(DRV_AVCtrl_VidARatio_t  tSDVidAR );
U32 DVBPlayer_Start_Subtitle(U16 subtitle_pid,U16 composite_pid, U16 ancillary_pid);
U32 DVBPlayer_Stop_Subtitle(void);
U32 DVBPlayer_Check_Video_Status(void);
U32 DVBPlayer_Check_Audio_Status( void);
U32 DVBPlayer_Disable_Screen(void);
U32 DVBPlayer_Set_Video_StopMode(U32 mode);
U32 DVBPlayer_Set_Video_Volume( U32 Volume );
U32 DVBPlayer_Set_VPathPara( U32 isDvb );

//void DVBPlayer_AVEventCallBack( DRV_AVCtrl_EvtType_t tEvtType, void *EventData );
DRV_AVCtrl_ErrorCode_t DVBPlayer_SetAudioMode( DRV_AVCtrl_AudStereo_t tStereo );
DRV_AVCtrl_ErrorCode_t  DVBPlayer_GetAudioMode(  DRV_AVCtrl_AudStereo_t *mode);


DRV_AVCtrl_ErrorCode_t DVBPlayer_AVEventCallBack( DRV_AVCTRL_FRAME_INFO_t tFrameInfo );
void  DVBPlayer_Show_BroadcastBG(void);
void  DVBPlayer_Clear_BroadcastBG(void);




#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __DVBPLAYER_AV_H */
/* End of zap_parentrate.h  --------------------------------------------------------- */


