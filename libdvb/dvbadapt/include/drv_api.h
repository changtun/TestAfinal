
/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : drv_api.h
* Description :The header file includes all application functions used to initialize the driver module.
              	This allows to be used in the main function for initializing driver module . 
* Author : Zheng Dong sheng
* History :
*	2010/02/05 : Initial Version
******************************************************************************/
#ifndef __DRVAPI_H
#define __DRVAPI_H
#ifdef __cplusplus
extern "C" {
#endif

#include "osapi.h"
#include "pvddefs.h"
#include "drv_tuner.h"
#include "drv_filter.h"
#include "drv_avctrl.h"


/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/
#define SYS_PMOC_WKUP_IRKEY_MAXNUM    6
/*******************************************************/
/*              Exported Types			                */
/*******************************************************/
typedef enum SYS_DRV_ErrorCode_e
{
	SYS_DRV_NO_ERROR = 0,
	SYS_DRV_ERROR
}SYS_DRV_ErrorCode_t;

typedef struct
{
    U32  u32IrPmocNum;
    U32  u32IrPowerKey0 [SYS_PMOC_WKUP_IRKEY_MAXNUM];
    U32  u32IrPowerKey1 [SYS_PMOC_WKUP_IRKEY_MAXNUM];
    U32  u32KeypadPowerKey;
    U32  u32WakeUpTime;
}SYS_DRV_PMOC_WKUP_T;
/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/
SYS_DRV_ErrorCode_t Pvware_DRV_Init( void );
SYS_DRV_ErrorCode_t DrvPlayerInit( int SleepFlag );
SYS_DRV_ErrorCode_t DrvPlayerUnInit();
SYS_DRV_ErrorCode_t DrvPmSetWakeUp(SYS_DRV_PMOC_WKUP_T tPmocWkup);
#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __DRVAPI_H */
/* End of drv_api.h  --------------------------------------------------------- */

