/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : DVBCore
* File name   : dvbcore.h
* Description : Definition of common types of DVBCore.
*
* History :
*   2012-04-27 ZYJ
*       Initial Version.
*******************************************************************************/

#ifndef __DVBCORE_H_
#define __DVBCORE_H_

#include "pvddefs.h"

#ifdef __cplusplus
extern "C" {
#endif


#define DVBC_SUPPORT
//#define DVBT_SUPPORT
//#define DMBT_SUPPORT



#define PV_SUCCESS          (0)
#define PV_FAILURE          (-1)
#define PV_INVALID_PARAM    (-2)
#define PV_NULL_PTR         (-3)
#define PV_NO_MEMORY        (-4)


#define PBI_SUCCESS         (0)
#define PBI_FAILURE         (-1)
#define PBI_INVALID_PARAM   (-2)
#define PBI_NULL_PTR        (-3)
#define PBI_NO_MEMORY       (-4)


/* QAM 调制类型 */
typedef enum
{
    eDVBCORE_MOD_16QAM = 0,
    eDVBCORE_MOD_32QAM,
    eDVBCORE_MOD_64QAM,
    eDVBCORE_MOD_128QAM,
    eDVBCORE_MOD_256QAM,
    eDVBCORE_MOD_512QAM,
    eDVBCORE_MOD_QPSK,
} DVBCore_Modulation_e;


typedef enum
{
    eDVBCORE_CONV_CODE_RATE_AUTO = 0,
    eDVBCORE_CONV_CODE_RATE_1_2,            /* 1/2 convolutional code rate */
    eDVBCORE_CONV_CODE_RATE_2_3,            /* 2/3 convolutional code rate */
    eDVBCORE_CONV_CODE_RATE_3_4,            /* 3/4 convolutional code rate */
    eDVBCORE_CONV_CODE_RATE_5_6,            /* 5/6 convolutional code rate */
    eDVBCORE_CONV_CODE_RATE_7_8,            /* 7/8 convolutional code rate */
    eDVBCORE_NO_CONV_CODING,                /* no convolutional coding */
} DVBCore_Conv_Code_Rate_e;


typedef enum
{
    eDVBCORE_FEC_OUTER_NOT_DEFINED = 0,
    eDVBCORE_FEC_OUTER_NO_CODING,           /* no outer FEC coding */
    eDVBCORE_FEC_OUTER_RS_204_188,          /* RS(204/188) Reed-Solomon codes */
} DVBCore_FEC_Outer_e;

typedef enum
{
    eDVBCORE_BW_8MHZ,
    eDVBCORE_BW_7MHZ,
    eDVBCORE_BW_6MHZ
} DVBCore_Bandwidth;/*频带宽度*/


typedef enum
{
    eDVBCORE_POLAR_LINEAR_HORIZONTAL = 0,
    eDVBCORE_POLAR_LINEAR_VERTICAL,
    eDVBCORE_POLAR_CIRCULAR_LEFT,
    eDVBCORE_POLAR_CIRCULAR_RIGHT
} DVBCore_Polarization_e;


typedef enum
{
    eDVBCORE_BANDWIDTH_8MHz = 0,
    eDVBCORE_BANDWIDTH_7MHz,
    eDVBCORE_BANDWIDTH_6MHz,
    eDVBCORE_BANDWIDTH_5MHz
} DVBCore_BandWidth_e;


typedef enum
{
    eDVBCORE_NON_HIERARCHICAL = 0,
    eDVBCORE_HIERARCHICAL_ALPHA1,
    eDVBCORE_HIERARCHICAL_ALPHA2,
    eDVBCORE_HIERARCHICAL_ALPHA3
} DVBCore_Hierarchical_Info_e;


typedef enum
{
    eDVBCORE_GUARD_INTERVAL_1_32 = 0,
    eDVBCORE_GUARD_INTERVAL_1_16,
    eDVBCORE_GUARD_INTERVAL_1_8,
    eDVBCORE_GUARD_INTERVAL_1_4
} DVBCore_Guard_Interval_e;


typedef enum
{
    eDVBCORE_TRANSMISSION_MODE_2K = 0,
    eDVBCORE_TRANSMISSION_MODE_8K,
    eDVBCORE_TRANSMISSION_MODE_4K
} DVBCore_Transmission_Mode_e;


typedef enum
{
	eDVBCORE_SIGNAL_SOURCE_DVBS,
	eDVBCORE_SIGNAL_SOURCE_DVBS2,
	eDVBCORE_SIGNAL_SOURCE_DVBC,
	eDVBCORE_SIGNAL_SOURCE_DVBT,
	eDVBCORE_SIGNAL_SOURCE_DMBT,
	eDVBCORE_SIGNAL_SOURCE_ANALOG
} DVBCore_Signal_Source_e;


typedef struct
{
    DVBCore_Signal_Source_e     eSignalSource;
    U8                          u8TunerId;
    U32                         u32Freq;
    U32                         u32SymRate;
    U16                         u16OrbitalPosition;
    U8                          u8WestEastFlag;         /* 0 - west; 1 - east */
    DVBCore_Polarization_e      ePolarization;
    DVBCore_Modulation_e        eMod;
    DVBCore_Conv_Code_Rate_e    eFEC_Inner;
} DVBCore_Sat_Desc_t;


typedef struct
{
    DVBCore_Signal_Source_e     eSignalSource;
    U8                          u8TunerId;
    U32					        u32Freq;
    U32					        u32SymbRate;
    DVBCore_Modulation_e        eMod;
    DVBCore_Conv_Code_Rate_e    eFEC_Inner;
    DVBCore_FEC_Outer_e         eFEC_Outer;
	/* BEGIN: Added by zshang, 2013/6/17 */
	DVBCore_Bandwidth		Bandwidth;			/*频宽*/
	/* END:   Added by zshang, 2013/6/17 */
} DVBCore_Cab_Desc_t;


typedef struct
{
    DVBCore_Signal_Source_e     eSignalSource;
    U8                          u8TunerId;
    U32                         u32CentreFreq;
    U8                          u8OtherFreqFlag;
    U8                          u8BandWidth;
    DVBCore_Modulation_e        eConstellation;
    DVBCore_Hierarchical_Info_e eHierarchyInfo;
    DVBCore_Conv_Code_Rate_e    eCodeRateHP_Stream;
    DVBCore_Conv_Code_Rate_e    eCodeRateLP_Stream;
    DVBCore_Guard_Interval_e    eGuardInterval;
    DVBCore_Transmission_Mode_e eTransmissionMode;
} DVBCore_Ter_Desc_t;


typedef struct
{
    DVBCore_Signal_Source_e     eSignalSource;
    U8                          u8TunerId;
    U32						    u32FreqKHz;	            /*单位:1000Hz*/
    U8					        u8AnalogPal;
} DVBCore_Ana_Desc_t;


typedef union
{
    DVBCore_Signal_Source_e     eSignalSource;
    DVBCore_Sat_Desc_t          tSatellite;
    DVBCore_Cab_Desc_t          tCable;
    DVBCore_Ter_Desc_t          tTerrestrial;
    DVBCore_Ana_Desc_t          tAnalogue;
} DVBCore_Tuner_Desc_u;


#ifdef __cplusplus
}
#endif

#endif /* __DVBCORE_H_ */

