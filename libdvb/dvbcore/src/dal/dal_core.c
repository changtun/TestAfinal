

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl.h>
#include <types.h>
#include <easy.h>


#include "osapi.h"
#include "pvddefs.h"
#include "drv_avctrl.h"
#include "pbitrace.h"

#ifdef USE_ST_PLAYREC
#include "playrec.h"
#include "drv_container.h"

#endif

#include "ring_buffer.h"
#include "pvlist.h"

#include "dal_wrap.h"
#include "dal_common.h"
#include "dal.h"
#include "dal_message.h"
#include "dal_hw_player.h"

#include "dal_download.h"
#include "dal_parse.h"
#include "dal_inject.h"
#include "dal_timeshift.h"
#include "dal_core.h"
#include "dal_mms.h"
#include "dal_hls.h"

#include "flv_parse.h"
#include "flv_list_play.h"

#include "am_dsc.h"

#define DAL_CORE_TASK_STACK_SIZE		1024*64
#define DAL_CORE_TASK_PRIOITY			5
#define DAL_CORE_QUEUE_DEPTH			20
#define DAL_CORE_QUEUE_DATA_SIZE		DAL_MESSAGE_SIZE

#define P2S_SWITCH_PAUSE_VIDEO			//  p2s  切台定屏
//#define CONVERT_FLVLIST_URL

#define FLV_MAX_RETRY_TIMES				3

typedef enum CoreStatus_e
{
	CORE_IDEL,							// 模块未初始化
	CORE_STREAM_START,
	CORE_STREAM_WAIT_PID,
	CORE_STREAM_WAIT_FLV_PARSE,
	CORE_STREAM_PAUSE,
	CORE_STREAM_STOP,					// 没有正在进行的播放
	CORE_STREAM_WAIT_FLVPLAY,
}CoreStatus_t;

typedef enum FristFrameAction_e
{
	FF_ACTION_NONE,
	FF_ACTION_CLEAR_NOTIFY,			// 清除之前的buffering 提示框
	FF_ACTION_SET_PAUSE,			// pause后换台情况，换台后执行暂停动作
	FF_ACTION_CLEAR_BANNER,
	FF_ACTION_DISPLAY_PROGRESS,		// 显示进度条
}FristFrameAction_t;

typedef struct DAL_CoreControl_s
{
	CoreStatus_t			status;
	DAL_TaskID_t			taskID;
	DAL_QueueID_t			queueID;

	DAL_Player_Input_t		input;
	DAL_Player_Output_t		output;

	BOOL					playrec_start_flag;			// 标识playrec 是否已经开启
	BOOL					first_frame_display;		// 标志视频播放的第一帧是否呈现
	BOOL					checking_first_frame;		// 标志dal_core 是否有正在执行 check_frame 的动作
	FristFrameAction_t		first_frame_action;			// 定义发现第一帧呈现后的动作

	DAL_TaskID_t			hdd_ts_read_taskID;
	DAL_Event_Notify_t		core_event_callback;
	BOOL					recovery_flag;

	BOOL					flvlist_parse_ok_flag;

	int						flvplay_try_times;		// flv播放时，打开网络文件失败后，重新尝试次数
	BOOL					flvlist_in_seek_proc;	// core 是否处于seek 的proc中
	
	BOOL					p2v_in_seek_proc;
	
	BOOL					HLS_in_seek_proc;
	BOOL					timeshift_in_seek_proc;
}DAL_CoreControl_t;

static DAL_CoreControl_t		g_CoreControl;
C8 hls_http[256];
static U32 p2v_movie_playtime = 0;
char p2vheader[255]="data/data/com.pbi.dvb/test_header.txt";
U32 p2v_movie_length=0;
U32 p2v_movie_duration=0;
U8 resume_flag=0;/*1,resume flag*/
extern DAL_DownloadControl_t	g_DownloadControl;
U8 multicast_flag=0;

/*add by zyliu to count video frame displayed*/
void reset_video_frame_display_count(void);
int	 get_video_frame_display_count(void);
void new_video_frame_display(void);
/*add by zyliu 2011-7-28}}}*/

/*{{{ add by zyliu 2011-8-4  设置ststream http 连接超时标志位*/
int playrec_set_http_timeout_flag(BOOL flag);
BOOL playrec_get_http_timeout_flag(void);
/*add by zyliu 2011-8-4}}}*/

int PLAYREC_SET_PLAY_FLVLIST(BOOL value);
int PLAYREC_SET_PLAY_MMS(BOOL value);


static void dal_flvlist_parse_callback_func( flvlist_parse_result_t result);
static void dal_mms_download_event_func(mms_event_t event, void* param);
void dal_core_event_notify( dal_core_event_t event, void* param);

static void* dal_core_task(void* param);

void dal_stream_start_proc(DAL_Message_t* pMsg);
void dal_stream_restart_proc(DAL_Message_t* pMsg);
void dal_stream_stop_proc(DAL_Message_t* pMsg);
void dal_stream_pause_proc(DAL_Message_t* pMsg);
void dal_stream_resume_proc(DAL_Message_t* pMsg);
void dal_stream_setspeed_proc(DAL_Message_t* pMsg);
void dal_stream_seek_proc(DAL_Message_t* pMsg);
void dal_stream_seek_prev_proc(DAL_Message_t* pMsg);
void dal_stream_seek_next_proc(DAL_Message_t* pMsg);
void dal_stream_play_next_proc(DAL_Message_t* pMsg);
void dal_stream_getinfo_proc(DAL_Message_t* pMsg);
void dal_new_pids_proc(DAL_Message_t* pMsg);
void dal_pids_changed_proc(DAL_Message_t* pMsg);
void dal_stream_data_timeout_proc(DAL_Message_t* pMsg);
void dal_stream_data_resume_proc(DAL_Message_t* pMsg);
void dal_stream_first_frame_display_proc(DAL_Message_t* pMsg);
void dal_stream_check_first_frame_proc(DAL_Message_t* pMsg);
void dal_stream_hdd_ts_start(DAL_Message_t* pMsg);
#ifdef FLV
void dal_stream_flvlist_parse_ok_proc(DAL_Message_t* pMsg);
void dal_stream_flvlist_parse_failed_proc(DAL_Message_t* pMsg);
void dal_stream_flvlist_parse_later_ok(DAL_Message_t* pMsg);
void dal_stream_flvlist_seek_action_done_proc(DAL_Message_t* pMsg);
void dal_stream_wait_retry_flvplay_proc(DAL_Message_t* pMsg);
#endif

void dal_stream_dvb_check_frist_frame_proc(DAL_Message_t* pMsg);
void dal_stream_recovery_proc(DAL_Message_t* pMsg);
int Set_DSC_key(void);
int del_DSC_key(void);



int DAL_CoreInit( Core_Param_t* param)
{
	DAL_CoreControl_t*		p_control = &g_CoreControl;
	int						iret = 0;
#ifdef MMS
	mms_init_param_t		mms_init_param;
#endif
	memset(p_control,0,sizeof(DAL_CoreControl_t));

	/* core event regisiter*/
	p_control->core_event_callback = param->core_event_callback;
#ifdef FLV	
	iret = flv_list_create_list();
	if( iret != 0)
	{
		DAL_ERROR(("flv_list_create_list failed\n"));
		return -1;
	}
	iret = flv_list_create_list_1();
	if( iret != 0)
	{
		DAL_ERROR(("flv_list_create_list_1 failed\n"));
		return -1;
	}
#endif
	iret = DAL_MessageInit();
	if( iret != 0)
	{
		//DAL_ERROR(("DAL_MessageInit failed\n"));
		DebugMessage("DAL_MessageInit failed\n");
		return -1;
	}
	iret = DAL_CoreTaskCreate();
	if( iret != 0)
	{
		//DAL_ERROR(("DAL_CoreTaskCreate failed\n"));
		DebugMessage("DAL_CoreTaskCreate failed\n");
		return -1;
	}
	iret = DAL_DownloadTaskCreate();
	if( iret != 0)
	{
		//DAL_ERROR(("DAL_DownloadTaskCreate failed\n"));
		DebugMessage("DAL_DownloadTaskCreate failed\n");
		return -1;
	}
	iret = DAL_ParseTaskCreate();
	if( iret != 0)
	{
		//DAL_ERROR(("DAL_ParseTaskCreate failed\n"));
		DebugMessage("DAL_ParseTaskCreate failed\n");
		return -1;
	}	
	iret = DAL_InjectTaskCreate();
	if( iret != 0)
	{
		//DAL_ERROR(("DAL_InjectTaskCreate failed\n"));
		DebugMessage("DAL_InjectTaskCreate failed\n");
		return -1;
	}
#ifdef MMS
	mms_init_param.dl_notify = dal_mms_download_event_func;
	mms_init_param.auto_timeshift = 0;
	iret = dal_mms_init(&mms_init_param);
	if( iret != 0)
	{
		DAL_ERROR(("dal_mms_init failed\n"));
		return -1;
	}
#endif
	iret = DAL_M3u8TaskCreate();
	if( iret != 0 )
	{
		DebugMessage("DAL_M3u8TaskCreate failed\n");
		return -1;
	}
	
	//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "DAL_CoreInit OK");	
	DebugMessage( "DAL_CoreInit OK");
	return 0;
}

int DAL_CoreRelease(void)
{
	DAL_ERROR(("Not support yet\n"));
	return 0;
}

int DAL_CoreTaskCreate(void)
{
	DAL_CoreControl_t*		p_control = &g_CoreControl;
	int						iret = 0;

	iret = DAL_MessageQueueCreate( &(p_control->queueID), "DAL_CoreQueue", DAL_CORE_QUEUE_DEPTH, DAL_CORE_QUEUE_DATA_SIZE, 0);
	if( iret != 0)
	{
		//DAL_ERROR(("DAL_MessageQueueCreate failed\n"));
		DebugMessage("DAL_MessageQueueCreate failed");
		return -1;
	}
	iret = DAL_TaskCreate( &(p_control->taskID), "DAL_CoreTask", dal_core_task, NULL, NULL, DAL_CORE_TASK_STACK_SIZE, DAL_CORE_TASK_PRIOITY, 0);
	if( iret != 0)
	{
		//DAL_ERROR(("DAL_TaskCreate failed\n"));
		DebugMessage("DAL_TaskCreate failed\n");
		DAL_MessageQueueDelete(p_control->queueID);
		return -1;
	}
	p_control->status = CORE_STREAM_STOP;
	return 0;
}



int DAL_CoreTaskDelete(void)
{
	DAL_CoreControl_t*		p_control = &g_CoreControl;
	
	DAL_TaskDelete( p_control->taskID);
	DAL_MessageQueueDelete( p_control->queueID);
	p_control->status = CORE_IDEL;
	return 0;
}

DAL_QueueID_t DAL_GetCoreQueueID(void)
{
	DAL_CoreControl_t*		p_control = &g_CoreControl;

	if( p_control->status == CORE_IDEL)
	{
		//DAL_ERROR(("CORE_IDEL\ns"));
		DebugMessage("CORE_IDEL\ns");
		return -1;
	}
	return p_control->queueID;
}



int DAL_P2S_PlayStart( DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg){
		pMsg->MsgType = DALMSG_STREAM_START;
		memcpy( &(pMsg->MsgData.StreamParam.input), input, sizeof(DAL_Player_Input_t));
		memcpy( &(pMsg->MsgData.StreamParam.output), output, sizeof(DAL_Player_Output_t));
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}else{
		//DAL_ERROR(("DAL_MessageAlloc failed\n"));
		DebugMessage("DAL_MessageAlloc failed\n");
	}
	return 0;
}


int DAL_P2S_PlayRestart( DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg){
		pMsg->MsgType = DALMSG_STREAM_RESTART;
		memcpy( &(pMsg->MsgData.StreamParam.input), input, sizeof(DAL_Player_Input_t));
		memcpy( &(pMsg->MsgData.StreamParam.output), output, sizeof(DAL_Player_Output_t));
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}else{
		//DAL_ERROR(("DAL_MessageAlloc failed\n"));
		DebugMessage("DAL_MessageAlloc failed\n");
	}
	return 0;
}

int DAL_P2S_PlayStop( void)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg){
		pMsg->MsgType = DALMSG_STREAM_STOP;
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}else{
		//DAL_ERROR(("DAL_MessageAlloc failed\n"));
		DebugMessage("DAL_MessageAlloc failed\n");
	}
	return 0;
}

int DAL_P2S_PlayIsStart(void)
{
	DAL_CoreControl_t*		p_control = &g_CoreControl;

	//printf("---------------------------------------------DAL_P2S_PlayIsStart entry\n");
	DebugMessage("[DAL]DAL_P2S_PlayIsStart ->DAL_P2S_PlayIsStart entry");

	if( (p_control->input.SourceType != SourceType_P2S)
		&&(p_control->input.SourceType != SourceType_P2V))
	{
		//printf("DAL_P2S_PlayIsStart SourceType: %d\n", p_control->input.SourceType);
		DebugMessage("[DAL]DAL_P2S_PlayIsStart ->DAL_P2S_PlayIsStart SourceType: %d\n", p_control->input.SourceType);
		return 0;
	}
	if( p_control->status == CORE_STREAM_START)
	{
		//printf("DAL_P2S_PlayIsStart state: %d\n", p_control->status);	
		DebugMessage("[DAL]DAL_P2S_PlayIsStart ->DAL_P2S_PlayIsStart state: %d\n", p_control->status);
		return 1;
	}
//	printf("DAL_P2S_PlayIsStart state: %d\n", p_control->status);
	DebugMessage("[DAL]DAL_P2S_PlayIsStart ->DAL_P2S_PlayIsStart state: %d\n", p_control->status);
	return 0;
}

//{{{add by LYN, for UDP multicast function,2011-08-22
//the four function is same with the P2S start
int DAL_Multicast_PlayStart( DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{
    DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg){
		pMsg->MsgType = DALMSG_STREAM_START;
		multicast_flag=1;
		memcpy( &(pMsg->MsgData.StreamParam.input), input, sizeof(DAL_Player_Input_t));
		DebugMessage("[DAL_Multicast_PlayStart] %s",pMsg->MsgData.StreamParam.input.Source.udp.URL);
		DebugMessage("[DAL_Multicast_PlayStart] %s",input->Source.udp.URL);
		memcpy( &(pMsg->MsgData.StreamParam.output), output, sizeof(DAL_Player_Output_t));
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}else{
		DebugMessage("DAL_MessageAlloc failed\n");
	}
	return 0;
}

int DAL_Multicast_PlayRestart( DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{   
    DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg){
		pMsg->MsgType = DALMSG_STREAM_RESTART;
		memcpy( &(pMsg->MsgData.StreamParam.input), input, sizeof(DAL_Player_Input_t));
		memcpy( &(pMsg->MsgData.StreamParam.output), output, sizeof(DAL_Player_Output_t));
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}else{
		DAL_ERROR(("DAL_MessageAlloc failed\n"));
	}
	return 0;
}

int DAL_Multicast_PlayStop( void)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg){
		pMsg->MsgType = DALMSG_STREAM_STOP;
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
		multicast_flag=0;
	}else{
		DAL_ERROR(("DAL_MessageAlloc failed\n"));
	}
	return 0;
}

int DAL_Multicast_PlayIsStart(void)
{
	DAL_CoreControl_t*		p_control = &g_CoreControl;

	printf("---------------------------------------------DAL_Multicast_PlayIsStart entry\n");

	if( p_control->input.SourceType != SourceType_UDP)
	{
		printf("DAL_Multicast_PlayIsStart SourceType: %d\n", p_control->input.SourceType);
		return 0;
	}
	if(CORE_STREAM_START == p_control->status)
	{
		printf("DAL_Multicast_PlayIsStart state: %d\n", p_control->status);
		return 1;
	}
	printf("DAL_Multicast_PlayIsStart state: %d\n", p_control->status);
	return 0;
}
//}}}add by LYN, for UDP multicast function,2011-08-22

#ifdef MMS
int DAL_MMS_PlayStart( DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{
	DAL_Message_t*		pMsg = NULL;
	
	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg){
		pMsg->MsgType = DALMSG_STREAM_START;
		memcpy( &(pMsg->MsgData.StreamParam.input), input, sizeof(DAL_Player_Input_t));
		memcpy( &(pMsg->MsgData.StreamParam.output), output, sizeof(DAL_Player_Output_t));
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}else{
		DAL_ERROR(("DAL_MessageAlloc failed\n"));
	}
	return 0;
}

int DAL_MMS_PlayRestart( DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{
	DAL_Message_t*		pMsg = NULL;
	
	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg){
		pMsg->MsgType = DALMSG_STREAM_RESTART;
		memcpy( &(pMsg->MsgData.StreamParam.input), input, sizeof(DAL_Player_Input_t));
		memcpy( &(pMsg->MsgData.StreamParam.output), output, sizeof(DAL_Player_Output_t));
		dal_mms_interrupt();
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}else{
		DAL_ERROR(("DAL_MessageAlloc failed\n"));
	}
	return 0;
}

int DAL_MMS_PlayStop( void)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg){
		pMsg->MsgType = DALMSG_STREAM_STOP;
		dal_mms_interrupt();
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}else{
		DAL_ERROR(("DAL_MessageAlloc failed\n"));
	}
	return 0;
}

int DAL_MMS_PlayIsStart(void)
{
	return 0;
}
#endif

#ifdef FLV
int DAL_FlvList_PlayStart( DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_STREAM_START;
	memcpy( &(pMsg->MsgData.StreamParam.input), input, sizeof(DAL_Player_Input_t));
	memcpy( &(pMsg->MsgData.StreamParam.output), output, sizeof(DAL_Player_Output_t));
	DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	return 0;
}

int DAL_FlvList_PlayStop( void)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_STREAM_STOP;
	DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	set_ststream_http_break_flag(1);	// to break ststream http connect
	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^DAL_FlvList_PlayStop send msg ok!\n");
	return 0;
}

int DAL_FlvList_PlayPause( void)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_STREAM_PAUSE;
	DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	return 0;	
}

int DAL_FlvList_PlayResume( void)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_STREAM_RESUME;
	DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	return 0;
}

int DAL_FlvList_PlayNext( void)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_STREAM_NEXT;
	DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	return 0;
}

int DAL_FlvList_PlaySeek( U32 seconds)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_STREAM_SEEK;
	pMsg->MsgData.Common.param1 = seconds/1000;
	DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	return 0;
}

int DAL_FlvList_PlaySeekPrev(void)
{
	DAL_CoreControl_t*	p_control = &g_CoreControl;
	flv_play_list_t*	p_flv_list = flv_list_get_list();
	DAL_Message_t*		pMsg = NULL;
	
	if( CORE_STREAM_WAIT_FLVPLAY == p_control->status ){
		/* DAL_Core 正在尝试重新播放*/
		SYS_DEBUG(SD_DEBUG,MODULE_DAL_CORE,"********************** DAL_Core is in flv retry playrec start, do not send msg to seek prev\n");
		return 0;
	}
	if( TRUE == p_control->flvlist_in_seek_proc){
		/* DAL_Core 正在处理SEEK */
		SYS_DEBUG(SD_DEBUG,MODULE_DAL_CORE,"********************** DAL_Core is in seek proc, do not send msg to seek prev\n");
		return 0;
	}
	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_STREAM_SEEK_PREV;
	DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	if( 0 != p_flv_list->cur_play){
		/* 如果当前播放不是第一段，这里设置标志，中断底层的http 连接 */
		set_ststream_http_break_flag(1);	// to break ststream http connect
		SYS_DEBUG(SD_DEBUG,MODULE_DAL_CORE,"**********************  set_ststream_http_break_flag 1 cur_play: %d", p_flv_list->cur_play);
	}
	return 0;
}

int DAL_FlvList_PlaySeekNext(void)
{
	DAL_CoreControl_t*	p_control = &g_CoreControl;
	flv_play_list_t*	p_flv_list = flv_list_get_list();
	DAL_Message_t*		pMsg = NULL;

	if( CORE_STREAM_WAIT_FLVPLAY == p_control->status ){
		/* DAL_Core 正在尝试重新播放*/
		SYS_DEBUG(SD_DEBUG,MODULE_DAL_CORE,"********************** DAL_Core is in flv retry playrec start, do not send msg to seek prev\n");
		return 0;
	}
	if( TRUE == p_control->flvlist_in_seek_proc){
		/* DAL_Core 正在处理SEEK */
		SYS_DEBUG(SD_DEBUG,MODULE_DAL_CORE,"********************** DAL_Core is in seek proc, do not send msg to seek prev\n");
		return 0;
	}

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_STREAM_SEEK_NEXT;
	DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	if( p_flv_list->cur_play != p_flv_list->num - 1){
		/* 如果当前播放不是最后一段，这里设置标志，中断底层的http 连接 */
		set_ststream_http_break_flag(1);	// to break ststream http connect
		SYS_DEBUG(SD_DEBUG,MODULE_DAL_CORE,"**********************  set_ststream_http_break_flag 1 num: %d cur_play: %d", p_flv_list->num, p_flv_list->cur_play);
	}
	return 0;
}

int DAL_FlvList_PlayPauseAndResume(void)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;

	if( p_control->status == CORE_STREAM_START)
	{
		return DAL_FlvList_PlayPause();
	}
	else if( p_control->status == CORE_STREAM_PAUSE)
	{
		return DAL_FlvList_PlayResume();
	}
	return 0;
}

int DAL_FlvList_PlayGetPlayTime( unsigned int* total_time, unsigned int* current_time)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	flv_play_list_t*			p_flv_list = NULL;
	ST_ErrorCode_t				err = ST_NO_ERROR;
	U32							BeginOfFileTimeInMs;
	U32							CurrentPlayTimeInMs;
	U32							EndOfFileTimeInMs;
	
	if( p_control->status != CORE_STREAM_START &&  p_control->status != CORE_STREAM_PAUSE)
	{
		DAL_ERROR(("NOT CORE_STREAM_START or CORE_STREAM_PAUSE\n"));
		*total_time = 0;
		*current_time = 0;
		return -1;
	}
	if( FALSE == p_control->flvlist_parse_ok_flag){
		DAL_ERROR(("not all flvlist parse ok, we can not unkonw the total time of video!!\n"));
		return -1;
	}

	err = PLAYREC_PlayGetTime( 0, &BeginOfFileTimeInMs, &CurrentPlayTimeInMs, &EndOfFileTimeInMs);
	if( err != ST_NO_ERROR )
	{
		DAL_ERROR(("PLAYREC_PlayGetTime failed\n"));
		*total_time = 0;
		*current_time = 0;
		return -1;
	}

	p_flv_list = flv_list_get_list();
	*total_time = p_flv_list->list[p_flv_list->num-1].time_end;
	if( p_flv_list->cur_play == 0)
	{
		*current_time = CurrentPlayTimeInMs/1000;
	}
	else
	{
		*current_time = CurrentPlayTimeInMs/1000 + p_flv_list->list[p_flv_list->cur_play-1].time_end;
	}
	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "total_time: %d  current_time: %d ", *total_time, *current_time);
	return 0;
}
#endif

/* 为了按时间点seek 功能添加，给ststream 完成seek 动作所需的下载之后调用
	通知上层seek 动作真正完成，开始播放seek 后的帧*/
void ststream_seek_done_notify( void)
{
/*
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	DAL_Message_t*				pMsg = NULL;

	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "-----------------------------flvlist_in_seek_proc: %d", p_control->flvlist_in_seek_proc);
	if(p_control->flvlist_in_seek_proc){
		pMsg = DAL_MessageAlloc();
		pMsg->MsgType = DALMSG_FLVLIST_SEEK_ACTION_DONE;
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}
*/
}

int DAL_DVB_CheckFristFrame(void)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_DVB_CHECK_FRIST_FRAME;
	DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "--------------------------------------------DAL_DVB_CheckFristFrame ");
	DebugMessage("[DAL]DAL_DVB_CheckFristFrame ->DAL_DVB_CheckFristFrame");

	return 0;
}

int DAL_Core_P2S_Recovery(void)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_P2S_RECOVERY;
	DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	DebugMessage("[DAL]DAL_Core_P2S_Recovery ->DAL_Core_P2S_Recovery");

	//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "--------------------------------------------DAL_Core_P2S_Recovery  ");
	return 0;
}

int DAL_core_is_stoped(void)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	return (p_control->status == CORE_STREAM_STOP) ? TRUE : FALSE;
}

char* DAL_CoreEventString(dal_core_event_t event)
{
#define E(e) case e: return #e
	switch(event)
	{
		/* dal core start p2s event*/
		E(DAL_CORE_EVENT_PLAY_ALREADY_START);
		E(DAL_CORE_EVENT_DL_BUF_ALLOC_FAILED);
		E(DAL_CORE_EVENT_DL_START_FAILED);
		E(DAL_CORE_EVENT_PIDS_PARSE_START_FAILED);
		E(DAL_CORE_EVENT_STATUS_WIAT_PIDS);
		E(DAL_CORE_EVENT_NEW_PIDS_BAD_STATUS);
		E(DAL_CORE_EVENT_PIDS_CHANGED_BAD_STATUS);
		E(DAL_CORE_EVENT_PLAY_PIDS_FAILED);
		E(DAL_CORE_EVENT_PLAY_INJECT_FAILED);
		E(DAL_CORE_EVENT_PLAY_PIDS_OK);
		E(DAL_CORE_EVENT_PID_CHANGED_FAILED);
		E(DAL_CORE_EVENT_PID_CHANGED_OK);
		E(DAL_CORE_EVENT_DATA_INTERRUPT);
		E(DAL_CORE_EVENT_DATA_RESUME);

		
#ifdef FLV		
		/* dal core start flvlist event*/
		E(DAL_CORE_EVENT_PARSE_FLVLIST_START_FAILED);
		E(DAL_CORE_EVENT_STATUS_WIAT_FLVLIST_PARSE);
		E(DAL_CORE_EVENT_FLV_PARSE_OK_BAD_STATUS);
		E(DAL_CORE_EVENT_FLVLIST_PARSE_FAILED);
		E(DAL_CORE_EVENT_GET_LIST_URL_FAILED);
		E(DAL_CORE_EVENT_PLAY_FLV_FAILED);
		E(DAL_CORE_EVENT_PLAY_FLV_OK);
#endif
#ifdef MMS		
		/* dal core start mms event*/
		E(DAL_CORE_EVENT_MMS_CONNECTING);
		E(DAL_CORE_EVENT_MMS_CONNECT_FAILED);
		E(DAL_CORE_EVENT_PLAY_MMS_BUFFERING);
		E(DAL_CORE_EVENT_PLAY_MMS_FAILED);
		E(DAL_CORE_EVENT_PLAY_MMS_OK);
#endif
		/* dal core restart p2s event*/
		E(DAL_CORE_EVENT_RESTART_BAD_STATUS);
		E(DAL_CORE_EVENT_RESTART_SAME_URL);		
	
		/* dal core stop p2s event*/
		E(DAL_CORE_EVENT_P2S_STOP_BAD_STATUS);
		E(DAL_CORE_EVENT_FLV_STOP_BAD_STATUS);
		E(DAL_CORE_EVENT_PLAY_WILL_STOP);
		E(DAL_CORE_EVENT_CONNECT_ERROR);
		E(DAL_CORE_EVENT_BEFORE_PLAYREC_STOP);
		E(DAL_CORE_EVENT_STOP_OK);

		/* dal core stop mms event*/
		E(DAL_CORE_EVENT_MMS_CLOSE_FAILED);

		/* dal core pause event*/
		E(DAL_CORE_EVENT_PAUSE_FAILED);
		E(DAL_CORE_EVENT_PAUSE_OK);
		
		/* dal core resume event*/
		E(DAL_CORE_EVENT_RESUME_OK);
		
		/* dal core seek event*/
		E(DAL_CORE_EVENT_SEEK_OK);
		E(DAL_CORE_EVENT_SEEK_PAUSE_FAILED);
		E(DAL_CORE_EVENT_SEEK_FAILED);
		E(DAL_CORE_EVENT_SEEK_ALREADY_FIRST);
		E(DAL_CORE_EVENT_SEEK_ALREADY_LAST);
		E(DAL_CORE_EVENT_SEEK_ACTION_START);
		E(DAL_CORE_EVENT_SEEK_ACTION_DONE);
		
		/*hls*/
		E(DAL_CORE_EVENT_HLS_STOP_BAD_STATUS);
		E(DAL_CORE_EVENT_M3U8_START_BAD_STATUS);
		E(DAL_CORE_EVENT_M3U8_RESTART_BAD_STATUS);
		E(DAL_CORE_EVENT_M3U8_DOWNLOAD_BAD_STATUS);
		
		/* first frame*/
		E(DAL_CORE_EVENT_FRIST_FRAME_DISPLAY);
		
		/*p2v resume*/
		E(DAL_CORE_EVENT_P2V_RESUME_DL_BUF_ALLOC_FAILED);
		E(DAL_CORE_EVENT_P2V_RESUME_DL_START_FAILED);
		E(DAL_CORE_EVENT_P2V_RESUME_PIDS_PARSE_START_FAILED);

		default:
			SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "event: %d\n", event);
			return "unknow event\n";
	}
}


#ifdef FLV
static void dal_flvlist_parse_callback_func( flvlist_parse_result_t result)
{
	DAL_Message_t*		pMsg = NULL;
	
	if( FLVLIST_PARSE_SUCCESS == result){
		pMsg = DAL_MessageAlloc();
		pMsg->MsgType = DALMSG_FLVLIST_PARSE_OK;
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);

	}
	else if( FLVLIST_PARSE_FAILED == result){
		pMsg = DAL_MessageAlloc();
		pMsg->MsgType = DALMSG_FLVLIST_PARSE_ERROR;
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}
	else if( FLVLIST_PARSE_FAILED_LATER_RETRY == result){
		pMsg = DAL_MessageAlloc();
		pMsg->MsgType = DALMSG_FLVLIST_PARSE_ERROR_LATER_RETRY;
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}
	else if(FLVLIST_PARSE_RETRY_SUCCESS == result){
		pMsg = DAL_MessageAlloc();
		pMsg->MsgType = DALMSG_FLVLIST_PARSE_LATER_OK;
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}
}
#endif

#ifdef MMS
static void dal_mms_download_event_func(mms_event_t event, void* param)
{
	dal_evt_param_t		dal_evt_param;
	
	switch(event){
	case MMS_DOWNLOAD_START:
		dal_core_event_notify(DAL_CORE_EVENT_PLAY_MMS_BUFFERING,&dal_evt_param);
	break;
	case MMS_FRIST_DATA_BLOCK_OK:
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "-----------------------------------------------  ^^^^^^^^^^^^^^^^^^^^ MMS_FRIST_DATA_BLOCK_OK");
		dal_core_event_notify(DAL_CORE_EVENT_FRIST_FRAME_DISPLAY,&dal_evt_param);
	break;
	case MMS_DATA_RECEIVE_TIMEOUT:
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "-----------------------------------------------  ^^^^^^^^^^^^^^^^^^^^ MMS_DATA_RECEIVE_TIMEOUT");
		dal_core_event_notify(DAL_CORE_EVENT_DATA_INTERRUPT,&dal_evt_param);
	break;
	case MMS_DATA_RECEIVE_RESUME:
		dal_core_event_notify(DAL_CORE_EVENT_DATA_RESUME,&dal_evt_param);
	break;	
	default:
	break;
	}
}
#endif
void dal_core_event_notify( dal_core_event_t event, void* param)
{
	DAL_CoreControl_t*	p_control = &g_CoreControl;

	if( NULL != p_control->core_event_callback)
	{
		p_control->core_event_callback(event,(void*)param);
	}
}

static char* dal_core_status_string(CoreStatus_t status)
{
#define E(e) case e: return #e
	switch(status)
	{
		E(CORE_IDEL);
		E(CORE_STREAM_START);
		E(CORE_STREAM_WAIT_PID);
		E(CORE_STREAM_WAIT_FLV_PARSE);
		E(CORE_STREAM_PAUSE);
		E(CORE_STREAM_STOP);
		E(CORE_STREAM_WAIT_FLVPLAY);
		default: return "unknow status"; 
	}
}

static void dal_core_set_status(CoreStatus_t s)
{
	DAL_CoreControl_t*	p_control = &g_CoreControl;
	p_control->status = s;
	DebugMessage("[DAL]dal_core_set_status -> dal_core status %s", dal_core_status_string(p_control->status));
	//SYS_DEBUG(SD_DEBUG,MODULE_DAL_CORE,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> dal_core status %s", dal_core_status_string(p_control->status));
}

static void ststream_firstframe_callback(void)
{
	DAL_CoreControl_t*	p_control = &g_CoreControl;
	dal_evt_param_t		dal_evt_param;
	int					iret = 0;

	//DAL_DEBUG(("	ststream_firstframe_callback	>>>>>>>\n"));
	DebugMessage("[DAL]ststream_firstframe_callback -> ststream_firstframe_callback ");

	if( FF_ACTION_SET_PAUSE == p_control->first_frame_action)
	{
	#ifdef FLV
		if( SourceType_FLV_LIST == p_control->input.SourceType)
		{
			iret = PLAYREC_PlayPause(0);
			if( iret != ST_NO_ERROR)
			{
				DAL_ERROR(("PLAYREC_PlayPause failed\n"));	
				dal_core_set_status(CORE_STREAM_START);
				dal_core_event_notify(DAL_CORE_EVENT_SEEK_PAUSE_FAILED,&dal_evt_param);
				return ;
			}
		}
	#endif	
	}
	else if( FF_ACTION_CLEAR_NOTIFY == p_control->first_frame_action){
		dal_core_event_notify(DAL_CORE_EVENT_FRIST_FRAME_DISPLAY,&dal_evt_param);	
	}	
	else if( FF_ACTION_CLEAR_BANNER == p_control->first_frame_action){
		dal_core_event_notify(DAL_CORE_EVENT_FRIST_FRAME_DISPLAY,&dal_evt_param);	// 用于给 dvb  节目清除banner
	}
	else if( FF_ACTION_DISPLAY_PROGRESS == p_control->first_frame_action){
		dal_core_event_notify( DAL_CORE_EVENT_FRIST_FRAME_DISPLAY,&dal_evt_param);
		//dal_core_event_notify( DAL_CORE_EVENT_SEEK_OK, &dal_evt_param);	// seek 之后，第一帧呈现后才进行进度显示
	}
}


/* type:    0 -> dvb      1 -> 点播*/
static void dal_core_check_frame_start(FristFrameAction_t action)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t				dal_evt_param;
	DAL_Message_t*				p_msg;

	p_control->first_frame_action = action;
#ifdef USE_ST_PLAYREC
	ststream_set_next_firstframe_callback(ststream_firstframe_callback);
	prm_set_next_firstframe_callback(ststream_firstframe_callback);

	/* 这里不再发送消息检测第一帧，把要执行的操作直接注册到ststram 中，加快响应速度*//*
	if( !(p_control->checking_first_frame) ){
		p_control->first_frame_display = 0;
		p_msg = DAL_MessageAlloc();
		p_msg->MsgType = DALMSG_CHECK_FIRST_FRAME;
		DAL_MessageSend( DAL_GetCoreQueueID(), &p_msg);
		p_control->checking_first_frame = TRUE;
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "start new check frist frame");
	}
	else{
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "already in check frist frame");
	}
	*/
#else
//?amlogic

#endif	
}

void dal_core_check_frame(void)
{
	dal_evt_param_t				dal_evt_param;
	DAL_Message_t*				p_msg;

	U32 state = amlogic_Check_Video_Status();
	
	if(state == 0)
	{
	
	DebugMessage("[DAL]dal_core_check_frame -> Entry");
		dal_core_event_notify(DAL_CORE_EVENT_FRIST_FRAME_DISPLAY,&dal_evt_param);	
	}
	else
	{
		p_msg = DAL_MessageAlloc();
		p_msg->MsgType = DALMSG_CHECK_FIRST_FRAME;
		DAL_MessageSend( DAL_GetCoreQueueID(), &p_msg);
	}
}

static void* dal_core_task(void* param)
{
	DAL_CoreControl_t*	p_control = &g_CoreControl;
	DAL_Message_t*		pMsg = NULL;
	U32					SizeCopy;
	int					iret = 0;
	
	while(1)
	{
		iret = DAL_MessageQueueReceive( p_control->queueID, &pMsg, DAL_CORE_QUEUE_DATA_SIZE, &SizeCopy, DAL_PEND);
		if( iret < 0)
		{
			continue;
		}
		else
		{
			//DebugMessage("[DAL]dal_core_task -> Recv MSG: %s  \n", DAL_MessageString(pMsg->MsgType));
		
			if( pMsg->MsgType != DALMSG_CHECK_FIRST_FRAME)
			{
				DebugMessage("[DAL]dal_core_task -> Recv MSG: %s  \n", DAL_MessageString(pMsg->MsgType));

				//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "      Recv MSG: %s  \n", DAL_MessageString(pMsg->MsgType));
			}
			
			switch( pMsg->MsgType)
			{
				case DALMSG_STREAM_START: dal_stream_start_proc(pMsg); break;	
				case DALMSG_STREAM_RESTART: 	dal_stream_restart_proc(pMsg); 		break;	
				case DALMSG_STREAM_STOP: 		dal_stream_stop_proc(pMsg);			break;	
				case DALMSG_STREAM_PAUSE: 		dal_stream_pause_proc(pMsg);		break;	
				case DALMSG_STREAM_RESUME: 		dal_stream_resume_proc(pMsg);		break;	
				case DALMSG_STREAM_SETSPEED: 	dal_stream_setspeed_proc(pMsg);		break;	
				case DALMSG_STREAM_SEEK: 		dal_stream_seek_proc(pMsg);			break;
				case DALMSG_STREAM_SEEK_PREV:	dal_stream_seek_prev_proc(pMsg);	break;
				case DALMSG_STREAM_SEEK_NEXT:	dal_stream_seek_next_proc(pMsg);	break;				
				case DALMSG_STREAM_GETINFO: 	dal_stream_getinfo_proc(pMsg);		break;
				case DALMSG_NEW_PIDS:			dal_new_pids_proc(pMsg);			break;
				case DALMSG_PIDS_CHANGED:		dal_pids_changed_proc(pMsg);		break;
				case DALMSG_STREAM_NEXT:		dal_stream_play_next_proc(pMsg);	break;
				case DALMSG_DATA_TIMEOUT:		dal_stream_data_timeout_proc(pMsg);	break;
				case DALMSG_DATA_RESUME:		dal_stream_data_resume_proc(pMsg);	break;
				//case DALMSG_FIRST_FRAME_DISPLAY: dal_stream_first_frame_display_proc(pMsg);	break;
				//case DALMSG_CHECK_FIRST_FRAME:  dal_stream_check_first_frame_proc(pMsg);	break;
				case DALMSG_CHECK_FIRST_FRAME:  dal_core_check_frame();	break;
#ifdef FLV
				case DALMSG_FLVLIST_PARSE_OK: 	dal_stream_flvlist_parse_ok_proc(pMsg);		break;
				case DALMSG_FLVLIST_PARSE_ERROR_LATER_RETRY: dal_stream_flvlist_parse_ok_proc(pMsg);		break;
				case DALMSG_FLVLIST_PARSE_ERROR: dal_stream_flvlist_parse_failed_proc(pMsg); break;
				case DALMSG_FLVLIST_PARSE_LATER_OK: dal_stream_flvlist_parse_later_ok(pMsg); break;
				case DALMSG_FLVLIST_SEEK_ACTION_DONE: dal_stream_flvlist_seek_action_done_proc(pMsg); break;		
				case DALMSG_WIAT_RETRY_FLVPLAY: dal_stream_wait_retry_flvplay_proc(pMsg);break;
#endif
				case DALMSG_DVB_CHECK_FRIST_FRAME: dal_stream_dvb_check_frist_frame_proc(pMsg);break;
				case DALMSG_P2S_RECOVERY: dal_stream_recovery_proc(pMsg);break;
				//case DALMSG_PARSE_TS_PREPARE:m3u8_perpare_parse();break;
				case DALMSG_P2V_SEEK_NEXT:P2V_seek_next(pMsg);break;
				case DALMSG_P2V_SEEK_PREV:P2V_seek_prev(pMsg);break;
				case DALMSG_HLS_SEEK_NEXT:hls_seek_next(pMsg);break;
				case DALMSG_HLS_SEEK_PREV:hls_seek_prev(pMsg);break;
				case DALMSG_TIMESHIFT_SEEK_NEXT:Timeshift_seek_next(pMsg);break;
				case DALMSG_TIMESHIFT_SEEK_PREV:Timeshift_seek_prev(pMsg);break;
				case DALMSG_FLUSH_RINGBUFFER: dal_flush_ring_buffer(pMsg);break;
				default: break;
			}
			DAL_MessageFree(pMsg);
		}
		//DAL_TaskDelay(100);
	}
	return (void*)0;
}

void dal_stream_start_proc(DAL_Message_t* pMsg)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	flv_play_list_t*			p_flv_list = NULL;
	dal_download_param_t		download_param;
	//DAL_Message_t*			p_msg;
	dal_evt_param_t				dal_evt_param;
	int							iret = 0;
	//char						url[FLV_URL_MAX_LEN];
	M3u8_address_t				m3u8_param;
	

	//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "Entry");
	DebugMessage("[DAL]dal_stream_start_proc -> Entry");
	if( p_control->status != CORE_STREAM_STOP)
	{
		//DAL_ERROR(("DAL CORE is RUNNING!!!!!!!!!!!!!!\n"));
		DebugMessage("DAL CORE is RUNNING!!!!!!!!!!!!!!\n");
		dal_core_event_notify(DAL_CORE_EVENT_PLAY_ALREADY_START,&dal_evt_param);
		return ;
	}
	
	/* copy the input and output param form dal*/
	memset( &(p_control->input), 0, sizeof(DAL_Player_Input_t));
	memcpy( &(p_control->input), &(pMsg->MsgData.StreamParam.input), sizeof(DAL_Player_Input_t));
	memset( &(p_control->output), 0, sizeof(DAL_Player_Output_t));
	memcpy( &(p_control->output), &(pMsg->MsgData.StreamParam.output), sizeof(DAL_Player_Output_t));

	/* process p2s play start*/
	if(( pMsg->MsgData.StreamParam.input.SourceType == SourceType_P2S)
		||(pMsg->MsgData.StreamParam.input.SourceType == SourceType_P2V))
	{		
	
		/*frist time to download data , alloc memory for download buffer*/
		iret = DAL_DownloadBufferInit();
		if( iret != 0)
		{
			//DAL_ERROR(("DAL_DownloadBufferInit failed\n"));
			DebugMessage("[DAL]dal_stream_start_proc->DAL_DownloadBufferInit failed\n");
			dal_core_event_notify(DAL_CORE_EVENT_DL_BUF_ALLOC_FAILED,&dal_evt_param);
			return ;
		}
		//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "DAL_DownloadBufferInit OK\n");
		DebugMessage("[DAL]dal_stream_start_proc-> DAL_DownloadBufferInit OK");

		/* start the download task*/
		
		SetP2vResumeflag(0);
		memset( &download_param, 0, sizeof(dal_download_param_t));
		strcpy( download_param.url, (char*)pMsg->MsgData.StreamParam.input.Source.http.URL);		
		iret = DAL_DownloadStart( &download_param);
		if( iret != 0)
		{
			//DAL_ERROR(("DAL_DownloadStart failed\n"));
			DebugMessage("DAL_DownloadStart failed\n");
			DAL_DownloadBufferRelease();
			dal_core_event_notify(DAL_CORE_EVENT_DL_START_FAILED,&dal_evt_param);
			return ;
		}
		//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "DAL_DownloadStart OK");
		DebugMessage("[DAL]DAL_DownloadStart OK");

		/* start the pids parse task*/
		iret = DAL_ParseStart();
		if( iret != 0)
		{
			//DAL_ERROR(("DAL_ParseStart failed\n"));			
			DebugMessage("DAL_ParseStart failed\n");
			DAL_DownloadStop();
			DAL_DownloadBufferRelease();
			dal_core_event_notify(DAL_CORE_EVENT_PIDS_PARSE_START_FAILED,&dal_evt_param);
			return ;
		}
		//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "DAL_ParseStart OK");
		DebugMessage("[DAL]DAL_ParseStart OK");

		#ifdef P2S_SWITCH_PAUSE_VIDEO	
		p_control->playrec_start_flag = 0;
		#endif	

		/* set status CORE_STREAM_WAIT_PID, when patse task get new pids and notify core task, program will be play start*/
		dal_core_set_status(CORE_STREAM_WAIT_PID);
		p_control->recovery_flag = 0;
		dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);
		//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "P2S PLAY Start download and parse OK --> CORE_STREAM_WAIT_PID");
		DebugMessage("[DAL]P2S PLAY Start download and parse OK --> CORE_STREAM_WAIT_PID ");
	}
#if defined(USE_DAL_MULTICAST)
    //{{{add by LYN, for multicast ,2011-08-22
    else if(SourceType_UDP == pMsg->MsgData.StreamParam.input.SourceType)
    {
    
        /*frist time to download data , alloc memory for download buffer*/
		iret = DAL_DownloadBufferInit();
		if( iret != 0)
		{
			DebugMessage("DAL_DownloadBufferInit failed\n");
			dal_core_event_notify(DAL_CORE_EVENT_DL_BUF_ALLOC_FAILED,&dal_evt_param);
			return ;
		}
		DebugMessage("DAL_DownloadBufferInit OK");

		/* start the download task*/
		memset( &download_param, 0, sizeof(dal_download_param_t));
		strcpy( download_param.url, (char*)pMsg->MsgData.StreamParam.input.Source.udp.URL);		
		DebugMessage("DAL_DownloadStart :%s\n",pMsg->MsgData.StreamParam.input.Source.udp.URL);
		
		iret = DAL_MulticastStart( &download_param);
		if( iret != 0)
		{
			DebugMessage("DAL_DownloadStart failed\n");
			DAL_DownloadBufferRelease();
			dal_core_event_notify(DAL_CORE_EVENT_DL_START_FAILED,&dal_evt_param);
			return ;
		}
		DebugMessage("DAL_DownloadStart OK");

		/* start the pids parse task*/
		iret = DAL_ParseStart();
		if( iret != 0)
		{
			DebugMessage("DAL_ParseStart failed\n");
			DAL_DownloadStop();
			DAL_DownloadBufferRelease();
			dal_core_event_notify(DAL_CORE_EVENT_PIDS_PARSE_START_FAILED,&dal_evt_param);
			return ;
		}
		DebugMessage("DAL_ParseStart OK");

		#ifdef P2S_SWITCH_PAUSE_VIDEO	
		p_control->playrec_start_flag = 0;
		#endif	

		/* set status CORE_STREAM_WAIT_PID, when patse task get new pids and notify core task, program will be play start*/
		dal_core_set_status(CORE_STREAM_WAIT_PID);
		p_control->recovery_flag = 0;
		dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);
		DebugMessage("UDP PLAY Start download and parse OK --> CORE_STREAM_WAIT_PID");
	}
    //}}}add by LYN, for multicast ,2011-08-22
#endif

#ifdef FLV
	/* process flvlist play start*/
	else if( pMsg->MsgData.StreamParam.input.SourceType == SourceType_FLV_LIST)
	{
		p_control->flvlist_parse_ok_flag = FALSE;

		/* get the global flvlist, before play start, the flvlist should be correct filled */
		p_flv_list = flv_list_get_list();

		if( 0 == p_flv_list->num){
/*发现js那边对于shtml类型的电影取不到list，但是任然调用播放接口，所以这里判断下*/
			DAL_ERROR(("flv_list is empty\n"));
			dal_core_event_notify(DAL_CORE_EVENT_PARSE_FLVLIST_START_FAILED,&dal_evt_param);
			return ;
		}

		/* play flvlist start with the frist part*/
		p_flv_list->cur_play = 0;

		#ifdef CONVERT_FLVLIST_URL
		/*if flvlist is not the real url which we can play directly, we should convert it to real url*/
		flv_list_convert(p_flv_list);
		#endif

		/* 开启解析FLV LIST 的信息的task ，主要是解析每小段的时长*/
		iret = flv_list_parse_start_async( p_flv_list, dal_flvlist_parse_callback_func,1);
		if( iret != 0)
		{
			/*如果这里出错，错误处理不能这么简单*/
			DAL_ERROR(("flv_list_parse_start_async failed\n"));
			dal_core_event_notify(DAL_CORE_EVENT_PARSE_FLVLIST_START_FAILED,&dal_evt_param);
			return ;
		}
		//flv_list_debug( p_flv_list);

		/* set the status CORE_STREAM_WAIT_FLV_PARSE, when flvlist parse ok, the frist flv part will be play start*/
		dal_core_set_status(CORE_STREAM_WAIT_FLV_PARSE);
		
		dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_FLVLIST_PARSE,&dal_evt_param);
		
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "FLVLIST PLAY Start flvlist parse start OK --> CORE_STREAM_WAIT_FLV_PARSE");
	}

#endif
#ifdef MMS
	/* process mms play start*/
	else if( pMsg->MsgData.StreamParam.input.SourceType == SourceType_MMS)
	{
		dal_core_event_notify(DAL_CORE_EVENT_MMS_CONNECTING,&dal_evt_param);	

		dal_core_event_notify(DAL_CORE_EVENT_PLAY_MMS_BUFFERING,&dal_evt_param);

		#if 1

		iret = get_actual_mms_url(pMsg->MsgData.StreamParam.input.Source.http.URL, sizeof(pMsg->MsgData.StreamParam.input.Source.http.URL));
		if( 0 != iret){
			SYS_DEBUG(SD_ERROR, MODULE_DAL_CORE, "get_actual_mms_url failed");
			dal_core_set_status(CORE_STREAM_STOP);
			dal_core_event_notify(DAL_CORE_EVENT_MMS_CONNECT_FAILED,0);	
		}

		iret = dal_mms_connect(pMsg->MsgData.StreamParam.input.Source.http.URL);
		if( iret != 0)
		{
			if( iret == 1){
				/* user interrupt connect*/
				SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "user interrupt connect , set dal core status CORE_STREAM_START");
				dal_core_set_status(CORE_STREAM_START);
				return ;
			}
			
			DAL_ERROR(("dal_mms_connect failed\n"));
			dal_core_set_status(CORE_STREAM_STOP);
			dal_core_event_notify(DAL_CORE_EVENT_MMS_CONNECT_FAILED,&dal_evt_param);	
			return ;		
		}
		#endif

		//dal_core_event_notify(DAL_CORE_EVENT_PLAY_MMS_BUFFERING,0);	

		PLAYREC_SET_PLAY_MMS(1);
		/*  注意DAL_PlayrecStartWithFile  只会从input->Source.http.URL 中拷贝数据*/
		iret = DAL_PlayrecStartWithFile( &(p_control->input), &(p_control->output));
		if( iret != 0)
		{
			DAL_ERROR(("MMS DAL_PlayrecStartWithFile failed\n"));
			PLAYREC_SET_PLAY_MMS(0);
			dal_core_set_status(CORE_STREAM_STOP);
			dal_mms_close();
			dal_core_event_notify(DAL_CORE_EVENT_PLAY_MMS_FAILED,&dal_evt_param);			
			return ;
		}
		
		dal_core_set_status(CORE_STREAM_START);
		dal_core_event_notify(DAL_CORE_EVENT_PLAY_MMS_OK,&dal_evt_param);		
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "MMS play start  OK           status: CORE_STREAM_START");
	}
#endif
	else if( pMsg->MsgData.StreamParam.input.SourceType == SourceType_HLS)
	{		
		/*frist time to download ts data , alloc memory for download buffer*/
		iret = DAL_DownloadBufferInit();
		if( iret != 0)
		{
			DebugMessage("[DAL]hls dal_stream_start_proc->DAL_DownloadBufferInit failed\n");
			dal_core_event_notify(DAL_CORE_EVENT_DL_BUF_ALLOC_FAILED,&dal_evt_param);
			return ;
		}
		DebugMessage("[DAL]hls dal_stream_start_proc-> DAL_DownloadBufferInit OK");
		
	//	init_ring_buffer();
		
		memset( &m3u8_param, 0, sizeof(m3u8_param));
		strcpy( m3u8_param.Url, (char*)pMsg->MsgData.StreamParam.input.Source.http.URL);	
		DebugMessage("****%s failed\n",m3u8_param.Url);
		DebugMessage("*****%s failed\n",(char*)pMsg->MsgData.StreamParam.input.Source.http.URL);
		iret = DAL_M3u8DownloadStart(&m3u8_param);
		if( iret != 0)
		{
			DebugMessage("DAL_M3u8downloadStart failed\n");
			DAL_DownloadBufferRelease();
			dal_core_event_notify(DAL_CORE_EVENT_M3U8_START_BAD_STATUS,&dal_evt_param);
			return;
		}

#if 0
		/* start the download task*/
		memset( &download_param, 0, sizeof(dal_download_param_t));
		strcpy( download_param.url, (char*)pMsg->MsgData.StreamParam.input.Source.http.URL);		
		iret = DAL_DownloadStart( &download_param);
		if( iret != 0)
		{
			//DAL_ERROR(("DAL_DownloadStart failed\n"));
			DebugMessage("DAL_DownloadStart failed\n");
			DAL_DownloadBufferRelease();
			dal_core_event_notify(DAL_CORE_EVENT_DL_START_FAILED,&dal_evt_param);
			return ;
		}

		DebugMessage("[DAL]DAL_DownloadStart OK");
#endif

	}
	else if( pMsg->MsgData.StreamParam.input.SourceType == SourceType_TIMESHIFT)
	{		
	
		/*frist time to download data , alloc memory for download buffer*/
		iret = DAL_DownloadBufferInit();
		if( iret != 0)
		{
			DebugMessage("[DAL]dal_stream_start_proc->DAL_DownloadBufferInit failed\n");
			dal_core_event_notify(DAL_CORE_EVENT_DL_BUF_ALLOC_FAILED,&dal_evt_param);
			return ;
		}
		DebugMessage("[DAL]dal_stream_start_proc-> DAL_DownloadBufferInit OK");

		/* start the download task*/
		
	//	SetP2vResumeflag(0);		
		iret = DAL_preDownloadStart();
		if( iret != 0)
		{
			DebugMessage("DAL_preDownloadStart failed\n");
			DAL_DownloadBufferRelease();
			dal_core_event_notify(DAL_CORE_EVENT_DL_START_FAILED,&dal_evt_param);
			return ;
		}
		DebugMessage("[DAL]DAL_preDownloadStart OK");

		/* start the pids parse task*/
		iret = DAL_ParseStart();
		if( iret != 0)
		{
			DebugMessage("DAL_ParseStart failed\n");
			dal_timeshift_download_stop();
			DAL_DownloadBufferRelease();
			dal_core_event_notify(DAL_CORE_EVENT_PIDS_PARSE_START_FAILED,&dal_evt_param);
			return ;
		}
		DebugMessage("[DAL]DAL_ParseStart OK");
#ifdef P2S_SWITCH_PAUSE_VIDEO	
		p_control->playrec_start_flag = 0;
#endif	

		/* set status CORE_STREAM_WAIT_PID, when patse task get new pids and notify core task, program will be play start*/
		dal_core_set_status(CORE_STREAM_WAIT_PID);
		p_control->recovery_flag = 0;
		dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);
		DebugMessage("[DAL]P2S PLAY Start download and parse OK --> CORE_STREAM_WAIT_PID ");
	}
	else
	{
		DebugMessage("[dal ]illegal SourceType!!!\n");
		return ;
	}
}

void dal_stream_restart_proc(DAL_Message_t* pMsg)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_download_param_t		param;
	dal_evt_param_t				dal_evt_param;
	int							iret = 0;
	M3u8_address_t				m3u8_param;
	dal_download_param_t		download_param;


	if( (pMsg->MsgData.StreamParam.input.SourceType == SourceType_P2S)
		||(pMsg->MsgData.StreamParam.input.SourceType == SourceType_P2V))
	{
	
		/* check dal core status*/
	 	if( p_control->status != CORE_STREAM_WAIT_PID && p_control->status != CORE_STREAM_START)
		{
			//DAL_ERROR(("NOT CORE_STREAM_WAIT_PID OR CORE_STREAM_START but %s\n", dal_core_status_string(p_control->status)));
			DebugMessage("[dal]dal_stream_restart_proc->NOT CORE_STREAM_WAIT_PID OR CORE_STREAM_START but %s\n", dal_core_status_string(p_control->status));

			dal_evt_param.status = p_control->status;
			dal_core_event_notify(DAL_CORE_EVENT_RESTART_BAD_STATUS,&dal_evt_param);
			return ;
		}
#if 1
		if( p_control->recovery_flag == 0)
		{
			/* 如果正常情况，不处理相同的请求，只有P2SRecovery 时处理该请求*/
			
			/* check if restart the same url*/
			if( strcmp((char*)pMsg->MsgData.StreamParam.input.Source.http.URL, (char*)p_control->input.Source.http.URL) == 0)
			{
				//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "switch same program >>>>>>>>>>>>>");
				DebugMessage("[dal]dal_stream_restart_proc->switch same program >>>>>>>>>>>>>");
				dal_core_event_notify(DAL_CORE_EVENT_RESTART_SAME_URL,&dal_evt_param);
				return ;
			}
		}
		else
		{
			p_control->recovery_flag = 0;
		}
#endif
		memset( &(p_control->input), 0, sizeof(DAL_Player_Input_t));
		memcpy( &(p_control->input), &(pMsg->MsgData.StreamParam.input), sizeof(DAL_Player_Input_t));
		memset( &(p_control->output), 0, sizeof(DAL_Player_Output_t));
		memcpy( &(p_control->output), &(pMsg->MsgData.StreamParam.output), sizeof(DAL_Player_Output_t));

		memset( &param, 0, sizeof(dal_download_param_t));
		strcpy( param.url, (char*)pMsg->MsgData.StreamParam.input.Source.http.URL);
	//	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "core: %s",param.url);
		DebugMessage("[dal]dal_stream_restart_proc->core: %s",param.url);
		SetP2vResumeflag(0);

		if( p_control->status == CORE_STREAM_WAIT_PID)
		{
			/*节目还没有playrecstart 和inject , 只需要重新下载和解析PIDS*/
		//	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "CORE_STREAM_WAIT_PID --> PROGRAM NOT PLAY START\n");
			DebugMessage("[dal]dal_stream_restart_proc->CORE_STREAM_WAIT_PID --> PROGRAM NOT PLAY START");

			iret = DAL_DownloadStart(&param);
			if( iret != 0)
			{
				//DAL_ERROR(("DAL_DownloadStart failed\n"));
				DebugMessage("[dal]dal_stream_restart_proc->DAL_DownloadStart failed");
				DAL_ParseStop();
				DAL_DownloadStop();
				DAL_DownloadBufferRelease();
				dal_core_set_status(CORE_STREAM_STOP);
				dal_core_event_notify(DAL_CORE_EVENT_DL_START_FAILED,&dal_evt_param);
				return ;
			}
			//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "DAL_DownloadStart OK");
			DebugMessage("[dal]dal_stream_restart_proc->DAL_DownloadStart OK");
			iret = DAL_ParseStart();
			if( iret != 0)
			{
				//DAL_ERROR(("DAL_ParseStart failed\n"));
				DebugMessage("[dal]dal_stream_restart_proc->DAL_ParseStart failed");
				DAL_ParseStop();
				DAL_DownloadStop();
				DAL_DownloadBufferRelease();
				dal_core_set_status(CORE_STREAM_STOP);
				dal_core_event_notify(DAL_CORE_EVENT_PIDS_PARSE_START_FAILED,&dal_evt_param);
				return ;
			}
			//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "DAL_ParseStart OK");
			DebugMessage("[dal]dal_stream_restart_proc->DAL_ParseStart OK");
			dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);
		}		
		else if( p_control->status == CORE_STREAM_START)
		{
			/*节目已经开始播放*/
		//	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "CORE_STREAM_START --> PROGRAM ALREADY PLAY START\n");
			DebugMessage("[dal]dal_stream_restart_proc->CORE_STREAM_START --> PROGRAM ALREADY PLAY START");
#if 0/*

			#ifdef P2S_SWITCH_PAUSE_VIDEO
			// if need switch pause video, we don't stop playrec here 
#ifdef USE_ST_PLAYREC			
			iret = PLAYREC_PlayPause(0);
#else

#endif
			if( iret != 0)
			{
				//DAL_ERROR(("PLAYREC_PlayPause or failed\n"));
				DebugMessage("[dal]dal_stream_restart_proc->PLAYREC_PlayPause or failed");
				//.......................
				dal_core_set_status(CORE_STREAM_STOP);
				return ;
			}			
			#else*/

#endif
			iret = DAL_PlayrecStop();
			if( iret != 0)
			{
				//DAL_ERROR(("DAL_PlayrecStop or failed\n"));
				
				DebugMessage("[dal]dal_stream_restart_proc->DAL_PlayrecStop or failed");
				//.......................
				dal_core_set_status(CORE_STREAM_STOP);
				return ;
			}
//			#endif
			
			
			iret = DAL_InjectPause();
			if( iret != 0)
			{
				//DAL_ERROR(("DAL_InjectPause failed\n"));
				DebugMessage("[dal]DAL_InjectPause failed\n");
				DAL_PlayrecStop();
				DAL_InjectStop();
				DAL_ParseStop();
				DAL_DownloadStop();
				DAL_DownloadBufferRelease();
				dal_core_set_status(CORE_STREAM_STOP);
				return ;
			}

			/*这里没有DAL_DownloadStop, 因为DAL_DownloadStart里会自动停止上一个CURL任务*/
			iret = DAL_DownloadStart(&param);
			if( iret != 0)
			{
				//DAL_ERROR(("DAL_DownloadStart failed\n"));
				DebugMessage("[dal]DAL_DownloadStart failed");
				DAL_PlayrecStop();
				DAL_InjectStop();
				DAL_ParseStop();
				DAL_DownloadStop();
				DAL_DownloadBufferRelease();
				dal_core_set_status(CORE_STREAM_STOP);
				return ;
			}
			iret = DAL_ParseStart();
			if( iret != 0)
			{
				//DAL_ERROR(("DAL_ParseStart failed\n"));
				DebugMessage("[dal]DAL_ParseStart failed");
				DAL_PlayrecStop();
				DAL_InjectStop();
				DAL_ParseStop();
				DAL_DownloadStop();
				DAL_DownloadBufferRelease();
				dal_core_set_status(CORE_STREAM_STOP);
				return ;
			}	
			SetDemuxP2S();
		}

		dal_core_set_status(CORE_STREAM_WAIT_PID);
		dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);
	//	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "P2S PLAY Start download and parse OK --> CORE_STREAM_WAIT_PID");
		DebugMessage("[dal]dal_stream_restart_proc ->P2S PLAY Start download and parse OK --> CORE_STREAM_WAIT_PID");
	}
#if defined(USE_DAL_MULTICAST)
	//{{{add by LYN, for multicast ,2011-08-22
	else if(SourceType_UDP== pMsg->MsgData.StreamParam.input.SourceType)
	{
	 	/* check dal core status*/
	 	if( p_control->status != CORE_STREAM_WAIT_PID && p_control->status != CORE_STREAM_START)
		{
			DebugMessage("NOT CORE_STREAM_WAIT_PID OR CORE_STREAM_START but %s\n", dal_core_status_string(p_control->status));
			dal_evt_param.status = (U32)p_control->status;
			dal_core_event_notify(DAL_CORE_EVENT_RESTART_BAD_STATUS, &dal_evt_param);
			return ;
		}

		/* check if restart the same url*/
		if( strcmp((char*)pMsg->MsgData.StreamParam.input.Source.http.URL, (char*)p_control->input.Source.http.URL) == 0)
		{
			DebugMessage("switch same program >>>>>>>>>>>>>");
			dal_core_event_notify(DAL_CORE_EVENT_RESTART_SAME_URL,&dal_evt_param);
			return ;
		}		

		memset( &(p_control->input), 0, sizeof(DAL_Player_Input_t));
		memcpy( &(p_control->input), &(pMsg->MsgData.StreamParam.input), sizeof(DAL_Player_Input_t));
		memset( &(p_control->output), 0, sizeof(DAL_Player_Output_t));
		memcpy( &(p_control->output), &(pMsg->MsgData.StreamParam.output), sizeof(DAL_Player_Output_t));

		memset( &param, 0, sizeof(dal_download_param_t));
		strcpy( param.url, (char*)pMsg->MsgData.StreamParam.input.Source.udp.URL);
		DebugMessage("core: %s",param.url);

		if( p_control->status == CORE_STREAM_WAIT_PID)
		{
			/*节目还没有playrecstart 和inject , 只需要重新下载和解析PIDS*/
			DebugMessage("CORE_STREAM_WAIT_PID --> PROGRAM NOT PLAY START");
			iret = DAL_MulticastStart(&param);
			if( iret != 0)
			{
				DebugMessage("DAL_DownloadStart failed\n");
				DAL_ParseStop();
				DAL_MulticastStop();
				DAL_DownloadBufferRelease();
				dal_core_set_status(CORE_STREAM_STOP);
				dal_core_event_notify(DAL_CORE_EVENT_DL_START_FAILED,&dal_evt_param);
				return ;
			}
			DebugMessage("DAL_DownloadStart OK");
			iret = DAL_ParseStart();
			if( iret != 0)
			{
				DebugMessage("DAL_ParseStart failed\n");
				DAL_ParseStop();
				DAL_MulticastStop();
				DAL_DownloadBufferRelease();
				dal_core_set_status(CORE_STREAM_STOP);
				dal_core_event_notify(DAL_CORE_EVENT_PIDS_PARSE_START_FAILED,&dal_evt_param);
				return ;
			}
			DebugMessage("DAL_ParseStart OK");
			dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);
		}	
		else if( p_control->status == CORE_STREAM_START)
		{
			/*节目已经开始播放*/
			DebugMessage("CORE_STREAM_START --> PROGRAM ALREADY PLAY START");
#if 0
			#ifdef P2S_SWITCH_PAUSE_VIDEO
			// if need switch pause video, we don't stop playrec here 
			iret = PLAYREC_PlayPause(0);
			if( iret != 0)
			{
				DAL_ERROR(("PLAYREC_PlayPause or failed\n"));
				//.......................
				dal_core_set_status(CORE_STREAM_STOP);
				return ;
			}
			
			#else
			#endif
#endif
			iret = DAL_PlayrecStop();
			if( iret != 0)
			{
				DebugMessage("DAL_PlayrecStop or failed\n");
				//.......................
				dal_core_set_status(CORE_STREAM_STOP);
				return ;
			}
			
			
			iret = DAL_InjectPause();
			if( iret != 0)
			{
				DebugMessage("DAL_InjectPause failed\n");
				DAL_PlayrecStop();
				DAL_InjectStop();
				DAL_ParseStop();
				DAL_MulticastStop();
				DAL_DownloadBufferRelease();
				dal_core_set_status(CORE_STREAM_STOP);
				return ;
			}

			/*这里没有DAL_DownloadStop, 因为DAL_DownloadStart里会自动停止上一个CURL任务*/
			iret = DAL_MulticastStart(&param);
			if( iret != 0)
			{
				DebugMessage("DAL_DownloadStart failed\n");
				DAL_PlayrecStop();
				DAL_InjectStop();
				DAL_ParseStop();
				DAL_MulticastStop();
				DAL_DownloadBufferRelease();
				dal_core_set_status(CORE_STREAM_STOP);
				return ;
			}
			iret = DAL_ParseStart();
			if( iret != 0)
			{
				DebugMessage("DAL_ParseStart failed\n");
				DAL_PlayrecStop();
				DAL_InjectStop();
				DAL_ParseStop();
				DAL_MulticastStop();
				DAL_DownloadBufferRelease();
				dal_core_set_status(CORE_STREAM_STOP);
				return ;
			}		
		}

		dal_core_set_status(CORE_STREAM_WAIT_PID);
		dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);
		DebugMessage("P2S PLAY Start download and parse OK --> CORE_STREAM_WAIT_PID");
	}
	//}}}add by LYN, for multicast ,2011-08-22
#endif
#ifdef MMS
	else if( pMsg->MsgData.StreamParam.input.SourceType == SourceType_MMS)
	{
		/* check dal core status*/
	 	if( p_control->status != CORE_STREAM_START)
		{
			DAL_ERROR(("NOT CORE_STREAM_WAIT_PID OR CORE_STREAM_START but %s\n", dal_core_status_string(p_control->status)));
			dal_evt_param.status = (U32)p_control->status;
			dal_core_event_notify(DAL_CORE_EVENT_RESTART_BAD_STATUS,&dal_evt_param);
			return ;
		}

		/* check if restart the same url*/
		if( strcmp((char*)pMsg->MsgData.StreamParam.input.Source.http.URL, (char*)p_control->input.Source.http.URL) == 0)
		{
			SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "switch same program >>>>>>>>>>>>>");
			dal_core_event_notify(DAL_CORE_EVENT_RESTART_SAME_URL,&dal_evt_param);
			return ;
		}
		
		memset( &(p_control->input), 0, sizeof(DAL_Player_Input_t));
		memcpy( &(p_control->input), &(pMsg->MsgData.StreamParam.input), sizeof(DAL_Player_Input_t));
		memset( &(p_control->output), 0, sizeof(DAL_Player_Output_t));
		memcpy( &(p_control->output), &(pMsg->MsgData.StreamParam.output), sizeof(DAL_Player_Output_t));		
			
		//PLAYREC_PlayPause(0);

		iret = get_actual_mms_url(pMsg->MsgData.StreamParam.input.Source.http.URL,sizeof(pMsg->MsgData.StreamParam.input.Source.http.URL));
		if( 0 != iret){
			SYS_DEBUG(SD_ERROR, MODULE_DAL_CORE, "get_actual_mms_url failed");
			dal_core_set_status(CORE_STREAM_STOP);
			dal_core_event_notify(DAL_CORE_EVENT_MMS_CONNECT_FAILED,&dal_evt_param);	
		}

		#if 1
		iret = dal_mms_close();
		if( iret != 0)
		{
			DAL_ERROR(("dal_mms_close failed\n"));
			//dal_core_event_notify(DAL_CORE_EVENT_MMS_CLOSE_FAILED, 0);
		}
		#endif
		
		iret = DAL_PlayrecStop();
		if( iret != 0)
		{
			DAL_ERROR((" ????? DAL_PlayrecStop failed\n"));
			//dal_core_event_notify(DAL_CORE_EVENT_MMS_CLOSE_FAILED, 0);
		}
		
		dal_core_event_notify(DAL_CORE_EVENT_MMS_CONNECTING,&dal_evt_param);	

		dal_core_event_notify(DAL_CORE_EVENT_PLAY_MMS_BUFFERING,&dal_evt_param);	
		
		iret = dal_mms_connect(pMsg->MsgData.StreamParam.input.Source.http.URL);
		if( iret != 0)
		{
			if( iret == 1){
				/* user interrupt connect*/
				SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "user interrupt connect , set dal status CORE_STREAM_START");
				dal_core_set_status(CORE_STREAM_START);
				return ;
			}
			DAL_ERROR(("dal_mms_connect failed\n"));
			dal_core_set_status(CORE_STREAM_STOP);
			dal_core_event_notify(DAL_CORE_EVENT_MMS_CONNECT_FAILED,&dal_evt_param);	
			return ;		
		}
		//dal_core_event_notify(DAL_CORE_EVENT_PLAY_MMS_BUFFERING,0);	
		
		/*  注意DAL_PlayrecStartWithFile  只会从input->Source.http.URL 中拷贝数据*/
		PLAYREC_SET_PLAY_MMS(1);
		iret = DAL_PlayrecStartWithFile( &(p_control->input), &(p_control->output));
		if( iret != 0)
		{
			DAL_ERROR(("MMS DAL_PlayrecStartWithFile failed\n"));
			PLAYREC_SET_PLAY_MMS(0);
			dal_core_set_status(CORE_STREAM_STOP);
			dal_mms_close();
			dal_core_event_notify(DAL_CORE_EVENT_PLAY_MMS_FAILED,&dal_evt_param);			
			return ;
		}
		dal_core_set_status(CORE_STREAM_START);
		dal_core_event_notify(DAL_CORE_EVENT_PLAY_MMS_OK,&dal_evt_param);
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "MMS play restart  OK           status: CORE_STREAM_START");
	}
#endif
	else if( pMsg->MsgData.StreamParam.input.SourceType == SourceType_HLS)
	{
		/* check dal core status*/
		if( p_control->status != CORE_STREAM_WAIT_PID && p_control->status != CORE_STREAM_START)
		{
		
			DebugMessage("[dal] HLS dal_stream_restart_proc->NOT CORE_STREAM_WAIT_PID OR CORE_STREAM_START but %s\n", dal_core_status_string(p_control->status));
		//	dal_evt_param.status = p_control->status;
		//	dal_core_event_notify(DAL_CORE_EVENT_M3U8_RESTART_BAD_STATUS,&dal_evt_param);
			
			memset( &m3u8_param, 0, sizeof(m3u8_param));
			strcpy( m3u8_param.Url, (char*)pMsg->MsgData.StreamParam.input.Source.http.URL);		
			iret = DAL_M3u8DownloadStart(&m3u8_param);
			if( iret != 0)
			{
				DebugMessage("DAL_M3u8downloadStart failed\n");
				return;
			}
			
			DebugMessage("[dal] HLS dal_stream_restart_proc->try restart next");
			dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);
			return ;
		}

		if( p_control->recovery_flag == 0)
		{
			/* 如果正常情况，不处理相同的请求，只有P2SRecovery 时处理该请求*/
			
			/* check if restart the same url*/
			if( strcmp((char*)pMsg->MsgData.StreamParam.input.Source.http.URL, (char*)p_control->input.Source.http.URL) == 0)
			{
				DebugMessage("[dal] HLS dal_stream_restart_proc->switch same program >>>>>>>>>>>>>");
				dal_core_event_notify(DAL_CORE_EVENT_RESTART_SAME_URL,&dal_evt_param);
				//return ;
			}
		}
		else
		{
			p_control->recovery_flag = 0;
		}

		memset( &(p_control->input), 0, sizeof(DAL_Player_Input_t));
		memcpy( &(p_control->input), &(pMsg->MsgData.StreamParam.input), sizeof(DAL_Player_Input_t));
		memset( &(p_control->output), 0, sizeof(DAL_Player_Output_t));
		memcpy( &(p_control->output), &(pMsg->MsgData.StreamParam.output), sizeof(DAL_Player_Output_t));

		memset( &m3u8_param, 0, sizeof(m3u8_param));
		strcpy( m3u8_param.Url, (char*)pMsg->MsgData.StreamParam.input.Source.http.URL);		
		DebugMessage("[dal] HLS dal_stream_restart_proc->core: %s",m3u8_param.Url);

		if( p_control->status == CORE_STREAM_WAIT_PID)
		{
			/*节目还没有playrecstart 和inject , 只需要重新下载和解析PIDS*/
			DebugMessage("[dal] HLS dal_stream_restart_proc->CORE_STREAM_WAIT_PID --> PROGRAM NOT PLAY START");
			
			
			iret = DAL_M3u8DownloadStop();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc -> ????? DAL_M3u8DownloadStop failed");
			}
			
			iret = ts_download_stop();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc ->????? ts_download_stop failed");
			}
			
			iret = DAL_ParseStop();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc ->	????? DAL_ParseStop failed");
			}
			dal_core_set_status(CORE_STREAM_STOP);

			iret = DAL_M3u8DownloadStart(&m3u8_param);
			if( iret != 0)
			{
				DebugMessage("DAL_M3u8downloadStart failed\n");
				return;
			}
			
			DebugMessage("[dal] HLS dal_stream_restart_proc->DAL_ParseStart OK");
			dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);
		}		
		else if( p_control->status == CORE_STREAM_START)
		{
			/*节目已经开始播放*/
			DebugMessage("[dal] HLS dal_stream_restart_proc->CORE_STREAM_START --> PROGRAM ALREADY PLAY START");
						
			iret = DAL_M3u8DownloadStop();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc -> ????? DAL_M3u8DownloadStop failed");
			}
			iret = ts_download_stop();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc ->????? ts_download_stop failed");
			}
			
			iret = DAL_PlayrecStop();
			if( iret != 0)
			{	
				DebugMessage("[dal] HLS dal_stream_restart_proc->DAL_PlayrecStop or failed");
				dal_core_set_status(CORE_STREAM_STOP);
				return ;
			}
			
			iret = DAL_ParseStop();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc ->	????? DAL_ParseStop failed");
			}
			
			iret = DAL_InjectPause();
			if( iret != 0)
			{
				DebugMessage("[dal] HLS DAL_InjectPause failed\n");
				DAL_InjectStop();
				//DAL_DownloadBufferRelease();
				dal_core_set_status(CORE_STREAM_STOP);
				return ;
			}
			
			dal_core_set_status(CORE_STREAM_STOP);
			iret = SetDemuxP2S();
			if( iret != 0)
			{
				DebugMessage("[DAL]SetDemuxP2S failed %x ",iret);
				return;
			}

			iret = DAL_M3u8DownloadStart(&m3u8_param);
			if( iret != 0)
			{
				DebugMessage("[dal] HLS DAL_DownloadStart failed");
				DAL_InjectStop();
				//DAL_DownloadBufferRelease();
				dal_core_set_status(CORE_STREAM_STOP);
				return;
			}
		
		}

		DebugMessage("[dal] HLS dal_stream_restart_proc ->m3u8 Start download --> CORE_STREAM_STOP");
	}	
	else
	{
		DebugMessage("[dal]dal_stream_restart_proc ->illegal SourceType!!!   %d\n",pMsg->MsgData.StreamParam.input.SourceType);
		return ;
	}
}

void dal_stream_stop_proc(DAL_Message_t* pMsg)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t				dal_evt_param;
	int							iret = 0;

	//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "entry");
	DebugMessage("[dal]dal_stream_stop_proc ->entry");
	DebugMessage("[dal]HLS dal_stream_stop_proc ->->->->->-> p_control->status %d->->->->->->->->", p_control->status);

	if(( p_control->input.SourceType == SourceType_P2S)
		||( p_control->input.SourceType == SourceType_P2V))
	{
		if( p_control->status != CORE_STREAM_WAIT_PID && p_control->status != CORE_STREAM_START)
		{
			//DAL_ERROR(("NOT CORE_STREAM_WAIT_PID OR CORE_STREAM_START"));
			DebugMessage("[dal]dal_stream_stop_proc ->NOT CORE_STREAM_WAIT_PID OR CORE_STREAM_START");
			dal_evt_param.status = (U32)p_control->status;
			dal_core_event_notify(DAL_CORE_EVENT_P2S_STOP_BAD_STATUS,&dal_evt_param);
			return ;
		}

		dal_core_event_notify( DAL_CORE_EVENT_PLAY_WILL_STOP,&dal_evt_param);
		SetP2vResumeflag(0);

		if( p_control->status == CORE_STREAM_WAIT_PID)
		{
			//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "CORE_STREAM_WAIT_PID --> PROGRAM NOT PLAY START");
			DebugMessage("[dal]dal_stream_stop_proc ->CORE_STREAM_WAIT_PID --> PROGRAM NOT PLAY START");
			iret = DAL_DownloadStop();
			if( iret != 0)
			{
				//DAL_ERROR((" ????? DAL_DownloadStop failed\n"));
				DebugMessage("[dal]dal_stream_stop_proc ->????? DAL_DownloadStop failed");
			}
			iret = DAL_DownloadBufferRelease();
			if( iret != 0)
			{
				//DAL_ERROR((" ????? DAL_DownloadBufferRelease failed\n"));
				DebugMessage("[dal]dal_stream_stop_proc -> ????? DAL_DownloadBufferRelease failed");
			}
			
			#ifdef P2S_SWITCH_PAUSE_VIDEO
			if( p_control->playrec_start_flag == 1)
			{
				iret = DAL_PlayrecStop();

				if( iret != 0)
				{
					//DAL_ERROR((" ????? DAL_PlayrecStop failed\n"));
					DebugMessage("[dal]dal_stream_stop_proc -> ????? DAL_PlayrecStop failed");
				}
				p_control->playrec_start_flag = 0;
			}
			#endif

			iret = DAL_ParseStop();
			if( iret != 0)
			{
				//DAL_ERROR((" ????? DAL_ParseStop failed\n"));
				DebugMessage("[dal]dal_stream_stop_proc -> ????? DAL_ParseStop failed");
			}
		}
		else if( p_control->status == CORE_STREAM_START)
		{
			//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "CORE_STREAM_START --> PROGRAM ALREADY PLAY START");
			DebugMessage("[dal]dal_stream_stop_proc -> CORE_STREAM_START --> PROGRAM ALREADY PLAY START");
			iret = DAL_DownloadStop();
			if( iret != 0)
			{
				//DAL_ERROR((" ????? DAL_DownloadStop failed\n"));	
				DebugMessage("[dal]dal_stream_stop_proc -> ????? DAL_DownloadStop failed");
			}
			iret = DAL_DownloadBufferRelease();
			if( iret != 0)
			{
				//DAL_ERROR((" ????? DAL_DownloadBufferRelease failed\n"));
				DebugMessage("[dal]dal_stream_stop_proc ->  ????? DAL_DownloadBufferRelease failed");
			}	
			iret = DAL_ParseStop();
			if( iret != 0)
			{
				DebugMessage("[dal]dal_stream_stop_proc ->	????? DAL_ParseStop failed");
				//DAL_ERROR((" ????? DAL_ParseStop failed\n"));
			}
			iret = DAL_PlayrecStop();
			if( iret != 0)
			{
				DebugMessage("[dal]dal_stream_stop_proc ->	 ????? DAL_PlayrecStop failed");
				//DAL_ERROR((" ????? DAL_PlayrecStop failed\n"));
			}
			iret = DAL_InjectStop();
			if( iret != 0)
			{
				DebugMessage("[dal]dal_stream_stop_proc -> ????DAL_InjectStop failed");
				//DAL_ERROR((" ????? DAL_InjectStop failed\n"));
			}
		}
		Amlogic_SetTSSource();
		dal_core_set_status(CORE_STREAM_STOP);		
		dal_core_event_notify( DAL_CORE_EVENT_STOP_OK, &dal_evt_param);
		//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "P2S PLAY Stop --> CORE_STREAM_STOP");
		DebugMessage("[dal]dal_stream_stop_proc -> P2S PLAY Stop --> CORE_STREAM_STOP");

	}

#if defined(USE_DAL_MULTICAST)	
    //{{{add by LYN, for multicast ,2011-08-22
    else if( SourceType_UDP == p_control->input.SourceType)
    {
       if( p_control->status != CORE_STREAM_WAIT_PID && p_control->status != CORE_STREAM_START)
		{
			DebugMessage("NOT CORE_STREAM_WAIT_PID OR CORE_STREAM_START");
			dal_evt_param.status = (U32)p_control->status;
			dal_core_event_notify(DAL_CORE_EVENT_P2S_STOP_BAD_STATUS,&dal_evt_param);
			return ;
		}

		dal_core_event_notify( DAL_CORE_EVENT_PLAY_WILL_STOP, &dal_evt_param);

		if( p_control->status == CORE_STREAM_WAIT_PID)
		{
			DebugMessage("CORE_STREAM_WAIT_PID --> PROGRAM NOT PLAY START");
		
			iret = DAL_MulticastStop();
			if( iret != 0)
			{
				DebugMessage(" ????? DAL_DownloadStop failed\n");
			}
			iret = DAL_DownloadBufferRelease();
			if( iret != 0)
			{
				DebugMessage(" ????? DAL_DownloadBufferRelease failed\n");
			}
			
			#ifdef P2S_SWITCH_PAUSE_VIDEO
			if( p_control->playrec_start_flag == 1)
			{
				iret = DAL_PlayrecStop();
				if( iret != 0)
				{
					DebugMessage(" ????? DAL_PlayrecStop failed\n");
				}
				p_control->playrec_start_flag = 0;
			}
			#endif

			iret = DAL_ParseStop();
			if( iret != 0)
			{
				DebugMessage(" ????? DAL_ParseStop failed\n");
			}
		}
		else if( p_control->status == CORE_STREAM_START)
		{
			DebugMessage("CORE_STREAM_START --> PROGRAM ALREADY PLAY START");
			
			iret = DAL_MulticastStop();
			if( iret != 0)
			{
				DebugMessage(" ????? DAL_DownloadStop failed\n");
			}
			iret = DAL_DownloadBufferRelease();
			if( iret != 0)
			{
				DebugMessage(" ????? DAL_DownloadBufferRelease failed\n");
			}	
			iret = DAL_ParseStop();
			if( iret != 0)
			{
				DebugMessage(" ????? DAL_ParseStop failed\n");
			}
			iret = DAL_PlayrecStop();
			if( iret != 0)
			{
				DebugMessage(" ????? DAL_PlayrecStop failed\n");
			}
			iret = DAL_InjectStop();
			if( iret != 0)
			{
				DebugMessage(" ????? DAL_InjectStop failed\n");
			}
		}
		
		dal_core_set_status(CORE_STREAM_STOP);
		dal_core_event_notify( DAL_CORE_EVENT_STOP_OK, &dal_evt_param);
		DebugMessage("P2S PLAY Stop --> CORE_STREAM_STOP");
    }
    //}}}add by LYN, for multicast ,2011-08-22
#endif
#ifdef FLV
	else if( p_control->input.SourceType == SourceType_FLV_LIST)
	{
		/*check status*/
		if( p_control->status != CORE_STREAM_WAIT_FLV_PARSE 
			&& p_control->status != CORE_STREAM_START
			&&  p_control->status != CORE_STREAM_WAIT_FLVPLAY
			&&  p_control->status != CORE_STREAM_PAUSE)
		{
			DAL_ERROR(("NOT CORE_STREAM_WAIT_PID OR CORE_STREAM_START\n"));
			dal_evt_param.status = (U32)p_control->status;
			dal_core_event_notify(DAL_CORE_EVENT_FLV_STOP_BAD_STATUS,&dal_evt_param);
			return ;
		}

		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "type SourceType_FLV_LIST  before DAL_PlayrecStop!");
		dal_core_event_notify( DAL_CORE_EVENT_PLAY_WILL_STOP, &dal_evt_param);

		if( CORE_STREAM_START == p_control->status
			|| CORE_STREAM_PAUSE == p_control->status)
		{
			if( playrec_get_http_timeout_flag())
			{
				/*如果因为http curl 连接超时而结束播放*/
				dal_core_event_notify( DAL_CORE_EVENT_CONNECT_ERROR, &dal_evt_param);
			
				/*
				#if defined(USE_CONTAINER)
							Oplayer_DisplayNotify( CL_NOTIFY_CONNECT_TIMEOUT);
							SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "##### Oplayer_DisplayNotify  CL_NOTIFY_CONNECT_TIMEOUT");
				#endif
							*/
			}
			else
			{
				dal_core_event_notify( DAL_CORE_EVENT_BEFORE_PLAYREC_STOP, &dal_evt_param);
			
				/*
				#if defined(USE_CONTAINER)
							Oplayer_DisplayNotify( CL_NOTIFY_CLOSEING_PLAY);
							SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "##### Oplayer_DisplayNotify  CL_NOTIFY_CLOSEING_PLAY");
				#endif
							*/
			}
			iret = DAL_PlayrecStop();
			if( iret != 0){
				DAL_ERROR((" ????? DAL_PlayrecStop failed\n"));
			}
			iret = flv_list_parse_stop();
			if( iret != 0){
				DAL_ERROR((" ????? flv_list_parse_stop failed\n"));
			}
		}
		PLAYREC_SET_PLAY_FLVLIST(0);
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "type SourceType_FLV_LIST  after DAL_PlayrecStop!");

		dal_core_set_status(CORE_STREAM_STOP);
		dal_core_event_notify( DAL_CORE_EVENT_STOP_OK, &dal_evt_param);
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "FLVLIST PLAY Stop --> CORE_STREAM_STOP");
	}
#endif
#ifdef MMS
	/* process mms play start*/
	else if( p_control->input.SourceType == SourceType_MMS)
	{
		if( p_control->status != CORE_STREAM_START)
		{
			DAL_ERROR(("NOT CORE_STREAM_WAIT_PID OR CORE_STREAM_START"));
			//dal_core_event_notify(DAL_CORE_EVENT_P2S_STOP_BAD_STATUS,(U32)p_control->status);
			return ;
		}

		dal_core_event_notify( DAL_CORE_EVENT_PLAY_WILL_STOP, &dal_evt_param);
		//PLAYREC_PlayPause(0);

		iret = dal_mms_close();
		if( iret != 0)
		{
			DAL_ERROR(("dal_mms_close failed\n"));
			//dal_core_event_notify(DAL_CORE_EVENT_MMS_CLOSE_FAILED, &dal_evt_param);
		}

		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "****************************************************************************************** after dal_mms_close");

		iret = DAL_PlayrecStop();
		if( iret != 0)
		{
			DAL_ERROR((" ????? DAL_PlayrecStop failed\n"));
			//dal_core_event_notify(DAL_CORE_EVENT_MMS_CLOSE_FAILED, &dal_evt_param);
		}
		PLAYREC_SET_PLAY_MMS(0);

		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE,"****************************************************************************************** after DAL_PlayrecStop");

		dal_core_set_status(CORE_STREAM_STOP);
		dal_core_event_notify( DAL_CORE_EVENT_STOP_OK, &dal_evt_param);
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "MMS PLAY Stop --> CORE_STREAM_STOP\n");
	}
#endif
	else if( p_control->input.SourceType == SourceType_HLS)
	{
	
	DebugMessage("[dal]HLS dal_stream_stop_proc ->->->->->->->->->->->->->->");
	DebugMessage("[dal]HLS dal_stream_stop_proc ->->->->->-> p_control->status %d->->->->->->->->", p_control->status);
	DebugMessage("[dal]HLS dal_stream_stop_proc ->->->->->->->->->->->->->->");

		if( p_control->status != CORE_STREAM_WAIT_PID && p_control->status != CORE_STREAM_START)
		{
			DebugMessage("[dal]HLS dal_stream_stop_proc ->NOT CORE_STREAM_WAIT_PID OR CORE_STREAM_START");
			dal_evt_param.status = (U32)p_control->status;
			
			dal_set_state(1,PLAYER_IDLE);
			dal_core_event_notify(DAL_CORE_EVENT_HLS_STOP_BAD_STATUS,&dal_evt_param);
			return ;
		}

		dal_core_event_notify( DAL_CORE_EVENT_PLAY_WILL_STOP,&dal_evt_param);

		if( p_control->status == CORE_STREAM_WAIT_PID)
		{
			DebugMessage("[dal]HLS dal_stream_stop_proc ->CORE_STREAM_WAIT_PID --> PROGRAM NOT PLAY START");
			iret = DAL_M3u8DownloadStop();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc -> ????? DAL_M3u8DownloadStop failed");
			}
			iret = ts_download_stop();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc ->????? ts_download_stop failed");
			}
			
			iret = DAL_DownloadBufferRelease();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc -> ????? DAL_DownloadBufferRelease failed");
			}
			
#ifdef P2S_SWITCH_PAUSE_VIDEO
			if( p_control->playrec_start_flag == 1)
			{
				iret = DAL_PlayrecStop();

				if( iret != 0)
				{
					DebugMessage("[dal]HLS dal_stream_stop_proc -> ????? DAL_PlayrecStop failed");
				}
				p_control->playrec_start_flag = 0;
			}
#endif
			iret = DAL_ParseStop();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc -> ????? DAL_ParseStop failed");
			}
		}
		else if( p_control->status == CORE_STREAM_START)
		{
			DebugMessage("[dal]HLS dal_stream_stop_proc -> CORE_STREAM_START --> PROGRAM ALREADY PLAY START");
			iret = DAL_M3u8DownloadStop();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc -> ????? DAL_M3u8DownloadStop failed");
			}
			
			iret = ts_download_stop();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc ->????? ts_download_stop failed");
			}
			
			iret = DAL_InjectStop();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc -> ????DAL_InjectStop failed");
			}
			iret = DAL_DownloadBufferRelease();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc ->	????? DAL_DownloadBufferRelease failed");
			}
			
			iret = DAL_PlayrecStop();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc ->	 ????? DAL_PlayrecStop failed");
			}
			iret = DAL_ParseStop();
			if( iret != 0)
			{
				DebugMessage("[dal]HLS dal_stream_stop_proc ->	????? DAL_ParseStop failed");
			}
			
		}
		dal_core_set_status(CORE_STREAM_STOP);
		DebugMessage("[dal]HLS dal_stream_stop_proc -> HLS PLAY Stop --> CORE_STREAM_STOP");
		del_DSC_key();
		Amlogic_SetTSSource();
		
		dal_core_event_notify( DAL_CORE_EVENT_STOP_OK, &dal_evt_param);
	}
	else if( p_control->input.SourceType == SourceType_TIMESHIFT)
	{
		if( p_control->status != CORE_STREAM_WAIT_PID && p_control->status != CORE_STREAM_START)
		{
			DebugMessage("[dal]dal_stream_stop_proc ->NOT CORE_STREAM_WAIT_PID OR CORE_STREAM_START");
			dal_evt_param.status = (U32)p_control->status;
			dal_core_event_notify(DAL_CORE_EVENT_P2S_STOP_BAD_STATUS,&dal_evt_param);
			return ;
		}

		dal_core_event_notify( DAL_CORE_EVENT_PLAY_WILL_STOP,&dal_evt_param);
	//	SetP2vResumeflag(0);

		if( p_control->status == CORE_STREAM_WAIT_PID)
		{
			DebugMessage("[dal]dal_stream_stop_proc ->CORE_STREAM_WAIT_PID --> PROGRAM NOT PLAY START");
			iret = dal_timeshift_download_stop();
			if( iret != 0)
			{
				DebugMessage("[dal]dal_stream_stop_proc ->????? DAL_DownloadStop failed");
			}
			iret = DAL_DownloadBufferRelease();
			if( iret != 0)
			{
				DebugMessage("[dal]dal_stream_stop_proc -> ????? DAL_DownloadBufferRelease failed");
			}
			
			#ifdef P2S_SWITCH_PAUSE_VIDEO
			if( p_control->playrec_start_flag == 1)
			{
				iret = DAL_PlayrecStop();

				if( iret != 0)
				{
					//DAL_ERROR((" ????? DAL_PlayrecStop failed\n"));
					DebugMessage("[dal]dal_stream_stop_proc -> ????? DAL_PlayrecStop failed");
				}
				p_control->playrec_start_flag = 0;
			}
			#endif

			iret = DAL_ParseStop();
			if( iret != 0)
			{
				DebugMessage("[dal]dal_stream_stop_proc -> ????? DAL_ParseStop failed");
			}
		}
		else if( p_control->status == CORE_STREAM_START)
		{
			DebugMessage("[dal]dal_stream_stop_proc -> CORE_STREAM_START --> PROGRAM ALREADY PLAY START");
			iret = dal_timeshift_download_stop();
			if( iret != 0)
			{
				DebugMessage("[dal]dal_stream_stop_proc -> ????? DAL_DownloadStop failed");
			}
			iret = DAL_DownloadBufferRelease();
			if( iret != 0)
			{
				DebugMessage("[dal]dal_stream_stop_proc ->  ????? DAL_DownloadBufferRelease failed");
			}	
			iret = DAL_ParseStop();
			if( iret != 0)
			{
				DebugMessage("[dal]dal_stream_stop_proc ->	????? DAL_ParseStop failed");
			}
			iret = DAL_PlayrecStop();
			if( iret != 0)
			{
				DebugMessage("[dal]dal_stream_stop_proc ->	 ????? DAL_PlayrecStop failed");
			}
			iret = DAL_InjectStop();
			if( iret != 0)
			{
				DebugMessage("[dal]dal_stream_stop_proc -> ????DAL_InjectStop failed");
			}
		}
		
		dal_core_set_status(CORE_STREAM_STOP);
		dal_core_event_notify( DAL_CORE_EVENT_STOP_OK, &dal_evt_param);
		DebugMessage("[dal]dal_stream_stop_proc -> P2S PLAY Stop --> CORE_STREAM_STOP");

	}
	else
	{
		DebugMessage("[dal]dal_stream_stop_proc -> illegal SourceType!!!\n");
		//DAL_ERROR(("illegal SourceType!!!\n"));
		return ;
	}
}


void dal_stream_pause_proc(DAL_Message_t* pMsg)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t				dal_evt_param;
	S32				error;

	if( p_control->input.SourceType == SourceType_FLV_LIST)
	{
		if( p_control->status != CORE_STREAM_START)
		{

		}
#ifdef USE_ST_PLAYREC		
		error = PLAYREC_PlayPause(0);
		if( error != ST_NO_ERROR)
		{
			DAL_ERROR(("PLAYREC_PlayPause failed\n"));
			dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
			return ;
		}
#else
// ?hisi
#endif
		dal_core_set_status(CORE_STREAM_PAUSE);
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_OK, &dal_evt_param);
		DebugMessage("[dal]dal_stream_pause_proc -> FLVLIST PAUSE OK --> CORE_STREAM_STOP");

		//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "FLVLIST PAUSE OK --> CORE_STREAM_STOP");
	}
	else
	{
	
		DebugMessage("[dal]dal_stream_pause_proc ->illegal SourceType!!!\n");
		//DAL_ERROR(("illegal SourceType!!!\n"));
		return ;
	}
}

void dal_stream_resume_proc(DAL_Message_t* pMsg)
{	
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t				dal_evt_param;
#ifdef USE_ST_PLAYREC	
	ST_ErrorCode_t				error;
#else
	S32				error =0;

#endif
	if( p_control->input.SourceType == SourceType_FLV_LIST)
	{
#ifdef FLV
		error = PLAYREC_PlayResume(0);
		if( error != ST_NO_ERROR)
		{
			DAL_ERROR(("PLAYREC_PlayPause failed\n"));
			return ;
		}
		dal_core_set_status(CORE_STREAM_START);
		dal_core_event_notify( DAL_CORE_EVENT_RESUME_OK,&dal_evt_param);
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "FLVLIST Resume OK --> CORE_STREAM_START");
#endif		
	}
	else
	{
		DAL_ERROR(("illegal SourceType!!!\n"));
		return ;
	}
}

void dal_stream_setspeed_proc(DAL_Message_t* pMsg)
{
	return ;
}

void dal_stream_seek_proc(DAL_Message_t* pMsg)
{
	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "NOT SUPPORT YET");

#if 0

	DAL_CoreControl_t*			p_control = &g_CoreControl;
	flv_play_list_t*			p_flv_list;
	double						msec_in_part;
	int							iret = 0;
	unsigned int				index;
	ST_ErrorCode_t				error;

	



	if( p_control->input.SourceType == SourceType_FLV_LIST)
	{
		p_flv_list = flv_list_get_list();
		
		#ifdef CONVERT_FLVLIST_URL		
		flv_list_convert(p_flv_list);
		#endif
		
		iret = flv_list_get_seek_info( p_flv_list, (double)(pMsg->MsgData.Common.param1), &index, &msec_in_part);
		if( iret != 0)
		{
			DAL_ERROR(("flv_list_get_seek_info failed\n"));
			//dal_core_event_notify( DAL_CORE_EVENT_SEEK_FAILED, 0);
			return ;
		}

		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "index: %u    msec_in_part: %f\n", index, msec_in_part);

		if( index == p_flv_list->cur_play)
		{
			error = PLAYREC_PlaySeek( 0, (int)(msec_in_part*1000), 0);
			if( error != ST_NO_ERROR)
			{
				DAL_ERROR(("PLAYREC_PlaySeek failed\n"));
				//dal_core_event_notify( DAL_CORE_EVENT_SEEK_FAILED, 0);
				return ;
			}
		}
		else
		{
			iret = DAL_PlayrecStop();
			if( iret != 0)
			{
				DAL_ERROR((" ????? DAL_PlayrecStop failed\n"));
				return ;
			}

			p_flv_list->cur_play = index;
			
#ifdef CONVERT_FLVLIST_URL			
			flv_list_convert(p_flv_list);
#endif

			memset( p_control->input.Source.http.URL, 0, sizeof(p_control->input.Source.http.URL));
			strcpy( (char*)p_control->input.Source.http.URL, p_flv_list->list[p_flv_list->cur_play].url);
			iret = DAL_PlayrecStartWithFile( &(p_control->input), &(p_control->output));
			if( iret != 0)
			{
				DAL_ERROR(("DAL_PlayrecStartWithFile failed\n"));
				return ;
			}
			error = PLAYREC_PlaySeek( 0, (int)(msec_in_part*1000), 0);
			if( error != ST_NO_ERROR)
			{
				DAL_ERROR(("PLAYREC_PlaySeek failed\n"));
				return ;
			}			
		}
	}
	else
	{
		DAL_ERROR(("illegal SourceType!!!\n"));
		return ;
	}
#endif

}

void dal_stream_seek_prev_proc(DAL_Message_t* pMsg)
{
#ifdef FLV
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	flv_play_list_t*			p_flv_list = flv_list_get_list();
	char						url[FLV_URL_MAX_LEN];
	DAL_Message_t*				p_msg = NULL;
	dal_evt_param_t				dal_evt_param;
	int							iret = 0;

	if( p_control->input.SourceType == SourceType_FLV_LIST)
	{

		// 按自然段
		/*if current play the first part, don't need seek*/
		if( p_flv_list->cur_play == 0){
			dal_core_event_notify( DAL_CORE_EVENT_SEEK_ALREADY_FIRST, &dal_evt_param);
			p_control->flvlist_in_seek_proc = FALSE;
			return ;
		}
		p_control->flvlist_in_seek_proc = TRUE;

		// seek action start notify
		DAL_FlvList_PlayGetPlayTime(&dal_evt_param.total_time, &dal_evt_param.cur_time);
		DAL_DEBUG(("DAL_FlvList_PlayGetPlayTime:  total_time: %u  cur_time: %u \n",dal_evt_param.total_time,dal_evt_param.cur_time));
		dal_core_event_notify(DAL_CORE_EVENT_SEEK_ACTION_START,&dal_evt_param);

		/*get next part url */
		p_flv_list->cur_play--;
		memset(url,0,sizeof(url));
		iret = flv_list_get_url_by_index( p_flv_list, p_flv_list->cur_play, url);
		if( iret != 0){
			DAL_ERROR(("flv_list_get_url_by_index failed\n"));
			dal_core_event_notify( DAL_CORE_EVENT_SEEK_FAILED, &dal_evt_param);
			p_control->flvlist_in_seek_proc = FALSE;
			return ;
		}
		memset( p_control->input.Source.http.URL, 0, sizeof(p_control->input.Source.http.URL));
		strcpy( (char*)p_control->input.Source.http.URL, url);
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "  %s\n",(char*)p_control->input.Source.http.URL);

		/*stop the current part play*/
		iret = DAL_PlayrecStop();
		if( iret != 0){
			DAL_ERROR((" ????? DAL_PlayrecStop failed\n"));
			//dal_core_event_notify( DAL_CORE_EVENT_SEEK_FAILED, &dal_evt_param);
			//return ;
		}

#ifdef USE_ST_PLAYREC		
		/*start play prev part*/
		//PLAYREC_SET_PLAY_FLVLIST(1);
		PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_ffmpeg);
		iret = DAL_PlayrecStartWithFile( &(p_control->input), &(p_control->output));
		if( iret != 0)
		{
			p_control->flvplay_try_times = 1;
			SYS_DEBUG(SD_WARNING, MODULE_DAL_CORE,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  DAL_PlayrecStartWithFile failed %d times\n", p_control->flvplay_try_times);
			PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_none);
			
			dal_core_set_status(CORE_STREAM_WAIT_FLVPLAY);
			p_msg = DAL_MessageAlloc();
			p_msg->MsgType = DALMSG_WIAT_RETRY_FLVPLAY;
			DAL_MessageSend( DAL_GetCoreQueueID(), &p_msg);
			p_control->flvlist_in_seek_proc = FALSE;
			return ;
		}
		PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_none);
#else
// ?hisi
#endif
		/* 播放开始后，启动第一帧检测*/
		if( CORE_STREAM_PAUSE == p_control->status){
			dal_core_check_frame_start(FF_ACTION_SET_PAUSE);
		}
		else{
			dal_core_check_frame_start(FF_ACTION_DISPLAY_PROGRESS);
		}
		dal_core_set_status(CORE_STREAM_START);
		//dal_core_event_notify( DAL_CORE_EVENT_SEEK_OK, &dal_evt_param); //这里改用SEEK_ACTION_DONE 来通知上层
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "seek prev part ok!");
		p_control->flvlist_in_seek_proc = FALSE;
	}
#endif
}

void dal_stream_seek_next_proc(DAL_Message_t* pMsg)
{
#ifdef FLV
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	flv_play_list_t*			p_flv_list = flv_list_get_list();;
	char						url[FLV_URL_MAX_LEN];
	DAL_Message_t*				p_msg = NULL;
	dal_evt_param_t				dal_evt_param;
	int							iret = 0;

	if( p_control->input.SourceType == SourceType_FLV_LIST)
	{

		/*if current play the next part, don't need seek*/
		if( p_flv_list->cur_play == p_flv_list->num - 1){
			dal_core_event_notify( DAL_CORE_EVENT_SEEK_ALREADY_LAST, &dal_evt_param);
			p_control->flvlist_in_seek_proc = FALSE;
			return ;
		}

		p_control->flvlist_in_seek_proc = TRUE;

		// seek action start notify
		DAL_FlvList_PlayGetPlayTime(&dal_evt_param.total_time, &dal_evt_param.cur_time);
		dal_core_event_notify(DAL_CORE_EVENT_SEEK_ACTION_START,&dal_evt_param);


		/*get next part url */
		p_flv_list->cur_play++;
		memset(url,0,sizeof(url));
		iret = flv_list_get_url_by_index( p_flv_list, p_flv_list->cur_play, url);
		if( iret != 0){
			DAL_ERROR(("flv_list_get_url_by_index failed\n"));
			dal_core_event_notify( DAL_CORE_EVENT_SEEK_FAILED, &dal_evt_param);
			p_control->flvlist_in_seek_proc = FALSE;
			return ;
		}
		memset( p_control->input.Source.http.URL, 0, sizeof(p_control->input.Source.http.URL));
		strcpy( (char*)p_control->input.Source.http.URL, url);
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "  %s\n",(char*)p_control->input.Source.http.URL);

		/*stop the current part play*/
		iret = DAL_PlayrecStop();
		if( iret != 0){
			DAL_ERROR((" ????? DAL_PlayrecStop failed\n"));
			//dal_core_event_notify( DAL_CORE_EVENT_SEEK_FAILED, &dal_evt_param);
			//return ;
		}
#ifdef USE_ST_PLAYREC		
		/*start play next part*/
		//PLAYREC_SET_PLAY_FLVLIST(1);
		PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_ffmpeg);
		iret = DAL_PlayrecStartWithFile( &(p_control->input), &(p_control->output));
		if( iret != 0)
		{
			p_control->flvplay_try_times = 1;
			SYS_DEBUG(SD_WARNING, MODULE_DAL_CORE,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  DAL_PlayrecStartWithFile failed %d times\n", p_control->flvplay_try_times);
			PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_none);
			
			dal_core_set_status(CORE_STREAM_WAIT_FLVPLAY);
			p_msg = DAL_MessageAlloc();
			p_msg->MsgType = DALMSG_WIAT_RETRY_FLVPLAY;
			DAL_MessageSend( DAL_GetCoreQueueID(), &p_msg);
			p_control->flvlist_in_seek_proc = FALSE;
			return ;
		}
		PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_none);
#else
// ?hisi
#endif
		/* 播放开始后，启动第一帧检测*/
		if( CORE_STREAM_PAUSE == p_control->status){
			dal_core_check_frame_start(FF_ACTION_SET_PAUSE);
		}
		else{
			dal_core_check_frame_start(FF_ACTION_DISPLAY_PROGRESS);
		}

		dal_core_set_status(CORE_STREAM_START);
		//dal_core_event_notify( DAL_CORE_EVENT_SEEK_OK, &dal_evt_param);  //这里改用SEEK_ACTION_DONE 来通知上层
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "seek next part ok!");
		p_control->flvlist_in_seek_proc = FALSE;

	}
#endif	
}

void dal_stream_play_next_proc(DAL_Message_t* pMsg)
{

	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "NOT SUPPORT YET");

#if 0
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	flv_play_list_t*			p_flv_list;
	char						url[FLV_URL_MAX_LEN];
	int							iret = 0;

	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "Entry");
	if( p_control->input.SourceType == SourceType_FLV_LIST )
	{
		/*因为playrec内部已经续播，并且修改p_flv_list->cur_play 的值，
		所以这时一定是所有片段播放完成了*/
		PLAYREC_SET_PLAY_FLVLIST(0);
		
		p_flv_list = flv_list_get_list();
		if( p_flv_list->cur_play >= p_flv_list->num - 1)
		{
			SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "b DAL_PlayrecStop");
			iret = DAL_PlayrecStop();
			if( iret != 0)
			{
				DAL_ERROR((" ????? DAL_PlayrecStop failed\n"));
				return ;
			}
		}
		else
		{
			SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "b1 DAL_PlayrecStop");
			iret = DAL_PlayrecStop();
			if( iret != 0)
			{
				DAL_ERROR((" ????? DAL_PlayrecStop failed\n"));
				return ;
			}
			
			p_flv_list->cur_play++;

#ifdef CONVERT_FLVLIST_URL
			flv_list_convert(p_flv_list);
#endif

			memset(url,0,sizeof(url));
			iret = flv_list_get_url_by_index( p_flv_list, p_flv_list->cur_play, url);
			if( iret != 0)
			{
				DAL_ERROR(("flv_list_get_url_by_index failed\n"));
				return ;
			}
			
			memset( p_control->input.Source.http.URL, 0, sizeof(p_control->input.Source.http.URL));
			strcpy( (char*)p_control->input.Source.http.URL, url);

			SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "b1 DAL_PlayrecStartWithFile  cur_play: %d\n", p_flv_list->cur_play);
			SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "b1 DAL_PlayrecStartWithFile  url: %s\n", p_flv_list->list[p_flv_list->cur_play].url);

			PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_ffmpeg);
			iret = DAL_PlayrecStartWithFile( &(p_control->input), &(p_control->output));
			if( iret != 0)
			{
				DAL_ERROR(("DAL_PlayrecStartWithFile failed\n"));
				return ;
			}
			PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_none);
		}
	}
	else
	{
		DAL_ERROR(("illegal SourceType!!!\n"));
		return ;
	}
#endif
}

void dal_stream_getinfo_proc(DAL_Message_t* pMsg)
{
	/*
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	if( p_control->input.SourceType == SourceType_FLV_LIST )
	{
		
	}
	*/
	// 目前getinfo 操作不需要异步执行
}

void dal_new_pids_proc(DAL_Message_t* pMsg)
{
	DAL_CoreControl_t*		p_control = &g_CoreControl;
	dal_evt_param_t			dal_evt_param;
	DAL_Message_t*			p_msg = NULL;
	int						iret = 0;
	int						i = 0;
	
	if( p_control->status != CORE_STREAM_WAIT_PID )
	{
		//DAL_ERROR(("NOT CORE_STREAM_WAIT_PID\n"));
		DebugMessage("[dal]dal_new_pids_proc -> NOT CORE_STREAM_WAIT_PID\n");
		dal_evt_param.status = (U32)p_control->status;
		dal_core_event_notify(DAL_CORE_EVENT_NEW_PIDS_BAD_STATUS,&dal_evt_param);
		return ;
	}

//	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "CORE_STREAM_WAIT_PID");
	DebugMessage("[dal]dal_new_pids_proc -> CORE_STREAM_WAIT_PID\n");

	#ifdef P2S_SWITCH_PAUSE_VIDEO
	if( p_control->playrec_start_flag == 1)
	{
		iret = DAL_PlayrecStop();
		if( iret != 0)
		{
	//		DAL_ERROR(("DAL_PlayrecStop failed\n"));
			DebugMessage("[dal]dal_new_pids_proc -> DAL_PlayrecStop failed");
			dal_core_set_status(CORE_STREAM_STOP);
			return ;
		}
	}
	p_control->playrec_start_flag = 0;
	#endif	

	/*			1.ts
		p_control->input.Pids[0].type = 2;
		p_control->input.Pids[0].pid = 256;
		p_control->input.Pids[1].type = 3;
		p_control->input.Pids[1].pid = 272;
	*/

	p_control->input.PidsNum = pMsg->MsgData.PidsParam.PidsNum;
	for( i = 0; i < p_control->input.PidsNum; i++)
	{
		p_control->input.Pids[i].type = pMsg->MsgData.PidsParam.Pids[i].type;
		p_control->input.Pids[i].pid = pMsg->MsgData.PidsParam.Pids[i].pid;
	}

	//p_control->input.SourceType = SourceType_UDP;
	iret = DAL_PlayrecStartWithDemux( &(p_control->input), &(p_control->output));
	if( iret != DAL_NO_ERROR)
	{
//		DAL_ERROR(("DAL_PlayrecStartWithDemux failed\n"));
		DebugMessage("[dal]dal_new_pids_proc -> DAL_PlayrecStartWithDemux failed");
		DAL_InjectStop();
		DAL_ParseStop();
		DAL_DownloadStop();
		DAL_DownloadBufferRelease();
		dal_core_set_status(CORE_STREAM_STOP);
		dal_core_event_notify(DAL_CORE_EVENT_PLAY_PIDS_FAILED, &dal_evt_param);
		return ;
	}
#ifdef USE_ST_PLAYREC
	#ifdef P2S_SWITCH_PAUSE_VIDEO	
	p_control->playrec_start_flag = 1;
	#endif
#endif

	iret =Set_DSC_key();
	DebugMessage("[Set_DSC_key]Set_DSC_key -> >>>>>>>>>>>>>>>>>>>>>>>>>>> %d",iret);



	if( DAL_InjectIsPause())
	{
	
		DebugMessage("[dal]dal_new_pids_proc -> >>>>>>>>>>>>>>>>>>>>>>>>>>> injectResume");
		iret = DAL_InjectResume();
	}
	else
	{
		iret = DAL_InjectStart();
		
		dal_core_check_frame();
	}
	if( iret != 0)
	{
		//DAL_ERROR(("DAL_InjectStart failed\n"));
		DebugMessage("[dal]dal_new_pids_proc -> DAL_InjectStart failed");
		DAL_PlayrecStop();
		DAL_InjectStop();
		DAL_ParseStop();
		DAL_DownloadStop();
		DAL_DownloadBufferRelease();
		dal_core_set_status(CORE_STREAM_STOP);
		dal_core_event_notify(DAL_CORE_EVENT_PLAY_INJECT_FAILED, &dal_evt_param);
		return ;
	}
	
	if(p_control->p2v_in_seek_proc == TRUE)
		p_control->p2v_in_seek_proc =FALSE;
	if(p_control->timeshift_in_seek_proc == TRUE)
		p_control->timeshift_in_seek_proc =FALSE;
#if defined(USE_DAL_SUBT)
	//{{{ add by LYN, for subtitle, 2011-08-25
	{
	    U32 Subtitle_language;
	    U16 Subtitle_composition_pid;
	    U16 Subtitle_ancillary_pid;
	    U16 Subtitle_pid;
		#if 1
	    if(1 == Get_SubFlag())
	    {
	        Subtitle_pid = Get_SubtPid();
	        Get_SubData(0,&Subtitle_language,&Subtitle_composition_pid,&Subtitle_ancillary_pid);
	        printf("-----------LYN::subtiltepid = %d\n",Subtitle_pid);
	        SYS_DVBSubt_Start(Subtitle_pid,Subtitle_composition_pid,Subtitle_ancillary_pid);
	    }
		#endif
	}
	//USM_DrawRectangle(0, 0, 1920, 1080, 0xffffffff,1, 0, 0, 1);
	//}}} add by LYN, for subtitle, 2011-08-25
#endif

	/* 播放开始后，启动第一帧检测*/
//	dal_core_check_frame_start(FF_ACTION_CLEAR_NOTIFY);

	dal_core_set_status(CORE_STREAM_START);
	dal_core_event_notify(DAL_CORE_EVENT_PLAY_PIDS_OK, &dal_evt_param);
	//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "P2S PlatStart and Inject OK");
	DebugMessage("[dal]dal_new_pids_proc -> P2S PlayStart and Inject OK");
}
static U32 DescramblerHandle_V=0;
static U32 DescramblerHandle_A=0;

int Set_DSC_key(void)
{
	int  ret;
	U8 key[8]={0x12,0x34,0x56,0x78,0x90,0x12,0x34,0x56};
	U32 vPid= DRV_AVCtrl_Vid_GetPID();
	U32 aPid= DRV_AVCtrl_Aud_GetPID();
	

	//AM_DSC_Source_t src =AM_DSC_SRC_DMX0;	
	ret= AM_DSC_SetSource(0,0);//demux 0
	if(ret != AM_SUCCESS)
	{
		pbiinfo("[Set_DSC_key] %s	%d	error %x ",__FUNCTION__,__LINE__,ret);
		return 1;
	}
	ret = AM_DSC_AllocateChannel( 0, &DescramblerHandle_V);
	if ( ret != AM_SUCCESS)
	{
		pbierror("[Set_DSC_key] Descrambler_Allocate video failed!!\n*************\n");
		return 2;
	}
	ret = AM_DSC_AllocateChannel( 0, &DescramblerHandle_A);
	if ( ret != AM_SUCCESS)
	{
		pbierror("[Set_DSC_key] Descrambler_Allocate audio failed!!\n*************\n");
		return 2;
	}

	ret = AM_DSC_SetChannelPID(0, (int)DescramblerHandle_V, vPid);
	if ( ret != AM_SUCCESS)
	{
		pbierror("[Set_DSC_key] AM_DSC_SetChannelPID video failed!! ==0x%x\n*************\n",ret);
		return 3;
	}
	
	ret = AM_DSC_SetChannelPID(0, (int)DescramblerHandle_A, aPid);
	if ( ret != AM_SUCCESS)
	{
		pbierror("[Set_DSC_key] AM_DSC_SetChannelPID audio failed!! ==0x%x\n*************\n",ret);
		return 3;
	}
	ret = AM_DSC_SetKey(0, (int)DescramblerHandle_V, 0,key );//奇数
	if(ret != AM_SUCCESS)
	{
		pbierror("[Set_DSC_key] AM_DSC_SetevenKey( ) video failed!!\n *************\n");
		return 4;
	}
	ret = AM_DSC_SetKey(0, (int)DescramblerHandle_V, 1,key );//偶数
	if(ret != AM_SUCCESS)
	{
		pbierror("[Set_DSC_key] AM_DSC_SetoddKey( ) video failed!!\n *************\n");
		return 4;
	}
	
	ret = AM_DSC_SetKey(0, (int)DescramblerHandle_A, 0,key );
	if(ret != AM_SUCCESS)
	{
		pbierror("[Set_DSC_key] AM_DSC_SetevenKey( ) audio failed!!\n *************\n");
		return 4;
	}
	ret = AM_DSC_SetKey(0, (int)DescramblerHandle_A, 1,key );
	if(ret != AM_SUCCESS)
	{
		pbierror("[Set_DSC_key] AM_DSC_SetoddKey( ) audio failed!!\n *************\n");
		return 4;
	}
	pbierror("[Set_DSC_key] AM_DSC_SetKey   ok \n");
	return 0;
}

int del_DSC_key(void)
{
	int  ret;

	ret = AM_DSC_FreeChannel(0, (int)DescramblerHandle_V);
	if ( ret != AM_SUCCESS)
	{
		pbierror("[del_DSC_key] AM_DSC_FreeChannel video failed!! ==0x%x\n*************\n",ret);
		return 1;
	}	
	ret = AM_DSC_FreeChannel(0, (int)DescramblerHandle_A);
	if ( ret != AM_SUCCESS)
	{
		pbierror("[del_DSC_key] AM_DSC_FreeChannel audio failed!! ==0x%x\n*************\n",ret);
		return 1;
	}	

	return 0;
}
void dal_pids_changed_proc(DAL_Message_t* pMsg)
{
	DAL_CoreControl_t*		p_control = &g_CoreControl;
	dal_evt_param_t			dal_evt_param;
	int						iret = 0;
	int						i = 0;

	if( p_control->status != CORE_STREAM_START)
	{
		//DAL_ERROR(("NOT CORE_STREAM_START\n"));
		DebugMessage("[dal]dal_pids_changed_proc ->NOT CORE_STREAM_START");
		dal_evt_param.status = (U32)p_control->status;
		dal_core_event_notify(DAL_CORE_EVENT_PIDS_CHANGED_BAD_STATUS,&dal_evt_param);
		return ;
	}
//	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "CORE_STREAM_START");
	DebugMessage("[dal]dal_pids_changed_proc -> CORE_STREAM_START");
#if 0
	iret = DAL_PlayrecStop();
	if( iret != 0)
	{
		//DAL_ERROR(("DAL_PlayrecStop failed\n"));	
		DebugMessage("[dal]dal_pids_changed_proc ->DAL_PlayrecStop failed");
		dal_core_set_status(CORE_STREAM_STOP);
		dal_core_event_notify(DAL_CORE_EVENT_PID_CHANGED_FAILED, &dal_evt_param);	
		return ;
	}
#endif	
	iret = DAL_InjectPause();
	if( iret != 0)
	{
		//DAL_ERROR(("DAL_InjectPause failed\n"));
		DebugMessage("[dal]dal_pids_changed_proc ->DAL_InjectPause failed");
		dal_core_set_status(CORE_STREAM_STOP);
		dal_core_event_notify(DAL_CORE_EVENT_PID_CHANGED_FAILED, &dal_evt_param);
		return ;
	}
	
	p_control->input.PidsNum = pMsg->MsgData.PidsParam.PidsNum;
	for( i = 0; i < p_control->input.PidsNum; i++)
	{
		p_control->input.Pids[i].type = pMsg->MsgData.PidsParam.Pids[i].type;
		p_control->input.Pids[i].pid = pMsg->MsgData.PidsParam.Pids[i].pid;
	}
	
	iret = DAL_PlayrecStartWithDemux( &(p_control->input), &(p_control->output));
	if( iret != DAL_NO_ERROR)
	{
		//DAL_ERROR(("DAL_PlayrecStartWithDemux failed\n"));
		DebugMessage("[dal]dal_pids_changed_proc ->DAL_PlayrecStartWithDemux failed");
		dal_core_set_status(CORE_STREAM_STOP);
		dal_core_event_notify(DAL_CORE_EVENT_PID_CHANGED_FAILED, &dal_evt_param);	
		return ;
	}
	if( DAL_InjectIsPause())
	{
		iret = DAL_InjectResume();			
	}
	else
	{
		iret = DAL_InjectStart();
	}
	if( iret != 0)
	{
	//	DAL_ERROR(("DAL_InjectStart failed\n"));
		DebugMessage("[dal]dal_pids_changed_proc ->DAL_InjectStart failed");
		DAL_PlayrecStop();
		dal_core_set_status(CORE_STREAM_STOP);
		dal_core_event_notify(DAL_CORE_EVENT_PID_CHANGED_FAILED, &dal_evt_param);
		return ;
	}

	dal_core_event_notify(DAL_CORE_EVENT_PID_CHANGED_OK, &dal_evt_param);	

	//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "P2S Change Pids Restart Playrec and Inject OK\n");	
	DebugMessage("[dal]dal_pids_changed_proc ->P2S Change Pids Restart Playrec and Inject OK");
}



void dal_stream_data_timeout_proc(DAL_Message_t* pMsg)
{
	dal_evt_param_t		dal_evt_param;
	dal_core_event_notify(DAL_CORE_EVENT_DATA_INTERRUPT,&dal_evt_param);
}

void dal_stream_data_resume_proc(DAL_Message_t* pMsg)
{
	dal_evt_param_t		dal_evt_param;
	dal_core_event_notify(DAL_CORE_EVENT_DATA_RESUME,&dal_evt_param);
}


void dal_stream_first_frame_display_proc(DAL_Message_t* pMsg)
{
	DAL_CoreControl_t*		p_control = &g_CoreControl;
	dal_evt_param_t			dal_evt_param;
	int						iret = 0;

	//sys_print_time(0,"dal_stream_first_frame_display_proc");
	if( FF_ACTION_SET_PAUSE == p_control->first_frame_action)
	{
#ifdef FLV		
		if( SourceType_FLV_LIST == p_control->input.SourceType)
		{
			iret = PLAYREC_PlayPause(0);
			if( iret != ST_NO_ERROR)
			{
				DAL_ERROR(("PLAYREC_PlayPause failed\n"));	
				dal_core_set_status(CORE_STREAM_START);
				dal_core_event_notify(DAL_CORE_EVENT_SEEK_PAUSE_FAILED,&dal_evt_param);
				return ;
			}
		}
#endif		
	}
	else if( FF_ACTION_CLEAR_NOTIFY == p_control->first_frame_action){
		dal_core_event_notify(DAL_CORE_EVENT_FRIST_FRAME_DISPLAY,&dal_evt_param);	
	}	
	else if( FF_ACTION_CLEAR_BANNER == p_control->first_frame_action){
		dal_core_event_notify(DAL_CORE_EVENT_FRIST_FRAME_DISPLAY,&dal_evt_param);	// 用于给 dvb  节目清除banner
	}
	else if( FF_ACTION_DISPLAY_PROGRESS == p_control->first_frame_action){
		dal_core_event_notify(DAL_CORE_EVENT_FRIST_FRAME_DISPLAY,&dal_evt_param);
		//dal_core_event_notify( DAL_CORE_EVENT_SEEK_OK, &dal_evt_param);	// seek 之后，第一帧呈现后才进行进度显示
	}
}

void dal_stream_check_first_frame_proc(DAL_Message_t* pMsg)
{
	DAL_CoreControl_t*		p_control = &g_CoreControl;
	unsigned int			video_frame_displayed = 0;
	DAL_Message_t*			pmsg = NULL;
	dal_evt_param_t			dal_evt_param;
	int						iret = 0;

	if( p_control->checking_first_frame && !(p_control->first_frame_display) )
	{
		video_frame_displayed = get_video_frame_display_count(); 
		if( 0 == video_frame_displayed)
		{
			pmsg = DAL_MessageAlloc();
			pmsg->MsgType = DALMSG_CHECK_FIRST_FRAME;
			DAL_MessageSend( DAL_GetCoreQueueID(), &pmsg);
			//SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "---- ---- ---- aiting for frist frame display!!!\n");	
			DebugMessage("[dal]dal_stream_check_first_frame_proc ->waiting for frist frame display!!!");
		}
		else
		{
			/* 这里不再发送消息，直接执行第一帧呈现的事件回调，*//*
			pmsg = DAL_MessageAlloc();
			pmsg->MsgType = DALMSG_FIRST_FRAME_DISPLAY;
			DAL_MessageSend( DAL_GetCoreQueueID(), &pmsg);
			p_control->first_frame_display = TRUE;
			p_control->checking_first_frame = FALSE;
			SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "send mes to dal core frist frame display!!!  video_frame_display %d\n", video_frame_displayed);
			*/

		
			//sys_print_time(0,"dal_stream_check_first_frame_proc DALMSG_FIRST_FRAME_DISPLAY");
			//DAL_DEBUG(("p_control->first_frame_action: %d    video_frame_displayed: %d\n", p_control->first_frame_action, video_frame_displayed));
			DebugMessage("[dal]dal_stream_check_first_frame_proc ->p_control->first_frame_action: %d    video_frame_displayed: %d\n", p_control->first_frame_action, video_frame_displayed);


		
			if( FF_ACTION_SET_PAUSE == p_control->first_frame_action)
			{
	#ifdef FLV		
				if( SourceType_FLV_LIST == p_control->input.SourceType)
				{
					iret = PLAYREC_PlayPause(0);
					if( iret != ST_NO_ERROR)
					{
						DAL_ERROR(("PLAYREC_PlayPause failed\n"));	
						dal_core_set_status(CORE_STREAM_START);
						dal_core_event_notify(DAL_CORE_EVENT_SEEK_PAUSE_FAILED,&dal_evt_param);
						return ;
					}
				}
		#endif		
			}
			else if( FF_ACTION_CLEAR_NOTIFY == p_control->first_frame_action){
				DAL_DEBUG(("    FF_ACTION_CLEAR_NOTIFY   >>\n"));
				dal_core_event_notify(DAL_CORE_EVENT_FRIST_FRAME_DISPLAY,&dal_evt_param);	
			}	
			else if( FF_ACTION_CLEAR_BANNER == p_control->first_frame_action){
				dal_core_event_notify(DAL_CORE_EVENT_FRIST_FRAME_DISPLAY,&dal_evt_param);	// 用于给 dvb  节目清除banner
			}
			else if( FF_ACTION_DISPLAY_PROGRESS == p_control->first_frame_action){
				DAL_DEBUG(("    FF_ACTION_DISPLAY_PROGRESS   >>\n"));
				dal_core_event_notify( DAL_CORE_EVENT_FRIST_FRAME_DISPLAY,&dal_evt_param);
				//dal_core_event_notify( DAL_CORE_EVENT_SEEK_OK, &dal_evt_param);	// seek 之后，第一帧呈现后才进行进度显示
			}

			p_control->first_frame_display = TRUE;
			p_control->checking_first_frame = FALSE;
		}
	}
}
#ifdef FLV
void dal_stream_flvlist_parse_ok_proc(DAL_Message_t* pMsg)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	flv_play_list_t*			p_flv_list =  flv_list_get_list();
	DAL_Message_t*				p_msg = NULL;
	dal_evt_param_t				dal_evt_param;
	int							iret = 0;
	char						url[FLV_URL_MAX_LEN];

	if( CORE_STREAM_WAIT_FLV_PARSE != p_control->status
		&& CORE_STREAM_START != p_control->status)
	{
		DAL_ERROR(("Receive flvlist parse ok ,but not in right status  : %s\n", dal_core_status_string(p_control->status)));
		/*zyliu 2011-12-21*/
		flv_list_parse_stop();
		/*zyliu 2011-12-21*/
		dal_evt_param.status = (U32)p_control->status;
		dal_core_event_notify(DAL_CORE_EVENT_FLV_PARSE_OK_BAD_STATUS,&dal_evt_param);
		return ;
	}
	
	if( DALMSG_FLVLIST_PARSE_OK == pMsg->MsgType){
		p_control->flvlist_parse_ok_flag = TRUE;
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ all flvlist parse ok!!!");
	}
	else if(DALMSG_FLVLIST_PARSE_ERROR_LATER_RETRY){
		SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ not all flvlist parse ok, but we will play start right now");
	}
	else{
		return ;
	}
		
	// if alreadt play start, do not need to play start again, return now
	if( CORE_STREAM_START == p_control->status){
	flv_list_debug( p_flv_list);
		return ;
	}

	/*从flvlist 中获取实际需要播放的视频地址*/
	memset(url,0,sizeof(url));
	iret = flv_list_get_url_by_index( p_flv_list, p_flv_list->cur_play, url);
	if( iret != 0)
	{
		DAL_ERROR(("flv_list_get_url_by_index failed\n"));
		dal_core_set_status(CORE_STREAM_STOP);
		dal_core_event_notify(DAL_CORE_EVENT_GET_LIST_URL_FAILED,&dal_evt_param);			
		return ;
	}

	memset( p_control->input.Source.http.URL, 0, sizeof(p_control->input.Source.http.URL));
	strcpy( (char*)p_control->input.Source.http.URL, url);

	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "will start play: ");
	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "%s\n",(char*)p_control->input.Source.http.URL);

	/*set the flvlist play flag*/
	PLAYREC_SET_PLAY_FLVLIST(1);
	
	/*设置播放器优先选择的demux */
	PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_ffmpeg);
	
	iret = DAL_PlayrecStartWithFile( &(p_control->input), &(p_control->output));
	if( iret != 0)
	{
		DAL_ERROR(("DAL_PlayrecStartWithFile failed\n"));
		PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_none);
		PLAYREC_SET_PLAY_FLVLIST(0);
		
		dal_core_set_status(CORE_STREAM_STOP);
		dal_core_event_notify(DAL_CORE_EVENT_PLAY_FLV_FAILED,&dal_evt_param);			
		return ;
	}
	PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_none);
	dal_core_set_status(CORE_STREAM_START);

	/* 播放开始后，启动第一帧检测*/
	dal_core_check_frame_start(FF_ACTION_CLEAR_NOTIFY);
	
	dal_core_event_notify(DAL_CORE_EVENT_PLAY_FLV_OK,&dal_evt_param);
	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "DAL FLV PLAY START");
}

void dal_stream_flvlist_parse_failed_proc(DAL_Message_t* pMsg)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t				dal_evt_param;
	
	dal_core_set_status(CORE_STREAM_STOP);
	dal_core_event_notify(DAL_CORE_EVENT_FLVLIST_PARSE_FAILED,&dal_evt_param);
}

void dal_stream_flvlist_parse_later_ok(DAL_Message_t* pMsg)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t				dal_evt_param;

	if( p_control->status != CORE_STREAM_START){
		DAL_ERROR(("Receive flvlist later parse ok ,but not in right status\n"));
		//dal_core_event_notify(DAL_CORE_EVENT_FLV_PARSE_OK_BAD_STATUS,(U32)p_control->status);
		return ;
	}else{
		p_control->flvlist_parse_ok_flag = TRUE;
	}
}

void dal_stream_flvlist_seek_action_done_proc(DAL_Message_t* pMsg)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t				dal_evt_param;

	if(p_control->flvlist_in_seek_proc){
		dal_core_event_notify(DAL_CORE_EVENT_SEEK_ACTION_DONE,&dal_evt_param);
		p_control->flvlist_in_seek_proc = FALSE;
	}
}
#endif
void dal_stream_dvb_check_frist_frame_proc(DAL_Message_t* pMsg)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	DAL_Message_t*				p_msg;

	reset_video_frame_display_count();
	dal_core_check_frame_start(FF_ACTION_CLEAR_BANNER);	
}

void dal_stream_recovery_proc(DAL_Message_t* pMsg)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	DAL_Player_Input_t			input;
	DAL_Player_Output_t			output;
	int							iret = 0;
	
	if( CORE_STREAM_START != p_control->status)
	{
		DAL_ERROR((" CORE_STREAM_START != p_control->status  \n"));
		return ;
	}
	if( (p_control->input.SourceType != SourceType_P2S)
		&&(p_control->input.SourceType != SourceType_P2V))
	{
		DAL_ERROR((" p_control->input.SourceType != SourceType_P2S  \n"));
		return ;
	}

	memset( &input, 0, sizeof(DAL_Player_Input_t));
	memset( &output, 0, sizeof(DAL_Player_Output_t));

	memcpy( &input, &(p_control->input), sizeof(DAL_Player_Input_t));
	memcpy( &output, &(p_control->output), sizeof(DAL_Player_Output_t));

	p_control->recovery_flag = 1;
	iret = DAL_P2S_PlayRestart(&input, &output);
	if( iret != 0)
	{
		DAL_ERROR(("DAL_P2S_PlayRestart failed\n"));
	}

}

#ifdef FLV
void dal_stream_wait_retry_flvplay_proc(DAL_Message_t* pMsg)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	DAL_Message_t*				p_msg;
	dal_evt_param_t				dal_evt_param;
	int							iret = 0;
	
	if( p_control->status != CORE_STREAM_WAIT_FLVPLAY){
		DAL_ERROR(("bad status, NOT CORE_STREAM_WAIT_FLVPLAY, but %s\n", dal_core_status_string(p_control->status)));
		return ;
	}

	DAL_TaskDelay(1000);
	PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_ffmpeg);
	iret = DAL_PlayrecStartWithFile( &(p_control->input), &(p_control->output));
	if( iret != 0)
	{
		p_control->flvplay_try_times++;
		SYS_DEBUG(SD_WARNING, MODULE_DAL_CORE,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  DAL_PlayrecStartWithFile failed %d times\n", p_control->flvplay_try_times);
		if( p_control->flvplay_try_times <= FLV_MAX_RETRY_TIMES){
			dal_core_set_status(CORE_STREAM_WAIT_FLVPLAY);
			p_msg = DAL_MessageAlloc();
			p_msg->MsgType = DALMSG_WIAT_RETRY_FLVPLAY;
			DAL_MessageSend( DAL_GetCoreQueueID(), &p_msg);
		}
		else{
			p_control->flvplay_try_times = 0;
			dal_core_set_status(CORE_STREAM_STOP);
			dal_core_event_notify( DAL_CORE_EVENT_SEEK_FAILED,&dal_evt_param);
		}
		PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_none);
		return ;
	}
	PLAYREC_SET_STSTREAM_FAVO_DEMUX(playrec_demux_none);

	/* 播放开始后，启动第一帧检测*/
	if( CORE_STREAM_PAUSE == p_control->status){
		dal_core_check_frame_start(FF_ACTION_SET_PAUSE);
	}
	else{
		dal_core_check_frame_start(FF_ACTION_NONE);
	}
	dal_core_event_notify( DAL_CORE_EVENT_SEEK_OK, &dal_evt_param);
	SYS_DEBUG(SD_DEBUG, MODULE_DAL_CORE, "Flv retry play start success!\n");
}

#endif

#if 0
void flvlist_playstart_test(void){


	flv_play_list_t*		p_flv_list = NULL;
	flv_play_list_t*		p_flv_list_real = NULL;
	int 					iret = 0;
//int i =0;
//printf("flvlist_playstart_test entry\n");

//for( i = 0; i < 10; i++){
	//
	//sleep(1);
	//printf(" <> %d\n",i);
//}

	p_flv_list = flv_list_get_list_1();
	memset(p_flv_list,0,sizeof(flv_play_list_t));

	flv_list_add_url( p_flv_list, "http://data.video.qiyi.com/videos/movie/20110304/421d516cb389a5f88e51a8059910a02d.f4v");
	flv_list_add_url( p_flv_list, "http://data.video.qiyi.com/videos/movie/20110304/a8665c3e55f9b000b0e315884fed075f.f4v");
	//flv_list_add_url( p_flv_list, "http://data.video.qiyi.com/videos/movie/20110304/2feed001b587fbcaae0abc9e1c56a626.f4v");
	//flv_list_add_url( p_flv_list, "http://data.video.qiyi.com/videos/movie/20110304/c5c0da840a6369f713ecdea112d4e5fe.f4v");
	//flv_list_add_url( p_flv_list, "http://data.video.qiyi.com/videos/movie/20110304/fe53871dc6a807888f7eb62de8b88d6f.f4v");
	//flv_list_add_url( p_flv_list, "http://data.video.qiyi.com/videos/movie/20110304/17a041161495dbbfb48982758f3c6930.f4v");
	//flv_list_add_url( p_flv_list, "http://data.video.qiyi.com/videos/movie/20110304/e821a81494363ded8b19aeb5cfdfb9f6.f4v");

	p_flv_list_real = flv_list_get_list();
	iret = flv_list_convert( p_flv_list_real);
	if( iret != 0)
	{
		printf("flv_list_convert failed\n");
		return -1;
	}

	flv_list_debug(p_flv_list_real);
	js_play_start(3, NULL);
	return 0;
}
#endif

void reset_video_frame_display_count(void)
{

}
int	 get_video_frame_display_count(void)
{
	S32 Error=0;
	DRV_AVCTRL_STATUS_AVCOUNT_INFO_t sCountInfo;
	Error = DRV_AVCtrl_Get_Status_Info(&sCountInfo);
	if(Error!=0)
	{
		DebugMessage("[dal]get_video_frame_display_count ->DRV_AVCtrl_Get_Status_Info error:%d",Error);
		return -1;
	}
	return sCountInfo.u32VidFrameCount;
}


S32 DAL_hls_PlayStart( DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg)
	{
		memset(hls_http,0,sizeof(hls_http));
		pMsg->MsgType = DALMSG_STREAM_START;
	//	memcpy( &(pMsg->MsgData.DownloadStart.url),(char*)input->Source.http.URL,sizeof(input->Source.http.URL));
		memcpy( hls_http, (char*)input->Source.http.URL,sizeof(input->Source.http.URL));
		memcpy( &(pMsg->MsgData.StreamParam.input), input, sizeof(DAL_Player_Input_t));
		memcpy( &(pMsg->MsgData.StreamParam.output), output, sizeof(DAL_Player_Output_t));
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}
	else
	{
		DebugMessage("DAL_MessageAlloc failed\n");
	}
	return 0;
}


S32 DAL_hls_PlayStop( void)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg)
	{
		pMsg->MsgType = DALMSG_STREAM_STOP;
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}
	else
	{
		DebugMessage("DAL_MessageAlloc failed\n");
	}
	return 0;
}

S32 DAL_HLS_PlayRestart( DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{
    DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg)
	{
		pMsg->MsgType = DALMSG_STREAM_RESTART;
		memcpy( &(pMsg->MsgData.StreamParam.input), input, sizeof(DAL_Player_Input_t));
		memcpy( &(pMsg->MsgData.StreamParam.output), output, sizeof(DAL_Player_Output_t));
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}
	else
	{
		DebugMessage("[DAL_HLS_PlayRestart] DAL_MessageAlloc failed\n");
	}
	return 0;
}

S32 m3u8_perpare_parse( void)
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t				dal_evt_param;
	S32 iret=0;

	
	/* start the pids parse task*/
	iret = DAL_ParseStart();
	if( iret != 0)
	{
		DebugMessage("[m3u8_perpare_parse] failed\n");
		return -1;
	}
	DebugMessage("[m3u8_perpare_parse] perpare OK");
	
#ifdef P2S_SWITCH_PAUSE_VIDEO	
	p_control->playrec_start_flag = 0;
#endif	
	
	
	/* set status CORE_STREAM_WAIT_PID, when patse task get new pids and notify core task, program will be play start*/
	dal_core_set_status(CORE_STREAM_WAIT_PID);
	dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);
	DebugMessage("[m3u8_perpare_parse] --> CORE_STREAM_WAIT_PID ");
	return 0;

}

U32 get_core_controlstatus(void)
{
	return g_CoreControl.status;

}

S32 DAL_P2V_seek_next(DAL_Player_Input_t* input,DAL_Player_Output_t* output)
{
    DAL_Message_t*		pMsg = NULL;
	DAL_CoreControl_t*			p_control = &g_CoreControl;

	if( TRUE == p_control->p2v_in_seek_proc)
	{
		/* DAL_Core 正在处理SEEK */
		DebugMessage("********************** DAL_Core is in seek proc, do not send msg to seek prev");
		return 0;
	}
	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg)
	{
		pMsg->MsgType = DALMSG_P2V_SEEK_NEXT;
				
		memcpy( &(pMsg->MsgData.StreamParam.input), input, sizeof(DAL_Player_Input_t));
		memcpy( &(pMsg->MsgData.StreamParam.output), output, sizeof(DAL_Player_Output_t));
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
		
		DebugMessage("[DAL_P2V_seek_next] --> DALMSG_P2V_SEEK_NEXT  %d",pMsg->MsgData.StreamParam.input.param);
		return 0;
	}
	else
	{
		DebugMessage("[DAL_P2V_seek_next] --> send message failed ");
		return -1;
	}


}
S32 DAL_P2V_seek_prev(DAL_Player_Input_t* input,DAL_Player_Output_t* output)
{
	DAL_Message_t*			pMsg = NULL;
	DAL_CoreControl_t*		p_control = &g_CoreControl;

	if( TRUE == p_control->p2v_in_seek_proc){
		/* DAL_Core 正在处理SEEK */
		DebugMessage("********************** DAL_Core is in seek proc, do not send msg to seek next");
		return 0;
 	}
	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg)
	{
		pMsg->MsgType = DALMSG_P2V_SEEK_PREV;
		
		memcpy( &(pMsg->MsgData.StreamParam.input), input, sizeof(DAL_Player_Input_t));
		memcpy( &(pMsg->MsgData.StreamParam.output), output, sizeof(DAL_Player_Output_t));
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
		
		DebugMessage("[DAL_P2V_seek_prev] --> DALMSG_P2V_SEEK_PREV  %d",pMsg->MsgData.StreamParam.input.param);
			DebugMessage("[DAL_P2V_seek_prev] --> %s ",input->Source.http.URL);
		return 0;
	}
	else
	{
		DebugMessage("[DAL_P2V_seek_prev] --> send message failed ");
		return -1;
	}


}

S32 P2V_seek_next(DAL_Message_t* pMsg)
{
	S32 iret=0;
	dal_download_param_t		download_param;
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t				dal_evt_param;
	C8* str=NULL;		
	S32 length=0,Slen=0;
	U32 temptime=0;
	C8 tmp[256],tmp1[256];
	
	U32 x=0,y=0;

	/* start the download task*/
	memset( &download_param, 0, sizeof(dal_download_param_t));
	
	p_control->p2v_in_seek_proc = TRUE;
	iret = P2V_pause();
	if(iret !=0 )
	{
		DebugMessage("[P2V_seek_next] -> -> P2V_seek_prev failed");
		return -1;

	}
	//加入偏移量

	memset(tmp,0,sizeof(tmp));
	memset(tmp1,0,sizeof(tmp1));
	
	strcpy(tmp,pMsg->MsgData.StreamParam.input.Source.http.URL);
	length=strlen(tmp);
	
	DebugMessage("[P2V_seek_next]:^^^^^%s^^^^%d",tmp,length);
	str=tmp+length-1;
	while(*str!='/')
	{
		str=str-1;
	}
	Slen=strlen(str);
	DebugMessage("[P2V_seek_next]:^^^^^%s^^^^%d",str,Slen);
		
	strncpy(download_param.url,tmp,(length-Slen+1));
	temptime = p2v_movie_playtime+pMsg->MsgData.StreamParam.input.param*1000;
	if(temptime >= p2v_movie_duration)
	{
		dal_core_event_notify(DAL_CORE_EVENT_P2V_SEEK_FILE_END,&dal_evt_param);
		DebugMessage("[P2V_seek_next]:seek movie end!");
		return 0;
	}
	//sprintf(tmp1,"offset=%d",temptime*p2v_movie_length/p2v_movie_duration);
	
	y=p2v_movie_length/p2v_movie_duration;
	x=temptime*y;
	
	DebugMessage("[P2V_seek_next]:time to %d !",temptime);
	
	sprintf(tmp1,"offset=%d",x);
	strcat(download_param.url,tmp1);
	strcat(download_param.url,str);
	
	DebugMessage("[P2V_seek_next]:^^^^^%s",download_param.url);
	
	SetP2vResumeflag(1);
	dal_core_event_notify(DAL_CORE_EVENT_SEEK_ACTION_START,&dal_evt_param);

	iret = DAL_DownloadBufferInit();
	if( iret != 0)
	{
		DebugMessage("[P2V_seek_next]->DAL_DownloadBufferInit failed\n");
		p_control->p2v_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_P2V_RESUME_DL_BUF_ALLOC_FAILED,&dal_evt_param);	
		return -1;
	}


	iret = DAL_DownloadStart( &download_param);
	if( iret != 0)
	{
		DebugMessage("[P2V_seek_next]->DAL_DownloadStart failed\n");
		DAL_DownloadBufferRelease();
		p_control->p2v_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_P2V_RESUME_DL_START_FAILED,&dal_evt_param);	
		return -1;
	}

	/* start the pids parse task*/
	iret = DAL_ParseStart();
	if( iret != 0)
	{
		DebugMessage("[P2V_seek_next]->DAL_ParseStart failed\n");
		DAL_DownloadStop();
		DAL_DownloadBufferRelease();
		p_control->p2v_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_P2V_RESUME_PIDS_PARSE_START_FAILED,&dal_evt_param);
		return -1;
	}
	DebugMessage("[DAL]P2V_seek_next OK");


	/* set status CORE_STREAM_WAIT_PID, when patse task get new pids and notify core task, program will be play start*/
	dal_core_set_status(CORE_STREAM_WAIT_PID);
	p_control->recovery_flag = 0;		
	dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);

	return 0;
}

S32 P2V_seek_prev(DAL_Message_t* pMsg)
{
	S32 iret=0;
	dal_download_param_t		download_param;
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t				dal_evt_param;
	C8* str=NULL;		
	S32 length=0,Slen=0;
	U32 temptime=0;
	C8 tmp[256],tmp1[256];
	U32 x=0,y=0;

	
	/* start the download task*/
	memset( &download_param, 0, sizeof(dal_download_param_t));
	//加入偏移量

	memset(tmp,0,sizeof(tmp));
	memset(tmp1,0,sizeof(tmp1));
	
	p_control->p2v_in_seek_proc = TRUE;
	iret = P2V_pause();
	if(iret !=0 )
	{
		DebugMessage("[P2V_seek_prev] -> -> P2V_seek_next failed");
		return -1;

	}
	
	strcpy(tmp,pMsg->MsgData.StreamParam.input.Source.http.URL);
		DebugMessage("[P2V_seek_prev]:^^^^^%s",pMsg->MsgData.StreamParam.input.Source.http.URL);
	length=strlen(tmp);
	
	DebugMessage("[P2V_seek_prev]:^^^^^%s^^^^%d",tmp,length);
	str=tmp+length-1;
	while(*str!='/')
	{
		str=str-1;
	}
	Slen=strlen(str);
	DebugMessage("[P2V_seek_prev]:^^^^^%s^^^^%d",str,Slen);
		
	strncpy(download_param.url,tmp,(length-Slen+1));
	if(p2v_movie_playtime<=pMsg->MsgData.StreamParam.input.param*1000)
	{
		dal_core_event_notify(DAL_CORE_EVENT_P2V_SEEK_FILE_TITLE,&dal_evt_param);
		DebugMessage("[P2V_seek_prev]:seek movie titles!");
		return 0;
	}
	else
	{
		temptime = p2v_movie_playtime-pMsg->MsgData.StreamParam.input.param*1000;


	}
	//sprintf(tmp1,"offset=%d",temptime*p2v_movie_length/p2v_movie_duration);
	
	y=p2v_movie_length/p2v_movie_duration;
	x=temptime*y;
	
	DebugMessage("[P2V_seek_prev]:time to %d !",temptime);
	sprintf(tmp1,"offset=%d",x);
	strcat(download_param.url,tmp1);
	strcat(download_param.url,str);
	
	DebugMessage("[P2V_seek_prev]:^^^^^%s",download_param.url);
	
	SetP2vResumeflag(1);
	dal_core_event_notify(DAL_CORE_EVENT_SEEK_ACTION_START,&dal_evt_param);

	iret = DAL_DownloadBufferInit();
	if( iret != 0)
	{
		DebugMessage("[P2V_seek_prev]->DAL_DownloadBufferInit failed\n");
		
		p_control->p2v_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_P2V_RESUME_DL_BUF_ALLOC_FAILED,&dal_evt_param);
		return -1;
	}

	iret = DAL_DownloadStart( &download_param);
	if( iret != 0)
	{
		DebugMessage("[P2V_seek_prev]->DAL_DownloadStart failed\n");
		DAL_DownloadBufferRelease();
		
		p_control->p2v_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_P2V_RESUME_DL_START_FAILED,&dal_evt_param);
		return -1;
	}

	/* start the pids parse task*/
	iret = DAL_ParseStart();
	if( iret != 0)
	{
		DebugMessage("[P2V_seek_prev]->DAL_ParseStart failed\n");
		DAL_DownloadStop();
		DAL_DownloadBufferRelease();
		
		p_control->p2v_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_P2V_RESUME_PIDS_PARSE_START_FAILED,&dal_evt_param);
		return -1;
	}
	DebugMessage("[DAL]P2V_seek_prev OK");


	/* set status CORE_STREAM_WAIT_PID, when patse task get new pids and notify core task, program will be play start*/
	dal_core_set_status(CORE_STREAM_WAIT_PID);
	p_control->recovery_flag = 0;
	dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);

	return 0;
}

S32 P2V_pause( void )
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t				dal_evt_param;
	S32							iret = 0;
	
	
	iret=DRV_AVCtrl_Set_PlayPause();
	if( iret != 0)
	{
		DebugMessage("[dal]P2V_pause -> -> DRV_AVCtrl_Set_PlayPause failed");
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
		return -1;
	}
	p2v_movie_playtime=Get_play_time();
	
	pbiinfo("[dal]P2V_pause -> -> p2v now play time [%d]",p2v_movie_playtime);
	iret =DAL_InjectPause();
	if( iret != 0)
	{
		DebugMessage("[dal]P2V_pause -> -> DAL_InjectPause failed");
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
		return -1;
	}
	
	
	iret = DAL_DownloadStop();
	if( iret != 0)
	{
		DebugMessage("[dal]P2V_pause -> -> DAL_DownloadStop failed");		
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
		return -1;
	}
	iret = DAL_DownloadBufferRelease();
	if( iret != 0)
	{
		DebugMessage("[dal]P2V_pause ->	-> DAL_DownloadBufferRelease failed");		
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
		return -1;
	}	
	iret = DAL_ParseStop();
	if( iret != 0)
	{
		DebugMessage("[dal]P2V_pause ->	-> DAL_ParseStop failed");		
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
		return -1;
	}
	
	dal_core_set_status(CORE_STREAM_PAUSE);
	dal_core_event_notify( DAL_CORE_EVENT_PAUSE_OK, &dal_evt_param);

	return 0;
}

S32 P2V_resume( U8* p2vurl )
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t				dal_evt_param;
	S32							iret = 0;
	dal_download_param_t		download_param;

	/*frist time to download data , alloc memory for download buffer*/
	iret = DAL_DownloadBufferInit();
	if( iret != 0)
	{
		DebugMessage("[DAL]P2V_resume->DAL_DownloadBufferInit failed\n");
		dal_core_event_notify(DAL_CORE_EVENT_P2V_RESUME_DL_BUF_ALLOC_FAILED,&dal_evt_param);
		return -1;
	}

	/* start the download task*/
	memset( &download_param, 0, sizeof(dal_download_param_t));
	
	SetP2vResumeflag(1);
	//加入偏移量
	{
		C8* str=NULL;		
		S32 length=0,Slen=0;
		C8 tmp[256],tmp1[256];
		U32 x=0,y=0;
		memset(tmp,0,sizeof(tmp));
		memset(tmp1,0,sizeof(tmp1));
		
		strcpy(tmp,p2vurl);
		length=strlen(tmp);
		
		DebugMessage("[P2V_resume]:^^^^^%s^^^^%d",tmp,length);
		str=tmp+length-1;
		while(*str!='/')
		{
			str=str-1;
		}
		Slen=strlen(str);
		DebugMessage("[P2V_resume]:^^^^^%s^^^^%d",str,Slen);
			
		strncpy(download_param.url,tmp,(length-Slen+1));
	y=p2v_movie_length/p2v_movie_duration;
	x=p2v_movie_playtime*y;
	
		sprintf(tmp1,"offset=%d",x);
		strcat(download_param.url,tmp1);
		strcat(download_param.url,str);
		
		DebugMessage("[P2V_resume]:^^^^^%s[%d][%d][%d][%d][%d]",download_param.url,p2v_movie_playtime,p2v_movie_length,p2v_movie_duration,x,y);
	}
	iret = DAL_DownloadStart( &download_param);
	if( iret != 0)
	{
		DebugMessage("[DAL]P2V_resume->DAL_DownloadStart failed\n");
		DAL_DownloadBufferRelease();
		dal_core_event_notify(DAL_CORE_EVENT_P2V_RESUME_DL_START_FAILED,&dal_evt_param);
		return -1;
	}

	/* start the pids parse task*/
	iret = DAL_ParseStart();
	if( iret != 0)
	{
		DebugMessage("[DAL]P2V_resume->DAL_ParseStart failed\n");
		DAL_DownloadStop();
		DAL_DownloadBufferRelease();
		dal_core_event_notify(DAL_CORE_EVENT_P2V_RESUME_PIDS_PARSE_START_FAILED,&dal_evt_param);
		return -1;
	}
	DebugMessage("[DAL]P2V_resume OK");


	/* set status CORE_STREAM_WAIT_PID, when patse task get new pids and notify core task, program will be play start*/
	dal_core_set_status(CORE_STREAM_WAIT_PID);
	p_control->recovery_flag = 0;
	dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);

	return 0;
}


S32 ParseP2vHttpHeader(char *headername,U32* movie_time,U32* movie_length)
{
	FILE* headerfp = NULL;
	
	headerfp = fopen(headername,"r");
	fseek(headerfp, 0, SEEK_END); 
	if( ftell(headerfp)== 0 )
	{
		fclose(headerfp);
		DebugMessage("file size 0!!");

		return -1;
	}
	fseek(headerfp, 0, SEEK_SET);
	while ( !feof(headerfp) )
	{
		C8 cString[512];
		
		memset(cString, 0, sizeof(cString));
		fscanf(headerfp,"%[^\n]%*c",cString);
		
		if(!strncmp(cString,"Content-Duration:",17))
		{
			C8*pstr=NULL;
			
			pstr = strchr(cString, ':');
			if(pstr!=NULL)
			{
				pstr+=2;
				*movie_time=atoi(pstr);
			}
			else
			{
				DebugMessage("[ParseHttpHeader]Content-Duration: point NULL!");
				fclose(headerfp);
				return -1;
			}
		}
		else if(!strncmp(cString,"Content-Length:",15))
		{
			C8*pstr=NULL;
			
			pstr = strchr(cString, ':');
			if(pstr!=NULL)
			{
				pstr=pstr+2;
				*movie_length=atoi(pstr);
			}
			else
			{
				DebugMessage("[ParseHttpHeader]Content-Length: point NULL!");
				
				fclose(headerfp);
				return -1;
			}
		}
	}
	
	fclose(headerfp);
	return 0;			
}




S32 DAL_HLS_seek_next(DAL_Player_Input_t* input,DAL_Player_Output_t* output)
{
    DAL_Message_t*		pMsg = NULL;
	DAL_CoreControl_t*	p_control = &g_CoreControl;
	
	if( TRUE == p_control->HLS_in_seek_proc)
	{
		/* DAL_Core 正在处理SEEK */
		DebugMessage("[DAL_HLS_seek_next]********************** DAL_Core is in seek proc, do not send msg to seek prev");
		return 0;
	}
	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg)
	{
		pMsg->MsgType = DALMSG_HLS_SEEK_NEXT;
		
		memcpy( &(pMsg->MsgData.StreamParam.input), input, sizeof(DAL_Player_Input_t));
		memcpy( &(pMsg->MsgData.StreamParam.output), output, sizeof(DAL_Player_Output_t));
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
		
		DebugMessage("[DAL_HLS_seek_next] --> DALMSG_HLS_SEEK_NEXT  %d",pMsg->MsgData.StreamParam.input.param);
		return 0;
	}
	else
	{
		DebugMessage("[DAL_HLS_seek_next] --> send message failed ");
		return -1;
	}


}
S32 DAL_HLS_seek_prev(DAL_Player_Input_t* input,DAL_Player_Output_t* output)
{
	DAL_Message_t*			pMsg = NULL;
	DAL_CoreControl_t*		p_control = &g_CoreControl;

	if( TRUE == p_control->HLS_in_seek_proc){
		/* DAL_Core 正在处理SEEK */
		DebugMessage("[DAL_HLS_seek_prev]********************** DAL_Core is in seek proc, do not send msg to seek next");
		return 0;
 	}
	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg)
	{
		pMsg->MsgType = DALMSG_HLS_SEEK_PREV;
		
		memcpy( &(pMsg->MsgData.StreamParam.input), input, sizeof(DAL_Player_Input_t));
		memcpy( &(pMsg->MsgData.StreamParam.output), output, sizeof(DAL_Player_Output_t));
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
		
		DebugMessage("[DAL_HLS_seek_prev] --> DALMSG_HLS_SEEK_prev  %d",pMsg->MsgData.StreamParam.input.param);
		return 0;
	}
	else
	{
		DebugMessage("[DAL_HLS_seek_prev] --> send message failed ");
		return -1;
	}
}

S32 hls_seek_next(DAL_Message_t* pMsg)
{
	S32 iret=0;
	dal_download_param_t		download_param;
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t 			dal_evt_param;
	C8* str=NULL;		
	S32 length=0,Slen=0;
	U32 temptime=0,ptime=0;
	C8 tmp[256],tmp1[256];

	
	if(get_catch_state()!=1)
	{
		DebugMessage("[hls_seek_next] -> -> hls_seek_next not support!");
		return -1;
	}	
	
	p_control->HLS_in_seek_proc = TRUE;
	iret = HLS_pause();
	if(iret !=0 )
	{
		DebugMessage("[hls_seek_next] -> -> HLS_pause failed");
		return -1;

	}
//		DAL_TaskDelay(500);
	set_hls_resume_state(1);
	ptime=Get_hls_pause_time();

	temptime = ptime+pMsg->MsgData.StreamParam.input.param*1000;//应该用另一个参数

	if(temptime >= Get_HLS_file_info())
	{
		dal_core_event_notify(DAL_CORE_EVENT_HLS_SEEK_FILE_END,&dal_evt_param);
		DebugMessage("[hls_seek_next]:seek movie end!");
		return 0;
	}
	Set_hls_pause_time(temptime);
	DebugMessage("[hls_seek_next]:time to %d !",temptime);
	
	dal_core_event_notify(DAL_CORE_EVENT_SEEK_ACTION_START,&dal_evt_param);
	DAL_TaskDelay(200);

	iret = DAL_DownloadBufferInit();
	if( iret != 0)
	{
		DebugMessage("[hls_seek_next]->DAL_DownloadBufferInit failed\n");
		p_control->HLS_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_HLS_RESUME_DL_BUF_ALLOC_FAILED,&dal_evt_param);	
		return -1;
	}


	iret =  M3u8DownloadResume();
	if( iret != 0)
	{
		DebugMessage("[hls_seek_next]->DAL_DownloadStart failed\n");
		DAL_DownloadBufferRelease();
		p_control->HLS_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_HLS_RESUME_DL_START_FAILED,&dal_evt_param);	
		return -1;
	}

	DebugMessage("[DAL]hls_seek_next OK");


	/* set status CORE_STREAM_WAIT_PID, when patse task get new pids and notify core task, program will be play start*/
	
	dal_core_set_status(CORE_STREAM_WAIT_PID);
	p_control->recovery_flag = 0;		
	//dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);
	//p_control->HLS_in_seek_proc = FALSE;

	return 0;
}

S32 hls_seek_prev(DAL_Message_t* pMsg)
{
	S32 iret=0;
	dal_download_param_t		download_param;
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t 			dal_evt_param;
	C8* str=NULL;		
	S32 length=0,Slen=0;
	U32 temptime=0,ptime=0;
	C8 tmp[256],tmp1[256];

	
	if(get_catch_state()!=1)
	{
		DebugMessage("[hls_seek_prev] -> -> hls_seek_prev not support!");
		return -1;
	}	
	
	p_control->HLS_in_seek_proc = TRUE;
	
	iret = HLS_pause();
	if(iret !=0 )
	{
		DebugMessage("[hls_seek_prev] -> -> hls_seek_prev failed");
		return -1;

	}
	
	set_hls_resume_state(1);
	ptime=Get_hls_pause_time();


	if(ptime<=pMsg->MsgData.StreamParam.input.param*1000)
	{
		dal_core_event_notify(DAL_CORE_EVENT_HLS_SEEK_FILE_TITLE,&dal_evt_param);
		DebugMessage("[hls_seek_prev]:seek movie start!");
		return 0;
	}
	else
	{
		temptime = ptime- pMsg->MsgData.StreamParam.input.param*1000;//应该用另一个参数
	}

	Set_hls_pause_time(temptime);
	DebugMessage("[hls_seek_prev]:time to %d !",temptime);
	
	dal_core_event_notify(DAL_CORE_EVENT_SEEK_ACTION_START,&dal_evt_param);
	DAL_TaskDelay(200);

	iret = DAL_DownloadBufferInit();
	if( iret != 0)
	{
		DebugMessage("[hls_seek_prev]->DAL_DownloadBufferInit failed\n");
		
		p_control->HLS_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_HLS_RESUME_DL_BUF_ALLOC_FAILED,&dal_evt_param);
		return -1;
	}

	iret =  M3u8DownloadResume();
	if( iret != 0)
	{
		DebugMessage("[hls_seek_prev]->DAL_DownloadStart failed\n");
		DAL_DownloadBufferRelease();
		
		p_control->HLS_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_HLS_RESUME_DL_START_FAILED,&dal_evt_param);
		return -1;
	}

	DebugMessage("[DAL]hls_seek_prev OK");


	/* set status CORE_STREAM_WAIT_PID, when patse task get new pids and notify core task, program will be play start*/
	dal_core_set_status(CORE_STREAM_WAIT_PID);
	p_control->recovery_flag = 0;
	//dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);
	//p_control->HLS_in_seek_proc = FALSE;

	return 0;
}

void SetCoreStatus(U32 flag)
{
if(flag==1)
	dal_core_set_status(CORE_STREAM_PAUSE);
else if(flag==0)
	dal_core_set_status(CORE_STREAM_WAIT_PID);
}

void SetP2vResumeflag(U8 flag)
{
	resume_flag=flag;

}
U8 GetP2vResumeflag(void)
{
	return resume_flag;
}
void stop_hls_seek_proc(void)
{
g_CoreControl.HLS_in_seek_proc=FALSE;
}

U32 Get_P2V_file_info(void)
{
	return p2v_movie_duration;

}

S32 DAL_Timeshift_PlayStart( DAL_Player_Input_t* input, DAL_Player_Output_t* output)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg)
	{
		pMsg->MsgType = DALMSG_STREAM_START;
		memcpy( &(pMsg->MsgData.StreamParam.input), input, sizeof(DAL_Player_Input_t));
		memcpy( &(pMsg->MsgData.StreamParam.output), output, sizeof(DAL_Player_Output_t));
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}
	else
	{
		DebugMessage("DAL_MessageAlloc failed\n");
	}
	return 0;
}


S32 DAL_Timeshift_PlayStop( void)
{
	DAL_Message_t*		pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg)
	{
		pMsg->MsgType = DALMSG_STREAM_STOP;
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	}
	else
	{
		DebugMessage("DAL_MessageAlloc failed\n");
	}
	return 0;
}

S32 DAL_TimeShift_seek_next(DAL_Player_Input_t* input,DAL_Player_Output_t* output)
{
	DAL_Message_t*			pMsg = NULL;
	DAL_CoreControl_t*		p_control = &g_CoreControl;

	if( TRUE == p_control->timeshift_in_seek_proc){
		/* DAL_Core 正在处理SEEK */
		DebugMessage("[DAL_TimeShift_seek_next]********************** DAL_Core is in seek proc, do not send msg to seek next");
		return 0;
 	}
	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg)
	{
		pMsg->MsgType = DALMSG_TIMESHIFT_SEEK_NEXT;
		
		//memcpy( &(pMsg->MsgData.StreamParam.input), input, sizeof(DAL_Player_Input_t));
		//memcpy( &(pMsg->MsgData.StreamParam.output), output, sizeof(DAL_Player_Output_t));
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
		
		DebugMessage("[DAL_TimeShift_seek_next] --> DALMSG_TIMESHIFT_SEEK_NEXT  %d",pMsg->MsgData.StreamParam.input.param);
		return 0;
	}
	else
	{
		DebugMessage("[DAL_TimeShift_seek_next] --> send message failed ");
		return -1;
	}
}
S32 DAL_TimeShift_seek_prev(DAL_Player_Input_t* input,DAL_Player_Output_t* output)
{
	DAL_Message_t*			pMsg = NULL;
	DAL_CoreControl_t*		p_control = &g_CoreControl;

	if( TRUE == p_control->timeshift_in_seek_proc){
		/* DAL_Core 正在处理SEEK */
		DebugMessage("[DAL_TimeShift_seek_prev]********************** DAL_Core is in seek proc, do not send msg to seek prev");
		return 0;
 	}
	pMsg = DAL_MessageAlloc();
	if( NULL != pMsg)
	{
		pMsg->MsgType = DALMSG_TIMESHIFT_SEEK_PREV;
		
		DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
		
		DebugMessage("[DAL_TimeShift_seek_prev] --> DALMSG_TIMESHIFT_SEEK_PREV  %d",pMsg->MsgData.StreamParam.input.param);
		return 0;
	}
	else
	{
		DebugMessage("[DAL_TimeShift_seek_prev] --> send message failed ");
		return -1;
	}
}


S32 Timeshift_pause( void )
{
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t				dal_evt_param;
	S32							iret = 0;
	
	
	iret=DRV_AVCtrl_Set_PlayPause();
	if( iret != 0)
	{
		DebugMessage("[dal]Timeshift_pause -> -> DRV_AVCtrl_Set_PlayPause failed");
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
		return -1;
	}
	//p2v_movie_playtime=Get_play_time();
	
	//pbiinfo("[dal]Timeshift_pause -> -> p2v now play time [%d]",p2v_movie_playtime);
	iret =DAL_InjectPause();
	if( iret != 0)
	{
		DebugMessage("[dal]Timeshift_pause -> -> DAL_InjectPause failed");
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
		return -1;
	}
	
	
	iret = timeshift_Download_Pause();
	if( iret != 0)
	{
		DebugMessage("[dal]Timeshift_pause -> -> DAL_DownloadStop failed");		
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
		return -1;
	}
	iret = DAL_DownloadBufferRelease();
	if( iret != 0)
	{
		DebugMessage("[dal]Timeshift_pause ->	-> DAL_DownloadBufferRelease failed");		
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
		return -1;
	}	
	iret = DAL_ParseStop();
	if( iret != 0)
	{
		DebugMessage("[dal]Timeshift_pause ->	-> DAL_ParseStop failed");		
		dal_core_event_notify( DAL_CORE_EVENT_PAUSE_FAILED, &dal_evt_param);
		return -1;
	}
	
	dal_core_set_status(CORE_STREAM_PAUSE);
	dal_core_event_notify( DAL_CORE_EVENT_PAUSE_OK, &dal_evt_param);

	return 0;
}

S32 Timeshift_seek_next(DAL_Message_t* pMsg)
{
	S32 iret=0;
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t				dal_evt_param;
	U32 x=0,y=0;


	p_control->timeshift_in_seek_proc = TRUE;
	iret=check_refresh_timeshift_list();
	if(iret==1)
	{
		DebugMessage("[Timeshift_seek_next] -> -> Timeshift_seek_next file end");
		dal_core_event_notify(DAL_CORE_EVENT_TIMESHIFT_SEEK_FILE_END,&dal_evt_param);
		return -1;
	}
	iret = Timeshift_pause();
	if(iret !=0 )
	{
		DebugMessage("[Timeshift_seek_next] -> -> Timeshift_seek_next failed");
		return -1;

	}
	
	
	//SetP2vResumeflag(1);
	dal_core_event_notify(DAL_CORE_EVENT_SEEK_ACTION_START,&dal_evt_param);
	DAL_TaskDelay(80);
	iret = DAL_DownloadBufferInit();
	if( iret != 0)
	{
		DebugMessage("[Timeshift_seek_next]->DAL_DownloadBufferInit failed\n");
		
		p_control->timeshift_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_TIMESHIFT_RESUME_DL_BUF_ALLOC_FAILED,&dal_evt_param);
		return -1;
	}

	iret = timeshift_Download_Resume();
	if( iret != 0)
	{
		DebugMessage("[Timeshift_seek_next]->DAL_DownloadStart failed\n");
		DAL_DownloadBufferRelease();
		
		p_control->timeshift_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_TIMESHIFT_RESUME_DL_START_FAILED,&dal_evt_param);
		return -1;
	}

	/* start the pids parse task*/
	iret = DAL_ParseStart();
	if( iret != 0)
	{
		DebugMessage("[Timeshift_seek_next]->DAL_ParseStart failed\n");
		dal_timeshift_download_stop();
		DAL_DownloadBufferRelease();
		
		p_control->timeshift_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_TIMESHIFT_RESUME_PIDS_PARSE_START_FAILED,&dal_evt_param);
		return -1;
	}
	DebugMessage("[DAL]Timeshift_seek_next OK");


	/* set status CORE_STREAM_WAIT_PID, when patse task get new pids and notify core task, program will be play start*/
	dal_core_set_status(CORE_STREAM_WAIT_PID);
	p_control->recovery_flag = 0;
	dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);

	return 0;
}

S32 Timeshift_seek_prev(DAL_Message_t* pMsg)
{
	S32 iret=0;
	DAL_CoreControl_t*			p_control = &g_CoreControl;
	dal_evt_param_t				dal_evt_param;
	U32 x=0,y=0;


	p_control->timeshift_in_seek_proc = TRUE;
	iret=check_timeshift_listhead();
	if(iret==1)
	{
		DebugMessage("[Timeshift_seek_prev] -> -> Timeshift_seek_prev file title");
		dal_core_event_notify(DAL_CORE_EVENT_TIMESHIFT_SEEK_FILE_TITLE,&dal_evt_param);
		return -1;
	}
	iret = Timeshift_pause();
	if(iret !=0 )
	{
		DebugMessage("[Timeshift_seek_prev] -> -> Timeshift_seek_prev failed");
		return -1;

	}
	
	
	//SetP2vResumeflag(1);
	dal_core_event_notify(DAL_CORE_EVENT_SEEK_ACTION_START,&dal_evt_param);
	DAL_TaskDelay(80);
	iret = DAL_DownloadBufferInit();
	if( iret != 0)
	{
		DebugMessage("[Timeshift_seek_prev]->DAL_DownloadBufferInit failed\n");
		
		p_control->timeshift_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_TIMESHIFT_RESUME_DL_BUF_ALLOC_FAILED,&dal_evt_param);
		return -1;
	}
	
	timeshift_prev_num();

	iret = timeshift_Download_Resume();
	if( iret != 0)
	{
		DebugMessage("[Timeshift_seek_prev]->DAL_DownloadStart failed\n");
		DAL_DownloadBufferRelease();
		
		p_control->timeshift_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_TIMESHIFT_RESUME_DL_START_FAILED,&dal_evt_param);
		return -1;
	}

	/* start the pids parse task*/
	iret = DAL_ParseStart();
	if( iret != 0)
	{
		DebugMessage("[Timeshift_seek_prev]->DAL_ParseStart failed\n");
		dal_timeshift_download_stop();
		DAL_DownloadBufferRelease();
		
		p_control->timeshift_in_seek_proc = FALSE;
		dal_core_event_notify(DAL_CORE_EVENT_TIMESHIFT_RESUME_PIDS_PARSE_START_FAILED,&dal_evt_param);
		return -1;
	}
	DebugMessage("[DAL]Timeshift_seek_prev OK");


	/* set status CORE_STREAM_WAIT_PID, when patse task get new pids and notify core task, program will be play start*/
	dal_core_set_status(CORE_STREAM_WAIT_PID);
	p_control->recovery_flag = 0;
	dal_core_event_notify(DAL_CORE_EVENT_STATUS_WIAT_PIDS,&dal_evt_param);

	return 0;
}

void dal_flush_ring_buffer(void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;

	DebugMessage("[DAL]dal_flush_ring_buffer OK");

	ring_buffer_flush(&(p_control->download_buffer));
}

void flush_ring_buffer(void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*			pMsg = NULL;

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_FLUSH_RINGBUFFER;
	DAL_MessageSend( DAL_GetCoreQueueID(), &pMsg);
	p_control->parse_pids_flag = PPF_WAIT_READY;
}

