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



#include "AVL63X1_include.h"

/// This file will include all the functions declared in AVL_DTMB_IBase.h and their implementation is same as functions in IBase.c in DTMB SDK.
AVL63X1_ErrorCode AVL_DTMB_Halt( struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_lock_status, &uiTemp);
	uiTemp &= 0xFFFFFEFF;
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_lock_status, uiTemp);

	return(r);
}

AVL63X1_ErrorCode AVL_DTMB_Wakeup( struct AVL63X1_Chip * pAVL_Chip )
{    
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;    
	AVL_uint32 uiTemp;
	AVL_uint32 latencyValue;
	
	// Configure the SDRAM controller
	r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_sdram_clk_Hz, pAVL_Chip->m_SDRAMFrequency_Hz);
	uiTemp = (((AVL_uint32)7) << 16);
	uiTemp |= (((AVL_uint32)10) << 8);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_sdram_config , uiTemp);
	latencyValue = ConvertNsToSDRAM_Clocks(pAVL_Chip->m_SDRAMFrequency_Hz, pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_TRC);
	uiTemp = ((latencyValue - 1) << 24);
	latencyValue = ConvertNsToSDRAM_Clocks(pAVL_Chip->m_SDRAMFrequency_Hz, pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_TXSR);
	uiTemp |= ((latencyValue - 1) << 16);
	latencyValue = ConvertNsToSDRAM_Clocks(pAVL_Chip->m_SDRAMFrequency_Hz, pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_TRCAR);
	uiTemp |= ((latencyValue - 1) << 8);
	uiTemp |= (AVL_uint32)(pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_TWR - 1);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_sdram_timing_config_0 , uiTemp);
	latencyValue = ConvertNsToSDRAM_Clocks(pAVL_Chip->m_SDRAMFrequency_Hz, pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_TRP);
	uiTemp = ((latencyValue - 1) << 24);
	latencyValue = ConvertNsToSDRAM_Clocks(pAVL_Chip->m_SDRAMFrequency_Hz, pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_TRCD);
	uiTemp |= ((latencyValue - 1) << 16);
	latencyValue = ConvertNsToSDRAM_Clocks(pAVL_Chip->m_SDRAMFrequency_Hz, pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_TRAS);
	uiTemp |= ((latencyValue - 1) << 8);
	uiTemp |= (AVL_uint32)(pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_CAS_Latency - 1);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_sdram_timing_config_1, uiTemp);	
	
	//send command to init sdram
	r |= IBase_SendRxOP_63X1(OP_RX_INIT_SDRAM, pAVL_Chip);

	r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_dmd_clk_Hz, pAVL_Chip->m_DemodFrequency_Hz);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_fec_clk_Hz, pAVL_Chip->m_FECFrequency_Hz);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_sdram_clk_Hz, pAVL_Chip->m_SDRAMFrequency_Hz);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_sample_rate_Hz, pAVL_Chip->m_RefFrequency_Hz);
	
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_ddc_input_select, pAVL_Chip->m_RxInput);
	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_iq_comp_control, &uiTemp);
	uiTemp &= 0xFFFEFFFF;
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_iq_comp_control, uiTemp);
	
	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_equalizer_control, &uiTemp);
	uiTemp |=  0x01;
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_equalizer_control, uiTemp);

	r |= AVL63X1_SetAnalogAGC_Pola(AGC_NORMAL,	pAVL_Chip);	
	return(r);
}

AVL63X1_ErrorCode AVL_DTMB_GetRunningLevel( enum AVL63X1_RunningLevel *pRunningLevel,  struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uchar uiState;
	
	r = II2C_Read8_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_demod_status, &uiState);
	if(uiState == 0)
	{
		*pRunningLevel = AVL63X1_RL_HALT;
	}
	else
	{
		*pRunningLevel = AVL63X1_RL_NORMAL;
	}

	return(r);
}


AVL_uint32 Table_420 [4][3] = {
								{15, 	 35,   	 56},
						 		{73, 	 96,	130},
						 		{0,  	  0,	155},
						 		{121,	164,	187}
					   		  };

///
/// Defines Threshold SNR for Header Size 945

AVL_uint32 Table_945 [4][3] =  {
								 {14,		34,		 55},
		    					 {72,		96,		130},
								 {0,		0,		155},
								 {120,		164,	190}
							   };

///
/// Defines Threshold SNR for Header Size 595

AVL_uint32 Table_595 [4][3] = {
	    						 {15,		35,		71},
		  						 {73,		96,		130},
								 {0,		0,		159},
								 {0,		0,		0}
					   		  };

#define THRESHOLD_NR_595	25

// This file will include all the functions declared in AVL_DTMB_IRx.h and their implementation is same as functions in IRx.c in DTMB SDK.
AVL63X1_ErrorCode AVL_DTMB_GetSpectrumInversion(enum AVL63X1_SpectrumInversion * pSpectrumInversion, const struct AVL63X1_Chip * pAVL_Chip)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;
	AVL_int32 iFrequencyValue;

	if(pAVL_Chip->m_iInputFrequencyHz == 0)
	{
		r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_ddc_input_select, &uiTemp);
		uiTemp = ((uiTemp >> 8) & 0x00000001);
		*pSpectrumInversion = (enum AVL63X1_SpectrumInversion)uiTemp;
	}
	else
	{
		r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_nom_carrier_freq_Hz, (AVL_puint32)&iFrequencyValue);
		if(iFrequencyValue == (pAVL_Chip->m_iInputFrequencyHz - pAVL_Chip->m_RefFrequency_Hz))
		{
			*pSpectrumInversion = SPECTRUM_NOT_INVERTED;
		}
		else
		{
			*pSpectrumInversion = SPECTRUM_INVERTED;
		}
	}

	return(r);
}

AVL63X1_ErrorCode AVL_DTMB_GetLockStatus( AVL_puint16 puiLockStatus, AVL_puint16 puiNumLockRetries, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp1;
	AVL_uint16 uiTemp2;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_lock_status, &uiTemp1);
	*puiLockStatus = (AVL_uint16)((uiTemp1 >> 8) & 0x00000001);

	r |= II2C_Read16_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_num_lock_retries, &uiTemp2);
	*puiNumLockRetries = uiTemp2;

	return(r);
}


AVL63X1_ErrorCode AVL_DTMB_GetCarrierFreqOffset( AVL_pint32 piFreqOffsetHz, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;
	AVL_int32 iTemp;
	struct AVL63X1_int64 iProduct;
	AVL_uint32 uiBinOffset;
	AVL_uint32 uiRefFreqHz;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_sample_rate_Hz, &uiRefFreqHz);
	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_cfo_freq_est, &uiTemp);
	iTemp = (AVL_int32)(uiTemp << 2);
	iTemp >>= 2;
	Multiply32By16(&iProduct, iTemp, (AVL_int16)(uiRefFreqHz/(AVL_int16)10000));
	iTemp = (AVL_int32)(((iProduct.m_HighWord & 0x000001FF) << 23) + (iProduct.m_LowWord >> 9));
	Multiply32By16(&iProduct, iTemp, 625);
	iTemp = (AVL_int32)(((iProduct.m_HighWord & 0x0001FFFF) << 15) + (iProduct.m_LowWord >> 17));
	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_carrier_freq_bin_Hz, &uiBinOffset);
	iTemp -= (AVL_int32)uiBinOffset;

	*piFreqOffsetHz = iTemp;

	return(r);
}

AVL63X1_ErrorCode AVL_DTMB_GetScatterData( AVL_puint32 puiIQ_Data, AVL_puint16 puiSize, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint16 uiSize;
	AVL_uint32 i;
	AVL_puchar pucData;
	AVL_uint32 uiTemp;
	enum AVL_DTMB_CarrierMode enumCarrierMode;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_mode_status, &uiTemp);
	enumCarrierMode = (enum AVL_DTMB_CarrierMode)((uiTemp >> 16) & 0x00000001);
	uiSize = *puiSize;
	if(uiSize > 256)
	{
		uiSize = 256;
	}
	pucData = (AVL_puchar)puiIQ_Data;
	r |= II2C_Read_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_scatter_data, pucData, (uiSize << 2));
	for(i=0; i<uiSize; i++)
	{
		uiTemp = DeChunk32(pucData);
		*puiIQ_Data = uiTemp;
		puiIQ_Data++;
		pucData += 4;
	}
	

	if(r != AVL63X1_EC_OK)
	{
		uiSize = 0;
	}
	*puiSize = uiSize;

	return(r);
}

AVL63X1_ErrorCode AVL_DTMB_GetSignalInfo(struct AVL_DTMB_SignalInfo *pSignalInfo, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_mode_status, &uiTemp);
	pSignalInfo->m_HeaderMode = (enum AVL_DTMB_FrameHeaderMode)(uiTemp & 0x00000003);
	pSignalInfo->m_CodeRate = (enum AVL_DTMB_FEC_Rate)((uiTemp >> 8) & 0x00000003);
	pSignalInfo->m_CarrierMode = (enum AVL_DTMB_CarrierMode)((uiTemp >> 16) & 0x00000001);
	pSignalInfo->m_InterleaveMode = (enum AVL_DTMB_InterleaveMode)((uiTemp >> 24) & 0x00000001);

	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_modulation_status, &uiTemp);
	pSignalInfo->m_Modulation = (enum AVL_DTMB_ModulationMode)(uiTemp & 0x00000003);
	pSignalInfo->m_MappingMode = (enum AVL_DTMB_NR_Mode)((uiTemp >> 8) & 0x00000001);

	return(r);
}

AVL63X1_ErrorCode AVL_DTMB_GetSignalLevel(AVL_puint16 puiSignalLevel , const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;
	AVL_uint16 uiSignalLevel;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_aagc_gain, &uiTemp);
	if( AVL63X1_EC_OK == r)
	{
		uiTemp += 0x2000000;
		uiTemp &= 0x3ffffff;
		uiSignalLevel = (AVL_uint16)(uiTemp >> 10);
		*puiSignalLevel = uiSignalLevel;
	}

	return(r);
}

AVL63X1_ErrorCode AVL_DTMB_GetSNR( AVL_puint32 puiSNR_db, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_channel_status, &uiTemp);
	*puiSNR_db = (uiTemp & 0x0000FFFF);

	return(r);
}

AVL63X1_ErrorCode AVL_DTMB_Initialize(struct AVL63X1_Chip * pAVL_Chip)
{
	AVL63X1_ErrorCode r;
	AVL_uint32 uiTemp;
	AVL_uint32 latencyValue;

	r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_dmd_clk_Hz, pAVL_Chip->m_DemodFrequency_Hz);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_fec_clk_Hz, pAVL_Chip->m_FECFrequency_Hz);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_sdram_clk_Hz, pAVL_Chip->m_SDRAMFrequency_Hz);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_sample_rate_Hz, pAVL_Chip->m_RefFrequency_Hz);

	uiTemp = (((AVL_uint32)7) << 16);
	uiTemp |= (((AVL_uint32)10) << 8);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_sdram_config, uiTemp);
		
	latencyValue = ConvertNsToSDRAM_Clocks(pAVL_Chip->m_SDRAMFrequency_Hz, pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_TRC);
	uiTemp = ((latencyValue - 1) << 24);
	latencyValue = ConvertNsToSDRAM_Clocks(pAVL_Chip->m_SDRAMFrequency_Hz, pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_TXSR);
	uiTemp |= ((latencyValue - 1) << 16);
	latencyValue = ConvertNsToSDRAM_Clocks(pAVL_Chip->m_SDRAMFrequency_Hz, pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_TRCAR);
	uiTemp |= ((latencyValue - 1) << 8);
	uiTemp |= (AVL_uint32)(pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_TWR - 1);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_sdram_timing_config_0, uiTemp);
	latencyValue = ConvertNsToSDRAM_Clocks(pAVL_Chip->m_SDRAMFrequency_Hz, pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_TRP);
	uiTemp = ((latencyValue - 1) << 24);
	latencyValue = ConvertNsToSDRAM_Clocks(pAVL_Chip->m_SDRAMFrequency_Hz, pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_TRCD);
	uiTemp |= ((latencyValue - 1) << 16);
	latencyValue = ConvertNsToSDRAM_Clocks(pAVL_Chip->m_SDRAMFrequency_Hz, pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_TRAS);
	uiTemp |= ((latencyValue - 1) << 8);
	uiTemp |= (AVL_uint32)(pAVL_Chip->m_pSDRAM_Conf->m_SDRAM_CAS_Latency - 1);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_sdram_timing_config_1, uiTemp);	

	//send command to init sdram
	r |= IBase_SendRxOP_63X1(OP_RX_INIT_SDRAM, pAVL_Chip);
	//This delay is needed to make sure the command gets processed by the firmware incase it is busy doing something else.

	uiTemp = ((((AVL_uint32)pAVL_Chip->m_InputSignal_Info.m_RxInputType_DTMB_mode) >> 1) << 24) | 0x00010001;
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_ddc_input_select, uiTemp);
	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_iq_comp_control, &uiTemp);
	uiTemp &= 0xFFFEFFFF;
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_iq_comp_control, uiTemp);
	if(pAVL_Chip->m_InputSignal_Info.m_RxInputType_DTMB_mode == RX_COMPLEX_BASEBAND)
	{
		pAVL_Chip->m_iInputFrequencyHz = 0;
	}
	else
	{
		pAVL_Chip->m_iInputFrequencyHz = pAVL_Chip->m_InputSignal_Info.m_InputFrequency_Hz_DTMB_mode;
		r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_ddc_input_select, &uiTemp);
		if(pAVL_Chip->m_InputSignal_Info.m_RxInputType_DTMB_mode == RX_REAL_IF_I)
		{
			uiTemp = (uiTemp & 0xFFFFFEFF);
		}
		else
		{
			uiTemp = (uiTemp & 0xFFFFFEFF);
			uiTemp |= 0x00000100;
		}
		pAVL_Chip->m_RxInput = uiTemp;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_ddc_input_select, uiTemp);
	}
	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_equalizer_control, &uiTemp);
	uiTemp |=  0x01;
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_equalizer_control, uiTemp);

	
	return(r);
}

AVL63X1_ErrorCode AVL_DTMB_LockChannel( struct AVL63X1_Chip * pAVL_Chip, enum AVL63X1_LockMode enumLockMode, const struct AVL_DTMB_Channel * psChannel )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	enum AVL63X1_SpectrumInversion enumSpectrumInversionCfg;
	AVL_uint32 uiTemp;
	struct AVL_DTMB_SignalInfo *pSignalInfo;
	AVL_int32 iFrequencyValue;

	if(enumLockMode == LOCK_MODE_MANUAL)
	{
		pSignalInfo = (struct AVL_DTMB_SignalInfo *)&psChannel->m_SignalInfo;
		if((pSignalInfo->m_Modulation == RX_DTMB_32QAM) ||
			((pSignalInfo->m_Modulation == RX_DTMB_4QAM) && (pSignalInfo->m_MappingMode == RX_NR_ON)))
		{
			if(pSignalInfo->m_CodeRate != RX_FEC_4_5)
			{
				r |= AVL63X1_EC_GENERAL_FAIL;
				return(r);
			}
		}
	}

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_acquisition_control, &uiTemp);

	switch(enumLockMode)
	{
	case LOCK_MODE_AUTO:
		uiTemp |= 0x00000001;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_acquisition_control, uiTemp);
		break;
	case LOCK_MODE_MANUAL:
		uiTemp &= 0xFFFFFFFE;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr),rc_AVL63X1_acquisition_control, uiTemp);
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_mode_status, &uiTemp);
		uiTemp &= 0xFEFEFCFF;
		uiTemp |= (((AVL_uint32)(pSignalInfo->m_InterleaveMode << 24)) | ((AVL_uint32)(pSignalInfo->m_CarrierMode << 16)) | ((AVL_uint32)(pSignalInfo->m_CodeRate << 8)));
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_mode_status, uiTemp);
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_modulation_status, &uiTemp);
		uiTemp &= 0xFFFFFEFC;
		uiTemp |= (((AVL_uint32)(pSignalInfo->m_MappingMode << 8)) | ((AVL_uint32)pSignalInfo->m_Modulation));
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_modulation_status, uiTemp);
		break;
	}
	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_modulation_status, &uiTemp);
	uiTemp &= 0xFFFEFFFF;
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_modulation_status, uiTemp);
	enumSpectrumInversionCfg = psChannel->m_SpectrumInversion;

	if(pAVL_Chip->m_iInputFrequencyHz == 0)
	{
		iFrequencyValue = 0;
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_ddc_input_select, &uiTemp);
		uiTemp = ((uiTemp & 0xFFFFFCFF) | (((AVL_uint32)enumSpectrumInversionCfg) << 8));
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_ddc_input_select, uiTemp);
	}
	else
	{
		iFrequencyValue = pAVL_Chip->m_iInputFrequencyHz - pAVL_Chip->m_RefFrequency_Hz;
		//if(enumSpectrumInversionCfg == SPECTRUM_AUTO)
		//{
		//	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_ddc_input_select, &uiTemp);
		//	uiTemp &= 0xFFFFFDFF;
		//	uiTemp |= 0x00000200;
		//	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_ddc_input_select, uiTemp);
		//}
		//else
		//{
			//The SPECTRUM_NOT_INVERTED operations
			r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_ddc_input_select, &uiTemp);
			uiTemp &= 0xFFFFFDFF;
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_ddc_input_select, uiTemp);

			if(enumSpectrumInversionCfg == SPECTRUM_INVERTED)
			{
				iFrequencyValue = -iFrequencyValue;
			}
		//}
	}
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_nom_carrier_freq_Hz, iFrequencyValue);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_demod_lock_status, 0);
	r |= IBase_SendRxOP_63X1(OP_RX_HALT	, pAVL_Chip);	
	if(r == AVL63X1_EC_OK)
	{
		//This delay is needed to make sure the command gets processed by the firmware incase it is busy doing something else.
		r |= AVL63X1_IBSP_Delay(2);
		r |= IBase_SendRxOP_63X1(OP_RX_ACQUIRE, pAVL_Chip);
	}

	return(r);
}


AVL63X1_ErrorCode AVL_DTMB_GetSignalQuality( AVL_puint32 puiSignal_Quality, AVL_int32 iNormalization_Factor,const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiEstimated_SNR = 0;
	AVL_DTMB_SignalInfo SignalInfo;
	AVL_int32 iThreshold_SNR = 0, window;
	AVL_int32 iSignal_Quality = 0,iEstimated_SNR = 0;

	if ((iNormalization_Factor < 4) || (iNormalization_Factor >6))
	{
		iNormalization_Factor = 4;
	}

	r = AVL_DTMB_GetSNR(&uiEstimated_SNR, pAVL_Chip);

	if(r != AVL63X1_EC_OK)
	{
		*puiSignal_Quality = 0;
		return(r);
	}

	r |= AVL_DTMB_GetSignalInfo(&SignalInfo, pAVL_Chip);

	if(r != AVL63X1_EC_OK)
	{
		*puiSignal_Quality = 0;
		return(r);
	}

	iEstimated_SNR = uiEstimated_SNR;

	if (SignalInfo.m_MappingMode == RX_NR_ON)
		iThreshold_SNR = THRESHOLD_NR_595;
	else if (SignalInfo.m_HeaderMode == RX_PN420)
		iThreshold_SNR	 =	Table_420[SignalInfo.m_Modulation][SignalInfo.m_CodeRate];
	else if (SignalInfo.m_HeaderMode == RX_PN595)
		iThreshold_SNR	 =	Table_595[SignalInfo.m_Modulation][SignalInfo.m_CodeRate];
	else if (SignalInfo.m_HeaderMode == RX_PN945)
		iThreshold_SNR	 =	Table_945[SignalInfo.m_Modulation][SignalInfo.m_CodeRate];


    if ((iThreshold_SNR == 0) || (iEstimated_SNR == 0))
	{
		 *puiSignal_Quality = 0;
	}
	else
		{

			if ((iThreshold_SNR * 2) < 100)
				window = iThreshold_SNR * 2;
			else
				window = 100;

			iSignal_Quality = (iEstimated_SNR - (((iThreshold_SNR * 10) - (iNormalization_Factor * window))))/(window/10);

			if (iSignal_Quality > 100)
				*puiSignal_Quality = 100;
			else if (iSignal_Quality < 0)
				*puiSignal_Quality = 0;
			else
				*puiSignal_Quality = iSignal_Quality;
		}

	return(r);
}

AVL63X1_ErrorCode AVL_DTMB_SetDemodMode( AVL_uint32 uiSetMode,const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	if (uiSetMode == AUTO_MODE)
	{
		r = II2C_Write8_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr),rc_AVL_DTMB_demod_profile,1);
		r |= II2C_Write8_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr),rc_AVL_DTMB_mobile_static_det_en,1);
	}

	if (uiSetMode == MOBILE_MODE)
	{
		r = II2C_Write8_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr),rc_AVL_DTMB_demod_profile,1);
		r |= II2C_Write8_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr),rc_AVL_DTMB_mobile_static_det_en,0);
	}


	if (uiSetMode == STATIC_MODE)
	{
		r = II2C_Write8_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr),rc_AVL_DTMB_demod_profile,0);
		r |= II2C_Write8_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr),rc_AVL_DTMB_mobile_static_det_en,0);
	}

	return(r);

}

