/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : dvbplayer_callbackandpmt.h
* Description :player 的父母锁控制部分
* Author : LSZ
* History :
*	2007/12/28 : Initial Version
******************************************************************************/
#ifndef __DVBPLAYER_CALLBACKANDPMT_H
#define __DVBPLAYER_CALLBACKANDPMT_H

/*******************************************************/
/*              Includes				                   */
/*******************************************************/
/* include ANSI C .h file */
/* include OS, STAPI, Driver .h file */
/* include CA .h file */
/* include Public .h file */
/* include Local .h file */
#include "dvbplayer.h"
#include "background.h"
#ifdef __cplusplus
extern "C" {
#endif

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
U32 DVBPlayer_Check_PMT();
U32 DVBPlayer_Start_PMT(U16 usPMTPid, U16 usServiceID);
U32 DVBPlayer_Stop_PMT(void);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __DVBPLAYER_CALLBACKANDPMT_H */
/* End of zap_parentrate.h  --------------------------------------------------------- */

