/*****************************************************************************
*    (c)    Copyright Pro. Broadband Inc. PVware
*
* File name : drv_tuner.c
* Description :  This file  contain test case for tuner module
* History :
*    Date               Modification                                Name
*    ----------         ------------                                ----------
*    2010/06/03          Created                                zheng dong sheng
*   
******************************************************************************/
/*******************************************************/
/*              Includes				                                        */
/*******************************************************/
#include <stdio.h>
#include "pbitrace.h"
#include "bspdatadef.h"
#include "AVL_tuner.h"
#include "AVL63X1_include.h"
#include "AVL63X1.h"
#include "SemcoMxL601.h"
#include "ITuner.h"

/*******************************************************/
/*               Private Defines and Macros			                    */
/*******************************************************/


/*******************************************************/
/*               Private Types						                   */
/*******************************************************/

/*******************************************************/
/*               Private Constants                                                    */
/*******************************************************/

/*******************************************************/
/*               Global Variables					                          */
/*******************************************************/
AVL63X1_Chip pAVL_Chip;
struct AVL_Tuner pTuner_info;

extern AVL_uchar AVL63X1_FwData[];

/*******************************************************/
/*               Private Variables (static)			                          */
/*******************************************************/


/*******************************************************/
/*               Private Function prototypes		                          */
/*******************************************************/

/*******************************************************/
/*               Functions							                   */
/*******************************************************/

/*****************************************************************************
 �� �� ��  : AVL63X1_Tuner_Initialize
 ��������  : ��ʼ������tuner������demo
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2012��6��8��
    ��    ��   : zhwu
    �޸�����   : �����ɺ���

*****************************************************************************/
AVL63X1_ErrorCode AVL63X1_Tuner_Initialize( void )
{
#if  1
	AVL63X1_ErrorCode ret = AVL63X1_EC_OK;
	
	memset( &pAVL_Chip, 0, sizeof(AVL63X1_Chip) );
	memset( &pTuner_info, 0, sizeof(struct AVL_Tuner) );
//	AVL63X1_IBSP_Initialize();/* �ź�����ʼ���͸�λ */
	AVL63X1_IBSP_Initialize();/* �ź�����ʼ���͸�λ */
/*------------------ ����demo��ʼ��--------------------- */
	pAVL_Chip.m_SlaveAddr = AVL63X1_SA_0;
	pAVL_Chip.m_current_demod_mode = AVL_DEMOD_MODE_DVBC;
	pAVL_Chip.m_pInitialData = (AVL_puchar)AVL63X1_FwData;
	pAVL_Chip.m_InputSignal_Info.m_RxInputType_DVBC_mode = RX_REAL_IF_I;
	pAVL_Chip.m_InputSignal_Info.m_InputFrequency_Hz_DVBC_mode = 6*1000000;
	pAVL_Chip.m_MPEG_Info.m_mpcm = AVL63X1_MPCM_RISING;
	pAVL_Chip.m_MPEG_Info.m_mpfm_DVBC_mode = AVL63X1_MPF_TS;
	pAVL_Chip.m_MPEG_Info.m_mpm = AVL63X1_MPM_PARALLEL;
	pAVL_Chip.m_MPEG_Info.m_MPEG_ErrorBit = MPEG_ERROR_BIT_DISABLE;

	ret = AVL63X1_Initialize(&pAVL_Chip);
	if ( ret != AVL63X1_EC_OK )
	{
	    pbiinfo("----%s----%d--ret %d--\n\n",__FUNCTION__,__LINE__,ret);
    		return (AVL63X1_EC_I2C_FAIL);
	}

	ret = AVL63X1_SetAnalogAGC_Pola(AGC_NORMAL, &pAVL_Chip);
	if ( ret != AVL63X1_EC_OK)
	{
	    pbiinfo("----%s----%d----\n\n",__FUNCTION__,__LINE__);
	    	return (AVL63X1_EC_I2C_FAIL);
	}

	ret = AVL63X1_DriveIFAGC(AVL63X1_ON, &pAVL_Chip);
	if ( ret != AVL63X1_EC_OK)
	{
	    pbiinfo("----%s----%d----\n\n",__FUNCTION__,__LINE__);
	    	return (AVL63X1_EC_I2C_FAIL);
	}

	ret = AVL63X1_SetMPEG_Mode(&pAVL_Chip);
	if ( ret != AVL63X1_EC_OK)
	{
	pbiinfo("----%s----%d----\n\n",__FUNCTION__,__LINE__);
	    	return (AVL63X1_EC_I2C_FAIL);
	}

	ret = AVL63X1_DriveMpegOutput(AVL63X1_ON, &pAVL_Chip);
	if ( ret != AVL63X1_EC_OK)
	{
	pbiinfo("----%s----%d----\n\n",__FUNCTION__,__LINE__);
	    	return (AVL63X1_EC_I2C_FAIL);
	}

	pAVL_Chip.m_RepeaterInfo[0].m_TunerAddress = 0x60;
	pAVL_Chip.m_RepeaterInfo[0].m_RepeaterClockKHz = 400;
	pAVL_Chip.m_RepeaterInfo[0].m_uiRepeaterMode = MESSAGE_TYPE;

	pAVL_Chip.m_RepeaterInfo[1].m_TunerAddress = 0x60;//TUNER1_ADDRESS;
	pAVL_Chip.m_RepeaterInfo[1].m_RepeaterClockKHz = 400;//TUNER1_CLOCK_SPEED_KHZ;
	pAVL_Chip.m_RepeaterInfo[1].m_uiRepeaterMode = MESSAGE_TYPE;
	
/* ------------------����tuner��ʼ�� -------------------------*/

	pTuner_info.m_uiSlaveAddress = 0x60;
	pTuner_info.m_uiFrequency_Hz = 859*1000000;/* ��ʼ��Ĭ��Ƶ�� */
	pTuner_info.m_pAVL_Chip = &pAVL_Chip;

	ret = SemcoMxL601_Initialize(&pTuner_info);
	if ( ret != AVL63X1_EC_OK )
	{
	    pbiinfo("----%s----%d----\n\n",__FUNCTION__,__LINE__);
    		return (AVL63X1_EC_I2C_FAIL);
	}
	
#endif /* #if 1 */
	return(AVL63X1_EC_OK);
	
}


/*****************************************************************************
 �� �� ��  : AVL_Tuner_SetFrequency
 ��������  : lock tuner��channel
 �������  : AVL_uchar ucTuner_ID          
             AVL_TUNER_PARM_STRU *ptSrcInfo  
             AVL_uint32 uiTimeOut             
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2012��6��8��
    ��    ��   : zhwu
    �޸�����   : �����ɺ���

*****************************************************************************/
AVL63X1_ErrorCode AVL63X1_Tuner_SetFrequency(AVL_uchar ucTuner_ID, AVL_TUNER_PARM_STRU *ptSrcInfo, AVL_uint32 uiTimeOut )
{

	AVL63X1_ErrorCode ret = AVL63X1_EC_OK;

	AVL_DVBC_Channel sChannelDVBC;

	pTuner_info.m_uiFrequency_Hz = ptSrcInfo->m_uiFrequency_Hz;

	/*-----------Lock tuner--------*/	
	ret = SemcoMxL601_Lock(&pTuner_info);
	if ( ret != AVL63X1_EC_OK )
	{
		pbiinfo("SemcoMxL601_Lock error\n");
		return ret;
	}

	/*-----------Lock demo--------*/	

	if ( AVL_DEMOD_MODE_DVBC == ptSrcInfo->m_demod_mode )
	{
		sChannelDVBC.m_uiSymbolRate_Hz = ptSrcInfo->m_uiSymbolRate_Hz ;
	}
	sChannelDVBC.m_demod_mode = ptSrcInfo->m_demod_mode;
	sChannelDVBC.pAVLChip = &pAVL_Chip;
	sChannelDVBC.m_SpectrumInversion = SPECTRUM_NOT_INVERTED;

	ret = AVL63X1_LockChannel(/*LOCK_MODE_MANUAL*/LOCK_MODE_AUTO, &sChannelDVBC);
	if ( ret != AVL63X1_EC_OK )
	{
		pbiinfo("AVL63X1_LockChannel error\n");
		return ret;
	}
	

	return AVL63X1_EC_OK;
	
}
/*****************************************************************************
 �� �� ��  : AVL63X1_Tuner_GetLockStatus
 ��������  : �����Ƶ״̬�ú�����������
 �������  : AVL_uchar ucTuner_ID  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2012��6��8��
    ��    ��   : zhwu
    �޸�����   : �����ɺ���

  2.��    ��   : 2012��6��14��
    ��    ��   : zhwu
    �޸�����   : �޸ĺ���Ϊ��������
*****************************************************************************/
AVL63X1_LockCode  AVL63X1_Tuner_GetLockStatus( AVL_uchar ucTuner_ID )
{
	AVL_int32 Status = 0;
	AVL63X1_ErrorCode ret = AVL63X1_EC_OK;
	AVL_int32 retry = 0;

//pbiinfo("zxguan   [%s %d]---------------\n",__FUNCTION__,__LINE__);	
	/*------------------tuner lock status-------------*/
	ret = SemcoMxL601_GetLockStatus(&pTuner_info);
	if ( ret != AVL63X1_EC_OK )
	{
		pbiinfo("SemcoMxL601_GetLockStatus error \n");
	    return AVL_Lock_Err;
	}
//pbiinfo("zxguan   [%s %d]---------------\n",__FUNCTION__,__LINE__);	
	/*------------------demo lock status-------------*/

	ret = AVL63X1_GetLockStatus(&Status,&pAVL_Chip);
	if ( ret != AVL63X1_EC_OK )
	{
		pbiinfo("AVL63X1_GetLockStatus error \n");
		return AVL_Lock_Err;
	}
	if (( pTuner_info.m_sStatus.m_uiLock == 1 )&&( Status == 1))
	{
		return AVL_Lock;
	}
//pbiinfo("zxguan   [%s %d]---------------\n",__FUNCTION__,__LINE__);
	return AVL_unLock;
}

/*****************************************************************************
 �� �� ��  : AVL63X1_Tuner_GetTunInfoSnr
 ��������  : ���tuner��snr
 �������  : AVL_uchar Tuner_ID        
             AVL_uint32 *puiSignalSnr  
 �������  : ��
 �� �� ֵ  : AVL63X1_EC_OK
 			AVL63X1_EC_I2C_FAIL
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2012��6��8��
    ��    ��   : zhwu
    �޸�����   : �����ɺ���

  2.��    ��   : 2012��6��19��
    ��    ��   : zhwu
    �޸�����   : ���StartPER

*****************************************************************************/
AVL63X1_ErrorCode AVL63X1_Tuner_GetTunInfoSnr(AVL_uchar Tuner_ID, AVL_puint32 puiSignalSnr)
{
	AVL63X1_ErrorCode ret = AVL63X1_EC_OK;
	AVL_uint32 uiTemp;
	AVL63X1_ErrorStats err_state;

	//ret = AVL63X1_StartPER(&pAVL_Chip, &err_state );
	ret = AVL63X1_GetSNR( &uiTemp, &pAVL_Chip );
	if ( ret != AVL63X1_EC_OK )
	{
		pbierror("ret = %d \n",ret);
	    return ret;
	}

	*puiSignalSnr = uiTemp;

	return AVL63X1_EC_OK;
}
/*****************************************************************************
 �� �� ��  : AVL63X1_Tuner_GetTunInfoStrengthPercent
 ��������  : ����ź�ǿ�����ź�ʱ����100��ע��
 �������  : AVL_uchar Tuner_ID          
             AVL_uint16 *puiSignalPower  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2012��6��11��
    ��    ��   : zhwu
    �޸�����   : �����ɺ���

  2.��    ��   : 2012��7��2��
    ��    ��   : zhwu
    �޸�����   : ������ź�ʱ����0���޸��˼����ź������ķ�Χ

*****************************************************************************/
AVL63X1_ErrorCode AVL63X1_Tuner_GetTunInfoStrengthPercent(AVL_uchar Tuner_ID, AVL_puint16 puiSignalPower)
{
	AVL63X1_ErrorCode ret = AVL63X1_EC_OK;
/* BEGIN: Added by zhwu, 2012/6/21 */
	AVL_int32 Status = 0;
	ret = AVL63X1_GetLockStatus(	&Status, &pAVL_Chip);
	if ( 0 == Status  )
	{
		*puiSignalPower = 0;
			return ret;
	}
/* END:   Added by zhwu, 2012/6/21 */
	ret = AVL63X1_GetStrength(puiSignalPower,&pAVL_Chip);
	if ( ret != AVL63X1_EC_OK )
	{
	    return ret;
	}
	pbiinfo(" puiSignalPower %d  ,line %d\n",*puiSignalPower,__LINE__);
	if (( *puiSignalPower >= 20000 ) && (*puiSignalPower <= 32000) )
	{
	    	*puiSignalPower =  (int)(((float)(*puiSignalPower)-10000)/(32000-10000)*100);
	}
	else
	{
	    *puiSignalPower = 100;
	}
		
	
	return AVL63X1_EC_OK;
}
/*****************************************************************************
 �� �� ��  : AVL63X1_Tuner_GetTunInfoQualityPercent
 ��������  : ��ȡ�ź��������ź�ʱ����100��ע��
 �������  : AVL_uchar Tuner_ID            
             AVL_uint32 *puiSignalQuality  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2012��6��11��
    ��    ��   : zhwu
    �޸�����   : �����ɺ���

  2.��    ��   : 2012��7��2��
    ��    ��   : zhwu
    �޸�����   : ������ź�ʱ����0ֵ

*****************************************************************************/
AVL63X1_ErrorCode AVL63X1_Tuner_GetTunInfoQualityPercent( AVL_uchar Tuner_ID, AVL_puint32 puiSignalQuality )
{
	AVL63X1_ErrorCode ret = AVL63X1_EC_OK;
	/* BEGIN: Added by zhwu, 2012/6/21 */
	AVL_int32 Status = 0;
	ret = AVL63X1_GetLockStatus(	&Status, &pAVL_Chip);
	if ( 0 == Status  )
	{
		*puiSignalQuality = 0;
			return ret;
	}

	/* END:   Added by zhwu, 2012/6/21 */
	ret = AVL63X1_GetSignalQuality(puiSignalQuality, 0, &pAVL_Chip );
	if ( ret != AVL63X1_EC_OK )
	{
		return ret;
	}

	return ( AVL63X1_EC_OK );
}
/*****************************************************************************
 �� �� ��  : AVL63X1_Tuner_GetTunInfoBer
 ��������  : ��ȡ�����ʣ���Ҫ�����������ʹ�ã�����DVBC
 �������  : AVL_uchar Tuner_ID        
             AVL_uint32 *puiSignalBer  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2012��6��11��
    ��    ��   : zhwu
    �޸�����   : �����ɺ���

*****************************************************************************/
AVL63X1_ErrorCode AVL63X1_Tuner_GetTunInfoBer( AVL_uchar Tuner_ID, AVL_uint32 *puiSignalBer )
{
	AVL63X1_ErrorCode ret = AVL63X1_EC_OK;
	AVL_int32 temp;
	
	ret = AVL63X1_DVBC_GetBER_BeforeRS( &temp, &pAVL_Chip );
	if ( ret != AVL63X1_EC_OK )
	{
		return ret;
	}

	*puiSignalBer =  temp;

	return ( AVL63X1_EC_OK ); 
}
/*****************************************************************************
 �� �� ��  : AVL63X1_Tuner_DeInit
 ��������  : AVL63X1ȥ��ʼ��������Ŀǰֻ��ȥ�ź�������
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2012��10��18��
    ��    ��   : zhwu
    �޸�����   : �����ɺ���

*****************************************************************************/
AVL63X1_ErrorCode AVL63X1_Tuner_DeInit( void )
{
	AVL63X1_IBSP_Dispose();

	return ( AVL63X1_EC_OK );
}

