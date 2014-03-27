/**
  Copyright (C) 2008 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmsysOM3971Instance.c
 *                %version: 1 %
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
   26-Nov-2007  | V.VRIGNAUD|       |       | COMPATIBILITY WITH NXPFE
                | C.CAZETTES|       |       | 
   -------------|-----------|-------|-------|-----------------------------------
                |           |       |       |
   -------------|-----------|-------|-------|-----------------------------------
   \endverbatim
 *
*/

#include "tmNxTypes.h"
#include "tmCompId.h"
#include "tmbslFrontEndCfgItem.h"
#include "tmFrontEnd.h"
#include "tmUnitParams.h"
#include "tmbslFrontEndTypes.h"
#include "tmsysFrontEndTypes.h"

#include "tmbslTDA10025.h"

#include "tmsysOM3971.h"
#include "tmsysOM3971local.h"
#include "tmsysOM3971Instance.h"
#include "tmsysOM3971_InstanceCustom.h"

//-----------------------------------------------------------------------------
// Global data:
//-----------------------------------------------------------------------------
//
//
tmOM3971Object_t gOM3971Instance[] =
{
    {
        (tmUnitSelect_t)(-1),                                       /* OM3971 unit number */
        (tmUnitSelect_t)(-1),                                       /* OM3971 unit number temporary */
        (tmUnitSelect_t)(-1),                                       /* TDA182I2 unit number */
        (tmUnitSelect_t)(-1),                                       /* TDA10025 unit number */
        (tmUnitSelect_t)(-1),                                       /* TDA10025 common unit number */
        Null,                                                       /* pMutex */
        False,                                                      /* init */
        {                                                           /* sRWFunc */
            Null,
            Null
        },
        {                                                           /* sTime */
            Null,
            Null
        },
        {                                                           /* sDebug */
            Null
        },
        {                                                           /* sMutex */
            Null,
            Null,
            Null,
            Null
        },
        tmPowerStandby,                                             /* Power State */
        False,                                                      /* resetDone */
        {                                                           /* uHwAddress */
            0x00,                                                   /* tmOM3971UnitDeviceTypeUnknown */
            0xC0,                                                   /* tmOM3971UnitDeviceTypeTDA18250 */
            0x1E,                                                   /* tmOM3971UnitDeviceTypeTDA10025 */
        },
        TRT_UNKNOWN,                                                /* Last tune request type */
        {                                                           /* tmDVBCRequest_t */
            (UInt32)tmbslFrontEndStateMax,                          /* eFrontEndLock */
            (UInt32)0,                                              /* dwFrequency */
            (UInt32)tmFrontEndModulationMax,                        /* dwModulationType */
            (UInt32)0,                                              /* dwSymbolRate */
            (UInt32)tmFrontEndSpecInvMax,                           /* dwSpectralInversion */
            (UInt32)0,												/* dwBandWidth */
        },
        {                                                           /* tmAnalogTVRequest_t */
            (UInt32)tmbslFrontEndStateMax,                          /* eFrontEndHLock */
            (UInt32)tmbslFrontEndStateMax,                          /* eFrontEndVLock */
            (UInt32)100000000,                                      /* dwFrequency */
            (UInt32)100000000,                                      /* dwLastFrequency */
            (UInt32)(-1),                                           /* dwCountry */
            (UInt32)ANALOG_NTSC_M,                                  /* tAnalogStandard */
            (UInt32)(-1),                                           /* dwKSAnalogStandard */
            (UInt32)(-1),                                           /* bAutoStdDet */
            (UInt32)(-1),                                           /* dwChannel */
            (UInt32)(-1),                                           /* dwTuningFlags */
        },
        0,                                                          /* lastIFSet */
        {
            0                                                       /* uI2CSwitchCounter */
        },
        OM3971_MAX_UNITS                                            /* uNbPath */
    },
    {
        (tmUnitSelect_t)(-1),                                       /* OM3971 unit number */
        (tmUnitSelect_t)(-1),                                       /* OM3971 unit number temporary */
        (tmUnitSelect_t)(-1),                                       /* TDA182I2 unit number */
        (tmUnitSelect_t)(-1),                                       /* TDA10025 unit number */
        (tmUnitSelect_t)(-1),                                       /* TDA10025 common unit number */
        Null,                                                       /* pMutex */
        False,                                                      /* init */
        {                                                           /* sRWFunc */
            Null,
            Null
        },
        {                                                           /* sTime */
            Null,
            Null
        },
        {                                                           /* sDebug */
            Null
        },
        {                                                           /* sMutex */
            Null,
            Null,
            Null,
            Null
        },
        tmPowerStandby,                                             /* Power State */
        False,                                                      /* resetDone */
        {                                                           /* uHwAddress */
            0x00,                                                   /* tmOM3971UnitDeviceTypeUnknown */
            0xC6,                                                   /* tmOM3971UnitDeviceTypeTDA18250 */
            0x1E,                                                   /* tmOM3971UnitDeviceTypeTDA10025 */
        },
        TRT_UNKNOWN,                                                /* Last tune request type */
        {                                                           /* tmDVBCRequest_t */
            (UInt32)tmbslFrontEndStateMax,                          /* eFrontEndLock */
            (UInt32)0,                                              /* dwFrequency */
            (UInt32)tmFrontEndModulationMax,                        /* dwModulationType */
            (UInt32)0,                                              /* dwSymbolRate */
            (UInt32)tmFrontEndSpecInvMax,                           /* dwSpectralInversion */
            (UInt32)0,												/* dwBandWidth */
        },
        {                                                           /* tmAnalogTVRequest_t */
            (UInt32)tmbslFrontEndStateMax,                          /* eFrontEndHLock */
            (UInt32)tmbslFrontEndStateMax,                          /* eFrontEndVLock */
            (UInt32)100000000,                                      /* dwFrequency */
            (UInt32)100000000,                                      /* dwLastFrequency */
            (UInt32)(-1),                                           /* dwCountry */
            (UInt32)ANALOG_NTSC_M,                                  /* tAnalogStandard */
            (UInt32)(-1),                                           /* dwKSAnalogStandard */
            (UInt32)(-1),                                           /* bAutoStdDet */
            (UInt32)(-1),                                           /* dwChannel */
            (UInt32)(-1),                                           /* dwTuningFlags */
        },
        0,                                                          /* lastIFSet */
        {
            0                                                       /* uI2CSwitchCounter */
        },
        OM3971_MAX_UNITS                                            /* uNbPath */
    }
};


//-----------------------------------------------------------------------------
// FUNCTION:    OM3971AllocInstance:
//
// DESCRIPTION: allocate new instance
//
// RETURN:      
//
// NOTES:       
//-----------------------------------------------------------------------------
//
tmErrorCode_t
OM3971AllocInstance
(
    tmUnitSelect_t      tUnit,      /* I: Unit number */
    pptmOM3971Object_t  ppDrvObject /* I: Device Object */
)
{ 
    tmErrorCode_t       err = OM3971_ERR_BAD_UNIT_NUMBER;
    ptmOM3971Object_t   pObj = Null;
    UInt32              uLoopCounter = 0;    

    /* Find a free instance */
    for(uLoopCounter = 0; uLoopCounter<OM3971_MAX_UNITS; uLoopCounter++)
    {
        pObj = &gOM3971Instance[uLoopCounter];
        if(pObj->init == False)
        {
            pObj->tUnit = tUnit;
            pObj->tUnitW = tUnit;

            *ppDrvObject = pObj;
            err = TM_OK;
            break;
        }
    }

    /* return value */
    return err;
}

//-----------------------------------------------------------------------------
// FUNCTION:    OM3971DeAllocInstance:
//
// DESCRIPTION: deallocate instance
//
// RETURN:      always TM_OK
//
// NOTES:       
//-----------------------------------------------------------------------------
//
tmErrorCode_t
OM3971DeAllocInstance
(
    tmUnitSelect_t  tUnit   /* I: Unit number */
)
{     
    tmErrorCode_t       err = OM3971_ERR_NOT_INITIALIZED;
    ptmOM3971Object_t   pObj = Null;

    /* check input parameters */
    err = OM3971GetInstance(tUnit, &pObj);

    /* check driver state */
    if (err == TM_OK)
    {
        if (pObj == Null || pObj->init == False)
        {
            err = OM3971_ERR_NOT_INITIALIZED;
        }
    }

    if ((err == TM_OK) && (pObj != Null)) 
    {
        pObj->init = False;
    }

    /* return value */
    return err;
}

//-----------------------------------------------------------------------------
// FUNCTION:    OM3971GetInstance:
//
// DESCRIPTION: get the instance
//
// RETURN:      always True
//
// NOTES:       
//-----------------------------------------------------------------------------
//
tmErrorCode_t
OM3971GetInstance
(
    tmUnitSelect_t      tUnit,      /* I: Unit number */
    pptmOM3971Object_t  ppDrvObject /* I: Device Object */
)
{     
    tmErrorCode_t       err = OM3971_ERR_NOT_INITIALIZED;
    ptmOM3971Object_t   pObj = Null;
    UInt32              uLoopCounter = 0;    

    /* get instance */
    for(uLoopCounter = 0; uLoopCounter<OM3971_MAX_UNITS; uLoopCounter++)
    {
        pObj = &gOM3971Instance[uLoopCounter];
        if(pObj->init == True && pObj->tUnit == GET_INDEX_TYPE_TUNIT(tUnit))
        {
            pObj->tUnitW = tUnit;

            *ppDrvObject = pObj;
            err = TM_OK;
            break;
        }
    }

    /* return value */
    return err;
}

