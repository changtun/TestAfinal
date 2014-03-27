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

AVL63X1_ErrorCode Init_ChipObject_63X1( struct AVL63X1_Chip * pAVL63X1_ChipObject)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
/* BEGIN: Deleted by zhwu, 2012/6/2 */
#if  1
	r = AVL63X1_IBSP_InitSemaphore(&(pAVL63X1_ChipObject->m_semRx));
	r = AVL63X1_IBSP_InitSemaphore(&(pAVL63X1_ChipObject->m_semI2CRepeater));
	r = AVL63X1_IBSP_InitSemaphore(&(pAVL63X1_ChipObject->m_semI2CRepeater_r));
#endif /* #if 0 */
/* END: Deleted by zhwu, 2012/6/2 */
/* BEGIN: Deleted by zhwu, 2012/6/5 */
	r = II2C_Initialize_63X1(); // there is internal protection to assure the II2C will be initialized only once.
/* END: Deleted by zhwu, 2012/6/5 */
	return (r);
}


///IRx module
AVL63X1_ErrorCode IRx_Initialize_63X1(struct AVL63X1_Chip * pAVL_Chip)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	pAVL_Chip->m_TunerAddress = 0x00;

	// Load the default configuration
	r = IBase_SendRxOP_63X1(OP_RX_LD_DEFAULT, pAVL_Chip);

	if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		r = AVL_DTMB_Initialize(pAVL_Chip);
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		r = AVL_DVBC_Initialize(pAVL_Chip);

	}

	//send command to init ADC with the selected Input signal type.
	r |= IBase_SendRxOP_63X1(OP_RX_INIT_ADC, pAVL_Chip);
	//This delay is needed to make sure the command gets processed by the firmware incase it is busy doing something else.
	r |= AVL63X1_IBSP_Delay(2);
	
	return r;
}


void Add32To64(struct AVL63X1_uint64 *pSum, AVL_uint32 uiAddend)
{
	AVL_uint32 uiTemp;

	uiTemp = pSum->m_LowWord;
	pSum->m_LowWord += uiAddend;
	pSum->m_LowWord &= 0xFFFFFFFF;

	if (pSum->m_LowWord < uiTemp)
	{
		pSum->m_HighWord++;
	}
}

AVL_uint32 Divide64(struct AVL63X1_uint64 y, struct AVL63X1_uint64 x)
{
   AVL_uint16 n;
   AVL_uint32 answer;
   struct AVL63X1_uint64 remainder;
   struct AVL63X1_uint64 divisor;

   answer = 0;
   remainder.m_HighWord = x.m_HighWord;
   remainder.m_LowWord = x.m_LowWord;
   divisor.m_HighWord = y.m_HighWord;
   divisor.m_LowWord = y.m_LowWord;

	if((divisor.m_HighWord == remainder.m_HighWord) &&
	(divisor.m_LowWord == remainder.m_LowWord))
	{
		answer |= (1 << 30);
	}
	else
	{
		for (n = 0; n < 29; n++)
		{
			remainder.m_HighWord = remainder.m_HighWord << 1;
			if((remainder.m_LowWord & 0x80000000) == 0x80000000)
			{
				remainder.m_HighWord |= 0x00000001;
			}
			remainder.m_LowWord = remainder.m_LowWord << 1;
			if(GreaterThanOrEqual64(remainder, divisor))
			{
				Subtract64(&remainder, divisor);
				answer |= (((AVL_uint32)1) << (29 - n));
			}
		}
	}
	return(answer);
}

AVL_uint32 GreaterThanOrEqual64(struct AVL63X1_uint64 a, struct AVL63X1_uint64 b)
{
	AVL_uint32 result;

	result = 0;

	if((a.m_HighWord == b.m_HighWord) && (a.m_LowWord == b.m_LowWord))
	{
		result = 1;
	}
	if(a.m_HighWord > b.m_HighWord)
	{
		result = 1;
	}
	else if(a.m_HighWord == b.m_HighWord)
	{
		if(a.m_LowWord > b.m_LowWord)
		{
			result = 1;
		}
	}

	return(result);
}

void Subtract64(struct AVL63X1_uint64 *pA, struct AVL63X1_uint64 b)
{
	struct AVL63X1_uint64 a;
	struct AVL63X1_uint64 temp;

	a.m_HighWord = pA->m_HighWord;
	a.m_LowWord = pA->m_LowWord;

	temp.m_HighWord = a.m_HighWord - b.m_HighWord;
	if(a.m_LowWord >= b.m_LowWord)
	{
		temp.m_LowWord = a.m_LowWord - b.m_LowWord;
	}
	else
	{
		temp.m_LowWord = b.m_LowWord - a.m_LowWord;
		temp.m_HighWord >>= 1;
	}

	pA->m_HighWord = temp.m_HighWord;
	pA->m_LowWord = temp.m_LowWord;
}


void Multiply32(struct AVL63X1_uint64 *pDst, AVL_uint32 m1, AVL_uint32 m2)
{
	pDst->m_LowWord = (m1 & 0xFFFF) * (m2 & 0xFFFF);
	pDst->m_HighWord = 0;

	AddScaled32To64(pDst, (m1 >> 16) * (m2 & 0xFFFF));
	AddScaled32To64(pDst, (m2 >> 16) * (m1 & 0xFFFF));

	pDst->m_HighWord += (m1 >> 16) * (m2 >> 16);
}

void AddScaled32To64(struct AVL63X1_uint64 *pDst, AVL_uint32 a)
{
	AVL_uint32 saved;

	saved = pDst->m_LowWord;
	pDst->m_LowWord += (a << 16);

	pDst->m_LowWord &= 0xFFFFFFFF;
	pDst->m_HighWord += ((pDst->m_LowWord < saved) ? 1 : 0) + (a >> 16);
}

void Multiply32By16(struct AVL63X1_int64 *pY, AVL_int32 a, AVL_int16 b)
{
	AVL_uint16 uiLswA;
	AVL_int16 iMswA;
	AVL_int32 iMswAB;
	AVL_uint32 uiLswAB;
	struct AVL63X1_int64 iTemp;

	// (mswA * 2^16 + lswA) * b = (mswA * b * 2^16) + (lswA * b)
	// Compute (mswA * b * 2^16):
	uiLswA = (AVL_uint16)(a & 0x0000FFFF);
	iMswA = (AVL_int16)((a >> 16) & 0x0000FFFF);
	iMswAB = (AVL_int32)(iMswA * b);
	iTemp.m_HighWord = iMswAB;
	iTemp.m_HighWord >>= 16;
	iTemp.m_LowWord = (AVL_uint32)(iMswAB << 16);
	// Compute (lswA * b):
	uiLswAB = (AVL_uint32)(uiLswA * b);

	// Add the results
	Add32To64((struct AVL63X1_uint64 *)&iTemp, uiLswAB);

	pY->m_HighWord = iTemp.m_HighWord;
	pY->m_LowWord = iTemp.m_LowWord;
}

AVL_uint32 ConvertNsToSDRAM_Clocks(AVL_uint32 SDRAM_ClkFrequency_Hz, AVL_uint16 Value_ns)
{
	AVL_uint32 SDRAM_Freq_100kHz;
	AVL_uint32 temp;
	AVL_uint32 uiValue_clks;

	SDRAM_Freq_100kHz = (SDRAM_ClkFrequency_Hz/100000);
	temp = (SDRAM_Freq_100kHz * 419 * Value_ns) - 41943;
	uiValue_clks = (temp >> 22);
	if((temp & 0x003FFFFF) != 0)
	{
		uiValue_clks++;
	}

	return(uiValue_clks);
}

///IBase Module
/// Rest of the functions defined in AVL63X1_IBase.h file have exact same implementation for both DTMB and DVBC. 
/// Put their implementaions(either from IBase.c in DVBC or DTMB SDK) in this file and replace AVL_DVBC(orAVL_DTMB) prefix with AVL63X1.
AVL63X1_ErrorCode IBase_SetPLL_63X1(const struct AVL63X1_PLL_Conf * pPLL_Conf, struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	int count = 0;

	r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_pll_clkf, pPLL_Conf->m_PLL_FbClkDivVal);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_pll_clkr, pPLL_Conf->m_PLL_RefClkDivVal);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_pll_od, pPLL_Conf->m_PLL_od);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_pll_od2, pPLL_Conf->m_PLL_od2);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_pll_od3, pPLL_Conf->m_PLL_od3);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_pll_bwadj, pPLL_Conf->m_PLL_FbClkDivVal);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_reset_register, 0);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_reset_register, 1);

	//r |= AVL63X1_IBSP_I2C_Write_Reset((AVL_uint16)(pAVL_Chip->m_SlaveAddr));

	pAVL_Chip->m_DemodFrequency_Hz = pPLL_Conf->m_DemodFrequency_Hz;
	pAVL_Chip->m_SDRAMFrequency_Hz = pPLL_Conf->m_SDRAM_Frequency_Hz;
	pAVL_Chip->m_FECFrequency_Hz = pPLL_Conf->m_FEC_Frequency_Hz;
	pAVL_Chip->m_RefFrequency_Hz = pPLL_Conf->m_RefFrequency_Hz;

	//Program DDL block with the appropriate phase delays
	r |= AVL63X1_IBSP_Delay(1);
	r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_dll_out_phase, pPLL_Conf->m_DDLL_out_phase);
	r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_dll_rd_phase, pPLL_Conf->m_DDLL_read_phase);
	r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_dll_init, 0);
	r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_dll_init, 1);
	r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_dll_init, 0);

	return(r);
}


AVL63X1_ErrorCode IBase_GetRxOP_Status_63X1(const struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiCmd;

	r = II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_command, &uiCmd);
	if( AVL63X1_EC_OK == r )
	{
		if( 0 != uiCmd )
		{
			r = AVL63X1_EC_RUNNING;
		}
	}

	return(r);
}

AVL63X1_ErrorCode IBase_Initialize_63X1( const struct AVL63X1_PLL_Conf * pPLL_Conf, struct AVL63X1_SDRAM_Conf * pSDRAM_Conf, struct AVL63X1_Chip * pAVL_Chip )
{
	//struct timeval tpStart, tpEnd;
	//float timeUse=0.0;
	
   //gettimeofday(&tpStart, NULL);
	AVL_uint32 uiSize;
	AVL_uint32 i1;
	AVL_uint32 uiCnt;
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_puchar pInitialData;
	
	//save the SDRAM, MPEG and Input signal configuration  for future use.
	pAVL_Chip->m_pSDRAM_Conf = pSDRAM_Conf; 
	
	r = II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_c306_top_srst, 1);

	// Configure the PLL
	r |= IBase_SetPLL_63X1(pPLL_Conf, pAVL_Chip );
	r |= AVL63X1_IBSP_Delay(1);

	//The code below needs to be modified in the final customer SDK delivery.
    pInitialData = pAVL_Chip->m_pInitialData;
	if( AVL63X1_EC_OK == r )
	{
		//load patch into the chip
		i1 = 0;
		uiCnt = 0;
		while(uiCnt < 3)
		{
			uiSize = DeChunk32(pInitialData+i1);
			if(0 == uiSize)
			{
				break;
			}
			// Increment index past the size field (4 bytes) and past first byte of address field (always zero)
			i1 += 5;

			// Send the address (3 bytes) + data (uiSize bytes)
			r |= II2C_Write_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), pInitialData+i1, (AVL_uint16)(uiSize+3));

			// Increment to the next burst of data to send
			i1 += (3 + uiSize);
			uiCnt++;
		}
		
		AVL_uchar fw_dw_app;

		r |= II2C_Read8_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_fw_download_method, &fw_dw_app);

		//Download_ram(pAVL_Chip, fw_dw_app, demod_mode);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_dtmb_compressed_ram_start_addr, 0x280000);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_dtmb_uncompressed_ram_start_addr,0x2d4000);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_dvbc_compressed_ram_start_addr, 0x287800);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_dvbc_uncompressed_ram_start_addr,0x2e0800);
		r |= II2C_Write8_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_fw_download_method ,fw_dw_app);
		r |= II2C_Write8_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_demod_mode, pAVL_Chip->m_current_demod_mode);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_core_ready_word, 0x00000000);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_c306_top_srst, 0);
	}

	//gettimeofday(&tpEnd, NULL);
	//timeUse = 1000*(tpEnd.tv_sec - tpStart.tv_sec) + 0.001 *(tpEnd.tv_usec - tpStart.tv_usec);
	//printf("time Use = %f\n",timeUse );

	return(r);
}


AVL63X1_ErrorCode IBase_TestSDRAM_63X1(struct AVL63X1_Chip * pAVL_Chip, AVL_puint32 puiTestResult, AVL_puint32 puiTestPattern)
{
    AVL63X1_ErrorCode r = AVL63X1_EC_OK;
    const AVL_uint16 uiTimeDelay = 100;
	const AVL_uint16 uiMaxRetries = 200;
	AVL_uint32 i=0;

	r = IBase_SendRxOP_63X1(OP_RX_SDRAM_TEST, pAVL_Chip);
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
		
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_sdram_test_return, puiTestPattern);
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rs_AVL63X1_sdram_test_result, puiTestResult);
	}

	return (r);
}


AVL63X1_ErrorCode IBase_SendRxOP_63X1(AVL_uchar ucOpCmd, struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uchar pucBuff[4];
	AVL_uint32 uiTemp;
	const AVL_uint16 uiTimeDelay = 10;
	const AVL_uint16 uiMaxRetries = 20;
	AVL_uint32 i;

/* BEGIN: Modified by zhwu, 2012/6/2 */
	r = AVL63X1_IBSP_WaitSemaphore(&(pAVL_Chip->m_semRx));
	//down(&m_semRx);
/* END:   Modified by zhwu, 2012/6/2 */
	i = 0;

	while (AVL63X1_EC_OK != IBase_GetRxOP_Status_63X1(pAVL_Chip))
	{
		if (uiMaxRetries < i++)
		{
			r |= AVL63X1_EC_RUNNING;
			break;
		}
		AVL63X1_IBSP_Delay(uiTimeDelay);
	}
	if( AVL63X1_EC_OK == r )
	{
		pucBuff[0] = ucOpCmd;
		pucBuff[1] = 0;
		pucBuff[2] = 0;
		pucBuff[3] = 0;
		uiTemp = DeChunk32(pucBuff);
		r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_command, uiTemp);
	}
	/* BEGIN: Modified by zhwu, 2012/6/2 */
	r |= AVL63X1_IBSP_ReleaseSemaphore(&(pAVL_Chip->m_semRx));
	//up(&m_semRx);
	/* END:   Modified by zhwu, 2012/6/2 */
	return(r);
}

//// The implementation of following functions is standard specific. The functions below will call the standard specific functions based on m_current_demod_mode member of the chip structure passed to them as argument.
AVL63X1_ErrorCode IBase_Halt_63X1( struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

    r = IBase_SendRxOP_63X1(OP_RX_HALT, pAVL_Chip);
	//This delay is needed to make sure the command gets processed by the firmware incase it is busy doing something else.
	r |= AVL63X1_IBSP_Delay(2);
	
    if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		r = AVL_DTMB_Halt(pAVL_Chip );
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		r = AVL_DVBC_Halt(pAVL_Chip );
	}
	
	return r;
}

AVL63X1_ErrorCode IBase_GetRunningLevel_63X1( enum AVL63X1_RunningLevel *pRunningLevel,  struct AVL63X1_Chip * pAVL_Chip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	
	if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		r = AVL_DTMB_GetRunningLevel(pRunningLevel, pAVL_Chip );
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		r = AVL_DVBC_GetRunningLevel(pRunningLevel, pAVL_Chip );
	}

	return r;
}

///II2C Repeater Module

AVL63X1_ErrorCode II2C_Repeater_SelectMode_63X1(AVL_uint32 uiSetMode , const struct AVL63X1_Chip * pAVLChip )
{
    AVL63X1_ErrorCode r = AVL63X1_EC_OK;
    
    if(MESSAGE_TYPE == uiSetMode)
    {
        r = II2C_Write32_63X1( pAVLChip->m_SlaveAddr, rc_AVL63X1_i2cm_repeater_type, MESSAGE_TYPE);
    }
    else if(BYTE_TYPE == uiSetMode)
    {
        r = II2C_Write32_63X1( pAVLChip->m_SlaveAddr, rc_AVL63X1_i2cm_repeater_type, BYTE_TYPE);
    }
	else if(BIT_TYPE == uiSetMode)
	{
		 r = II2C_Write32_63X1( pAVLChip->m_SlaveAddr, rc_AVL63X1_i2cm_repeater_type, BIT_TYPE);
	}
    else
    {
        return (AVL63X1_EC_GENERAL_FAIL);
    }

    return (r);
}


AVL63X1_ErrorCode II2C_Repeater_Initialize_63X1(AVL_uint16 uiTunerSlaveAddr, AVL_uint16 uiI2C_BusClock_kHz, struct AVL63X1_Chip * pAVL_Chip)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp, uiTemp2;
	AVL_uint16 uiClkDivValue;

    if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
        r = II2C_Read32_63X1(pAVL_Chip->m_SlaveAddr, rc_AVL_DTMB_i2cm_config, &uiTemp);
        uiTemp &= 0xFFFF0000;
        uiTemp |= (AVL_uint32)uiI2C_BusClock_kHz;
        r |= II2C_Write32_63X1(pAVL_Chip->m_SlaveAddr, rc_AVL_DTMB_i2cm_config, uiTemp); 
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
        r = II2C_Read32_63X1(pAVL_Chip->m_SlaveAddr, rc_AVL_DVBC_i2cm_config, &uiTemp);
        uiTemp &= 0xFFFF0000;
        uiTemp |= (AVL_uint32)uiI2C_BusClock_kHz;
        r |= II2C_Write32_63X1(pAVL_Chip->m_SlaveAddr, rc_AVL_DVBC_i2cm_config, uiTemp);
	}
	
	uiTemp = ((((AVL_uint32)uiTunerSlaveAddr) << 16) | 0x00000100);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_tuner_i2c_cntrl, uiTemp);

	if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DTMB_dmd_clk_Hz, &uiTemp);
		uiClkDivValue = (AVL_uint16)(uiTemp/8000/uiI2C_BusClock_kHz);
	}
	else if(pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL_DVBC_dmd_clk_Hz, &uiTemp);
		uiClkDivValue = (AVL_uint16)(uiTemp/8000/uiI2C_BusClock_kHz);
	}
	
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_tuner_i2c_clk_div, (AVL_uint32)uiClkDivValue);

	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_tuner_i2c_srst, 1);
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_tuner_i2c_srst, 0);
	
	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_intr_clr_reg, &uiTemp);
	uiTemp |= 0x4;
	r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_intr_clr_reg, uiTemp);

	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_i2cm_repeater_type, &uiTemp);
	r |= II2C_Read32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_tuner_i2c_cntrl	, &uiTemp2);
	if(AVL63X1_EC_OK == r)
	{
		if ((AVL63X1_II2C_Repeater_Type)(uiTemp) == MESSAGE_TYPE) //firmware
		{
			uiTemp2 = (uiTemp2 & 0xFFFFFFFE);
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_tuner_i2c_cntrl, uiTemp2);
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_tuner_i2cm_con, 0x000000C3);
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_tuner_hw_i2c_bit_rpt_cntrl, 0x0);
		}
		else if((AVL63X1_II2C_Repeater_Type)(uiTemp) == BYTE_TYPE) //hardware byte type
		{
			uiTemp2 = (uiTemp2 | 0x00000001);
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_tuner_i2c_cntrl, uiTemp2);
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_tuner_hw_i2c_bit_rpt_cntrl, 0x0);
		}
		else if((AVL63X1_II2C_Repeater_Type)(uiTemp) == BIT_TYPE) //hardware bit type
		{
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVLAPRT_tuner_hw_i2c_bit_rpt_clk_div, 0x2A);
			r |= II2C_Write32_63X1((AVL_uint16)(pAVL_Chip->m_SlaveAddr), rc_AVL63X1_tuner_hw_i2c_bit_rpt_cntrl, 0x2);

		}
		
	}
	return r;
}


AVL63X1_ErrorCode II2C_Repeater_FindByAddress_63X1(  AVL_uchar ucSlaveAddr, struct AVL63X1_Chip * pAVL_Chip, AVL_puint32 pIndex)
{
  AVL_uint32 i;

  for(i = 0; i < 2; i++)
  {
    if(ucSlaveAddr == (pAVL_Chip->m_RepeaterInfo[i].m_TunerAddress))
    {
      *pIndex = i;
      break;
    }
  }
  
  if(2 == i)
  {
    return AVL63X1_EC_GENERAL_FAIL;
  }
  else
  {
    return AVL63X1_EC_OK;
  }
	
}


AVL63X1_ErrorCode II2C_Repeater_SetReadBytes_63X1(struct AVL63X1_Chip * pAVL_Chip, AVL_uchar ucNumBytes)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	r = II2C_Write32_63X1(pAVL_Chip->m_SlaveAddr, rc_AVL63X1_tuner_i2c_read_cnt, ucNumBytes);

	return(r);
}

AVL63X1_ErrorCode II2C_Repeater_CheckTunerAddress_63X1(AVL_puint32 puiTunerAddrMismatch, struct AVL63X1_Chip * pAVL_Chip)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;

	r = II2C_Read32_63X1(pAVL_Chip->m_SlaveAddr, rs_AVL63X1_tuner_i2c_error_flag, &uiTemp);
	uiTemp &= 0x00000001;
	*puiTunerAddrMismatch = uiTemp;

	return(r);
}


///II2C Module
////the functions in this interface  have exact same implementation for both DTMB and DVBC. 
/// Put their implementaions(either from II2C.c in DVBC or DTMB SDK) in this file and replace AVL_DVBC(orAVL_DTMB) prefix with AVL63X1.
/// @cond
AVL_semaphore gI2C_Sem;
/// @endcond

AVL63X1_ErrorCode II2C_Initialize_63X1(void)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	static AVL_uchar gI2C_Sem_inited = 0;
	
	if( 0 == gI2C_Sem_inited )
	{
		gI2C_Sem_inited = 1;
		r = AVL63X1_IBSP_InitSemaphore(&gI2C_Sem);
	}
	return r;
}

AVL63X1_ErrorCode II2C_Read_63X1( AVL_uint16 uiSlaveAddr, AVL_uint32 uiOffset, AVL_puchar pucBuff, AVL_uint16 uiSize)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uchar pucBuffTemp[3];
	AVL_uint16 ui1, ui2;
	AVL_uint16 iSize;

	r = AVL63X1_IBSP_WaitSemaphore(&(gI2C_Sem));
	#if  1
	if( AVL63X1_EC_OK == r )
	{
		ChunkAddr(uiOffset, pucBuffTemp);
		ui1 = 3;
		r = AVL63X1_IBSP_I2C_Write(uiSlaveAddr, pucBuffTemp, &ui1);  
		if( AVL63X1_EC_OK == r )
		{
			iSize = uiSize;
			ui2 = 0;
			while( iSize > MAX_II2C_READ_SIZE )
			{
				ui1 = MAX_II2C_READ_SIZE;
				r |= AVL63X1_IBSP_I2C_Read(uiSlaveAddr, pucBuff+ui2, &ui1);
				ui2 += MAX_II2C_READ_SIZE;
				iSize -= MAX_II2C_READ_SIZE;
			}

			if( 0 != iSize )
			{
				r |= AVL63X1_IBSP_I2C_Read(uiSlaveAddr, pucBuff+ui2, &iSize);
			}
		}
	}
	#endif /* #if 0 */
	
	r |= AVL63X1_IBSP_ReleaseSemaphore(&(gI2C_Sem)); 
	return(r);

}
AVL63X1_ErrorCode II2C_Read8_63X1( AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_puchar puiData )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uchar Data;

	r = II2C_Read_63X1(uiSlaveAddr, uiAddr, &Data, 1);
	if( AVL63X1_EC_OK == r )
	{
		*puiData = Data;
	}
	return(r);
}
AVL63X1_ErrorCode II2C_Read16_63X1( AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_puint16 puiData )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uchar pBuff[2];

	r = II2C_Read_63X1(uiSlaveAddr, uiAddr, pBuff, 2);
	if( AVL63X1_EC_OK == r )
	{
		*puiData = DeChunk16(pBuff);
	}
	return(r);
}

AVL63X1_ErrorCode II2C_Read32_63X1( AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_puint32 puiData )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uchar pBuff[4];

	r = II2C_Read_63X1(uiSlaveAddr, uiAddr, pBuff, 4);
	if( AVL63X1_EC_OK == r )
	{
		*puiData = DeChunk32(pBuff);
	}
	return(r);
}

AVL63X1_ErrorCode II2C_ReadDirect_63X1( AVL_uint16 uiSlaveAddr, AVL_puchar pucBuff, AVL_uint16 uiSize)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint16 ui1, ui2;
	AVL_uint16 iSize;
	
	r = AVL63X1_IBSP_WaitSemaphore(&(gI2C_Sem));
	if( AVL63X1_EC_OK == r )
	{
		iSize = uiSize;
		ui2 = 0;
		while( iSize > MAX_II2C_READ_SIZE )
		{
			ui1 = MAX_II2C_READ_SIZE;
			r |= AVL63X1_IBSP_I2C_Read(uiSlaveAddr, pucBuff+ui2, &ui1);
			ui2 += MAX_II2C_READ_SIZE;
			iSize -= MAX_II2C_READ_SIZE;
		}

		if( 0 != iSize )
		{
			r |= AVL63X1_IBSP_I2C_Read(uiSlaveAddr, pucBuff+ui2, &iSize);
		}
	}
	r |= AVL63X1_IBSP_ReleaseSemaphore(&(gI2C_Sem)); 
	return(r);
}


AVL63X1_ErrorCode II2C_Write_63X1( AVL_uint16 uiSlaveAddr, AVL_puchar pucBuff, AVL_uint16 uiSize)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uchar pucBuffTemp[5];
	AVL_uint16 ui1, ui2, uTemp;
	AVL_uint16 iSize;
	AVL_uint32 uAddr;
	if( uiSize<3 )
	{
		pbierror("FUN:%s,LINE:%d,uiSize:%d\n",__FUNCTION__,__LINE__,uiSize);
		return(AVL63X1_EC_GENERAL_FAIL);	 //at least 3 bytes
	}

	uiSize -= 3;			//actual data size
	r = AVL63X1_IBSP_WaitSemaphore(&(gI2C_Sem));
	#if  1
	if( AVL63X1_EC_OK == r )
	{
		//dump address
		uAddr = pucBuff[0];
		uAddr = uAddr<<8;
		uAddr += pucBuff[1];
		uAddr = uAddr<<8;
		uAddr += pucBuff[2];

		iSize = uiSize;

		uTemp = (MAX_II2C_WRITE_SIZE-3) & 0xfffe; //how many bytes data we can transfer every time

		ui2 = 0;
		while( iSize > uTemp )
		{
			ui1 = uTemp+3;
			//save the data
			pucBuffTemp[0] = pucBuff[ui2];
			pucBuffTemp[1] = pucBuff[ui2+1];
			pucBuffTemp[2] = pucBuff[ui2+2];
			ChunkAddr(uAddr, pucBuff+ui2);
			r |= AVL63X1_IBSP_I2C_Write(uiSlaveAddr, pucBuff+ui2, &ui1);
			//restore data
			pucBuff[ui2] = pucBuffTemp[0];
			pucBuff[ui2+1] = pucBuffTemp[1];
			pucBuff[ui2+2] = pucBuffTemp[2];
			uAddr += uTemp;
			ui2 += uTemp;
			iSize -= uTemp;
		}
		ui1 = iSize+3;
		//save the data
		pucBuffTemp[0] = pucBuff[ui2];
		pucBuffTemp[1] = pucBuff[ui2+1];
		pucBuffTemp[2] = pucBuff[ui2+2];
		ChunkAddr(uAddr, pucBuff+ui2);
		r |= AVL63X1_IBSP_I2C_Write(uiSlaveAddr, pucBuff+ui2, &ui1);
		//restore data
		pucBuff[ui2] = pucBuffTemp[0];
		pucBuff[ui2+1] = pucBuffTemp[1];
		pucBuff[ui2+2] = pucBuffTemp[2];
		uAddr += iSize;
		ui2 += iSize;
	}
	#else
	{
		AVL_uchar pucBuffAddr[3];

		uAddr = pucBuff[0];
		uAddr = uAddr<<8;
		uAddr += pucBuff[1];
		uAddr = uAddr<<8;
		uAddr += pucBuff[2];
		ChunkAddr(uAddr, pucBuffAddr);
		//uiSize -= 3;
		printf("FUN:%s,LINE:%d,r:%d\n",__FUNCTION__,__LINE__,r);
		r = DRV_I2C_Write( uiSlaveAddr<<1, pucBuffAddr, 3, pucBuff, uiSize);
		printf("FUN:%s,LINE:%d,r:%d\n",__FUNCTION__,__LINE__,r);
	}
	#endif /* #if 0 */
	r |= AVL63X1_IBSP_ReleaseSemaphore(&(gI2C_Sem)); 
//printf("FUN:%s,LINE:%d,r:%d\n",__FUNCTION__,__LINE__,r);
	
	return(r);
}

AVL63X1_ErrorCode II2C_Write8_63X1( AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_uchar ucData )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uchar pBuff[4];

	ChunkAddr(uiAddr, pBuff);
	pBuff[3] = ucData;

	r = II2C_Write_63X1(uiSlaveAddr, pBuff, 4);
	return(r);
}



AVL63X1_ErrorCode II2C_Write16_63X1( AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_uint16 uiData )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uchar pBuff[5];

	ChunkAddr(uiAddr, pBuff);
	Chunk16(uiData, pBuff+3);

	r = II2C_Write_63X1(uiSlaveAddr, pBuff, 5);
	return(r);
}

AVL63X1_ErrorCode II2C_Write32_63X1( AVL_uint16 uiSlaveAddr, AVL_uint32 uiAddr, AVL_uint32 uiData )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uchar pBuff[7];

	ChunkAddr(uiAddr, pBuff);
	Chunk32(uiData, pBuff+3);
	r = II2C_Write_63X1(uiSlaveAddr, pBuff, 7);
	return(r);
}
AVL63X1_ErrorCode II2C_WriteDirect_63X1( AVL_uint16 uiSlaveAddr, AVL_puchar pucBuff, AVL_uint16 uiSize)
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uint16 ui1, ui2, uTemp;
	AVL_uint16 iSize;

	r = AVL63X1_IBSP_WaitSemaphore(&(gI2C_Sem));
	if( AVL63X1_EC_OK == r )
	{
		iSize = uiSize;
		uTemp = (MAX_II2C_WRITE_SIZE-3) & 0xfffe; 
		ui2 = 0;
		while( iSize > uTemp )
		{
			ui1 = uTemp;
			r |= AVL63X1_IBSP_I2C_Write(uiSlaveAddr, pucBuff+ui2, &ui1);
			ui2 += uTemp;
			iSize -= uTemp;
		}
		ui1 = iSize;
		r |= AVL63X1_IBSP_I2C_Write(uiSlaveAddr, pucBuff+ui2, &ui1);
		ui2 += iSize;
	}
	r |= AVL63X1_IBSP_ReleaseSemaphore(&(gI2C_Sem)); 

	return(r);
}


///II2C_Repeater Module
AVL63X1_ErrorCode II2C_Repeater_SendOP_63X1(AVL_puchar pBuff, AVL_uchar ucSize, struct AVL63X1_Chip * pAVLChip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	const AVL_uint16 uiTimeDelay = 40;
	const AVL_uint16 uiMaxRetries = 100;
	AVL_uint32 i;

	r = AVL63X1_IBSP_WaitSemaphore(&(pAVLChip->m_semI2CRepeater));
	i = 0;
	while (AVL63X1_EC_OK != II2C_Repeater_GetOPStatus_63X1(pAVLChip))
	{
		if (uiMaxRetries < i++)
		{
			r |= AVL63X1_EC_RUNNING;
			break;
		}
		AVL63X1_IBSP_Delay(uiTimeDelay);
	}

	if ( AVL63X1_EC_OK == r )
	{
		r = II2C_Write_63X1((AVL_uint16)(pAVLChip->m_SlaveAddr), pBuff, ucSize);   
	}
	r |= AVL63X1_IBSP_ReleaseSemaphore(&(pAVLChip->m_semI2CRepeater));

	return(r);
}


AVL63X1_ErrorCode II2C_Repeater_GetOPStatus_63X1(const struct AVL63X1_Chip * pAVLChip )
{
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	AVL_uchar pBuff[2];

	r = II2C_Read_63X1((AVL_uint16)(pAVLChip->m_SlaveAddr), rc_AVL63X1_i2cm_cmd_addr+I2CM_CMD_LENGTH-2, pBuff, 2);
	if ( AVL63X1_EC_OK == r )
	{
		if ( pBuff[1] != 0 )
		{
			r = AVL63X1_EC_RUNNING;
		}
	}
	return(r);
}


