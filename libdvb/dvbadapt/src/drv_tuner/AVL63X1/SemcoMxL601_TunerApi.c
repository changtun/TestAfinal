/*******************************************************************************
 *
 * FILE NAME          : MxL601_TunerApi.cpp
 * 
 * AUTHOR             : Sunghoon Park
 *                      Dong Liu 
 *                      
 *
 * DATE CREATED       : 07/12/2011
 *                      11/30/2011
 *
 * DESCRIPTION        : This file contains MxL601 driver APIs
 *
 *
 *******************************************************************************
 *                Copyright (c) 2011, MaxLinear, Inc.
 ******************************************************************************/

#include <math.h>
#include "SemcoMxL601_TunerApi.h"
#include "SemcoMxL_Debug.h"
#include "pbitrace.h"
/* MxLWare Driver version for MxL601 */
const UINT8 SEM_MxLWareDrvVersion[] = {2, 1, 6, 1}; 

// Candidate version number, 0 means for release version 
const UINT8 SEM_BuildNumber = 0; 

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_ConfigDevReset - MXL_DEV_SOFT_RESET_CFG
--| 
--| AUTHOR        : Dong Liu
--|
--| DATE CREATED  : 03/23/2011  
--|
--| DESCRIPTION   : By writing any value into address 0xFF, all control 
--|                 registers are initialized to the default value.
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_ConfigDevReset(PSEM_MXL_RESET_CFG_T resetCfg)
{
  UINT8 status = MXL_TRUE;
  
  MxL_DLL_DEBUG0("MXL_DEV_SOFT_RESET_CFG \n"); 
  
  // Write 0xFF with 0 to reset tuner 
  status = SemcoCtrl_WriteRegister(resetCfg->I2cSlaveAddr, SEM_AIC_RESET_REG, 0x00); 

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_ConfigDevOverwriteDefault - 
--|                                               MXL_DEV_OVERWRITE_DEFAULT_CFG
--| 
--| AUTHOR        : Sunghoon Park
--|                 Dong Liu
--|
--| DATE CREATED  : 06/14/2011  
--|                 06/21/2011
--|
--| DESCRIPTION   : Register(s) that requires default values to be overwritten 
--|                 during initialization
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_ConfigDevOverwriteDefault(PSEM_MXL_OVERWRITE_DEFAULT_CFG_T overDefaultPtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 readData = 0;

  MxL_DLL_DEBUG0("MXL_DEV_OVERWRITE_DEFAULT_CFG \n"); 
  status |= SemcoCtrl_ProgramRegisters(overDefaultPtr->I2cSlaveAddr, MxL_OverwriteDefaults);

  status |= SemcoCtrl_WriteRegister(overDefaultPtr->I2cSlaveAddr, 0x00, 0x01);
  status |= SemcoCtrl_ReadRegister(overDefaultPtr->I2cSlaveAddr, 0x31, &readData);
  readData &= 0x2F;
  readData |= 0xD0;
  status |= SemcoCtrl_WriteRegister(overDefaultPtr->I2cSlaveAddr, 0x31, readData);
  status |= SemcoCtrl_WriteRegister(overDefaultPtr->I2cSlaveAddr, 0x00, 0x00);


  /* If Single supply 3.3v is used */
  if (MXL_ENABLE == overDefaultPtr->SingleSupply_3_3V)
    status |= SemcoCtrl_WriteRegister(overDefaultPtr->I2cSlaveAddr, SEM_MAIN_REG_AMP, 0x04);

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_ConfigDevPowerMode - MXL_DEV_POWER_MODE_CFG
--| 
--| AUTHOR        : Dong Liu
--|                 Sunghoon Park
--|
--| DATE CREATED  : 06/21/2011
--|                 06/15/2011
--|
--| DESCRIPTION   : This API configures MxL601 power mode 
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_ConfigDevPowerMode(PSEM_MXL_PWR_MODE_CFG_T pwrModePtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 i2cAddr; 

  MxL_DLL_DEBUG0("MXL_DEV_POWER_MODE_CFG \n");
  i2cAddr = pwrModePtr->I2cSlaveAddr;

  switch(pwrModePtr->PowerMode)
  {
    case MXL_PWR_MODE_SLEEP:
      break;

    case MXL_PWR_MODE_ACTIVE:
      status |= SemcoCtrl_WriteRegister(i2cAddr, SEM_TUNER_ENABLE_REG, MXL_ENABLE);
      status |= SemcoCtrl_WriteRegister(i2cAddr, SEM_START_TUNE_REG, MXL_ENABLE);
      break;

    case MXL_PWR_MODE_STANDBY:
      status |= SemcoCtrl_WriteRegister(i2cAddr, SEM_START_TUNE_REG, MXL_DISABLE);
      status |= SemcoCtrl_WriteRegister(i2cAddr, SEM_TUNER_ENABLE_REG, MXL_DISABLE);
      break;

    default:
      return MXL_FALSE;
  }

  return(MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_ConfigDevXtalSet - MXL_DEV_XTAL_SET_CFG
--| 
--| AUTHOR        : Sunghoon Park
--|                 Dong Liu
--|
--| DATE CREATED  : 06/14/2011
--|                 10/20/2011
--|                 11/13/2011 (MK) - Update Reg address for V2
--|
--| DESCRIPTION   : This API is used to configure XTAL settings of MxL601 tuner
--|                 device. XTAL settings include frequency, capacitance & 
--|                 clock out
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_ConfigDevXtalSet(PSEM_MXL_XTAL_SET_CFG_T xtalSetPtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 control = 0;
 
  MxL_DLL_DEBUG0("MXL_DEV_XTAL_SET_CFG \n"); 

  // XTAL freq and cap setting, Freq set is located at bit<5>, cap bit<4:0> 
  control = (UINT8)((xtalSetPtr->XtalFreqSel << 5) | (xtalSetPtr->XtalCap & 0x1F));  
  control |= (xtalSetPtr->ClkOutEnable << 7);
  status |= SemcoCtrl_WriteRegister(xtalSetPtr->I2cSlaveAddr, SEM_XTAL_CAP_CTRL_REG, control);

  // XTAL frequency div 4 setting <1> and  XTAL clock out enable <0>
  // XTAL sharing mode in slave
  control = (0x01 & (UINT8)xtalSetPtr->ClkOutDiv);
  if (xtalSetPtr->XtalSharingMode == MXL_ENABLE) control |= 0x40; 
  else control &= 0x01; 
  status |= SemcoCtrl_WriteRegister(xtalSetPtr->I2cSlaveAddr, SEM_XTAL_ENABLE_DIV_REG, control); 

  // Main regulator re-program
  if (MXL_ENABLE == xtalSetPtr->SingleSupply_3_3V)
    status |= SemcoCtrl_WriteRegister(xtalSetPtr->I2cSlaveAddr, SEM_MAIN_REG_AMP, 0x14);
  
  return(MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_ConfigDevIfOutSet - MXL_DEV_IF_OUT_CFG
--| 
--| AUTHOR        : Sunghoon Park 
--|                 Dong Liu
--|
--| DATE CREATED  : 06/15/2011
--|                 09/22/2011
--|
--| DESCRIPTION   : This function is used to configure IF out settings of MxL601 
--|                 tuner device.
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_ConfigDevIfOutSet(PSEM_MXL_IF_OUT_CFG_T ifOutPtr)
{
  UINT16 ifFcw;
  UINT8 status = MXL_TRUE;
  UINT8 readData = 0;
  UINT8 control = 0;
 
  //MxL_DLL_DEBUG0("%s: Manual set = %d \n", 
  //                              __FUNCTION__, ifOutPtr->ManualFreqSet); 

  // Read back register for manual IF Out 
  status |= SemcoCtrl_ReadRegister(ifOutPtr->I2cSlaveAddr, SEM_IF_FREQ_SEL_REG, &readData);

  if (ifOutPtr->ManualFreqSet == MXL_ENABLE)
  {
    // IF out manual setting : bit<5>
    readData |= 0x20;
    status |= SemcoCtrl_WriteRegister(ifOutPtr->I2cSlaveAddr, SEM_IF_FREQ_SEL_REG, readData);

    MxL_DLL_DEBUG0("Manual Freq set = %d \n", ifOutPtr->ManualIFOutFreqInKHz); 

    // Manual IF freq set
    ifFcw = (UINT16)(ifOutPtr->ManualIFOutFreqInKHz * 8192 / 216000);
    control = (ifFcw & 0xFF); // Get low 8 bit 
    status |= SemcoCtrl_WriteRegister(ifOutPtr->I2cSlaveAddr, SEM_IF_FCW_LOW_REG, control); 

    control = ((ifFcw >> 8) & 0x0F); // Get high 4 bit 
    status |= SemcoCtrl_WriteRegister(ifOutPtr->I2cSlaveAddr, SEM_IF_FCW_HIGH_REG, control);
  }
  else 
  {
    // bit<5> = 0, use IF frequency from IF frequency table  
    readData &= 0xC0;

    // IF Freq <4:0>
    readData |= ifOutPtr->IFOutFreq;
    status |= SemcoCtrl_WriteRegister(ifOutPtr->I2cSlaveAddr, SEM_IF_FREQ_SEL_REG, readData);
  }
  
  // Set spectrum invert, gain level and IF path 
  // Spectrum invert indication is bit<7:6>
  if (MXL_ENABLE == ifOutPtr->IFInversion)
    control = 0x3 << 6;

  // Path selection is bit<5:4> 
  control += ((ifOutPtr->PathSel & 0x03) << 4);

  // Gain level is bit<3:0> 
  control += (ifOutPtr->GainLevel & 0x0F);
  status |= SemcoCtrl_WriteRegister(ifOutPtr->I2cSlaveAddr, SEM_IF_PATH_GAIN_REG, control);

  return(MXL_STATUS) status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_ConfigDevGPO - MXL_DEV_GPO_CFG
--| 
--| AUTHOR        : Dong Liu
--|
--| DATE CREATED  : 3/24/2011 
--|
--| DESCRIPTION   : This API configures GPOs of MxL601 tuner device.
--|                 (General Purpose Output Port) of the MxL601.
--|                 There are 3 GPOs in MxL601  
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS  SemcoMxL601_ConfigDevGPO(PSEM_MXL_GPO_CFG_T gpoParamPtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 regData = 0;

  MxL_DLL_DEBUG0("MXL_DEV_GPO_CFG \n"); 

  status |= SemcoCtrl_ReadRegister(gpoParamPtr->I2cSlaveAddr, SEM_GPO_SETTING_REG, &regData);

  switch (gpoParamPtr->GPOId)
  {
    case GPO1:
      if (PORT_AUTO_CTRL == gpoParamPtr->GPOState)
      {
        regData &= 0xFE;
      }
      else
      {
        regData &= 0xFC;
        // Bit<0> = 1, user manual set GP1
        regData |= (UINT8)(0x01 | (gpoParamPtr->GPOState << 1)); 
      }
      break;
    
    case GPO2:
      if (PORT_AUTO_CTRL == gpoParamPtr->GPOState)
      {
        regData &= 0xFB;
      }
      else
      {
        regData &= 0xF3;
        // Bit<2> = 1, user manual set GP2
        regData |= (UINT8) (0x04 | (gpoParamPtr->GPOState << 3)); 
      }
      break;

    case GPO3:
      if (PORT_AUTO_CTRL == gpoParamPtr->GPOState)
      {
        regData &= 0xEF;
      }
      else
      {
        regData &= 0xCF;

        // Bit<4> = 1, user manual set GP3
        regData |= (UINT8)(0x10 | (gpoParamPtr->GPOState << 5)); 
      }
      break;

    default:
      status |= MXL_FALSE;
      break;
  }
 
  if (MXL_FALSE != status)
    status |= SemcoCtrl_WriteRegister(gpoParamPtr->I2cSlaveAddr, SEM_GPO_SETTING_REG, regData);

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_GetDeviceInfo - MXL_DEV_ID_VERSION_REQ
--| 
--| AUTHOR        : Dong Liu
--|
--| DATE CREATED  : 6/21/2011 
--|
--| DESCRIPTION   : This function is used to get MxL601 version information.
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/
  
static MXL_STATUS SemcoMxL601_GetDeviceInfo(PSEM_MXL_DEV_INFO_T DevInfoPtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 readBack = 0;
  UINT8 i2cAddr = 0;
  UINT8 k = 0;

  MxL_DLL_DEBUG0("MXL_DEV_ID_VERSION_REQ \n");

  i2cAddr = DevInfoPtr->I2cSlaveAddr;

  status |= SemcoCtrl_ReadRegister(i2cAddr, SEM_CHIP_ID_REQ_REG, &readBack);
  DevInfoPtr->ChipId = (readBack & 0xFF); 

  status |= SemcoCtrl_ReadRegister(i2cAddr, SEM_CHIP_VERSION_REQ_REG, &readBack);
  DevInfoPtr->ChipVersion = (readBack & 0xFF); 

  MxL_DLL_DEBUG0("Chip ID = 0x%d, Version = 0x%d \n", 
                                    DevInfoPtr->ChipId, DevInfoPtr->ChipVersion);
  
  // Get MxLWare version infromation
  for (k = 0; k < MXL_VERSION_SIZE; k++)
    DevInfoPtr->MxLWareVer[k] = SEM_MxLWareDrvVersion[k];
  
  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_GetDeviceGpoState - MXL_DEV_GPO_STATE_REQ
--| 
--| AUTHOR        : Dong Liu
--|
--| DATE CREATED  : 3/24/2011
--|
--| DESCRIPTION   : This API is used to get GPO's status information from
--|                 MxL601 tuner device
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS  SemcoMxL601_GetDeviceGpoState(PSEM_MXL_GPO_INFO_T gpoPtr)
{
  MXL_STATUS status = MXL_TRUE;
  UINT8 regData = 0;

  MxL_DLL_DEBUG0("MXL_DEV_GPO_STATE_REQ \n"); 

  status = SemcoCtrl_ReadRegister(gpoPtr->I2cSlaveAddr, SEM_GPO_SETTING_REG, &regData);

  // GPO1 bit<1:0>
  if ((regData & 0x01) == 0) gpoPtr->GPO1 = PORT_AUTO_CTRL;
  else gpoPtr->GPO1 = (SEM_MXL_GPO_STATE_E)((regData & 0x02) >> 1);

  // GPO2 bit<3:2>
  if ((regData & 0x04) == 0) gpoPtr->GPO2 = PORT_AUTO_CTRL;
  else gpoPtr->GPO2 = (SEM_MXL_GPO_STATE_E)((regData & 0x08) >> 3);

  // GPO3 bit<5:4>
  if ((regData & 0x10) == 0) gpoPtr->GPO3 = PORT_AUTO_CTRL;
  else gpoPtr->GPO3 = (SEM_MXL_GPO_STATE_E)((regData & 0x20) >> 5);

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_ConfigTunerEnableState - MXL_TUNER_POWER_UP_CFG
--| 
--| AUTHOR        : Dong Liu
--|                 Sunghoon Park
--|
--| DATE CREATED  : 1/27/2011
--|                 6/22/2011
--|
--| DESCRIPTION   : This function is used to power up/down MxL601 tuner module
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_ConfigTunerPowerUp(PSEM_MXL_POWER_UP_CFG_T topMasterCfgPtr) 
{
  UINT8 status = MXL_TRUE;
  UINT8 regData = 0; 
  
  MxL_DLL_DEBUG0("MXL_TUNER_POWER_UP_CFG, ENABLE = %d \n", topMasterCfgPtr->Enable);

  // Power up tuner <0> = 1 for enable, 0 for disable
  if (MXL_ENABLE == topMasterCfgPtr->Enable) regData |= 0x01;

  status = SemcoCtrl_WriteRegister(topMasterCfgPtr->I2cSlaveAddr, SEM_TUNER_ENABLE_REG, regData);

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_ConfigTunerSequenceSet - MXL_TUNER_START_TUNE_CFG
--| 
--| AUTHOR        : Sunghoon Park
--|                 Dong Liu
--|                 Mahee
--|                 
--|
--| DATE CREATED  : 03/24/2011
--|                 08/04/2011
--|                 09/23/2011 - 1.1.11.1 
--|                 Change in flow chart for resetting RSSI when START_TUNE = 1
--|
--| DESCRIPTION   : This function is used to start or abort channel tune.
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_ConfigTunerSequenceSet(PSEM_MXL_START_TUNE_CFG_T sequnCfgPtr) 
{
  UINT8 status = MXL_TRUE;
  
  MxL_DLL_DEBUG0("MXL_TUNER_START_TUNE_CFG, SEQUENCE SET = %d \n", 
                                                      sequnCfgPtr->StartTune);

  status |= SemcoCtrl_WriteRegister(sequnCfgPtr->I2cSlaveAddr, SEM_START_TUNE_REG, 0x00); 

  // Bit <0> 1 : start , 0 : abort calibrations
  if (sequnCfgPtr->StartTune == MXL_ENABLE) 
    status |= SemcoCtrl_WriteRegister(sequnCfgPtr->I2cSlaveAddr, SEM_START_TUNE_REG, 0x01); 
  
  return(MXL_STATUS) status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_ConfigTunerApplicationModeSet - MXL_TUNER_MODE_CFG
--| 
--| AUTHOR        : Dong Liu
--|                 Sunghoon Park
--|
--| DATE CREATED  : 3/24/2011
--|                 6/16/2011
--|                 11/13/2011 (MK) - Change register address of 0x53 & 0x54 
--|                                   with 0x5A & 0x5B (SEM_DIG_ANA_IF_CFG_0 & 
--|                                   SEM_DIG_ANA_IF_CFG_1)
--|                                 - Update Application mode settings.
--|                 12/28/2011 (MK) - support conditional compilation for 
--|                                   customer specific settings.
--|
--| DESCRIPTION   : This fucntion is used to configure MxL601 tuner's 
--|                 application modes like DVB-T, DVB-C, ISDB-T, PAL, SECAM, 
--|                 ATSC, NTSC etc.
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_ConfigTunerApplicationModeSet(PSEM_MXL_TUNER_MODE_CFG_T modeSetPtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 dfeRegData = 0;
  SEM_REG_CTRL_INFO_T* tmpRegTable = NULL;

  {
    MxL_DLL_DEBUG0("MXL_TUNER_MODE_CFG, application mode = %d , IF = %d\n", 
                                                            modeSetPtr->SignalMode,
                                                            modeSetPtr->IFOutFreqinKHz);

    switch(modeSetPtr->SignalMode)
    {
      // Application modes settings for NTSC mode
      case ANA_NTSC_MODE:
        tmpRegTable = AnalogNtsc;
        status |= SemcoCtrl_ProgramRegisters(modeSetPtr->I2cSlaveAddr, tmpRegTable);

        if (modeSetPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ)
        {
          // Low power IF dependent settings
#ifdef CUSTOMER_SPECIFIC_SETTING_1
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xFE);
#else
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0x7E); 
#endif
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x10);

          if (modeSetPtr->XtalFreqSel == XTAL_16MHz) 
          {
            tmpRegTable = Ntsc_16MHzRfLutSwpLIF;
            dfeRegData = 0x00;
          }
          else 
          {
            tmpRegTable = Ntsc_24MHzRfLutSwpLIF;
            dfeRegData = 0x01;
          }
          
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, 0xDD, dfeRegData);
          
        }
        else
        {
          // High power IF dependent settings
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xD9);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x16);
  
          tmpRegTable = Ntsc_RfLutSwpHIF;
        }

        if (tmpRegTable)
          status |= SemcoCtrl_ProgramRegisters(modeSetPtr->I2cSlaveAddr, tmpRegTable);
        else
          status |= MXL_FALSE;
        
        break;

      // Application modes settings for PAL-BG mode  
      case ANA_PAL_BG:

        tmpRegTable = AnalogPal;
        status |= SemcoCtrl_ProgramRegisters(modeSetPtr->I2cSlaveAddr, tmpRegTable);

        if (modeSetPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ)
        {
          // Low power IF dependent settings
#ifdef CUSTOMER_SPECIFIC_SETTING_1
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xFE);
#else
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0x7E); 
#endif
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x10);

          status |= SemcoCtrl_ProgramRegisters(modeSetPtr->I2cSlaveAddr, Pal_RfLutSwpLIF);
        }
        else
        {
          // High power IF dependent settings
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xD9);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x16);
        }

        break;

      // Application modes settings for PAL-I mode  
      case ANA_PAL_I:
        status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, 0x0C, 0x0B);
        
        tmpRegTable = AnalogPal;
        status |= SemcoCtrl_ProgramRegisters(modeSetPtr->I2cSlaveAddr, tmpRegTable);

        if (modeSetPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ)
        {
          // Low power IF dependent settings
#ifdef CUSTOMER_SPECIFIC_SETTING_1
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xFE);
#else
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0x7E); 
#endif
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x10);

          status |= SemcoCtrl_ProgramRegisters(modeSetPtr->I2cSlaveAddr, Pal_RfLutSwpLIF);
        }
        else
        {
          // High power IF dependent settings
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xD9);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x16);

          status |= SemcoCtrl_ProgramRegisters(modeSetPtr->I2cSlaveAddr, PalI_RfLutSwpHIF);  

        }

        break;
  
      // Application modes settings for PAL-D mode    
      case ANA_PAL_D:
        status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, 0x0C, 0x0C);
        
        tmpRegTable = AnalogPal;
        status |= SemcoCtrl_ProgramRegisters(modeSetPtr->I2cSlaveAddr, tmpRegTable);

        if (modeSetPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ)
        {
          // Low power IF dependent settings
#ifdef CUSTOMER_SPECIFIC_SETTING_1
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xFE);
#else
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0x7E); 
#endif
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x10);

          status |= SemcoCtrl_ProgramRegisters(modeSetPtr->I2cSlaveAddr, Pal_RfLutSwpLIF);
        }
        else
        {
          // High power IF dependent settings
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xD9);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x16);

          status |= SemcoCtrl_ProgramRegisters(modeSetPtr->I2cSlaveAddr, PalD_RfLutSwpHIF);

        }
        break;

      // Application modes settings for SECAM-I mode    
      case ANA_SECAM_I:
        status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, 0x0C, 0x0B);

        tmpRegTable = AnalogSecam;
        status |= SemcoCtrl_ProgramRegisters(modeSetPtr->I2cSlaveAddr, tmpRegTable);

        if (modeSetPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ)
        {
          // Low power IF dependent settings
#ifdef CUSTOMER_SPECIFIC_SETTING_1
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xFE);
#else
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0x7E); 
#endif
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x10);
        }
        else
        {
          // High power IF dependent settings
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xD9);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x16);
        }
        break;

      // Application modes settings for SECAM-L mode    
      case ANA_SECAM_L:
        status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, 0x0C, 0x0C);

        tmpRegTable = AnalogSecam;
        status |= SemcoCtrl_ProgramRegisters(modeSetPtr->I2cSlaveAddr, tmpRegTable);

        if (modeSetPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ)
        {
          // Low power IF dependent settings
#ifdef CUSTOMER_SPECIFIC_SETTING_1
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xFE);
#else
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0x7E); 
#endif
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x10);
        }
        else
        {
          // High power IF dependent settings
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xD9);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x16);
        }
        break;

      // Application modes settings for DVB-C/J.83B modes    
      case DIG_DVB_C:
      case DIG_J83B:
        tmpRegTable = DigitalDvbc;
        status |= SemcoCtrl_ProgramRegisters(modeSetPtr->I2cSlaveAddr, tmpRegTable);

        if (modeSetPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ)
        {
          // Low power IF dependent settings
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xFE);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x10);
        }
        else
        {
          // High power IF dependent settings
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xD9);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x16);
        }

        if (modeSetPtr->XtalFreqSel == XTAL_16MHz) dfeRegData = 0x0D;
        else dfeRegData = 0x0E;
        
        status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DFE_CSF_SS_SEL, dfeRegData);

        break;

      // Application modes settings for ISDB-T & ATSC modes    
      case DIG_ISDBT_ATSC:
        tmpRegTable = DigitalIsdbtAtsc;
        status |= SemcoCtrl_ProgramRegisters(modeSetPtr->I2cSlaveAddr, tmpRegTable);

        if (modeSetPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ)
        {
          // Low power IF dependent settings
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xF9);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x18);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_PWR, 0xF1);
        }
        else
        {
          // High power IF dependent settings
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xD9);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x16);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_PWR, 0xB1);
        }

        if (XTAL_16MHz == modeSetPtr->XtalFreqSel) dfeRegData = 0x0D;
        else if (XTAL_24MHz == modeSetPtr->XtalFreqSel) dfeRegData = 0x0E;
        
        status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DFE_CSF_SS_SEL, dfeRegData);

        dfeRegData = 0;

        // IF gain dependent settings
        switch(modeSetPtr->IFOutGainLevel)
        {
          case 0x09: dfeRegData = 0x44; break;
          case 0x08: dfeRegData = 0x43; break;
          case 0x07: dfeRegData = 0x42; break;
          case 0x06: dfeRegData = 0x41; break;
          case 0x05: dfeRegData = 0x40; break;
          default: break;
        }
        status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DFE_DACIF_GAIN, dfeRegData);
       
        break;

      // Application modes settings for DVB-T mode
      case DIG_DVB_T:
        tmpRegTable = DigitalDvbt;
        status |= SemcoCtrl_ProgramRegisters(modeSetPtr->I2cSlaveAddr, tmpRegTable);

        if (modeSetPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ)
        {
          // Low power IF dependent settings
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xFE);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x18);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_PWR, 0xF1);
        }
        else
        {
          // High power IF dependent settings
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_0, 0xD9);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_CFG_1, 0x16);
          status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DIG_ANA_IF_PWR, 0xB1);
        }
        
        if (XTAL_16MHz == modeSetPtr->XtalFreqSel) dfeRegData = 0x0D;
        else if (XTAL_24MHz == modeSetPtr->XtalFreqSel) dfeRegData = 0x0E;
        
        status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DFE_CSF_SS_SEL, dfeRegData);

        dfeRegData = 0;
        // IF gain dependent settings
        switch(modeSetPtr->IFOutGainLevel)
        {
          case 0x09: dfeRegData = 0x44; break;
          case 0x08: dfeRegData = 0x43; break;
          case 0x07: dfeRegData = 0x42; break;
          case 0x06: dfeRegData = 0x41; break;
          case 0x05: dfeRegData = 0x40; break;
          default: break;
        }

        status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_DFE_DACIF_GAIN, dfeRegData);
        break;
      
      default:
        status |= MXL_FALSE;
        break;
    }

  }

  if (MXL_FALSE != status)
  {
    // XTAL calibration
    status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_XTAL_CALI_SET_REG, 0x00);   
    status |= SemcoCtrl_WriteRegister(modeSetPtr->I2cSlaveAddr, SEM_XTAL_CALI_SET_REG, 0x01);   
  
    // 50 ms sleep after XTAL calibration
    SemcoMxL_Sleep(50);
  }
  
  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_ConfigTunerAgcSet - MXL_TUNER_AGC_CFG
--| 
--| AUTHOR        : Dong Liu
--|                 Sunghoon Park
--|
--| DATE CREATED  : 06/21/2011
--|                 04/21/2011
--|                 11/13/2011 (MK) - Change bit address for AGC Type function.
--|
--| DESCRIPTION   : This function is used to configure AGC settings of MxL601
--|                 tuner device.
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_ConfigTunerAgcSet(PSEM_MXL_AGC_SET_CFG_T agcSetPtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 regData = 0; 

  MxL_DLL_DEBUG0("MXL_TUNER_AGC_CFG, AGC mode = %d, sel = %d, set point = %d", 
    agcSetPtr->AgcSel, agcSetPtr->AgcType, agcSetPtr->SetPoint);

  // AGC selecton <3:2> and mode setting <0>
  status |= SemcoCtrl_ReadRegister(agcSetPtr->I2cSlaveAddr, SEM_AGC_CONFIG_REG, &regData); 
  regData &= 0xF2; // Clear bits <3:2> & <0>
  regData = (UINT8) (regData | (agcSetPtr->AgcType << 2) | agcSetPtr->AgcSel);
  status |= SemcoCtrl_WriteRegister(agcSetPtr->I2cSlaveAddr, SEM_AGC_CONFIG_REG, regData);

  // AGC set point <6:0>
  status |= SemcoCtrl_ReadRegister(agcSetPtr->I2cSlaveAddr, SEM_AGC_SET_POINT_REG, &regData);
  regData &= 0x80; // Clear bit <6:0>
  regData |= agcSetPtr->SetPoint;
  status |= SemcoCtrl_WriteRegister(agcSetPtr->I2cSlaveAddr, SEM_AGC_SET_POINT_REG, regData);

  // AGC Polarity <4>
  status |= SemcoCtrl_ReadRegister(agcSetPtr->I2cSlaveAddr, SEM_AGC_FLIP_REG, &regData);
  regData &= 0xEF; // Clear bit <4>
  regData |= (agcSetPtr->AgcPolarityInverstion << 4);
  status |= SemcoCtrl_WriteRegister(agcSetPtr->I2cSlaveAddr, SEM_AGC_FLIP_REG, regData);

  return(MXL_STATUS) status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_ConfigTunerFineTune - MXL_TUNER_FINE_TUNE_CFG
--| 
--| AUTHOR        : Dong Liu
--|                 Sunghoon Park
--|
--| DATE CREATED  : 03/28/2011
--|                 04/22/2011
--|
--| DESCRIPTION   : This function is used to control fine tune step (freq offset)  
--|                 when MxL601 is used in Analog mode.
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_ConfigTunerFineTune(PSEM_MXL_FINE_TUNE_CFG_T fineTunePtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 regData = 0;

  MxL_DLL_DEBUG0("MXL_TUNER_FINE_TUNE_CFG, fine tune = %d", fineTunePtr->ScaleUp);

  status |= SemcoCtrl_ReadRegister(fineTunePtr->I2cSlaveAddr, SEM_FINE_TUNE_SET_REG, &regData);

  // Fine tune <1:0>
  regData &= 0xFC;
  status |= SemcoCtrl_WriteRegister(fineTunePtr->I2cSlaveAddr, SEM_FINE_TUNE_SET_REG, regData);

  if (MXL_FINE_TUNE_STEP_UP == fineTunePtr->ScaleUp)
    regData |= SEM_FINE_TUNE_FREQ_INCREASE;
  else 
    regData |= SEM_FINE_TUNE_FREQ_DECREASE;

  status |= SemcoCtrl_WriteRegister(fineTunePtr->I2cSlaveAddr, SEM_FINE_TUNE_SET_REG, regData); 

  return(MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_ConfigTunerEnableFineTune - 
--|                                           MXL_TUNER_ENABLE_FINE_TUNE_CFG
--| 
--| AUTHOR        : Sunghoon Park
--|                 Dong Liu 
--|
--| DATE CREATED  : 04/22/2011
--|                 07/01/2011  
--|
--| DESCRIPTION   : This fucntion is used to enable or disable fine tune function
--|                 when MxL601 device is used in Analog mode.
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_ConfigTunerEnableFineTune(PSEM_MXL_ENABLE_FINE_TUNE_CFG_T fineTunePtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 fineTuneReg1 = 0;
  UINT8 fineTuneReg2 = 0;
  
  MxL_DLL_DEBUG0("MXL_TUNER_ENABLE_FINE_TUNE_CFG, fine tune = %d",
                                                    fineTunePtr->EnableFineTune);

  status |= SemcoCtrl_ReadRegister(fineTunePtr->I2cSlaveAddr, SEM_FINE_TUNE_CTRL_REG_0, &fineTuneReg1);
  status |= SemcoCtrl_ReadRegister(fineTunePtr->I2cSlaveAddr, SEM_FINE_TUNE_CTRL_REG_1, &fineTuneReg2);
  
  if (MXL_ENABLE == fineTunePtr->EnableFineTune) 
  {
    fineTuneReg1 |= 0x04;
    fineTuneReg2 |= 0x20;
  }
  else
  {
    fineTuneReg1 &= 0xF8;
    fineTuneReg2 &= 0xDF;
  }

  status |= SemcoCtrl_WriteRegister(fineTunePtr->I2cSlaveAddr, SEM_FINE_TUNE_CTRL_REG_0, fineTuneReg1);    
  status |= SemcoCtrl_WriteRegister(fineTunePtr->I2cSlaveAddr, SEM_FINE_TUNE_CTRL_REG_1, fineTuneReg2);    
  
  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_ConfigTunerChanTune - MXL_TUNER_CHAN_TUNE_CFG
--| 
--| AUTHOR        : Sunghoon Park 
--|                 Mahendra Kondur
--|                 Dong Liu
--|
--| DATE CREATED  : 3/17/2011
--|                 9/23/2011 - NTSC HRC & IRC modes
--|                 11/16/2011
--|
--| DESCRIPTION   : This API configures RF channel frequency and bandwidth. 
--|                 Radio Frequency unit is Hz, and Bandwidth is in MHz units.
--|                 For analog broadcast standards, corresponding spur shifitng 
--|                 settiings will be aplied.
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_ConfigTunerChanTune(PSEM_MXL_TUNER_TUNE_CFG_T tuneParamPtr)
{
  UINT64 frequency;
  UINT32 freq = 0;
  UINT8 status = MXL_TRUE;
  UINT8 regData = 0;
  SEM_MXL_ENABLE_FINE_TUNE_CFG_T fineTuneCfg;
  SEM_CHAN_DEPENDENT_FREQ_TABLE_T *freqLutPtr = NULL;

  MxL_DLL_DEBUG0("MXL_TUNER_CHAN_TUNE_CFG, signal type = %d (%d), Freq = %d, BW = %d, Xtal = %d \n",  
                                              tuneParamPtr->SignalMode, 
                                              tuneParamPtr->NtscCarrier, 
                                              tuneParamPtr->FreqInHz, 
                                              tuneParamPtr->BandWidth, 
                                              tuneParamPtr->XtalFreqSel);

  // RF Frequency VCO Band Settings 
  regData = 0;
  if (tuneParamPtr->FreqInHz < 700000000) 
  {
    status |= SemcoCtrl_WriteRegister(tuneParamPtr->I2cSlaveAddr, 0x7C, 0x1F);
    if ((tuneParamPtr->SignalMode == DIG_DVB_C) || (tuneParamPtr->SignalMode == DIG_J83B)) 
      regData = 0xC1;
    else
      regData = 0x81;
    
  }
  else 
  {
    status |= SemcoCtrl_WriteRegister(tuneParamPtr->I2cSlaveAddr, 0x7C, 0x9F);

    if ((tuneParamPtr->SignalMode == DIG_DVB_C) || (tuneParamPtr->SignalMode == DIG_J83B)) 
      regData = 0xD1;
    else
      regData = 0x91;
    
  }

  status |= SemcoCtrl_WriteRegister(tuneParamPtr->I2cSlaveAddr, 0x00, 0x01);
  status |= SemcoCtrl_WriteRegister(tuneParamPtr->I2cSlaveAddr, 0x31, regData);
  status |= SemcoCtrl_WriteRegister(tuneParamPtr->I2cSlaveAddr, 0x00, 0x00);
  
  switch(tuneParamPtr->SignalMode)
  {
    // Signal type is NTSC mode
    case ANA_NTSC_MODE:

      // If 16MHz XTAL is used then need to program registers based on frequency value
      if (XTAL_16MHz == tuneParamPtr->XtalFreqSel)
      {
        if (MXL_NTSC_CARRIER_NA == tuneParamPtr->NtscCarrier)
        {
          // Select spur shifting LUT to be used based NTSC, HRC or IRC frequency
          if ((tuneParamPtr->FreqInHz % 1000000) == 0) 
          {
            // Apply spur shifting LUT based on HIF or LIF setting
            if (tuneParamPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ) freqLutPtr = NTSC_FREQ_LUT_XTAL_16MHZ_LIF;
            else freqLutPtr = NTSC_FREQ_LUT_XTAL_16MHZ_HIF;

          }
          else if ((tuneParamPtr->FreqInHz % 12500) == 0)
          {
            freqLutPtr = NTSC_FREQ_LUT_IRC_16MHZ;
          }
          else 
          {
            freqLutPtr = NTSC_FREQ_LUT_HRC_16MHZ;
            status |= SemcoCtrl_ProgramRegisters(tuneParamPtr->I2cSlaveAddr, Ntsc_HRCRfLutSwpLIF);
          }
         
        }
        else
        {
          // Select LUT based on NTSC carriers
          if (MXL_NTSC_CARRIER_HRC == tuneParamPtr->NtscCarrier) 
          {
            freqLutPtr = NTSC_FREQ_LUT_HRC_16MHZ;
            status |= SemcoCtrl_ProgramRegisters(tuneParamPtr->I2cSlaveAddr, Ntsc_HRCRfLutSwpLIF);
          }
          else
          {
            freqLutPtr = NTSC_FREQ_LUT_IRC_16MHZ;
          }        

        }

      } // end of if (16 MHz XTAL
      else if (XTAL_24MHz == tuneParamPtr->XtalFreqSel)
      {
        if (MXL_NTSC_CARRIER_NA == tuneParamPtr->NtscCarrier)
        {
          // Select spur shifting LUT to be used based NTSC, HRC or IRC frequency
          if ((tuneParamPtr->FreqInHz % 1000000) == 0) 
          {
            freqLutPtr = NTSC_FREQ_LUT_XTAL_24MHZ;
          }
          else if ((tuneParamPtr->FreqInHz % 12500) == 0) 
          {
            freqLutPtr = NTSC_FREQ_LUT_IRC_24MHZ;
          }
          else 
          {
            freqLutPtr = NTSC_FREQ_LUT_HRC_24MHZ;
            status |= SemcoCtrl_ProgramRegisters(tuneParamPtr->I2cSlaveAddr, Ntsc_HRCRfLutSwpLIF);
          }
         
        }
        else
        {
           // Select LUT based on NTSC carriers
          if (MXL_NTSC_CARRIER_HRC == tuneParamPtr->NtscCarrier) 
          {
            freqLutPtr = NTSC_FREQ_LUT_HRC_24MHZ;
            status |= SemcoCtrl_ProgramRegisters(tuneParamPtr->I2cSlaveAddr, Ntsc_HRCRfLutSwpLIF);
          }
          else 
          {
            freqLutPtr = NTSC_FREQ_LUT_IRC_24MHZ;
          }

        }

      } // end of if (24 MHz XTAL 

      if (freqLutPtr)
        status |= SemcoCtrl_SetRfFreqLutReg(tuneParamPtr->I2cSlaveAddr, tuneParamPtr->FreqInHz, freqLutPtr);

      break;

    // Signal type is PAL D
    case ANA_PAL_D:     

      // If 16MHz XTAL is used then need to program registers based on frequency value
      if (XTAL_16MHz == tuneParamPtr->XtalFreqSel)
      {
        if (tuneParamPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ) freqLutPtr = PAL_D_LUT_XTAL_16MHZ_LIF;
        else freqLutPtr = PAL_D_LUT_XTAL_16MHZ_HIF;

      } // end of if (16 MHz XTAL
      else if (XTAL_24MHz == tuneParamPtr->XtalFreqSel)
      {
        freqLutPtr = PAL_D_LUT_XTAL_24MHZ;
      } // end of if (24 MHz XTAL 

      if (freqLutPtr)
        status |= SemcoCtrl_SetRfFreqLutReg(tuneParamPtr->I2cSlaveAddr, tuneParamPtr->FreqInHz, freqLutPtr);

      break;

    case ANA_PAL_BG:

      switch(tuneParamPtr->BandWidth)
      {
        case ANA_TV_DIG_CABLE_BW_7MHz:
          regData = 0x09;
          break;

        case ANA_TV_DIG_CABLE_BW_8MHz:
          regData = 0x0A;
          break;
        
        case ANA_TV_DIG_CABLE_BW_6MHz:
        case DIG_TERR_BW_6MHz:
        case DIG_TERR_BW_7MHz:
        case DIG_TERR_BW_8MHz:
        default: 
          break;
      }

      status |= SemcoCtrl_WriteRegister(tuneParamPtr->I2cSlaveAddr, 0x0C, regData);

      // If 16MHz XTAL is used then need to program registers based on frequency value
      if (XTAL_16MHz == tuneParamPtr->XtalFreqSel)
      {
        // PAL - BG 7 MHz Frequency range
        if (ANA_TV_DIG_CABLE_BW_7MHz == tuneParamPtr->BandWidth) 
        {
          if (tuneParamPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ) 
          {
            freqLutPtr = PAL_BG_7MHZ_LUT_XTAL_16MHZ_LIF;
            status |= SemcoCtrl_ProgramRegisters(tuneParamPtr->I2cSlaveAddr, Pal_RfLutSwpLIF);
          }
          else
          {
            freqLutPtr = PAL_BG_7MHZ_LUT_XTAL_16MHZ_HIF;
            status |= SemcoCtrl_ProgramRegisters(tuneParamPtr->I2cSlaveAddr, PalBG_7MHzBW_RfLutSwpHIF);
          }

        }
        else
        {
          if (tuneParamPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ) 
          {
            freqLutPtr = PAL_BG_8MHZ_LUT_XTAL_16MHZ_LIF;
            status |= SemcoCtrl_ProgramRegisters(tuneParamPtr->I2cSlaveAddr, Pal_RfLutSwpLIF);
          }
          else
          {
            freqLutPtr = PAL_BG_8MHZ_LUT_XTAL_16MHZ_HIF;
            status |= SemcoCtrl_ProgramRegisters(tuneParamPtr->I2cSlaveAddr, PalBG_8MHzBW_RfLutSwpHIF);
          }
        }
        
      } // end of if (XTAL_16MHz     
      else if (XTAL_24MHz == tuneParamPtr->XtalFreqSel)
      { 
        // If 16MHz XTAL is used then need to program registers based on frequency value
        // PAL - BG 7 MHz Frequency range
        if (ANA_TV_DIG_CABLE_BW_7MHz == tuneParamPtr->BandWidth) 
        {
          freqLutPtr = PAL_BG_7MHZ_LUT_XTAL_24MHZ;

          if (tuneParamPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ) 
            status |= SemcoCtrl_ProgramRegisters(tuneParamPtr->I2cSlaveAddr, Pal_RfLutSwpLIF);
          else
            status |= SemcoCtrl_ProgramRegisters(tuneParamPtr->I2cSlaveAddr, PalBG_7MHzBW_RfLutSwpHIF);

        }
        else
        {
          freqLutPtr = PAL_BG_8MHZ_LUT_XTAL_24MHZ;   

          if (tuneParamPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ) 
            status |= SemcoCtrl_ProgramRegisters(tuneParamPtr->I2cSlaveAddr, Pal_RfLutSwpLIF);
          else
            status |= SemcoCtrl_ProgramRegisters(tuneParamPtr->I2cSlaveAddr, PalBG_8MHzBW_RfLutSwpHIF);
          
        }

      } // end of if (XTAL_24MHz

      if (freqLutPtr)
        status |= SemcoCtrl_SetRfFreqLutReg(tuneParamPtr->I2cSlaveAddr, tuneParamPtr->FreqInHz, freqLutPtr);
      
      break;

    case ANA_PAL_I:      

      // If 16MHz XTAL is used then need to program registers based on frequency value
      if (XTAL_16MHz == tuneParamPtr->XtalFreqSel)
      {
        if (tuneParamPtr->IFOutFreqinKHz < SEM_HIGH_IF_35250_KHZ) freqLutPtr = PAL_I_LUT_XTAL_16MHZ_LIF;
        else freqLutPtr = PAL_I_LUT_XTAL_16MHZ_HIF;
      } 
      else
      {
        freqLutPtr = PAL_I_LUT_XTAL_24MHZ;  
      }

      if (freqLutPtr)
        status |= SemcoCtrl_SetRfFreqLutReg(tuneParamPtr->I2cSlaveAddr, tuneParamPtr->FreqInHz, freqLutPtr);

      break;

    case ANA_SECAM_L:      

      // If 16MHz XTAL is used then need to program registers based on frequency value
      if (XTAL_16MHz == tuneParamPtr->XtalFreqSel)
      {
        freqLutPtr = SECAM_L_LUT_XTAL_16MHZ;
      } // end of if (16 MHz XTAL   
      else
      {
        freqLutPtr = SECAM_L_LUT_XTAL_24MHZ;
      }

      if (freqLutPtr)
        status |= SemcoCtrl_SetRfFreqLutReg(tuneParamPtr->I2cSlaveAddr, tuneParamPtr->FreqInHz, freqLutPtr);
      break;

    case DIG_DVB_C:
    case DIG_J83B:
      if ((tuneParamPtr->BandWidth == DIG_TERR_BW_6MHz) || (tuneParamPtr->BandWidth == DIG_TERR_BW_8MHz))
      {
        if (tuneParamPtr->BandWidth == DIG_TERR_BW_6MHz) freqLutPtr = DIG_CABLE_FREQ_LUT_BW_6MHZ;
        else freqLutPtr = DIG_CABLE_FREQ_LUT_BW_8MHZ;

        status |= SemcoCtrl_SetRfFreqLutReg(tuneParamPtr->I2cSlaveAddr, tuneParamPtr->FreqInHz, freqLutPtr);
      }
      break;
    case DIG_ISDBT_ATSC:
    case DIG_DVB_T:
      if (tuneParamPtr->BandWidth == DIG_TERR_BW_6MHz) freqLutPtr = DIG_TERR_FREQ_LUT_BW_6MHZ;
      else if (tuneParamPtr->BandWidth == DIG_TERR_BW_7MHz) freqLutPtr = DIG_TERR_FREQ_LUT_BW_7MHZ;
      else freqLutPtr = DIG_TERR_FREQ_LUT_BW_8MHZ;

      status |= SemcoCtrl_SetRfFreqLutReg(tuneParamPtr->I2cSlaveAddr, tuneParamPtr->FreqInHz, freqLutPtr);
      break;
    case MXL_SIGNAL_NA:
    default: 
      break;
  }

  // Tune mode <0>
  status |= SemcoCtrl_ReadRegister(tuneParamPtr->I2cSlaveAddr, SEM_TUNE_MODE_REG, &regData);
  regData &= 0xFE;

  if (SCAN_MODE == tuneParamPtr->TuneType) regData |= 0x01;

  status |= SemcoCtrl_WriteRegister(tuneParamPtr->I2cSlaveAddr, SEM_TUNE_MODE_REG, regData);

  // Bandwidth <7:0>
  status |= SemcoCtrl_WriteRegister(tuneParamPtr->I2cSlaveAddr, SEM_CHAN_TUNE_BW_REG, (UINT8)tuneParamPtr->BandWidth);

  // Frequency
  frequency = tuneParamPtr->FreqInHz;

  /* Calculate RF Channel = DIV(64*RF(Hz), 1E6) */
  frequency *= 64;
  freq = (UINT32)(frequency / 1000000); 

  // Set RF  
  status |= SemcoCtrl_WriteRegister(tuneParamPtr->I2cSlaveAddr, SEM_CHAN_TUNE_LOW_REG, (UINT8)(freq & 0xFF));
  status |= SemcoCtrl_WriteRegister(tuneParamPtr->I2cSlaveAddr, SEM_CHAN_TUNE_HI_REG, (UINT8)((freq >> 8 ) & 0xFF));

  if (tuneParamPtr->SignalMode <= ANA_SECAM_L)
  {
    // Disable Fine tune for Analog mode channels
    fineTuneCfg.I2cSlaveAddr = tuneParamPtr->I2cSlaveAddr;
    fineTuneCfg.EnableFineTune = MXL_DISABLE;
    status |= SemcoMxL601_ConfigTunerEnableFineTune(&fineTuneCfg);
    
    regData = 0;
    status |= SemcoCtrl_ReadRegister(tuneParamPtr->I2cSlaveAddr, SEM_FINE_TUNE_CTRL_REG_1, &regData);
    regData |= 0x20;
    status |= SemcoCtrl_WriteRegister(tuneParamPtr->I2cSlaveAddr, SEM_FINE_TUNE_CTRL_REG_1, regData);    

  }
      
  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_ConfigSpurShiftAdjustment - 
--|                                                MXL_TUNER_SPUR_SHIFT_ADJ_CFG
--| 
--| AUTHOR        : Mahee
--|
--| DATE CREATED  : 11/22/2011
--|                 12/20/2011
--|
--| DESCRIPTION   : This API configures MxL601 to move spurs caused by digital clock
--|                 out of band by adjusting digital clock frequency. The clock 
--|                 frequency  adjustment value should be from 205 & 227.
--|                 Apply sequencer settings after updating registers.
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_ConfigSpurShiftAdjustment(PSEM_MXL_SPUR_SHIFT_ADJ_CFG_T spurShiftAdjPtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 regData = 0;

  MxL_DLL_DEBUG0("MXL_TUNER_SPUR_SHIFT_ADJ_CFG, Clock freq adjustment value = %d\n", 
                                            spurShiftAdjPtr->SpurShiftingClkAdjValue);

  /* Check for spur shift clock adjustment value. 
     The value should be greater than 205 & less than 227 */
  if ((spurShiftAdjPtr->SpurShiftingClkAdjValue >= SEM_SPUR_SHIFT_CLOCK_ADJUST_MIN) && 
      (spurShiftAdjPtr->SpurShiftingClkAdjValue <= SEM_SPUR_SHIFT_CLOCK_ADJUST_MAX))
  {
    status |= SemcoCtrl_ReadRegister(spurShiftAdjPtr->I2cSlaveAddr, 0xEA, &regData);
    regData |= 0x20;
    status |= SemcoCtrl_WriteRegister(spurShiftAdjPtr->I2cSlaveAddr, 0xEA, regData);   

    status |= SemcoCtrl_WriteRegister(spurShiftAdjPtr->I2cSlaveAddr, 
                                 0xEB, 
                                 spurShiftAdjPtr->SpurShiftingClkAdjValue);   

    status |= SemcoCtrl_WriteRegister(spurShiftAdjPtr->I2cSlaveAddr, SEM_START_TUNE_REG, 0x00); 
    status |= SemcoCtrl_WriteRegister(spurShiftAdjPtr->I2cSlaveAddr, SEM_START_TUNE_REG, 0x01); 

  }
  else status = MXL_FALSE;
  
  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_ConfigHlfrfSettings - 
--|                                     MXL_TUNER_HLFRF_CFG
--| 
--| AUTHOR        : Mahee
--|
--| DATE CREATED  : 12/28/2011
--|
--| DESCRIPTION   : This API configures register settings to improve hlfrf
--|                 performance.
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_ConfigHlfrfSettings(PSEM_MXL_HLFRF_CFG_T hlfrfCfgPtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 regData0 = 0;
  UINT8 regData1 = 0;

  MxL_DLL_DEBUG0("MXL_TUNER_HLFRF_CFG\n");

  status |= SemcoCtrl_WriteRegister(hlfrfCfgPtr->I2cSlaveAddr, 0x00, 0x01);
  status |= SemcoCtrl_ReadRegister(hlfrfCfgPtr->I2cSlaveAddr, 0x96, &regData0);

  if (hlfrfCfgPtr->Enable == MXL_ENABLE) 
  {
    regData0 |= 0x10; 
    regData1 = 0x48;
  }
  else 
  {
    regData0 &= 0xEF;
    regData1 = 0x59;
  }

  status |= SemcoCtrl_WriteRegister(hlfrfCfgPtr->I2cSlaveAddr, 0x96, regData0);
  status |= SemcoCtrl_WriteRegister(hlfrfCfgPtr->I2cSlaveAddr, 0x00, 0x00);
  
  status |= SemcoCtrl_WriteRegister(hlfrfCfgPtr->I2cSlaveAddr, 0xA9, regData1);

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_GetTunerSignalType - MXL_TUNER_SIGNAL_TYPE_REQ
--|
--| AUTHOR        : Sunghoon Park
--|
--| DATE CREATED  : 6/24/2011
--|
--| DESCRIPTION   : This function returns signal type received.
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_GetTunerSignalType(PSEM_MXL_SIGNAL_TYPE_T tuneSigTypePtr)
{
  MXL_STATUS status = MXL_TRUE;
  UINT8 regData = 0;

  status = SemcoCtrl_ReadRegister(tuneSigTypePtr->I2cSlaveAddr, SEM_SIGNAL_TYPE_REG, &regData);

  // Signal type 
  if ((regData & 0x10) == 0x10) tuneSigTypePtr->SignalMode = SIGNAL_TYPE_ANALOG;
  else tuneSigTypePtr->SignalMode = SIGNAL_TYPE_DIGITAL;

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_GetTunerLockStatus - MXL_TUNER_LOCK_STATUS_REQ
--|
--| AUTHOR        : Dong Liu
--|                 Sunghoon Park
--|
--| DATE CREATED  : 6/21/2011
--|                 6/24/2011
--|
--| DESCRIPTION   : This function returns Tuner Lock status of MxL601 tuner.
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_GetTunerLockStatus(PSEM_MXL_TUNER_LOCK_STATUS_T tunerLockStatusPtr)
{
  MXL_STATUS status = MXL_TRUE;
  UINT8 regData = 0;
  MXL_BOOL rfSynthStatus = MXL_UNLOCKED;
  MXL_BOOL refSynthStatus = MXL_UNLOCKED;

  MxL_DLL_DEBUG0("MXL_TUNER_LOCK_STATUS_REQ \n");

  status = SemcoCtrl_ReadRegister(tunerLockStatusPtr->I2cSlaveAddr, SEM_RF_REF_STATUS_REG, &regData);  

  if ((regData & 0x01) == 0x01) refSynthStatus = MXL_LOCKED;

  if ((regData & 0x02) == 0x02) rfSynthStatus = MXL_LOCKED;

  MxL_DLL_DEBUG0(" RfSynthStatus = %d, RefSynthStatus = %d", 
                                    (UINT8)rfSynthStatus,(UINT8)refSynthStatus); 

  tunerLockStatusPtr->RfSynLock =  rfSynthStatus;
  tunerLockStatusPtr->RefSynLock = refSynthStatus;

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_GetTunerAgcLockStatus - MXL_TUNER_AGC_LOCK_REQ
--|
--| AUTHOR        : Dong Liu
--|
--| DATE CREATED  : 10/12/2011
--|
--| DESCRIPTION   : This function returns AGC Lock status of MxL601 tuner.
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_GetTunerAgcLockStatus(PSEM_MXL_TUNER_AGC_LOCK_T agcLockStatusPtr)
{
  MXL_STATUS status = MXL_FALSE;
  UINT8 regData = 0;
  MXL_BOOL lockStatus = MXL_UNLOCKED;

  MxL_DLL_DEBUG0("MXL_TUNER_AGC_LOCK_REQ \n");

  status = SemcoCtrl_ReadRegister(agcLockStatusPtr->I2cSlaveAddr, SEM_AGC_SAGCLOCK_STATUS_REG, &regData);  

  // Bit<3>
  if ((regData & 0x08) == 0x08) lockStatus = MXL_LOCKED;
  
  MxL_DLL_DEBUG0(" Agc lock = %d", (UINT8)lockStatus); 

  agcLockStatusPtr->AgcLock =  lockStatus;

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_GetTunerRxPower - MXL_TUNER_RX_PWR_REQ
--|
--| AUTHOR        : Dong Liu
--|                 Sunghoon Park
--|
--| DATE CREATED  : 03/24/2010
--|                 06/17/2011
--|
--| DESCRIPTION   : This function returns RF input power in dBm.
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_GetTunerRxPower(PSEM_MXL_TUNER_RX_PWR_T rxPwrPtr)
{
  UINT8 status = MXL_TRUE;
  UINT8 regData = 0;
  UINT16 tmpData = 0;

  MxL_DLL_DEBUG0("MXL_TUNER_RX_PWR_REQ \n");

  // RF input power low <7:0>
  status = SemcoCtrl_ReadRegister(rxPwrPtr->I2cSlaveAddr, SEM_RFPIN_RB_LOW_REG, &regData);
  tmpData = regData;

  // RF input power high <1:0>
  status |= SemcoCtrl_ReadRegister(rxPwrPtr->I2cSlaveAddr, SEM_RFPIN_RB_HIGH_REG, &regData);
  tmpData |= (regData & 0x03) << 8;

  // Fractional last 2 bits
  rxPwrPtr->RxPwr = (REAL32)((tmpData & 0x01FF) >> 2);

  if (tmpData & 0x02) rxPwrPtr->RxPwr += 0.5;
  if (tmpData & 0x01) rxPwrPtr->RxPwr += 0.25;
  if (tmpData & 0x0200) rxPwrPtr->RxPwr -= 128;
    
  MxL_DLL_DEBUG0(" Rx power = %f dBm \n", rxPwrPtr->RxPwr);

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxL601_GetTunerAfcCorrection - MXL_TUNER_AFC_CORRECTION_REQ
--|
--| AUTHOR        : Dong Liu
--|
--| DATE CREATED  : 11/15/2011
--|
--| DESCRIPTION   : This function returns AFC offset value, unit is kHz  
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

static MXL_STATUS SemcoMxL601_GetTunerAfcCorrection(PSEM_MXL_TUNER_AFC_CORRECTION_T afcCorrectionPtr)
{
  UINT32 tmpData = 0;
  UINT8 status = MXL_TRUE;
  UINT8 regData = 0;
  UINT8 regData2 = 0;
  UINT8 regData3 = 0;
  UINT8 afcLock = 0;

  MxL_DLL_DEBUG0("MXL_TUNER_AFC_CORRECTION_REQ \n");

  status |= SemcoCtrl_ReadRegister(afcCorrectionPtr->I2cSlaveAddr, SEM_RFPIN_RB_HIGH_REG, &regData);
  afcLock = (regData & 0x10) >> 4; 

  // AFC offset correction readback <7:0> correponding to bit<7:0>
  status |= SemcoCtrl_ReadRegister(afcCorrectionPtr->I2cSlaveAddr, SEM_DFE_CTRL_ACCUM_LOW_REG, &regData);

  // AFC offset correction readback <15:8> correponding to bit<7:0>
  status |= SemcoCtrl_ReadRegister(afcCorrectionPtr->I2cSlaveAddr, SEM_DFE_CTRL_ACCUM_MID_REG, &regData2);

  // AFC offset correction readback <17:16> correponding to bit<1:0>
  status |= SemcoCtrl_ReadRegister(afcCorrectionPtr->I2cSlaveAddr, SEM_DFE_CTRL_ACCUM_HI_REG, &regData3);
  
  tmpData = regData3 & 0x03;
  tmpData = (tmpData << 8) + regData2;
  tmpData = (tmpData << 8) + regData;

  if ((afcLock == 0) && (tmpData == 0))
  {
    // MxL601 AFC could not find peak
	  status = MXL_FALSE;  
    MxL_DLL_DEBUG0(" Can not find peak \n");
  }
  else
  {
    // Check if return data is negative number 
    if (tmpData & 0x020000)  // Bit<17> = 1 
      afcCorrectionPtr->AfcOffsetKHz = ((REAL64)tmpData - 262144) / 65536.0 * 1000.0;  
    else 
      afcCorrectionPtr->AfcOffsetKHz = (REAL64)tmpData / 65536.0 * 1000.0;

    MxL_DLL_DEBUG0(" AFC correction = %f kHz \n", afcCorrectionPtr->AfcOffsetKHz);
  }

  return (MXL_STATUS)status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxLWare601_API_ConfigDevice 
--| 
--| AUTHOR        : Dong Liu
--|
--| DATE CREATED  : 02/12/2011 
--|
--| DESCRIPTION   : The general device configuration shall be handled 
--|                 through this API. 
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS SemcoMxLWare601_API_ConfigDevice(SEM_MXL_COMMAND_T *ParamPtr)
{
  MXL_STATUS status = MXL_TRUE;
  SEM_MXL_CMD_TYPE_E cmdType;

  if (ParamPtr == NULL) return MXL_FALSE;

  cmdType = ParamPtr->commandId;

  MxL_DLL_DEBUG0("MxLWare601_API_ConfigDevice : cmdType = %d", cmdType);

  switch (cmdType)
  {
    case MXL_DEV_SOFT_RESET_CFG:
      status = SemcoMxL601_ConfigDevReset(&ParamPtr->MxLIf.cmdResetCfg);
      break;

    case MXL_DEV_OVERWRITE_DEFAULT_CFG:
      status = SemcoMxL601_ConfigDevOverwriteDefault(&ParamPtr->MxLIf.cmdOverwriteDefault);
      break;

    case MXL_DEV_XTAL_SET_CFG:
      status = SemcoMxL601_ConfigDevXtalSet(&ParamPtr->MxLIf.cmdXtalCfg);
      break;

    case MXL_DEV_POWER_MODE_CFG:
      status = SemcoMxL601_ConfigDevPowerMode(&ParamPtr->MxLIf.cmdPwrModeCfg);
      break;

    case MXL_DEV_IF_OUT_CFG:
      status = SemcoMxL601_ConfigDevIfOutSet(&ParamPtr->MxLIf.cmdIfOutCfg);
      break;

    case MXL_DEV_GPO_CFG: 
      status = SemcoMxL601_ConfigDevGPO(&ParamPtr->MxLIf.cmdGpoCfg);
      break;
 
    default:
      status = MXL_FALSE;
      break;
  }

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxLWare601_API_GetDeviceStatus 
--| 
--| AUTHOR        : Dong Liu
--|
--| DATE CREATED  : 01/30/2011
--|
--| DESCRIPTION   : The general device inquiries shall be handled 
--|                 through this API
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS SemcoMxLWare601_API_GetDeviceStatus(SEM_MXL_COMMAND_T *ParamPtr)
{
  MXL_STATUS status = MXL_TRUE;
  SEM_MXL_CMD_TYPE_E cmdType;

  if (ParamPtr == NULL) return MXL_FALSE;

  cmdType = ParamPtr->commandId;

  MxL_DLL_DEBUG0("MxLWare601_API_GetDeviceStatus : cmdType = %d", cmdType);

  switch (cmdType)
  {
    case MXL_DEV_ID_VERSION_REQ:
      status = SemcoMxL601_GetDeviceInfo(&ParamPtr->MxLIf.cmdDevInfoReq);
      break;

    case MXL_DEV_GPO_STATE_REQ:
      status = SemcoMxL601_GetDeviceGpoState(&ParamPtr->MxLIf.cmdGpoReq);
      break;

    default:
      status = MXL_FALSE;
      break;
  }

  return status;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MxLWare601_API_ConfigTuner 
--| 
--| AUTHOR        : Dong Liu
--|
--| DATE CREATED  : 02/12/2011 
--|                 12/28/2011
--|
--| DESCRIPTION   : The tuner block specific configuration shall be handled 
--|                 through this API
--|                 New API added - MXL_TUNER_HALF_RF_OPTIMIZATION_CFG
--|     
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS SemcoMxLWare601_API_ConfigTuner(SEM_MXL_COMMAND_T *ParamPtr)
{
  MXL_STATUS status = MXL_TRUE;
  SEM_MXL_CMD_TYPE_E cmdType;

  if (ParamPtr == NULL) return MXL_FALSE;

  cmdType = ParamPtr->commandId;

  MxL_DLL_DEBUG0("MxLWare601_API_ConfigTuner : cmdType =%d", cmdType);
  
  switch (cmdType)
  {
    case MXL_TUNER_POWER_UP_CFG:
      status = SemcoMxL601_ConfigTunerPowerUp(&ParamPtr->MxLIf.cmdTunerPoweUpCfg);
      break;

    case MXL_TUNER_START_TUNE_CFG:
      status = SemcoMxL601_ConfigTunerSequenceSet(&ParamPtr->MxLIf.cmdStartTuneCfg);
      break;      

    case MXL_TUNER_MODE_CFG:
      status = SemcoMxL601_ConfigTunerApplicationModeSet(&ParamPtr->MxLIf.cmdModeCfg);
      break;     

    case MXL_TUNER_AGC_CFG:
      status = SemcoMxL601_ConfigTunerAgcSet(&ParamPtr->MxLIf.cmdAgcSetCfg);
      break;

    case MXL_TUNER_FINE_TUNE_CFG:
      status = SemcoMxL601_ConfigTunerFineTune(&ParamPtr->MxLIf.cmdFineTuneCfg);
      break;

    case MXL_TUNER_ENABLE_FINE_TUNE_CFG:
      status = SemcoMxL601_ConfigTunerEnableFineTune(&ParamPtr->MxLIf.cmdEnableFineTuneCfg);
      break;

    case MXL_TUNER_CHAN_TUNE_CFG:
      status = SemcoMxL601_ConfigTunerChanTune(&ParamPtr->MxLIf.cmdChanTuneCfg);
      break;

    case MXL_TUNER_SPUR_SHIFT_ADJ_CFG:
      status = SemcoMxL601_ConfigSpurShiftAdjustment(&ParamPtr->MxLIf.cmdSuprShiftAdjCfg);
      break;

    case MXL_TUNER_HLFRF_CFG:
      status = SemcoMxL601_ConfigHlfrfSettings(&ParamPtr->MxLIf.cmdHlfrfCfg);
      break;

    default:
      status = MXL_FALSE;
      break;
  }
  return status;
}

/*--------------------------------------------------------------------------------
--| FUNCTION NAME : MxLWare601_API_GetTunerStatus 
--| 
--| AUTHOR        : Dong Liu
--|
--| DATE CREATED  : 02/12/2011
--|
--| DESCRIPTION   : The tuner specific inquiries shall be handled through this API         
--|
--| RETURN VALUE  : MXL_TRUE or MXL_FALSE
--|
--|-----------------------------------------------------------------------------*/

MXL_STATUS SemcoMxLWare601_API_GetTunerStatus(SEM_MXL_COMMAND_T *ParamPtr)
{
  MXL_STATUS status;
  SEM_MXL_CMD_TYPE_E cmdType;

  if (ParamPtr == NULL) return MXL_FALSE;

  cmdType = ParamPtr->commandId;

  MxL_DLL_DEBUG0("MxLWare601_API_GetTunerStatus : cmdType = %d", cmdType);

  //pbiinfo("zxguan   [%s %d]--------cmdtype-0x%x----MXL_TUNER_SIGNAL_TYPE_REQ = 0x%x--\n",__FUNCTION__,__LINE__,cmdType,MXL_TUNER_SIGNAL_TYPE_REQ);	

  switch (cmdType)
  {
    case MXL_TUNER_SIGNAL_TYPE_REQ:
      status = SemcoMxL601_GetTunerSignalType(&ParamPtr->MxLIf.cmdTunerSignalTypeReq);
      break;

    case MXL_TUNER_LOCK_STATUS_REQ:
      status = SemcoMxL601_GetTunerLockStatus(&ParamPtr->MxLIf.cmdTunerLockReq);
      break;

    case MXL_TUNER_AGC_LOCK_REQ:
      status = SemcoMxL601_GetTunerAgcLockStatus(&ParamPtr->MxLIf.cmdTunerAgcLockReq);
      break;

    case MXL_TUNER_RX_PWR_REQ:
      status = SemcoMxL601_GetTunerRxPower(&ParamPtr->MxLIf.cmdTunerPwrReq);
      break;

    case MXL_TUNER_AFC_CORRECTION_REQ:
      status = SemcoMxL601_GetTunerAfcCorrection(&ParamPtr->MxLIf.cmdTunerAfcCorectionReq);
      break;

    default:
      status = MXL_FALSE;
      break;
  }

  return status;
}
