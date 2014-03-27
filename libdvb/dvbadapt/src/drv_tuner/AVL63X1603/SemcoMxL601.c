/********************************************************************
* SemcoMxL601.c
* MxL601 Functions
* By WillowAndi 2011/11/14
*********************************************************************/

#include <unistd.h>
#include "SemcoMxL601_OEM_Drv.h"
#include "SemcoMxL601.h"
#include "pbitrace.h"

struct AVL63X1_Chip * pAVL_Semco_Chip;
MxL601ConfigInfo CI;

static int AccessBandWidthFromFile()
{
    int Ret = 0;
    //Ret = access("/mnt/expand/BandWidth", F_OK );
    Ret = access("/data/data/com.pbi.dvb/databases/BandWidth", F_OK );
    if( 0 == Ret ){
        return ANA_TV_DIG_CABLE_BW_8MHz;
    }
    return ANA_TV_DIG_CABLE_BW_6MHz;
}


static int _GetBandWidth(int BandWidth)
{
    int iRet = ANA_TV_DIG_CABLE_BW_6MHz;
    switch( BandWidth )
    {
        case 0:
        {
            iRet = ANA_TV_DIG_CABLE_BW_6MHz;
        }
        break;
        case 1:
        {
            iRet = ANA_TV_DIG_CABLE_BW_8MHz;
        }
        break;
    }
    pbiinfo("[%s %d] BandWidth = %d.\n",DEBUG_LOG, iRet);
    return iRet;
}

/* Initialize MxL601 */
AVL63X1_ErrorCode SemcoMxL601_Initialize(struct AVL_Tuner *pTuner)
{
	AVL63X1_ErrorCode r;
    SEM_MXL_COMMAND_T apiCmd;

	pAVL_Semco_Chip = pTuner->m_pAVL_Chip;

	CI.bIFInv = 1;			//0:IF Inversion Disable;  1:IF Inversion Enable;
	CI.bIFPath = 0;			//0:IF Path1;  1:IF Path2;
	CI.bPower3_3 = 0;		//0:3.3V single supply Enable;  1:3.3V single supply Disable
	CI.nBandWidth = DIG_TERR_BW_8MHz;	//Ref SEM_MXL_BW_E define
	CI.nGainLevel = 9;		//Ref Sharp2169 tuner driver
	CI.nMode = DIG_DVB_T;			//Ref SEM_MXL_SIGNAL_MODE_E define
	if (pTuner->m_pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DTMB)
	{
		CI.nMode = DIG_DVB_T;
		CI.nBandWidth = DIG_TERR_BW_8MHz;	//Ref SEM_MXL_BW_E define
	}
	else if (pTuner->m_pAVL_Chip->m_current_demod_mode == AVL_DEMOD_MODE_DVBC)
	{
		CI.nMode = DIG_DVB_C;		
		CI.nBandWidth = _GetBandWidth(pTuner->m_uiBandWidth);/*ANA_TV_DIG_CABLE_BW_8MHz;*/	//Ref SEM_MXL_BW_E define
	}
	CI.nSelAGC = 0;			//0:AGC1;  1:AGC2;
	/* BEGIN: Modified by zhwu, 2012/12/7 */
	CI.nSelIF = IF_6MHz;			//Ref SEM_MXL_IF_FREQ_E;
	/* END:   Modified by zhwu, 2012/12/7 */
	CI.nTOP = 66;
	CI.nType = 1;			//0:Internal AGC;  1:Exteral AGC;  Ref SEM_MXL_AGC_TYPE_E
	CI.nXtalCap = 31;


	/* Soft Reset */
	r = SemcoMxL601_SoftReset(pTuner);
	if (r != AVL63X1_EC_OK)
		return r;

	/* Overwrite Default */
	apiCmd.commandId = MXL_DEV_OVERWRITE_DEFAULT_CFG;
	apiCmd.MxLIf.cmdOverwriteDefault.I2cSlaveAddr = (AVL_uchar)pTuner->m_uiSlaveAddress;
	apiCmd.MxLIf.cmdOverwriteDefault.SingleSupply_3_3V = MXL_DISABLE;
	r = SemcoMxLWare601_API_ConfigDevice(&apiCmd);
	if(r != AVL63X1_EC_OK)
		return r;

	/* Xtal Setting : 16MHz */
	apiCmd.commandId = MXL_DEV_XTAL_SET_CFG;
	apiCmd.MxLIf.cmdXtalCfg.I2cSlaveAddr = (AVL_uchar)pTuner->m_uiSlaveAddress;
	apiCmd.MxLIf.cmdXtalCfg.XtalFreqSel  = XTAL_16MHz;
	apiCmd.MxLIf.cmdXtalCfg.XtalCap      = CI.nXtalCap;
	apiCmd.MxLIf.cmdXtalCfg.ClkOutEnable = MXL_ENABLE;
	apiCmd.MxLIf.cmdXtalCfg.ClkOutDiv    = MXL_DISABLE;

	if(CI.bPower3_3 == 0)
		apiCmd.MxLIf.cmdXtalCfg.SingleSupply_3_3V = MXL_ENABLE;  /* 3.3V single supply */
	else
		apiCmd.MxLIf.cmdXtalCfg.SingleSupply_3_3V = MXL_DISABLE; /* 3.3V single supply */

	apiCmd.MxLIf.cmdXtalCfg.XtalSharingMode = MXL_DISABLE; /* master case => Disable, slave case => enable */

	r = SemcoMxLWare601_API_ConfigDevice(&apiCmd);
	if(r != AVL63X1_EC_OK)
		return r;

	/* Power up setting */
	apiCmd.commandId = MXL_TUNER_POWER_UP_CFG;
	apiCmd.MxLIf.cmdTunerPoweUpCfg.I2cSlaveAddr = (AVL_uchar)pTuner->m_uiSlaveAddress;
	apiCmd.MxLIf.cmdTunerPoweUpCfg.Enable       = MXL_ENABLE;
	r = SemcoMxLWare601_API_ConfigTuner(&apiCmd);
	if(r != AVL63X1_EC_OK)
		return r;

	/* Set IF */
	r = SemcoMxL601_SetIF(pTuner);
	if(r != AVL63X1_EC_OK) /* IF 8MHz */
		return r;

	/* Set AGC */
	if(CI.nType == 0)
	{
		if(SemcoMxL601_SetAGC(pTuner)) /* Internal AGC */
		   return 1;
	}else{
		if(SemcoMxL601_SetAGC(pTuner)) /* External AGC */
		   return 1;
	}

	/* Set Mode */
	r = SemcoMxL601_SetMode(pTuner); 
	/* Set RF Performance */
    r |= SemcoMxL601_SetRFPerformance(pTuner);

	return r;
}

/* Soft Reset */
AVL63X1_ErrorCode SemcoMxL601_SoftReset(struct AVL_Tuner *pTuner)
{
	AVL63X1_ErrorCode r;
    SEM_MXL_COMMAND_T apiCmd;

	/* Soft Reset MxL601 */
	apiCmd.commandId = MXL_DEV_SOFT_RESET_CFG;
	apiCmd.MxLIf.cmdResetCfg.I2cSlaveAddr = (AVL_uchar)pTuner->m_uiSlaveAddress;
	r = SemcoMxLWare601_API_ConfigDevice(&apiCmd);
	return r;
}

/* Set IF */
AVL63X1_ErrorCode SemcoMxL601_SetIF(struct AVL_Tuner * pTuner)
{
	AVL63X1_ErrorCode r;
    SEM_MXL_COMMAND_T apiCmd;

	/* IF Out setting */
	apiCmd.commandId = MXL_DEV_IF_OUT_CFG;
	apiCmd.MxLIf.cmdIfOutCfg.I2cSlaveAddr = (AVL_uchar)pTuner->m_uiSlaveAddress;
	//apiCmd.MxLIf.cmdIfOutCfg.IFOutFreq = (SEM_MXL_IF_FREQ_E)CI.nSelIF;
	/* BEGIN: Modified by zhwu, 2012/12/7 */
	apiCmd.MxLIf.cmdIfOutCfg.ManualIFOutFreqInKHz = 6000;
	//apiCmd.MxLIf.cmdIfOutCfg.ManualIFOutFreqInKHz = 36000;
	/* END:   Modified by zhwu, 2012/12/7 */
	apiCmd.MxLIf.cmdIfOutCfg. ManualFreqSet = MXL_ENABLE;
    if(CI.bIFInv == 1)
		apiCmd.MxLIf.cmdIfOutCfg.IFInversion = MXL_ENABLE; 
	else
        apiCmd.MxLIf.cmdIfOutCfg.IFInversion = MXL_DISABLE;

	apiCmd.MxLIf.cmdIfOutCfg.GainLevel = CI.nGainLevel;

	if(CI.bIFPath == 0)
		apiCmd.MxLIf.cmdIfOutCfg.PathSel = IF_PATH1;
	else
		apiCmd.MxLIf.cmdIfOutCfg.PathSel = IF_PATH2;

	r = SemcoMxLWare601_API_ConfigDevice(&apiCmd);

	return r;
}

/* Set AGC */
AVL63X1_ErrorCode SemcoMxL601_SetAGC(struct AVL_Tuner * pTuner)
{
	AVL63X1_ErrorCode r;
    SEM_MXL_COMMAND_T apiCmd;

	/* AGC Setting */
	apiCmd.commandId = MXL_TUNER_AGC_CFG;
	apiCmd.MxLIf.cmdAgcSetCfg.I2cSlaveAddr = (AVL_uchar)pTuner->m_uiSlaveAddress;

	if(CI.nSelAGC == 0)
		apiCmd.MxLIf.cmdAgcSetCfg.AgcSel = AGC1;
	else
		apiCmd.MxLIf.cmdAgcSetCfg.AgcSel = AGC2;

	apiCmd.MxLIf.cmdAgcSetCfg.AgcType    = (SEM_MXL_AGC_TYPE_E)CI.nType;

	apiCmd.MxLIf.cmdAgcSetCfg.SetPoint   = CI.nTOP; /* AGC TOP */

    apiCmd.MxLIf.cmdAgcSetCfg.AgcPolarityInverstion = MXL_DISABLE; /* New Added in V2 - 2011.12.27 */

	r = SemcoMxLWare601_API_ConfigTuner(&apiCmd);

	return r;
}

/* Set Application Mode */
AVL63X1_ErrorCode SemcoMxL601_SetMode(struct AVL_Tuner * pTuner)
{
	AVL63X1_ErrorCode r;
    SEM_MXL_COMMAND_T apiCmd;

	/* Application Mode setting */
	apiCmd.commandId = MXL_TUNER_MODE_CFG;
	apiCmd.MxLIf.cmdModeCfg.I2cSlaveAddr   = (AVL_uchar)pTuner->m_uiSlaveAddress;
	apiCmd.MxLIf.cmdModeCfg.SignalMode     = (SEM_MXL_SIGNAL_MODE_E)CI.nMode;
    apiCmd.MxLIf.cmdModeCfg.XtalFreqSel    = XTAL_16MHz;    /* New Added in V2 - 2011.12.27 */
    apiCmd.MxLIf.cmdModeCfg.IFOutGainLevel = CI.nGainLevel; /* New Added in V2 - 2011.12.27 */
	r = SemcoMxLWare601_API_ConfigTuner(&apiCmd);

	return r;
}

/* Set 1/2RF Performance  */
AVL63X1_ErrorCode SemcoMxL601_SetRFPerformance(struct AVL_Tuner * pTuner)
{
	AVL63X1_ErrorCode r;
    SEM_MXL_COMMAND_T apiCmd;

	/* 1/2RF Performance Setting : Improve the 1/2RF performance  - 2012. 1.13 */
	apiCmd.commandId = MXL_TUNER_HLFRF_CFG;
	apiCmd.MxLIf.cmdHlfrfCfg.I2cSlaveAddr = (AVL_uchar)pTuner->m_uiSlaveAddress;

	if(CI.nMode == DIG_DVB_C || 
	   CI.nMode == DIG_ISDBT_ATSC ||
	   CI.nMode == DIG_DVB_T ||
	   CI.nMode ==	DIG_J83B)
			apiCmd.MxLIf.cmdHlfrfCfg.Enable = MXL_DISABLE; /* DTV ¡æ Disable , ATV ¡æ Enable */
	else
			apiCmd.MxLIf.cmdHlfrfCfg.Enable = MXL_ENABLE;

	r = SemcoMxLWare601_API_ConfigTuner(&apiCmd);

	return r;
}


/* Set Xtal and Power voltage */
AVL63X1_ErrorCode SemcoMxL601_SetXtalPower(struct AVL_Tuner * pTuner)
{
	AVL63X1_ErrorCode r;
    SEM_MXL_COMMAND_T apiCmd;

	/* Xtal Setting : 16MHz */
	apiCmd.commandId = MXL_DEV_XTAL_SET_CFG;
	apiCmd.MxLIf.cmdXtalCfg.I2cSlaveAddr = (AVL_uchar)pTuner->m_uiSlaveAddress;
	apiCmd.MxLIf.cmdXtalCfg.XtalFreqSel  = XTAL_16MHz;
	apiCmd.MxLIf.cmdXtalCfg.XtalCap      = CI.nXtalCap;
	apiCmd.MxLIf.cmdXtalCfg.ClkOutEnable = MXL_ENABLE;
	apiCmd.MxLIf.cmdXtalCfg.ClkOutDiv    = MXL_DISABLE;

	if(CI.bPower3_3 == 0)
		apiCmd.MxLIf.cmdXtalCfg.SingleSupply_3_3V = MXL_ENABLE;  /* 3.3V single supply */
	else
		apiCmd.MxLIf.cmdXtalCfg.SingleSupply_3_3V = MXL_DISABLE; /* 3.3V single supply */

	apiCmd.MxLIf.cmdXtalCfg.XtalSharingMode = MXL_DISABLE; /* master case => Disable, slave case => Enable. */

	r = SemcoMxLWare601_API_ConfigDevice(&apiCmd);

	return r;
}

/* Set Frequency */
AVL63X1_ErrorCode SemcoMxL601_Lock(struct AVL_Tuner * pTuner)
{
	AVL63X1_ErrorCode r;
    SEM_MXL_COMMAND_T apiCmd;

	/* Channel frequency & bandwidth setting */
	apiCmd.commandId = MXL_TUNER_CHAN_TUNE_CFG;
	apiCmd.MxLIf.cmdChanTuneCfg.I2cSlaveAddr = (AVL_uchar)pTuner->m_uiSlaveAddress;
	apiCmd.MxLIf.cmdChanTuneCfg.TuneType     = VIEW_MODE;
	apiCmd.MxLIf.cmdChanTuneCfg.BandWidth    = ( SEM_MXL_BW_E)CI.nBandWidth;
	apiCmd.MxLIf.cmdChanTuneCfg.FreqInHz     = pTuner->m_uiFrequency_Hz;
	apiCmd.MxLIf.cmdChanTuneCfg.SignalMode   = (SEM_MXL_SIGNAL_MODE_E)CI.nMode;
	apiCmd.MxLIf.cmdChanTuneCfg.XtalFreqSel  = XTAL_16MHz;
	r = SemcoMxLWare601_API_ConfigTuner(&apiCmd);
	if(r != AVL63X1_EC_OK)
		return r;

	/* Sequencer setting */
	apiCmd.commandId = MXL_TUNER_START_TUNE_CFG;
	apiCmd.MxLIf.cmdStartTuneCfg.I2cSlaveAddr = (AVL_uchar)pTuner->m_uiSlaveAddress;
	apiCmd.MxLIf.cmdStartTuneCfg.StartTune    = MXL_DISABLE;
	r = SemcoMxLWare601_API_ConfigTuner(&apiCmd);
	if(r != AVL63X1_EC_OK)
		return r;

	/* Sequencer setting */
	apiCmd.commandId = MXL_TUNER_START_TUNE_CFG;
	apiCmd.MxLIf.cmdStartTuneCfg.I2cSlaveAddr = (AVL_uchar)pTuner->m_uiSlaveAddress;
	apiCmd.MxLIf.cmdStartTuneCfg.StartTune    = MXL_ENABLE;
	r = SemcoMxLWare601_API_ConfigTuner(&apiCmd);
	if(r != AVL63X1_EC_OK)
		return r;

	/* Wait 100ms */
	SemcoMxL_Sleep(100);

	return 0;
}

/* Get Lock Status */
/* return : 1-lock, 0-unlock */
AVL63X1_ErrorCode SemcoMxL601_GetLockStatus(struct AVL_Tuner * pTuner)
{
	unsigned char bLock;
    SEM_MXL_COMMAND_T apiCmd;

	bLock = 0;

	/* Read back Tuner lock status */
	apiCmd.commandId = MXL_TUNER_LOCK_STATUS_REQ;
	apiCmd.MxLIf.cmdTunerLockReq.I2cSlaveAddr = (AVL_uchar)pTuner->m_uiSlaveAddress;
	if (AVL63X1_EC_OK == SemcoMxLWare601_API_GetTunerStatus(&apiCmd))
	{
		if (MXL_LOCKED == apiCmd.MxLIf.cmdTunerLockReq.RfSynLock &&	MXL_LOCKED == apiCmd.MxLIf.cmdTunerLockReq.RefSynLock)
		{
			pTuner->m_sStatus.m_uiLock = 1;
			return AVL63X1_EC_OK;
		}
		else
			pTuner->m_sStatus.m_uiLock = 0;
	}

	return AVL63X1_EC_GENERAL_FAIL;
}

/* Get Power Level */
float SemcoMxL601_GetPowerLevel(struct AVL_Tuner * pTuner)
{
	float nPowerLevel;
    SEM_MXL_COMMAND_T apiCmd;

	/* Read back Tuner lock status */
	apiCmd.commandId = MXL_TUNER_LOCK_STATUS_REQ;
	apiCmd.MxLIf.cmdTunerLockReq.I2cSlaveAddr = (AVL_uchar)pTuner->m_uiSlaveAddress;
	if(MXL_TRUE == SemcoMxLWare601_API_GetTunerStatus(&apiCmd))
	{
      nPowerLevel = apiCmd.MxLIf.cmdTunerPwrReq.RxPwr;
	}else{
      return 1;
	}

	return nPowerLevel;
}

/* Get Chip Information */
void SemcoMxL601_GetChipInfo(struct AVL_Tuner * pTuner, int *lpChipId, int *lpChipVer)
{
	SEM_MXL_COMMAND_T apiCmd;

	apiCmd.commandId = MXL_DEV_ID_VERSION_REQ;
	apiCmd.MxLIf.cmdTunerLockReq.I2cSlaveAddr = (AVL_uchar)pTuner->m_uiSlaveAddress;

	SemcoMxLWare601_API_GetDeviceStatus(&apiCmd);

	*lpChipId  = (int)apiCmd.MxLIf.cmdDevInfoReq.ChipId;
	*lpChipVer = (int)apiCmd.MxLIf.cmdDevInfoReq.ChipVersion;
}