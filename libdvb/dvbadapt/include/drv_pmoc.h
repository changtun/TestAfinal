/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : drv_pmoc.h
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/12/27
  Last Modified :
  Description   : drv_pmoc.c header file
  Function List :
  History       :
  1.Date        : 2012/12/27
    Author      : zxguan
    Modification: Created file

******************************************************************************/

/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

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

#ifndef __DRV_PMOC_H__
#define __DRV_PMOC_H__

#include "pvddefs.h"
#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern S32 Drv_Pmoc_Init();
extern S32 Drv_Pmoc_DeInit();
extern S32 Drv_Pmoc_SetStandbyDispMode();
extern S32 Drv_Pmoc_SetWakeUpAttr();
extern S32 Drv_Pmoc_SwitchSystemMode();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DRV_PMOC_H__ */
