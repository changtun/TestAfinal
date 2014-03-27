/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : drv_pmoc.c
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/12/27
  Last Modified :
  Description   : ´ý»ú
  Function List :
  History       :
  1.Date        : 2012/12/27
    Author      : zxguan
    Modification: Created file

******************************************************************************/

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "pbitrace.h"
//#include "hi_unf_ecs.h"
#include "drv_pmoc.h"
/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/
     /* the PowerKeyValue = ( xx\device\hisil-
                                      icon\godbox\driver\sdk\msp\android_dr-
                                      iver\hi_keypad\ir_keyboad.h )IR_KEY_P-
                                      OWER */
                                      
#define POWER_KEY_VALUE 0x23dcfd01
/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/
/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
S32 Drv_Pmoc_Init()
{
    S32 RetErr = 0;
    
    //RetErr = HI_UNF_PMOC_Init ();
    if( 0 != RetErr )
    {
        pbierror("[%s %d]  [Error]  ErrNo = 0x%x.\n",DEBUG_LOG,RetErr );
    }
    
    return RetErr;
}

S32 Drv_Pmoc_DeInit()
{
    S32 RetErr = 0;
    
    //RetErr = HI_UNF_PMOC_DeInit ();
    if( 0 != RetErr )
    {
        pbierror("[%s %d]  [Error]  ErrNo = 0x%x.\n",DEBUG_LOG,RetErr );
    }
    
    return RetErr;
}

S32 Drv_Pmoc_SetWakeUpAttr(void *pParam)
{
    S32 RetErr = 0;
    /*
    HI_UNF_PMOC_WKUP_S *pWakup = NULL;
    HI_UNF_PMOC_WKUP_S tWkup;

    if( NULL == pParam )
    {
        pbierror("[%s %d] Param is NULL!\n", DEBUG_LOG );
        return -1;
    }
    pWakup = (HI_UNF_PMOC_WKUP_S *)pParam;
    
    memset( &tWkup, 0, sizeof(HI_UNF_PMOC_WKUP_S) );
    tWkup.u32IrPmocNum = 1;//pWakup->u32IrPmocNum;
    memcpy( tWkup.u32IrPowerKey0, pWakup->u32IrPowerKey0, PMOC_WKUP_IRKEY_MAXNUM );
    memcpy( tWkup.u32IrPowerKey1, pWakup->u32IrPowerKey1, PMOC_WKUP_IRKEY_MAXNUM );

    tWkup.u32IrPowerKey0[0] = POWER_KEY_VALUE;
    tWkup.u32KeypadPowerKey = pWakup->u32KeypadPowerKey;
    tWkup.u32WakeUpTime = pWakup->u32WakeUpTime;

    pbiinfo("[%s %d] WakeUpTime %d.\n", DEBUG_LOG, tWkup.u32WakeUpTime);
    PBIDEBUG("--------------");
    RetErr = HI_UNF_PMOC_SetWakeUpAttr ( &tWkup ); 
    if( HI_SUCCESS != RetErr )
    {
        pbierror("[%s %d]  [Error]  ErrNo = 0x%x.\n",DEBUG_LOG,RetErr );
    }
    */
    
    return RetErr;
}

S32 Drv_Pmoc_SwitchSystemMode()
{
    S32 RetErr = 0;
    
    return RetErr;
}

S32 Drv_Pmoc_SetStandbyDispMode()
{
    S32 RetErr = 0;
    /*
    HI_UNF_PMOC_STANDBY_MODE_S stStandbyMode;

    stStandbyMode.u32Mode = 0;
    stStandbyMode.u32DispCode = 0;
    stStandbyMode.stTimeInfo.u32Hour = 0;
    stStandbyMode.stTimeInfo.u32Minute = 0;
    stStandbyMode.stTimeInfo.u32Second = 0;    
    
    RetErr = HI_UNF_PMOC_SetStandbyDispMode (&stStandbyMode);
    if( HI_SUCCESS != RetErr )
    {
        pbierror("[%s %d]  [Error]  ErrNo = 0x%x.\n",DEBUG_LOG,RetErr );
    }
    */
    return RetErr;
}

