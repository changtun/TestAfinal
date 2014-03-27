/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ������άͨѶ���Ӽ������޹�˾

 ******************************************************************************
  �� �� ��   : file_config.h
  �� �� ��   : ����
  ��    ��   : zxguan
  �������   : 2013��4��15��
  ����޸�   :
  ��������   : file_config.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��4��15��
    ��    ��   : zxguan
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ��ͷ�ļ�                                   *
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

#ifndef __FILE_CONFIG_H__
#define __FILE_CONFIG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#if 0
typedef enum
{
    BAND_WIDTH_6M = 0,
    BAND_WIDTH_8M,
}eTUNER_BAND_WIDTH;
#endif
#define	eTUNER_BAND_WIDTH	int
#define	BAND_WIDTH_6M 	0
#define	BAND_WIDTH_8M	1

typedef enum
{
    SEARCH_BAT_ON = 0,
    SEARCH_BAT_OFF,
}eSEARCH_BAT_SWITCH;

typedef enum
{
    CHARACTER_GBK = 0,
    CHARACTER_BIG5,
    CHARACTER_UNICODE,
    CHARACTER_UTF8,
}eDEFAULT_CHARACTER_ENCODE;

typedef struct
{
    eTUNER_BAND_WIDTH eBandWidth;
    eSEARCH_BAT_SWITCH eBatSwitch;
    eDEFAULT_CHARACTER_ENCODE eEncode;
}CONFIG_WRITE_T;

extern void ConfigInit();
extern eDEFAULT_CHARACTER_ENCODE GetDefaultCharacterEncode();
extern eSEARCH_BAT_SWITCH GetSearchBatSwitch();
extern eTUNER_BAND_WIDTH GetTunerBandWidth();
extern void SetDVBConfig(CONFIG_WRITE_T tWriteInfo);
extern void GetDVBConfig(CONFIG_WRITE_T *ptInfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __FILE_CONFIG_H__ */
