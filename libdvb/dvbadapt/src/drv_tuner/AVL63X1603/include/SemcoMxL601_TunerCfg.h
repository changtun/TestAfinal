/*******************************************************************************
 *
 * FILE NAME          : MxL601_TunerCfg.h
 * 
 * AUTHOR             : Dong Liu
 *  
 * DATE CREATED       : 11/16/2011
 *
 * DESCRIPTION        : This file contains MxL601 common control register 
 *                      definitions
 *
 *******************************************************************************
 *                Copyright (c) 2011, MaxLinear, Inc.
 ******************************************************************************/

#ifndef __SEMCOMXL601_TUNER_CFG_H__
#define __SEMCOMXL601_TUNER_CFG_H__

/******************************************************************************
    Include Header Files
    (No absolute paths - paths handled by make file)
******************************************************************************/

#include "SemcoMxL601_OEM_Drv.h"
#include "SemcoMxL601_TunerSpurTable.h"

/******************************************************************************
    Macros
******************************************************************************/

#define CUSTOMER_SPECIFIC_SETTING_1         

#define SEM_AIC_RESET_REG                  0xFF // For MxL601 Tuner

#define SEM_PAGE_CHANGE_REG                0x00 // Page change, can configured as page 0 or page 1 
#define SEM_XTAL_CAP_CTRL_REG              0x01 // Xtal frequency and CAP register 
#define SEM_XTAL_ENABLE_DIV_REG            0x02 // Xtal enable and frequency div 4 register 
#define SEM_XTAL_CALI_SET_REG              0x03 // Xtal calibration enable register enable register 
#define SEM_IF_FREQ_SEL_REG                0x04 // IF frequency selection and manual set bypass register  
#define SEM_IF_PATH_GAIN_REG               0x05 // IF gain level and path selection register  
#define SEM_IF_FCW_LOW_REG                 0x06 // Low register of IF FCW set when manual program IF frequency   
#define SEM_IF_FCW_HIGH_REG                0x07 // High register of IF FCW set when manual program IF frequency 
#define SEM_AGC_CONFIG_REG                 0x08 // AGC configuration, include AGC select and AGC type  
#define SEM_AGC_SET_POINT_REG              0x09
#define SEM_AGC_FLIP_REG                   0x5E
#define SEM_AGC_SLOPE_REG                  0xB5
#define SEM_AGC_OFFSET_REG                 0xB4
#define SEM_GPO_SETTING_REG                0x0A // GPO set and inquiring register 
#define SEM_TUNER_ENABLE_REG               0x0B // Power up register, bit<0>
#define SEM_TUNE_MODE_REG                  0x0D
#define SEM_MAIN_REG_AMP                   0x0E
#define SEM_CHAN_TUNE_BW_REG               0x0F // Band width register 
#define SEM_CHAN_TUNE_LOW_REG              0x10 // Tune frequency set low byte 
#define SEM_CHAN_TUNE_HI_REG               0x11 // Tune frequency set high byte 
#define SEM_START_TUNE_REG                 0x12 // sequencer setting register 
#define SEM_FINE_TUNE_SET_REG              0x13 // Fine tune operation register 
#define SEM_FINE_TUNE_CTRL_REG_0           0x13 // Fine tune operation register 
#define SEM_FINE_TUNE_CTRL_REG_1           0x14 // Fine tune operation register 

#define SEM_FINE_TUNE_OFFSET_LOW_REG       0x14 // Fine tune frequency offset low byte 
#define SEM_FINE_TUNE_OFFSET_HIGH_REG      0x15 // Fine tune frequency offset high byte
#define SEM_CHIP_ID_REQ_REG                0x18 // Tuner Chip ID register 
#define SEM_CHIP_VERSION_REQ_REG           0x1A // Tuner Chip Revision register 

#define SEM_RFPIN_RB_LOW_REG               0x1D // RF power low 8 bit register 
#define SEM_RFPIN_RB_HIGH_REG              0x1E // RF power high 8 bit register 
#define SEM_SIGNAL_TYPE_REG                0x1E // Signal type

#define SEM_DFE_CTRL_ACCUM_LOW_REG         0x24 // Bit<7:0> 
#define SEM_DFE_CTRL_ACCUM_MID_REG         0x25 // Bit<7:0>
#define SEM_DFE_CTRL_ACCUM_HI_REG          0x26 // Bit<1:0>

#define SEM_DFE_CTRL_TRIG_REG              0xA0 // Bit<3>
#define SEM_DFE_CTRL_RB_HI_REG             0x7B // Bit<7:0>
#define SEM_DFE_CTRL_RB_LOW_REG            0x7A // Bit<1:0>

#define SEM_RF_REF_STATUS_REG              0x2B // RF/REF lock status register 

#define SEM_AGC_SAGCLOCK_STATUS_REG        0x2C 

#define SEM_DFE_DACIF_BYP_GAIN_REG         0x43 
#define SEM_DIG_ANA_RFRSSI_REG             0x57 

#define SEM_RSSI_RESET_REG                 0x78

#define SEM_FINE_TUNE_INIT1_REG            0xA9 
#define SEM_FINE_TUNE_INIT2_REG            0xAA

#define SEM_DFE_AGC_CEIL1_REG              0xB0 

#define SEM_DFE_RFLUT_BYP_REG              0xDB  // Dec: 220, bit<7>
//#define DFE_RFLUT_DIV_IN_BYP_REG       0xDB  // Dec: 220, bit<6>
#define SEM_DFE_RFLUT_DIV_MOD_REG          0xDB  // Dec: 221 
//#define DFE_RFSX_INT_MOD_REG           0xDB  // Dec: 222

#define SEM_DFE_RFLUT_SWP1_REG             0x49 

#define SEM_DFE_RFSX_FRAC_MOD1_REG         0xDF
#define SEM_DFE_RFSX_FRAC_MOD2_REG         0xE0
#define SEM_DFE_RFSX_FRAC_MOD3_REG         0xE1
#define SEM_DFE_RFSX_FRAC_MOD4_REG         0xE2

#define SEM_DFE_REFLUT_BYP_REG             0xEA  // Dec: 240, bit<6>
#define SEM_DFE_REFSX_INT_MOD_REG          0xEB  // Dec: 241

#define SEM_APP_MODE_FREQ_HZ_THRESHOLD_1   358000000
#define SEM_APP_MODE_FREQ_HZ_THRESHOLD_2   625000000

#define SEM_SPUR_SHIFT_FREQ_WINDOW         500000  // +- 0.5MHz

#define SEM_IF_GAIN_SET_POINT1             10 
#define SEM_IF_GAIN_SET_POINT2             11 
#define SEM_IF_GAIN_SET_POINT3             12 

#define SEM_SPUR_SHIFT_CLOCK_ADJUST_MIN    205 
#define SEM_SPUR_SHIFT_CLOCK_ADJUST_MAX    227 

#define SEM_DIG_ANA_IF_CFG_0              0x5A
#define SEM_DIG_ANA_IF_CFG_1              0x5B
#define SEM_DIG_ANA_IF_PWR                0x5C

#define SEM_DFE_CSF_SS_SEL                0xEA
#define SEM_DFE_DACIF_GAIN                0xDC

#define SEM_FINE_TUNE_FREQ_DECREASE       0x01
#define SEM_FINE_TUNE_FREQ_INCREASE       0x02

#define SEM_RF_SX_FRAC_N_RANGE            0xDD

#define SEM_HIGH_IF_35250_KHZ             35250

/******************************************************************************
    User-Defined Types (Typedefs)
******************************************************************************/

typedef struct
{
  UINT8 regAddr;
  UINT8 mask;
  UINT8 data;
} SEM_REG_CTRL_INFO_T, *SEM_PREG_CTRL_INFO_T;

/******************************************************************************
    Global Variable Declarations
******************************************************************************/
extern SEM_REG_CTRL_INFO_T MxL_OverwriteDefaults[];
extern SEM_REG_CTRL_INFO_T AnalogNtsc[];
extern SEM_REG_CTRL_INFO_T AnalogPal[];
extern SEM_REG_CTRL_INFO_T AnalogSecam[];
extern SEM_REG_CTRL_INFO_T DigitalDvbc[];
extern SEM_REG_CTRL_INFO_T DigitalIsdbtAtsc[];
extern SEM_REG_CTRL_INFO_T DigitalDvbt[];

extern SEM_REG_CTRL_INFO_T Ntsc_RfLutSwpHIF[];
extern SEM_REG_CTRL_INFO_T Ntsc_16MHzRfLutSwpLIF[];
extern SEM_REG_CTRL_INFO_T Ntsc_24MHzRfLutSwpLIF[];

extern SEM_REG_CTRL_INFO_T Pal_RfLutSwpLIF[];
extern SEM_REG_CTRL_INFO_T PalD_RfLutSwpHIF[];
extern SEM_REG_CTRL_INFO_T PalI_RfLutSwpHIF[];
extern SEM_REG_CTRL_INFO_T PalBG_8MHzBW_RfLutSwpHIF[];
extern SEM_REG_CTRL_INFO_T PalBG_7MHzBW_RfLutSwpHIF[];
extern SEM_REG_CTRL_INFO_T Ntsc_HRCRfLutSwpLIF[];


/******************************************************************************
    Prototypes
******************************************************************************/

// Functions for register write operation 
MXL_STATUS SemcoCtrl_ProgramRegisters(UINT8 I2cAddr, SEM_PREG_CTRL_INFO_T ctrlRegInfoPtr);
MXL_STATUS SemcoCtrl_WriteRegField(UINT8 I2cAddr, SEM_PREG_CTRL_INFO_T ctrlRegInfoPtr); 

// Functions called by MxLWare API
MXL_STATUS SemcoCtrl_SetRfFreqLutReg(UINT8 i2cAddress, UINT32 FreqInHz, PSEM_CHAN_DEPENDENT_FREQ_TABLE_T freqLutPtr);

#endif /* __MXL601_TUNER_CFG_H__*/




