/**
Copyright (C) 2008 NXP B.V., All Rights Reserved.
This source code and any compilation or derivative thereof is the proprietary
information of NXP B.V. and is confidential in nature. Under no circumstances
is this software to be  exposed to or placed under an Open Source License of
any type without the expressed written permission of NXP B.V.
*
* \file          tmsysOM3971_63.c
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

/*============================================================================*/
/*                   STANDARD INCLUDE FILES                                   */
/*============================================================================*/


/*============================================================================*/
/*                   PROJECT INCLUDE FILES                                    */
/*============================================================================*/
#include "tmNxTypes.h"
#include "tmCompId.h"
#include "tmFrontEnd.h"
#include "tmbslFrontEndTypes.h"
#include "tmsysFrontEndTypes.h"
#include "tmbslFrontEndCfgItem.h"
#include "tmUnitParams.h"

#include "tmdlMutex.h"

#include "tmddTDA18250.h"
#include "tmbslTDA18250.h"
#include "tmbslTDA18250InstanceCustom.h"
#include "System_Lo_Detuning.h"

#include "tmbslTDA10025.h"

#include "tmsysOM3971.h"
#include "tmsysOM3971local.h"
#include "tmsysOM3971Instance.h"
#include "tmsysOM3971_InstanceCustom.h"

/*============================================================================*/
/*                   MACRO DEFINITION                                         */
/*============================================================================*/

#ifndef SIZEOF_ARRAY
#define SIZEOF_ARRAY(ar)        (sizeof(ar)/sizeof((ar)[0]))
#endif

#define GET_LOCK_STATUS_TIMEOUT 15

/*============================================================================*/
/*                   TYPE DEFINITION                                          */
/*============================================================================*/



/*============================================================================*/
/*                   PUBLIC VARIABLES DEFINITION                              */
/*============================================================================*/

/*============================================================================*/
/*                   STATIC VARIABLES DECLARATIONS                            */
/*============================================================================*/

/*============================================================================*/
/*                       EXTERN FUNCTION PROTOTYPES                           */
/*============================================================================*/


/*============================================================================*/
/*                   STATIC FUNCTIONS DECLARATIONS                            */
/*============================================================================*/
static tmErrorCode_t OM3971Init(tmUnitSelect_t tUnit);
static tmErrorCode_t OM3971Reset(tmUnitSelect_t tUnit);
static tmErrorCode_t OM3971SetFrequency(tmUnitSelect_t tUnit, ptmTunerOnlyRequest_t pTuneRequest);
static tmErrorCode_t OM3971SetDVBCFrequency(tmUnitSelect_t tUnit, ptmDVBCRequest_t pTuneRequest,tmTuneReqType_t tTuneReqType);


/*============================================================================*/
/*                   PUBLIC FUNCTIONS DEFINITIONS                             */
/*============================================================================*/



/*============================================================================*/
/*                   PROJECT INCLUDE FILES                                    */
/*============================================================================*/


/*============================================================================*/
/*                   TYPE DEFINITION                                          */
/*============================================================================*/


/*============================================================================*/
/*                   STATIC VARIABLES DECLARATIONS                            */
/*============================================================================*/

/* Lookup table for the VBER, SignalQuality per 100 */
/*    VBER   % */
tmOM3971VBER_LUT_t VBER_LUT[] =
{
    {   10,     100, " VBER < 1.0 10^(-7)              "},
    {   20,     100, " 2.0 10^(-7) > VBER > 1.0 10^(-7)"},
    {   40,     100, " 4.0 10^(-7) > VBER > 2.0 10^(-7)"},
    {   60,     100, " 6.0 10^(-7) > VBER > 4.0 10^(-7)"},
    {   80,     100, " 8.0 10^(-7) > VBER > 6.0 10^(-7)"},
    {  100,     100, " 1.0 10^(-6) > VBER > 8.0 10^(-7)"},
    {  200,     100, " 2.0 10^(-6) > VBER > 1.0 10^(-6)"},
    {  400,     100, " 4.0 10^(-6) > VBER > 2.0 10^(-6)"},
    {  600,     100, " 6.0 10^(-6) > VBER > 4.0 10^(-6)"},
    {  800,     100, " 8.0 10^(-6) > VBER > 6.0 10^(-6)"},
    { 1000,     100, " 1.0 10^(-5) > VBER > 8.0 10^(-6)"},
    { 2000,     100, " 2.0 10^(-5) > VBER > 1.0 10^(-5)"},
    { 4000,     100, " 4.0 10^(-5) > VBER > 2.0 10^(-5)"},
    { 6000,     100, " 6.0 10^(-5) > VBER > 4.0 10^(-5)"},
    { 8000,      90, " 8.0 10^(-5) > VBER > 6.0 10^(-5)"},
    { 10000,     80, " 1.0 10^(-4) > VBER > 8.0 10^(-5)"},
    { 20000,     70, " 2.0 10^(-4) > VBER > 1.0 10^(-4)"}, /* NorDig Level ~ 1MB/hour */
    { 40000,     60, " 4.0 10^(-4) > VBER > 2.0 10^(-4)"},
    { 60000,     50, " 6.0 10^(-4) > VBER > 4.0 10^(-4)"},
    { 80000,     40, " 8.0 10^(-4) > VBER > 6.0 10^(-4)"},
    { 100000,    35, " 1.0 10^(-3) > VBER > 8.0 10^(-4)"},
    { 200000,    30, " 2.0 10^(-3) > VBER > 1.0 10^(-3)"},
    { 300000,    25, " 3.0 10^(-3) > VBER > 2.0 10^(-3)"},
    { 400000,    20, " 4.0 10^(-3) > VBER > 3.0 10^(-3)"},
    { 500000,    15, " 5.0 10^(-3) > VBER > 4.0 10^(-3)"},
    { 600000,    10, " 6.0 10^(-3) > VBER > 5.0 10^(-3)"},
    { 700000,     9, " 7.0 10^(-3) > VBER > 6.0 10^(-3)"},
    { 800000,     8, " 8.0 10^(-3) > VBER > 7.0 10^(-3)"},
    { 900000,     7, " 9.0 10^(-3) > VBER > 8.0 10^(-3)"},
    { 1000000,    6, " 1.0 10^(-2) > VBER > 9.0 10^(-3)"},
    { 1200000,    5, " 1.2 10^(-2) > VBER > 1.0 10^(-2)"},
    { 1400000,    4, " 1.4 10^(-2) > VBER > 1.2 10^(-2)"},
    { 1600000,    3, " 1.6 10^(-2) > VBER > 1.4 10^(-2)"},
    { 1800000,    2, " 1.8 10^(-2) > VBER > 1.6 10^(-2)"},
    { 2000000,    1, " 2.0 10^(-2) > VBER > 1.8 10^(-2)"}, /* no signal at all !!! */
    { 3000000,    0, " 3.0 10^(-2) > VBER > 2.0 10^(-2)"},
    { 4000000,    0, " 4.0 10^(-2) > VBER > 3.0 10^(-2)"},
    { 5000000,    0, " 5.0 10^(-2) > VBER > 4.0 10^(-2)"},
    { 6000000,    0, " 6.0 10^(-2) > VBER > 5.0 10^(-2)"},
    { 7000000,    0, " 7.0 10^(-2) > VBER > 6.0 10^(-2)"},
    { 8000000,    0, " 8.0 10^(-2) > VBER > 7.0 10^(-2)"}, 
    { 9000000,    0, " 9.0 10^(-2) > VBER > 8.0 10^(-2)"},
    { 10000000,   0, " 1.0 10^(-1) > VBER > 9.0 10^(-2)"}  
}; 

Bool     btmOM3971LoDetuning ;              /* LO detuning Mode : Activated or not  */

/*============================================================================*/
/*                   PUBLIC FUNCTIONS DECLARATIONS                            */
/*============================================================================*/

/*============================================================================*/
/*                   STATIC FUNCTIONS DECLARATIONS                            */
/*============================================================================*/

/*============================================================================*/
/*                   PUBLIC FUNCTIONS DEFINITIONS                             */
/*============================================================================*/
/*============================================================================*/
/* tmsysOM3971GetSNR                                                       */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971GetSNR(
	tmUnitSelect_t tUnit,
	tmFrontEndFracNb32_t * psEsno
)
{
	tmErrorCode_t err = TM_OK;
	ptmOM3971Object_t pObj = Null;

	err = OM3971GetInstance(tUnit, &pObj);
	tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));
	
	if(err == TM_OK)
	{
	    err = tmbslTDA10025_GetChannelEsno(pObj->tUnitTDA10025, psEsno);
	}
	
	tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_GetChannelEsno(0x%08X) failed.\n", pObj->tUnitTDA10025));

	return err;
}

/*============================================================================*/
/* tmsysOM3971GetBER                                                       */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971GetBER(
    tmUnitSelect_t  tUnit,
    tmTuneReqType_t pathType,
    tmFrontEndFracNb32_t* psBER,
    UInt32 *puUncors
)
{
    tmErrorCode_t        err = TM_OK;
    ptmOM3971Object_t    pObj = Null;

    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));

    /* check that reset has been done */
    if( pObj->resetDone == 1 )
    {
        switch(pathType)
        {
            case TRT_DCABLE:
            case TRT_DVBC:
                err = tmbslTDA10025_GetBER(pObj->tUnitTDA10025, psBER, puUncors);
                break;

            default:
                tmDBGPRINTEx(DEBUGLVL_ERROR, "Error: Unsupported Path Type");
                psBER->lInteger = 1;
                psBER->uDivider = 1;
                err = OM3971_ERR_NOT_SUPPORTED;
                break;
        }
    }
    else
    {
        psBER->lInteger = 1;
        psBER->uDivider = 1;
        err = OM3971_ERR_NOT_INITIALIZED;
    }

    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmsysOM3971GetBER failed."));
    return err;
}

/*============================================================================*/
/* tmsysOM3971GetSignalQuality                                                */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971GetSignalQuality
(
    tmUnitSelect_t  tUnit,      /* I: FrontEnd unit number */
    UInt32          *pQuality   /* I/O: Signal Quality pointer */
)
{
    tmErrorCode_t       err = TM_OK;
    ptmOM3971Object_t   pObj = Null;

    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));

    if(err == TM_OK && !pQuality)
    {
        tmDBGPRINTEx(DEBUGLVL_TERSE,("Error: Invalid Pointer!"));
        err = OM3971_ERR_BAD_PARAMETER;
    }

    if(err == TM_OK)
    {
        err = tmbslTDA10025_GetSignalQuality(pObj->tUnitTDA10025, pQuality);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_GetSignalQuality(0x%08X) failed.\n", pObj->tUnitTDA10025));
    }
    
    return err;
}

/*============================================================================*/
/* tmsysOM3971GetSWVersion                                                    */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971GetSWVersion
(
    ptmsysSWVersion_t   pSWVersion  /* O: Receives SW Version  */
)
{
    tmErrorCode_t   err = TM_OK;
    static char     OM3971Name[] = "tmsysOM3971\0";
    static char     TDA18250Name[] = "tmbslTDA18250\0";
    static char     TDA10025Name[] = "tmbslTDA10025\0";

    pSWVersion->arrayItemsNumber = 3;
    pSWVersion->swVersionArray[0].pName = (char *)OM3971Name;
    pSWVersion->swVersionArray[0].nameBufSize = sizeof(OM3971Name);
    
    pSWVersion->swVersionArray[0].swVersion.compatibilityNr = OM3971_SYS_COMP_NUM;
    pSWVersion->swVersionArray[0].swVersion.majorVersionNr = OM3971_SYS_MAJOR_VER;
    pSWVersion->swVersionArray[0].swVersion.minorVersionNr = OM3971_SYS_MINOR_VER;
    
    pSWVersion->swVersionArray[1].pName = (char *)TDA18250Name;
    pSWVersion->swVersionArray[1].nameBufSize = sizeof(TDA18250Name);
    err = tmbslTDA18250GetSWVersion(&pSWVersion->swVersionArray[1].swVersion);
    //tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA18250GetSWVersion failed."));
    err = tmbslTDA18250GetSWSettingsVersion(&pSWVersion->swVersionArray[1].swSettingsVersion);
    //tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA18250GetSWSettingsVersion failed."));

    pSWVersion->swVersionArray[2].pName = (char *)TDA10025Name;
    pSWVersion->swVersionArray[2].nameBufSize = sizeof(TDA10025Name);
    err = tmbslTDA10025_GetSWVersion(&pSWVersion->swVersionArray[2].swVersion);
    //tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_GetSWVersion failed."));
    
    return err;
}

/*============================================================================*/
/* tmsysOM3971Init                                                            */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971Init
(
    tmUnitSelect_t  tUnit,   /* I: FrontEnd unit number */
    tmbslFrontEndDependency_t*  psSrvFunc   /* I: setup parameters     */
)
{
    ptmOM3971Object_t   pObj = Null;
    tmErrorCode_t       err = TM_OK;

    /* pObj initialization */
    err = OM3971GetInstance(tUnit, &pObj);

    /* check driver state */
    if (err == TM_OK || err == OM3971_ERR_NOT_INITIALIZED)
    {
        if (pObj != Null && pObj->init == True)
        {
            err = OM3971_ERR_NOT_INITIALIZED;
        }
        else 
        {

            /* initialize the Object */
            if (pObj == Null)
            {
                err = OM3971AllocInstance(tUnit, &pObj);
                if (err != TM_OK || pObj == Null)
                {
                    err = OM3971_ERR_NOT_INITIALIZED;        
                }
            }

            if (err == TM_OK)
            {
                /* initialize the Object by default values */
                pObj->sRWFunc = psSrvFunc->sIo;
                pObj->sTime = psSrvFunc->sTime;
                pObj->sDebug = psSrvFunc->sDebug;

                if(  psSrvFunc->sMutex.Init != Null
                    && psSrvFunc->sMutex.DeInit != Null
                    && psSrvFunc->sMutex.Acquire != Null
                    && psSrvFunc->sMutex.Release != Null)
                {
                    pObj->sMutex = psSrvFunc->sMutex;

                    err = pObj->sMutex.Init(&pObj->pMutex);
                }

                pObj->tUnitTDA18250 = GET_INDEX_TUNIT(tUnit)|UNIT_PATH_TYPE_VAL(tmOM3971UnitDeviceTypeTDA18250);
                pObj->tUnitTDA10025 = GET_INDEX_TUNIT(tUnit)|UNIT_PATH_TYPE_VAL(tmOM3971UnitDeviceTypeTDA10025);
               
                if ((tUnit == tmUnit0) || (tUnit == tmUnit1))
                {
                    pObj->tUnitTDA10025Common = GET_INDEX_TUNIT(tmUnit0)|UNIT_PATH_TYPE_VAL(tmOM3971UnitDeviceTypeTDA10025);
                }
                else if  ((tUnit == tmUnit2) || (tUnit == tmUnit3))
                {
                    pObj->tUnitTDA10025Common = GET_INDEX_TUNIT(tmUnit2)|UNIT_PATH_TYPE_VAL(tmOM3971UnitDeviceTypeTDA10025);
                }
                else if  ((tUnit == tmUnit4) || (tUnit == tmUnit5))
                {
                    pObj->tUnitTDA10025Common = GET_INDEX_TUNIT(tmUnit4)|UNIT_PATH_TYPE_VAL(tmOM3971UnitDeviceTypeTDA10025);
                }                


                pObj->init = True;
                err = TM_OK;

                err = OM3971Init(tUnit);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971Init(0x%08X) failed.", tUnit));
           }
        }
    }

    return err;
}

/*============================================================================*/
/* tmsysOM3971DeInit                                                          */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971DeInit
(
    tmUnitSelect_t  tUnit   /* I: FrontEnd unit number */
)
{
    ptmOM3971Object_t   pObj = Null;
    tmErrorCode_t       err = TM_OK;

    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));

    /************************************************************************/
    /* DeInitialize TDA18250 low layer setup                                */
    /************************************************************************/
    if (err == TM_OK)
    {
        if(pObj->sMutex.DeInit != Null)
        {
            if(pObj->pMutex != Null)
            {
                err = pObj->sMutex.DeInit(pObj->pMutex);
            }

            pObj->sMutex.Init = Null;
            pObj->sMutex.DeInit = Null;
            pObj->sMutex.Acquire = Null;
            pObj->sMutex.Release = Null;

            pObj->pMutex = Null;
        }
        
        err = tmbslTDA18250DeInit(pObj->tUnitTDA18250);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA18250DeInit(0x%08X) failed.", pObj->tUnitTDA18250));
    }

    /************************************************************************/
    /* DeInitialize TDA10025 low layer setup                                */
    /************************************************************************/
    if(err == TM_OK)
    {
        err = tmbslTDA10025_DeInit(pObj->tUnitTDA10025);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_DeInit(0x%08X) failed.", pObj->tUnitTDA10025));
    }
 
    err = OM3971DeAllocInstance(tUnit);

    return err;
}

/*============================================================================*/
/* tmsysOM3971Reset                                                           */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971Reset
(
    tmUnitSelect_t  tUnit   /* I: FrontEnd unit number */
)
{
    ptmOM3971Object_t   pObj = Null;
    tmErrorCode_t       err = OM3971_ERR_NOT_INITIALIZED;

    /* check input parameters */
    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        err = OM3971Reset(tUnit);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971Reset(0x%08X) failed.", tUnit));
    }

    return err;
}

/*============================================================================*/
/* tmsysOM3971SetPowerState                                                  */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971SetPowerState
(
    tmUnitSelect_t  tUnit,      /* I: FrontEnd unit number */
    tmsysFrontEndBlockType_t blockType,  /* I: Block Type: Shaed, Stream A/B */
    tmPowerState_t  ePowerState /* I: Power state of the device */
)
{
    ptmOM3971Object_t pObj = Null;
    tmErrorCode_t err = TM_OK;

    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        switch(ePowerState)
        {
        case tmPowerOn:
            pObj->powerState = ePowerState;

            /* TDA18250 powered on */
            err = tmbslTDA18250SetPowerState(pObj->tUnitTDA18250, tmTDA18250_PowerNormalMode); 
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA18250SetPowerState(0x%08X, PowerOn) failed.", pObj->tUnitTDA18250));

            if (err == TM_OK)
			{
				/* Set TDA10025 power state to On */
				err = tmbslTDA10025_SetPowerState(pObj->tUnitTDA10025, tmPowerOn);
				tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_SetPowerState(0x%08X, tmPowerOn) failed.", pObj->tUnitTDA10025));
			}

            break;

        case tmPowerStandby:
            pObj->powerState = ePowerState;

            /* 18250 powered on */
            err = tmbslTDA18250SetPowerState(pObj->tUnitTDA18250, tmTDA18250_PowerStandby);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA18250SetPowerState(0x%08X, PowerOn) failed.", pObj->tUnitTDA18250));
 
            if (err == TM_OK)
			{
				/* Set TDA10025 power state to On */
				tmbslTDA10025_SetPowerState(pObj->tUnitTDA10025, tmPowerOn);
				tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_SetPowerState(0x%08X, tmPowerOn) failed.", pObj->tUnitTDA10025));
			}
			if (err == TM_OK)
			{
                /* err = tmbslTDA10025_SetIFAGCOutput(pObj->tUnitTDA10025, tmbslFrontEndGpioConfTriState); */
				/* tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_SetIFAGCOutput(0x%08X, TriState) failed.", pObj->tUnitTDA10025)); */
			}
			if(err == TM_OK)
			{
                /* err = tmbslTDA10025SetTSOutput(pObj->tUnitTDA10025, tmbslFrontEndGpioConfTriState); */
				/* tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025SetTSOutput(0x%08X, TriState) failed.", pObj->tUnitTDA10025)); */
			}
			if (err == TM_OK)
			{
				/* Set TDA10025 power state to Standby*/
				tmbslTDA10025_SetPowerState(pObj->tUnitTDA10025, tmPowerStandby); 
				tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_SetPowerState(0x%08X, tmPowerStandby) failed.", pObj->tUnitTDA10025));
			}

            break;

        case tmPowerSuspend:
        case tmPowerOff:
        default:
            err = OM3971_ERR_NOT_SUPPORTED;
            break;
        }
    }

    if (err == TM_OK)
    {
        pObj->lastTuneReqType = TRT_UNKNOWN;
    }

    return err;
}

/*============================================================================*/
/* tmsysOM3971SetPowerState                                                  */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971GetPowerState
(
    tmUnitSelect_t  tUnit,      /* I: FrontEnd unit number */
    ptmPowerState_t pPowerState /* O: Power state of the device */
)
{
    ptmOM3971Object_t pObj = Null;
    tmErrorCode_t err = TM_OK;

    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        *pPowerState = pObj->powerState;
    }

    return err;
}

/*============================================================================*/
/* tmsysOM3971SendRequest                                                     */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971SendRequest
(
    tmUnitSelect_t  tUnit,              /* I: FrontEnd unit number */
    pVoid           pTuneRequest,       /* I/O: Tune Request Structure pointer */
    UInt32          dwTuneRequestSize,  /* I: Tune Request Structure size */
    tmTuneReqType_t tTuneReqType        /* I: Tune Request Type */
)
{
    ptmOM3971Object_t pObj = Null;
    tmErrorCode_t err = TM_OK;

    /* check pointer */
    if( !pTuneRequest )
    {
        tmDBGPRINTEx(DEBUGLVL_ERROR, "Error: Invalid Pointer!");
        err = OM3971_ERR_BAD_PARAMETER;
    }

    if (err == TM_OK)
    {
        err = OM3971GetInstance(tUnit, &pObj);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));
    }

    if (err == TM_OK && !pObj->resetDone)
    {
        err = OM3971_ERR_NOT_INITIALIZED;
    }

    if (err == TM_OK)
    {
        switch(tTuneReqType)
        {
        case TRT_TUNER_ONLY:
            if( dwTuneRequestSize != sizeof(tmTunerOnlyRequest_t) )
            {
                tmDBGPRINTEx(DEBUGLVL_ERROR, "Error: Bad parameter");
                err = OM3971_ERR_BAD_PARAMETER;
            }
            else
            {
                if (err == TM_OK)
                {
                    ptmTunerOnlyRequest_t pTunerOnlyRequest = (ptmTunerOnlyRequest_t)(pTuneRequest);

                    err = OM3971SetFrequency(tUnit, pTunerOnlyRequest);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "Error: OM3971SetFrequency failed"));
                }
            }
            break;

		case TRT_DCABLE:
        case TRT_DVBC:
            if( dwTuneRequestSize != sizeof(tmDVBCRequest_t) )
            {
                tmDBGPRINTEx(DEBUGLVL_ERROR, "Error: Bad parameter");
                err = OM3971_ERR_BAD_PARAMETER;
            }
            else
            {
                if (err == TM_OK)
                {
                    ptmDVBCRequest_t pDgtlTuneRequest = (ptmDVBCRequest_t)(pTuneRequest);

                    err = OM3971SetDVBCFrequency(tUnit, pDgtlTuneRequest,tTuneReqType);
                    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "Error: OM3971SetDVBCFrequency failed"));
                }
            }
            break;


        default:
            tmDBGPRINTEx(DEBUGLVL_ERROR, "Error: Unsupported tune request");
            err = OM3971_ERR_NOT_SUPPORTED;
            break;
        }
    }

    return err;
}

/*============================================================================*/
/* tmsysOM3971GetLastRequest                                                     */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971GetLastRequest
(
    tmUnitSelect_t  tUnit,              /* I: FrontEnd unit number */
    pVoid           pTuneRequest,       /* I/O: Tune Request Structure pointer */
    UInt32          dwTuneRequestSize,  /* I: Tune Request Structure size */
    tmTuneReqType_t tTuneReqType        /* I: Tune Request Type */
)
{
    ptmOM3971Object_t   pObj = Null;
    tmErrorCode_t       err = TM_OK;

    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));

   /* check pointer */
    if( !pTuneRequest )
    {
        tmDBGPRINTEx(DEBUGLVL_ERROR, "Error: Invalid Pointer!");
        err = OM3971_ERR_BAD_PARAMETER;
    }

    if(err == TM_OK)
    {
        switch(tTuneReqType)
        {
        case TRT_DCABLE:
        case TRT_DVBC:
            if( dwTuneRequestSize != sizeof(tmDVBCRequest_t) )
            {
                tmDBGPRINTEx(DEBUGLVL_ERROR, "Error: Bad parameter");
                err = OM3971_ERR_BAD_PARAMETER;
            }
            else
            {
                if (err == TM_OK)
                {
                    ptmDVBCRequest_t pDgtlTuneRequest = (ptmDVBCRequest_t)(pTuneRequest);

                    *pDgtlTuneRequest = pObj->lastTuneRequestDVBC;
                }
            }
            break;


        default:
            tmDBGPRINTEx(DEBUGLVL_ERROR, "Error: Unsupported tune request");
            err = OM3971_ERR_NOT_SUPPORTED;
        }
    }

    return err;
}


/*============================================================================*/
/* tmsysOM3971GetLockStatus                                                   */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971GetLockStatus
(
    tmUnitSelect_t          tUnit,      /* I: FrontEnd unit number */
    tmsysFrontEndState_t*   pLockStatus /* O: Lock status */
)
{
    ptmOM3971Object_t       pObj = Null;
    tmErrorCode_t           err = TM_OK;
    tmbslFrontEndState_t    lockStatus_tuner = tmbslFrontEndStateUnknown;
    tmbslFrontEndState_t    lockStatus_demod = tmbslFrontEndStateUnknown;

    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));


    if (err == TM_OK)
    {
        /* Get TDA18250 PLL Lock status */
        err = tmbslTDA18250GetLockStatus(pObj->tUnitTDA18250, &lockStatus_tuner);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA18250GetLockStatus(0x%08X) failed.", pObj->tUnitTDA18250));
    }
    if (err == TM_OK)
    {
        /* Get TDA10025 PLL Lock status */
        err = tmbslTDA10025_GetLockStatus(pObj->tUnitTDA10025, &lockStatus_demod);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_GetLockStatus(0x%08X) failed.", pObj->tUnitTDA10025));
    }

    if (err == TM_OK)
    {
        if( (lockStatus_tuner == tmbslFrontEndStateLocked) && (lockStatus_demod == tmbslFrontEndStateLocked))
        {
            *pLockStatus = tmsysFrontEndStateLocked;
        }
        else
        {
            *pLockStatus = tmsysFrontEndStateNotLocked;
        }
    }

    return err;
}

/*============================================================================*/
/* tmsysOM3971GetSignalStrength                                               */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971GetSignalStrength
(
     tmUnitSelect_t  tUnit,      /* I: FrontEnd unit number */
     Int32           *pStrength  /* I/O: Signal Strength pointer */
)
{
    ptmOM3971Object_t       pObj = Null;
    tmErrorCode_t           err = TM_OK;

    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3918GetInstance(0x%08X) failed.", tUnit));

    /* Check incoming pointer */
    if(err == TM_OK && !pStrength)
    {
        tmDBGPRINTEx(DEBUGLVL_TERSE,("Error: Invalid Pointer!"));
        err = OM3971_ERR_BAD_PARAMETER;
    }

    if (err == TM_OK)
    {
        /* Get Power level */
        err = tmbslTDA18250GetPowerLevel(pObj->tUnitTDA18250, pStrength);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA18250GetPowerLevel(0x%08X) failed.", pObj->tUnitTDA18250));
    }
    return err;
}

/*============================================================================*/
/* tmsysOM3971GetDeviceUnit                                                   */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971GetDeviceUnit
(
    tmUnitSelect_t              tUnit,      /* I: FrontEnd unit number */
    tmOM3971UnitDeviceType_t    deviceType, /* I: Device Type  */
    ptmUnitSelect_t             ptUnit      /* O: Device unit number */
)
{
    ptmOM3971Object_t   pObj = Null;
    tmErrorCode_t       err = TM_OK;

    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK && ptUnit!=Null)
    {
        switch(deviceType)
        {
        default:
        case tmOM3971UnitDeviceTypeUnknown:
            err = OM3971_ERR_BAD_PARAMETER;
            break;

        case tmOM3971UnitDeviceTypeTDA18250:
            *ptUnit = pObj->tUnitTDA18250;
            break;        
        case tmOM3971UnitDeviceTypeTDA10025:
            *ptUnit = pObj->tUnitTDA10025;
            break;

        }
    }

    return err;
}

/*============================================================================*/
/* tmsysOM3971SetHwAddress                                                    */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971SetHwAddress
(
    tmUnitSelect_t              tUnit,      /* I: FrontEnd unit number */
    tmOM3971UnitDeviceType_t    deviceType, /* I: Device Type  */
    UInt32                      uHwAddress  /* I: Hardware Address */
)
{
    ptmOM3971Object_t           pObj = Null;
    tmErrorCode_t               err = TM_OK;

    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        if(deviceType>tmOM3971UnitDeviceTypeUnknown && deviceType<tmOM3971UnitDeviceTypeMax)
        {
            pObj->uHwAddress[deviceType] = uHwAddress;
        }
        else
        {
            err = OM3971_ERR_BAD_PARAMETER;
        }
    }

    return err;
}

/*============================================================================*/
/* tmsysOM3971GetHwAddress                                                    */
/*============================================================================*/
extern tmErrorCode_t
tmsysOM3971GetHwAddress
(
    tmUnitSelect_t              tUnit,      /* I: FrontEnd unit number */
    tmOM3971UnitDeviceType_t    deviceType, /* I: Device Type  */
    UInt32*                     puHwAddress /* O: Hardware Address */
)
{
    ptmOM3971Object_t           pObj = Null;
    tmErrorCode_t               err = TM_OK;

    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));

    /* Check incoming pointer */
    if (err == TM_OK && !puHwAddress)
    {
        tmDBGPRINTEx(DEBUGLVL_TERSE,("Error: Invalid Pointer!"));
        err = OM3971_ERR_BAD_PARAMETER;
    }

    if (err == TM_OK)
    {
        if(deviceType>tmOM3971UnitDeviceTypeUnknown && deviceType<tmOM3971UnitDeviceTypeMax)
        {
            *puHwAddress = pObj->uHwAddress[deviceType];
        }
        else
        {
            err = OM3971_ERR_BAD_PARAMETER;
        }
    }

    return err;
}

/*============================================================================*/
/* tmsysOM3971SetLoDetuningMode                                               */
/*============================================================================*/
tmErrorCode_t
tmsysOM3971SetLoDetuningMode
(
    Bool     bLoDetuning                    /* I: LO detuning Mode : Activated or not  */
)
{
    tmErrorCode_t err = TM_OK;
    btmOM3971LoDetuning = bLoDetuning;
    return err;
}

/*============================================================================*/
/*                   STATIC FUNCTIONS DEFINITIONS                             */
/*============================================================================*/

/*============================================================================*/
/* OM3971Init                                                                 */
/*============================================================================*/
static tmErrorCode_t
OM3971Init
(
    tmUnitSelect_t  tUnit   /* I: FrontEnd unit number */
)
{
    ptmOM3971Object_t           pObj = Null;
    tmErrorCode_t               err = TM_OK;
    tmbslFrontEndDependency_t   sSrvFunc;   /* setup parameters */
    TDA10025_AdditionnalData_t  sTDA10025AdditionnalData;

    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.\n", tUnit));
 
    if (err == TM_OK)
    {
        /************************************************************************/
        /* TDA18250 low layer setup                                             */
        /************************************************************************/
        /* Fill function pointers structure */
        sSrvFunc.sIo                    = pObj->sRWFunc;
        sSrvFunc.sTime                  = pObj->sTime;
        sSrvFunc.sDebug                 = pObj->sDebug;
        sSrvFunc.sMutex                 = pObj->sMutex;
        sSrvFunc.dwAdditionalDataSize   = 0;
        sSrvFunc.pAdditionalData        = Null;

        err = tmbslTDA18250Init(pObj->tUnitTDA18250, &sSrvFunc);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA18250_Init(0x%08X) failed.\n", pObj->tUnitTDA18250));
    }

    if(err == TM_OK)
    {
        /************************************************************************/
        /* TDA10025 low layer setup                                             */
        /************************************************************************/
        sSrvFunc.dwAdditionalDataSize   = sizeof(TDA10025_AdditionnalData_t);
        sSrvFunc.pAdditionalData        = (void*)&sTDA10025AdditionnalData;

        sTDA10025AdditionnalData.tUnitCommon = pObj->tUnitTDA10025Common;
        if ((tUnit == tmUnit0) || (tUnit == tmUnit2) || (tUnit == tmUnit4))
        {
            if( pObj->uNbPath == 1 )
            {
                sTDA10025AdditionnalData.eChannelSel = TDA10025_ChannelSelectionIpA_SinglePath;
            }
            else
            {
                sTDA10025AdditionnalData.eChannelSel = TDA10025_ChannelSelectionIpA;
            }
        }
        else
        {
             sTDA10025AdditionnalData.eChannelSel = TDA10025_ChannelSelectionIpB;
        }
        
        err = tmbslTDA10025_Init(pObj->tUnitTDA10025, &sSrvFunc);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_Init(0x%08X) failed.", pObj->tUnitTDA10025));
    }
    

    return err;
}

/*============================================================================*/
/* OM3971Reset                                                                */
/*============================================================================*/
static tmErrorCode_t
OM3971Reset
(
    tmUnitSelect_t  tUnit   /* I: FrontEnd unit number */
)
{
    ptmOM3971Object_t   pObj = Null;
    tmErrorCode_t       err = TM_OK;
    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.\n", tUnit));

    /************************************************************************/
    /* TDA10025 initialization                                              */
    /************************************************************************/
    if ((err == TM_OK))
    {
        err = tmbslTDA10025_Reset(pObj->tUnitTDA10025); 
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_Reset(0x%08X) failed.", pObj->tUnitTDA10025));
    }

    /************************************************************************/
    /* TDA18250 initialization                                              */
    /************************************************************************/
    if (err == TM_OK) 
    {
        err = tmbslTDA18250Reset(pObj->tUnitTDA18250);
        if (err != TM_OK)
        {
            tmDBGPRINTEx(DEBUGLVL_ERROR, "tmbslTDA18250Reset(0x%08X) failed.", pObj->tUnitTDA18250);
        }
    }
   
    if( err == TM_OK )
    {
        /* in order to proceed faster and since there is no pb with uncor */
        err = tmbslTDA18250SetPowerTransitionMode( pObj->tUnitTDA18250, tmTDA18250_PowerTransitionMode_Regular );
    }

    if (err == TM_OK)
    {
        /* Put Front-End in Standby mode */
        err = tmsysOM3971SetPowerState(tUnit, tmsysFrontEndBlockStream, tmPowerStandby);
    }

    if( err == TM_OK )
    {
        /* set to default configuration */
        err = tmbslTDA18250SetPowerTransitionMode( pObj->tUnitTDA18250, tmTDA18250_PowerTransitionMode_Smooth );
    }

    if (err == TM_OK)
    {
        pObj->resetDone = 1;
    }

    return err;
}

/*============================================================================*/
/* OM3971SetFrequency                                                         */
/*============================================================================*/
static tmErrorCode_t
OM3971SetFrequency
(
    tmUnitSelect_t          tUnit,          /* I: FrontEnd unit number */
    ptmTunerOnlyRequest_t   pTuneRequest    /* I/O: Tuner Tune Request Structure pointer */
)
{
    ptmOM3971Object_t           pObj = Null;
    tmErrorCode_t               err = TM_OK;
    tmbslFrontEndState_t        eTunerLock = tmbslFrontEndStateUnknown;

    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));

    if (err == TM_OK)
    {
        tmDBGPRINTEx(DEBUGLVL_TERSE, "\n\n===========================================================================\n");
        tmDBGPRINTEx(DEBUGLVL_TERSE, " OM3971SetFrequency(0x%08X) is called with following parameters:\n", pObj->tUnit);
        tmDBGPRINTEx(DEBUGLVL_TERSE, "===========================================================================\n");
        tmDBGPRINTEx(DEBUGLVL_TERSE, "     Frequency:           %d Hz\n", pTuneRequest->dwFrequency);
        tmDBGPRINTEx(DEBUGLVL_TERSE, "     Standard:            %d\n",    pTuneRequest->dwStandard);
        tmDBGPRINTEx(DEBUGLVL_TERSE, "===========================================================================\n");
    }

    if ( (err == TM_OK) && ((pObj->lastTuneReqType != TRT_TUNER_ONLY) ) )
    {
        tmDBGPRINTEx(DEBUGLVL_TERSE,("Configuring Tuner!"));

        if (err == TM_OK)
        {
            err = tmbslTDA18250SetPowerState(pObj->tUnitTDA18250, tmTDA18250_PowerNormalMode);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA18250SetPowerState(0x%08X, PowerOn) failed.", pObj->tUnitTDA18250));
        }

        if (err == TM_OK)
        {
            pObj->powerState = tmPowerOn;
            pObj->lastTuneReqType = TRT_UNKNOWN;
        }
    }

    /************************************************************************/
    /* Program Tuner                                                        */
    /************************************************************************/
    if (err == TM_OK)
    {
        /* Set Tuner Standard mode */
        err = tmbslTDA18250SetStandardMode(pObj->tUnitTDA18250, pTuneRequest->dwStandard);
        if (err != TM_OK)
        {
            tmDBGPRINTEx(DEBUGLVL_ERROR, "tmbslTDA18250SetStandardMode(0x%08X, %d) failed.", pObj->tUnitTDA18250, pTuneRequest->dwStandard);
        }
    }

    if (err == TM_OK)
    {
        /* Set Tuner RF */
        err = tmbslTDA18250SetRf(pObj->tUnitTDA18250, pTuneRequest->dwFrequency);
        if (err != TM_OK)
        {
            tmDBGPRINTEx(DEBUGLVL_ERROR, "tmbslTDA18250SetRf(0x%08X, %d) failed.", pObj->tUnitTDA18250, pTuneRequest->dwFrequency);
        }
    }
    
    if (err == TM_OK)
    {
        /* Get Tuner PLL Lock status */
        err = tmbslTDA18250GetLockStatus(pObj->tUnitTDA18250, &eTunerLock);
        if (err != TM_OK)
        {
            tmDBGPRINTEx(DEBUGLVL_ERROR, "tmbslTDA18250GetLockStatus(0x%08X) failed.", pObj->tUnitTDA18250);
        }
    }

    if (err == TM_OK)
    {
        pTuneRequest->eTunerLock = (tmsysFrontEndState_t)eTunerLock;
    }

    if (err == TM_OK)
    {
        /* Print the result of the Manager function */
        switch(eTunerLock)
        {
            case tmbslFrontEndStateLocked:
                pObj->lastTuneReqType = TRT_TUNER_ONLY;
                tmDBGPRINTEx(DEBUGLVL_TERSE, "Tuner 0x%08X LOCKED.", pObj->tUnit);
                break;

            case tmbslFrontEndStateNotLocked:
                pObj->lastTuneReqType = TRT_UNKNOWN;
                tmDBGPRINTEx(DEBUGLVL_TERSE, "Tuner 0x%08X NOT LOCKED.", pObj->tUnit);
                break;

            case tmbslFrontEndStateSearching:
            default:
                pObj->lastTuneReqType = TRT_UNKNOWN;
                tmDBGPRINTEx(DEBUGLVL_ERROR, "Tuner 0x%08X TIMEOUT.", pObj->tUnit);
                break;
        }
    }

    return err;
}


/*============================================================================*/
/* OM3971SetDVBCFrequency                                                     */
/*============================================================================*/
static tmErrorCode_t
OM3971SetDVBCFrequency
(
    tmUnitSelect_t      tUnit,          /* I: FrontEnd unit number */
    ptmDVBCRequest_t    pTuneRequest,    /* I/O: DVB-C Tune Request Structure pointer */
    tmTuneReqType_t tTuneReqType        /* I: Tune Request Type */
)
{
    ptmOM3971Object_t           pObj = Null;
    tmErrorCode_t               err = TM_OK;
    tmTDA18250StandardMode_t    tda18250StdMode = tmTDA18250_DIG_8MHz;
    UInt32                      uIF = 4000000;
    tmbslFrontEndState_t        eTunerPLLLock = tmbslFrontEndStateUnknown;
    UInt32                      uLoopCount = 0;
    tmbslFrontEndState_t        eFrontEndLock = tmbslFrontEndStateUnknown;
    tmFrontEndFECMode_t         FECMode=tmFrontEndFECModeAnnexA; /* tmbslTDA10025_FECModeAnnexB or tmbslTDA10025_FECModeAnnexA */
    UInt32                      uLO = 0;

    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));

    if(err == TM_OK)
    {
        tmDBGPRINTEx(DEBUGLVL_TERSE, "\n\n===========================================================================\n");
        tmDBGPRINTEx(DEBUGLVL_TERSE, " OM3971SetDVBCFrequency(0x%08X) is called with following parameters:\n", pObj->tUnit);
        tmDBGPRINTEx(DEBUGLVL_TERSE, "===========================================================================\n");
        tmDBGPRINTEx(DEBUGLVL_TERSE, "     DVBC Frequency:      %d Hz\n", pTuneRequest->dwFrequency);
        tmDBGPRINTEx(DEBUGLVL_TERSE, "     ModulationType:      %d\n",    pTuneRequest->dwModulationType);
        tmDBGPRINTEx(DEBUGLVL_TERSE, "     SymbolRate:          %d\n",    pTuneRequest->dwSymbolRate);
        tmDBGPRINTEx(DEBUGLVL_TERSE, "     SpectralInversion:   %d\n",    pTuneRequest->dwSpectralInversion);
        tmDBGPRINTEx(DEBUGLVL_TERSE, "===========================================================================\n");
    }

    if ( (err == TM_OK) && ((pObj->lastTuneReqType != TRT_DVBC) || (pObj->lastTuneReqType != TRT_DCABLE)) )
    {
        //printf("Configuring DVBC Front-End!\n");
       
        if(err == TM_OK)
        {
            tmbslTDA10025_SetPowerState(pObj->tUnitTDA10025, tmPowerOn);//printf("FUN:%s,LINE:%d,ErrCode:%x \n ",__FUNCTION__,__LINE__,err);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_SetPowerState(0x%08X, PowerOn) failed.", pObj->tUnitTDA10025));
        }
/*
        if(err == TM_OK)
        {
            err = tmbslTDA10025_SetIFAGCOutput(pObj->tUnitTDA10025, tmbslFrontEndGpioConfOutputPushPull);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_SetIFAGCOutput(0x%08X, OutputPushPull) failed.", pObj->tUnitTDA10025));
        }
        if(err == TM_OK)
        {
            err = tmbslTDA10025_SetTSOutput(pObj->tUnitTDA10025, tmbslFrontEndGpioConfOutputPushPull);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_SetIFAGCOutput(0x%08X, OutputPushPull) failed.", pObj->tUnitTDA10025));
        }
        if(err == TM_OK)
        {
            err = tmbslTDA10025_SetGPIOConfig(pObj->tUnitTDA10025, tmbslTDA10025GPIOConfigFEL);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_SetGPIOConfig(0x%08X) failed.", pObj->tUnitTDA10025));
        }
*/
        if(err == TM_OK)
        {
            err = tmbslTDA18250SetPowerState(pObj->tUnitTDA18250, tmTDA18250_PowerNormalMode);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA18250SetPowerState(0x%08X, PowerOn) failed.", pObj->tUnitTDA18250));
        }

        if(err == TM_OK)
        {
            pObj->powerState = tmPowerOn;
        }
    }

    /************************************************************************/
    /* Program Tuner                                                        */
    /************************************************************************/

    if(err == TM_OK)
    {
        /* Set Tuner LPFC */
        switch(tTuneReqType)
        {
	    case TRT_DCABLE:
            tda18250StdMode = tmTDA18250_DIG_7MHz;
            FECMode= tmFrontEndFECModeAnnexB ;
            break;
        case TRT_DVBC:
			/* BEGIN: Modified by zhwu, 2013/3/8 */
			if ( pTuneRequest->dwBandWidth == 1 )
			    tda18250StdMode = tmTDA18250_DIG_7MHz;
			else
	            tda18250StdMode = tmTDA18250_DIG_9MHz;
			/* END:   Modified by zhwu, 2013/3/8 */
            FECMode = tmFrontEndFECModeAnnexA ;
            break;
        default:
            break;
        }
    }

    if(err == TM_OK)
    {
        /* Set Tuner Standard mode */
        err = tmbslTDA18250SetStandardMode(pObj->tUnitTDA18250, tda18250StdMode);
        if (err != TM_OK)
        {
            tmDBGPRINTEx(DEBUGLVL_ERROR, "tmbslTDA18250SetStandardMode(0x%08X, %d) failed.", pObj->tUnitTDA18250, tda18250StdMode);
        }
    }

    if( err == TM_OK )
    {
        if( btmOM3971LoDetuning == True )
        {
			// get tuner IF frequency
			err = tmbslTDA18250GetIF(pObj->tUnitTDA18250, &uIF);

			// detune LO if necessary
			uLO = pTuneRequest->dwFrequency + uIF;
            if (err == TM_OK)
            {
    			err= tmSystem_LO_Detuning(pObj->tUnitTDA18250, tmTDA18250DigitalChannel, &uLO);
                tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmLO_Detuning(0x%08X, %d) failed.", pObj->tUnitTDA18250, uLO));
            }
			pTuneRequest->dwFrequency = uLO - uIF;
        }
    }

    if(err == TM_OK)
    {
        /* Set Tuner RF */
        err = tmbslTDA18250SetRf(pObj->tUnitTDA18250, pTuneRequest->dwFrequency);
        if (err != TM_OK)
        {
            tmDBGPRINTEx(DEBUGLVL_ERROR, "tmbslTDA18250SetRf(0x%08X, %d) failed.", pObj->tUnitTDA18250, pTuneRequest->dwFrequency);
        }
    }
    if(err == TM_OK)
    {
        /* Get Tuner PLL Lock status */
        err = tmbslTDA18250GetLockStatus(pObj->tUnitTDA18250, &eTunerPLLLock);
        if (err != TM_OK)
        {
            tmDBGPRINTEx(DEBUGLVL_ERROR, "tmbslTDA18250GetLockStatus(0x%08X) failed.", pObj->tUnitTDA18250);
        }
    }
    if(err == TM_OK)
    {
        /* Get Tuner IF to pass it to channel decoder driver */
        err = tmbslTDA18250GetIF(pObj->tUnitTDA18250, &uIF);
        if (err != TM_OK)
        {
            tmDBGPRINTEx(DEBUGLVL_ERROR, "tmbslTDA18250GetIF(0x%08X) failed.", pObj->tUnitTDA18250);
        }
    }
    if(err == TM_OK)
    {
        tmDBGPRINTEx(DEBUGLVL_TERSE, "TDA18250(0x%08X) IF:%d Hz, PLL Lock:%d.", pObj->tUnitTDA18250, uIF, eTunerPLLLock);
    }
    /************************************************************************/
    /* Program Channel decoder                                              */
    /************************************************************************/
    //if(pObj->lastTuneReqType != tTuneReqType)
    {
        /* Change the Demod mode to DVB or MCNS only if it has changed  - Suppress Auto*/
        if(err == TM_OK)
        {
            /* Set Tuner bModeDvbMcns mode - SEt FECmode AnnexA ou B*/
            err = tmbslTDA10025_SetFECMode(pObj->tUnitTDA10025, FECMode);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_SetFECMode(0x%08X, %d) failed.", pObj->tUnitTDA18250, FECMode));
        }
    }

    if(err == TM_OK)
    {
        pObj->lastTuneReqType = tTuneReqType;
    }

    if(err == TM_OK)
    {
        /* Set channel decoder IF with IF from Tuner */
        err = tmbslTDA10025_SetIF(pObj->tUnitTDA10025, uIF);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_SetIF(0x%08X, %d) failed.", pObj->tUnitTDA10025, uIF));
        pObj->lastIFSet = uIF;
    }

    if(err == TM_OK)
    {
        /* Set channel decoder Spectrum Inversion */
        err = tmbslTDA10025_SetSI(pObj->tUnitTDA10025, pTuneRequest->dwSpectralInversion);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_SetSI(0x%08X, %d) failed.", pObj->tUnitTDA10025, pTuneRequest->dwSpectralInversion));
    }
    
    if(err == TM_OK)
    {
        /* Set channel decoder Modulation */
        err = tmbslTDA10025_SetMod(pObj->tUnitTDA10025, pTuneRequest->dwModulationType);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_SetMod(0x%08X, %d) failed.", pObj->tUnitTDA10025, pTuneRequest->dwModulationType));
    }

    if(err == TM_OK)
    {
        /* Set channel decoder Symbol Rate */
        err = tmbslTDA10025_SetSR(pObj->tUnitTDA10025, pTuneRequest->dwSymbolRate);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_SetSR(0x%08X, %d) failed.", pObj->tUnitTDA10025, pTuneRequest->dwSymbolRate));
    }

    if(err == TM_OK)
    {
        /* Wait for channel decoder lock procedure to complete or timeout after 5s */
        uLoopCount = 0;
        err = tmbslTDA10025_StartLock(pObj->tUnitTDA10025);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_StartLock(0x%08X) failed.", pObj->tUnit));

        do
        {
            err = tmbslTDA10025_GetLockStatus(pObj->tUnitTDA10025, &eFrontEndLock);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_GetLockStatus(0x%08X) failed.", pObj->tUnit));
            err = pObj->sTime.Wait(pObj->tUnit, 200);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "pObj->sTime.Wait(0x%08X) failed.", pObj->tUnit));

            uLoopCount++;
        } while ((eFrontEndLock == tmbslFrontEndStateSearching) && (uLoopCount < GET_LOCK_STATUS_TIMEOUT));
    }

    if(err == TM_OK)
    {
        pTuneRequest->eFrontEndLock = (tmsysFrontEndState_t)eFrontEndLock;
        
        /* Save tune request */
        pObj->lastTuneRequestDVBC = *pTuneRequest;
    }

    /* Print the result of the Manager function */
    switch(eFrontEndLock)
    {
    case tmbslFrontEndStateLocked:
        if(err == TM_OK)
        {
            tmDBGPRINTEx(DEBUGLVL_TERSE, "FrontEnd 0x%08X LOCKED.", pObj->tUnit);

            /* Get channel decoder Modulation */
            err = tmbslTDA10025_GetMod(pObj->tUnitTDA10025, (ptmFrontEndModulation_t)&pTuneRequest->dwModulationType);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_GetMod(0x%08X) failed.", pObj->tUnitTDA10025));
        }
        if(err == TM_OK)
        {
            /* Get channel decoder Symbol Rate */
            err = tmbslTDA10025_GetSR(pObj->tUnitTDA10025, &pTuneRequest->dwSymbolRate);
            tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmbslTDA10025_GetSR(0x%08X) failed.", pObj->tUnitTDA10025));
        }
        if(err == TM_OK)
        {
            tmDBGPRINTEx(DEBUGLVL_TERSE, "===========================================================================\n");
            tmDBGPRINTEx(DEBUGLVL_TERSE, " OM3971SetDVBCFrequency(0x%08X) found following parameters:\n", pObj->tUnit);
            tmDBGPRINTEx(DEBUGLVL_TERSE, "===========================================================================\n");
            tmDBGPRINTEx(DEBUGLVL_TERSE, "     DVBC Frequency:      %d Hz\n", pTuneRequest->dwFrequency);
            tmDBGPRINTEx(DEBUGLVL_TERSE, "     ModulationType:      %d\n",    pTuneRequest->dwModulationType);
            tmDBGPRINTEx(DEBUGLVL_TERSE, "     SymbolRate:          %d\n",    pTuneRequest->dwSymbolRate);
            tmDBGPRINTEx(DEBUGLVL_TERSE, "     SpectralInversion:   %d\n",    pTuneRequest->dwSpectralInversion);
			tmDBGPRINTEx(DEBUGLVL_TERSE, "     BandWidth :          %d\n",    pTuneRequest->dwBandWidth);
            tmDBGPRINTEx(DEBUGLVL_TERSE, "===========================================================================\n");
        }
        break;

    case tmbslFrontEndStateNotLocked:
        if(err == TM_OK)
        {
            tmDBGPRINTEx(DEBUGLVL_TERSE, "FrontEnd 0x%08X NOT LOCKED.", pObj->tUnit);
        }
        break;

    case tmbslFrontEndStateSearching:
    default:
        if(err == TM_OK)
        {
            tmDBGPRINTEx(DEBUGLVL_ERROR, "FrontEnd 0x%08X TIMEOUT.", pObj->tUnit);
        }
        break;
    }

    return err;
}

/*============================================================================*/
/* tmsysOM3971ResetUBK                                                     */
/*============================================================================*/
extern tmErrorCode_t
tmsysOM3971ResetUBK
(
    tmUnitSelect_t      tUnit  /* I: FrontEnd unit number */
)
{
    ptmOM3971Object_t pObj = Null;
    tmErrorCode_t     err = TM_OK;

    err = OM3971GetInstance(tUnit, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmsysOM3971ResetUBK(0x%08X) failed.", tUnit));
    
/*
    if( err == TM_OK)
    {
        err = tmbslTDA10025_ResetUBK(pObj->tUnitTDA10025);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "tmsysOM3971ResetUBK(0x%08X) failed.", tUnit));
    }
*/
    return err;
}
