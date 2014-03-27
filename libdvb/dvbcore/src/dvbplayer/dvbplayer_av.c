/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name : Dvbplayer_av.c
* Description : player对音视频的操作模块.
* History :
*	Date               Modification                                Name
*	----------         ------------                                ----------
*	2007/09/10         Created                                     LSZ
*   2007/11/26         Modified 								   LL/ZBL
******************************************************************************/
/*******************************************************/
/*              Includes				               */
/*******************************************************/
/* Standard headers */

/* ST_API headers */
/* include ANSI C .h file */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* include OS, STAPI file */
/* include Driver .h file */
/*

#include "Tuner_Adapt.h"
#include "pv_e2p.h"
*/
/* include System layer .h file */
#include "pvddefs.h"
#include "osapi.h"
//#include "pbi_systrace.h"
#include "drv_avctrl.h"
/* include Middle layer .h file */
/* include CA .h file */
/* include Local .h file */

#include "background.h"
#include "dvbplayer.h"
#include "tmsgqueue.h"
#include "dvbplayer_proc.h"
#include "dvbplayer_av.h"
#include "dvbplayer_callbackandpmt.h"

#include "tlock.h"
#include "pbitrace.h"
#define DB_ZAP_AV_MSG 0

#if DB_ZAP_AV_MSG
#define DB_ZAP_AV(__statement__)  TRC_DBMSG(__statement__)
TRC_DBMSG_SET_MODULE(SYS);
#else
#define DB_ZAP_AV(__statement__)
#endif


/*******************************************************/
/*               Private Defines and Macros			   */
/*******************************************************/


/*******************************************************/
/*               Private Types						   */
/*******************************************************/


/*******************************************************/
/*               Private Constants                     */
/*******************************************************/

/*******************************************************/
/*               Global Variables					   */
/*******************************************************/
extern TLock g_tPlayer_Lock;
extern DVBPlayer_Status_t   g_tStatus;


/*******************************************************/
/*               Private Variables (static)			   */
/*******************************************************/

static U32 g_new_picture_count = 0;
static U32 g_old_picture_count = 0xffffffff;
static U32 g_old_aud_count = 0xffffffff;
static U32 g_new_aud_count = 0;
static U32 g_AudioFrameCount = 0;
static U32 g_VideoFrameCount = 0;

/*******************************************************/
/*               Private Function prototypes		   */
/*******************************************************/

/*******************************************************/
/*               Functions							   */
/*******************************************************/

DRV_AVCtrl_ErrorCode_t DVBPlayer_AVEventCallBack( DRV_AVCTRL_FRAME_INFO_t tFrameInfo )
{
    switch(tFrameInfo.event_type)
    {
    case AVCtrl__EVENT_EOS: 				/*文件播放结束*/
    {
    }
    break;

    case AVCtrl__EVENT_STOP: 			/* 停掉音视频解码器*/
    {
    }
    break;

    case AVCtrl__EVENT_DISP: 				/*subtitle数据显示*/
    {
    }
    break;

    case AVCtrl__EVENT_RNG_BUF_STATE: 	/* 媒体缓存队列状态变化*/
    {
    }
    break;

    case AVCtrl__EVENT_NORM_SWITCH: 	/* P/N 制切换*/
    {
    }
    break;

    case AVCtrl__EVENT_NEW_VID_FRAME:	 /* 新视频帧*/
    {
			pbiinfo("DVBPlayer_Start_Audio----------------AVCtrl__EVENT_NEW_VID_FRAME  AVCtrl__EVENT_NEW_VID_FRAME\n");
    }
    break;

    case AVCtrl__EVENT_NEW_AUD_FRAME:	 /* 新音频帧*/
    {
    }
    break;

    case AVCtrl__EVENT_NEW_USER_DATA: 	/* 新的用户数据*/
    {
    }
    break;

    case AVCtrl__EVENT_VID_STREAM: 		/* 视频PES BUF事件*/
    {
    }
    break;

    case AVCtrl__EVENT_BUTT: 			/*无效事件*/
    {
    }
    break;

    default:
    {
    }
    break;
    }

    return AVCTRL_NO_ERROR;
}


DRV_AVCtrl_ErrorCode_t DVBPlayer_SetAudioParam( DRV_AVCtrl_AudStereo_t tStereo, int bIsMute, int nVolume )
{
    DRV_AVCtrl_ErrorCode_t ErrCode = AVCTRL_NO_ERROR;

    if( bIsMute )
    {
        //	ErrCode = DRV_AVCtrl_Aud_Mute();
    }
    else
    {
        //	ErrCode = DRV_AVCtrl_Aud_UnMute();
    }
    if (ErrCode != AVCTRL_NO_ERROR)
    {
        return AVCTRL_ERROR;
    }

    ErrCode = DRV_AVCtrl_Aud_SetVolume( nVolume );
    if (ErrCode != AVCTRL_NO_ERROR)
    {
        return AVCTRL_ERROR;
    }

    ErrCode = DRV_AVCtrl_Aud_SetStereo( tStereo );
    if (ErrCode != AVCTRL_NO_ERROR)
    {
        return AVCTRL_ERROR;
    }

    return AVCTRL_NO_ERROR;
}



DRV_AVCtrl_ErrorCode_t DVBPlayer_SetAudioMode( DRV_AVCtrl_AudStereo_t tStereo )
{
    DRV_AVCtrl_ErrorCode_t ErrCode = AVCTRL_NO_ERROR;

    pbiinfo("%s   mode %d ",__FUNCTION__,tStereo);	

    ErrCode = DRV_AVCtrl_Aud_SetStereo( tStereo );
    if (ErrCode != AVCTRL_NO_ERROR)
    {
        return AVCTRL_ERROR;
    }

    return AVCTRL_NO_ERROR;
}
DRV_AVCtrl_ErrorCode_t  DVBPlayer_GetAudioMode(  DRV_AVCtrl_AudStereo_t *mode)
{
    DRV_AVCtrl_AudStereo_t ret = AVCTRL_NO_ERROR;

    ret = DRV_AVCtrl_Aud_GetStereo( );
    if (ret < AVCTRL_NO_ERROR)
    {
        return AVCTRL_ERROR;
    }
    *mode=ret;
    return AVCTRL_NO_ERROR;
}



/******************************************************************************
* Function : DVBPlayer_Start_Audio
* parameters :
*		Channel_param : (IN) 当前播放节目的参数
* Return :
*		0 : 开启过程正常
*		1 : 开启过程出错
* Description :
*		此函数开启音频的播放
*
* Author : LSZ	2007/09/11
******************************************************************************/
U32 DVBPlayer_Start_Audio(void)
{
    DRV_AVCtrl_ErrorCode_t ErrorCode=AVCTRL_NO_ERROR;
    U16 aud_fmt;
    U16 aud_pid;

    TLock_lock(&g_tPlayer_Lock);
    TLock_unlock(&g_tPlayer_Lock);

    TLock_lock(&g_tPlayer_Lock);
    aud_fmt = g_tStatus.tProgStatus.u8AudioFmt;
    aud_pid = g_tStatus.tProgStatus.u16AudioPid;
    TLock_unlock(&g_tPlayer_Lock);

    switch(aud_fmt)
    {
    case STREAM_TYPE_MPEG1_AUDIO:
    {
        aud_fmt = AVCTRL_AUD_FORMAT_MPEG;
    }
    break;

    case STREAM_TYPE_MPEG2_AUDIO:
    {
        aud_fmt = AVCTRL_AUD_FORMAT_MPEG;
    }
    break;

    case STREAM_TYPE_MPEG4_AAC:
    {
        aud_fmt = AVCTRL_AUD_FORMAT_AAC_RAW;
    }
    break;
    case STREAM_TYPE_MPEG_AAC:
    {
        aud_fmt = AVCTRL_AUD_FORMAT_AAC;

    }
    break;

    case STREAM_TYPE_AC3:
    {
        aud_fmt = AVCTRL_AUD_FORMAT_AC3;
    }
    break;
    case STREASM_TYPE_EAC3:
    {
        aud_fmt = AVCTRL_AUD_FORMAT_EAC3;
    }
    break;

    default:
    {
        aud_fmt = AVCTRL_AUD_FORMAT_BULL;
    }
    break;
    }
    pbiinfo("DVBPlayer_Start_Audio----------------pid %x  fmt%d\n",aud_pid,aud_fmt);
    ErrorCode = DRV_AVCtrl_Aud_SetPID(aud_pid);
    ErrorCode |= DRV_AVCtrl_Aud_Start(aud_fmt);
    pbiinfo("DVBPlayer_Start_Audio----------------ret%x\n",ErrorCode);


    if(ErrorCode != AVCTRL_NO_ERROR)
        return 1;
    else
        return 0;
}

void  DVBPlayer_Show_BroadcastBG(void)
{
	DRV_AVCtrl_Inject_Mpg();
}


void  DVBPlayer_Clear_BroadcastBG(void)
{
	DRV_AVCtrl_Clear_Mpg();
}




/******************************************************************************
* Function : DVBPlayer_Start_Video
* parameters :
*		Channel_param : (IN) 当前播放节目的参数
* Return :
*		0 : 开启过程正常
*		1 : 开启过程出错
* Description :
*		此函数开启视频的播放
*
* Author : LSZ	2007/09/11
******************************************************************************/
U32  DVBPlayer_Start_Video(void)
{
    DRV_AVCtrl_ErrorCode_t	ErrorCode = AVCTRL_NO_ERROR;
    U16 vidpid;
    U16 vidfmt;

    TLock_lock(&g_tPlayer_Lock);
    vidpid = g_tStatus.tProgStatus.u16VideoPid;
    vidfmt = g_tStatus.tProgStatus.u8VideoFmt;
    TLock_unlock(&g_tPlayer_Lock);

    if((vidpid > 0 ) && (vidpid < 0x1fff ))
    {
        switch(vidfmt)
        {
        case STREAM_TYPE_MPEG1_VIDEO:
        case STREAM_TYPE_MPEG2_VIDEO:
        {
            vidfmt = AVCTRL_VID_FORMAT_MPEG2;
        }
        break;

        case STREAM_TYPE_MPEG4P2:
        {
            vidfmt = AVCTRL_VID_FORMAT_MPEG4;
        }
        break;

        case STREAM_TYPE_H264:
        {
            vidfmt = AVCTRL_VID_FORMAT_H264;
        }
        break;

        default:
        {
            vidfmt = AVCTRL_VID_FORMAT_MPEG2;
        }
        break;
        }

        ErrorCode |= DRV_AVCtrl_Vid_SetPID(vidpid);
        ErrorCode |= DRV_AVCtrl_Vid_Start(vidfmt);

        /*move here by lsz 2010.06.01需要先设置音视频的格式{{*/
        /*}}move here by lsz 2010.06.01*/
        if(ErrorCode != AVCTRL_NO_ERROR)
            return 1;
        else
            return 0;

    }
    else
    {    	
        return 1;
    }
}

/******************************************************************************
* Function : DVBPlayer_Stop_Video
* parameters :
*		nothing
* Return :
*		0 : 正确停止播放视频
*		1 : 停止播放视频失败
* Description :
*		此函数停止视频的播放
*
* Author : LSZ	2007/10/22
******************************************************************************/
U32 DVBPlayer_Stop_Video( void)
{

    DRV_AVCtrl_ErrorCode_t		ErrorCode=AVCTRL_NO_ERROR;

    //{{{ modified by LYN,2011/03/15, for His3716 stop program failed
    ErrorCode  = DRV_AVCtrl_Vid_Stop();
    ErrorCode  |= DRV_AVCtrl_Vid_ClearPID();
    //}}} modified by LYN,2011/03/15, for His3716 stop program failed

    if (ErrorCode != AVCTRL_NO_ERROR)
        return 1;
    else
        return 0;
}

/******************************************************************************
* Function :  DVBPlayer_Stop_Audio
* parameters :
*		nothing
* Return :
*		0 : 正确停止播放音频
*		1 : 停止播放音频失败
* Description :
*		此函数停止音频的播放
*
* Author : LSZ	2007/10/22
******************************************************************************/
U32  DVBPlayer_Stop_Audio( void)
{
    DRV_AVCtrl_ErrorCode_t		ErrorCode = AVCTRL_NO_ERROR;

    //{{{ modified by LYN,2011/03/15, for His3716 stop program failed
    ErrorCode  = DRV_AVCtrl_Aud_Stop();
    ErrorCode  |= DRV_AVCtrl_Aud_ClearPID();
    //}}} modified by LYN,2011/03/15, for His3716 stop program failed

    if (ErrorCode != AVCTRL_NO_ERROR)
        return 1;
    else
        return 0;
}
/******************************************************************************
* Function :  DRV_AVCtrl_StartTs
* parameters :
*		nothing
* Return :
*		0 : 正确播放
*		1 : 播放失败
* Description :
*		此函数停止音频的播放
*
* Author : zshang	2013/12/31
******************************************************************************/
U32  DVBPlayer_Start_Play( void)
{
    DRV_AVCtrl_ErrorCode_t		ErrorCode = AVCTRL_NO_ERROR;

    ErrorCode  = DRV_AVCtrl_StartTs();
    if (ErrorCode != AVCTRL_NO_ERROR)
        return 1;
    else
        return 0;
}

/******************************************************************************
* Function :  DVBPlayer_Blank_Screen
* parameters :
*			无
* Return :
*		0 : 操作成功
*		1 : 操作失败
* Description :
*		停止图片显示
*
* Author : LSZ	2007/09/11
******************************************************************************/
U32 DVBPlayer_Blank_Screen(void)
{
    if(AVCTRL_NO_ERROR != DRV_AVCtrl_Vid_DisableOutput() )
        return 1;
    else
        return 0;
}

/******************************************************************************
* Function : DVBPlayer_Start_Pcr
* parameters :
*			uPcrPID: (IN) PCR的PID
* Return :
*		0 : 打开PCR成功
*		1 : 打开PCR失败
* Description :
*		此函数打开PCR
*
* Author : LSZ	2007/09/11
******************************************************************************/

U32 DVBPlayer_Start_Pcr(U16 uPcrPID)
{
    DRV_AVCtrl_ErrorCode_t	ErrorCode;

    if((uPcrPID != 0) && (uPcrPID != 0x1fff))
    {
        ErrorCode = DRV_AVCtrl_PCR_SetPID(uPcrPID);
        if(ErrorCode != AVCTRL_NO_ERROR)
        {
            return 1;
        }
    }
    return 0;
}

/******************************************************************************
* Function : DVBPlayer_Stop_PCR
* parameters :
*			无
* Return :
*		0 : 停止PCR成功
*		1 : 停止PCR失败
* Description :
*		此函数停止PCR
*
* Author : LSZ	2007/09/11
******************************************************************************/
U32 DVBPlayer_Stop_PCR( void)
{
    DRV_AVCtrl_ErrorCode_t	ErrorCode;

    ErrorCode = DRV_AVCtrl_PCR_ClearPID();
    if(ErrorCode != AVCTRL_NO_ERROR)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/******************************************************************************
* Function   : DVBPlayer_Start_TTX
* parameters :
* 		teletextpid: (IN)  teletext语言的pid
* 		magazine_number: (IN) teletext所在的杂志标号
*	 	page_number: (IN) teletext所在杂志中的页数
* Return     :
*		   0 : 设置teletext语言成功
*		   1 : 设置teletext语言失败
* Description:
*		此函数功能为设置TELETEXT语言
*
* Author     : LSZ	2007/10/16
******************************************************************************/
U32 DVBPlayer_Start_TTX(U32 teletextpid, U32 magazine_number, U32 page_number)
{
    /*
    	SYS_Teletext_ErrorCode_t	ErrCode = TELETEXT_NO_ERROR;

    	if(g_tPlayer_State_And_Action.ttx_state == PLAYER_START)
    		return 0;
    	if(teletextpid>0 && teletextpid<0x1fff)
    	{
    		ErrCode = SYS_TTX_Start(teletextpid, magazine_number, page_number);
    		if(ErrCode != TELETEXT_NO_ERROR)
    			return 1;
    		else
    		{
    			g_tPlayer_State_And_Action.ttx_state = PLAYER_START;
    			return 0;
    		}
    	}
    	else */
    return 0;
}

/******************************************************************************
* Function :DVBPlayer_Start_TTX
* parameters :
*		pid: (IN)TTX的PID
*		mage: (IN)TTX所在的杂志
*		page: (IN)TTX所在杂志中的页面
* Return :
*		0: 打开TTX	成功
*		1: 打开TTX失败
* Description :
*		此函数打开TTX
*
* Author : ZDS	2007/12/21
******************************************************************************/
U32 DVBPlayer_Stop_TTX(void)
{
    /*
    	SYS_Teletext_ErrorCode_t	ErrorCode;
    	if(g_tPlayer_State_And_Action.ttx_state == PLAYER_STOP)
    		return 0;
    	ErrorCode = SYS_TTX_Stop();
    	if(ErrorCode != TELETEXT_NO_ERROR)
    		return 1;
    	else
    	{
    		g_tPlayer_State_And_Action.ttx_state = PLAYER_STOP;
    		return 0;
    	}
    */
    return 0;
}

/******************************************************************************
* Function : DVBPlayer_VID_SetIOWindows
* parameters :
*		window_param(IN),输出位置的参数(模式、坐标及高度和宽度)
* Return :
*		0 : PAL/NTSC制式转换成功
*		非0 : PAL/NTSC制式转换失败
* Description :
*		此函数用于设置video的输出位置和大小
*
* Author : LSZ	2007/09/11
******************************************************************************/
U32 DVBPlayer_VID_SetIOWindows(DVBPlayer_VidOutWindow_t window_param)
{
    DRV_AVCtrl_VidWindow_t  tWin_Param;
    tWin_Param.uHeight = window_param.u32WinHight;
    tWin_Param.uWidth = window_param.u32WinWidth;
    tWin_Param.uX_Axis = window_param.s32PositionX;
    tWin_Param.uY_Axis = window_param.s32PositionY;
    tWin_Param.uBaseHeight = 1080;
    tWin_Param.uBaseWidth = 1920;
    return (U32)DRV_AVCtrl_Vid_SetIOWindow(tWin_Param);
}

/******************************************************************************
* Function : DVBPlayer_Set_ScreenRatio
* parameters :
*		 tSDVidAR: (IN) SD模式下宽高比的参数
*		 tHDVidAR: (IN) HD模式下宽高比的参数
* Return :
*		0 : 屏幕宽高比转换成功
*		1 : 屏幕宽高比转换失败
* Description :
*		此函数用于转换屏幕宽高比
*
* Author : LSZ	2007/09/11
******************************************************************************/
U32 DVBPlayer_Set_ScreenRatio(DRV_AVCtrl_VidARatio_t  tSDVidAR )
{
    DRV_AVCtrl_ErrorCode_t	ErrorCode=AVCTRL_NO_ERROR;

    DRV_AVCtrl_Vid_SetAspectRatio(tSDVidAR);
    if (ErrorCode != AVCTRL_NO_ERROR)
    {
        return 1;
    }
    return 0;
}
/******************************************************************************
* Function :DVBPlayer_Start_Subtitle
* parameters :
*		subtitle_pid: (IN)subtitle的PID
*
* Return :
*		0: 打开subtitle	成功
*		1: 打开subtitle失败
* Description :
*		此函数打开subtitle
*
* Author : ZDS	2007/12/21
******************************************************************************/
U32 DVBPlayer_Start_Subtitle(U16 subtitle_pid, U16 composite_pid, U16 ancillary_pid)
{
    /*
    	SYS_AVCtrl_ErrorCode_t	ErrCode = AVCTRL_NO_ERROR;
    	if(g_tPlayer_State_And_Action.subtitle_state == PLAYER_START)
    		return 0;

    	if((subtitle_pid>0 && subtitle_pid<0x1fff)
    		&& (composite_pid>0 && composite_pid<0x1fff)
    		&& (ancillary_pid>0 && ancillary_pid<0x1fff))
    	{
    		printf("\nSubPID:%d, %d, %d\n", subtitle_pid, composite_pid, ancillary_pid);
    		ErrCode = SYS_DVBSubt_Start(subtitle_pid, composite_pid,ancillary_pid);
    		if(ErrCode != AVCTRL_NO_ERROR)
    			return 1;
    		else
    		{
    			g_tPlayer_State_And_Action.subtitle_state = PLAYER_START;
    			return 0;
    		}
    	}
    	else */
    return 0;
}

/******************************************************************************
* Function :DVBPlayer_Stop_Subtitle
* parameters :
*		subtitle_pid: (IN)subtitle的PID
*
* Return :
*		0: 关闭subtitle	成功
*		1: 关闭subtitle 失败
* Description :
*		此函数关闭subtitle
*
* Author : ZDS	2007/12/21
******************************************************************************/

U32 DVBPlayer_Stop_Subtitle(void)
{
    /*
    	SYS_AVCtrl_ErrorCode_t	ErrorCode;
    	if(g_tPlayer_State_And_Action.subtitle_state == PLAYER_STOP)
    		return 0;

    	ErrorCode = SYS_DVBSubt_Stop();
    	if(ErrorCode != AVCTRL_NO_ERROR)
    		return 1;
    	else
    	{
    		g_tPlayer_State_And_Action.subtitle_state = PLAYER_STOP;
    		return 0;
    	}
    */
    return 0;
}

/******************************************************************************
* Function : DVBPlayer_Check_Video_Status
* parameters :
*			无
* Return :
*		0 : 视频正常播放
*		1 : 视频没有播放
* Description :
*		检测视频是否正常播放
*
* Author : LSZ	2007/09/11
******************************************************************************/
U32 DVBPlayer_Check_Video_Status(void)
{
    DRV_AVCtrl_ErrorCode_t ret = AVCTRL_ERROR;
    DRV_AVCTRL_STATUS_AVCOUNT_INFO_t tCountInfo;
    ret = DRV_AVCtrl_Get_Status_Info( &tCountInfo );
    if( AVCTRL_ERROR == ret )
    {
        return 1;
    }
    if( g_VideoFrameCount == tCountInfo.u32VidFrameCount )
    {
        return 1;
    }
    g_VideoFrameCount = tCountInfo.u32VidFrameCount;
    return 0;
#if 0
    if(g_new_picture_count ==  g_old_picture_count)
    {
        return 1;
    }
    else
    {
        g_old_picture_count = g_new_picture_count;
        return 0;
        /* 为了迅速的清除节目中断框 rxh on 10.09.27*/
    }
#endif
}

/******************************************************************************
* Function : DVBPlayer_Check_Audio_Status
* parameters :
*		NONE
* Return :
*		0 : 音频正常播放
*		1: 音频没有正常播放
* Description :
*		此函数检测音频是否正常播放
*
* Author : LSZ	2007/09/11
******************************************************************************/
U32 DVBPlayer_Check_Audio_Status( void )
{
    DRV_AVCtrl_ErrorCode_t ret = AVCTRL_ERROR;
    DRV_AVCTRL_STATUS_AVCOUNT_INFO_t tCountInfo;

    ret = DRV_AVCtrl_Get_Status_Info( &tCountInfo );
    if( AVCTRL_ERROR == ret )
    {
        return 1;
    }
    if( g_AudioFrameCount == tCountInfo.u32AuddFrameCount )
    {
        return 1;
    }
    g_AudioFrameCount = tCountInfo.u32AuddFrameCount;
    return 0;
#if 0
    DRV_AVCtrl_Aud_GetPesAccount(&g_new_aud_count);
    if(g_new_aud_count == g_old_aud_count)
    {
        return 1;
    }
    else
    {
        g_old_aud_count = g_new_aud_count;
        return 0;
        /* 为了迅速的清除节目中断框 rxh on 10.09.27*/
    }
#endif
}

/******************************************************************************
* Function :  DVBPlayer_Disable_Screen
* parameters :
*			无
* Return :
*		0 : 操作成功
*		1 : 操作失败
* Description :
*		停止输出
*
* Author : JWF	2010/05/17
******************************************************************************/
U32 DVBPlayer_Disable_Screen(void)
{
    DRV_AVCtrl_ErrorCode_t tError;
    tError = DRV_AVCtrl_Vid_DisableOutput();
    if(tError != AVCTRL_NO_ERROR)
        return 1;

    return 0;
}

/******************************************************************************
* Function :  DVBPlayer_Disable_Screen
* parameters :
*			无
* Return :
*		0 : 操作成功
*		1 : 操作失败
* Description :
*		打开输出
*
* Author : JWF	2010/05/17
******************************************************************************/
U32 DVBPlayer_Enable_Screen(void)
{
    DRV_AVCtrl_ErrorCode_t tError;
    tError = DRV_AVCtrl_Vid_EnableOutput();
    if(tError != AVCTRL_NO_ERROR)
        return 1;

    return 0;
}

U32 DVBPlayer_Set_Video_StopMode(U32 mode)
{
    DRV_AVCtrl_Vid_Stop_Mode_t tStopMode = AVCTRL_VID_STOP_MODE_BLACK;
    if( 0 == mode )
    {
        tStopMode = AVCTRL_VID_STOP_MODE_STILL;
    }
    else
    {
        tStopMode = AVCTRL_VID_STOP_MODE_BLACK;
    }
    DRV_AVCtrl_Vid_Set_StopMode( tStopMode );

    return 0;
}

U32 DVBPlayer_Set_Video_Volume( U32 Volume )
{
    DRV_AVCtrl_ErrorCode_t tError = AVCTRL_NO_ERROR;
    tError = DRV_AVCtrl_Aud_SetVolume(Volume);
    if(tError != AVCTRL_NO_ERROR)
    {
        return 1;
    }
    return 0;
}
U32 DVBPlayer_Video_Mute( U32 isMute )
{
    DRV_AVCtrl_ErrorCode_t tError = AVCTRL_NO_ERROR;
    if( 0 == isMute )
    {
        tError = DRV_AVCtrl_Aud_UnMute();
    }
    else
    {
        tError = DRV_AVCtrl_Aud_Mute();
    }
    if(tError != AVCTRL_NO_ERROR)
    {
        return 1;
    }
    return 0;
}

U32 DVBPlayer_Set_VPathPara( U32 isDvb )
{
    DRV_AVCtrl_ErrorCode_t tError = AVCTRL_NO_ERROR;
    tError = DRV_AVCtrl_SetVPathPara(isDvb);
	if(tError != AVCTRL_NO_ERROR)
    {
        return 1;
    }
    return 0;
}

