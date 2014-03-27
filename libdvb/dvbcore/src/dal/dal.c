/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name : dal.c
* Description : 
* History : 
*	Date				Modification				Name
*	----------			------------			----------
*	2010/12/13			Created					zyliu
******************************************************************************/
/*-----------------------------------------------------*/
/*              Includes												*/
/*-----------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pvddefs.h"

#ifdef USE_ST_PLAYREC
#include "stapp_main.h"
#include "playrec.h"
#endif
#include <curl.h>
#include <types.h>
#include <easy.h>

#include "ring_buffer.h"
#include "dal_wrap.h"
#include "dal_common.h"
#include "dal.h"
#include "dal_message.h"
#include "dal_hw_player.h"
#include "dal_sw_player.h"
#include "dal_core.h"
#include "dal_download.h"
#include "dal_parse.h"
#include "dal_inject.h"
#include "flv_parse.h"
#include "flv_list_play.h"


#ifdef HAVE_TUNER_HEADER_FILE
#include "drv_tuner.h"
#endif

//#include "Mplayer_message.h"
//#include "Mplayer_ip_operate.h"

//#include "drv_container.h"




//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>

/*-----------------------------------------------------*/
/*              Defines												*/
/*-----------------------------------------------------*/
#define MAX_DAL_PLAYER_NUM	2
/*-----------------------------------------------------*/
/*              Global												*/
/*-----------------------------------------------------*/

/*-----------------------------------------------------*/
/*              Exported Types										*/
/*-----------------------------------------------------*/
typedef enum
{
	DAL_HardWare,
	DAL_SoftWare,	
}DAL_DecType_t;

typedef enum 
{
	DAL_FORMAT_AAC,
	DAL_FORMAT_MP2,
	DAL_FORMAT_MP3,
	//...

	DAL_FORMAT_MPEG4,
	DAL_FORMAT_MPEG2VIDEO,
	//...
	DAL_FORMAT_OTHER,
}DAL_Fmt_t;

typedef enum
{
	DAL_CONTAINER_AVI,
	DAL_CONTAINER_FLV,
	//...
}DAL_Container_t;

typedef struct DAL_AudLangInfo_s
{
	DAL_LanguageType_t		lang_type;
										//
}DAL_AudLangInfo_t;

typedef struct DAL_VideoInfo_s
{
	U32					stream_index;		
	DAL_Fmt_t			format;
	//C8				Codec[32];
	double				duration;	//单位秒
	double				frameRate;
	U32					bitRate;	//单位bps
	U32					width;
	U32					height;
}DAL_VideoInfo_t;

typedef struct DAL_AudioInfo_s
{
	U32 				stream_index;
	DAL_Fmt_t			format;
	DAL_LanguageType_t	language;
	//C8				Codec[32];
	U32					bitRate;			// bps
	double				duration;			// s
	double				sampleRate;
	U32					sampleBits;
	U32					channels;
	U32					blockAlign;	
} DAL_AudioInfo_t;

typedef struct DAL_SubInfo_s
{
	U32					stream_index;
}DAL_SubInfo_t;

typedef struct DAL_TTXInfo_s
{
	U32					stream_index;
}DAL_TTXInfo_t;

typedef struct DAL_MediaInfo_s
{
	DAL_Container_t		container;
	U32					video_num;
	DAL_VideoInfo_t		video_info[MAX_VID_STREAM_NUM];
	U32					audio_num;
	DAL_AudioInfo_t		audio_info[MAX_AUD_TRACK_NUM];
	U32					sub_num;
	DAL_SubInfo_t		sub_info[MAX_SUB_STREAM_NUM];
	U32					ttx_num;
	DAL_TTXInfo_t		ttx_info[MAX_TTX_STREAM_NUM];
}DAL_MediaInfo_t;

typedef struct DAL_Player_s
{
	/*	播放状态*/
	DAL_Player_States_t		states;

	/*	输入输出*/
	DAL_Player_Input_t		input;
	DAL_Player_Output_t		output;

	/*	播放状态*/
	U32						playing_filesize_t;//  seconds
	U32						nowposition;//  seconds
	BOOL					IsBuffering;
	U32						BufferingData;//0-99
	U32						record_filesize;// MB
	U32						record_filesize_t;//seconds

	U32						play_speed;

	/*	媒体信息*/
	DAL_MediaInfo_t			media_info;

	/*	解码方式*/
	DAL_DecType_t			dec_type;

	/*	事件通知*/
	DAL_Event_Notify_t		video_event_callback;		// 底层的Video 事件
	DAL_Event_Notify_t		audio_event_callback;		// 底层的Audio 事件
	DAL_Event_Notify_t		play_event_callback;		// dal 的各种执行情况和状态反馈
}DAL_Player_t;




static DAL_Player_t				g_DALPlayer[MAX_DAL_PLAYER_NUM];
#ifdef USE_ST_PLAYREC
static STEVT_Handle_t			g_event_handle;
#endif

static DAL_Player_t*			g_cur_hardware_player;
static DAL_Player_t*			g_cur_software_player;

/*-----------------------------------------------------*/
/*              Local Function Prototypes					*/
/*-----------------------------------------------------*/

void dal_core_event_callback( U32 event, void* param);
#ifdef USE_ST_PLAYREC
void dal_av_event_callback(STEVT_CallReason_t Reason,STEVT_EventConstant_t Event,const void *EventData);
#endif
void dal_playrec_event_callback(unsigned int event);

DAL_Player_t* dal_get_player_instance( DAL_PlayerInstance_t player);
DAL_Player_t* dal_get_player_instance_by_dectype( DAL_DecType_t dectype);


DAL_ERROR_t dal_check_input( DAL_Player_Input_t* input);
DAL_ERROR_t dal_check_output( DAL_Player_Output_t* output);
DAL_ERROR_t dal_get_media_info( DAL_Player_Input_t* input, DAL_MediaInfo_t* meida_info);
DAL_ERROR_t dal_get_decode_type( DAL_MediaInfo_t* media_info, DAL_DecType_t* type);

//DAL_Fmt_t dal_ffmpeg2dal_fmt( enum CodecID codec_id);
DAL_LanguageType_t dal_ffmpeg2dal_language( char* language);

DAL_ERROR_t dal_get_media_info1( DAL_Player_t* pDALPlayer, const char* filename);
void print_media_info( DAL_Player_t* pDALPlayer);

char* dal_status_string(DAL_Player_States_t status);
BOOL dal_is_stoped(void);

void Player_DalEventCallback( U32 event, void* param);



/*-----------------------------------------------------*/
/*              External Function  Definition								*/
/*-----------------------------------------------------*/
S32 DAL_Init( void)
{

#ifdef USE_ST_PLAYREC

	STEVT_SubscribeParams_t		SubscribeParams;
	STEVT_OpenParams_t			EVT_OpenParams;
	ST_ErrorCode_t				error = ST_NO_ERROR;
	
#endif
	S32				error = DAL_NO_ERROR;
	Core_Param_t	core_param;
	int				iret = 0;
	U32				i = 0;
    char header[256]="data/data/com.pbi.dvb/header";
    char file[256]="data/data/com.pbi.dvb/context";

	memset( g_DALPlayer, 0, sizeof(DAL_Player_t)*MAX_DAL_PLAYER_NUM);
	for( i = 0; i < MAX_DAL_PLAYER_NUM; i++)
	{
		g_DALPlayer[i].states = PLAYER_IDLE;
	}

#ifdef USE_ST_PLAYREC

	/* 硬件播放这边的event_callback注册*/
	SubscribeParams.NotifyCallback = dal_av_event_callback;
	memset( &EVT_OpenParams, 0, sizeof(STEVT_OpenParams_t));
	error = STEVT_Open(EVT_DeviceName[0],&EVT_OpenParams,&g_event_handle);
	if( error != ST_NO_ERROR)
	{
		return DAL_ERROR_STEVT_OPEN_FAILED;
	}
	error =  STEVT_Subscribe( g_event_handle, STVID_NEW_PICTURE_TO_BE_DISPLAYED_EVT, &SubscribeParams);
	error |= STEVT_Subscribe( g_event_handle, STVID_SEQUENCE_INFO_EVT, &SubscribeParams);
	//error = STEVT_Subscribe( g_event_handle, STVID_USER_DATA_EVT, &SubscribeParams);
	error |= STEVT_Subscribe( g_event_handle, STVID_FRAME_RATE_CHANGE_EVT, &SubscribeParams);
	error |= STEVT_Subscribe( g_event_handle, STVID_ASPECT_RATIO_CHANGE_EVT, &SubscribeParams);
	error |= STEVT_Subscribe( g_event_handle, STAUD_NEW_FRAME_EVT, &SubscribeParams);
	if( error != ST_NO_ERROR)
	{
		return DAL_ERROR_STEVT_SUBSCRIBE_FAILED;
	}
	
	PLAYREC_SET_EVENT_CALLBACK(dal_playrec_event_callback);
#endif	
	/* 软件播放的event_callback 注册*/
	//...

	/*dal core init , dal core willl process p2s play and flvlist play*/
	core_param.core_event_callback = dal_core_event_callback;
	iret = DAL_CoreInit( &core_param);
	if( iret != 0)
	{
	//	DAL_ERROR(("DAL_CoreInit failed\n"));
		DebugMessage("[DAL]DAL_CoreInit failed\n");
		return iret;
	}
	
	Set_Http_header_addr(header);
	Set_Http_File_addr(file);
	//DAL_Set_PlayEventCallback(Player1,Player_DalEventCallback);
	return DAL_NO_ERROR;
}

S32 DAL_Term( void)
{
	U32					i = 0;
//	int					iret = 0;

	for( i = 0; i < MAX_DAL_PLAYER_NUM; i++)
	{
		if( g_DALPlayer[i].states != PLAYER_IDLE)
		{
			return DAL_ERROR_PLAYER_RUNNING;
		}
	}
	for( i = 0; i < MAX_DAL_PLAYER_NUM; i++)
	{
		g_DALPlayer[i].states = PLAYER_IDLE;
		g_DALPlayer[i].audio_event_callback = NULL;
		g_DALPlayer[i].video_event_callback = NULL;
	}

	DAL_CoreRelease();
	return DAL_NO_ERROR;
}

S32 DAL_Stream_Start(DAL_PlayerInstance_t player,DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{
	DAL_Player_t*		pDALPlayer = NULL;
	DAL_MediaInfo_t		media_info;
	DAL_ERROR_t			error = DAL_NO_ERROR;


	DebugMessage("[DAL]>>>>> %s >>>>> SourceType: %d\n",__FUNCTION__, input->SourceType);

	if( player < Player1 || player >= PlayerTotal)
	{
		//SYS_DEBUG(SD_ERROR,MODULE_DAL,"player : %u", player);
		DebugMessage("[DAL]DAL_Stream_Start-> player id: %u",player);
		return DAL_ERROR_INVALID_HANDLE;
	}
	if( input == NULL || output == NULL)
	{
		//SYS_DEBUG(SD_ERROR,MODULE_DAL,"input : 0x%x  output: 0x%x", input, output);
		DebugMessage("[DAL]DAL_Stream_Start-> input :0x%x  output :0x%x",input,output);
		return DAL_ERROR_BAD_PARAMETER;
	}

	pDALPlayer = dal_get_player_instance( player);
	
	if( pDALPlayer->states != PLAYER_IDLE)
	{
		//SYS_DEBUG(SD_ERROR,MODULE_DAL,"DAL is not in PLAYER_IDLE");
		DebugMessage("[DAL]DAL_Stream_Start-> DAL is not in PLAYER_IDLE");
		return DAL_ERROR_BAD_OPERATE;
	}

	error = dal_check_input( input);
	if( error != DAL_NO_ERROR)
	{
		DebugMessage("[DAL]DAL_Stream_Start->dal_check_input failed");
		//printf(">>>>> %s >>>>> dal_check_input failed\n", __FUNCTION__);
		return error;
	}
	error = dal_check_output( output);
	if( error != DAL_NO_ERROR)
	{
		return error;
	}

	error = dal_get_media_info( input, &media_info);
	if( error != DAL_NO_ERROR)
	{
		return error;
	}
	error = dal_get_decode_type( &media_info, &(pDALPlayer->dec_type));
	if( error != DAL_NO_ERROR)
	{
		return error;
	}
	if( pDALPlayer->dec_type == DAL_HardWare)
	{
		//硬件播放接口		
		error = DAL_hw_play_start( input, output);
		if( error != DAL_NO_ERROR)
		{
			//SYS_DEBUG(SD_ERROR,MODULE_DAL,"DAL_hw_play_start failedwith %d\n",error);
			DebugMessage("[DAL]DAL_hw_play_start failed %d ",error);
			return error;
		}
		
		DebugMessage("[DAL]DAL_hw_play_start success");
		g_cur_hardware_player = pDALPlayer;

	/* {{{ add zyliu 2011-11-24 */
		// 在次事件回调时设置viewport handle给上层，用于av 数据控制
		if( input->SourceType == SourceType_TUNER 
			|| input->SourceType == SourceType_HTTP 
			|| input->SourceType == SourceType_FILE)
		{
			pDALPlayer->play_event_callback(DAL_PLAY_EVENT_PLAYREC_START_OK, output);
		}
	/*add zyliu 2011-11-24 }}}*/
			
	}
	else
	{
		//软件播放接口
		error = DAL_sw_play_start();
		if( error != DAL_NO_ERROR)
		{
			return error;
		}		
		g_cur_software_player = pDALPlayer;
	}
	
	//播放成功，把信息放入player 结构
	memcpy( &(pDALPlayer->input), input, sizeof(DAL_Player_Input_t));
	memcpy( &(pDALPlayer->output), output, sizeof(DAL_Player_Output_t));
	memcpy( &(pDALPlayer->media_info), &media_info, sizeof(DAL_MediaInfo_t));


/*   防止dal_core 还在处理播放动作时，接受关闭播放请求，但是状态却为PLAYER_IDLE
	因此在DAL_Stream_Start 调用成功时，便将dal 这层的状态设置为PLAYER_PLAY;
	*/

	dal_set_state(player,PLAYER_PLAY);


	return DAL_NO_ERROR;
}

S32 DAL_Stream_Stop(DAL_PlayerInstance_t player)
{
	DAL_Player_t*		pDALPlayer = NULL;
	DAL_ERROR_t			error = DAL_NO_ERROR;

	if( player < Player1 || player >= PlayerTotal)
	{
		return DAL_ERROR_INVALID_HANDLE;
	}

	pDALPlayer = dal_get_player_instance( player);

	if( pDALPlayer->states != PLAYER_PLAY && pDALPlayer->states != PLAYER_PAUSE)
	{
		//SYS_DEBUG(SD_WARNING,MODULE_DAL,"DAL states is not in PLAYER_PLAY or PLAYER_PAUSE, but %s, can't stop!",dal_status_string(pDALPlayer->states));
		DebugMessage("[DAL]DAL_Stream_Stop ->DAL states is not in PLAYER_PLAY or PLAYER_PAUSE, but %s, can't stop!",dal_status_string(pDALPlayer->states));
		return DAL_ERROR_BAD_OPERATE;
	}

	
	DebugMessage("[DAL]DAL_Stream_Stop ->type:  %d",pDALPlayer->input.SourceType);
	if( pDALPlayer->dec_type == DAL_HardWare)
	{
		error = DAL_hw_play_stop( &(pDALPlayer->input));
		if( error != DAL_NO_ERROR)
		{
			return error;
		}
		g_cur_hardware_player = NULL;
	}
	else
	{
		//软件播放接口
		error = DAL_sw_play_stop();
		if( error != DAL_NO_ERROR)
		{
			return error;
		}
		g_cur_software_player = NULL;
	}
	
	//memset(pDALPlayer, 0, sizeof(DAL_Player_t)); //这里会把eventback也清了


	/* SourceType_P2S   SourceType_FLV_LIST  关闭是异步的，
	所以这里不设置PLAYER_IDLE 的状态, add Multicast by LYN,the multicast function is same with P2S*/
	if( pDALPlayer->input.SourceType != SourceType_P2S 
		&& pDALPlayer->input.SourceType != SourceType_P2V
		&& pDALPlayer->input.SourceType != SourceType_FLV_LIST 
		&&pDALPlayer->input.SourceType != SourceType_UDP
		&& pDALPlayer->input.SourceType != SourceType_HLS
		&& pDALPlayer->input.SourceType != SourceType_TIMESHIFT)
	{
		dal_set_state(player,PLAYER_IDLE);
	}
	return DAL_NO_ERROR;
}

S32 DAL_Stream_Restart(DAL_PlayerInstance_t player,DAL_Player_Input_t* input,DAL_Player_Output_t* output)
{
	DAL_Player_t*		pDALPlayer = NULL;
	DAL_MediaInfo_t		media_info;
	DAL_DecType_t		dectype;
	DAL_ERROR_t			error = DAL_NO_ERROR;

	if( player < Player1 || player >= PlayerTotal)
	{
		return DAL_ERROR_INVALID_HANDLE;
	}
	if( input == NULL || output == NULL)
	{
		return DAL_ERROR_BAD_PARAMETER;
	}

	pDALPlayer = dal_get_player_instance( player);
	
	if( pDALPlayer->states != PLAYER_PLAY && pDALPlayer->states != PLAYER_PAUSE)
	{
		//SYS_DEBUG(SD_ERROR,MODULE_DAL,"dal_get_player_instance failed,player: %d\n", pDALPlayer, player);
		DebugMessage("[DAL]DAL_Stream_Restart ->dal_get_player_instance failed,player: %d\n", player);
		return DAL_ERROR_BAD_OPERATE;
	}

	error = dal_check_input( input);
	if( error != DAL_NO_ERROR)
	{
		return error;
	}

	error = dal_check_output( output);
	if( error != DAL_NO_ERROR)
	{
		return error;
	}

	error = dal_get_media_info( input, &media_info);
	if( error != DAL_NO_ERROR)
	{
		return error;
	}

	error = dal_get_decode_type( &media_info, &dectype);
	if( error != DAL_NO_ERROR)
	{
		return error;
	}

	if( dectype == DAL_HardWare)
	{
		error = DAL_hw_play_restart( &(pDALPlayer->input), input, &(pDALPlayer->output), output);
		if( error != DAL_NO_ERROR)
		{
			/**/
			//printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 2 PLAYER_IDLE\n");
			DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 2 PLAYER_IDLE\n");
			pDALPlayer->states = PLAYER_IDLE;
			return error;
		}
		g_cur_hardware_player = pDALPlayer;
	}
	else
	{
		error = DAL_sw_play_restart();
		if( error != DAL_NO_ERROR)
		{
			pDALPlayer->states = PLAYER_IDLE;
			return error;
		}
		g_cur_software_player = pDALPlayer;
	}
	memcpy( &(pDALPlayer->input), input, sizeof(DAL_Player_Input_t));
	memcpy( &(pDALPlayer->output), output, sizeof(DAL_Player_Output_t));
	memcpy( &(pDALPlayer->media_info), &media_info, sizeof(DAL_MediaInfo_t));
	pDALPlayer->dec_type = dectype;
	pDALPlayer->states = PLAYER_PLAY;	
	return DAL_NO_ERROR;
}

S32 DAL_Stream_Pause( DAL_PlayerInstance_t player)
{
	DAL_Player_t*		pDALPlayer = NULL;
	DAL_ERROR_t			error = DAL_NO_ERROR;	


	DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  DAL_Stream_Pause\n");

	if( player < Player1 || player >= PlayerTotal)
	{
		return DAL_ERROR_INVALID_HANDLE;
	}

	pDALPlayer = dal_get_player_instance( player);

	if( pDALPlayer->states != PLAYER_PLAY)
	{
		return DAL_ERROR_BAD_OPERATE;
	}

	if( pDALPlayer->dec_type == DAL_HardWare)
	{
		error = DAL_hw_play_pause( &(pDALPlayer->input));
		if( error != DAL_NO_ERROR)
		{
			return error;
		}
	}
	else
	{
		error = DAL_sw_play_pause();
		if( error != DAL_NO_ERROR)
		{
			return error;
		}
	}
	pDALPlayer->states = PLAYER_PAUSE;
	return DAL_NO_ERROR;
}

S32 DAL_Stream_resume(DAL_PlayerInstance_t player)
{
	DAL_Player_t*		pDALPlayer = NULL;
	DAL_ERROR_t			error = DAL_NO_ERROR;	

	DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  DAL_Stream_resume\n");

	if( player < Player1 || player >= PlayerTotal)
	{
		return DAL_ERROR_INVALID_HANDLE;
	}

	pDALPlayer = dal_get_player_instance( player);

	if( pDALPlayer->states != PLAYER_PAUSE)
	{
		return DAL_ERROR_BAD_OPERATE;
	}

	if( pDALPlayer->dec_type == DAL_HardWare)
	{
		error = DAL_hw_play_resume( &(pDALPlayer->input));
		if( error != DAL_NO_ERROR)
		{
			return error;
		}		
	}
	else
	{
		error = DAL_sw_play_resume();
		if( error != DAL_NO_ERROR)
		{
			return error;
		}	
	}
	pDALPlayer->states = PLAYER_PLAY;
	return DAL_NO_ERROR;
}

S32 DAL_Stream_SetSpeed(DAL_PlayerInstance_t player, DAL_PlayerSpeed_t speed)
{
	DAL_Player_t*		pDALPlayer = NULL;
	DAL_ERROR_t			error = DAL_NO_ERROR;	

	if( player < Player1 || player >= PlayerTotal)
	{
		return DAL_ERROR_INVALID_HANDLE;
	}

	if( speed < PlayerSpeed1X || speed > PlayerSpeed4X)
	{
		return DAL_ERROR_BAD_PARAMETER;
	}

	pDALPlayer = dal_get_player_instance( player);

	if( pDALPlayer->states != PLAYER_PLAY && pDALPlayer->states != PLAYER_PAUSE && pDALPlayer->states != PLAYER_TIMESHIFT_PLAY)
	{
		return DAL_ERROR_BAD_OPERATE;
	}

	if( pDALPlayer->dec_type == DAL_HardWare)
	{
		error = DAL_hw_play_setspeed( &(pDALPlayer->input), speed);
		if( error != DAL_NO_ERROR)
		{
			return error;
		}
	}
	else
	{
		error = DAL_sw_play_setspeed();
		if( error != DAL_NO_ERROR)
		{
			return error;
		}
	}
	pDALPlayer->play_speed = speed;
	return DAL_NO_ERROR;
}

S32 DAL_Stream_Seek(DAL_PlayerInstance_t player, U32 seconds)
{
	DAL_Player_t*		pDALPlayer = NULL;
	DAL_ERROR_t			error = DAL_NO_ERROR;	

	if( player < Player1 || player >= PlayerTotal)
	{
		return DAL_ERROR_INVALID_HANDLE;
	}

	pDALPlayer = dal_get_player_instance( player);

	if( pDALPlayer->states != PLAYER_PLAY && pDALPlayer->states != PLAYER_PAUSE)
	{
		return DAL_ERROR_BAD_OPERATE;
	}

	if( pDALPlayer->dec_type == DAL_HardWare)
	{
		error = DAL_hw_play_seek( &(pDALPlayer->input), seconds*1000);
		if( error != DAL_NO_ERROR)
		{
			return error;
		}		
	}
	else
	{
		error = DAL_sw_play_seek();
		if( error != DAL_NO_ERROR)
		{
			return error;
		}	

	}
	return DAL_NO_ERROR;
}

S32 DAL_Stream_Seek_prev(DAL_PlayerInstance_t player,DAL_Player_Input_t* input,DAL_Player_Output_t* output)
{
	S32 iret=0;

	DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  DAL_Stream_Seek_prev\n");
	if( player < Player1 || player >= PlayerTotal)
	{
		return DAL_ERROR_INVALID_HANDLE;
	}
	if(input->SourceType==SourceType_P2V)
	{
		iret=DAL_P2V_seek_prev(input, output);
		if(iret != DAL_NO_ERROR)
			return iret;
	}
	else if(input->SourceType==SourceType_HLS)
	{
	 	iret=DAL_HLS_seek_prev(input, output);
		if(iret != DAL_NO_ERROR)
			return iret;

	}
	else if(input->SourceType==SourceType_TIMESHIFT)
	{
	 	iret=DAL_TimeShift_seek_prev(input, output);
		if(iret != DAL_NO_ERROR)
			return iret;

	}
	else
	{
		return DAL_ERROR_NOT_SUPPORT;
	}
	return DAL_NO_ERROR;

}


S32 DAL_Stream_Seek_next(DAL_PlayerInstance_t player,DAL_Player_Input_t* input,DAL_Player_Output_t* output)
{
	S32 iret=0;
	
	DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  DAL_Stream_Seek_next\n");

	if( player < Player1 || player >= PlayerTotal)
	{
		return DAL_ERROR_INVALID_HANDLE;
	}
	if(input->SourceType==SourceType_P2V)
	{
	 	iret=DAL_P2V_seek_next(input, output);
		if(iret != DAL_NO_ERROR)
			return iret;
	}
	else if(input->SourceType==SourceType_HLS)
	{
	 	iret=DAL_HLS_seek_next(input, output);
		if(iret != DAL_NO_ERROR)
			return iret;
	}
	else if(input->SourceType==SourceType_TIMESHIFT)
	{
	 	iret=DAL_TimeShift_seek_next(input, output);
		if(iret != DAL_NO_ERROR)
			return iret;

	}
	else
	{
		return DAL_ERROR_NOT_SUPPORT;
	}
	return DAL_NO_ERROR;

}

BOOL DAL_WaitCodec(DAL_PlayerInstance_t player, int flag)
	{
	if( flag == 0){
		return dal_is_stoped();
	}
	else if( flag == 1){
		return DAL_core_is_stoped();
	}
		return FALSE;
	}

S32 DAL_Set_VideoEventCallback(DAL_PlayerInstance_t player,DAL_Event_Notify_t event_callback)
{
	DAL_Player_t*		pDALPlayer = NULL;

	if( player < Player1 || player >= PlayerTotal)
	{
		return DAL_ERROR_INVALID_HANDLE;
	}
	pDALPlayer = dal_get_player_instance( player);
	pDALPlayer->video_event_callback = event_callback;
	return DAL_NO_ERROR;
}

S32 DAL_Set_AudioEventCallback(DAL_PlayerInstance_t player,DAL_Event_Notify_t event_callback)
{
	DAL_Player_t*		pDALPlayer = NULL;

	if( player < Player1 || player >= PlayerTotal)
	{
		return DAL_ERROR_INVALID_HANDLE;
	}
	pDALPlayer = dal_get_player_instance( player);
	pDALPlayer->audio_event_callback = event_callback;
	return DAL_NO_ERROR;
}

S32 DAL_Set_PlayEventCallback(DAL_PlayerInstance_t player,DAL_Event_Notify_t event_callback)
{
	DAL_Player_t*		pDALPlayer = NULL;

	if( player < Player1 || player >= PlayerTotal)
	{
		return DAL_ERROR_INVALID_HANDLE;
	}
	pDALPlayer = dal_get_player_instance( player);
	pDALPlayer->play_event_callback = event_callback;
	return DAL_NO_ERROR;
}

S32 DAL_Get_PlayerInfo(DAL_PlayerInstance_t player, DAL_Player_Info_t* player_info)
{
	DAL_Player_t*		pDALPlayer = NULL;
	DAL_ERROR_t			error = DAL_NO_ERROR;

	if( player < Player1 || player >= PlayerTotal)
	{
		return DAL_ERROR_INVALID_HANDLE;
	}
	if( player_info == NULL)
	{
		return DAL_ERROR_BAD_PARAMETER;
	}

	pDALPlayer = dal_get_player_instance( player);
	if( pDALPlayer->states == PLAYER_IDLE)
	{
		//printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 1 PLAYER_IDLE\n");
		DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 1 PLAYER_IDLE\n");
		memset(player_info, 0, sizeof(DAL_Player_Info_t));
		player_info->states = PLAYER_IDLE;
	}
	else
	{
		// 需要先更新info
		if( pDALPlayer->dec_type == DAL_HardWare)
		{
			error = DAL_hw_play_getinfo( &(pDALPlayer->input), player_info);
			if( error != DAL_NO_ERROR)
			{
				//printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DAL_hw_play_getinfo error\n");
				DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DAL_hw_play_getinfo error\n");
				return error;
			}
		}
		else
		{
			error = DAL_sw_play_getinfo();
			if( error != DAL_NO_ERROR)
			{
				return error;
			}
		}
	}
	memcpy( &(player_info->input), &(pDALPlayer->input), sizeof(DAL_Player_Input_t));
	memcpy( &(player_info->output), &(pDALPlayer->output), sizeof(DAL_Player_Output_t));
	player_info->states = pDALPlayer->states;
	return DAL_NO_ERROR;
}

S32 DAL_Get_AudioLanguage(DAL_PlayerInstance_t player, DAL_LanguageInfo_t* language_info)
{
	return DAL_NO_ERROR;
}

S32 DAL_Set_AudioLanguage(DAL_PlayerInstance_t player, DAL_LanguageType_t  language_type)
{
	return DAL_NO_ERROR;
}

S32 DAL_Stream_PvrStart(DAL_PlayerInstance_t player,const U8* filename)
{
	return DAL_NO_ERROR;
}

S32 DAL_Stream_PvrStop(DAL_PlayerInstance_t player)
{
	return DAL_NO_ERROR;
}

S32 DAL_Stream_TimeshiftStart_Record(DAL_PlayerInstance_t player)
{
	return DAL_NO_ERROR;
}

S32 DAL_Stream_TimeshiftStart_Play(DAL_PlayerInstance_t player)
{
	return DAL_NO_ERROR;
}

S32 DAL_Stream_TimeshiftStop(DAL_PlayerInstance_t player)
{
	return DAL_NO_ERROR;
}


S32 DAL_Stream_GetPlayTime(DAL_PlayerInstance_t player, U32* p_total_time, U32* p_cur_time)
{
	DAL_Player_t*		pDALPlayer = NULL;
	DAL_ERROR_t			error = DAL_NO_ERROR;

	if( player < Player1 || player >= PlayerTotal)
	{
		return DAL_ERROR_INVALID_HANDLE;
	}
	if( p_total_time == NULL || p_cur_time == NULL)
	{
		return DAL_ERROR_BAD_PARAMETER;
	}

	pDALPlayer = dal_get_player_instance( player);
	if( pDALPlayer->states == PLAYER_IDLE)
	{
		*p_total_time = 0;
		*p_cur_time = 0;
		return DAL_ERROR_BAD_PARAMETER;
	}
	else
	{
		if( pDALPlayer->dec_type == DAL_HardWare)
		{
			error = DAL_hw_play_getplaytime( &(pDALPlayer->input), p_total_time, p_cur_time);
			if( error != DAL_NO_ERROR)
			{
				return error;
			}
		}
		else
		{
			*p_total_time = 0;
			*p_cur_time = 0;
			return DAL_ERROR_NOT_SUPPORT;
		}
	}
	return DAL_NO_ERROR;
}


S32 DAL_SetTuner( void)
{
#ifdef HAVE_TUNER_HEADER_FILE
	ts_src_info_t		ts;

	ts.CountryID = 0;
	ts.MediumType = MEDIUM_TYPE_DVBC;
	ts.uiQuality = 0;
	ts.uiSignalSouceID = 0;
	ts.uiStrength = 29;
		
	ts.u.Cable.FrequencyKHertz = 698000;
	ts.u.Cable.SymbolRateSPS = 6875000;
	ts.u.Cable.Modulation = ePI_64QAM;
	ts.u.Cable.FecInner = ePI_FECIN_DVBS_1_2;
	ts.u.Cable.FecOuter = ePI_FECOUT_NOT_CODING;
	
	DRV_Tuner_SetFrequency( 0, &ts);
#endif	
	return 0;	
}

S32 DAL_ffmpeg_test( void)
{
/*
	AVFormatContext*	format_context;
	U32 				iret = 0;
	
	av_register_all();
	format_context = av_malloc(sizeof(AVFormatContext));;
	iret = av_open_input_file(&format_context,"/mount0/mp4.AVI",(AVInputFormat *)NULL,(int)0,(AVFormatParameters*)NULL);
	if( iret != 0)
	{
		printf("av_open_input_file failed with %d\n", iret);
	}
	printf("av_open_input_file success\n");
	
	iret = av_find_stream_info( format_context);
	if( iret != 0)
	{
		printf("av_find_stream_info failed with %d\n", iret);
	}
	printf("av_find_stream_info success\n");
	
	
	iret = av_open_input_file(&format_context,"/mount0/1.MP3",(AVInputFormat *)NULL,(int)0,(AVFormatParameters*)NULL);
	if( iret != 0)
	{
		printf("av_open_input_file failed with %d\n", iret);
	}
	printf("av_open_input_file success\n");
	
	iret = av_find_stream_info( format_context);
	if( iret != 0)
	{
		printf("av_find_stream_info failed with %d\n", iret);
	}
	printf("av_find_stream_info success\n");

*/
	return 0;
}

S32 DAL_ffmpeg_get_mediainfo_test( void)
{
/*
	DAL_Player_t		temp;
	DAL_Player_Input_t	input;

	dal_get_media_info1( &temp, "/mount0/video_test/1.MP4");
	dal_get_media_info1( &temp, "/mount0/video_test/btv.trp");
	//dal_get_media_info1( &temp, "/mount0/video_test/1.MP4");
	//dal_get_media_info1( &temp, "/mount0/video_test/1.MP4");
	//dal_get_media_info1( &temp, "/mount0/video_test/1.MP4");
	//dal_get_media_info1( &temp, "/mount0/video_test/1.MP4");
	*/
	return 0;
}

S32 DAL_PlayEventCallback( dal_play_event_t event, U32 param)
{
	DAL_Player_t*		pDALPlayer = NULL;

	pDALPlayer = dal_get_player_instance(1);
	if( NULL != pDALPlayer && NULL != pDALPlayer->play_event_callback)
	{
		pDALPlayer->play_event_callback(event,(void*)param);
	}
	return 0;
}


S32 DAL_FirstTSPlayStart(void)
{
#ifdef USE_STATIC_FRAME_KO
	playrec_set_prm_setvid_updateDisplay(1);
#endif
	return 0;
}

BOOL dal_is_stoped(void)
{
	DAL_Player_t*		pDALPlayer = dal_get_player_instance(1);;

	if( NULL != pDALPlayer){
		return (pDALPlayer->states == PLAYER_IDLE) ? TRUE : FALSE;
}
	return TRUE;
}


/*-----------------------------------------------------*/
/*              Internal Function	 Definition							*/
/*-----------------------------------------------------*/



/* dal  注册到 dal_core 的事件处理函数*/
void dal_core_event_callback( U32 event, void* param)
{
	DAL_Player_t*		pDALPlayer = NULL;
	
	DebugMessage("[DAL] **********************dal_core_event_callback **********************");
	
	pDALPlayer = dal_get_player_instance(1);
	switch( (dal_core_event_t)event)
	{
		/*p2s play start*/
		case DAL_CORE_EVENT_PLAY_ALREADY_START:
		case DAL_CORE_EVENT_DL_BUF_ALLOC_FAILED:
		case DAL_CORE_EVENT_DL_START_FAILED:
		case DAL_CORE_EVENT_PIDS_PARSE_START_FAILED:
		case DAL_CORE_EVENT_PLAY_PIDS_FAILED:
		case DAL_CORE_EVENT_PLAY_INJECT_FAILED:

		/*flvlist play start*/
		case DAL_CORE_EVENT_PARSE_FLVLIST_START_FAILED:
		case DAL_CORE_EVENT_FLVLIST_PARSE_FAILED:
		case DAL_CORE_EVENT_GET_LIST_URL_FAILED:
		case DAL_CORE_EVENT_PLAY_FLV_FAILED:
		case DAL_CORE_EVENT_SEEK_FAILED:
		{
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			if( pDALPlayer->play_event_callback != NULL){
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_PLAY_START_FAILED, NULL);
			}
			/* dal core failed, reset dal states*/
			pDALPlayer->states = PLAYER_IDLE;
		}
		break;			

		/* mms play start */
		case DAL_CORE_EVENT_MMS_CONNECT_FAILED:	
		case DAL_CORE_EVENT_PLAY_MMS_FAILED:
		{
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			if( pDALPlayer->play_event_callback != NULL){
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_MMS_PLAY_START_FAILED, NULL);
			}
			/* dal core failed, reset dal states*/
			pDALPlayer->states = PLAYER_IDLE;
		}
		break;


		case DAL_CORE_EVENT_DATA_INTERRUPT:
		{
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));		
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_DATA_INTERRUPT, NULL);
			}
		}
		break;
		case DAL_CORE_EVENT_DATA_RESUME:
		{
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_DATA_RESUME, NULL);
			}
		}
		break;
			
		case DAL_CORE_EVENT_STATUS_WIAT_PIDS:
		case DAL_CORE_EVENT_STATUS_WIAT_FLVLIST_PARSE:
		case DAL_CORE_EVENT_PLAY_MMS_BUFFERING:
		case DAL_CORE_EVENT_MMS_CONNECTING:		// 该消息可以分离出来，提示用户更细致的执行情况
		{
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));	
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			/* dal core is waiting for pids, this mean dal is start*/
			pDALPlayer->states = PLAYER_PLAY;
			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_BUFFERING, param);
			}	
		}
		break;
		case DAL_CORE_EVENT_PLAY_PIDS_OK:
		case DAL_CORE_EVENT_PLAY_FLV_OK:
		{
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			pDALPlayer->states = PLAYER_PLAY;
		}
		break;
		case DAL_CORE_EVENT_RESTART_SAME_URL:
		{	
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));	
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_RESTART_SAME_URL, NULL);
			}
		}
		break;		
		case DAL_CORE_EVENT_PLAY_WILL_STOP:
		{
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
			
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_PLAY_WILL_STOP, NULL);
			}
		}
		break;
		case DAL_CORE_EVENT_CONNECT_ERROR:
		{

			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
		}
		break;
		case DAL_CORE_EVENT_BEFORE_PLAYREC_STOP:
		{
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
		}
		break;		
		case DAL_CORE_EVENT_STOP_OK:
		{
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));

			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_PLAY_STOP_OK, NULL);
			}
			pDALPlayer->states = PLAYER_IDLE;
		}
		break;	
		case DAL_CORE_EVENT_PAUSE_OK:
		{
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));

			if( pDALPlayer->play_event_callback != NULL)
			{
				amlogic_Get_Pause_Video_Frame();
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_PLAY_PAUSE_OK, NULL);
			}
			pDALPlayer->states = PLAYER_PAUSE;
		}
		break;
		case DAL_CORE_EVENT_RESUME_OK:
		{
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
			
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_PLAY_RESUME_OK, NULL);
			}
			pDALPlayer->states = PLAYER_PLAY;
		}
		break;
		case DAL_CORE_EVENT_SEEK_OK:
		{
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));

			if( pDALPlayer->play_event_callback != NULL)
			{
				if( pDALPlayer->states == PLAYER_PLAY)
				{
					pDALPlayer->play_event_callback( DAL_PLAY_EVENT_PLAY_RESUME_OK, NULL);
				}
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_PLAY_SEEK_OK, NULL);
			}
		}
		break;
		case DAL_CORE_EVENT_SEEK_ALREADY_LAST:
		case DAL_CORE_EVENT_SEEK_ALREADY_FIRST:
		{
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
		}
		break;

		case DAL_CORE_EVENT_SEEK_ACTION_START:
		{
			//DAL_DEBUG(("                    >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_PLAY_SEEK_ACTION_START, param);
			}			
		}
		break;
		case DAL_CORE_EVENT_SEEK_ACTION_DONE:
		{
			//DAL_DEBUG(("                    >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_PLAY_SEEK_ACTION_DONE, NULL);
			}			
		}
		break;

		
		case DAL_CORE_EVENT_FRIST_FRAME_DISPLAY:
		{
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_FIRST_FRAME, NULL);
			}
		}
		break;
		case DAL_CORE_EVENT_PLAY_MMS_OK:
		{
			//DAL_DEBUG(("                                            >>>>> %s <<<<<\n", DAL_CoreEventString(event)));
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			//( pDALPlayer->play_event_callback != NULL)
			//{ /* MMS 播放成功时发送DAL_PLAY_EVENT_FIRST_FRAME  消息清除banner */
				//pDALPlayer->play_event_callback( DAL_PLAY_EVENT_FIRST_FRAME, NULL);
			//}
		}
		break;

		//hls
		case DAL_CORE_EVENT_TIMESHIFT_STOP_BAD_STATUS:
		case DAL_CORE_EVENT_HLS_STOP_BAD_STATUS:
		{
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));

		}
		break;
		case DAL_CORE_EVENT_TIMESHIFT_START_BAD_STATUS:
		case DAL_CORE_EVENT_M3U8_START_BAD_STATUS:
		{
			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_PLAY_START_FAILED, NULL);
			}
			/* dal core failed, reset dal states*/
			pDALPlayer->states = PLAYER_IDLE;
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
		}
		break;
		case DAL_CORE_EVENT_M3U8_RESTART_BAD_STATUS:
		{
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));

		}
		break;
		case DAL_CORE_EVENT_M3U8_DOWNLOAD_BAD_STATUS:
		{
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			
			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_M3U8_DOWNSTART_FAILED, NULL);
			}
		}
		break;

		/* 不应该出现的事件*/
		case DAL_CORE_EVENT_NEW_PIDS_BAD_STATUS:
		case DAL_CORE_EVENT_PIDS_CHANGED_BAD_STATUS:
		case DAL_CORE_EVENT_PID_CHANGED_FAILED:
		case DAL_CORE_EVENT_PID_CHANGED_OK:
		case DAL_CORE_EVENT_RESTART_BAD_STATUS:
		case DAL_CORE_EVENT_FLV_PARSE_OK_BAD_STATUS:
		case DAL_CORE_EVENT_P2S_STOP_BAD_STATUS:
		case DAL_CORE_EVENT_FLV_STOP_BAD_STATUS:
		case DAL_CORE_EVENT_PAUSE_FAILED:
		{
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
		}
		break;	
		
		/*p2v*/
		case DAL_CORE_EVENT_P2V_RESUME_DL_BUF_ALLOC_FAILED:
		case DAL_CORE_EVENT_P2V_RESUME_DL_START_FAILED:
		case DAL_CORE_EVENT_P2V_RESUME_PIDS_PARSE_START_FAILED:
		/*timeshift*/	
		case DAL_CORE_EVENT_TIMESHIFT_RESUME_DL_BUF_ALLOC_FAILED:
		case DAL_CORE_EVENT_TIMESHIFT_RESUME_DL_START_FAILED:
		case DAL_CORE_EVENT_TIMESHIFT_RESUME_PIDS_PARSE_START_FAILED:
			
		{
			DebugMessage("[DAL] dal_core_event_callback    -- >>>>> %s <<<<<\n", DAL_CoreEventString(event));
			
			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_P2V_RESUME_FAILED, NULL);
			}
			pDALPlayer->states = PLAYER_PAUSE;

		}
		break;
		case DAL_CORE_EVENT_TIMESHIFT_PLAYEND:
		case DAL_CORE_EVENT_PLAYEND:
		{
			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_PLAY_END, NULL);
			}

		}
		break;
		
		case DAL_CORE_EVENT_P2V_SEEK_FILE_END:
		case DAL_CORE_EVENT_HLS_SEEK_FILE_END:
		case DAL_CORE_EVENT_TIMESHIFT_SEEK_FILE_END:
		{
			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_PLAY_SEEK_END, NULL);
			}

		}
		break;
		
		case DAL_CORE_EVENT_P2V_SEEK_FILE_TITLE:
		case DAL_CORE_EVENT_HLS_SEEK_FILE_TITLE:
		case DAL_CORE_EVENT_TIMESHIFT_SEEK_FILE_TITLE:
		{
			if( pDALPlayer->play_event_callback != NULL)
			{
				pDALPlayer->play_event_callback( DAL_PLAY_EVENT_PLAY_SEEK_TITLE, NULL);
			}

		}
		break;
		default:
		break;
	}
}

#ifdef USE_ST_PLAYREC

/* dal 注册到STEVT 的事件处理函数*/
void dal_av_event_callback(STEVT_CallReason_t Reason,STEVT_EventConstant_t Event,const void *EventData)
{	

	DAL_Player_t*		pDALPlayer = NULL;

	pDALPlayer = dal_get_player_instance(1);
	if( pDALPlayer->states == PLAYER_IDLE)
	{
		return ;
	}
	if( g_cur_hardware_player == NULL)
	{
		//DAL_ERROR(("g_cur_hardware_player is NULL\n"));
		return ;
	}
	
	switch( Event)
	{
		case STVID_NEW_PICTURE_TO_BE_DISPLAYED_EVT:
		{
			//printf("in dal_av_event_callback STVID_NEW_PICTURE_TO_BE_DISPLAYED_EVT\n");
			if( g_cur_hardware_player->video_event_callback != NULL)
			{
				g_cur_hardware_player->video_event_callback((U32)Event, (void*)EventData);
			}
		}
		break;
		case STVID_SEQUENCE_INFO_EVT:
		{
		//	printf("in dal_av_event_callback STVID_SEQUENCE_INFO_EVT\n");
		}
		break;
		case STVID_USER_DATA_EVT:
		{
		//	printf("in dal_av_event_callback STVID_USER_DATA_EVT\n");
		}
		break;
		case STVID_FRAME_RATE_CHANGE_EVT:
		{
		//	printf("in dal_av_event_callback STVID_FRAME_RATE_CHANGE_EVT\n");
		}
		break;
		case STVID_ASPECT_RATIO_CHANGE_EVT:
		{
		//	printf("in dal_av_event_callback STVID_ASPECT_RATIO_CHANGE_EVT\n");
		}
		break;
		case STAUD_NEW_FRAME_EVT:
		{
			//printf("in dal_av_event_callback STAUD_NEW_FRAME_EVT\n");
			if( g_cur_hardware_player->audio_event_callback != NULL)
			{
				g_cur_hardware_player->audio_event_callback((U32)Event, (void*)EventData);
			}
		}
		break;
		default:
		break;
	}


}
#endif

/* dal 注册到到Playrec 的时间回调函数*/
void dal_playrec_event_callback(unsigned int event)
{
	//DAL_DEBUG(("Entry\n"));
	
	DebugMessage("[DAL] dal_core_event_callback ->Entry ");
	if( g_cur_hardware_player == NULL)
	{
		return ;
	}
#ifdef USE_ST_PLAYREC

	switch(event)
	{
		case PLAYREC_EVT_END_OF_PLAYBACK:
		{
			SYS_DEBUG(SD_DEBUG,MODULE_DAL,"PLAYREC_EVT_END_OF_PLAYBACK\n");
			{
				flv_play_list_t*			p_flv_list = NULL;
				p_flv_list = flv_list_get_list();
				SYS_DEBUG(SD_DEBUG,MODULE_DAL,"--------------------------------------PLAYREC_EVT_END_OF_PLAYBACK   %d %d\n",p_flv_list->num,p_flv_list->cur_play);
				SYS_DEBUG(SD_DEBUG,MODULE_DAL,"--------------------------------------PLAYREC_EVT_END_OF_PLAYBACK \n");
				
			}
			if( g_cur_hardware_player->input.SourceType == SourceType_FLV_LIST)
			{
				SYS_DEBUG(SD_DEBUG,MODULE_DAL,"--------------------------------------PLAYREC_EVT_END_OF_PLAYBACK SourceType_FLV_LIST\n");
				if( g_cur_hardware_player->play_event_callback != NULL 
					&& get_ststream_http_break_flag() == 0 /* 必须确定不是seek等用户主动中断引起的事件*/)
				{
					g_cur_hardware_player->play_event_callback(DAL_PLAY_EVENT_FLVLIST_END,NULL);
				}				
			}
			else
			{
				SYS_DEBUG(SD_DEBUG,MODULE_DAL,"--------------------------------------PLAYREC_EVT_END_OF_PLAYBACK NOT SourceType_FLV_LIST\n");
				if( g_cur_hardware_player->play_event_callback != NULL)
				{
					g_cur_hardware_player->play_event_callback(DAL_PLAY_EVENT_PLAY_END,NULL);
				}
			}
		}
		break;
		case PLAYREC_EVT_END_OF_FILE:
		{
			SYS_DEBUG(SD_DEBUG,MODULE_DAL," PLAYREC_EVT_END_OF_FILE\n");
		}
		break;
		default:
		break;
	}
#endif
}

DAL_Player_t* dal_get_player_instance( DAL_PlayerInstance_t player)
{
	return &(g_DALPlayer[player-1]);
}

DAL_Player_t* dal_get_player_instance_by_dectype( DAL_DecType_t dectype)
{
	int			i = 0;
	
	for( i = 0; i <= MAX_DAL_PLAYER_NUM; i++)
	{
		if( g_DALPlayer[i].dec_type == dectype)
		{
			return &(g_DALPlayer[i]);
		}
	}
	return NULL;
}

DAL_ERROR_t dal_check_input( DAL_Player_Input_t* input)
{
	if( input == NULL)
	{
		return DAL_ERROR_BAD_PARAMETER;
	}

	switch( input->SourceType)
	{
		case SourceType_TUNER:
		{
			
		}
		break;
		case SourceType_HTTP:
		{

		}
		break;
		case SourceType_UDP:
		{
			//return DAL_ERROR_NOT_SUPPORT;
		}
		break;
		case SourceType_FILE:
		{
			
		}
		break;
		case SourceType_BUFFER:
		{
			//return DAL_ERROR_NOT_SUPPORT;
		}
		break;
		default:
		{
			//return DAL_ERROR_NOT_SUPPORT;
		}
		break;
	}
	
	return DAL_NO_ERROR;
}
DAL_ERROR_t dal_check_output( DAL_Player_Output_t* output)
{
	if( output == NULL)
	{
		return DAL_ERROR_BAD_PARAMETER;
	}

	//switch( output.Type)
	{

	}
	return DAL_NO_ERROR;
}

DAL_ERROR_t dal_get_media_info( DAL_Player_Input_t* input, DAL_MediaInfo_t* meida_info)
{
/*
	DAL_ERROR_t			error = DAL_NO_ERROR;

	if( input == NULL || meida_info == NULL)
	{
		return DAL_ERROR_BAD_PARAMETER;
	}

	switch( input->SourceType)
	{
		case SourceType_TUNER:
		{
			error = DAL_NO_ERROR;
		}
		break;
		case SourceType_HTTP:
		case SourceType_FILE:	
		{
			AVFormatContext*	format_context;
			U32					iret = 0;
			U32					i = 0;

			av_register_all();
			format_context = av_malloc(sizeof(AVFormatContext));;
			iret = av_open_input_file(&format_context,"/mount0/mp4.AVI",(AVInputFormat *)NULL,(int)0,(AVFormatParameters*)NULL);
			if( iret != 0)
			{
				printf("av_open_input_file failed with %d\n", iret);
			}
			printf("av_open_input_file success\n");

			iret = av_find_stream_info( format_context);
			if( iret != 0)
			{
				printf("av_find_stream_info failed with %d\n", iret);
			}
			printf("av_find_stream_info success\n");

			memset( meida_info, 0, sizeof(DAL_MediaInfo_t));
			for( i = 0; i < format_context->nb_streams; i++)
			{
				if( format_context->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)
				{
					meida_info->video_info[meida_info->video_num].stream_index = i;
					meida_info->video_info[meida_info->video_num].format = dal_ffmpeg2dal_fmt(format_context->streams[i]->codec->codec_id);
					meida_info->video_info[meida_info->video_num].height = format_context->streams[i]->codec->height;
					meida_info->video_info[meida_info->video_num].width = format_context->streams[i]->codec->width;
					meida_info->video_info[meida_info->video_num].bitRate = format_context->streams[i]->codec->bit_rate;
					meida_info->video_info[meida_info->video_num].frameRate = (double)(format_context->streams[i]->r_frame_rate.num)/(double)(format_context->streams[i]->r_frame_rate.den);
					meida_info->video_info[meida_info->video_num].duration = format_context->streams[i]->duration;
					meida_info->video_num++;
				}
				else if( format_context->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO)
				{
					meida_info->audio_info[meida_info->audio_num].stream_index = i;
					meida_info->audio_info[meida_info->audio_num].format = dal_ffmpeg2dal_fmt(format_context->streams[i]->codec->codec_id);
					meida_info->audio_info[meida_info->audio_num].bitRate = format_context->streams[i]->codec->bit_rate;
					meida_info->audio_info[meida_info->audio_num].channels = format_context->streams[i]->codec->channels;
					meida_info->audio_info[meida_info->audio_num].blockAlign = format_context->streams[i]->codec->block_align;
					meida_info->audio_info[meida_info->audio_num].sampleBits = format_context->streams[i]->codec->bits_per_sample;
					meida_info->audio_info[meida_info->audio_num].sampleRate = format_context->streams[i]->codec->sample_rate;
					meida_info->audio_info[meida_info->audio_num].duration = format_context->streams[i]->duration;
					meida_info->audio_info[meida_info->audio_num].language = dal_ffmpeg2dal_language(format_context->streams[i]->language);
					meida_info->audio_num++;
				}
				else if( format_context->streams[i]->codec->codec_type == CODEC_TYPE_SUBTITLE)
				{
					meida_info->sub_info[meida_info->sub_num].stream_index = i;
					meida_info->sub_num++;
					//
				}
				else
				{
					//
				}
			}
			
			av_close_input_file( format_context);
			av_free(format_context);
			
		}
		break;
		case SourceType_UDP:
		{

		}
		break;
		case SourceType_BUFFER:
		{

		}
		break;
		default:
		{
			
		}
		break;

	}

	//print_media_info(pDALPlayer);
	*/
	return DAL_NO_ERROR;
}

DAL_ERROR_t dal_get_media_info1( DAL_Player_t* pDALPlayer, const char* filename)
{
/*
	DAL_ERROR_t			error = DAL_NO_ERROR;


			DAL_MediaInfo_t*	meida_info = &(pDALPlayer->media_info);
			AVFormatContext*	format_context;
			U32					iret = 0;
			U32					i = 0;

			av_register_all();
			format_context = av_malloc(sizeof(AVFormatContext));;
			iret = av_open_input_file(&format_context, filename,(AVInputFormat *)NULL,(int)0,(AVFormatParameters*)NULL);
			if( iret != 0)
			{
				printf("av_open_input_file failed with %d\n", iret);
			}
			printf("av_open_input_file success\n");

			iret = av_find_stream_info( format_context);
			if( iret != 0)
			{
				printf("av_find_stream_info failed with %d\n", iret);
			}
			printf("av_find_stream_info success\n");

			memset( meida_info, 0, sizeof(DAL_MediaInfo_t));
			for( i = 0; i < format_context->nb_streams; i++)
			{
				if( format_context->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)
				{
					meida_info->video_info[meida_info->video_num].stream_index = i;
					meida_info->video_info[meida_info->video_num].format = dal_ffmpeg2dal_fmt(format_context->streams[i]->codec->codec_id);
					meida_info->video_info[meida_info->video_num].height = format_context->streams[i]->codec->height;
					meida_info->video_info[meida_info->video_num].width = format_context->streams[i]->codec->width;
					meida_info->video_info[meida_info->video_num].bitRate = format_context->streams[i]->codec->bit_rate;
					meida_info->video_info[meida_info->video_num].frameRate = (double)(format_context->streams[i]->r_frame_rate.num)/(double)(format_context->streams[i]->r_frame_rate.den);
					//meida_info->video_info[meida_info->video_num].duration = format_context->streams[i]->duration;
					meida_info->video_info[meida_info->video_num].duration = (double)format_context->streams[i]->duration * (double)format_context->streams[i]->r_frame_rate.den / (double)format_context->streams[i]->r_frame_rate.num;
					meida_info->video_num++;
				}
				else if( format_context->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO)
				{
					meida_info->audio_info[meida_info->audio_num].stream_index = i;
					meida_info->audio_info[meida_info->audio_num].format = dal_ffmpeg2dal_fmt(format_context->streams[i]->codec->codec_id);
					meida_info->audio_info[meida_info->audio_num].bitRate = format_context->streams[i]->codec->bit_rate;
					meida_info->audio_info[meida_info->audio_num].channels = format_context->streams[i]->codec->channels;
					meida_info->audio_info[meida_info->audio_num].blockAlign = format_context->streams[i]->codec->block_align;
					meida_info->audio_info[meida_info->audio_num].sampleBits = format_context->streams[i]->codec->bits_per_sample;
					meida_info->audio_info[meida_info->audio_num].sampleRate = format_context->streams[i]->codec->sample_rate;
					//meida_info->audio_info[meida_info->audio_num].duration = format_context->streams[i]->duration;
					meida_info->audio_info[meida_info->audio_num].duration = (double)format_context->streams[i]->duration * (double)format_context->streams[i]->time_base.num / (double)format_context->streams[i]->time_base.den;
					
					meida_info->audio_info[meida_info->audio_num].language = dal_ffmpeg2dal_language(format_context->streams[i]->language);
					meida_info->audio_num++;
				}
				else if( format_context->streams[i]->codec->codec_type == CODEC_TYPE_SUBTITLE)
				{
					meida_info->sub_info[meida_info->sub_num].stream_index = i;
					meida_info->sub_num++;
					//
				}
				else
				{
					//
				}
			}
			
			av_close_input_file( format_context);
			av_free(format_context);
			


	print_media_info(pDALPlayer);
	*/
	return DAL_NO_ERROR;
}


DAL_ERROR_t dal_get_decode_type( DAL_MediaInfo_t* media_info, DAL_DecType_t* type)

{
	if( type == NULL)
	{

	}
	//
	*type = DAL_HardWare;
	//
	return DAL_NO_ERROR;
}

/*

DAL_Fmt_t dal_ffmpeg2dal_fmt( enum CodecID codec_id)
{

	switch(codec_id)
	{
		//video
		case CODEC_ID_MPEG4:		return DAL_FORMAT_MPEG4;
		case CODEC_ID_MPEG2VIDEO:	return DAL_FORMAT_MPEG2VIDEO;

		//aduio
		case CODEC_ID_AAC:		return DAL_FORMAT_AAC;
		case CODEC_ID_MP2:		return DAL_FORMAT_MP2;
		case CODEC_ID_MP3:		return DAL_FORMAT_MP3;

		//other
		default:				return DAL_FORMAT_OTHER;
	}
}
*/

char* dal_fmt2string( DAL_Fmt_t fmt)
{
	switch(fmt)
	{
		case DAL_FORMAT_AAC:	return "DAL_FORMAT_AAC";
		case DAL_FORMAT_MP2:	return "DAL_FORMAT_MP2";
		case DAL_FORMAT_MP3:	return "DAL_FORMAT_MP3";

		case DAL_FORMAT_MPEG4:	return "DAL_FORMAT_MPEG4";

		default:				return "DAL_FORMAT_OTHER";	
	}
}

DAL_LanguageType_t dal_ffmpeg2dal_language( char* language)
{
	return CH;
}

void print_media_info( DAL_Player_t* pDALPlayer)
{
	DAL_VideoInfo_t*		p_videoinfo;
	DAL_AudioInfo_t*		p_audioinfo;
	DAL_SubInfo_t*			p_subinfo;
	int						i = 0;

	for( i = 0; i < pDALPlayer->media_info.video_num; i++)
	{
		p_videoinfo = &(pDALPlayer->media_info.video_info[i]);
		printf("video index %d   stream_index: %d\n", i, p_videoinfo->stream_index);
		printf("format: %s\n", dal_fmt2string(p_videoinfo->format));
		printf("width: %d  height: %d\n", p_videoinfo->width, p_videoinfo->height);
		printf("frameRate: %f  bitRate: %d  duration: %f\n\n", p_videoinfo->frameRate, p_videoinfo->bitRate, p_videoinfo->duration);
	}
	for( i = 0; i < pDALPlayer->media_info.audio_num; i++)
	{
		p_audioinfo = &(pDALPlayer->media_info.audio_info[i]);
		printf("audio index: %d  stream_index: %d\n", i, p_audioinfo->stream_index);
		printf("format: %s\n", dal_fmt2string(p_audioinfo->format));
		printf("bitRate: %d  sampleRate: %f  sampleBits: %d  channels: %d  blockAlign: %d\n",
			p_audioinfo->bitRate,p_audioinfo->sampleRate,p_audioinfo->sampleBits,p_audioinfo->channels,p_audioinfo->blockAlign);
		printf("duration: %f  language: %d\n\n", p_audioinfo->duration,p_audioinfo->language);
	}		
	for( i = 0; i < pDALPlayer->media_info.sub_num; i++)
	{
		p_subinfo = &(pDALPlayer->media_info.sub_info[i]);
		printf("subt index: %d  stream index %d\n", i, p_subinfo->stream_index);
	}		
	for( i = 0; i < pDALPlayer->media_info.ttx_num; i++)
	{

	}		
}

void dal_set_state(DAL_PlayerInstance_t id, DAL_Player_States_t state)
{
	DAL_Player_t*		pDALPlayer = dal_get_player_instance(id);
	if( pDALPlayer){
		pDALPlayer->states = state;
		//SYS_DEBUG(SD_DEBUG,MODULE_DAL,">>>>> >>>>> >>>>> dal set state: %s", dal_status_string(state));
		
		DebugMessage("[DAL][dal_set_state]-->>>>>  dal set state: %s", dal_status_string(state));
	}
}

char* dal_status_string(DAL_Player_States_t status)
{
#define E(e) case e: return #e
	switch(status)
	{
		E(PLAYER_IDLE);
		E(PLAYER_PLAY);
		E(PLAYER_PAUSE);
		E(PLAYER_PVR_RECORD);
		E(PLAYER_TIMESHIFT_RECORD);
		E(PLAYER_TIMESHIFT_PLAY);
		default: return "unknow status";
	}
}

/******************************************************************************
* Function : 
* parameters :		 
* Return :	
* Description :
* Author : zyliu
******************************************************************************/

/******************************************************************************
* Function : 	
* Description :
* Author : zyliu
******************************************************************************/

struct timeval g_tv_0={0,0};
struct timeval g_tv_1={0,0};
void sys_print_time(int i, const char* tag){

	int diff_s = 0;
	gettimeofday(&g_tv_1, 0);
	diff_s = g_tv_1.tv_sec - g_tv_0.tv_sec;
	printf("\n\n\n\n\n");
	printf("g_tv_0.tv_sec: %u  g_tv_0.tv_usec: %u  g_tv_1.tv_sec: %u  g_tv_1.tv_usec: %u \n",g_tv_0.tv_sec,g_tv_0.tv_usec,g_tv_1.tv_sec,g_tv_1.tv_usec);	
	if(  diff_s > 0){
		printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^[%d][%s]      us: %u\n", i,tag, g_tv_1.tv_usec + diff_s*1000000 - g_tv_0.tv_usec);
	}else{ 
		printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^[%d][%s]      us: %u\n", i,tag, g_tv_1.tv_usec - g_tv_0.tv_usec);
	}
	printf("\n\n\n\n\n");
	g_tv_0.tv_usec = g_tv_1.tv_usec;
	g_tv_0.tv_sec = g_tv_1.tv_sec;

}

void sys_print_pid( const char* name)
{
/*
	printf("\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~new tak create name:  %s    id: %u\n", name, OSAL_GetTaskID());
	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n");
*/
}


//char url[5][256] ={"http://192.168.30.210:8080/group1/M00/01/25/wKgesFKB48WaGLb2CYDAAPrJLH06143.ts","0","0","0","0"};
//char url[5][256] ={"http://127.0.0.1:9906/p2p-vod/124.219.23.162:4000/123456/311dbea0db93fa40b8e7d03d85cf4ed0","0","0","0","0"};
//char url[5][256] ={"http://124.160.184.137:80/live/5/30/3791a3d4b5214540bfe99b0680fbc76f.m3u8?type=tv","0","0","0","0"};
//char url[5][256]={"udp://239.1.1.1:1234","0","0","0","0"}; 
//char url[10][256] ={"http://tsl1.hls.cutv.com/cutvlive/2q76Sw2/hls/live_sd.m3u8","http://tsl5.hls.cutv.com/cutvlive/hw2bvyV/hls/live_sd.m3u8","http://biz.vsdn.tv380.com/playlive.php?5B63686E5D445830303030303034367C343535357C317C313030307C35326974762E636E7C687474707C74735B2F63686E5DVSDNSOOONERCOM00","http://qqlive.rec.hdl.lxdns.com/1128831868.flv","http://so.52itv.cn/live/72676552616B34796E4E6C73484E516B726634376F6759386E41314453356E51.m3u8","http://hlslive1.cntv.wscdns.com/cache/24_/seg0/index.m3u8","http://hlslive1.cntv.wscdns.com/cache/44_/seg0/index.m3u8"};


char url[5][256] ={"http://192.168.30.210:8080/group3/M00/07/CE/wKge-1KDC02jLV8IAE0RuM19bbg4804504","0","0","0","0"};
int iUrl_num=0;


U32 dal_http_start(void)
{
	int						iret = 0,i=0;
	S32 					Error = 0;
	DAL_Player_Input_t 		DAL_Input;
	DAL_PlayerInstance_t    player=1;
	DAL_Player_Output_t     output;

#if 1	
//	char *url = "http://192.168.0.80:12345";
//	char *url = "http://10.10.90.228:8080/3000v_ts_out.ts";
//	char *url = "udp://239.1.1.1:6666";
//	char *url ="http://sw.live.cntv.cn/cctv_p2p_hdcctv1.m3u8";
//                  "http://so.52itv.cn/live/72676552616B34796E4E6C73484E516B726634376F67767953356E73.m3u8";
//	char *url ="http://192.168.30.251/flist11.m3u8";
//	char *url ="http://192.168.0.201/hls_server/harry/cache_file.m3u8";
//	char *url ="http://hlslive1.cntv.wscdns.com/cache/9_/seg0/index.m3u8 ";
//   char *url ="http://192.168.30.216:9906/p2p-live/192.168.0.201:3001/lxl/ee8ee6fd017b40e0c6800cf4ceb5c9ba";
    char header[256]="data/data/com.pbi.dvb/header";
    char file[256]="data/data/com.pbi.dvb/context";
	Set_Http_header_addr(header);
	Set_Http_File_addr(file);


//	test_timeshift_url();


	memset(&output,0,sizeof(DAL_Player_Output_t));
	memset(&DAL_Input.Source.http.URL,0,255);
//	memset(&DAL_Input.Source.udp.URL,0,255);

	DebugMessage("[dal] dal_http_start  in iUrl_num=%d!",iUrl_num);

/*	iret = DAL_Init();
	if( iret != 0)
	{
		DebugMessage("[DAL][dal_http_start]:DAL_Init failed\n");
		return DAL_ERROR_BAD_PARAMETER;
	}*/


//	DAL_Input.Source.udp.UDPTYPE=1;
//	strncpy(DAL_Input.Source.udp.URL, url[i], strlen(url[i]));

	i=iUrl_num;
//	set_mpts_audio_pid(68,0x03);
//	set_mpts_video_pid(70,0x1b);

	strncpy(DAL_Input.Source.http.URL, url[i], strlen(url[i]));
	DebugMessage("[DAL][dal_http_start]:DAL_Stream_Start :%s\n", DAL_Input.Source.http.URL);
	DAL_Input.SourceType =SourceType_TIMESHIFT;//SourceType_UDP;//SourceType_HLS;//SourceType_P2V;//SourceType_P2V;//SourceType_TIMESHIFT;SourceType_HLS
//	iUrl_num+=1;
	
	if(iUrl_num==5)
		iUrl_num=0;
/*	{
		U32 error=0;
			error = SetDemuxP2S();
		if( error != 0)
		{
			printf("[dal_http_start]:SetDemuxP2S failed:%d\n",error);
			return iret;
		}
	}*/
	system("echo 1 > /sys/class/graphics/fb0/blank");
	Error = DAL_Stream_Start(player, &DAL_Input, &output);
	if( Error != 0)
	{
		DebugMessage("[DAL][dal_http_start]:DAL_Stream_Start failed with %d\n", Error);
		return Error;
	}
	
	DebugMessage("[dal] dal_http_start  OK ! iUrl_num=%d!",iUrl_num);
	return Error;
#else

//	Error= dal_dowload_m3u8();
//	DebugMessage("[dal] res	%x !",Error);

{
	char mm_url[]={"http://192.168.30.251:7500/v1/tfs/T14RdTByKT1RCvBVdK&key=1234567898765432&time=23456789"};

	char *pKey=NULL,*pTTime=NULL;
	char Key[8]={0},tmp[2]={0};
	char time[20]={0};
	U32 movietime=0,i=0,y=0;
	

	
	pKey=strstr(mm_url,"&key=");
	pKey=pKey+5;
	
	for(i=0,y=0;i<16;i=i+2)
	{
		if(strncpy(tmp,pKey,2)==NULL)
		{
			DebugMessage("[DAL][dal_http_start]:key copy failed !");
			return 1;
		}	
		pKey=pKey+2;
		Key[y]=atoi(tmp);
		DebugMessage("[DAL][dal_http_start]:key %d !",Key[y]);
			y++;
	}


	pTTime=strstr(mm_url,"&time=");
	pTTime=pTTime+6;
	if(strncpy(time,pTTime,strlen(pTTime))==NULL)
	{
		DebugMessage("[DAL][dal_http_start]:time copy failed!");
		return 1;
	}
	movietime =atoi(time);
	
	DebugMessage("[DAL][dal_http_start]:time %s - %d!",time,movietime);
}
return Error;

#endif
}

U32 dal_http_stop(void)
{
#if 1
	S32 ret=0;
	DAL_PlayerInstance_t	m_player=1;

	ret=DAL_Stream_Stop(m_player);
	if( ret != 0)
		DebugMessage("[dal_http_stop]:DAL_Stream_Stop failed with %d\n", ret);
	else
		DebugMessage("[dal_http_stop] OK\n");
	return ret;
#else
//S32 					Error = 0;

//Error= dal_dowload_m3u8_first();
//DebugMessage("[dal] res first%x !",Error);

//return Error;
DAL_PlayerInstance_t	player=1;
S32 					Error = 0,i=0;
DAL_Player_Input_t		DAL_Input;
DAL_Player_Output_t    output;

memset(&output,0,sizeof(DAL_Player_Output_t));
memset(&DAL_Input.Source.http.URL,0,255);
i=iUrl_num;

strncpy(DAL_Input.Source.http.URL, url[i], strlen(url[i]));
DAL_Input.SourceType =SourceType_HLS;
DAL_Input.param=10;

//DebugMessage(" get p2v film total time :%d ms",Get_P2V_file_info());
Get_HLS_file_info();
//Get_play_time();

//Error = DAL_Stream_Pause(player);
Error = DAL_Stream_Seek_prev(player,&DAL_Input,&output);
if( Error != 0)
{
	DebugMessage("[DAL][dal_http_stop]:DAL_Stream_Pause failed with %d\n", Error);
return Error;
}


return 0;
#endif
}

U32 dal_http_start2(void)
{

	int						iret = 0,i=0;
	S32 					Error = 0;
	DAL_Player_Input_t 		DAL_Input;
	DAL_PlayerInstance_t    player=1;
	DAL_Player_Output_t    output;
#if 1	
	//	char *url = "http://192.168.0.80:12345";
	//	char *url = "http://10.10.90.228:8080/3000v_ts_out.ts";
	//	char *url = "udp://239.1.1.1:6666";
	
	//	char *url ="http://sw.live.cntv.cn/cctv_p2p_hdcctv1.m3u8";
	//	char *url ="http://192.168.30.251/flist11.m3u8";
	//	char *url ="http://192.168.0.201/hls_server/harry/cache_file.m3u8";
	//	char *url ="http://192.168.30.251:7502/T1Oy_TB4KT1RCvBVdK.m3u8";
	//    char *url ="http://192.168.30.216:9906/p2p-live/192.168.0.201:3001/lxl/ee8ee6fd017b40e0c6800cf4ceb5c9ba";

	memset(&output,0,sizeof(DAL_Player_Output_t));
	memset(&DAL_Input.Source.http.URL,0,255);

	//	DAL_Input.Source.udp.UDPTYPE=1;
	strncpy(DAL_Input.Source.http.URL, url[i], strlen(url[i]));
	set_mpts_audio_pid(69,0x03);
	set_mpts_video_pid(71,0x1b);

	//strncpy(DAL_Input.Source.http.URL, url, strlen(url));
	DAL_Input.SourceType = SourceType_P2S;//SourceType_P2S;//SourceType_UDP;//SourceType_P2V;
	
	Error = DAL_Stream_Start(player, &DAL_Input, &output);
	if( Error != 0)
	{
		DebugMessage("[DAL][dal_http_start2]:DAL_Stream_Start failed with %d\n", Error);
		return Error;
	}
	
	return 0;
#else
	Error= dal_dowload_m3u8_pro();
	DebugMessage("[dal] res pro %x !",Error);

	return Error;

#endif
}

static int a=0;

U32 dal_http_restart(void)
{
	int							iret = 0,i=0;
	S32 					Error = 0;
	DAL_Player_Input_t 		DAL_Input;
	DAL_PlayerInstance_t    player=1;
	DAL_Player_Output_t    output;
	
	//char *url = "http://192.168.0.80:12345";
//	char *url = "http://10.10.90.228:8080/3000v_ts_out.ts";
	//	char *url = "udp://239.1.1.1:6666";
//char url[255];
#if 0
	memset(url,0,sizeof(url));
	
	if(a==1)
	{
		//sprintf(url,"http://127.0.0.1:9906/p2p-live/211.39.253.179:3001/353535/78119aff0378524b31f1b6b92bb416bf");
		//sprintf(url,"http://127.0.0.1:9906/p2p-live/192.168.0.201:3001/lxl/ee8ee6fd017b40e0c6800cf4ceb5c9ba");
		sprintf(url,"http://hlslive1.cntv.wscdns.com/cache/7_/seg0/index.m3u8");
		a=0;
	}
	else if(a==0)
	{
		//sprintf(url,"http://127.0.0.1:9906/p2p-live/61.38.252.88:3001/353535/2837c87fd7584066ce858b1fd1c97e15"); 
		//sprintf(url,"http://127.0.0.1:9906/p2p-live/192.168.0.201:3001/lxl/789ec20964e85b1ec6a052a71e369e45");
		sprintf(url,"http://hlslive1.cntv.wscdns.com/cache/8_/seg0/index.m3u8");
		a=1;
	}

#endif
#if 0
	memset(&output,0,sizeof(DAL_Player_Output_t));
	memset(&DAL_Input.Source.http.URL,0,255);

//	DAL_Input.Source.udp.UDPTYPE=1;
	i=iUrl_num;
	strncpy(DAL_Input.Source.http.URL, url[i], strlen(url[i]));

//	strncpy(DAL_Input.Source.http.URL, url, strlen(url));
	set_mpts_audio_pid(69,0x03);
	set_mpts_video_pid(71,0x1b);

	DAL_Input.SourceType =SourceType_P2S;// SourceType_P2S;//SourceType_UDP;//SourceType_P2V;
//	iUrl_num+=1;
	if(iUrl_num==5)
		iUrl_num=0;
	Error = DAL_Stream_Restart(player, &DAL_Input, &output);
	if( Error != 0)
	{
		DebugMessage("[DAL][dal_http_restart]:DAL_Stream_Start failed with %d\n", Error);
		return Error;
	}
#else

memset(&output,0,sizeof(DAL_Player_Output_t));
memset(&DAL_Input.Source.http.URL,0,255);
i=iUrl_num;

//strncpy(DAL_Input.Source.http.URL, url[i], strlen(url[i]));
DAL_Input.SourceType =SourceType_TIMESHIFT;
//DAL_Input.param=10;


Error = DAL_Stream_Seek_next(player,&DAL_Input,&output);
	
//Error = DAL_Stream_resume(player);
if( Error != 0)
{
	DebugMessage("[DAL][dal_http_start2]:DAL_Stream_resume failed with %d\n", Error);
	return Error;
}

//hisi_test_ts_rate();

#endif
	return 0;
}

void Player_DalEventCallback( U32 event, void* param)
{
	dal_evt_param_t*		p_dal_evt_param = (dal_evt_param_t*)param;
		
	switch((dal_play_event_t)event)
	{
		//case DAL_PLAY_EVENT_PLAYREC_START_OK:
		//{
		//}
		//break;

		case DAL_PLAY_EVENT_PLAY_START: 
		break;
		case DAL_PLAY_EVENT_NEW_PROGRESS:
		break;
		
		case DAL_PLAY_EVENT_PLAY_END: 
		{
			/*hls 节目播完*/
		}
		break;
		case DAL_PLAY_EVENT_FLVLIST_END:
		{
		}
		break;
	
		case DAL_PLAY_EVENT_BUFFERING:
		{
			/*数据 缓冲提示*/
			DebugMessage("[Player_DalEventCallback][------------------->]:DAL_PLAY_EVENT_BUFFERING");
		}
		break;
		case DAL_PLAY_EVENT_PLAY_SEEK_ACTION_START:		
		{
			/*  seek 动作中，相当于缓冲*/
		}
		break;
		case DAL_PLAY_EVENT_DATA_INTERRUPT:
		{
			/*网络接收数据timeout*/
			
			DebugMessage("[Player_DalEventCallback][------------------->]:DAL_PLAY_EVENT_DATA_INTERRUPT");
		}
		break;
		case DAL_PLAY_EVENT_DATA_RESUME:
		{
			/*网络接收数据timeout后恢复*/
			
			DebugMessage("[Player_DalEventCallback][------------------->]:DAL_PLAY_EVENT_DATA_RESUME");
		}
		break;
		case DAL_PLAY_EVENT_RESTART_SAME_URL:
		{
			/*新的换台会显示新的banner，但是由于是同一个台，
			没有真正换台，为防止banner 一直不消失，
			在此事情里做一次清除banner 的操作*/
		}
		break;
		case DAL_PLAY_EVENT_PLAY_WILL_STOP:
		{
			/*播放将要结束*/
			
			DebugMessage("[Player_DalEventCallback][------------------->]:DAL_PLAY_EVENT_PLAY_WILL_STOP");
			/* 在即将关闭播放之前，清除可能还存在于播放界面的提示框*/
		}
		break;
		case DAL_PLAY_EVENT_PLAY_PAUSE_OK:
		{
			DebugMessage("[Player_DalEventCallback][------------------->]:DAL_PLAY_EVENT_PLAY_PAUSE_OK");
			/*pause ok 不是实际停止状态，resume状态走buffering*/
		}
		break;
		case DAL_PLAY_EVENT_PLAY_RESUME_OK:
		{
		}
		break;
		case DAL_PLAY_EVENT_PLAY_SEEK_OK:
		case DAL_PLAY_EVENT_PLAY_SEEK_ACTION_DONE:		// seek 动作结束
		{
		}
		break;
		case DAL_PLAY_EVENT_FIRST_FRAME:
		{
			/*视频开始播放第一帧*/
			DebugMessage("[Player_DalEventCallback][------------------->]:DAL_PLAY_EVENT_FIRST_FRAME");
		}
		break;
		case DAL_PLAY_EVENT_PLAY_STOP_OK:
		{
			
			DebugMessage("[Player_DalEventCallback][------------------->]:DAL_PLAY_EVENT_PLAY_STOP_OK");
			/*播放结束成功*/
		}
		break;

		case DAL_PLAY_EVENT_MMS_PLAY_START_FAILED:
		{
		}
		break;
		case DAL_PLAY_EVENT_PLAY_START_FAILED:
		{
			DebugMessage("[Player_DalEventCallback][------------------->]:DAL_PLAY_EVENT_PLAY_START_FAILED");
			dal_set_state(1,PLAYER_IDLE);
		
			dal_http_restart();
		}
		break;
		case DAL_PLAY_EVENT_M3U8_DOWNSTART_FAILED:
		{
			DebugMessage("[Player_DalEventCallback][------------------->]:DAL_PLAY_EVENT_M3U8_DOWNSTART_FAILED");
			
			dal_http_restart();
		}
		break;
		default: 
		break;
	}
}
static U32 g_videoPID=0,g_videoFMT=0;
static U32 g_audioPID=0,g_audioFMT=0;
void set_mpts_audio_pid(U32 value,U32 format)
{
	g_audioPID=value;
	g_audioFMT=format;
}

U32 get_mpts_audio_pid(void)
{
	return g_audioPID;
	
}
U32 get_mpts_audio_FMT(void)
{
	return g_audioFMT;
}

void set_mpts_video_pid(U32 value,U32 format)
{
	g_videoPID = value;
	g_videoFMT = format;
}

U32 get_mpts_video_pid(void)
{
	return g_videoPID;		
}
U32 get_mpts_video_FMT(void)
{	
	return g_videoFMT;
}


