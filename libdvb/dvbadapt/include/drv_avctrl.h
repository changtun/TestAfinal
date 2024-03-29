/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : drv_avctrl.h
* Description : Includes all other AVCTRL driver header files.
              	This allows a AVCTRL application to include only this one header
              	file directly. 
* Author : zheng dong sheng
* History :
*	2010/06/08 : Initial Version
******************************************************************************/

#ifndef _DRV_AVCTRL_H_
#define _DRV_AVCTRL_H_
 #ifdef __cplusplus
extern "C" {
#endif

/*******************************************************/
/*              Includes				                                        */
/*******************************************************/
#include "pvddefs.h" 
#include "am_av.h"
#include "am_aout.h"
#include "am_vout.h"

/*******************************************************/
/*              Exported Defines and Macros                                     */
/*******************************************************/
/*******************************************************/
/*              Exported Types			                                        */
/*******************************************************/

typedef enum DRV_AVCtrl_ErrorCode_e
{
	AVCTRL_NO_ERROR = 0,
	AVCTRL_ERROR
}DRV_AVCtrl_ErrorCode_t;

typedef enum DRV_AVCtrl_VIDFormat_e
{
	AVCTRL_VID_FORMAT_MPEG2,
	AVCTRL_VID_FORMAT_MPEG4,
	AVCTRL_VID_FORMAT_AVS,
	AVCTRL_VID_FORMAT_H263,
	AVCTRL_VID_FORMAT_H264,
	AVCTRL_VID_FORMAT_REAL8,
	AVCTRL_VID_FORMAT_REAL9,
	AVCTRL_VID_FORMAT_VC1,
	AVCTRL_VID_FORMAT_DIVX3,
	AVCTRL_VID_FORMAT_BULL
}DRV_AVCtrl_VIDFormat_t;



typedef struct DRV_AVCtrl_VidWindow_e
{
    S32                 uX_Axis;
    S32                 uY_Axis;
    S32                 uWidth;
    S32                 uHeight;
    S32                 uBaseWidth;
    S32                 uBaseHeight;
}DRV_AVCtrl_VidWindow_t;

typedef enum DRV_AVCtrl_VidStandard_e
{
    AVCTRL_VID_STANDARD_UNKNOWN = 0,
    AVCTRL_VID_STANDARD_PAL,
    AVCTRL_VID_STANDARD_PAL_N,
    AVCTRL_VID_STANDARD_PAL_Nc,
    AVCTRL_VID_STANDARD_SECAM_SIN,
    AVCTRL_VID_STANDARD_SECAM_COS,
    AVCTRL_VID_STANDARD_NTSC,
    AVCTRL_VID_STANDARD_NTSC_J,
    AVCTRL_VID_STANDARD_NTSC_PAL_M,
    AVCTRL_VID_STANDARD_AUTO,
    AVCTRL_VID_STANDARD_BUTT
}DRV_AVCtrl_VidStandard_t;


typedef enum DRV_AVCtrl_VidResolution_e
{
    AVCTRL_VID_RESOLUTION_UNKNOWN = 0,
	AVCTRL_VID_RESOLUTION_1080P,
	AVCTRL_VID_RESOLUTION_1080I,
	AVCTRL_VID_RESOLUTION_720P,
	AVCTRL_VID_RESOLUTION_576P,
	AVCTRL_VID_RESOLUTION_480P,
	AVCTRL_VID_RESOLUTION_BUTT
}DRV_AVCtrl_VidResolution_t;


typedef enum DRV_AVCtrl_VidFrame_Type_e
{
    AVCTRL_VID_FRAME_TYPE_UNKNOWN,
    AVCTRL_VID_FRAME_TYPE_I,
    AVCTRL_VID_FRAME_TYPE_P,
    AVCTRL_VID_FRAME_TYPE_B,
    AVCTRL_VID_FRAME_TYPE_BUTT
}DRV_AVCtrl_VidFrame_Type_t;

typedef enum DRV_AVCtrl_VidARatio_e /* Aspect Ratio */
{
	AVCTRL_VID_AR_AUTO  = 0,
	AVCTRL_VID_AR_4TO3   = 1,
	AVCTRL_VID_AR_16TO9  = 2,	
	AVCTRL_VID_AR_SQUARE = 3,
	AVCTRL_VID_AR_14TO9 = 4,
	AVCTRL_VID_AR_221TO1 = 5,
	AVCTRL_VID_AR_BUTT
} DRV_AVCtrl_VidARatio_t;

typedef enum DRV_AVCtrl_VIDACvrs_e
{
    AVCTRL_VID_ACVRS_IGNORE = 0x0,
    AVCTRL_VID_ACVRS_LETTERBOX,
    AVCTRL_VID_ACVRS_PANANDSCAN,
    AVCTRL_VID_ACVRS_COMBINED,
    AVCTRL_VID_ACVRS_MODE_BUTT
} DRV_AVCtrl_VIDACvrs_t;

typedef enum DRV_AVCtrl_VIDOUT_INTERFACE_e
{
	AVCTRL_VID_OUTPUT_INTERFACE_CVBS_YPbPr = 0x0,
	AVCTRL_VID_OUTPUT_INTERFACE_CVBS_SVIDEO,
	AVCTRL_VID_OUTPUT_INTERFACE_CVBS,
	AVCTRL_VID_OUTPUT_INTERFACE_NONE
}DRV_AVCtrl_VIDOUT_INTERFACE_t;

typedef enum DRV_AVCtrl_Vid_Stop_Mode_e
{
	AVCTRL_VID_STOP_MODE_STILL = 0,  /* stop 后静帧 */
	AVCTRL_VID_STOP_MODE_BLACK = 1,  /* stop 后黑屏 */
	AVCTRL_VID_STOP_MODE_BUTT
} DRV_AVCtrl_Vid_Stop_Mode_t;

typedef enum DRV_AVCtrl_AudFormat_e
{
	AVCTRL_AUD_FORMAT_AAC=0,
	AVCTRL_AUD_FORMAT_AAC_RAW,
	AVCTRL_AUD_FORMAT_MPEG,
	AVCTRL_AUD_FORMAT_AC3,
	AVCTRL_AUD_FORMAT_EAC3,	
	AVCTRL_AUD_FORMAT_PCM,
	AVCTRL_AUD_FORMAT_BULL
} DRV_AVCtrl_AudFormat_t;

typedef enum DRV_AVCtrl_AudStereo_e
{
	AUD_STEREO_STEREO,	
	AUD_STEREO_LEFT_ONLY,
	AUD_STEREO_RIGHT_ONLY,
	AUD_STEREO_MONO,
	AUD_STEREO_BUTT
}DRV_AVCtrl_AudStereo_t;

typedef enum DRV_AVCtrl__EVENT_e
{
    AVCtrl__EVENT_EOS = 0,				/*文件播放结束*/
    AVCtrl__EVENT_STOP = 1,				/* 停掉音视频解码器*/
    AVCtrl__EVENT_DISP = 2,				/*subtitle数据显示*/
    AVCtrl__EVENT_RNG_BUF_STATE = 3, 	/* 媒体缓存队列状态变化*/
    AVCtrl__EVENT_NORM_SWITCH = 4,		/* P/N 制切换*/
    AVCtrl__EVENT_NEW_VID_FRAME = 5,	/* 新视频帧*/
    AVCtrl__EVENT_NEW_AUD_FRAME = 6,	/* 新音频帧*/
    AVCtrl__EVENT_NEW_USER_DATA = 7,	/* 新的用户数据*/
    AVCtrl__EVENT_VID_STREAM = 8,		/* 视频PES BUF事件*/
    AVCtrl__EVENT_BUTT					/*无效事件*/
}DRV_AVCtrl__EVENT_t;
//{{{ add by LYN,2011/03/26
#define DRV_AVCTRL_PLAY_SPEED_NORMAL (1024)
typedef enum DRV_AVCTRL_PLAY_SPEED_E
{
    DRV_AVCTRL_PLAY_SPEED_2X_FAST_FORWARD   = 2 * DRV_AVCTRL_PLAY_SPEED_NORMAL,      /**< 2倍速快放 */
    DRV_AVCTRL_PLAY_SPEED_4X_FAST_FORWARD   = 4 * DRV_AVCTRL_PLAY_SPEED_NORMAL,      /**< 4倍速快放 */
    DRV_AVCTRL_PLAY_SPEED_8X_FAST_FORWARD   = 8 * DRV_AVCTRL_PLAY_SPEED_NORMAL,      /**< 8倍速快放 */
    DRV_AVCTRL_PLAY_SPEED_16X_FAST_FORWARD  = 16 * DRV_AVCTRL_PLAY_SPEED_NORMAL,     /**< 16倍速快放 */
    DRV_AVCTRL_PLAY_SPEED_32X_FAST_FORWARD  = 32 * DRV_AVCTRL_PLAY_SPEED_NORMAL,     /**< 32倍速快放 */
	DRV_AVCTRL_PLAY_SPEED_1X_FAST_FORWARD	= DRV_AVCTRL_PLAY_SPEED_NORMAL,
	DRV_AVCTRL_PLAY_SPEED_2X_FAST_BACKWARD  = -2 * DRV_AVCTRL_PLAY_SPEED_NORMAL,     /**< 2倍速快退 */
    DRV_AVCTRL_PLAY_SPEED_4X_FAST_BACKWARD  = -4 * DRV_AVCTRL_PLAY_SPEED_NORMAL,     /**< 4倍速快退 */
    DRV_AVCTRL_PLAY_SPEED_8X_FAST_BACKWARD  = -8 * DRV_AVCTRL_PLAY_SPEED_NORMAL,     /**< 8倍速快退 */
    DRV_AVCTRL_PLAY_SPEED_16X_FAST_BACKWARD = -16 * DRV_AVCTRL_PLAY_SPEED_NORMAL,    /**< 16倍速快退 */
    DRV_AVCTRL_PLAY_SPEED_32X_FAST_BACKWARD = -32 * DRV_AVCTRL_PLAY_SPEED_NORMAL,    /**< 32倍速快退 */
    DRV_AVCTRL_PLAY_SPEED_BUTT                                                          /**< 无效的速度值 */
} DRV_AVCTRL_PLAY_SPEED_E;

//}}}add by LYN,2011/03/26
typedef struct DRV_AVCTRL_VIDEO_FRAME_e
{
	U32							uiVidFrameIndex;
	DRV_AVCtrl_VidARatio_t		eVideoARatio;
	DRV_AVCtrl_VidStandard_t		eVideoStandard;
	DRV_AVCtrl_VidFrame_Type_t	eVideoFrameType;
}DRV_AVCTRL_VIDEO_FRAME_t;

typedef struct DRV_AVCTRL_AUDIO_FRAME_e
{
	U32 uiAudFrameIndex;
}DRV_AVCTRL_AUDIO_FRAME_t;

typedef struct DRV_AVCTRL_FRAME_INFO_e
{
	DRV_AVCTRL_VIDEO_FRAME_t	tVideoFrameInfo;
	DRV_AVCTRL_AUDIO_FRAME_t	tAudioFrameInfo;
	DRV_AVCtrl_VidStandard_t		eVideoStandard;
	U32 event_type;
}DRV_AVCTRL_FRAME_INFO_t;

typedef struct DRV_AVCTRL_STATUS_AVCOUNT_INFO_s
{
	U32 u32VidFrameCount;
	U32 u32AuddFrameCount;
	U32 u32VidErrorFrameCount;
}DRV_AVCTRL_STATUS_AVCOUNT_INFO_t;
typedef DRV_AVCtrl_ErrorCode_t (*DRV_AVCTRL_EVENT_CALLBACK_FUN)( DRV_AVCTRL_FRAME_INFO_t tVideoFrameInfo );
/*******************************************************/
/*              Exported Variables		                                        */
/*******************************************************/

/*******************************************************/
/*              External				                                        */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                                        */
/*******************************************************/

U8 DRV_AVCtrl_InitFlag();
U8 Drv_AVCtrl_ModeType();
U32 *DRV_AVCtrl_GetAVHandle();

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_PVR_Stop();
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Init( U32 DemuxId );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_UnInit( void );

DRV_AVCtrl_ErrorCode_t  DRV_AVCtrl_Vid_Start( DRV_AVCtrl_VIDFormat_t eVidFmt );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_Set_StopMode( DRV_AVCtrl_Vid_Stop_Mode_t mode );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_Get_StopMode( DRV_AVCtrl_Vid_Stop_Mode_t *mode );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_Stop( void );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_SetPID( U32 uiVidPID );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_ClearPID( void );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_SetIOWindow( DRV_AVCtrl_VidWindow_t tVidOut_Window );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_GetIOWindow( DRV_AVCtrl_VidWindow_t* tVidOut_Window  );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_SetAlpha( U32 uiAlpha );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_GetAlpha( U32* uiAlpha );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_SetAspectRatio( DRV_AVCtrl_VidARatio_t  eVidAsRatio );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_GetAspectRatio( DRV_AVCtrl_VidARatio_t*  eVidAsRatio );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_SetAspectCvrs( DRV_AVCtrl_VIDACvrs_t  eVidAsCvrs );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_GetAspectCvrs( DRV_AVCtrl_VIDACvrs_t*  eVidAsCvrs );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_EnableOutput( void );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_DisableOutput( void );
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_EnableOutput( void );
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_DisableOutput( void );
//HI_BOOL DRV_AVCTR_GetVid_Status( void );//add by xyfeng

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_PCR_SetPID( U32 uiPcrPID );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_PCR_ClearPID( void );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_Start( DRV_AVCtrl_AudFormat_t eAudFmt );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_Stop( void );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_SetPID( U32 uiAudPID );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_ClearPID( void );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_SetVolume( U32 u32Volume );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_Mute( void );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_UnMute( void );


DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_SetStereo( DRV_AVCtrl_AudStereo_t eStereo );
/*added by zhengdongsheng to get audio pes account on 10.09,.29*/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Aud_GetPesAccount( U32 *puiAudPesCount );
/*added by zhengdongsheng to get audio pes account on 10.09,.29*/
DRV_AVCtrl_ErrorCode_t   DRV_AVCtrl_Set_PlayPause( void );

DRV_AVCtrl_ErrorCode_t   DRV_AVCtrl_Set_PlayResume( void );
/*{{{ xhren 0720*/
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Set_Format(DRV_AVCtrl_VidStandard_t tFormat);
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Get_Format(DRV_AVCtrl_VidStandard_t* pFormat);
/*xhren0720}}}*/

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_StartTs( );
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_StopTs( );


DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Set_Resolution(DRV_AVCtrl_VidResolution_t resolution);

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Get_Resolution(DRV_AVCtrl_VidStandard_t *pResolution);


DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Vid_SetOutputInterface( 
						DRV_AVCtrl_VIDOUT_INTERFACE_t eVidOut_Interface );

void DRV_AVCtrl_Regist_CallBackFun( DRV_AVCTRL_EVENT_CALLBACK_FUN FEvent_CallBack_Fun );

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Test(void);

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Set_Brightness(U32 brightness);
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Set_Contrast(U32 contrast);
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Set_Saturation(U32 Saturationess);

//DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Still_Start( HI_UNF_AVPLAY_I_FRAME_S *piframe );
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Still_Stop( void );
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Still_Stop_ByMode(  U8 mode  );//add by xyfeng

U32 DRV_AVCtrl_Get_AVPlayHandle(void);

DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_Get_Status_Info(DRV_AVCTRL_STATUS_AVCOUNT_INFO_t *tCountInfo);

DRV_AVCtrl_AudFormat_t _DRV_AVCTRL_GetAudFmt( void );
DRV_AVCtrl_VIDFormat_t _DRV_AVCTRL_GetVidFmt( void );
U32 DRV_AVCtrl_Aud_GetPID( void );
U32 DRV_AVCtrl_Vid_GetPID( void );
void DRV_AVCtrl_CloseResample();
AM_AV_AFormat_t DRV_AVCtrl_SwitchAudFmt( DRV_AVCtrl_AudFormat_t eAudFmt );
AM_AV_VFormat_t DRV_AVCtrl_SwitchVidFmt(DRV_AVCtrl_VIDFormat_t eVidFmt);
DRV_AVCtrl_ErrorCode_t DRV_AVCtrl_SetVPathPara(int isDvb);




#ifdef __cplusplus
}
#endif

#endif  /*_DRV_AVCTRL_H_*/
/* End of drv_avctrl.h  --------------------------------------------------------- */
