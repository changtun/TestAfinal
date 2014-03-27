
/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : drv_tuner.h
* Description : Includes all other TUNER driver header files.
              	This allows a TUNER application to include only this one header
              	file directly. 
* Author : zheng dong sheng
* History :
*	2010/06/07 : Initial Version
******************************************************************************/
#ifndef _DRV_TUNER_H
#define _DRV_TUNER_H
#ifdef __cplusplus 
extern "C" { 
#endif 

#include "pvddefs.h"
/*******************************************************/
/*              Exported Defines and Macros                                     */
/*******************************************************/

/*******************************************************/
/*              Exported Types			                                        */
/*******************************************************/
typedef enum
{
    ePI_WEST = 0,
    ePI_EAST
}tPI_WestEast;

typedef enum
{
    ePI_LINEAR_HORIZONTAL = 0x01,	/*ˮƽ����*/
    ePI_LINEAR_VERTICAL = 0x02,		/*��ֱ����*/
    ePI_CIRCULAR_LEFT = 0x04,		/*��������*/
    ePI_CIRCULAR_RIGHT = 0x08,		/*��������*/
    ePI_POLARISATION_ALL = 0x0f
} tPI_Polarization;/*����:*/

typedef enum
{
	 ePI_NONE,
	 ePI_QPSK,
	 ePI_8PSK,
	 ePI_QAM,
	 ePI_4QAM,
	 ePI_16QAM,
	 ePI_32QAM,
	 ePI_64QAM,
	 ePI_128QAM,
	 ePI_256QAM,
	 ePI_512QAM,				/*added by zhengdongsheng on 10.06.07*/
	 ePI_BPSK,
	 ePI_8VSB,
	 ePI_16APSK,
	 ePI_32APSK,
	 ePI_ALL
} tPI_Modulation;/*����ģʽ*/

typedef enum 
{
    ePI_FECIN_DVBS_1_2 = 0,		/*FEC: 0x00*/
    ePI_FECIN_DVBS_2_3 = 1,		/*FEC: 0x01*/
    ePI_FECIN_DVBS_3_4 = 2,	/*FEC: 0x02*/
    ePI_FECIN_DVBS_5_6 = 3,		/*FEC: 0x03*/
    ePI_FECIN_DVBS_6_7 = 4,	/*FEC: 0x04*/
    ePI_FECIN_DVBS_7_8  = 5,
    ePI_FECIN_DVBS2_1_4  = 6,
    ePI_FECIN_DVBS2_1_3  = 7,
    ePI_FECIN_DVBS2_2_5  = 8,
    ePI_FECIN_DVBS2_1_2  = 9,
    ePI_FECIN_DVBS2_3_5  = 10,
    ePI_FECIN_DVBS2_2_3   = 11,
    ePI_FECIN_DVBS2_3_4  = 12,
    ePI_FECIN_DVBS2_4_5  = 13,
    ePI_FECIN_DVBS2_5_6  = 14,
    ePI_FECIN_DVBS2_8_9  = 15,
    ePI_FECIN_DVBS2_9_10  = 16
} tPI_FecInner;/*��Ч���������*/

typedef enum
{
    ePI_FECOUT_NOT_CODING,	/*No use*/
    ePI_FECOUT_RS_204_188	/*Add 16 bytes to 188*/
} tPI_FecOuter;/*ǰ�����*/
#if 0
typedef enum
{
    ePI_BW_8MHZ,
    ePI_BW_7MHZ,
	ePI_BW_6MHZ
} tPI_Bandwidth;/*Ƶ�����*/
#endif
#define	tPI_Bandwidth	int
typedef enum
{
    ePI_HI_NONE,
    ePI_HI_ALPHA1,
    ePI_HI_ALPHA2,
    ePI_HI_ALPHA4
} tPI_HierarchicalInfo;

typedef enum
{
    ePI_TM_2K,
    ePI_TM_8k
} tPI_TransmissionMode;

typedef enum
{
	ePI_GI_32,/*PN420*/
	ePI_GI_16,/*PN595*/
	ePI_GI_8,/*PN945*/
	ePI_GI_4,
	ePI_GI_NONE	
} tPI_GuardInterval;

typedef enum
{
 	eDISEQC_LNB_POWER_DEFAULT,
	eDISEQC_LNB_POWER_13V,
 	eDISEQC_LNB_POWER_18V
} eDISEQC_LNBPower;

typedef enum
{
	MEDIUM_TYPE_DVBS,
	MEDIUM_TYPE_DVBS2,
	MEDIUM_TYPE_DVBC,
	MEDIUM_TYPE_DVBT,
	MEDIUM_TYPE_DMBT,
	MEDIUM_TYPE_ANALOG
}MediumType_t;

/* Program Info Data Base satellite info structure */
typedef struct
{
    U32					FrequencyKHertz;	/*��λ:1000Hz*/
    U32					SymbolRateSPS; 		/* symbol per secondes */
    tPI_WestEast		WestEastFlag;		/*��������ʾ:0-����;1-����*/
    tPI_Polarization	Polarization;		/*���뼫��*/
    tPI_Modulation		Modulation;			/*���Ʒ�ʽ*/
    tPI_FecInner 		FecInner;			/*��Ч���ʱ���*/
	U16					OrbitalPosition;	/*������(��λ:0.1��)*/

}tPI_SatelliteDesc;

/* Program Info Data Base cable info structure */
typedef struct
{
    U32					FrequencyKHertz;	/*��λ:1000Hz*/
    U32					SymbolRateSPS;		/* symbol per secondes */	
    tPI_Modulation		Modulation;			/*���Ʒ�ʽ*/
    tPI_FecInner		FecInner;			/*��Ч���ʱ���*/
    tPI_FecOuter		FecOuter;			/*ǰ������ʾ*/
	/* BEGIN: Added by zhwu, 2013/3/8 */
	tPI_Bandwidth		Bandwidth;			/*Ƶ��*/
	/* END:   Added by zhwu, 2013/3/8 */
}tPI_CableDesc;

typedef struct
{
    U32					FrequencyKHertz;	/*��λ:1000Hz*/
    U8					AnalogPal;
}uPI_AnalogDesc;

/* Program Info Data Base terrestrial info structure */
typedef struct
{
    U32						FrequencyKHertz;	/*��λ:1000Hz*/
    U8						OtherFrequencyFlag;	/*����Ƶ�ʱ�ʶ*/
    tPI_Bandwidth			Bandwidth;			/*Ƶ��*/
    tPI_Modulation			Constellation; 		/*���Ʒ�ʽ*/
    tPI_FecInner			CodeRateHP;			/**/
    tPI_FecInner			CodeRateLP;			/**/	
    tPI_HierarchicalInfo	HierarchicalInfo;	/*Hierarchicalģʽ*/
    tPI_GuardInterval		GuardInterval;		/**/
    tPI_TransmissionMode	TransmissionMode;	/*the number of carriers in an OFDM frame.*/
}tPI_TerrestrialDesc;

typedef struct TUN_SIGNALINFO_S
{
	U32			uiStrengthValue;	/*signal level value from tuner*/
	U32			uiQualityValue;	/*SNR value from tuner*/
}Tun_SignalInfo_t;

typedef struct ts_src_info_s
{
	U32						uiSignalSouceID;	/*�ź�Դ��� ����ID*/
	MediumType_t 			MediumType;			/*����������*/
	union
	{
		tPI_SatelliteDesc		Satellite;		/*���Ǵ������*/
		tPI_TerrestrialDesc 	Terrestrial;	/*���洫�����*/
		tPI_CableDesc 			Cable;			/*���ߴ������*/
		uPI_AnalogDesc		Analog;
	}u;
}ts_src_info_t;/*��Ƶ����*/

typedef enum {
	eTUNER_NO_ERROR=0,
	eTUNER_NOT_DONE,
	eTUNER_BAD_PARAMETER,
	eTUNER_ERROR
} TUNER_ErrorCode_t;

typedef enum
{
	eTun_unLock=0,				/*ʧ��*/
	eTun_Lock,					/*����*/
	eTun_Lock_Err
}TUNER_LOCKSTATUS_t;/*����״̬*/

/*******************************************************/
/*              Exported Variables		                                        */
/*******************************************************/

/*******************************************************/
/*              External				                                        */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                                        */
/*******************************************************/
TUNER_ErrorCode_t DRV_Tuner_Init( void );
void DRV_Tuner_Deinit( void );
TUNER_ErrorCode_t DRV_Tuner_SetFrequency(U8 ucTuner_ID, ts_src_info_t *ptSrcInfo, U32 uiTimeOut );
TUNER_ErrorCode_t DRV_Tuner_GetTunInfoStrengthPercent( U8 Tuner_ID, U32 *puiSignalPower );
TUNER_ErrorCode_t DRV_Tuner_GetTunInfoQualityPercent( U8 Tuner_ID, U32 *puiSignalQuality );
TUNER_ErrorCode_t DRV_Tuner_GetTunInfoBer( U8 Tuner_ID, U32 *puiSignalBer );
TUNER_ErrorCode_t DRV_Tuner_GetTunInfoSnr( U8 Tuner_ID, U32 *puiSignalSnr );
TUNER_LOCKSTATUS_t DRV_Tuner_GetLockStatus( U8 ucTuner_ID );
/*qbxu add for test*/
void tuner_test(void);

#ifdef __cplusplus 
} 
#endif 

#endif  /* #ifndef _DRV_TUNER_H */
/* End of drv_tuner.h  --------------------------------------------------------- */

