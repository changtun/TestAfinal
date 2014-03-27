/*
 *           Copyright 2007-2012 Availink, Inc.
 *
 *  This software contains Availink proprietary information and
 *  its use and disclosure are restricted solely to the terms in
 *  the corresponding written license agreement. It shall not be 
 *  disclosed to anyone other than valid licensees without
 *  written permission of Availink, Inc.
 *
 */


///
/// @file
/// @brief Implements functions declared in IBSP.h. 
/// 
//$Revision: 390 $ 
//$Date: 2008-05-20 15:44:17 -0400 (Tue, 20 May 2008) $
//
#include <stdio.h>
#include "stdlib.h"
#include <unistd.h>
#include "unistd.h"
#include <fcntl.h> 
#include <errno.h>
#include "semaphore.h"
#include "sys/time.h"

#include "IBSP.h"
#include "SemcoMxL601.h"
#include "ITuner.h"

#include "osapi.h"
#include "pbitrace.h"

#define AVL_MAX_SEMAPHORES 10



typedef struct AVL_SEM_RECORD_T 
{
	AVL_int32 free;
    sem_t id;
}AVL_SEM_RECORD_T_STRU;

struct AVL_SEM_RECORD_T AVL_sem_table[AVL_MAX_SEMAPHORES];

///
/// Users can remove unused elements from this array to reduce the footprint.
/// Please note that some compilers do not support the comma after the last element. 
/// Be aware of this when removing the last element.
const struct AVL63X1_PLL_Conf pll_conf[] = 
{
	 {3,139,3,10,5,110,0,30000000,210000000,87500000,150000000, SIP} // Ref Clk 30 MHz, Demod Clk 210 MHz, SDRAM Clk 87.5 MHz, FEC Clk 150 MHz, SDRAM Clock Type = SIP	
	,{3,146,3,10,5,110,0,30000000,220500000,91875000,157500000, SIP} // Ref Clk 30 MHz, Demod Clk 220.5 MHz, SDRAM Clk 91.875 MHz, FEC Clk 157.5MHz, SDRAM Clock Type = SIP
	,{1,79,3,10,6,110,0,30000000, 240000000,100000000,150000000, SIP} // Ref Clk 30 MHz, Demod Clk 240 MHz, SDRAM Clk 100 MHz, FEC Clk 150 MHz, SDRAM Clock Type = SIP
	,{4,162,2,8,4,110,0,30000000, 244500000,97800000,163000000, SIP} // Ref Clk 30 MHz, Demod Clk 244.5 MHz, SDRAM Clk 97.8 MHz, FEC Clk 163 MHz, SDRAM Clock Type = SIP
	,{1,74,4,10,6,110,0,31000000,193750000,96875000,145312500, SIP} //Ref Clk 31 MHz, Demod Clk 193.75 MHz, SDRAM Clk 96.875 MHz, FEC Clk 145.3125 MHz, SDRAM Type = SIP 
	,{1,56,2,8,4,110,0,31000000,220875000,88350000,147250000, SIP} //Ref Clk 31 MHz, Demod Clk 220.875 MHz, SDRAM Clk 88.35 MHz, FEC Clk 147.25 MHz, SDRAM Type = SIP
	,{3, 149,3,10,6,110,0,32000000, 240000000,100000000,150000000, SIP} // Ref Clk 32 MHz, Demod Clk 240 MHz, SDRAM Clk 100 MHz, FEC Clk 150 MHz, SDRAM Clock Type = SIP
	,{3,137,3,10,6,110,0,32000000,220800000,92000000,138000000, SIP} //Ref Clk 32 MHz, Demod Clk 220.8 MHz, SDRAM Clk 92 MHz, FEC Clk 138 MHz, SDRAM Clock Type = SIP
	,{3,139,3,10,5,48,0,30000000,210000000,87500000,150000000, NON_SIP} // Ref Clk 30 MHz, Demod Clk 210 MHz, SDRAM Clk 87.5 MHz, FEC Clk 150 MHz, SDRAM Clock Type = NON SIP						
	,{3,146,3,10,5,50,0,30000000,220500000,91875000,157500000, NON_SIP} // Ref Clk 30 MHz, Demod Clk 220.5 MHz, SDRAM Clk 91.875 MHz, FEC Clk 157.5MHz, SDRAM Clock Type = NON SIP
	,{1,79,3,10,6,55,0,30000000, 240000000,100000000,150000000, NON_SIP} // Ref Clk 30 MHz, Demod Clk 240 MHz, SDRAM Clk 100 MHz, FEC Clk 150 MHz, SDRAM Clock Type = NON SIP
	,{4,162,2,8,4,54,0,30000000, 244500000,97800000,163000000, NON_SIP} // Ref Clk 30 MHz, Demod Clk 244.5 MHz, SDRAM Clk 97.8 MHz, FEC Clk 163 MHz, SDRAM Clock Type = NON SIP
	,{1,74,4,10,6,54,0,31000000,193750000,96875000,145312500, NON_SIP} //Ref Clk 31MHz, Demod Clk 193.75 MHz, SDRAM Clk 96.875MHz, FEC Clk 145.3125MHz, SDRAM Type = NON SIP
	,{1,56,2,8,4,52,0,31000000,220875000,88350000,147250000, NON_SIP} //Ref Clk 31 MHz, Demod Clk 220.875 MHz, SDRAM Clk 88.35 MHz, FEC Clk 147.25 MHz, SDRAM Type = NON_SIP
	,{3,149,3,10,6,55,0,32000000, 240000000,100000000,150000000, NON_SIP} // Ref Clk 32 MHz, Demod Clk 240 MHz, SDRAM Clk 100 MHz, FEC Clk 150 MHz, SDRAM Clock Type = NON SIP
	,{3,137,3,10,6,51,0,32000000,220800000,92000000,138000000, NON_SIP} //Ref Clk 32 MHz, Demod Clk 220.8 MHz, SDRAM Clk 92 MHz, FEC Clk 138 MHz, SDRAM Clock Type = NON_SIP
};
const AVL_uint16 pll_array_size = sizeof(pll_conf)/sizeof(struct AVL63X1_PLL_Conf);

struct AVL63X1_SDRAM_Conf sdram_conf = 
{
	63, 84, 84, 1, 42, 21, 42, 2  
};
const AVL_uint16 sdram_array_size = sizeof(sdram_conf)/sizeof(struct AVL63X1_SDRAM_Conf);

AVL63X1_ErrorCode AVL63X1_IBSP_Delay( AVL_uint32 uiDelay_ms )
{
	#if  1
	usleep( (unsigned long) uiDelay_ms * 1000);

	#else
	
	struct timeval tpStart, tpEnd;
	float timeUse;
	
    gettimeofday(&tpStart, NULL);
    do{
    	gettimeofday(&tpEnd, NULL);
		timeUse = 1000*(tpEnd.tv_sec - tpStart.tv_sec) + 0.001 *(tpEnd.tv_usec - tpStart.tv_usec);
    } while(timeUse < uiDelay_ms);

	#endif /* #if 0 */

	return(AVL63X1_EC_OK);
}
AVL63X1_ErrorCode AVL63X1_IBSP_I2C_Read(  AVL_uint16 uiSlaveAddr, AVL_puchar pucBuff, AVL_puint16 puiSize )
{
	int res;
	res = DRV_I2C_Read( uiSlaveAddr, NULL, 0, pucBuff, *puiSize );
	if(res != 0)
	{
		pbierror("FUN:%s,LINE:%d, error! [%x] \n",__FUNCTION__,__LINE__, res);
		return AVL63X1_EC_I2C_FAIL;
	}
	return(AVL63X1_EC_OK);
}

AVL63X1_ErrorCode AVL63X1_IBSP_I2C_Write(  AVL_uint16 uiSlaveAddr,  AVL_puchar pucBuff,  AVL_puint16  puiSize )
{	
	int res = 0; 
	res = DRV_I2C_Write( uiSlaveAddr, NULL, 0, pucBuff, *puiSize);
	if(res != 0)
	{
		pbierror("FUN:%s,LINE:%d, error! [%x] \n",__FUNCTION__,__LINE__, res);
		return AVL63X1_EC_I2C_FAIL;
	}
	return(AVL63X1_EC_OK);
}

AVL63X1_ErrorCode AVL63X1_IBSP_InitSemaphore( AVL_psemaphore pSemaphore )
{	
	AVL_int32 iI = 0;
	AVL_int32 retCode = 0;
	for ( iI = 0 ; iI < AVL_MAX_SEMAPHORES; iI++ )
	{
	    if ( AVL_sem_table[iI].free == 1 )
	    {
	        break;
	    }
	}
	/* It isn't seem  happen any more */
    if((iI >= AVL_MAX_SEMAPHORES) || ( AVL_sem_table[iI].free != 1 ))
        return AVL63X1_EC_BSP_ERROR1;
		
	retCode = sem_init( &(AVL_sem_table[iI].id), 0, 1 );
	if ( retCode != 0 )
	{
	    pbierror("AVL_DVBSx_IBSP_InitSemaphore error! retCode %x \n",retCode);
		return AVL63X1_EC_BSP_ERROR1;
	}
	AVL_sem_table[iI].free = 0;
	*pSemaphore = iI;

	return(AVL63X1_EC_OK);
}

AVL63X1_ErrorCode AVL63X1_IBSP_ReleaseSemaphore( AVL_psemaphore pSemaphore )
{
	AVL_int32 iI = 0;

	iI = *pSemaphore;
    if(( iI>= AVL_MAX_SEMAPHORES ) || (AVL_sem_table[iI].free == 1 ))
        return AVL63X1_EC_BSP_ERROR1;
	
	sem_post( &AVL_sem_table[iI].id );

	return(AVL63X1_EC_OK);
}

AVL63X1_ErrorCode AVL63X1_IBSP_WaitSemaphore( AVL_psemaphore pSemaphore )
{
	AVL_int32 iI = 0;

	iI = *pSemaphore;
	if( ( AVL_sem_table[iI].free == 1) || ( iI >= AVL_MAX_SEMAPHORES ))
    	return AVL63X1_EC_BSP_ERROR1;
	sem_wait( &AVL_sem_table[iI].id );

	return(AVL63X1_EC_OK);
}


AVL63X1_ErrorCode AVL63X1_IBSP_Initialize(void)
{
	AVL_int32 iI = 0;

	/* 标识信号量状态，所有均可用 */
	for ( iI = 0 ; iI < AVL_MAX_SEMAPHORES; iI++ )
	{
	    AVL_sem_table[iI].free = 1;
	}
	system("echo w:d:8:1 > /sys/class/gpio/cmd ");
	AVL63X1_IBSP_Delay(10);
	system("echo w:d:8:0 > /sys/class/gpio/cmd ");
	AVL63X1_IBSP_Delay(300);
	system("echo w:d:8:1 > /sys/class/gpio/cmd ");
	AVL63X1_IBSP_Delay(10);
	
	return(AVL63X1_EC_OK);
}
/*****************************************************************************
 函 数 名  : AVL63X1_IBSP_Dispose
 功能描述  : 目前只是释放tuner驱动中的信号量
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年10月18日
    作    者   : zhwu
    修改内容   : 新生成函数

*****************************************************************************/
AVL63X1_ErrorCode AVL63X1_IBSP_Dispose(void)
{
	AVL_int32 iI = 0;

	for ( iI = 0 ; iI < AVL_MAX_SEMAPHORES ; iI++ )
	{
		if ( AVL_sem_table[iI].free == 0 )
		{
		    sem_destroy( &(AVL_sem_table[iI].id) );
		}
	}
	
	return(AVL63X1_EC_OK);
}
