/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ������άͨѶ���Ӽ������޹�˾

 ******************************************************************************
  �� �� ��   : tuner_test.c
  �� �� ��   : ����
  ��    ��   : zhwu
  ��������   : 2012��6��12��
  ����޸�   :
  ��������   : tuner���Թ��ܺ���
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��6��12��
    ��    ��   : zhwu
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "stdio.h"
#include "AVL_tuner.h"
#include "tuner_test.h"
/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/

AVL63X1_ErrorCode AVL63X1_TestTuner()
{
	printf("-----------AVL63X1_TestTuner start ----------\n");
	#if  1
	AVL63X1_ErrorCode ret = AVL63X1_EC_OK;	
	
	AVL_TUNER_PARM_STRU ptSrcInfo;
	AVL63X1_LockCode Status = 0;
	AVL_uint16 strength=0;
	AVL_uint32 quality=0;
	AVL_uint32 puiSignalBer = 0;
	float puiSignalSnr = 0;
	AVL_int32 i = 0;
	
	/* ��ʼ��tuner */
	ret = AVL63X1_Tuner_Initialize();

	/* ��Ƶ */
	ptSrcInfo.m_demod_mode = AVL_DEMOD_MODE_DVBC;
	ptSrcInfo.m_uiFrequency_Hz = 698000000;//427000000;
	ptSrcInfo.m_uiSymbolRate_Hz = 6875000;//6952000;
	
	ret |= AVL63X1_Tuner_SetFrequency(0, &ptSrcInfo, 0);
	/* ��Ƶ״̬ */
	while( i < 50 )
	{
		Status = AVL63X1_Tuner_GetLockStatus(0);
		if ( Status == 1 )
		{
		    	break;
		}
		i++;
		usleep(100);
	}
	printf("---------- Lock status %d    i = %d-------\n",Status, i );

	/* �ź�ǿ�� */
	ret |= AVL63X1_Tuner_GetTunInfoStrengthPercent( 0, &strength);
	
	/* �ź����� */
	ret |= AVL63X1_Tuner_GetTunInfoQualityPercent( 0, &quality );

	printf("strength %d%, quality %d%\n",strength,quality);

	/* �ź������� */
	ret |= AVL63X1_Tuner_GetTunInfoBer(0, &puiSignalBer );

	printf("puiSignalBer %e.\n",puiSignalBer);
	
	usleep(10);

	while(1)
	{
	usleep(10000);
	/* ��ȡ����� */
	ret |= AVL63X1_Tuner_GetTunInfoSnr(0, &puiSignalSnr);

	printf("puiSignalSnr %.2f.\n",puiSignalSnr);
		
	}
#else
	demo_test_main();
#endif /* #if 0 */
	
	#if  0
	AVL63X1_ErrorCode ret = AVL63X1_EC_OK;


	AVL_int32 Status = 0;
	
//	struct AVL_Tuner tuner_info;
	AVL_DVBC_Channel sChannelDVBC;

	
	pTuner_info.m_uiFrequency_Hz = 666000000;

	ret = SemcoMxL601_Lock(&pTuner_info);
	if ( ret != AVL63X1_EC_OK )
	{
			printf("SemcoMxL601_Lock error\n");
		return ret;
	}

	usleep(200);//mdelay(100);

sChannelDVBC.m_demod_mode = AVL_DEMOD_MODE_DVBC;
	sChannelDVBC.m_uiSymbolRate_Hz = 6875000;
	sChannelDVBC.m_SignalInfo.m_Modulation = RX_DVBC_64QAM;
	sChannelDVBC.pAVLChip = &pAVL_Chip;
	sChannelDVBC.m_SpectrumInversion = SPECTRUM_NOT_INVERTED;
	
	ret = AVL63X1_LockChannel( LOCK_MODE_AUTO, &sChannelDVBC );
	if ( ret != AVL63X1_EC_OK )
	{
		printf("AVL63X1_LockChannel error\n");
		return ret;
	}
/*------------------tuner lock status-------------*/
	ret = SemcoMxL601_GetLockStatus(&pTuner_info);
	if ( pTuner_info.m_sStatus.m_uiLock == 1 )
	{
   	 	printf("---------------tuner Lock -------\n");
	}
	else
	{
		printf("--------------tuner UnLock -------\n");
	}
	

	/*------------------demo lock status-------------*/
	int count = 0;
for(;;)
{	
	
	ret = AVL63X1_GetLockStatus(&Status, &pAVL_Chip);
	if ( ret != AVL63X1_EC_OK )
	{
		printf("AVL63X1_LockChannel error\n");
		return ret;
	}

	if ( Status == 1 )
	{
   	 	printf("---------------demo Lock  count = %d-------\n",count);
		break;
	}
	else
	{
		//printf("--------------demo UnLock -------\n");
	}

	count +=1;// count++;
	usleep(100);
}

#endif /* #if 0 */

	printf("-----------AVL63X1_TestTuner end ----------\n");


	return AVL63X1_EC_OK;//ret;
}






