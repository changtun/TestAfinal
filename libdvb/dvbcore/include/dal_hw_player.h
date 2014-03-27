/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : dal_hw_player.h
* Description : 
* Author : zyliu
* History :
*	2010/12/13 : Initial Version
******************************************************************************/
#ifndef __DAL_HW_PLAYER_H
#define __DAL_HW_PLAYER_H

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
#define  SOURCENAME_MAX_LENGTH_NAME  512
typedef struct hw_play_start_param_s
{
	C8				source_name[SOURCENAME_MAX_LENGTH_NAME];
}hw_play_start_param_t;

/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/

DAL_ERROR_t DAL_hw_play_start( DAL_Player_Input_t* input, DAL_Player_Output_t* output);
DAL_ERROR_t DAL_hw_play_stop( DAL_Player_Input_t* input);
DAL_ERROR_t DAL_hw_play_pause( DAL_Player_Input_t* input);
DAL_ERROR_t DAL_hw_play_resume( DAL_Player_Input_t* input);
DAL_ERROR_t DAL_hw_play_seek( DAL_Player_Input_t* input, U32 msecond);
DAL_ERROR_t DAL_hw_play_setspeed( DAL_Player_Input_t* input, DAL_PlayerSpeed_t speed);
DAL_ERROR_t DAL_hw_play_restart( DAL_Player_Input_t* old_input, DAL_Player_Input_t* new_input, DAL_Player_Output_t* old_output, DAL_Player_Output_t* new_output);
DAL_ERROR_t DAL_hw_play_getinfo( DAL_Player_Input_t* input, DAL_Player_Info_t* info);


DAL_ERROR_t DAL_PlayrecStartWithDemux( DAL_Player_Input_t* input, DAL_Player_Output_t* output);
DAL_ERROR_t DAL_PlayrecStartWithFile( DAL_Player_Input_t* input, DAL_Player_Output_t* output);
DAL_ERROR_t DAL_PlayrecStop(void);


DAL_ERROR_t DAL_hw_play_getplaytime( DAL_Player_Input_t* input, unsigned int* p_total_time, unsigned int* p_current_time);
#ifdef FLV
DAL_ERROR_t DAL_hw_play_flvlist_getplaytime(unsigned int* p_total_time, unsigned int* p_current_time);
DAL_ERROR_t DAL_hw_play_flvlist_play_prev( void);
DAL_ERROR_t DAL_hw_play_flvlist_play_next( void);
DAL_ERROR_t DAL_hw_play_flvlist_play_pause_and_resume( void);
#endif


/******************************************************************************
* Function : 
* parameters :		 
* Return :	
* Description :
* Author : zyliu
******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __DAL_HW_PLAYER_H */
/* End of dal_hw_player.h  --------------------------------------------------------- */

