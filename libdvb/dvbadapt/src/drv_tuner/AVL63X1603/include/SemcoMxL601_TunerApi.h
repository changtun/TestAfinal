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

#ifndef __SEMCOMXL601_TUNER_API_H__
#define __SEMCOMXL601_TUNER_API_H__

/******************************************************************************
    Include Header Files
    (No absolute paths - paths handled by make file)
******************************************************************************/

#include "MaxLinearDataTypes.h"
#include "SemcoMxL601_TunerCfg.h"

/******************************************************************************
    Macros
******************************************************************************/

#define  MXL_VERSION_SIZE         4   

/******************************************************************************
    User-Defined Types (Typedefs)
******************************************************************************/

/* Command Types  */
typedef enum
{ 
  // For API_ConfigDevice interface
  MXL_DEV_SOFT_RESET_CFG = 0,
  MXL_DEV_OVERWRITE_DEFAULT_CFG,
  MXL_DEV_XTAL_SET_CFG,
  MXL_DEV_POWER_MODE_CFG,
  MXL_DEV_IF_OUT_CFG,
  MXL_DEV_GPO_CFG,         

  // For API_GetDeviceStatus interface 
  MXL_DEV_ID_VERSION_REQ,
  MXL_DEV_GPO_STATE_REQ,             

  // For API_ConfigTuner interface
  MXL_TUNER_MODE_CFG,
  MXL_TUNER_POWER_UP_CFG,
  MXL_TUNER_START_TUNE_CFG,
  MXL_TUNER_AGC_CFG,
  MXL_TUNER_CHAN_TUNE_CFG,
  MXL_TUNER_SPUR_SHIFT_ADJ_CFG,
  MXL_TUNER_FINE_TUNE_CFG,
  MXL_TUNER_ENABLE_FINE_TUNE_CFG,
  MXL_TUNER_HLFRF_CFG,

  // For API_GetTunerStatus interface
  MXL_TUNER_SIGNAL_TYPE_REQ,
  MXL_TUNER_LOCK_STATUS_REQ, 
  MXL_TUNER_AGC_LOCK_REQ, 
  MXL_TUNER_RX_PWR_REQ,
  MXL_TUNER_AFC_CORRECTION_REQ,

  MAX_NUM_COMMAND
} SEM_MXL_CMD_TYPE_E;

/* MXL_DEV_SOFT_RESET_CFG  */
typedef struct
{
  UINT8 I2cSlaveAddr;           /* IN, I2C Address of MxL Device */
} SEM_MXL_RESET_CFG_T, *PSEM_MXL_RESET_CFG_T;

/* MXL_DEV_OVERWRITE_DEFAULT_CFG */
typedef struct
{
  UINT8 I2cSlaveAddr;           /* IN, I2C Address of MxL Device */
  MXL_BOOL SingleSupply_3_3V;   /* IN, Single Supply 3.3v */
} SEM_MXL_OVERWRITE_DEFAULT_CFG_T, *PSEM_MXL_OVERWRITE_DEFAULT_CFG_T;

/* MXL_DEV_XTAL_SET_CFG  */
typedef enum
{
  XTAL_16MHz = 0,
  XTAL_24MHz = 1,
  XTAL_FREQ_NA = 99
} SEM_MXL_XTAL_FREQ_E;

typedef struct
{
  UINT8 I2cSlaveAddr;           /* IN, I2C Address of MxL Device */
  SEM_MXL_XTAL_FREQ_E XtalFreqSel;  /* IN, XTAL Frequency, refers above */
  UINT8 XtalCap;                /* IN, XTAL capacity, 1 LSB = 1pF, maximum is 31pF */
  MXL_BOOL ClkOutEnable;        /* IN, enable or disable clock out */
  MXL_BOOL ClkOutDiv;           /* IN, indicate if XTAL frequency is dived by 4 or not */
  MXL_BOOL SingleSupply_3_3V;   /* IN, Single Supply 3.3v */
  MXL_BOOL XtalSharingMode;     /* IN, XTAL sharing mode. default Master, MXL_ENABLE to config Slave mode */
} SEM_MXL_XTAL_SET_CFG_T, *PSEM_MXL_XTAL_SET_CFG_T;

/* MXL_DEV_POWER_MODE_CFG  */
typedef enum
{
  MXL_PWR_MODE_SLEEP = 0,
  MXL_PWR_MODE_ACTIVE,
  MXL_PWR_MODE_STANDBY
} SEM_MXL_PWR_MODE_E;

typedef struct
{
  UINT8 I2cSlaveAddr;           /* IN, I2C Address of MxL Device */
  SEM_MXL_PWR_MODE_E PowerMode;     /* IN, power saving mode */
} SEM_MXL_PWR_MODE_CFG_T, *PSEM_MXL_PWR_MODE_CFG_T;

/* MXL_DEV_IF_OUT_CFG  */
typedef enum
{
  IF_PATH1 = 1,
  IF_PATH2 = 2
} SEM_MXL_IF_PATH_E;

typedef enum
{
  IF_3_65MHz  = 0,
  IF_4MHz     = 1,
  IF_4_1MHz   = 2,
  IF_4_15MHz  = 3,
  IF_4_5MHz   = 4, 
  IF_4_57MHz  = 5,
  IF_5MHz     = 6,
  IF_5_38MHz  = 7, 
  IF_6MHz     = 8,
  IF_6_28MHz  = 9,
  IF_7_2MHz   = 10, 
  IF_8_25MHz  = 11,
  IF_35_25MHz = 12,
  IF_36MHz    = 13, 
  IF_36_15MHz = 14, 
  IF_36_65MHz = 15,
  IF_44MHz    = 16,
  IF_FREQ_NA  = 99
} SEM_MXL_IF_FREQ_E;

typedef struct
{
  UINT8 I2cSlaveAddr;           /* IN, I2C Address of MxL Device */
  SEM_MXL_IF_FREQ_E IFOutFreq;      /* IN, band width of IF out signal */
  UINT32 ManualIFOutFreqInKHz;  /* IN, IF out frequency selection when ManualIFOutCtrl = 0*/
  MXL_BOOL ManualFreqSet;       /* IN, IF out frequency is set by manual or not */
  MXL_BOOL IFInversion;         /* IN, IF spectrum is inverted or not */
  UINT8 GainLevel;              /* IN, IF out gain level */
  SEM_MXL_IF_PATH_E PathSel;        /* IN, define which path is selected */
}SEM_MXL_IF_OUT_CFG_T, *PSEM_MXL_IF_OUT_CFG_T;

/* MXL_DEV_GPO_CFG  */
typedef enum
{
  GPO1 = 0,
  GPO2 = 1,
  GPO3 = 2
} SEM_MXL_GPO_ID_E;

typedef enum
{
  PORT_LOW = 0,
  PORT_HIGH = 1,
  PORT_AUTO_CTRL = 2
} SEM_MXL_GPO_STATE_E;

typedef struct
{
  UINT8 I2cSlaveAddr;           /* IN, I2C Address of MxL Device */
  SEM_MXL_GPO_ID_E GPOId;           /* IN, GPO port number. 0 for GPO0, 1 for GPO1, and 2 for GPO2 */
  SEM_MXL_GPO_STATE_E GPOState;     /* IN, configuration. */
} SEM_MXL_GPO_CFG_T, *PSEM_MXL_GPO_CFG_T;

/* MXL_DEV_ID_VERSION_REQ */
typedef struct
{
  UINT8 I2cSlaveAddr;                 /* IN, I2C Address of MxL Device */
  UINT8 ChipId;                       /* OUT, Device chip ID information  */
  UINT8 ChipVersion;                  /* OUT, Device chip revision  */
  UINT8 MxLWareVer[MXL_VERSION_SIZE]; /* OUT, MxLWare version information */    
} SEM_MXL_DEV_INFO_T, *PSEM_MXL_DEV_INFO_T;

/* MXL_DEV_GPO_STATE_REQ  */
typedef struct
{
  UINT8 I2cSlaveAddr;                 /* IN, I2C Address of MxL Device */   
  SEM_MXL_GPO_STATE_E GPO1;               /* OUT, GPO0 status */
  SEM_MXL_GPO_STATE_E GPO2;               /* OUT, GPO1 status */
  SEM_MXL_GPO_STATE_E GPO3;               /* OUT, GPO2 status */        
} SEM_MXL_GPO_INFO_T, *PSEM_MXL_GPO_INFO_T;

/* MXL_TUNER_MODE_CFG */
typedef enum
{
  ANA_NTSC_MODE = 0x0,
  ANA_PAL_BG = 0x01,
  ANA_PAL_I = 0x02,
  ANA_PAL_D = 0x03,
  ANA_SECAM_I = 0x04,
  ANA_SECAM_L = 0x05,
  DIG_DVB_C = 0x06, 
  DIG_ISDBT_ATSC = 0x07,
  DIG_DVB_T = 0x08,
  DIG_J83B = 0x09,
  MXL_SIGNAL_NA = 99
} SEM_MXL_SIGNAL_MODE_E;

typedef struct
{
  UINT8 I2cSlaveAddr;                 /* IN, I2C Address of MxL Device */
  SEM_MXL_SIGNAL_MODE_E SignalMode;       /* IN , Tuner application mode */
  UINT32 IFOutFreqinKHz;              /* IN, IF Frequency in KHz */
  SEM_MXL_XTAL_FREQ_E XtalFreqSel;        /* XTAL Frequency, refers above */
  UINT8 IFOutGainLevel;               /* IF out gain level */
} SEM_MXL_TUNER_MODE_CFG_T, *PSEM_MXL_TUNER_MODE_CFG_T;

/* MXL_TUNER_POWER_UP_CFG */
typedef struct
{
  UINT8 I2cSlaveAddr;                 /* IN, I2C Address of MxL Device */
  MXL_BOOL Enable;                    /* IN , enable or disable all sections of main path */
} SEM_MXL_POWER_UP_CFG_T, *PSEM_MXL_POWER_UP_CFG_T;

/* MXL_TUNER_START_TUNE_CFG */ 
typedef struct
{
  UINT8 I2cSlaveAddr;                 /* IN, I2C Address of MxL Device */
  MXL_BOOL StartTune;                 /* IN , sequence set, 0 means start sequence, 1 means finish */
} SEM_MXL_START_TUNE_CFG_T, *PSEM_MXL_START_TUNE_CFG_T;

/* MXL_TUNER_AGC_CFG */ 
typedef enum
{
  AGC2 = 0,
  AGC1 = 1
} SEM_MXL_AGC_ID_E;

typedef enum
{
  AGC_SELF = 0,
  AGC_EXTERNAL = 1
} SEM_MXL_AGC_TYPE_E;

typedef struct
{
  UINT8 I2cSlaveAddr;                 /* IN, I2C Address of MxL Device */
  SEM_MXL_AGC_ID_E AgcSel;                /* IN, AGC selection, AGC1 or AGC2 */
  SEM_MXL_AGC_TYPE_E AgcType;             /* IN, AGC mode selection, self or closed loop */
  UINT8 SetPoint;                     /* IN, AGC attack point set value */
  MXL_BOOL AgcPolarityInverstion;     /* IN, Config AGC Polarity inversion */
} SEM_MXL_AGC_SET_CFG_T, *PSEM_MXL_AGC_SET_CFG_T;

/* MXL_TUNER_CHAN_TUNE_CFG */
typedef enum
{
  ANA_TV_DIG_CABLE_BW_6MHz = 0x00,    /* Analog TV and Digital Cable Mode 6MHz */ 
  ANA_TV_DIG_CABLE_BW_7MHz = 0x01,    /* Analog TV and Digital Cable Mode 7MHz */
  ANA_TV_DIG_CABLE_BW_8MHz = 0x02,    /* Analog TV and Digital Cable Mode 8MHz */
  DIG_TERR_BW_6MHz = 0x20,            /* Digital Terrestrial Mode 6MHz */
  DIG_TERR_BW_7MHz = 0x21,            /* Digital Terrestrial Mode 7MHz */
  DIG_TERR_BW_8MHz = 0x22             /* Digital Terrestrial Mode 8MHz */
} SEM_MXL_BW_E;

typedef enum
{
  VIEW_MODE = 0, 
  SCAN_MODE 
} SEM_MXL_TUNE_TYPE_E;

typedef enum
{
  MXL_NTSC_CARRIER_NA = 0,
  MXL_NTSC_CARRIER_HRC = 1,
  MXL_NTSC_CARRIER_IRC = 2
} SEM_MXL_NTSC_CARRIERS_E;

typedef struct
{
  UINT8 I2cSlaveAddr;                 /* IN, I2C Address of MxL Device */
  SEM_MXL_TUNE_TYPE_E TuneType;           /* IN, Tune mode, view mode or channel scan mode */
  SEM_MXL_BW_E BandWidth;                 /* IN, band width in MHz */
  UINT32 FreqInHz;                    /* IN, Radio Frequency in Hz */
  SEM_MXL_SIGNAL_MODE_E SignalMode;       /* IN , Tuner application mode */
  SEM_MXL_XTAL_FREQ_E XtalFreqSel;        /* IN, Xtal frequency */
  SEM_MXL_NTSC_CARRIERS_E NtscCarrier;    /* IN, NTSC Carriers HRC or IRC (used for tunning NTSC channels only) */
  UINT32 IFOutFreqinKHz;              /* IN, IF Frequency in KHz */
} SEM_MXL_TUNER_TUNE_CFG_T, *PSEM_MXL_TUNER_TUNE_CFG_T;

/* MXL_TUNER_SPUR_SHIFT_ADJ_CFG */
typedef struct
{
  UINT8 I2cSlaveAddr;                 /* IN, I2C Address of MxL Device */
  UINT8 SpurShiftingClkAdjValue;      /* IN, Spur shifting clock adjust value, range [205, 227] */ 
} SEM_MXL_SPUR_SHIFT_ADJ_CFG_T, *PSEM_MXL_SPUR_SHIFT_ADJ_CFG_T;

/* MXL_TUNER_HLFRF_OPTIMIZATION_CFG  */
typedef struct
{
  UINT8 I2cSlaveAddr;           /* IN, I2C Address of MxL Device */
  MXL_BOOL Enable;              /* IN, Enable or Disable the control */
} SEM_MXL_HLFRF_CFG_T, *PSEM_MXL_HLFRF_CFG_T;

/* MXL_TUNER_FINE_TUNE_CFG */
typedef struct
{
  UINT8 I2cSlaveAddr;                 /* IN, I2C Address of MxL Device */
  MXL_BOOL ScaleUp;                   /* IN, 0 means decrease one step, 1 means increase one step */
} SEM_MXL_FINE_TUNE_CFG_T, *PSEM_MXL_FINE_TUNE_CFG_T;

/* MXL_TUNER_ENABLE_FINE_TUNE_CFG */
typedef struct
{
  UINT8 I2cSlaveAddr;                 /* IN, I2C Address of MxL Device */
  MXL_BOOL EnableFineTune;            /* IN, enable/disable fine tune */
} SEM_MXL_ENABLE_FINE_TUNE_CFG_T, *PSEM_MXL_ENABLE_FINE_TUNE_CFG_T;

/* MXL_TUNER_SIGNAL_TYPE_REQ */
typedef enum
{
  SIGNAL_TYPE_DIGITAL = 0,
  SIGNAL_TYPE_ANALOG 
} SEM_MXL_SIGNAL_TYPE_E;

typedef struct
{
  UINT8 I2cSlaveAddr;      
  SEM_MXL_SIGNAL_TYPE_E SignalMode;  
} SEM_MXL_SIGNAL_TYPE_T, *PSEM_MXL_SIGNAL_TYPE_T;

/* MXL_TUNER_LOCK_STATUS_REQ */
typedef struct
{
  UINT8 I2cSlaveAddr;                 /* IN, I2C Address of MxL Device */
  MXL_BOOL RfSynLock;                 /* OUT, Tuner RF synthesis lock Status */
  MXL_BOOL RefSynLock;                /* OUT, Tuner Ref synthesis lock Status */
} SEM_MXL_TUNER_LOCK_STATUS_T, *PSEM_MXL_TUNER_LOCK_STATUS_T;

/* MXL_TUNER_AGC_LOCK_REQ */
typedef struct
{
  UINT8 I2cSlaveAddr;                 /* IN, I2C Address of MxL Device */
  MXL_BOOL AgcLock;                   /* OUT, Tuner AGC lock Status */
} SEM_MXL_TUNER_AGC_LOCK_T, *PSEM_MXL_TUNER_AGC_LOCK_T;

/* MXL_TUNER_RX_PWR_REQ */
typedef struct
{
  UINT8 I2cSlaveAddr;                 /* IN, I2C Address of MxL Device */
  REAL32 RxPwr;                       /* OUT, Tuner RF Input Power in dBm */
} SEM_MXL_TUNER_RX_PWR_T, *PSEM_MXL_TUNER_RX_PWR_T;

/* MXL_TUNER_AFC_CORRECTION_REQ */
typedef struct
{
  UINT8 I2cSlaveAddr;                 /* IN, I2C Address of MxL Device */
  REAL64 AfcOffsetKHz;                /* OUT, Tuner AFC correction offset in kHz */
} SEM_MXL_TUNER_AFC_CORRECTION_T, *PSEM_MXL_TUNER_AFC_CORRECTION_T;

/* API COMMAND interface structure */
typedef struct
{
  SEM_MXL_CMD_TYPE_E commandId;  /* Command Identifier */

  union
  {
    SEM_MXL_RESET_CFG_T cmdResetCfg;
    SEM_MXL_OVERWRITE_DEFAULT_CFG_T cmdOverwriteDefault;
    SEM_MXL_XTAL_SET_CFG_T cmdXtalCfg;
    SEM_MXL_PWR_MODE_CFG_T cmdPwrModeCfg;
    SEM_MXL_IF_OUT_CFG_T cmdIfOutCfg;    
    SEM_MXL_GPO_CFG_T cmdGpoCfg;

    SEM_MXL_DEV_INFO_T cmdDevInfoReq;
    SEM_MXL_GPO_INFO_T cmdGpoReq;

    SEM_MXL_TUNER_MODE_CFG_T cmdModeCfg;
    SEM_MXL_POWER_UP_CFG_T cmdTunerPoweUpCfg;
    SEM_MXL_START_TUNE_CFG_T cmdStartTuneCfg;
    SEM_MXL_AGC_SET_CFG_T cmdAgcSetCfg; 
    SEM_MXL_TUNER_TUNE_CFG_T cmdChanTuneCfg;
    SEM_MXL_SPUR_SHIFT_ADJ_CFG_T cmdSuprShiftAdjCfg;
    SEM_MXL_FINE_TUNE_CFG_T cmdFineTuneCfg;
    SEM_MXL_ENABLE_FINE_TUNE_CFG_T cmdEnableFineTuneCfg;
    SEM_MXL_HLFRF_CFG_T cmdHlfrfCfg;

    SEM_MXL_SIGNAL_TYPE_T cmdTunerSignalTypeReq;
    SEM_MXL_TUNER_LOCK_STATUS_T cmdTunerLockReq;
    SEM_MXL_TUNER_RX_PWR_T cmdTunerPwrReq;
    SEM_MXL_TUNER_AGC_LOCK_T cmdTunerAgcLockReq;
    SEM_MXL_TUNER_AFC_CORRECTION_T cmdTunerAfcCorectionReq;
  } MxLIf;
} SEM_MXL_COMMAND_T;

/******************************************************************************
    Global Variable Declarations
******************************************************************************/

extern const UINT8 SEM_MxLWareDrvVersion[];
extern const UINT8 SEM_BuildNumber;

/******************************************************************************
    Prototypes
******************************************************************************/

#ifndef __MXL_GUI__
MXL_STATUS SemcoMxLWare601_API_ConfigDevice(SEM_MXL_COMMAND_T *ParamPtr);
MXL_STATUS SemcoMxLWare601_API_GetDeviceStatus(SEM_MXL_COMMAND_T *ParamPtr);
MXL_STATUS SemcoMxLWare601_API_ConfigTuner(SEM_MXL_COMMAND_T *ParamPtr);
MXL_STATUS SemcoMxLWare601_API_GetTunerStatus(SEM_MXL_COMMAND_T *ParamPtr);
#endif

#endif /* __MXL601_TUNER_API_H__*/




