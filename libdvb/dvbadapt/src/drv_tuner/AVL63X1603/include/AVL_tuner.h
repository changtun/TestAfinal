/******************************************************************************

                  版权所有 (C), 2001-2011, 北京加维通讯电子技术有限公司

 ******************************************************************************
  文 件 名   : AVL_tuner.h
  版 本 号   : 初稿
  作    者   : zhwu
  生成日期   : 2012年6月11日
  最近修改   :
  功能描述   : AVL_tuner.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2012年6月11日
    作    者   : zhwu
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
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
	AVL_unLock=0,				/*失锁*/
	AVL_Lock,					/*锁定*/
	AVL_Lock_Err
}AVL63X1_LockCode;/*锁定状态*/

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
