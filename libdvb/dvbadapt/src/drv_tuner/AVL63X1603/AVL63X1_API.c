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

#include <semaphore.h>

#include "AVL63X1_include.h"


//DECLARE_MUTEX(m_semI2CRepeater_r);

AVL63X1_ErrorCode AVL63X1_LockChannel( enum AVL63X1_LockMode enumLockMode, const void * psChannel )
{
	AVL63X1_ErrorCode r=AVL63X1_EC_OK;
	AVL63X1_RunningLevel enumRunningLevel=AVL63X1_RL_NORMAL;
	AVL_uint32 	uiCnt = 0;

	r = IBase_Halt_63X1(((AVL_Channel *)(psChannel))->pAVLChip);
	if(r != AVL63X1_EC_OK)
	{
		return(r);
	}

	while((enumRunningLevel != AVL63X1_RL_HALT) && (uiCnt<10))
	{
		r = IBase_GetRunningLevel_63X1(&enumRunningLevel, ((AVL_Channel *)(psChannel))->pAVLChip);
		if(r != AVL63X1_EC_OK)
		{
			return(r);
		}
		AVL63X1_IBSP_Delay(10);
		uiCnt++;
	}

	if(enumRunningLevel != AVL63X1_RL_HALT)
	{
		return AVL63X1_EC_RUNNING;
	}

	if(((AVL_Channel *)(psChannel))->m_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		if(((AVL_Channel *)(psChannel))->m_demod_mode != ((AVL_DTMB_Channel *)(psChannel))->pAVLChip->m_current_demod_mode)
		{
			r = AVL63X1_SwitchMode(((AVL_DTMB_Channel *)(psChannel))->pAVLChip,AVL_DEMOD_MODE_DTMB); 
			AVL63X1_IBSP_Delay(200); //give sometime to the chip to change the mode.
			r |= AVL63X1_CheckChipReady (((AVL_DTMB_Channel *)(psChannel))->pAVLChip);
			if(r == AVL63X1_EC_OK)
			{
				r = AVL63X1_Initialize(((AVL_DTMB_Channel *)(psChannel))->pAVLChip);
				r |= AVL63X1_SetAnalogAGC_Pola(AGC_NORMAL, ((AVL_DTMB_Channel *)(psChannel))->pAVLChip);
				r |= AVL63X1_DriveIFAGC(AVL63X1_ON, ((AVL_DTMB_Channel *)(psChannel))->pAVLChip);
				r |= AVL63X1_SetMPEG_Mode(((AVL_DTMB_Channel *)(psChannel))->pAVLChip);
				r |= AVL63X1_DriveMpegOutput(AVL63X1_ON,((AVL_DTMB_Channel *)(psChannel))->pAVLChip);
			}
		}

		r = AVL_DTMB_LockChannel( ((AVL_DTMB_Channel *)(psChannel))->pAVLChip, enumLockMode, (AVL_DTMB_Channel *)(psChannel));
		
	}
	else if(((AVL_Channel *)(psChannel))->m_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		if(((AVL_Channel *)(psChannel))->m_demod_mode != ((AVL_DVBC_Channel *)(psChannel))->pAVLChip->m_current_demod_mode)
		{
			r = AVL63X1_SwitchMode(((AVL_DVBC_Channel *)(psChannel))->pAVLChip,AVL_DEMOD_MODE_DVBC);
			r = AVL63X1_Initialize(((AVL_DVBC_Channel *)(psChannel))->pAVLChip);
			r |= AVL63X1_SetAnalogAGC_Pola(AGC_NORMAL, ((AVL_DVBC_Channel *)(psChannel))->pAVLChip);
			r |= AVL63X1_DriveIFAGC(AVL63X1_ON, ((AVL_DVBC_Channel *)(psChannel))->pAVLChip);
			r |= AVL63X1_SetMPEG_Mode(((AVL_DVBC_Channel *)(psChannel))->pAVLChip);
			r |= AVL63X1_DriveMpegOutput(AVL63X1_ON,((AVL_DVBC_Channel *)(psChannel))->pAVLChip);
		}
		r = AVL_DVBC_LockChannel( ((AVL_DVBC_Channel *)(psChannel))->pAVLChip, enumLockMode, (AVL_DVBC_Channel *)(psChannel));
	}

	return r;

}

AVL63X1_ErrorCode AVL63X1_Initialize(struct AVL63X1_Chip * pAVL_Chip)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	r = Init_ChipObject_63X1(pAVL_Chip);
	//Perform reset
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), 0x38fffc, 0);
	AVL63X1_IBSP_Delay(1);

	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), 0x38fffc, 1);

	r |= II2C_Write8_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_fw_download_method, 0);
	
	r = IBase_Initialize_63X1((const AVL63X1_PLL_Conf *)(pll_conf+2), &sdram_conf, pAVL_Chip);
	r |= AVL63X1_IBSP_Delay(300); //Give a delay to make sure the chip boot up.
	r |= AVL63X1_CheckChipReady (pAVL_Chip);
	
	// IRx Initialize
	r |= IRx_Initialize_63X1(pAVL_Chip);
	return r;
}

AVL63X1_ErrorCode AVL63X1_GetBER( struct AVL63X1_ErrorStats *pErrorStats , const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiHwCntBitErrors;
	AVL_uint32 uiHwCntNumBits;
	AVL_uint32 uiTemp;
	struct AVL63X1_uint64 uiTemp64;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_ber_err_cnt, &uiHwCntBitErrors);
	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_byte_num, &uiHwCntNumBits);
	uiHwCntNumBits <<= 3;

	if(uiHwCntNumBits > (1 << 30))
	{
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, &uiTemp);
		uiTemp |= 0x00000002;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_ber_err_cnt, &uiHwCntBitErrors);
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_byte_num, &uiHwCntNumBits);
		uiTemp &= 0xFFFFFFFD;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);
		uiHwCntNumBits <<= 3;
		Add32To64(&pErrorStats->m_SwCntNumBits, uiHwCntNumBits);
		Add32To64(&pErrorStats->m_SwCntBitErrors, uiHwCntBitErrors);
		uiHwCntNumBits = 0;
		uiHwCntBitErrors = 0;
	}

	pErrorStats->m_NumBits.m_HighWord = pErrorStats->m_SwCntNumBits.m_HighWord;
	pErrorStats->m_NumBits.m_LowWord = pErrorStats->m_SwCntNumBits.m_LowWord;
	Add32To64(&pErrorStats->m_NumBits, uiHwCntNumBits);
	pErrorStats->m_BitErrors.m_HighWord = pErrorStats->m_SwCntBitErrors.m_HighWord;
	pErrorStats->m_BitErrors.m_LowWord = pErrorStats->m_SwCntBitErrors.m_LowWord;
	Add32To64(&pErrorStats->m_BitErrors, uiHwCntBitErrors);

	// Compute the BER
	uiTemp = Divide64(pErrorStats->m_NumBits, pErrorStats->m_BitErrors);
	Multiply32(&uiTemp64, uiTemp, (AVL_uint32)1e9);
	pErrorStats->m_BER = (uiTemp64.m_LowWord >> 30) + (uiTemp64.m_HighWord << 2);

    if(AVL_DEMOD_MODE_DTMB == pAVL_Chip->m_current_demod_mode)
    {
	    r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_lock_status, &uiTemp);
    }
    else if(AVL_DEMOD_MODE_DVBC == pAVL_Chip->m_current_demod_mode)
    {
        r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_lock_status, &uiTemp);
    }
    else
    {
        return (AVL63X1_EC_GENERAL_FAIL);
    }
	uiTemp = ((uiTemp >> 16) & 0x00000001);
	pErrorStats->m_LostLock = (AVL_uint16)uiTemp;

	return(r);
}


AVL63X1_ErrorCode AVL63X1_GetPER( struct AVL63X1_ErrorStats *pErrorStats, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiHwCntPktErrors;
	AVL_uint32 uiHwCntNumPkts;
	AVL_uint32 uiTemp;
	struct AVL63X1_uint64 uiTemp64;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_packet_err_cnt, &uiHwCntPktErrors);
	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_packet_num, &uiHwCntNumPkts);

	if(uiHwCntNumPkts > (1 << 27))
	{
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, &uiTemp);
		uiTemp |= 0x00000001;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_packet_err_cnt, &uiHwCntPktErrors);
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_packet_num, &uiHwCntNumPkts);
		uiTemp &= 0xFFFFFFFE;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);
		Add32To64(&pErrorStats->m_SwCntNumPkts, uiHwCntNumPkts);
		Add32To64(&pErrorStats->m_SwCntPktErrors, uiHwCntPktErrors);
		uiHwCntNumPkts = 0;
		uiHwCntPktErrors = 0;
	}

	pErrorStats->m_NumPkts.m_HighWord = pErrorStats->m_SwCntNumPkts.m_HighWord;
	pErrorStats->m_NumPkts.m_LowWord = pErrorStats->m_SwCntNumPkts.m_LowWord;
	Add32To64(&pErrorStats->m_NumPkts, uiHwCntNumPkts);
	pErrorStats->m_PktErrors.m_HighWord = pErrorStats->m_SwCntPktErrors.m_HighWord;
	pErrorStats->m_PktErrors.m_LowWord = pErrorStats->m_SwCntPktErrors.m_LowWord;
	Add32To64(&pErrorStats->m_PktErrors, uiHwCntPktErrors);
	// Compute the PER
	uiTemp = Divide64(pErrorStats->m_NumPkts, pErrorStats->m_PktErrors);
	Multiply32(&uiTemp64, uiTemp, (AVL_uint32)1e9);
	pErrorStats->m_PER = (uiTemp64.m_LowWord >> 30) + (uiTemp64.m_HighWord << 2);

	if(AVL_DEMOD_MODE_DTMB == pAVL_Chip->m_current_demod_mode)
    {
	    r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_lock_status, &uiTemp);
    }
    else if(AVL_DEMOD_MODE_DVBC == pAVL_Chip->m_current_demod_mode)
    {
        r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_lock_status, &uiTemp);
    }
    else
    {
        return (AVL63X1_EC_GENERAL_FAIL);
    }
	uiTemp = ((uiTemp >> 16) & 0x00000001);
	pErrorStats->m_LostLock = (AVL_uint16)uiTemp;

	return(r);
}


AVL63X1_ErrorCode AVL63X1_GetSpectrumInversion(enum AVL63X1_SpectrumInversion * pSpectrumInversion, const struct AVL63X1_Chip * pAVL_Chip)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		r = AVL_DTMB_GetSpectrumInversion(pSpectrumInversion, pAVL_Chip);
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		r = AVL_DVBC_GetSpectrumInversion(pSpectrumInversion, pAVL_Chip);
	}

	return r;

}

AVL63X1_ErrorCode AVL63X1_GetLockRetries(AVL_puint16 puiNumLockRetries,const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint16 uiTemp;

	if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		r = II2C_Read16_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_num_lock_retries, &uiTemp);
		*puiNumLockRetries = uiTemp;
	}
	else
	{
		return AVL63X1_EC_GENERAL_FAIL;
	}

	return r;
}


AVL63X1_ErrorCode AVL63X1_GetLockStatus( AVL_puint16 puiLockStatus, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint16 uiNumLockRetries;

	if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		r = AVL_DTMB_GetLockStatus(puiLockStatus, &uiNumLockRetries,pAVL_Chip);
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		r = AVL_DVBC_GetLockStatus(puiLockStatus, &uiNumLockRetries,pAVL_Chip);
	}

	return r;

}


AVL63X1_ErrorCode AVL63X1_GetCarrierFreqOffset( AVL_pint32 piFreqOffsetHz, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		r = AVL_DTMB_GetCarrierFreqOffset(piFreqOffsetHz, pAVL_Chip);
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		r = AVL_DVBC_GetCarrierFreqOffset(piFreqOffsetHz, pAVL_Chip);
	}

	return r;

}


AVL63X1_ErrorCode AVL63X1_GetSignalInfo(void *pSignalInfo, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		r = AVL_DTMB_GetSignalInfo((AVL_DTMB_SignalInfo *)pSignalInfo, pAVL_Chip);
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		r = AVL_DVBC_GetSignalInfo((AVL_DVBC_SignalInfo *)pSignalInfo, pAVL_Chip);
	}

	return r;

}


AVL63X1_ErrorCode AVL63X1_GetSNR( AVL_puint32 puiSNR_db, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		r = AVL_DTMB_GetSNR(puiSNR_db, pAVL_Chip);
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		r = AVL_DVBC_GetSNR(puiSNR_db, pAVL_Chip);
	}

	return r;

}


AVL63X1_ErrorCode AVL63X1_GetStrength(AVL_puint16 puiSignalLevel , const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		r = AVL_DTMB_GetSignalLevel(puiSignalLevel, pAVL_Chip);
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		r = AVL_DVBC_GetSignalLevel(puiSignalLevel, pAVL_Chip);
	}

	return r;

}

AVL63X1_ErrorCode AVL63X1_SetAnalogAGC_Pola( enum AVL63X1_AnalogAGC_Pola enumAGC_Pola, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;
	
	if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_if_agc_control, &uiTemp);
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_if_agc_control, &uiTemp);
	}
	
	if(r == AVL63X1_EC_OK)
	{
		uiTemp = ((uiTemp & 0xFFFFFFFE) | (AVL_uint32)enumAGC_Pola);
		if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
		{
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_if_agc_control, uiTemp);
		}
		else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
		{
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_if_agc_control, uiTemp);
		}
	}	

	return(r);
}


AVL63X1_ErrorCode AVL63X1_DriveIFAGC(enum AVL63X1_Switch         enumOn_Off,   const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

    if(AVL63X1_ON == enumOn_Off)
    {
	    r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_rfagc_tri_enb, 1);
    }
    else if(AVL63X1_OFF == enumOn_Off)
    {
    	r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_rfagc_tri_enb, 0);
    }
    else
    {
        r = AVL63X1_EC_GENERAL_FAIL;
    }
	
	return(r);
}


AVL63X1_ErrorCode AVL63X1_StartBER( struct AVL63X1_Chip * pAVL_Chip, struct AVL63X1_ErrorStats * pErrorStats, enum AVL63X1_TestPattern enumTestPattern, enum AVL63X1_LFSR_FbBit enumLFSR_InvertFb, AVL_puint16 puiBER_Sync)
{
	AVL_uint16 uiLFSR_Sync;
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;
	AVL_uint32 uiCnt;
	AVL_uint32 uiByteCnt;
	AVL_uint32 uiBER_FailCnt;
	AVL_uint32 uiBitErrors;

	uiLFSR_Sync = 0;
	uiBER_FailCnt = 0;

	pErrorStats->m_LFSR_Sync = 0;
	pErrorStats->m_LostLock = 0;
	pErrorStats->m_SwCntBitErrors.m_HighWord = 0;
	pErrorStats->m_SwCntBitErrors.m_LowWord = 0;
	pErrorStats->m_SwCntNumBits.m_HighWord = 0;
	pErrorStats->m_SwCntNumBits.m_LowWord = 0;
	pErrorStats->m_BitErrors.m_HighWord = 0;
	pErrorStats->m_BitErrors.m_LowWord = 0;
	pErrorStats->m_NumBits.m_HighWord = 0;
	pErrorStats->m_NumBits.m_LowWord = 0;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, &uiTemp);
	uiTemp |= 0x00000002;
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);
	if(AVL_DEMOD_MODE_DTMB == pAVL_Chip->m_current_demod_mode)
	{
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_lock_status, &uiTemp);
		uiTemp &= 0xFFFEFFFF;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_lock_status, uiTemp);
	}
	else if(AVL_DEMOD_MODE_DVBC == pAVL_Chip->m_current_demod_mode)
	{
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_lock_status, &uiTemp);
		uiTemp &= 0xFFFEFFFF;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_lock_status, uiTemp);
	}
	else
	{
		return (AVL63X1_EC_GENERAL_FAIL);
	}    
	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, &uiTemp);
	uiTemp &= 0xFFFFFFCF;
	uiTemp |= ((((AVL_uint32)enumTestPattern) << 5) | (((AVL_uint32)enumLFSR_InvertFb) << 4));
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);

	while(!uiLFSR_Sync)
	{
		uiTemp |= 0x00000006;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);
		uiTemp &= 0xFFFFFFFD;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);

		uiCnt = 0;
		uiByteCnt = 0;
		while((uiByteCnt < 1000) && (uiCnt < 200))
		{
			r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_byte_num, &uiByteCnt);
			uiCnt++;
		}

		uiTemp |= 0x00000006;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);
		uiTemp &= 0xFFFFFFF9;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);

		uiCnt = 0;
		uiByteCnt = 0;
		while((uiByteCnt < 10000) && (uiCnt < 200))
		{
			uiCnt++;
			r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_byte_num, &uiByteCnt);
		}

		uiTemp &= 0xFFFFFFF9;
		uiTemp |= 0x00000002;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);

		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_byte_num, &uiByteCnt);
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_ber_err_cnt, &uiBitErrors);
		if(uiCnt == 200)
		{
			break;
		}
		else if((uiByteCnt << 3) < (10 * uiBitErrors))
		{
			uiBER_FailCnt++;
			if(uiBER_FailCnt > 10)
			{
				break;
			}
		}
		else
		{
			uiLFSR_Sync = 1;
		}
	}

	if(uiLFSR_Sync == 1)
	{
		uiTemp &= 0xFFFFFFF9;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);
	}

	*puiBER_Sync = uiLFSR_Sync;

	return(r);
}


AVL63X1_ErrorCode AVL63X1_StartPER( struct AVL63X1_Chip * pAVL_Chip, struct AVL63X1_ErrorStats * pErrorStats)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;

	pErrorStats->m_LostLock = 0;
	pErrorStats->m_SwCntPktErrors.m_HighWord = 0;
	pErrorStats->m_SwCntPktErrors.m_LowWord = 0;
	pErrorStats->m_SwCntNumPkts.m_HighWord = 0;
	pErrorStats->m_SwCntNumPkts.m_LowWord = 0;
	pErrorStats->m_PktErrors.m_HighWord = 0;
	pErrorStats->m_PktErrors.m_LowWord = 0;
	pErrorStats->m_NumPkts.m_HighWord = 0;
	pErrorStats->m_NumPkts.m_LowWord = 0;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, &uiTemp);
	uiTemp |= 0x00000001;
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);

	if(AVL_DEMOD_MODE_DTMB == pAVL_Chip->m_current_demod_mode)
	{
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_lock_status, &uiTemp);
		uiTemp &= 0xFFFEFFFF;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_lock_status, uiTemp);
	}
	else if(AVL_DEMOD_MODE_DVBC == pAVL_Chip->m_current_demod_mode)
	{
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_lock_status, &uiTemp);
		uiTemp &= 0xFFFEFFFF;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_lock_status, uiTemp);
	}
	else
	{
		return (AVL63X1_EC_GENERAL_FAIL);
	}    
    
	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, &uiTemp);
	uiTemp |= 0x00000008;
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);
	uiTemp |= 0x00000001;
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);
	uiTemp &= 0xFFFFFFFE;
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_esm_cntrl, uiTemp);

	return(r);
}


AVL63X1_ErrorCode AVL63X1_GetSignalQuality( AVL_puint32 puiSignal_Quality, AVL_int32 iNormalization_Factor,const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		r = AVL_DTMB_GetSignalQuality(puiSignal_Quality, iNormalization_Factor, pAVL_Chip);
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		r = AVL_DVBC_GetSignalQuality(puiSignal_Quality, iNormalization_Factor, pAVL_Chip);
	}

	return r;

}


AVL63X1_ErrorCode AVL63X1_SetMPEG_Mode(const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;

	if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		uiTemp = 0x00010000;
		uiTemp |= (((AVL_uint32)(pAVL_Chip->m_MPEG_Info.m_mpcm) << 8) | (AVL_uint32)(pAVL_Chip->m_MPEG_Info.m_mpm));
		r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_mpeg_control_1, uiTemp);
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_mpeg_control_2, &uiTemp);
		uiTemp &=0xFFFFFFFE; 
		uiTemp |=(AVL_uint32)(pAVL_Chip->m_MPEG_Info.m_mpfm_DTMB_mode);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_mpeg_control_2, uiTemp);
		uiTemp = (AVL_uint32)pAVL_Chip->m_MPEG_Info.m_MPEG_ErrorBit;
		r = II2C_Write8_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr),rc_AVL_DTMB_mpeg_ts_error_bit_en,uiTemp);
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		uiTemp = 0x00010000;
		uiTemp |= (((AVL_uint32)(pAVL_Chip->m_MPEG_Info.m_mpcm) << 8) | (AVL_uint32)(pAVL_Chip->m_MPEG_Info.m_mpm));       
		r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_mpeg_control_1, uiTemp);
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_mpeg_control_2, &uiTemp);
		uiTemp &=0xFFFFFFFE; 
		uiTemp |=(AVL_uint32)(pAVL_Chip->m_MPEG_Info.m_mpfm_DVBC_mode);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_mpeg_control_2, uiTemp);
		uiTemp = (AVL_uint32)pAVL_Chip->m_MPEG_Info.m_MPEG_ErrorBit;
		r = II2C_Write8_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr),rc_AVL_DVBC_mpeg_ts_error_bit_en,uiTemp);
	}
	else
	{
		r = AVL63X1_EC_GENERAL_FAIL;
	}
	
	return(r);
}


AVL63X1_ErrorCode AVL63X1_DriveMpegOutput(enum AVL63X1_Switch         enumOn_Off, const struct AVL63X1_Chip * pAVL_Chip)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	if(AVL63X1_ON == enumOn_Off)
	{
		r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_mpeg_bus_tri_enb, 1);
	}
	else if(AVL63X1_OFF == enumOn_Off)
	{
		r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_mpeg_bus_tri_enb, 0);
	}
	else
	{
		r = AVL63X1_EC_GENERAL_FAIL;
	}

	return(r);
}


AVL63X1_ErrorCode AVL63X1_DisableMpegContinuousMode(const struct AVL63X1_Chip * pAVL_Chip)
{
	AVL_uint32 uiTemp;
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	
	if(AVL_DEMOD_MODE_DTMB ==pAVL_Chip->m_current_demod_mode)
	{
		r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_mpeg_control_3, &uiTemp );
		uiTemp &= 0xFFFFFFFE;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_mpeg_control_3, uiTemp );
	}
	else if(AVL_DEMOD_MODE_DVBC == pAVL_Chip->m_current_demod_mode)
	{
		r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_mpeg_control_5, &uiTemp );
		uiTemp &= 0xFFFF0000;
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_mpeg_control_5, uiTemp );
	}
	return(r);
}


AVL63X1_ErrorCode AVL63X1_EnableMpegContinuousMode(const struct AVL63X1_Chip * pAVL_Chip, AVL_uint32 uiByteClockFreq_Hz)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiMpegRefClkFreq_Hz;
	AVL_uint32 uiTemp;
	
	if(AVL_DEMOD_MODE_DTMB == pAVL_Chip->m_current_demod_mode)
	{
		r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_mpeg_control_3, &uiTemp );
		uiTemp |= 0x00000001;
		r |=II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_mpeg_control_3, uiTemp );
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_fec_clk_Hz, &uiMpegRefClkFreq_Hz);

		if(pAVL_Chip->m_MPEG_Info.m_mpm == AVL63X1_MPM_SERIAL)
		{
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_cntns_pkt_seri_rate_frac_n, (((AVL_uint32)uiByteClockFreq_Hz)<<4));		
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_cntns_pkt_seri_rate_frac_d, (AVL_uint32)uiMpegRefClkFreq_Hz);	//Hz
		}
		else
		{
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_cntns_pkt_para_rate_frac_n,(((AVL_uint32)uiByteClockFreq_Hz)<<1));		
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_cntns_pkt_para_rate_frac_d, (AVL_uint32)uiMpegRefClkFreq_Hz);		//Hz
		}
	}
	else if(AVL_DEMOD_MODE_DVBC == pAVL_Chip->m_current_demod_mode)
	{
		r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_mpeg_control_5, &uiTemp );
		uiTemp &= 0xFFFF0000;
		uiTemp |= 0x00000001;
		r |=II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_mpeg_control_5, uiTemp );
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_fec_clk_Hz, &uiMpegRefClkFreq_Hz);
#if 1    // enable it after get update DVBC_config.xml with addinh the following registers 
		if(pAVL_Chip->m_MPEG_Info.m_mpm == AVL63X1_MPM_SERIAL)
		{
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_cntns_pkt_seri_rate_frac_n_32bit, (((AVL_uint32)uiByteClockFreq_Hz)<<4));		
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_cntns_pkt_seri_rate_frac_d_32bit, (AVL_uint32)uiMpegRefClkFreq_Hz);	//Hz
		}
		else
		{
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_cntns_pkt_para_rate_frac_n_32bit,(((AVL_uint32)uiByteClockFreq_Hz)<<1));		
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_cntns_pkt_para_rate_frac_d_32bit, (AVL_uint32)uiMpegRefClkFreq_Hz);		//Hz
		}
#endif
	}
	else
	{
		return (AVL63X1_EC_GENERAL_FAIL);
	}
	
	return(r);
}


AVL63X1_ErrorCode AVL63X1_GetScatterData( AVL_puint32 puiIQ_Data, AVL_puint16 puiSize, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		r = AVL_DTMB_GetScatterData(puiIQ_Data, puiSize, pAVL_Chip );
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		r = AVL_DVBC_GetScatterData(puiIQ_Data, puiSize, pAVL_Chip );
	}

	return r;
}


AVL63X1_ErrorCode AVL63X1_SwitchMode(struct AVL63X1_Chip * pAVL_Chip, AVL_uchar demod_mode)
{
	AVL63X1_ErrorCode r= AVL63X1_EC_OK;
	const AVL_uint16 uiTimeDelay = 10;
	const AVL_uint16 uiMaxRetries = 20;
	AVL_uint32 i=0;

	pAVL_Chip->m_current_demod_mode = demod_mode;
	
	r = IBase_Halt_63X1(pAVL_Chip);
	
	if(AVL63X1_EC_OK == r )
	{
		while (AVL63X1_EC_OK != IBase_GetRxOP_Status_63X1(pAVL_Chip))
		{
			if (uiMaxRetries < i++)
			{
				r |= AVL63X1_EC_RUNNING;
				break;
			}
			AVL63X1_IBSP_Delay(uiTimeDelay);
		}
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_cpu_ddc_srst, 0);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_cpu_valu_srst, 0);
		AVL63X1_IBSP_Delay(1);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_cpu_ddc_srst, 1);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_cpu_valu_srst,1);
		
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_dtmb_compressed_ram_start_addr, 0x280000);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_dtmb_uncompressed_ram_start_addr,0x2d4000);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_dvbc_compressed_ram_start_addr, 0x287800);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_dvbc_uncompressed_ram_start_addr,0x2e0800);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_core_ready_word, 0x00000000);

		r |= IBase_SendRxOP_63X1(OP_RX_CHANGE_MODE, pAVL_Chip);
	}

	return r;
}


AVL63X1_ErrorCode AVL63X1_Sleep( struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	r = IBase_SendRxOP_63X1(OP_RX_SLEEP, pAVL_Chip);	
	//This delay is needed to make sure the command gets processed by the firmware incase it is busy doing something else.
	r |= AVL63X1_IBSP_Delay(20);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), 0x38fffc, 0x00);	

	return r;
}


AVL63X1_ErrorCode AVL63X1_Wakeup( struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), 0x38fffc, 0x01);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_core_ready_word, 0x00000000);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_c306_top_srst, 0);
	r |= IBase_SendRxOP_63X1(OP_RX_WAKE_UP, pAVL_Chip);	
	//This delay is needed to make sure the command gets processed by the firmware incase it is busy doing something else.
	r |= AVL63X1_IBSP_Delay(2);
	// Load the default configuration
	r |= IBase_SendRxOP_63X1(OP_RX_LD_DEFAULT, pAVL_Chip);
	//This delay is needed to make sure the command gets processed by the firmware incase it is busy doing something else.
	r |= AVL63X1_IBSP_Delay(2);
	
	if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		r = AVL_DTMB_Wakeup(pAVL_Chip );
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		r = AVL_DVBC_Wakeup(pAVL_Chip );
	}

	return r;
}



AVL63X1_ErrorCode AVL63X1_CheckChipReady( struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiCoreReadyWord;
	AVL_uint32 uiCoreRunning;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_c306_top_srst, &uiCoreRunning);
	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_core_ready_word, &uiCoreReadyWord);
	if( (AVL63X1_EC_OK == r) )
	{
		if( (1 == uiCoreRunning) || (uiCoreReadyWord != 0x5aa57ff7) )
		{
			r = AVL63X1_EC_GENERAL_FAIL;
			pbierror("FUN:%s,LINE:%d,r:%d\n",__FUNCTION__,__LINE__,r);
		}
	}
	return(r);
}

AVL63X1_ErrorCode AVL63X1_GetVersion( struct AVL63X1_VerInfo * pVerInfo, const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL_uint32 uiTemp;
	AVL_uchar ucBuff[4];
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	r =  II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_rom_ver, &uiTemp);
	if( AVL63X1_EC_OK == r )
	{
		Chunk32(uiTemp, ucBuff);
		pVerInfo->m_Chip.m_Major = ucBuff[0];
		pVerInfo->m_Chip.m_Minor = ucBuff[1];
		pVerInfo->m_Chip.m_Build = ucBuff[2];
		pVerInfo->m_Chip.m_Build = ((AVL_uint16)((pVerInfo->m_Chip.m_Build)<<8)) + ucBuff[3];
		if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
		{
			r |=  II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DTMB_patch_ver, &uiTemp);
		}
		else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
		{
			r |=  II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_patch_ver, &uiTemp);
		}
		if( AVL63X1_EC_OK == r )
		{
			Chunk32(uiTemp, ucBuff);
			pVerInfo->m_Patch.m_Major = ucBuff[0];
			pVerInfo->m_Patch.m_Minor = ucBuff[1];
			pVerInfo->m_Patch.m_Build = ucBuff[2];
			pVerInfo->m_Patch.m_Build = ((AVL_uint16)((pVerInfo->m_Patch.m_Build)<<8)) + ucBuff[3];

			pVerInfo->m_API.m_Major = AVL63X1_API_VER_MAJOR;
			pVerInfo->m_API.m_Minor = AVL63X1_API_VER_MINOR;
			pVerInfo->m_API.m_Build = AVL63X1_API_VER_BUILD;
		}
	}
	return(r);
}

///II2C_Repeater Module
AVL63X1_ErrorCode AVL63X1_II2C_Repeater_ReadData(  AVL_uchar ucSlaveAddr, AVL_puchar pucBuff, AVL_uint16 uiSize, struct AVL63X1_Chip * pAVLChip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uchar pBuff[I2CM_RSP_LENGTH];
	AVL_uint16 uiTimeOut;
	AVL_uint32 uiI2CM_status;
	const AVL_uint16 uiTimeOutTh = 90;
	const AVL_uint32 uiTimeDelay = 100;  //100 ms 
	AVL_uint32 uiTemp;
	AVL63X1_II2C_Repeater_Info *spRepeater_Info;
  
	if(ucSlaveAddr != pAVLChip->m_TunerAddress)
	{
		r = II2C_Repeater_FindByAddress_63X1(ucSlaveAddr, pAVLChip, &uiTemp);
		if(AVL63X1_EC_OK != r)
		{
	  		return(r);
		}
		spRepeater_Info = (&(pAVLChip->m_RepeaterInfo[uiTemp]));
		r = II2C_Repeater_SelectMode_63X1(spRepeater_Info->m_uiRepeaterMode, pAVLChip);
		r |= II2C_Repeater_Initialize_63X1(ucSlaveAddr, spRepeater_Info->m_RepeaterClockKHz, pAVLChip);
		if(AVL63X1_EC_OK != r)
		{
    		return (r);
		}
		pAVLChip->m_TunerAddress = ucSlaveAddr;
	}
 
	if ( uiSize > I2CM_RSP_LENGTH )
	{
		return(AVL63X1_EC_GENERAL_FAIL);
	}

	r = II2C_Read32_63X1((AVL_uint16)(pAVLChip->m_SlaveAddr), rc_AVL63X1_i2cm_repeater_type, &uiTemp);
	if(AVL63X1_EC_OK != r)
	{
		return (r);
	}

	if(MESSAGE_TYPE == uiTemp) //firmware
	{
		r = AVL63X1_IBSP_WaitSemaphore(&(pAVLChip->m_semI2CRepeater_r));
		ChunkAddr(rc_AVL63X1_i2cm_cmd_addr+I2CM_CMD_LENGTH-4, pBuff);
		pBuff[3] = 0x0;
		pBuff[4] = (AVL_uchar)uiSize;
		pBuff[5] = (ucSlaveAddr<<1); //change 7-bit tuner slave address to 8-bit.
		pBuff[6] = OP_I2CM_READ;
		r |= II2C_Repeater_SendOP_63X1(pBuff, 7, pAVLChip);
		if ( AVL63X1_EC_OK == r )
		{
			uiTimeOut = 0;
			while ( AVL63X1_EC_OK != II2C_Repeater_GetOPStatus_63X1(pAVLChip) )
			{
				if((++uiTimeOut) >= uiTimeOutTh)
				{
					r |= AVL63X1_EC_TIMEOUT;
					r |= AVL63X1_IBSP_ReleaseSemaphore(&(pAVLChip->m_semI2CRepeater_r));

					return(r);
				}
				r |= AVL63X1_IBSP_Delay(uiTimeDelay);
			}

			r |= II2C_Read32_63X1((AVL_uint16)(pAVLChip->m_SlaveAddr), rs_AVL63X1_i2cm_status_addr, &uiI2CM_status);
			r |= (II2C_Read_63X1((AVL_uint16)(pAVLChip->m_SlaveAddr), rc_AVL63X1_i2cm_rsp_addr, pucBuff, uiSize));
			if( AVL63X1_EC_OK == r )
			{
				if( I2CM_STATUS_OK != (AVL63X1_MessageType_II2C_Repeater_Status)(uiI2CM_status))
				{
					r |= AVL63X1_EC_I2C_REPEATER_FAIL;
				}
			}
		}
		r |= AVL63X1_IBSP_ReleaseSemaphore(&(pAVLChip->m_semI2CRepeater_r));
	}
	else if(BYTE_TYPE == uiTemp || BIT_TYPE == uiTemp) //hardware
	{
		if(BIT_TYPE == uiTemp)
		{
			r |= II2C_Write32_63X1((AVL_uint16)(pAVLChip->m_SlaveAddr),rc_AVL63X1_tuner_hw_i2c_bit_rpt_cntrl, 0x03);
		}
		r = II2C_Repeater_SetReadBytes_63X1(pAVLChip, (AVL_uchar)(uiSize));
		r |= II2C_ReadDirect_63X1(ucSlaveAddr, pucBuff, uiSize);
		
		if(BIT_TYPE == uiTemp)
		{
			r |= II2C_Write32_63X1((AVL_uint16)(pAVLChip->m_SlaveAddr),rc_AVL63X1_tuner_hw_i2c_bit_rpt_cntrl, 0x02);
		}
	}
	else
	{
		r = AVL63X1_EC_GENERAL_FAIL;
	}
	
	return(r);
}

AVL63X1_ErrorCode AVL63X1_II2C_Repeater_WriteData(  AVL_uchar ucSlaveAddr, const AVL_puchar ucBuff, AVL_uint16 uiSize, struct AVL63X1_Chip * pAVLChip )
{
	AVL_uchar pBuff[I2CM_CMD_LENGTH+3];
	AVL_uint16 i1, i2;
	AVL_uint16 cmdSize;
	AVL_uint32 uiI2CM_status;
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;
	AVL63X1_II2C_Repeater_Info *spRepeater_Info;
   
	if(ucSlaveAddr != pAVLChip->m_TunerAddress)
	{
		r = II2C_Repeater_FindByAddress_63X1(ucSlaveAddr, pAVLChip, &uiTemp);
		if(AVL63X1_EC_OK != r)
		{
			return(r);
		}
		spRepeater_Info = (&(pAVLChip->m_RepeaterInfo[uiTemp]));
		r = II2C_Repeater_SelectMode_63X1(spRepeater_Info->m_uiRepeaterMode, pAVLChip);
		r |= II2C_Repeater_Initialize_63X1(ucSlaveAddr, spRepeater_Info->m_RepeaterClockKHz, pAVLChip);
		if(AVL63X1_EC_OK != r)
		{
			return (r);
		}
		pAVLChip->m_TunerAddress = ucSlaveAddr;
	}
  
	if ( uiSize>I2CM_CMD_LENGTH-3 )
	{
		return(AVL63X1_EC_GENERAL_FAIL);
	}

	r = II2C_Read32_63X1((AVL_uint16)(pAVLChip->m_SlaveAddr), rc_AVL63X1_i2cm_repeater_type, &uiTemp);
	if(AVL63X1_EC_OK != r)
	{
        return (r);
	}

	if(MESSAGE_TYPE == uiTemp) //firmware
	{
		cmdSize = ((3+uiSize)%2)+3+uiSize;	  /* How many bytes need send to Availink device through i2c interface */
		ChunkAddr(rc_AVL63X1_i2cm_cmd_addr+I2CM_CMD_LENGTH-cmdSize, pBuff);

		i1 = 3+((3+uiSize)%2);	  /* skip one byte if the uisize+3 is odd*/

		for ( i2=0; i2<uiSize; i2++ )
		{
			pBuff[i1++] = ucBuff[i2];
		}
		pBuff[i1++] = (AVL_uchar)uiSize;
		pBuff[i1++] = (ucSlaveAddr <<1); //change 7-bit tuner slave address to 8-bit.
		pBuff[i1++] = OP_I2CM_WRITE;

		r = II2C_Repeater_SendOP_63X1(pBuff, (AVL_uchar)(cmdSize+3), pAVLChip);
		//insure I2C repeater is set by above command
		AVL63X1_IBSP_Delay(10);
		if(AVL63X1_EC_OK == r )
		{
			r |= II2C_Read32_63X1((AVL_uint16)(pAVLChip->m_SlaveAddr), rs_AVL63X1_i2cm_status_addr, &uiI2CM_status);
			if( AVL63X1_EC_OK == r )
			{
				if( I2CM_STATUS_OK != (AVL63X1_MessageType_II2C_Repeater_Status)(uiI2CM_status))
				{
					r |= AVL63X1_EC_I2C_REPEATER_FAIL;	
				}
			}	
		}
	}
	else if(BYTE_TYPE == uiTemp || BIT_TYPE == uiTemp) //hardware
	{
		if(BIT_TYPE == uiTemp)
		{
			r |= II2C_Write32_63X1((AVL_uint16)(pAVLChip->m_SlaveAddr),rc_AVL63X1_tuner_hw_i2c_bit_rpt_cntrl, 0x03);
		}
		r = II2C_WriteDirect_63X1(ucSlaveAddr, ucBuff, uiSize);
		if(BIT_TYPE == uiTemp)
		{
			r |= II2C_Write32_63X1((AVL_uint16)(pAVLChip->m_SlaveAddr),rc_AVL63X1_tuner_hw_i2c_bit_rpt_cntrl, 0x02);
		}
	}
	else
	{
		r = AVL63X1_EC_GENERAL_FAIL;
	}
	
	return(r);
}


AVL63X1_ErrorCode AVL63X1_II2C_Repeater_ReadData_Multi(  AVL_uchar ucSlaveAddr, AVL_puchar pucBuff, AVL_uchar ucRegAddr, AVL_uint16 uiSize, struct AVL63X1_Chip * pAVLChip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uchar pBuff[I2CM_RSP_LENGTH];
	AVL_uint16 uiTimeOut;
	AVL_uint32 uiI2CM_status;
	const AVL_uint16 uiTimeOutTh = 10;
	const AVL_uint32 uiTimeDelay = 100;  //100 ms 
	AVL_uint32 uiTemp;
	AVL63X1_II2C_Repeater_Info *spRepeater_Info;
   
	if(ucSlaveAddr != pAVLChip->m_TunerAddress)
	{
		r = II2C_Repeater_FindByAddress_63X1(ucSlaveAddr, pAVLChip, &uiTemp);
		if(AVL63X1_EC_OK != r)
		{
 			return(r);
		}
		spRepeater_Info = (&(pAVLChip->m_RepeaterInfo[uiTemp]));
		r = II2C_Repeater_SelectMode_63X1(spRepeater_Info->m_uiRepeaterMode, pAVLChip);
		r |= II2C_Repeater_Initialize_63X1(ucSlaveAddr, spRepeater_Info->m_RepeaterClockKHz, pAVLChip);
		if(AVL63X1_EC_OK != r)
		{
    		return (r);
		}
		pAVLChip->m_TunerAddress = ucSlaveAddr;
	}
  
	if ( uiSize > I2CM_RSP_LENGTH )
	{
		return(AVL63X1_EC_GENERAL_FAIL);
	}

	r = II2C_Read32_63X1((AVL_uint16)(pAVLChip->m_SlaveAddr), rc_AVL63X1_i2cm_repeater_type, &uiTemp);
	if(AVL63X1_EC_OK != r)
	{
        return (r);
	}

	if(MESSAGE_TYPE == uiTemp) //firmware
	{
		ChunkAddr(rc_AVL63X1_i2cm_cmd_addr+I2CM_CMD_LENGTH-6, pBuff);
		pBuff[3] = 0;
		pBuff[4] = ucRegAddr;
		pBuff[5] = 0x1;
		pBuff[6] = (AVL_uchar)uiSize;
		pBuff[7] = (ucSlaveAddr<<1); //change 7-bit tuner slave address to 8-bit.
		pBuff[8] = OP_I2CM_READ;
		r |= II2C_Repeater_SendOP_63X1(pBuff, 9, pAVLChip);
		if ( AVL63X1_EC_OK == r )
		{
			uiTimeOut = 0;
			r = AVL63X1_IBSP_WaitSemaphore(&(pAVLChip->m_semI2CRepeater_r));
			while ( AVL63X1_EC_OK != II2C_Repeater_GetOPStatus_63X1(pAVLChip) )
			{
				if((++uiTimeOut) >= uiTimeOutTh)
				{
					r |= AVL63X1_EC_TIMEOUT;
					r |= AVL63X1_IBSP_ReleaseSemaphore(&(pAVLChip->m_semI2CRepeater_r));
					return(r);
				}
				r |= AVL63X1_IBSP_Delay(uiTimeDelay);
			}
			r |= AVL63X1_IBSP_ReleaseSemaphore(&(pAVLChip->m_semI2CRepeater_r));
			r |= II2C_Read32_63X1((AVL_uint16)(pAVLChip->m_SlaveAddr), rs_AVL63X1_i2cm_status_addr, &uiI2CM_status);
			if( AVL63X1_EC_OK == r )
			{
				if( I2CM_STATUS_OK == (AVL63X1_MessageType_II2C_Repeater_Status)(uiI2CM_status))
				{
					r |= (II2C_Read_63X1((AVL_uint16)(pAVLChip->m_SlaveAddr), rc_AVL63X1_i2cm_rsp_addr, pucBuff, uiSize));
				}
				else
				{
					r |= AVL63X1_EC_I2C_REPEATER_FAIL;	
				}
			}
		}
	}
   
	else
	{
        r = AVL63X1_EC_GENERAL_FAIL;
	}
	
	return(r);
}

AVL63X1_ErrorCode AVL63X1_DVBC_GetBER_BeforeRS( AVL_puint32 puiBER_BeforeRS,const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL_DVBC_berBeforRS_xE9, puiBER_BeforeRS);	

	return(r);
}
