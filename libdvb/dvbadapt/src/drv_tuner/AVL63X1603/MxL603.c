
#include <unistd.h>
#include "MxL603.h"
#include "pbitrace.h"

struct AVL63X1_Chip * pAVLChipForMxL603;
AVL_uchar devId = 0x61;

AVL63X1_ErrorCode MxL603_Initialize(struct AVL_Tuner *pTuner)
{
	//AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	MXL_STATUS status = MXL_TRUE;
	MXL_BOOL singleSupply_3_3V;
	MXL603_XTAL_SET_CFG_T xtalCfg;
	MXL603_IF_OUT_CFG_T ifOutCfg;
	MXL603_AGC_CFG_T agcCfg;
	MXL603_TUNER_MODE_CFG_T tunerModeCfg;

	devId = (AVL_uchar)pTuner->m_uiSlaveAddress;
	pAVLChipForMxL603 = pTuner->m_pAVL_Chip;

	//Step 1 : Soft Reset MxL603
	status = MxLWare603_API_CfgDevSoftReset(devId);
	if (status != MXL_SUCCESS)
	{
		//printf("Error! MxLWare603_API_CfgDevSoftReset\n");
		return AVL63X1_EC_GENERAL_FAIL;
	}

	//Step 2 : Overwrite Default
	singleSupply_3_3V = MXL_ENABLE;
	status = MxLWare603_API_CfgDevOverwriteDefaults(devId, singleSupply_3_3V);
	if (status != MXL_SUCCESS)
	{
		//printf("Error! MxLWare603_API_CfgDevOverwriteDefaults\n");    
		return AVL63X1_EC_GENERAL_FAIL;
	}

	//Step 3 : XTAL Setting
	xtalCfg.xtalFreqSel = MXL603_XTAL_16MHz;
	xtalCfg.xtalCap = 12;
	xtalCfg.clkOutEnable = MXL_ENABLE;
	xtalCfg.clkOutDiv = MXL_DISABLE;
	xtalCfg.clkOutExt = MXL_DISABLE;
	xtalCfg.singleSupply_3_3V = MXL_ENABLE;
	xtalCfg.XtalSharingMode = MXL_DISABLE;
	status = MxLWare603_API_CfgDevXtal(devId, xtalCfg);
	if (status != MXL_SUCCESS)
	{
		//printf("Error! MxLWare603_API_CfgDevXtal\n");    
		return AVL63X1_EC_GENERAL_FAIL;
	}

	//Step 4 : IF Out setting
	ifOutCfg.ifOutFreq = MXL603_IF_6MHz;
	ifOutCfg.ifInversion = MXL_DISABLE;
	ifOutCfg.gainLevel = 11;
	ifOutCfg.manualFreqSet = MXL_DISABLE;
	ifOutCfg.manualIFOutFreqInKHz = 0;
	status = MxLWare603_API_CfgTunerIFOutParam(devId, ifOutCfg);
	if (status != MXL_SUCCESS)
	{
		//printf("Error! MxLWare603_API_CfgTunerIFOutParam\n");    
		return AVL63X1_EC_GENERAL_FAIL;
	}

	//Step 5 : AGC Setting
	agcCfg.agcType = MXL603_AGC_EXTERNAL;
	agcCfg.setPoint = 66;
	agcCfg.agcPolarityInverstion = MXL_DISABLE;
	status = MxLWare603_API_CfgTunerAGC(devId, agcCfg);
	if (status != MXL_SUCCESS)
	{
		//printf("Error! MxLWare603_API_CfgTunerAGC\n");    
		return AVL63X1_EC_GENERAL_FAIL;
	}

	//Step 6 : Application Mode setting
	tunerModeCfg.signalMode = AVL_DEMOD_MODE_DVBC;
	if (pTuner->m_pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		tunerModeCfg.signalMode = MXL603_DIG_DVB_T;
	}
	else if (pTuner->m_pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		tunerModeCfg.signalMode = MXL603_DIG_DVB_C;
	}
	tunerModeCfg.ifOutFreqinKHz = 6000;
	tunerModeCfg.xtalFreqSel = MXL603_XTAL_16MHz;
	tunerModeCfg.ifOutGainLevel = 11;
	status = MxLWare603_API_CfgTunerMode(devId, tunerModeCfg);
	if (status != MXL_SUCCESS)
	{
		//printf("Error! MxLWare603_API_CfgTunerMode\n");    
		return AVL63X1_EC_GENERAL_FAIL;
	}

	return AVL63X1_EC_OK;
}

AVL63X1_ErrorCode MxL603_GetStatus(struct AVL_Tuner *pTuner)
{
	//AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	MXL_STATUS status; 
	MXL_BOOL refLockPtr = MXL_UNLOCKED;
	MXL_BOOL rfLockPtr = MXL_UNLOCKED;		

	status = MxLWare603_API_ReqTunerLockStatus(devId, &rfLockPtr, &refLockPtr);
	if (status == MXL_TRUE )
	{
		if (MXL_LOCKED == rfLockPtr && MXL_LOCKED == refLockPtr)
		{
			pTuner->m_sStatus.m_uiLock = 1;
			return AVL63X1_EC_OK;
		}
		else
			return AVL63X1_EC_GENERAL_FAIL;
	}
	return AVL63X1_EC_GENERAL_FAIL;
}

static MXL603_BW_E _GetBandWidth(int BandWidth)
{

    MXL603_BW_E iRet = MXL603_CABLE_BW_6MHz;
    switch( BandWidth )
    {
        case 0:
    {
            iRet = MXL603_CABLE_BW_6MHz;
    }
        break;
        case 1:
        {
            iRet = MXL603_CABLE_BW_8MHz;
        }
        break;
    }
    pbiinfo("[%s %d] BandWidth = %d.\n",DEBUG_LOG, iRet);
    return iRet;
}

AVL63X1_ErrorCode MxL603_Lock(struct AVL_Tuner *pTuner)
{
	//AVL63X1_ErrorCode r = AVL63X1_EC_OK;
	MXL_STATUS status; 
	MXL603_CHAN_TUNE_CFG_T chanTuneCfg;

	chanTuneCfg.freqInHz = pTuner->m_uiFrequency_Hz;		//Unit:Hz
	chanTuneCfg.bandWidth = MXL603_TERR_BW_8MHz;
	chanTuneCfg.signalMode = MXL603_DIG_DVB_T;
	if (pTuner->m_pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		chanTuneCfg.signalMode = MXL603_DIG_DVB_T;
		chanTuneCfg.bandWidth = MXL603_TERR_BW_8MHz;
	}
	else if (pTuner->m_pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		chanTuneCfg.signalMode = MXL603_DIG_DVB_C;
		chanTuneCfg.bandWidth = _GetBandWidth(pTuner->m_uiBandWidth);//Modified from F835, 2013/2/23 
	}
	chanTuneCfg.xtalFreqSel = MXL603_XTAL_16MHz;
	chanTuneCfg.startTune = MXL_START_TUNE;
	status = MxLWare603_API_CfgTunerChanTune(devId, chanTuneCfg);
	if (status != MXL_SUCCESS)
	{
		return AVL63X1_EC_GENERAL_FAIL;
		//printf("Error! MxLWare603_API_CfgTunerChanTune\n");    
	}

	// Wait 15 ms 
	MxLWare603_OEM_Sleep(15);

	return AVL63X1_EC_OK;
}
