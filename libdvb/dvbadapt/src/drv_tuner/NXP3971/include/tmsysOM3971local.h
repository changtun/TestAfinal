/**
Copyright (C) 2008 NXP B.V., All Rights Reserved.
This source code and any compilation or derivative thereof is the proprietary
information of NXP B.V. and is confidential in nature. Under no circumstances
is this software to be  exposed to or placed under an Open Source License of
any type without the expressed written permission of NXP B.V.
*
* \file          tmsysOM3971local.h
*                %version: 2 %
*
* \date          %date_modified%
*
* \brief         Describe briefly the purpose of this file.
*
* REFERENCE DOCUMENTS :
*
* Detailed description may be added here.
*
* \section info Change Information
*
* \verbatim
Date          Modified by CRPRNr  TASKNr  Maintenance description
-------------|-----------|-------|-------|-----------------------------------
17-MAR-2009  | X.RAZAVET |       |       | CREATION OF OM3971 ARCHITECTURE 2.0.0
-------------|-----------|-------|-------|-----------------------------------
             |           |       |       |
-------------|-----------|-------|-------|-----------------------------------
\endverbatim
*
*/


#ifndef TMSYSOM3971LOCAL_H
#define TMSYSOM3971LOCAL_H

/*============================================================================*/
/*                       INCLUDE FILES                                        */
/*============================================================================*/


#ifdef __cplusplus
extern "C" {
#endif


    /*============================================================================*/
    /*                       MACRO DEFINITION                                     */
    /*============================================================================*/

#define OM3971_SYS_COMP_NUM         0
#define OM3971_SYS_MAJOR_VER        0
#define OM3971_SYS_MINOR_VER        5

#define POBJ_SRVFUNC_SIO pObj->sRWFunc
#define POBJ_SRVFUNC_STIME pObj->sTime
/* BEGIN: Deleted by zhwu, 2013/3/7 */
#define P_DBGPRINTEx pObj->sDebug.Print //tmsysOM3971Print
/* END: Deleted by zhwu, 2013/3/7 */
#define P_DBGPRINTVALID (True)

/*-------------*/
/* ERROR CODES */
/*-------------*/

/*---------*/
/* CHANNEL */
/*---------*/

#define OM3971_MAX_UNITS            2


/*-------------------------------------------------*/
/*   DEFAULT VALUES FOR CONFIGURATION MANAGEMENT   */
/*-------------------------------------------------*/

/*---------*/
/*  INDEX  */
/*---------*/


/*------------------*/
/*special config    */
/*------------------*/

/*------------------*/
/*  DEFAULT VALUES*/
/*------------------*/

/*----------------*/
/*  DEFINE MASKS*/
/*----------------*/

/*---------------*/
/*  DEFINE BITS  */
/*---------------*/

/*============================================================================*/
/*                       ENUM OR TYPE DEFINITION                              */
/*============================================================================*/

typedef struct _tmOM3971VBER_LUT_t {
        UInt32	dwVBER;
        UInt8	ucPerCent;
        char	cInfoString[50];
    } tmOM3971VBER_LUT_t, *ptmOM3971VBER_LUT_t;

typedef struct _tmOM3971config_t
    {
        UInt8   uI2CSwitchCounter;
    } tmOM3971config_t, *ptmOM3971config_t;

typedef struct _tmOM3971Object_t
{
    tmUnitSelect_t              tUnit;
    tmUnitSelect_t              tUnitW;
    tmUnitSelect_t              tUnitTDA18250;
    tmUnitSelect_t              tUnitTDA10025;
    tmUnitSelect_t              tUnitTDA10025Common;
    ptmbslFrontEndMutexHandle   pMutex;
    Bool                        init;
    tmbslFrontEndIoFunc_t       sRWFunc;
    tmbslFrontEndTimeFunc_t     sTime;
    tmbslFrontEndDebugFunc_t    sDebug;
    tmbslFrontEndMutexFunc_t    sMutex;
    tmPowerState_t              powerState;
    Bool                        resetDone;
    UInt32                      uHwAddress[tmOM3971UnitDeviceTypeMax];
    tmTuneReqType_t             lastTuneReqType;
    tmDVBCRequest_t             lastTuneRequestDVBC;
    tmAnalogTVRequest_t         lastTuneRequestAnalogTV;
    UInt32                      lastIFSet;
    tmOM3971config_t            sConfig;
    UInt8                       uNbPath;
} tmOM3971Object_t, *ptmOM3971Object_t,**pptmOM3971Object_t;


/*============================================================================*/
/*                       EXTERN DATA DEFINITION                               */
/*============================================================================*/

/*============================================================================*/
/*                       EXTERN FUNCTION PROTOTYPES                           */
/*============================================================================*/
#ifdef __cplusplus
}
#endif

#endif /* TMSYSOM3971LOCAL_H */
/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/

