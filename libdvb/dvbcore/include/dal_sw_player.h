/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : dal_sw_player.h
* Description : 
* Author : zyliu
* History :
*	2010/12/17 : Initial Version
******************************************************************************/
#ifndef __DAL_SW_PLAYER_H
#define __DAL_SW_PLAYER_H

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

/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

DAL_ERROR_t DAL_sw_play_start( void);
DAL_ERROR_t DAL_sw_play_stop( void);
DAL_ERROR_t DAL_sw_play_pause( void);
DAL_ERROR_t DAL_sw_play_resume( void);
DAL_ERROR_t DAL_sw_play_seek( void);
DAL_ERROR_t DAL_sw_play_setspeed( void);
DAL_ERROR_t DAL_sw_play_restart( void);
DAL_ERROR_t DAL_sw_play_getinfo( void);

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/

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

#endif  /* #ifndef __DAL_SW_PLAYER_H */
/* End of dal_sw_player.h  --------------------------------------------------------- */

