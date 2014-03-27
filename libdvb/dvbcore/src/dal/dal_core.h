/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : 
* Description : 
* Author : zyliu
* History :
*	2011/4/11 : Initial Version
******************************************************************************/
#ifndef __DAL_CORE_H
#define __DAL_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************/
/*              Includes											*/
/*******************************************************/

/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/

/*******************************************************/
/*              Exported Types			                */
/*******************************************************/
typedef enum dal_core_event_e
{
	/* dal core start p2s event*/
	DAL_CORE_EVENT_PLAY_ALREADY_START,				// core is already started, should stop it before a new start 
	DAL_CORE_EVENT_DL_BUF_ALLOC_FAILED,				// core alloc download buffer failed
	DAL_CORE_EVENT_DL_START_FAILED,
	DAL_CORE_EVENT_PIDS_PARSE_START_FAILED,	
	DAL_CORE_EVENT_STATUS_WIAT_PIDS,				//	p2s data download and parse started, wait pids to play start
	DAL_CORE_EVENT_NEW_PIDS_BAD_STATUS,
	DAL_CORE_EVENT_PIDS_CHANGED_BAD_STATUS,
	DAL_CORE_EVENT_PLAY_PIDS_FAILED,
	DAL_CORE_EVENT_PLAY_INJECT_FAILED,
	DAL_CORE_EVENT_PLAY_PIDS_OK,
	DAL_CORE_EVENT_PID_CHANGED_FAILED,
	DAL_CORE_EVENT_PID_CHANGED_OK,
	DAL_CORE_EVENT_DATA_INTERRUPT,
	DAL_CORE_EVENT_DATA_RESUME,
	

	/* dal core start flvlist event*/
	DAL_CORE_EVENT_PARSE_FLVLIST_START_FAILED,
	DAL_CORE_EVENT_STATUS_WIAT_FLVLIST_PARSE,		// flvlist play in wait flvlist parse status
	DAL_CORE_EVENT_FLV_PARSE_OK_BAD_STATUS,
	DAL_CORE_EVENT_FLVLIST_PARSE_FAILED,
	DAL_CORE_EVENT_GET_LIST_URL_FAILED,
	DAL_CORE_EVENT_PLAY_FLV_FAILED,
	DAL_CORE_EVENT_PLAY_FLV_OK,

	/* dal core start mms event*/
	DAL_CORE_EVENT_MMS_CONNECTING,
	DAL_CORE_EVENT_MMS_CONNECT_FAILED,
	DAL_CORE_EVENT_PLAY_MMS_BUFFERING,
	DAL_CORE_EVENT_PLAY_MMS_FAILED,
	DAL_CORE_EVENT_PLAY_MMS_OK,
	
	/* dal core restart p2s event*/
	DAL_CORE_EVENT_RESTART_BAD_STATUS,
	DAL_CORE_EVENT_RESTART_SAME_URL,

	/* dal core stop p2s event*/
	DAL_CORE_EVENT_P2S_STOP_BAD_STATUS,
	DAL_CORE_EVENT_FLV_STOP_BAD_STATUS,
	DAL_CORE_EVENT_PLAY_WILL_STOP,
	DAL_CORE_EVENT_CONNECT_ERROR,
	DAL_CORE_EVENT_BEFORE_PLAYREC_STOP,
	DAL_CORE_EVENT_STOP_OK,

	/* dal core stop mms event*/
	DAL_CORE_EVENT_MMS_CLOSE_FAILED,

	/* dal core stop hls event*/
	DAL_CORE_EVENT_HLS_STOP_BAD_STATUS,
	DAL_CORE_EVENT_M3U8_START_BAD_STATUS,	
	DAL_CORE_EVENT_M3U8_RESTART_BAD_STATUS,
	DAL_CORE_EVENT_M3U8_DOWNLOAD_BAD_STATUS,
	
	/* dal core  timeshift event*/
	DAL_CORE_EVENT_TIMESHIFT_START_BAD_STATUS,
	DAL_CORE_EVENT_TIMESHIFT_STOP_BAD_STATUS,


	/* dal core pause event*/
	DAL_CORE_EVENT_PAUSE_FAILED,
	DAL_CORE_EVENT_PAUSE_OK,

	/* dal core resume event*/
	DAL_CORE_EVENT_RESUME_OK,
	
	/* dal core seek event*/
	DAL_CORE_EVENT_SEEK_OK,
	DAL_CORE_EVENT_SEEK_PAUSE_FAILED,				// when seek in pause status ,  pause after seek failed
	DAL_CORE_EVENT_SEEK_FAILED,
	DAL_CORE_EVENT_SEEK_ALREADY_FIRST,
	DAL_CORE_EVENT_SEEK_ALREADY_LAST,
	DAL_CORE_EVENT_SEEK_ACTION_START,
	DAL_CORE_EVENT_SEEK_ACTION_DONE,

	/* first frame*/
	DAL_CORE_EVENT_FRIST_FRAME_DISPLAY,
	
	/*dal p2v resume event*/
	DAL_CORE_EVENT_P2V_RESUME_DL_BUF_ALLOC_FAILED,
	DAL_CORE_EVENT_P2V_RESUME_DL_START_FAILED,
	DAL_CORE_EVENT_P2V_RESUME_PIDS_PARSE_START_FAILED,
	DAL_CORE_EVENT_P2V_SEEK_FILE_TITLE,
	DAL_CORE_EVENT_P2V_SEEK_FILE_END,

	/*dal hls resume event*/
	DAL_CORE_EVENT_HLS_PAUSE_RESUME_NOT_SUPPORT,
	DAL_CORE_EVENT_HLS_RESUME_DL_BUF_ALLOC_FAILED,
	DAL_CORE_EVENT_HLS_RESUME_DL_START_FAILED,
	DAL_CORE_EVENT_HLS_RESUME_PIDS_PARSE_START_FAILED,
	DAL_CORE_EVENT_HLS_SEEK_FILE_TITLE,
	DAL_CORE_EVENT_HLS_SEEK_FILE_END,
	DAL_CORE_EVENT_PLAYEND,

	/*dal timeshift resume event*/
	DAL_CORE_EVENT_TIMESHIFT_RESUME_DL_BUF_ALLOC_FAILED,
	DAL_CORE_EVENT_TIMESHIFT_RESUME_DL_START_FAILED,
	DAL_CORE_EVENT_TIMESHIFT_RESUME_PIDS_PARSE_START_FAILED,
	DAL_CORE_EVENT_TIMESHIFT_SEEK_FILE_TITLE,
	DAL_CORE_EVENT_TIMESHIFT_SEEK_FILE_END,
	DAL_CORE_EVENT_TIMESHIFT_PLAYEND,
	
}dal_core_event_t;

typedef struct Core_Param_s
{
	DAL_Event_Notify_t		core_event_callback;
}Core_Param_t;

/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/

int DAL_CoreInit(Core_Param_t* param);
int DAL_CoreRelease(void);

int DAL_CoreTaskCreate(void);
int DAL_CoreTaskDelete(void);

DAL_QueueID_t DAL_GetCoreQueueID(void);



int DAL_P2S_PlayStart( DAL_Player_Input_t* input, DAL_Player_Output_t* output);
int DAL_P2S_PlayRestart( DAL_Player_Input_t* input, DAL_Player_Output_t* output);
int DAL_P2S_PlayStop( void);
int DAL_P2S_PlayIsStart(void);
#ifdef FLV
int DAL_FlvList_PlayStart( DAL_Player_Input_t* input, DAL_Player_Output_t* output);
int DAL_FlvList_PlayStop( void);
int DAL_FlvList_PlayPause( void);
int DAL_FlvList_PlayResume( void);
int DAL_FlvList_PlayNext( void);
int DAL_FlvList_PlaySeek( U32 seconds);
int DAL_FlvList_PlayGetPlayTime( unsigned int* total_time, unsigned int* current_time);
int DAL_FlvList_PlaySeekPrev(void);
int DAL_FlvList_PlaySeekNext(void);	
int DAL_FlvList_PlayPauseAndResume(void);
#endif
#ifdef MMS
int DAL_MMS_PlayStart( DAL_Player_Input_t* input, DAL_Player_Output_t* output);
int DAL_MMS_PlayRestart( DAL_Player_Input_t* input, DAL_Player_Output_t* output);
int DAL_MMS_PlayStop( void);
int DAL_MMS_PlayIsStart(void);
#endif


int DAL_DVB_CheckFristFrame(void);

int DAL_Core_P2S_Recovery(void);

BOOL DAL_core_is_stoped(void);


char* DAL_CoreEventString(dal_core_event_t event);
U32 get_core_controlstatus(void);
void dal_core_check_frame(void);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __XXXXXXX_H */
/* End of __DAL_CORE_H.h  --------------------------------------------------------- */


