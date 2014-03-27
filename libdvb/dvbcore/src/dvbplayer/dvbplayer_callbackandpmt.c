
/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name : dvbplayer_callbackandpmt.c
* Description : player对音视频的操作模块. 
* History :
*	Date               Modification                                Name
*	----------         ------------                                ----------
*	2007/12/28        Created                                     LSZ
******************************************************************************/
/*******************************************************/
/*              Includes				                   */
/*******************************************************/
/* Standard headers */

/* include ANSI C .h file */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* include OS, STAPI file */
#include "pvddefs.h"
#include "osapi.h"
//#include "pbi_systrace.h"
#include "drv_avctrl.h"

/* include Middle layer .h file */
/* include CA .h file */
/* include Local .h file */

#include "background.h"
#include "dvbplayer.h"
#include "tmsgqueue.h"
#include "dvbplayer_av.h"
#include "tlock.h"
#include "dvbplayer_callbackandpmt.h"
#include "pbitrace.h"

#define DB_CALLBACKANDPMT_MSG 0

#if DB_CALLBACKANDPMT_MSG
	#define DB_CALLBACKANDPMT(__statement__)  TRC_DBMSG(__statement__)
	TRC_DBMSG_SET_MODULE(SYS);
#else
	#define DB_CALLBACKANDPMT(__statement__)
#endif


/*****************************************************************************
 函 数 名  : DVBPlayer_Check_PMT
 功能描述  : Seach PAT Check PMT
 输入参数  : U16 TsId       
             U16 ServiceId  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年2月27日
    作    者   : zxguan
    修改内容   : 新生成函数

*****************************************************************************/
U32 DVBPlayer_Check_PMT()
{
    U32 Ret = SYS_TABLE_NOERROR;
   // Ret = SYS_BackGround_TableResume(SYS_BG_PAT);
    return Ret;
}

/******************************************************************************
* Function :DVBPlayer_Search_PMT
* parameters :	
*		U32* puiPMTSem(IN): PMT Semaphore pointer
*		U16  usPMTPid(IN): PMT pid
*		U16  usServiceID(IN): Service ID
* Return :	
*		1 : 开启PMT搜索失败
*		0 : 开启PMT搜索成功
* Description :
*		设置filter并且搜索PMT
*
* Author : LL	2007/12/11
******************************************************************************/
U32 DVBPlayer_Start_PMT(U16 usPMTPid, U16 usServiceID)
{
    U32 Ret = SYS_TABLE_NOERROR;
   // SYS_BackGround_TablePause(SYS_BG_PAT);
    Ret = SYS_BackGround_PMT_Reset(usPMTPid, usServiceID);
    if( SYS_TABLE_NOERROR != Ret )
    {
      //  PBIDEBUG("SYS_BackGround_PMT_Reset error!");
      //  pbiinfo("Ret = %d.\n",Ret );
    }
    return 0;
}

/******************************************************************************
* Function : DVBPlayer_Stop_PMT
* parameters :	
*		无
* Return :	
*		1 : 停止PMT搜索失败
*		0 : 停止PMT搜索成功
* Description :
*		停止搜索PMT
*
* Author : LL	2007/12/11
******************************************************************************/
U32 DVBPlayer_Stop_PMT(void)
{
	U32 ret = SYS_BackGround_TablePause(SYS_BG_PMT);
	if(SYS_TABLE_NOERROR == ret)
	{
		return 0;
	}
	return 1;
}

