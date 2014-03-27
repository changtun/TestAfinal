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
#include <unistd.h>
#include "unistd.h"
#include "semaphore.h"
#include "sys/time.h"

#include "hi_type.h"
//#include "hi_common.h"
//#include "hi_unf_ecs.h"

#include "IBSP.h"
#include "SemcoMxL601.h"
#include "ITuner.h"

#include "drv_tuner.h"

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
#if 1
	HI_U32  u32RegAddr = 0, u32RegNum = 0,  u32DataLen = 0;
	HI_U8   *pu8Data = NULL;
	HI_S32 ret = 0;

	uiSlaveAddr	= (uiSlaveAddr << 1) | 1; 
	u32RegAddr = (pucBuff[0] << 24) + (pucBuff[1] << 16) + pucBuff[2];
	//u32RegAddr = (HI_U32)uiRegAddr;
	u32RegNum = 0;
	u32DataLen = (HI_U32)(*puiSize);
	pu8Data = pucBuff;
	//pbiinfo("zxguan[%s %d]----datalen- 0x%x--------\n",u32DataLen);
	//ret = HI_I2C_Read(0, (HI_U8)uiSlaveAddr, u32RegAddr,u32RegNum, pu8Data, u32DataLen);
	//printf("[R] uiSlaveAddr = 0x%x, pucBuff[0] = %x, pucBuff[1] = %x, pucBuff[2] = %x\n, uiSize = %d.\n",uiSlaveAddr,pucBuff[0],pucBuff[1],pucBuff[2],*puiSize);
	ret = HI_UNF_I2C_Read(0, (HI_U8)uiSlaveAddr, u32RegAddr,u32RegNum, pu8Data, u32DataLen);
	if ( ret != 0 )
	{
		pbiinfo("zxguan[%s %d]-----ret--0x%x--\n",__FUNCTION__,__LINE__,ret);
	    	return (AVL63X1_EC_I2C_FAIL);
	}
#endif
	return(AVL63X1_EC_OK);
}



AVL63X1_ErrorCode AVL63X1_IBSP_I2C_Write_Reset(  AVL_uint16 uiSlaveAddr )
{
#if 0
/* I2C_COM_REB */
#define I2C_SEND_ACK (~(1 << 4))
#define I2C_START (1 << 3)
#define I2C_READ (1 << 2)
#define I2C_WRITE (1 << 1)
#define I2C_STOP (1 << 0)

/* I2C_ICR_REG */
#define I2C_CLEAR_START (1 << 6)
#define I2C_CLEAR_END (1 << 5)
#define I2C_CLEAR_SEND (1 << 4)
#define I2C_CLEAR_RECEIVE (1 << 3)
#define I2C_CLEAR_ACK (1 << 2)
#define I2C_CLEAR_ARBITRATE (1 << 1)
#define I2C_CLEAR_OVER (1 << 0)
#define I2C_CLEAR_ALL (I2C_CLEAR_START | I2C_CLEAR_END | \
                       I2C_CLEAR_SEND | I2C_CLEAR_RECEIVE | \
                       I2C_CLEAR_ACK | I2C_CLEAR_ARBITRATE | \
                       I2C_CLEAR_OVER)


#define I2C0_PHY_ADDR     (0x101F7000)
#define I2C1_PHY_ADDR     (0x101F8000)
#define I2C2_PHY_ADDR     (0x101F9000)
#define I2C3_PHY_ADDR     (0x101FA000)
#define I2C4_PHY_ADDR     (0x101F6000)

#define I2C_CTRL_REG      (0x000)
#define I2C_COM_REB       (0x004)
#define I2C_ICR_REG       (0x008)
#define I2C_SR_REG        (0x00C)
#define I2C_SCL_H_REG     (0x010)
#define I2C_SCL_L_REG     (0x014)
#define I2C_TXR_REG       (0x018)
#define I2C_RXR_REG       (0x01C)


#define READ_OPERATION     (1)
#define WRITE_OPERATION    0xfe

   HI_U32          i;
   HI_U8 pu8Buff[7] = {0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
   HI_U32  u32Size = 7;

	//pbiinfo("zxguan--[%s %d]-------------------\n",__FUNCTION__,__LINE__);
	uiSlaveAddr = uiSlaveAddr << 1;

    /*  clear interrupt flag*/
    HI_SYS_WriteRegister((I2C0_PHY_ADDR + I2C_ICR_REG), I2C_CLEAR_ALL);

    /* send devide address */
    HI_SYS_WriteRegister((I2C0_PHY_ADDR + I2C_TXR_REG), (uiSlaveAddr & WRITE_OPERATION));
    HI_SYS_WriteRegister((I2C0_PHY_ADDR + I2C_COM_REB),(I2C_WRITE | I2C_START));
    usleep(10);
    HI_SYS_WriteRegister((I2C0_PHY_ADDR + I2C_ICR_REG), I2C_CLEAR_ALL);


	/* send data */
	for (i=0; i<u32Size; i++)
	{
        HI_SYS_WriteRegister((I2C0_PHY_ADDR + I2C_TXR_REG), (*(pu8Buff+i)));
    	HI_SYS_WriteRegister((I2C0_PHY_ADDR + I2C_COM_REB), I2C_WRITE);
        usleep(10);
    HI_SYS_WriteRegister((I2C0_PHY_ADDR + I2C_ICR_REG), I2C_CLEAR_ALL);

	}

	/*   send stop flag bit*/
	HI_SYS_WriteRegister((I2C0_PHY_ADDR + I2C_COM_REB), I2C_STOP);
        usleep(10);
    HI_SYS_WriteRegister((I2C0_PHY_ADDR + I2C_ICR_REG), I2C_CLEAR_ALL);

#endif
    return AVL63X1_EC_OK;
}





AVL63X1_ErrorCode AVL63X1_IBSP_I2C_Write(  AVL_uint16 uiSlaveAddr,  AVL_puchar pucBuff,  AVL_puint16  puiSize )
{	
#if 0
	HI_U32  u32RegAddr = 0, u32RegNum= 0, u32DataLen = 0;
	HI_U8   *pData = NULL;
	HI_S32 ret = 0;

	
	uiSlaveAddr = uiSlaveAddr << 1;
	u32RegAddr = (pucBuff[0] << 24) + (pucBuff[1] << 16) + pucBuff[2];
	/* I2cRegAddrByteNum1：表示8bit子地址；2：表示16bit子地址；3：表示24bit子地址；4：表示32bit子地址。*/
	u32RegNum = 0;
	u32DataLen = (HI_U32)( *puiSize);// - 1);
	pData = (HI_U8*)(pucBuff);//+1);

	if( NULL == pucBuff )
	{
		pbiinfo("zxguan [%s %d]-----buffer is NULL ----\n",__FUNCTION__,__LINE__);
	}
	
	/* I2C的端口号定为0号 I2cNum*/
	//ret = HI_I2C_Write( 0, (HI_U8)uiSlaveAddr, u32RegAddr, u32RegNum, pData, u32DataLen );
	// printf("uiSlaveAddr = 0x%x , u32RegAddr = 0x%x , %x , %x , %x \n",uiSlaveAddr,u32RegAddr,u32RegNum,pData,u32DataLen);
	//printf("[W] uiSlaveAddr = 0x%x, pucBuff[0] = %x, pucBuff[1] = %x, pucBuff[2] = %x\n, uiSize = %d.\n",uiSlaveAddr,pucBuff[0],pucBuff[1],pucBuff[2],*puiSize);
	//pbiinfo("zxguan[%s %d]----datalen- 0x%x-----addr 0x%x-----\n",__FUNCTION__,__LINE__,u32DataLen, uiSlaveAddr);
	ret = HI_UNF_I2C_Write( 0, (HI_U8)uiSlaveAddr, u32RegAddr, 0, pData, u32DataLen);
	if ( ret != 0 )
	{
		pbiinfo("zxguan[%s %d]-----ret--0x%x--\n",__FUNCTION__,__LINE__,ret);
		//printf("[W] uiSlaveAddr = 0x%x, %d. %02X, %02X, %02X, %02X, %02X, %02X, %02X.\n",uiSlaveAddr, *puiSize,
		//				pucBuff[0],pucBuff[1],pucBuff[2], pucBuff[3],pucBuff[4],pucBuff[5], pucBuff[6]);
	  	return (AVL63X1_EC_I2C_FAIL);
	}
#endif
	return(AVL63X1_EC_OK);
}

AVL63X1_ErrorCode AVL63X1_IBSP_InitSemaphore( AVL_psemaphore pSemaphore )
{	
#if 0
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
	       pbiinfo("AVL_DVBSx_IBSP_InitSemaphore error! retCode %x \n",retCode);
		return AVL63X1_EC_BSP_ERROR1;
	}
	AVL_sem_table[iI].free = 0;
	*pSemaphore = iI;
#endif
	return(AVL63X1_EC_OK);
}

AVL63X1_ErrorCode AVL63X1_IBSP_ReleaseSemaphore( AVL_psemaphore pSemaphore )
{
	AVL_int32 iI = 0;
#if 0
	iI = *pSemaphore;
    if(( iI>= AVL_MAX_SEMAPHORES ) || (AVL_sem_table[iI].free == 1 ))
        return AVL63X1_EC_BSP_ERROR1;
	
	sem_post( &AVL_sem_table[iI].id );
#endif
	return(AVL63X1_EC_OK);
}

AVL63X1_ErrorCode AVL63X1_IBSP_WaitSemaphore( AVL_psemaphore pSemaphore )
{
	AVL_int32 iI = 0;
#if 0
	iI = *pSemaphore;
	if( ( AVL_sem_table[iI].free == 1) || ( iI >= AVL_MAX_SEMAPHORES ))
    	return AVL63X1_EC_BSP_ERROR1;
	sem_wait( &AVL_sem_table[iI].id );
#endif
	return(AVL63X1_EC_OK);
}
AVL63X1_ErrorCode AVL63X1_IBSP_Initialize(void)
{
	HI_S32 res = 0;
	AVL_int32 iI = 0;

#if 0	
	/* 标识信号量状态，所有均可用 */
	for ( iI = 0 ; iI < AVL_MAX_SEMAPHORES; iI++ )
	{
	    AVL_sem_table[iI].free = 1;
	}
	#if  1
	/* GPIO软件复位 */
	HI_UNF_GPIO_Open();
	
	res = HI_UNF_GPIO_SetDirBit( 12, HI_FALSE );/* Gpio1_4 1*8+4=12 */
	res |= HI_UNF_GPIO_WriteBit( 12, HI_TRUE );
	AVL63X1_IBSP_Delay(10);
	res |= HI_UNF_GPIO_WriteBit( 12, HI_FALSE );
	AVL63X1_IBSP_Delay(100);
	res |= HI_UNF_GPIO_WriteBit( 12, HI_TRUE );
	AVL63X1_IBSP_Delay(10);

	HI_UNF_GPIO_Close();
		#endif /* #if 0 */
#endif		
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
#if 0
	for ( iI = 0 ; iI < AVL_MAX_SEMAPHORES ; iI++ )
	{
		if ( AVL_sem_table[iI].free == 0 )
		{
		    sem_destroy( &(AVL_sem_table[iI].id) );
		}
	}
#endif	
	return(AVL63X1_EC_OK);
}