/******************************************************************************

                  ��Ȩ���� (C), 2011-2015, ������άͨѶ���Ӽ������޹�˾

 ******************************************************************************
  �� �� ��   : nim_TDA10025.h
  �� �� ��   : ����
  ��    ��   : �����
  ��������   : 2012��7��24��
  ����޸�   :
  ��������   : TDA10025�ӿ�ͷ�ļ���
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��7��24��
    ��    ��   : �����
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef _NIM_TDA10025_H_
#define _NIM_TDA10025_H_
/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �������Ͷ���                                   *
 *----------------------------------------------*/
/* BEGIN: Deleted by zhwu, 2012/12/14 */
#if  0
struct nim_tda10025_private
{
	/* struct for QAM Configuration */
	struct   QAM_TUNER_CONFIG_DATA tuner_config_data;
	
	/* Tuner Initialization Function */
	Int32 (*nim_Tuner_Init)(UInt32 * ptrTun_id, struct QAM_TUNER_CONFIG_EXT * ptrTuner_Config);

	/* Tuner Parameter Configuration Function */
	Int32 (*nim_Tuner_Control)(UInt32 Tun_id, UInt32 freq, UInt32 sym, UInt8 AGC_Time_Const, UInt8 _i2c_cmd);//since there will no bandwidth demand, so pass "sym" for later use.

	/* Get Tuner Status Function */
	Int32 (*nim_Tuner_Status)(UInt32 Tun_id, UInt8 *lock);

	/* zkliu. Add open and close API functions. */
	/* Open  Function*/
	Int32 (*nim_Tuner_Open)(UInt32 Tun_id);

	/* Close Function. */
	Int32 (*nim_Tuner_Close)(UInt32 Tun_id);
	/* END */
	
	/* Extension struct for Tuner Configuration */
	struct QAM_TUNER_CONFIG_EXT tuner_config_ext;
	//struct QAM_TUNER_CONFIG_API TUNER_PRIV;	

	UInt32 tuner_id;
};
#endif /* #if 0 */
/* END: Deleted by zhwu, 2012/12/14 */
/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/
 /* BEGIN: Modified by zhwu, 2012/12/14 */
#if  0
Int32 nim_TDA10025_Attach( char *name, struct QAM_TUNER_CONFIG_API * ptrQAM_Tuner);
#else
Int32 nim_TDA10025_Attach( void );
#endif /* #if 0 */
/* END:   Modified by zhwu, 2012/12/14 */
/* BEGIN: Added by zhwu, 2013/1/9 */
extern Int32 nim_TDA10025_Open( void );
extern Int32 nim_TDA10025_SetDemo( Int32 Modulation, UInt32 SymbolRate );
/* END:   Added by zhwu, 2013/1/9 */
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



#endif

