/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : dal_common.h
* Description : 
* Author : zyliu
* History :
*	2010/5/5 : Initial Version
******************************************************************************/
#ifndef __DAL_COMMON_H
#define __DAL_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************/
/*              Includes											*/
/*******************************************************/
#define DAL_USE_SYS_DEBUG
#ifdef DAL_USE_SYS_DEBUG
#include "sys_debug.h"
#else
#define SYS_DEBUG(l,m,fmt,args...)
#endif

/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/
#define MAX_STREAM_PIDS		32 //PLAYREC_MAX_PIDS
#define MAX_AUD_TRACK_NUM	16

#define MAX_VID_STREAM_NUM	1
//#define MAX_AUD_STREAM_NUM	MAX_AUD_TRACK_NUM
#define MAX_SUB_STREAM_NUM	16
#define MAX_TTX_STREAM_NUM	16

/*******************************************************/
/*              Exported Types			                */
/*******************************************************/
typedef enum
{
	DAL_NO_ERROR = 0,						
	DAL_ERROR_INVALID_HANDLE,  // 1
	DAL_ERROR_BAD_OPERATE,  // 2
	DAL_ERROR_BAD_PARAMETER,	// 3				
	DAL_ERROR_PLAYER_RUNNING,
	
	DAL_ERROR_STEVT_OPEN_FAILED, //5
	DAL_ERROR_STEVT_SUBSCRIBE_FAILED, //6
	
	DAL_ERROR_PLAYREC_INIT_FAILED,	// 7
	DAL_ERROR_PLAYREC_START_FAILED, //8
	DAL_ERROR_PLAYREC_STOP_FAILED,
	DAL_ERROR_PLAYREC_TERM_FAILED,
	DAL_ERROR_PLAYREC_PAUSE_FAILED,
	DAL_ERROR_PLAYREC_RESUME_FAILED, // 12
	DAL_ERROR_PLAYREC_SEEK_FAILED,
	DAL_ERROR_PLAYREC_SETSPEED_FAILED,
	DAL_ERROR_PLAYREC_GETSTREAMINFO_FAILED, // 15
	DAL_ERROR_PLAYREC_GetParams_FAILED,
	DAL_ERROR_PLAYREC_CHANGEPIDS_FAILED,

	DAL_ERROR_P2S_DLTask_Create_FAILED,
	DAL_ERROR_P2S_IJTask_Create_FAILED,
	DAL_ERROR_P2S_DL_START_FAILED, // 20
	DAL_ERROR_P2S_DL_STOP_FAILED,
	DAL_ERROR_P2S_IJ_START_FAILED,
	DAL_ERROR_P2S_IJ_STOP_FAILED,
	DAL_ERROR_P2S_ParsePID_FAILED,

	DAL_ERROR_NOT_SUPPORT,
}DAL_ERROR_t;

typedef enum
{
	DAL_STREAMTYPE_INVALID =   0, /* None     : Invalid type                         */
	DAL_STREAMTYPE_MP1V    =   1, /* Video    : MPEG1                                */
	DAL_STREAMTYPE_MP2V    =   2, /* Video    : MPEG2                                */
	DAL_STREAMTYPE_MP4V    =   3, /* Video    : H264                                 */
	DAL_STREAMTYPE_MP1A    =   4, /* Audio    : MPEG 1 Layer I                       */
	DAL_STREAMTYPE_MP2A    =   5, /* Audio    : MPEG 1 Layer II                      */
	DAL_STREAMTYPE_MP4A    =   6, /* Audio    : like HEAAC,Decoder LOAS / LATM - AAC */
	DAL_STREAMTYPE_TTXT    =   7, /* Teletext : Teletext pid                         */
	DAL_STREAMTYPE_SUBT    =   8, /* Subtitle : Subtitle pid                         */
	DAL_STREAMTYPE_PCR     =   9, /* Synchro  : PCR pid                              */
	DAL_STREAMTYPE_AC3     =  10, /* Audio    : AC3                                  */
	DAL_STREAMTYPE_H264    =  11, /* Video    : H264                                 */
	DAL_STREAMTYPE_MPEG4P2 =  12, /* Video    : MPEG4 Part II                        */
	DAL_STREAMTYPE_VC1     =  13, /* Video    : Decode Simple/Main/Advanced profile  */
	DAL_STREAMTYPE_AAC     =  14, /* Audio    : Decode ADTS - AAC                    */
	DAL_STREAMTYPE_HEAAC   =  15, /* Audio    : Decoder LOAS / LATM - AAC            */
	DAL_STREAMTYPE_WMA     =  16, /* Audio    : WMA,WMAPRO                           */
	DAL_STREAMTYPE_DDPLUS  =  17, /* Audio    : DD+ Dolby digital                    */
	DAL_STREAMTYPE_DTS     =  18, /* Audio    : DTS                                  */
	DAL_STREAMTYPE_MMV     =  19, /* Video    : Multimedia content                   */
	DAL_STREAMTYPE_MMA     =  20, /* Audio    : Multimedia content                   */
	DAL_STREAMTYPE_AVS     =  21, /* Video    : AVS Video format                     */
	DAL_STREAMTYPE_OTHER   = 255  /* Misc     : Non identified pid                   */
}DAL_StreamType_t;

typedef struct DAL_StreamData_s
{
	DAL_StreamType_t		type;
	U16						pid;
}DAL_StreamData_t;

typedef enum
{
	SourceType_TUNER = 1,
	SourceType_HTTP,
	SourceType_UDP,
	SourceType_FILE,
	SourceType_BUFFER,
	SourceType_P2S,
	SourceType_P2V,/*加密ts，和ppc p2v */
	SourceType_FLV_LIST,
	SourceType_MMS,
	SourceType_HLS,
	SourceType_TIMESHIFT,/*服务器时实time shift*/
}DAL_PlayerStreamSourceType_t;

typedef struct DAL_HTTP_source_s
{
	U8 URL[255];
}DAL_HTTP_Source_t;

typedef struct DAL_UDP_source_s
{
     U8 UDPTYPE;// 0: unicast,1 mutilcast
     U8 URL[255];
}DAL_UDP_Source_t;

typedef struct DAL_FILE_source_s
{
	U8 filename[255];
	U8 fileuri[255];
}DAL_FILE_Source_t;

typedef struct DAL_BUFFER_source_s
{
	void* 		buffer;
}DAL_BUFFER_Source_t;

typedef struct DAL_Player_Input_s{
	DAL_PlayerStreamSourceType_t  SourceType;
	union
	 {
	  DAL_HTTP_Source_t			http;
	  DAL_UDP_Source_t			udp;
	  DAL_FILE_Source_t			file;
	  DAL_BUFFER_Source_t		buffer;
	 }Source;
	U32					 	PidsNum;
	DAL_StreamData_t 		Pids[MAX_STREAM_PIDS];
	U32						TunerIndex;
	U32 					param;
}DAL_Player_Input_t;

typedef enum{
	OUTPUT_VIDLAYER = 1<<1,
	OUTPUT_OSDLAYER = 1<<2,
	OUTPUT_HTTP = 1<<3,
	OUTPUT_UDP_U = 1<<4,// UDP Unicast
	OUTPUT_UDP_M = 1<<5// UDP muticast
}DAL_OUTPUT_TYPE_t;


typedef enum 
{
	DAL_VID_ZOOM_NONE = 0,
	DAL_VID_ZOOM_BY_2,
	DAL_VID_ZOOM_BY_2_5,
	DAL_VID_ZOOM_BY_3,
	DAL_VID_ZOOM_BY_3_5,
	DAL_VID_ZOOM_BY_4
} DAL_VidZoomMode;


typedef struct DAL_VidOutWindow_s
{
	DAL_VidZoomMode	eZoom_mode;
	S32 					siPosition_X;
	S32 					siPosition_Y;
	U32 					siWinWidth;
	U32 					siWinHight;
}DAL_VidOutWindow_t;


typedef enum
{
	DAL_VID_AR_16TO9 = 1,
	DAL_VID_AR_4TO3 = 2,
	DAL_VID_AR_14TO9 = 3,
	DAL_VID_AR_IGNORE =4,
	DAL_VID_AR_221TO1 = 5
} DAL_VidARatio;

typedef enum
{
	DAL_VID_NONE,
	DAL_VID_PAL,
	DAL_VID_NTSC
}DAL_VidSYSColorMode;

typedef struct DAL_Player_Output_s
{
	DAL_OUTPUT_TYPE_t 		Type;//eg. Type = OUTPUT_HTTP|OUTPUT_VIDLAYER;
	DAL_VidOutWindow_t  	tCur_VidOutWin; 		
	DAL_VidARatio			eS_Ratio;				
	DAL_VidSYSColorMode 	eCur_vidMode;

	U32						vidhandle;		/*STVID_Handle_t*/
	U32						vpSDhandle;		/*STVID_ViewPortHandle_t*/
	U32						vpHDhandle;		/*STVID_ViewPortHandle_t*/
	
	/*the follow items used by Http,udp_u,udp_m output*/
	U32  					IPaddress;
	U32  					port;
	U8  					outputName[255];
}DAL_Player_Output_t;

typedef enum
{
	PLAYER_IDLE = 1,
	PLAYER_PLAY,
	PLAYER_PAUSE,
	PLAYER_PVR_RECORD,
	PLAYER_TIMESHIFT_RECORD,
	PLAYER_TIMESHIFT_PLAY,
}DAL_Player_States_t;

typedef struct DAL_Player_Info_s
{
	DAL_Player_States_t	states;
	DAL_Player_Input_t	input;
	DAL_Player_Output_t	output;
	U32 				playing_filesize_t;//  seconds
	U32					nowposition;//  seconds
	BOOL				IsBuffering;
	U32					BufferingData;//0-99
	U32					record_filesize;// MB
	U32					record_filesize_t;//seconds
}DAL_Player_Info_t;

typedef enum
{
	PlayerSpeed1X =1,
	PlayerSpeed2X,
	PlayerSpeed4X,
	PlayerSpeed8X
}DAL_PlayerSpeed_t;

typedef enum dal_play_event_e
{
	DAL_PLAY_EVENT_PLAY_START=0,//目前没有用
	//DAL_PLAY_EVENT_BUFFERING,
	DAL_PLAY_EVENT_NEW_PROGRESS=1,//目前没有用
	DAL_PLAY_EVENT_PLAY_END=2,//节目播放完
	DAL_PLAY_EVENT_FLVLIST_END=3,//目前没有用

	DAL_PLAY_EVENT_PLAY_START_FAILED=4,//播放器start 失败
	DAL_PLAY_EVENT_BUFFERING=5,//缓冲
	DAL_PLAY_EVENT_DATA_INTERRUPT=6,//节目数据中断
	DAL_PLAY_EVENT_DATA_RESUME=7,//节目数据中断后恢复
	DAL_PLAY_EVENT_RESTART_SAME_URL=8,//切换的节目时同一个节目地址，报错
	DAL_PLAY_EVENT_PLAY_WILL_STOP=9,//播放器将要停止
	DAL_PLAY_EVENT_PLAY_PAUSE_OK=10,//播放器暂停
	DAL_PLAY_EVENT_PLAY_RESUME_OK=11,//播放器继续播放
	DAL_PLAY_EVENT_PLAY_SEEK_OK=12,//seek成功
	DAL_PLAY_EVENT_PLAY_SEEK_ACTION_START=13,		// seek 动作开始
	DAL_PLAY_EVENT_PLAY_SEEK_ACTION_DONE=14,		// seek 动作结束
	DAL_PLAY_EVENT_FIRST_FRAME=15,//视频开始播放第一帧
	DAL_PLAY_EVENT_PLAY_STOP_OK=16,//播放结束已成功
	DAL_PLAY_EVENT_MMS_PLAY_START_FAILED=17,//目前没有用
	DAL_PLAY_EVENT_PLAYREC_START_OK=18,	//目前没有用			// 	playrec_start_src_demux() 调用成功时
	DAL_PLAY_EVENT_M3U8_DOWNSTART_FAILED=19,//m3u8文件下载失败
	DAL_PLAY_EVENT_P2V_RESUME_FAILED=20,//p2v,timeshift恢复播放失败
	
	DAL_PLAY_EVENT_PLAY_SEEK_END=21,	//seek到了影片尾
	DAL_PLAY_EVENT_PLAY_SEEK_TITLE=22,//seek到了影片开始
}dal_play_event_t;

/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/
#ifdef MMS
int get_actual_mms_url(char* mms_url,int url_len);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __DAL_COMMON_H */
/* End of dal_common.h  --------------------------------------------------------- */




