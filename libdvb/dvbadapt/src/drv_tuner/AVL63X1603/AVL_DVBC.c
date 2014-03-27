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

// This file will include all the functions declared in AVL_DVBC_IBase.h and their implementation is same as functions in IBase.c in DVBC SDK.
AVL63X1_ErrorCode AVL_DVBC_Halt( struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_lock_status, &uiTemp);	
	uiTemp &= 0xFFFFFEFF;
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_lock_status, uiTemp);
	
	return(r);
}


AVL63X1_ErrorCode AVL_DVBC_Wakeup( struct AVL63X1_Chip * pAVL_Chip )
{    
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;    
	
	r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_dmd_clk_Hz, pAVL_Chip->m_DemodFrequency_Hz);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_fec_clk_Hz, pAVL_Chip->m_FECFrequency_Hz);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_sdram_clk_Hz, pAVL_Chip->m_SDRAMFrequency_Hz);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_sample_rate_Hz, pAVL_Chip->m_RefFrequency_Hz);
	
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_ddc_input_select, pAVL_Chip->m_RxInput);
	
	return(r);
}


AVL63X1_ErrorCode AVL_DVBC_GetRunningLevel( enum AVL63X1_RunningLevel *pRunningLevel,  struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiState;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_mode_status, &uiState);
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

#define OUTPUTx	 100
#define DATAx	 1000
#define LOG10E   4343  //LOG10(e0)*10000

AVL_int32	BlindEqu_G2_fxd[5] ={
    0x000fa1ca,
    0x000b3f7c,
    0x00106666,
    0x000b8937,
    0x000a9fbe
};

AVL_int32   a[]= { 
    3162, 1778,   1349   //3.162*DATAx
}; 

AVL_int32   b[]= { 
    50, 25,13     //0.5*OUTPUTx
}; 

AVL_int32  AVL_log10(AVL_int32   x) 
{ 
	AVL_int32   i; 
	AVL_int32   r=0;
	AVL_int32 y;
	AVL_int32 t;
	
	y = x;
	while(x >= 10) 
	{ 
		x/=10; 
		r++; 
	} 

	t = 1;
	for(i = 0; i < r; i++)
	{
		t = t*10;
	}

	y = y*DATAx/t;
	r = r*OUTPUTx;
	for(i = 0; i < 3; i++) 
	{ 
		if(y >= a[i]) 
		{ 
			y/=a[i]; 
			r+=b[i]; 
		}
	} 

	y -= 1*DATAx; 
	r += ((2* LOG10E * y)/(2*DATAx+y))/100;
	
	return   r; 
} 
/// This file will include all the functions declared in AVL_DVBC_IRx.h and their implementation is same as functions in IRx.c in DVBC SDK.
AVL63X1_ErrorCode AVL_DVBC_GetSpectrumInversion(enum AVL63X1_SpectrumInversion * pSpectrumInversion, const struct AVL63X1_Chip * pAVL_Chip)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_demod_lock_indicator, &uiTemp);
	if((uiTemp & 0xff) == 0)
	{
		*pSpectrumInversion = SPECTRUM_NOT_INVERTED;
	}
	else
	{
		*pSpectrumInversion = SPECTRUM_INVERTED;
	}
	return(r);
}


AVL63X1_ErrorCode AVL_DVBC_GetLockStatus( AVL_puint16 puiLockStatus, AVL_puint16 puiNumLockRetries, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp1;

    *puiLockStatus = 0;
	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), /*rs_AVL_DVBC_demod_status*/rs_AVL_DVBC_mode_status, &uiTemp1);
	if((r == AVL63X1_EC_OK) && (0x15 == ((uiTemp1)&0xff)))
	{
	    *puiLockStatus = 1;
	}
	
	//r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_num_lock_retries, &uiTemp1);
	*puiNumLockRetries = 0;

	return(r);
}


AVL63X1_ErrorCode AVL_DVBC_GetCarrierFreqOffset( AVL_pint32 piFreqOffsetHz, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;
	AVL_uint32 uiRefFreqHz;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_get_btr_crl, &uiTemp);
	if(0x00 == uiTemp)
	{
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_carrier_freq_offset_Hz, &uiRefFreqHz);
		*piFreqOffsetHz = uiRefFreqHz;
		// r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr),   rs_AVL_DVBC_timing_offset, (AVL_puint32)piTimeOffsetHz);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_get_btr_crl, 0x01);
	}
	else
	{
		r = AVL63X1_EC_RUNNING;
	}
	return(r);
}



AVL63X1_ErrorCode AVL_DVBC_GetScatterData( AVL_puint32 puiIQ_Data, AVL_puint16 puiSize, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint16 uiSize;
	AVL_uint32 i;
	AVL_puchar pucData;
	AVL_uint32 uiTemp;
	
	uiSize = *puiSize;	
	if(uiSize > 512)
	{
		uiSize = 512;
	}
	pucData = (AVL_puchar)puiIQ_Data;
	r = II2C_Read_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_scatter_data, pucData, (uiSize << 2));
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

AVL63X1_ErrorCode AVL_DVBC_GetSignalInfo(struct AVL_DVBC_SignalInfo *pSignalInfo, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_qam_mode, &uiTemp);
	pSignalInfo->m_Modulation = (enum AVL_DVBC_ModulationMode)(uiTemp & 0x00000007);

	return(r);
}

AVL63X1_ErrorCode AVL_DVBC_GetSignalLevel(AVL_puint16 puiSignalLevel , const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;
	AVL_uint16 uiSignalLevel;
	
	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_aagc_gain, &uiTemp);
	if( AVL63X1_EC_OK == r)
	{
		uiTemp += 0x2000000;
		uiTemp &= 0x3ffffff;
		uiSignalLevel = (AVL_uint16)(uiTemp >> 10);
		*puiSignalLevel = uiSignalLevel;
	}

	return(r);
}

AVL63X1_ErrorCode AVL_DVBC_GetSNR( AVL_puint32 puiSNR_db, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint16 uiSNR_x100db;
	AVL_uint32 uiTemp;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_get_btr_crl, &uiTemp);
	if(0x00 == uiTemp)
	{
		r = II2C_Read16_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_snr_dB_x100, &uiSNR_x100db);
		*puiSNR_db = (AVL_uint32)uiSNR_x100db;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_get_btr_crl, 0x01);
	}
	else
	{
		r = AVL63X1_EC_RUNNING;
	}

	return(r);
}

AVL63X1_ErrorCode AVL_DVBC_Initialize(struct AVL63X1_Chip * pAVL_Chip)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;  
	AVL_uint32 uiTemp;

	r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_dmd_clk_Hz, pAVL_Chip->m_DemodFrequency_Hz);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_fec_clk_Hz, pAVL_Chip->m_FECFrequency_Hz);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_sample_rate_Hz, pAVL_Chip->m_RefFrequency_Hz);
	
	uiTemp = ((((AVL_uint32)pAVL_Chip->m_InputSignal_Info.m_RxInputType_DVBC_mode) >> 1) << 24) | 0x00010001;
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_ddc_input_select, uiTemp);

	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_if_freq_Hz, pAVL_Chip->m_InputSignal_Info.m_InputFrequency_Hz_DVBC_mode);
	//	pAVL_Chip->m_iInputFrequencyHz = pAVL_Chip->m_InputSignal_Info.m_InputFrequency_Hz_DVBC_mode;
	//	pAVL_Chip->m_RxInput = uiTemp;

	if(pAVL_Chip->m_InputSignal_Info.m_RxInputType_DVBC_mode == RX_COMPLEX_BASEBAND)
	{
		pAVL_Chip->m_iInputFrequencyHz = 0;
	}
	else
	{
		pAVL_Chip->m_iInputFrequencyHz = pAVL_Chip->m_InputSignal_Info.m_InputFrequency_Hz_DVBC_mode;
		r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_ddc_input_select, &uiTemp);
		if(pAVL_Chip->m_InputSignal_Info.m_RxInputType_DVBC_mode == RX_REAL_IF_I)
		{
			uiTemp = (uiTemp & 0xFFFFFEFF);
		}
		else
		{
			uiTemp = (uiTemp & 0xFFFFFEFF);
			uiTemp |= 0x00000100;
		}
		pAVL_Chip->m_RxInput = uiTemp;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_ddc_input_select, uiTemp);
	}
	
	return(r);
}

AVL63X1_ErrorCode AVL_DVBC_LockChannel( struct AVL63X1_Chip * pAVL_Chip, enum AVL63X1_LockMode enumLockMode, const struct AVL_DVBC_Channel * psChannel )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp = 0;
	
    r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_symbol_rate_Hz, (psChannel->m_uiSymbolRate_Hz));
    r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_qam_mode, &uiTemp);					
    uiTemp &= 0xFFFFFF00;
    if(enumLockMode == LOCK_MODE_MANUAL)
    {    	
    	uiTemp |= ((AVL_uint32)(psChannel->m_SignalInfo.m_Modulation))<<0;
    	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_qam_mode, uiTemp);	
		r |= IBase_SendRxOP_63X1(OP_RX_ACQUIRE, pAVL_Chip);
    }
    else
    {
        uiTemp |= ((AVL_uint32)(RX_DVBC_16QAM))<<0;
        r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_qam_mode, uiTemp);	
        r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_qam_mode_scan_control, 0x0101);	
    }
       
	//r |= IBase_SendRxOP_63X1(OP_RX_ACQUIRE, pAVL_Chip);

	return(r);
}


AVL63X1_ErrorCode AVL_DVBC_GetSignalQuality( AVL_puint32 puiSignal_Quality, AVL_int32 iNormalization_Factor,const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiEstimated_SNR = 0;
	//AVL_DVBC_SignalInfo SignalInfo;
	AVL_int32 iThreshold_SNR = 0;
	AVL_int32 iSignal_Quality = 0,iEstimated_SNR = 0;

	if ((iNormalization_Factor < 4) || (iNormalization_Factor >6))
	{
		iNormalization_Factor = 4;
	}
	
	r = AVL_DVBC_GetSNR(&uiEstimated_SNR, pAVL_Chip);

	if(r != AVL63X1_EC_OK)
	{
		*puiSignal_Quality = 0;
		return(r);
	}

	iSignal_Quality = (uiEstimated_SNR/45) + (iNormalization_Factor * 10);

	if (iSignal_Quality > 100)
		*puiSignal_Quality = 100;
	else if (iSignal_Quality < 0)
		*puiSignal_Quality = 0;
	else
		*puiSignal_Quality = iSignal_Quality;

	//r |= AVL_DVBC_IRx_GetSignalInfo(&SignalInfo, pAVL_Chip);

	return(r);
}
