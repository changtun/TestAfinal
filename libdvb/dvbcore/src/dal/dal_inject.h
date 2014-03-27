/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : 
* Description : 
* Author : zyliu
* History :
*	2011/4/11 : Initial Version
******************************************************************************/
#ifndef __DAL_INJECT_H
#define __DAL_INJECT_H

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

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/

int DAL_InjectTaskCreate( void);
int DAL_InjectTaskDelete( void);
int DAL_InjectStart( void);
int DAL_InjectPause( void);
int DAL_InjectResume( void);
int DAL_InjectStop( void);
U32 Get_play_time(void);
BOOL DAL_InjectIsPause(void);

DAL_QueueID_t DAL_GetInjectQueueID(void);


int DAL_InjectTest(void);


#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __XXXXXXX_H */
/* End of __DAL_INJECT_H.h  --------------------------------------------------------- */

