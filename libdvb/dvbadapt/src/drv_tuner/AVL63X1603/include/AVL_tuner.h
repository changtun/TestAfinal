/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ������άͨѶ���Ӽ������޹�˾

 ******************************************************************************
  �� �� ��   : AVL_tuner.h
  �� �� ��   : ����
  ��    ��   : zhwu
  ��������   : 2012��6��11��
  ����޸�   :
  ��������   : AVL_tuner.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��6��11��
    ��    ��   : zhwu
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/

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

#ifndef __AVL_TUNER_H__
#define __AVL_TUNER_H__
#include "AVL63X1_include.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef struct AVL_TUNER_PARM
{
	AVL_uint32	m_uiFrequency_Hz;
	AVL_uchar	m_demod_mode;
	AVL_uint32	m_uiSymbolRate_Hz;
}AVL_TUNER_PARM_STRU;


typedef enum
{
	AVL_unLock=0,				/*ʧ��*/
	AVL_Lock,					/*����*/
	AVL_Lock_Err
}AVL63X1_LockCode;/*����״̬*/

 AVL63X1_LockCode AVL63X1_Tuner_GetLockStatus( AVL_uchar ucTuner_ID );
 AVL63X1_ErrorCode AVL63X1_Tuner_GetTunInfoBer( AVL_uchar Tuner_ID, AVL_uint32 *puiSignalBer );
 AVL63X1_ErrorCode AVL63X1_Tuner_GetTunInfoQualityPercent( AVL_uchar Tuner_ID, AVL_puint32 puiSignalQuality );
 AVL63X1_ErrorCode AVL63X1_Tuner_GetTunInfoSnr(AVL_uchar Tuner_ID, AVL_puint32 puiSignalSnr);
 AVL63X1_ErrorCode AVL63X1_Tuner_GetTunInfoStrengthPercent(AVL_uchar Tuner_ID, AVL_puint16 puiSignalPower);
 AVL63X1_ErrorCode AVL63X1_Tuner_Initialize( int BandWidth );
 AVL63X1_ErrorCode AVL63X1_Tuner_SetFrequency(AVL_uchar ucTuner_ID, AVL_TUNER_PARM_STRU *ptSrcInfo, AVL_uint32 uiTimeOut );
 AVL63X1_ErrorCode AVL63X1_Tuner_DeInit( void );

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __AVL_TUNER_H__ */
