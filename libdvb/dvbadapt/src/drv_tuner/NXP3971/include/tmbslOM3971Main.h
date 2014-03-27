/******************************************************************************

                  版权所有 (C), 2001-2011, 北京加维通讯电子技术有限公司

 ******************************************************************************
  文 件 名   : tmbslOM3971Main.h
  版 本 号   : 初稿
  作    者   : zhwu
  生成日期   : 2013年1月14日
  最近修改   :
  功能描述   : tmbslOM3971Main.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年1月14日
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

#ifndef __TMBSLOM3971MAIN_H__
#define __TMBSLOM3971MAIN_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern int OM3971_Open(void);
/* BEGIN: Modified by zhwu, 2013/3/8 */
extern tmErrorCode_t OM3971_SetFrequence( UInt32 Frequence, UInt32 SymbolRate, UInt16 Modulation, UInt8 BandWidth );
/* END:   Modified by zhwu, 2013/3/8 */
extern tmErrorCode_t OM3971_GetLockStatus( UInt8 *LStatus );
extern tmErrorCode_t OM3971_GetSignalBer( Int32 *Integer, UInt32 *Divider );
extern tmErrorCode_t OM3971_GetSignalQuality( UInt32 *Quality );
extern tmErrorCode_t OM3971_GetSignalSnr( Int32 *Integer, UInt32 *Divider );
extern tmErrorCode_t OM3971_GetSignalStrength( UInt32 *Strength );
extern tmErrorCode_t OM3971_Close( void );
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __TMBSLOM3971MAIN_H__ */
