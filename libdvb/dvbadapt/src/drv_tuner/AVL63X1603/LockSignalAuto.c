/*
 *           Copyright 2007-2012 Availink, Inc.
 *
 *  This software contains Availink proprietary information and
 *  its use and disclosure are restricted solely to the terms in
 *  the corresponding written license agreement. It shall not be 
 *  disclosed to anyone other than valid licensees without
 *  written permission of Availink, Inc.
 *
 */



#include <stdio.h>
#include <memory.h>
#include "LockSignalAuto.h"

#define DEBUG												/*open the debug info*/

/*----------------------*
 | Firmware data buffer |
 *----------------------*/
extern AVL_uchar AVL63X1_FwData[];

/*--------------------*
 | I2C Server setting |
 *--------------------*/
#define SERVER_IP               "10.209.156.165"			/*I2C Server IP Address*/
#define	SERVER_PORT             88							/*I2C Server Port Number*/

/*---------------*
 | Demod setting |
 *---------------*/
#define CHIP_I2C_SLAVE_ADDR     AVL63X1_SA_0				/*demod I2C address*/
#define	CHIP_WORKING_MODE       AVL_DEMOD_MODE_DVBC			/*demod working mode*/

/*----------------------------*
 | inputted IF signal setting |
 *----------------------------*/
#define DVBC_IF_INPUT_TYPE          RX_REAL_IF_I			/*I channel is used*/
#define	DVBC_IF_INPUT_FREQUENCY     36*1000000				/*IF signal frequency*/    

/*-----------------------------*
 | outputted mpeg data setting |
 *-----------------------------*/
#define MPEG_SAMPLE_TYPE        AVL63X1_MPCM_RISING         /*data sample type*/
#define	MPEG_OUTPUT_MODE        AVL63X1_MPM_PARALLEL        /*output type*/ 
#define MPEG_OUTPUT_FORMAT_DVBC AVL63X1_MPF_TS              /*output format is TS*/
#define	MPEG_ERROR_BIT          MPEG_ERROR_BIT_DISABLE      /*error bit is not used*/

/*----------------------*
 | I2C repeater setting |
 *----------------------*/
#define TUNER0_REPEATER_TYPE        MESSAGE_TYPE            /*repeater type*/
#define	TUNER0_ADDRESS              0x60                   /*tuner I2C address*/ 
#define TUNER0_CLOCK_SPEED_KHZ      400                     /*I2C clock*/
#define TUNER1_REPEATER_TYPE        MESSAGE_TYPE            /*repeater type*/
#define	TUNER1_ADDRESS				0x61					/*tuner I2C address*/ 
#define TUNER1_CLOCK_SPEED_KHZ      200                     /*I2C clock*/

#define TUNER_CHANNEL_FREQUENCY     698*1000000             /*channel frequency*/
#define DVBC_SYMBOLRATE_Hz          6875000                 /*channel symbol rate*/

/*------------------*
 | global variables |
 *------------------*/
struct AVL63X1_Chip avl_chip;                               /*AVL63X1 demod chip object*/
struct AVL_Tuner tuner_info;                                /*tuner object*/

int demo_test_main(void)
{
	AVL63X1_ErrorCode return_code = AVL63X1_EC_OK;          /* return code container */
	int lock_status = 0;                                    /* lock status container */
	unsigned short signal_strength = 0;                     /* signal strength value container */
	int signal_quality = 0;                                 /* signal quality value container */
	AVL_DVBC_Channel sChannelDVBC;                          /* chnanel info container */

	int count = 1;
	/*-----------------------*
	 | connect to I2C Server |
	 *-----------------------*/
	return_code = AVL63X1_IBSP_Initialize();
	if(return_code != AVL63X1_EC_OK)
	{
		printf("Failed to initialize the BSP!\n");
		return return_code;
	}

	/*----------------------------*
	 | initialize demod and tuner |
	 *----------------------------*/
	return_code = AVL63X1_init();
	if(return_code != AVL63X1_EC_OK)
	{
		printf("Failed to initialize the AVL63X1 device!\n");
		return return_code;
	}

#ifdef DEBUG
	printf("Succeed to initlize the demod and tuner!\n\n");
#endif

	/*----------------------------------------*
	 | set channel frequency and lock channel |
	 *----------------------------------------*/
	tuner_info.m_uiSlaveAddress = TUNER1_ADDRESS;
	tuner_info.m_uiFrequency_Hz = TUNER_CHANNEL_FREQUENCY;
	tuner_info.m_pAVL_Chip = &avl_chip;
	return_code = lock_channel(&tuner_info);

	if(return_code != AVL63X1_EC_OK)
	{
		printf("Failed to lock channel!\n");
		return return_code;
	}

#ifdef DEBUG
	printf("Succeed to lock channel.\n");
#endif

	/*------------------------------*
	 | get the lock status of demod |
	 *------------------------------*/
	return_code = get_lock_status(&lock_status);
	if(return_code != AVL63X1_EC_OK)
	{
		printf("Failed to get the lock status!\n");
		return return_code;
	}
	if(lock_status == 1)
	{
		//Demod is locked
#ifdef DEBUG
		printf("The channel is in locked state.\n\n");
#endif	
	}
	else
	{
		//Demod is not locked
		printf("The channel is unlocked.\n\n");
		return return_code;
	}

	/*---------------------*
	 | get the signal info |
	 *---------------------*/
#if	1
	return_code = get_signal_info(&sChannelDVBC);
	if(return_code != AVL63X1_EC_OK)
	{
		printf("Failed to get the signal info!\n");
		return return_code;
	}
	
	/*-------------------------*
	 | display the signal info |
	 *-------------------------*/
	display_signal_info(&(sChannelDVBC.m_SignalInfo));

	/*--------------------------------*
	 | get the strength of the signal |
	 *--------------------------------*/
	 	#endif
	AVL63X1_IBSP_Delay(2000);

	return_code = get_signal_strength(&signal_strength);
	if(return_code != AVL63X1_EC_OK)
	{
		printf("Failed to get the signal strength!\n");
		return return_code;
	}
	printf("The signal strength is %d%%.\n\n", signal_strength);

	/*-------------------------------*
	 | get the quality of the signal |
	 *-------------------------------*/
	return_code = get_signal_quality(&signal_quality);
	if(return_code != AVL63X1_EC_OK)
	{
		printf("Failed to get the signal quality!\n");
		return return_code;
	}
	printf("The signal quality is %d%%.\n\n",signal_quality);

	/*-----------------------*
	 | get PER of the signal |
	 *-----------------------*/
	#if 1
	float per;
	return_code = get_per(&per);
	if(return_code != AVL63X1_EC_OK)
	{
		printf("Failed to get the per!\n");
		return return_code;
	}
	printf("The PER is %e.\n\n",per);
#endif
	/*--------------------------------*
	| get BER before RS of the signal |
	*---------------------------------*/
	float ber;
	return_code = get_ber_beforeRS(&ber);
	if (return_code != AVL63X1_EC_OK)
	{
		printf("Failed to get the ber!\n");
		return return_code;
	}
	printf("The BER is %e.\n\n", ber);

	/*----------------------*
	| get SNR of the signal |
	*-----------------------*/
	float snr;
	return_code = get_snr(&snr);
	if (return_code != AVL63X1_EC_OK)
	{
		printf("Failed to get the SNR! return_code %d\n",return_code);
		return return_code;
	}
	printf("The SNR is %.2f.\n\n", snr);

	return return_code;
}

AVL63X1_ErrorCode AVL63X1_init(void)
{
	AVL63X1_ErrorCode return_code = AVL63X1_EC_OK;

	memset(&avl_chip,0,sizeof(struct AVL63X1_Chip));

	return_code = demod_init();
	if(return_code != AVL63X1_EC_OK)
	{
		//printf("Failed to initialize the demod!\n");
		return return_code;
	}

	return_code = tuner_init();
	if(return_code != AVL63X1_EC_OK)
	{
		//printf("Failed to initialize the tuner!\n");
		return return_code;
	}

	return return_code;
}

AVL63X1_ErrorCode demod_init(void)
{
	AVL63X1_ErrorCode return_code = AVL63X1_EC_OK;

	avl_chip.m_SlaveAddr = AVL63X1_SA_0;
	avl_chip.m_current_demod_mode = CHIP_WORKING_MODE;	
	avl_chip.m_pInitialData = (AVL_puchar)AVL63X1_FwData;
	avl_chip.m_InputSignal_Info.m_RxInputType_DVBC_mode = DVBC_IF_INPUT_TYPE;
	avl_chip.m_InputSignal_Info.m_InputFrequency_Hz_DVBC_mode = DVBC_IF_INPUT_FREQUENCY;
	avl_chip.m_MPEG_Info.m_mpcm = MPEG_SAMPLE_TYPE;
	avl_chip.m_MPEG_Info.m_mpfm_DVBC_mode = MPEG_OUTPUT_FORMAT_DVBC;
	avl_chip.m_MPEG_Info.m_mpm = MPEG_OUTPUT_MODE;
	avl_chip.m_MPEG_Info.m_MPEG_ErrorBit = MPEG_ERROR_BIT;

	return_code = AVL63X1_Initialize(&avl_chip);
	if(return_code != AVL63X1_EC_OK)
	{
		printf("AVL63X1_Initialize Failed!\n");
	}

	return_code = AVL63X1_SetAnalogAGC_Pola(AGC_NORMAL, &avl_chip);
	if(return_code != AVL63X1_EC_OK)
	{
		printf("AVL63X1_SetAnalogAGC_Pola Failed!\n");
	}

	return_code = AVL63X1_DriveIFAGC(AVL63X1_ON, &avl_chip);
	if(return_code != AVL63X1_EC_OK)
	{
		printf("AVL63X1_DriveIFAGC Failed!\n");
	}

	return_code = AVL63X1_SetMPEG_Mode(&avl_chip);
	if(return_code != AVL63X1_EC_OK)
	{
		printf("AVL63X1_SetMPEG_Mode Failed!\n");
	}

	return_code = AVL63X1_DriveMpegOutput(AVL63X1_ON, &avl_chip);
	if(return_code != AVL63X1_EC_OK)
	{
		printf("AVL63X1_DriveMpegOutput Failed!\n");
	}

	return return_code;
}

AVL63X1_ErrorCode tuner_init(void)
{
	AVL63X1_ErrorCode return_code = AVL63X1_EC_OK;


	avl_chip.m_RepeaterInfo[0].m_TunerAddress = TUNER0_ADDRESS;
	avl_chip.m_RepeaterInfo[0].m_RepeaterClockKHz = TUNER0_CLOCK_SPEED_KHZ;
	avl_chip.m_RepeaterInfo[0].m_uiRepeaterMode = TUNER0_REPEATER_TYPE;

	avl_chip.m_RepeaterInfo[1].m_TunerAddress = TUNER1_ADDRESS;
	avl_chip.m_RepeaterInfo[1].m_RepeaterClockKHz = TUNER1_CLOCK_SPEED_KHZ;
	avl_chip.m_RepeaterInfo[1].m_uiRepeaterMode = TUNER1_REPEATER_TYPE;
	
	return return_code;
}

AVL63X1_ErrorCode lock_channel(struct AVL_Tuner *ptuner)
{
	AVL63X1_ErrorCode return_code = AVL63X1_EC_OK;
	AVL_uint32 times_out_cnt;
	AVL_DVBC_Channel sChannelDVBC;
	AVL_uint16 uiLockStatus;
	//AVL_uint16 uiNumLockRetries;

	/*------------*
	 | lock tuner |
	 *------------*/
	return_code = lock_tuner(&tuner_info);
	if(return_code != AVL63X1_EC_OK)
	{
		printf("lock_tuner Failed!\n");
		return return_code;
	}
	if(tuner_info.m_sStatus.m_uiLock == 1)
	{
#ifdef DEBUG
	printf("Lock tuner at %d MHz.\n\n", ptuner->m_uiFrequency_Hz/1000000);
#endif
	}
	else
	{
		printf("Can not lock the tuner!\n");
		return return_code;
	}

	sChannelDVBC.m_demod_mode = CHIP_WORKING_MODE;
	sChannelDVBC.pAVLChip = &avl_chip;
	sChannelDVBC.m_SpectrumInversion = SPECTRUM_NOT_INVERTED;
	sChannelDVBC.m_uiSymbolRate_Hz = DVBC_SYMBOLRATE_Hz;

	/*-----------------*
	 |   lock channel  |
	 *-----------------*/
	return_code = AVL63X1_LockChannel(LOCK_MODE_AUTO, &sChannelDVBC);
	if(return_code != AVL63X1_EC_OK){
		//printf("Failed to lock the channel!\n");
		return return_code;
	}

	/*-----------------------------------*
	 |check the lock status of the demod |
	 *-----------------------------------*/
	times_out_cnt = 0;
	while(times_out_cnt < 500)//this time-out window can be customized
	{
		uiLockStatus = 0;

		//return_code=AVL63X1_GetLockRetries(&uiNumLockRetries,&avl_chip);
		//if(return_code!=AVL63X1_EC_OK){
		//	printf("AVL63X1_GetLockRetries Failed!\n");
		//	return return_code;
		//}
		//if(uiNumLockRetries>MAX_LOCK_RETRIES)
		//{
		//	uiLockStatus=0;
		//	break;
		//}

		//get the lock status
		return_code = AVL63X1_GetLockStatus(&uiLockStatus, &avl_chip);
		if(return_code != AVL63X1_EC_OK)
		{
			printf("AVL63X1_GetLockStatus Failed!\n");
			return return_code;
		}
		if(uiLockStatus == 1)
		{
			return_code = AVL63X1_EC_OK;
			printf("get_lock times_out_cnt = %d\n",times_out_cnt);
			break;
		}
		AVL63X1_IBSP_Delay(15);
		times_out_cnt++;
	}

	if(uiLockStatus == 0)
	{
		return_code = -1;// this constant -1 can be replaced by your own definition
		return return_code;
	}

	return return_code;
}


AVL63X1_ErrorCode lock_tuner(struct AVL_Tuner *ptuner)
{
	AVL63X1_ErrorCode return_code = AVL63X1_EC_OK;

	MxL603_Initialize(ptuner);
	/*-------------------------------------*
	 |lock tuner using specified function  |
	 *-------------------------------------*/
	return_code = MxL603_Lock(ptuner);

	if(return_code != AVL63X1_EC_OK)
	{
		printf("Failed to lock Tuner using lock function!\n");
		return return_code;
	}	
	AVL63X1_IBSP_Delay(150);

	/*check the tuner is locked*/
	return_code = MxL603_GetStatus(ptuner);
	if(return_code != AVL63X1_EC_OK)
	{
		printf("Failed to get the state of Tuner!\n");
		return return_code;
	}
	if(ptuner->m_sStatus.m_uiLock == 0)
	{
		printf("Failed to get the state of Tuner!\n");
		return return_code;
	}

	return return_code;
}

AVL63X1_ErrorCode get_lock_status(int *lock_status)
{
	AVL63X1_ErrorCode return_code = AVL63X1_EC_OK;

	//get the lock status
	return_code = AVL63X1_GetLockStatus((AVL_puint16)lock_status, &avl_chip);
	if(return_code != AVL63X1_EC_OK)
	{
		printf("AVL63X1_GetLockStatus Failed!\n");
		return return_code;
	}

	return return_code;
}

AVL63X1_ErrorCode get_signal_info(AVL_DVBC_Channel *dvbc_channel)
{
	AVL63X1_ErrorCode return_code = AVL63X1_EC_OK;
	
	AVL63X1_GetSignalInfo(&(dvbc_channel->m_SignalInfo), &avl_chip);
	if(return_code != AVL63X1_EC_OK)
	{
		//printf("Failed to get the signal info!\n");
		return return_code;
	}

	return return_code;
}

void display_signal_info(void *pSignalInfo)
{
	char *ModulationDVBC[5] = { "16 QAM", "32 QAM", "64 QAM", "128 QAM", "256 QAM" };

	AVL_uint32 uiModulationCode;
	AVL_DVBC_SignalInfo *pSignalInfoDVBC;

	pSignalInfoDVBC = (AVL_DVBC_SignalInfo *)pSignalInfo;
	uiModulationCode = (AVL_uint32)pSignalInfoDVBC->m_Modulation;

	printf("%s.\n\n",ModulationDVBC[uiModulationCode]);
}

AVL63X1_ErrorCode get_signal_strength(unsigned short *strength)
{
	AVL63X1_ErrorCode return_code = AVL63X1_EC_OK;

	return_code = AVL63X1_GetStrength((AVL_puint16)strength, &avl_chip);
	if(return_code != AVL63X1_EC_OK)
	{
		//printf("Failed to get the signal strength!\n");
		return return_code;
	}

	/*-------------------------------------------------------------------------*
	 | Following calculation between AGC and signal strength is used for Sharp |
	 | tuner.For more info, please contact with Avainlink.                     |
	 *-------------------------------------------------------------------------*/
	if(*strength >= 10000 && *strength <= 27000)
	{
		*strength = (int)(((float)(*strength)-10000) /(27000-10000)*100);
	}
	else //larger than 27000
	{
		*strength=100;
	}

	return return_code;
}

AVL63X1_ErrorCode get_signal_quality(int *quality)
{
	int return_code = AVL63X1_EC_OK;

	return_code = AVL63X1_GetSignalQuality((AVL_puint32)quality, 0, &avl_chip);
	if(return_code != AVL63X1_EC_OK)
	{
		//printf("Failed to get the signal quality!\n");
		return return_code;
	}

	return return_code;
}

AVL63X1_ErrorCode get_per(float *per)
{
	AVL63X1_ErrorCode return_code = AVL63X1_EC_OK;
	AVL63X1_ErrorStats err_state;
	
	return_code = AVL63X1_StartPER(&avl_chip, &err_state);
	if(return_code != AVL63X1_EC_OK)
	{
		return return_code;
	}

	AVL63X1_IBSP_Delay(1000);// wait 1s for calculation

	return_code = AVL63X1_GetPER(&err_state, &avl_chip);
	if(return_code != AVL63X1_EC_OK)
	{
		return return_code;
	}

	*per = (float)(err_state.m_PER/1e9);

	return return_code;
}

AVL63X1_ErrorCode get_snr(float *snr)
{
	AVL63X1_ErrorCode return_code = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;

	return_code = AVL_DVBC_GetSNR(&uiTemp, &avl_chip);
	if (return_code != AVL63X1_EC_OK)
	{
		return return_code;
	}

	*snr = (float)(uiTemp/100.0);

	return return_code;
}

AVL63X1_ErrorCode get_ber_beforeRS(float *ber)
{
	AVL63X1_ErrorCode return_code = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;

	return_code = AVL63X1_DVBC_GetBER_BeforeRS(&uiTemp, &avl_chip);
	if (return_code != AVL63X1_EC_OK)
	{
		return return_code;
	}

	*ber = (float)(uiTemp/1e9);

	return return_code;
}
