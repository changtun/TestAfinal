/**
  Copyright (C) 2008 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmsysOM3971.h
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
   9-JULY-2007  | A.TANT    |       |       | CREATION OF OM3971 ARCHITECTURE 2.0.0
   -------------|-----------|-------|-------|-----------------------------------
                |           |       |       |
   -------------|-----------|-------|-------|-----------------------------------
   \endverbatim
 *
*/


#ifndef TMSYSOM3971_H
#define TMSYSOM3971_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*                       INCLUDE FILES                                        */
/*============================================================================*/

/*============================================================================*/
/*                       MACRO DEFINITION                                     */
/*============================================================================*/

/* SW Error codes */
#define OM3971_ERR_BASE               (CID_COMP_TUNER | CID_LAYER_DEVLIB)									/* 0x101E4000 */
#define OM3971_ERR_COMP               (CID_COMP_TUNER | CID_LAYER_DEVLIB | TM_ERR_COMP_UNIQUE_START)		/* 0x101E4800 */

#define OM3971_ERR_BAD_UNIT_NUMBER       (OM3971_ERR_BASE + TM_ERR_BAD_UNIT_NUMBER)
#define OM3971_ERR_NOT_INITIALIZED       (OM3971_ERR_BASE + TM_ERR_NOT_INITIALIZED)
#define OM3971_ERR_BAD_PARAMETER         (OM3971_ERR_BASE + TM_ERR_BAD_PARAMETER)
#define OM3971_ERR_NOT_SUPPORTED         (OM3971_ERR_BASE + TM_ERR_NOT_SUPPORTED)
#define OM3971_ERR_HW                    (OM3971_ERR_COMP + 0x0001)
#define OM3971_ERR_POWER_LEVEL_TOO_LOW   (OM3971_ERR_COMP + 0x0002)
#define OM3971_ERR_POWER_LEVEL_TOO_HIGH  (OM3971_ERR_COMP + 0x0003)

/*============================================================================*/
/*                       ENUM OR TYPE DEFINITION                              */
/*============================================================================*/

    /* OM3971 Device Types */
    typedef enum _tmOM3971UnitDeviceType_t
    {
        tmOM3971UnitDeviceTypeUnknown = 0,
        tmOM3971UnitDeviceTypeTDA18250,
        tmOM3971UnitDeviceTypeTDA10025,
        tmOM3971UnitDeviceTypeMax
    } tmOM3971UnitDeviceType_t, *ptmOM3971UnitDeviceType_t;

/*============================================================================*/
/*                       EXTERN DATA DEFINITION                               */
/*============================================================================*/



/*============================================================================*/
/*                       EXTERN FUNCTION PROTOTYPES                           */
/*============================================================================*/


extern tmErrorCode_t
tmsysOM3971GetSWVersion
(
    ptmsysSWVersion_t   pSWVersion  /* O: Receives SW Version  */
);

extern tmErrorCode_t
tmsysOM3971Init
(
    tmUnitSelect_t  tUnit,   /* I: FrontEnd unit number */
    tmbslFrontEndDependency_t*  psSrvFunc   /* I: setup parameters     */
);

extern tmErrorCode_t
tmsysOM3971DeInit
(
    tmUnitSelect_t  tUnit   /* I: FrontEnd unit number */
);

extern tmErrorCode_t
tmsysOM3971Reset
(
    tmUnitSelect_t  tUnit   /* I: FrontEnd unit number */
);

extern tmErrorCode_t
tmsysOM3971SetPowerState
(
    tmUnitSelect_t  tUnit,      /* I: FrontEnd unit number */
    tmsysFrontEndBlockType_t blockType,  /* I: Block Type: Shaed, Stream A/B */
    tmPowerState_t  ePowerState /* I: Power state of the device */
);

extern tmErrorCode_t
tmsysOM3971GetPowerState
(
    tmUnitSelect_t  tUnit,      /* I: FrontEnd unit number */
    ptmPowerState_t pPowerState /* O: Power state of the device */
);

extern tmErrorCode_t
tmsysOM3971SendRequest
(
    tmUnitSelect_t  tUnit,              /* I: FrontEnd unit number */
    pVoid           pTuneRequest,       /* I/O: Tune Request Structure pointer */
    UInt32          dwTuneRequestSize,  /* I: Tune Request Structure size */
    tmTuneReqType_t tTuneReqType        /* I: Tune Request Type */
);

extern tmErrorCode_t
tmsysOM3971GetLastRequest
(
    tmUnitSelect_t  tUnit,              /* I: FrontEnd unit number */
    pVoid           pTuneRequest,       /* I/O: Tune Request Structure pointer */
    UInt32          dwTuneRequestSize,  /* I: Tune Request Structure size */
    tmTuneReqType_t tTuneReqType        /* I: Tune Request Type */
);

extern tmErrorCode_t
tmsysOM3971SetI2CSwitchState
(
    tmUnitSelect_t                  tUnit,  /* I: FrontEnd unit number */
    tmsysFrontEndI2CSwitchState_t   eState  /* I: I2C switch state */
);

extern tmErrorCode_t
tmsysOM3971GetI2CSwitchState
(
    tmUnitSelect_t                  tUnit,
    tmsysFrontEndI2CSwitchState_t*  peState,
    UInt32*                         puI2CSwitchCounter
);

extern tmErrorCode_t
tmsysOM3971GetLockStatus
(
    tmUnitSelect_t          tUnit,      /* I: FrontEnd unit number */
    tmsysFrontEndState_t*   pLockStatus /* O: Lock status */
);

extern tmErrorCode_t
tmsysOM3971GetSignalStrength
(
    tmUnitSelect_t  tUnit,      /* I: FrontEnd unit number */
    Int32           *pStrength  /* I/O: Signal Strength pointer */
);

extern tmErrorCode_t
tmsysOM3971GetSignalQuality
(
    tmUnitSelect_t  tUnit,      /* I: FrontEnd unit number */
    UInt32          *pQuality   /* I/O: Signal Quality pointer */
);

extern tmErrorCode_t
tmsysOM3971GetDeviceUnit
(
    tmUnitSelect_t              tUnit,      /* I: FrontEnd unit number */
    tmOM3971UnitDeviceType_t    deviceType, /* I: Device Type  */
    ptmUnitSelect_t             ptUnit      /* O: Device unit number */
);

extern tmErrorCode_t
tmsysOM3971SetHwAddress
(
    tmUnitSelect_t              tUnit,      /* I: FrontEnd unit number */
    tmOM3971UnitDeviceType_t    deviceType, /* I: Device Type  */
    UInt32                      uHwAddress  /* I: Hardware Address */
);

extern tmErrorCode_t
tmsysOM3971GetHwAddress
(
    tmUnitSelect_t              tUnit,      /* I: FrontEnd unit number */
    tmOM3971UnitDeviceType_t    deviceType, /* I: Device Type  */
    UInt32*                     puHwAddress /* O: Hardware Address */
);

extern tmErrorCode_t tmsysOM3971I2CRead(tmUnitSelect_t tUnit, UInt32 AddrSize, UInt8* pAddr, UInt32 ReadLen, UInt8* pData);

extern tmErrorCode_t tmsysOM3971I2CWrite(tmUnitSelect_t tUnit, UInt32 AddrSize, UInt8* pAddr, UInt32 WriteLen, UInt8* pData);

extern tmErrorCode_t tmsysOM3971Wait(tmUnitSelect_t tUnit, UInt32 tms);

extern tmErrorCode_t tmsysOM3971Print(UInt32 level, const char* format, ...);
extern tmErrorCode_t tmsysOM3971GetBER(tmUnitSelect_t  tUnit, tmTuneReqType_t pathType, tmFrontEndFracNb32_t *psBER, UInt32 *puUncors);
extern tmErrorCode_t tmsysLID1149GetIQ(tmUnitSelect_t tUnit, tmTuneReqType_t tTuneReqType, UInt32* puIQ);

extern tmErrorCode_t
tmsysOM3971SetLoDetuningMode
(
    Bool     bLoDetuning                    /* I: LO detuning Mode : Activated or not  */
);

extern tmErrorCode_t
tmsysOM3971ResetUBK
(
    tmUnitSelect_t      tUnit  /* I: FrontEnd unit number */
);

#ifdef __cplusplus
}
#endif

#endif /* TMSYSOM3971_H */
/*============================================================================*/
/*                            END OF FILE                                     */
/*============================================================================*/

