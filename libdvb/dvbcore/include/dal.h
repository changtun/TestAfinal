/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : 
* Description : 
* Author : zyliu
* History :
*	2010/12/13 : Initial Version
******************************************************************************/
#ifndef __DAL_H
#define __DAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************/
/*              Includes											*/
/*******************************************************/

/*******************************************************/
/*              Exported Defines and Macros            				*/
/*******************************************************/

#ifdef HAVE_TUNER_HEADER_FILE 
#undef HAVE_TUNER_HEADER_FILE
#endif

#ifdef USE_ST_PLAYREC
#undef USE_ST_PLAYREC
#endif
#ifndef USE_DAL_MULTICAST
#define USE_DAL_MULTICAST
#endif
#ifdef MMS
#undef MMS
#endif

#ifdef FLV
#undef FLV
#endif


#ifndef DEFINED_BOOL
#define DEFINED_BOOL
typedef int BOOL;
#endif

/* BOOL type constant values */
#ifndef TRUE
    #define TRUE (1 == 1)
#endif
#ifndef FALSE
    #define FALSE (!TRUE)
#endif




/*******************************************************/
/*              Exported Types			               				 */
/*******************************************************/
typedef enum
{
	Player1 =1,
	Player2,
	PlayerTotal,
}DAL_PlayerInstance_t;

typedef enum
{
	CH =1,
	EN
}DAL_LanguageType_t;

typedef struct DAL_AudioLanguage_s
{
	U32					language_num;
	DAL_LanguageType_t	language[MAX_AUD_TRACK_NUM];
}DAL_LanguageInfo_t;

typedef struct dal_evt_s
{
	U32		total_time;
	U32		cur_time;
	U32		status;
}dal_evt_param_t;

typedef void (*DAL_Event_Notify_t)( U32 event, void* param);

/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/



S32 DAL_Init( void);
S32 DAL_Term( void);


S32 DAL_Stream_Start(DAL_PlayerInstance_t player,DAL_Player_Input_t* input, DAL_Player_Output_t* output);
S32 DAL_Stream_Stop(DAL_PlayerInstance_t player);
S32 DAL_Stream_Restart(DAL_PlayerInstance_t player,DAL_Player_Input_t* input,DAL_Player_Output_t* output);

S32 DAL_Stream_Pause(DAL_PlayerInstance_t player);
S32 DAL_Stream_resume(DAL_PlayerInstance_t player);
S32 DAL_Stream_SetSpeed(DAL_PlayerInstance_t player, DAL_PlayerSpeed_t speed);
S32 DAL_Stream_Seek(DAL_PlayerInstance_t player, U32 seconds);

/*当前仅提供给still 检测playerc是否已关闭*/
BOOL DAL_Stream_TestStop(DAL_PlayerInstance_t player);


S32 DAL_Set_VideoEventCallback(DAL_PlayerInstance_t player,DAL_Event_Notify_t event_callback);
S32 DAL_Set_AudioEventCallback(DAL_PlayerInstance_t player,DAL_Event_Notify_t event_callback);
S32 DAL_Set_PlayEventCallback(DAL_PlayerInstance_t player,DAL_Event_Notify_t event_callback);





S32 DAL_Get_PlayerInfo(DAL_PlayerInstance_t player, DAL_Player_Info_t* player_info);

S32 DAL_Get_AudioLanguage(DAL_PlayerInstance_t player, DAL_LanguageInfo_t* language_info);
S32 DAL_Set_AudioLanguage(DAL_PlayerInstance_t player, DAL_LanguageType_t  language_type);

S32 DAL_Stream_PvrStart(DAL_PlayerInstance_t player,const U8* filename);
S32 DAL_Stream_PvrStop(DAL_PlayerInstance_t player);
S32 DAL_Stream_TimeshiftStart_Record(DAL_PlayerInstance_t player);
S32 DAL_Stream_TimeshiftStart_Play(DAL_PlayerInstance_t player);
S32 DAL_Stream_TimeshiftStop(DAL_PlayerInstance_t player);
S32 DAL_Stream_Seek_next(DAL_PlayerInstance_t player,DAL_Player_Input_t* input,DAL_Player_Output_t* output);
S32 DAL_Stream_Seek_prev(DAL_PlayerInstance_t player,DAL_Player_Input_t* input,DAL_Player_Output_t* output);

S32 DAL_Stream_GetPlayTime(DAL_PlayerInstance_t player, U32* p_tt_time_in_s, U32* p_cur_time_in_s);

S32 DAL_TEST(void);
S32 DAL_SetTuner( void);
S32 DAL_ffmpeg_test( void);
S32 DAL_ffmpeg_get_mediainfo_test( void);
int DAL_ffmpeg_decode(const char* filename);

S32 DAL_SetStatus( DAL_Player_States_t status);
S32 DAL_PlayEventCallback( dal_play_event_t event, U32 param);


/* 第一次进入normal_view 播放节目前调用，设置后解码开始直接刷新显示*/
S32 DAL_FirstTSPlayStart(void);
void dal_set_state(DAL_PlayerInstance_t id, DAL_Player_States_t state);


/** test by zxguan {{{ */
extern U32 dal_http_start();
/** }}}*/
#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __DAL_H */
/* End of dal.h  --------------------------------------------------------- */










