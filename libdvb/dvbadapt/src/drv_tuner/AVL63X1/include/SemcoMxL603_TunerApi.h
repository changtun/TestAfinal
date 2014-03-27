/*******************************************************************************
 *
 * FILE NAME          : MxL601_TunerApi.h
 * 
 * AUTHOR             : Sunghoon Park 
 *                      Dong Liu
 *
 * DATE CREATED       : 07/10/2011
 *                      11/26/2011
 *
 * DESCRIPTION        : This file is header file for MxL601_TunerApi.cpp 
 *
 *******************************************************************************
 *                Copyright (c) 2011, MaxLinear, Inc.
 ******************************************************************************/

#ifndef __MXL603_TUNER_API_H__
#define __MXL603_TUNER_API_H__

/******************************************************************************
    Include Header Files
    (No absolute paths - paths handled by make file)
******************************************************************************/

#include "MaxLinearDataTypes.h"

/******************************************************************************
    Macros
******************************************************************************/

#define MXL603_VERSION_SIZE   4

/******************************************************************************
    User-Defined Types (Typedefs)
******************************************************************************/
 
typedef struct
{
  UINT8 chipId;      
  UINT8 chipVersion;
  UINT8 mxlwareVer[MXL603_VERSION_SIZE];
} MXL603_VER_INFO_T, *PMXL603_VER_INFO_T;

typedef enum
{
  MXL603_XTAL_16MHz = 0,
  MXL603_XTAL_24MHz
} MXL603_XTAL_FREQ_E;

typedef struct
{
  MXL603_XTAL_FREQ_E xtalFreqSel; 
  UINT8 xtalCap;  
  MXL_BOOL clkOutEnable;
  MXL_BOOL clkOutDiv;
  MXL_BOOL clkOutExt;
  MXL_BOOL singleSupply_3_3V;
} MXL603_XTAL_SET_CFG_T, *PMXL603_XTAL_SET_CFG_T;

typedef enum
{
  MXL603_PWR_MODE_SLEEP = 0,
  MXL603_PWR_MODE_ACTIVE,
  MXL603_PWR_MODE_STANDBY
} MXL603_PWR_MODE_E;

typedef enum
{
  MXL603_GPO_LOW = 0,
  MXL603_GPO_HIGH = 1,
  MXL603_GPO_AUTO_CTRL = 2
} MXL603_GPO_STATE_E;

typedef enum
{
  MXL603_DIG_DVB_C,
  MXL603_DIG_ISDBT_ATSC,
  MXL603_DIG_DVB_T,
  MXL603_DIG_J83B
} MXL603_SIGNAL_MODE_E;

typedef enum
{
  MXL603_IF_3_65MHz  = 0,
  MXL603_IF_4MHz     = 1,
  MXL603_IF_4_1MHz   = 2,
  MXL603_IF_4_15MHz  = 3,
  MXL603_IF_4_5MHz   = 4, 
  MXL603_IF_4_57MHz  = 5,
  MXL603_IF_5MHz     = 6,
  MXL603_IF_5_38MHz  = 7, 
  MXL603_IF_6Hz      = 8,
  MXL603_IF_6_28MHz  = 9,
  MXL603_IF_7_2MHz   = 10, 
  MXL603_IF_8_25MHz  = 11,
  MXL603_IF_35_25MHz = 12,
  MXL603_IF_36MHz    = 13, 
  MXL603_IF_36_15MHz = 14, 
  MXL603_IF_36_65MHz = 15,
  MXL603_IF_44MHz    = 16
} MXL603_IF_FREQ_E;

typedef enum
{
  MXL603_AGC_SELF = 0,   
  MXL603_AGC_EXTERNAL = 1 
} MXL603_AGC_TYPE_E;

typedef struct
{
  UINT8 setPoint;      
  MXL_BOOL agcPolarityInverstion; 
  MXL603_AGC_TYPE_E agcType;   
} MXL603_AGC_CFG_T, *PMXL603_AGC_CFG_T;

typedef enum
{
  MXL603_BW_6MHz = 0x20,         
  MXL603_BW_7MHz = 0x21,         
  MXL603_BW_8MHz = 0x22          
} MXL603_BW_E;

typedef struct
{
  UINT32 freqInHz;
  MXL603_BW_E bandWidth; 
  MXL603_SIGNAL_MODE_E signalMode;
  MXL603_XTAL_FREQ_E xtalFreqSel;
} MXL603_CHAN_TUNE_CFG_T, *PMXL603_CHAN_TUNE_CFG_T;

typedef struct
{
  UINT8 gainLevel; 
  MXL_BOOL manualFreqSet;       
  MXL_BOOL ifInversion;
  UINT32 manualIFOutFreqInKHz; 
  MXL603_IF_FREQ_E ifOutFreq;   
}MXL603_IF_OUT_CFG_T, *PMXL603_IF_OUT_CFG_T;

/******************************************************************************
    Global Variable Declarations
******************************************************************************/

extern const UINT8 MxLWare603DrvVersion[];
extern const UINT8 MxLWare603BuildNumber;

/******************************************************************************
    Prototypes
******************************************************************************/
MXL_STATUS MxLWare603_API_CfgDrvInit(UINT8 devId, void* oemDataPtr);

MXL_STATUS MxLWare603_API_CfgDevSoftReset(UINT8 devId);
MXL_STATUS MxLWare603_API_CfgDevOverwriteDefaults(UINT8 devId, MXL_BOOL singleSupply_3_3V);
MXL_STATUS MxLWare603_API_CfgDevXtal(UINT8 devId, MXL603_XTAL_SET_CFG_T xtalCfg);
MXL_STATUS MxLWare603_API_CfgDevPowerMode(UINT8 devId, MXL603_PWR_MODE_E powerMode);
MXL_STATUS MxLWare603_API_CfgDevGPO(UINT8 devId, MXL603_GPO_STATE_E gpoState);
MXL_STATUS MxLWare603_API_ReqDevVersionInfo(UINT8 devId, MXL603_VER_INFO_T* mxlDevVerInfoPtr);
MXL_STATUS MxLWare603_API_ReqDevGPOStatus(UINT8 devId, MXL603_GPO_STATE_E* gpoStatusPtr);
MXL_STATUS MxLWare603_API_CfgTunerMode(UINT8 devId, 
                                       MXL603_SIGNAL_MODE_E signalMode,                                                                                                         
                                       MXL603_IF_FREQ_E ifOutFreq,
                                       MXL603_XTAL_FREQ_E xtalFreqSel,
                                       UINT8 ifOutGainLevel);
MXL_STATUS MxLWare603_API_CfgTunerOpMode(UINT8 devId, MXL_BOOL enable);
MXL_STATUS MxLWare603_API_CfgTunerStartTune(UINT8 devId, MXL_BOOL startTune);
MXL_STATUS MxLWare603_API_CfgTunerAGC(UINT8 devId, MXL603_AGC_CFG_T agcCfg);
MXL_STATUS MxLWare603_API_CfgTunerLoopThrough(UINT8 devId, MXL_BOOL loopThroughCtrl);
MXL_STATUS MxLWare603_API_CfgTunerChanTune(UINT8 devId, MXL603_CHAN_TUNE_CFG_T* chanTuneCfgPtr);
MXL_STATUS MxLWare603_API_CfgTunerIFOutParam(UINT8 devId, MXL603_IF_OUT_CFG_T ifOutCfg);
MXL_STATUS MxLWare603_API_ReqTunerAGCLock(UINT8 devId, MXL_BOOL* agcLockStatusPtr);
MXL_STATUS MxLWare603_API_ReqTunerLockStatus(UINT8 devId, MXL_BOOL* rfSynLockPtr, MXL_BOOL* refSynLockPtr);
MXL_STATUS MxLWare603_API_ReqTunerRxPower(UINT8 devId, REAL32* rxPwrPtr);
MXL_STATUS MxLWare603_API_ReqTunerAFCOffset(UINT8 devId, REAL32* afcOffsetinKHzPtr);

#endif /* __MXL601_TUNER_API_H__*/




