/*
  Copyright (C) 2006-2009 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          main.c
 *
 *                3
 *
 * \date          %modify_time%
 *
 * \brief         Sample application code of the NXP TDA18260 driver.
 *
 * REFERENCE DOCUMENTS :
 *                TDA18250_Driver_User_Guide.pdf
 *
 * Detailed description may be added here.
 *
 * \section info Change Information
 *
*/
/*----------------------------------------------------------------------------*/
/* Include Standard files                                                     */
/*----------------------------------------------------------------------------*/
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "semaphore.h"
#include <sys/time.h>
#include "linux/i2c.h"
#include "fcntl.h"
#include "errno.h"
#include "stdarg.h"
#include "pbitrace.h"
#include "osapi.h"

#include "tmNxTypes.h"
#include "tmCompId.h"
#include "tmFrontEnd.h"
#include "tmbslFrontEndTypes.h"
#include "tmsysFrontEndTypes.h"
#include "tmUnitParams.h"
//#include "tmsysScanXpress.h"
#include "tmbslOM3971Main.h"
/*----------------------------------------------------------------------------*/
/* Include Driver files                                                       */
/*----------------------------------------------------------------------------*/
#include "tmsysOM3971.h"
#include "tmsysOM3971local.h"
#include "tmsysOM3971Instance.h"
/*----------------------------------------------------------------------------*/
/* Prototype of function to be provided by customer                           */
/*----------------------------------------------------------------------------*/
tmErrorCode_t     UserWrittenI2CRead(tmUnitSelect_t tUnit,UInt32 AddrSize, UInt8* pAddr,UInt32 ReadLen, UInt8* pData);
tmErrorCode_t     UserWrittenI2CWrite (tmUnitSelect_t tUnit, UInt32 AddrSize, UInt8* pAddr,UInt32 WriteLen, UInt8* pData);
tmErrorCode_t     UserWrittenWait(tmUnitSelect_t tUnit, UInt32 tms);
tmErrorCode_t     UserWrittenPrint(UInt32 level, const char* format, ...);
tmErrorCode_t     UserWrittenMutexInit(ptmbslFrontEndMutexHandle *ppMutexHandle);
tmErrorCode_t     UserWrittenMutexDeInit( ptmbslFrontEndMutexHandle pMutex);
tmErrorCode_t     UserWrittenMutexAcquire(ptmbslFrontEndMutexHandle pMutex, UInt32 timeOut);
tmErrorCode_t     UserWrittenMutexRelease(ptmbslFrontEndMutexHandle pMutex);


/* BEGIN: Added by zhwu, 2013/1/10 */
#define I2C_BUFFER_SIZE 	127
#define DeviceTDA18250Addr 	0xC0
#define DeviceTDA10025Addr 	0x1E
#define iOM3971_FAILED(x)          (x>0)
#define iOM3971_FUCHECK(err)         if(iOM3971_FAILED(err)) { 									\
										pbierror("FUN:%s,LINE:%d,ErrCode:0x%X \n ",__FUNCTION__,	\
											__LINE__,err);										\
											return err; } 
#define OM3971_MAX_SEMAPHORES 10

typedef struct OM3971_SEM_RECORD_T 
{
	Int32 free;
    sem_t id;
}OM3971_SEM_RECORD_T_STRU;

struct i2c_rdwr_ioctl_data {
	struct i2c_msg __user *msgs;	/* pointers to i2c_msgs */
	__u32 nmsgs;			/* number of i2c_msgs */
};

struct OM3971_SEM_RECORD_T OM3971_sem_table[OM3971_MAX_SEMAPHORES];

/* END:   Added by zhwu, 2013/1/10 */

/*----------------------------------------------------------------------------*/
/* Function Name       : Main                                                 */
/* Object              : Software entry point                                 */
/* Input Parameters    : none.                                                */
/* Output Parameters   : none.                                                */
/*----------------------------------------------------------------------------*/
int OM3971_Open(void)
{
    /* Variable declarations */
    tmErrorCode_t err = TM_OK;
    tmbslFrontEndDependency_t sSrvTunerFunc;
    //tmDVBCRequest_t tuneRequestDVBC;
    //tmsysFrontEndState_t  LockStatus;
    tmUnitSelect_t  sysUnit= 0;
	Int8 iI = 0;
	
	/* Hardware reset */
	system("echo w:d:8:1 > /sys/class/gpio/cmd ");
	usleep(10*1000);
	system("echo w:d:8:0 > /sys/class/gpio/cmd ");
	usleep(300*1000);
	system("echo w:d:8:1 > /sys/class/gpio/cmd ");
	usleep(10*1000);

	/* Makesure all sem can be used */
	for ( iI = 0 ; iI < OM3971_MAX_SEMAPHORES; iI++ )
	{
	    OM3971_sem_table[iI].free = 1;
	}

	/* Low layer struct set-up to link with user written functions */
    sSrvTunerFunc.sIo.Write             = UserWrittenI2CWrite;
    sSrvTunerFunc.sIo.Read              = UserWrittenI2CRead;
    sSrvTunerFunc.sTime.Get             = Null;
    sSrvTunerFunc.sTime.Wait            = UserWrittenWait;
    sSrvTunerFunc.sDebug.Print          = UserWrittenPrint;
    sSrvTunerFunc.sMutex.Init           = UserWrittenMutexInit;
    sSrvTunerFunc.sMutex.DeInit         = UserWrittenMutexDeInit;
    sSrvTunerFunc.sMutex.Acquire        = UserWrittenMutexAcquire;
    sSrvTunerFunc.sMutex.Release        = UserWrittenMutexRelease;
    sSrvTunerFunc.dwAdditionalDataSize  = 0;
    sSrvTunerFunc.pAdditionalData       = Null;
    
   /* system init : TDA18260 + TDA10025 */
   err = tmsysOM3971Init(sysUnit, &sSrvTunerFunc);
   iOM3971_FUCHECK(err);

   /* Hardware initialization */
   err = tmsysOM3971Reset(sysUnit);
   iOM3971_FUCHECK(err);
   
   /* Hardware power state */
   err = tmsysOM3971SetPowerState(sysUnit, tmsysFrontEndBlockStream, tmPowerOn);
   iOM3971_FUCHECK(err);

    return err;
}
/*----------------------------------------------------------------------------*/
/* Template of function to be provided by customer                            */
/*----------------------------------------------------------------------------*/
/*****************************************************************************
 函 数 名  : OM3971_SetFrequence
 功能描述  : OM3971锁频
 输入参数  : UInt32 Frequence   
             UInt32 SymbolRate  
             UInt16 Modulation  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月11日
    作    者   : zhwu
    修改内容   : 新生成函数

  2.日    期   : 2013年3月8日
    作    者   : zhwu
    修改内容   : 添加带宽参数
    			 UInt8 BandWidth
*****************************************************************************/
tmErrorCode_t OM3971_SetFrequence( UInt32 Frequence, UInt32 SymbolRate, UInt16 Modulation, UInt8 BandWidth )
{
	tmErrorCode_t err = TM_OK;
	tmUnitSelect_t  sysUnit= 0;
	tmDVBCRequest_t tuneRequestDVBC;
	
	tuneRequestDVBC.eFrontEndLock = tmsysFrontEndStateUnknown;
	tuneRequestDVBC.dwSpectralInversion = tmFrontEndSpecInvAuto;
	tuneRequestDVBC.dwFrequency = Frequence * 1000;
	tuneRequestDVBC.dwSymbolRate = SymbolRate * 1000;
	switch ( Modulation )
	{
	    case 5:	/* ePI_16QAM */
	        tuneRequestDVBC.dwModulationType = tmFrontEndModulationQam16;
	        break;
	    case 6: /* ePI_32QAM */
	        tuneRequestDVBC.dwModulationType = tmFrontEndModulationQam32;
	        break;
	    case 7: /* ePI_64QAM */
	        tuneRequestDVBC.dwModulationType = tmFrontEndModulationQam64;
	        break;
	    case 8: /* ePI_128QAM */
	        tuneRequestDVBC.dwModulationType = tmFrontEndModulationQam128;
	        break;
		case 9: /* ePI_256QAM */
		    tuneRequestDVBC.dwModulationType = tmFrontEndModulationQam256;
		    break;
	    default:
	        tuneRequestDVBC.dwModulationType = tmFrontEndModulationQam64;
	}
	switch ( BandWidth )
	{
	    case 0 : /* ePI_BW_8MHZ */
	        tuneRequestDVBC.dwBandWidth = 0;
	        break;
	    case 2 : /* ePI_BW_6MHZ */
	    default: /* 传递错误带宽参数，按照6M带宽设置 */
	        tuneRequestDVBC.dwBandWidth = 1;
	}
	
	err = tmsysOM3971SendRequest( sysUnit, &tuneRequestDVBC, sizeof(tuneRequestDVBC), TRT_DVBC );
	iOM3971_FUCHECK(err);
	
	return err;
}
/*****************************************************************************
 函 数 名  : OM3971_GetLockStatus
 功能描述  : 信号锁定状态
 输入参数  : UInt8 *LStatus  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月11日
    作    者   : zhwu
    修改内容   : 新生成函数

*****************************************************************************/
tmErrorCode_t OM3971_GetLockStatus( UInt8 *LStatus )
{
	tmErrorCode_t err = TM_OK;
	tmUnitSelect_t  sysUnit= 0;
	tmsysFrontEndState_t  LockStatus;
	
	err = tmsysOM3971GetLockStatus( sysUnit,  &LockStatus );
	*LStatus = LockStatus;
	
	return err;
}
/*****************************************************************************
 函 数 名  : OM3971_GetSignalQuality
 功能描述  : 信号质量
 输入参数  : UInt32 *Quality  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月14日
    作    者   : zhwu
    修改内容   : 新生成函数

*****************************************************************************/
tmErrorCode_t OM3971_GetSignalQuality( UInt32 *Quality )
{
	tmErrorCode_t err = TM_OK;
	tmUnitSelect_t  sysUnit= 0;
	UInt32 pQuality = 0;
	Int8 iI = 0;
	
	for ( iI = 0 ; iI < 2 ; iI++ )
	{
	    err = tmsysOM3971GetSignalQuality( sysUnit, &pQuality );
		iOM3971_FUCHECK(err);
	}
	
	*Quality = pQuality;
		
	return err;
}
/*****************************************************************************
 函 数 名  : OM3971_GetSignalStrength
 功能描述  : 信号强度
 输入参数  : Int32 *Strength  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月14日
    作    者   : zhwu
    修改内容   : 新生成函数

*****************************************************************************/
tmErrorCode_t OM3971_GetSignalStrength( UInt32 *Strength )
{
	tmErrorCode_t err = TM_OK;
	tmUnitSelect_t  sysUnit= 0;
	Int32 pStrength = 0;
	/* BEGIN: Added by zhwu, 2013/1/25 */
	*Strength = 0;/* no signal return error! */
	/* END:   Added by zhwu, 2013/1/25 */
	err = tmsysOM3971GetSignalStrength( sysUnit, &pStrength );
	iOM3971_FUCHECK(err);
	if ( pStrength < -1600 )
	{
	    *Strength = 0;
	}
	pStrength = pStrength * (-1);
	pStrength += 1600; 
	if ( pStrength > 5600 )
	{
	    *Strength = 100;
	}
	if ( pStrength > 2800 )
	{
	    if ( pStrength > 4200 )
	    {
	        *Strength = 88;
	    }
		else
		{
		    *Strength = 63;
		}
	}
	else
	{
	    if ( pStrength > 1400 )
	    {
	        *Strength = 38;
	    }
		else
			*Strength = 13;
	}

	return err;
}
/*****************************************************************************
 函 数 名  : OM3971_GetSignalBer
 功能描述  : 误码率
 输入参数  : Int32 *Integer   
             UInt32 *Divider  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月14日
    作    者   : zhwu
    修改内容   : 新生成函数

*****************************************************************************/
tmErrorCode_t OM3971_GetSignalBer( Int32 *Integer, UInt32 *Divider )
{
	tmErrorCode_t err = TM_OK;
	tmUnitSelect_t  sysUnit= 0;
	tmFrontEndFracNb32_t pBER;
	UInt32 pUncors;

	err = tmsysOM3971GetBER( sysUnit, TRT_DVBC, &pBER, &pUncors );
	iOM3971_FUCHECK(err);

	*Integer = pBER.lInteger;
	*Divider = pBER.uDivider;

	return err;
}
/*****************************************************************************
 函 数 名  : OM3971_GetSignalSnr
 功能描述  : 信噪比
 输入参数  : Int32 *Integer   
             UInt32 *Divider  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月14日
    作    者   : zhwu
    修改内容   : 新生成函数

*****************************************************************************/
tmErrorCode_t OM3971_GetSignalSnr( Int32 *Integer, UInt32 *Divider )
{
	tmErrorCode_t err = TM_OK;
	tmUnitSelect_t  sysUnit= 0;
	tmFrontEndFracNb32_t pEsno;

	err = tmsysOM3971GetSNR(sysUnit, &pEsno);
	iOM3971_FUCHECK(err);
	*Integer = pEsno.lInteger;
	*Divider = pEsno.uDivider;
	
	return err;
}
/*****************************************************************************
 函 数 名  : OM3971_Close
 功能描述  : 去初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月15日
    作    者   : zhwu
    修改内容   : 新生成函数

*****************************************************************************/
tmErrorCode_t OM3971_Close( void )
{
    tmErrorCode_t err = TM_OK;
    tmUnitSelect_t  sysUnit= 0;
	
    err = tmsysOM3971DeInit(sysUnit);
    iOM3971_FUCHECK(err);

	return err;
}
/*----------------------------------------------------------------------------*/
/* Function Name       : UserWrittenI2CRead                                   */
/* Object              :                                                      */
/* Input Parameters    : tmUnitSelect_t tUnit                                 */
/*                          UInt32 AddrSize,                                  */
/*                          UInt8* pAddr,                                     */
/*                          UInt32 ReadLen,                                   */
/*                          UInt8* pData                                      */
/* Output Parameters   : tmErrorCode_t.                                       */
/*----------------------------------------------------------------------------*/
tmErrorCode_t UserWrittenI2CRead
				(
					tmUnitSelect_t tUnit,    
					UInt32 AddrSize, 
					UInt8* pAddr,
					UInt32 ReadLen, 
					UInt8* pData
				)
{
   /* Variable declarations */
   tmErrorCode_t err = TM_OK;

/* Customer code here */
/* ...*/
    ptmOM3971Object_t  				pObj = Null;
	tmErrorCode_t      				errI2C = TM_OK;
	UInt8							i = 0;
    UInt8              				uBytesBuffer[I2C_BUFFER_SIZE];
    UInt8              				uSubAddress16[2] = {0,0};
	tmUnitSelect_t     				tUnitOM3971 = GET_SYSTEM_TUNIT(tUnit);
	struct i2c_msg 					msgs[2];
	struct i2c_rdwr_ioctl_data 		ioctl_data; 

    err = OM3971GetInstance(tUnitOM3971, &pObj);
    tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3949GetInstance(0x%08X) failed.\n", tUnit));
	if(err == TM_OK)
	{
		switch ( AddrSize )
		{
		    case 1 :
		        uSubAddress16[0] = *pAddr&0xFF;
		        break;
		    case 2 :
		        uSubAddress16[0] = pAddr[1];
				uSubAddress16[1] = pAddr[0];
		        break;
		    case 0 :
		    default:
		        uSubAddress16[0] = 0; 
		}

		if ( ReadLen > I2C_BUFFER_SIZE )
		{
		    err = OM3971_ERR_BAD_PARAMETER;
		}
		else
		{
			switch(UNIT_PATH_TYPE_GET(tUnit))
			{
				case tmOM3971UnitDeviceTypeTDA18250:
					//errI2C = HI_UNF_I2C_Read( 0, DeviceTDA18250Addr, uSubAddress, AddrSize, uBytesBuffer, ReadLen );
					errI2C = DRV_I2C_Read( DeviceTDA18250Addr, uSubAddress16, AddrSize, uBytesBuffer, ReadLen);
				break;

				case tmOM3971UnitDeviceTypeTDA10025:
					//errI2C = HI_UNF_I2C_Read( 0, DeviceTDA10025Addr, uSubAddress16, AddrSize, uBytesBuffer, ReadLen );
					errI2C = DRV_I2C_Read( DeviceTDA10025Addr, uSubAddress16, AddrSize, uBytesBuffer, ReadLen);			
				break;
				default:
					tmDBGPRINTEx(DEBUGLVL_ERROR, "tmsysOM3949I2CRead(%08X) failed. Unknown Device Type.", tUnit);
					err = OM3971_ERR_BAD_PARAMETER;
				break;
			}
		}
	    if(errI2C == 0 && err == TM_OK)
	    {
	        switch(AddrSize)
	        {
		        default:
		        case 0:
		        case 1:						
		            for ( i = 0; i < ReadLen ; i++ ) 
		            {
		                *(pData + i) = (UInt8)uBytesBuffer[i];
		            }
		            break;

		        case 2:
		            for ( i= 0 ; i < ReadLen ; i += 2 ) 
		            {
		                *(pData + i) = (UInt8)uBytesBuffer[i+1];
		                *(pData + i + 1) = (UInt8)uBytesBuffer[i];
		            }
		            break;
	        }
	    }
	    else
	    {
	        tmDBGPRINTEx(DEBUGLVL_ERROR, "tmsysOM3949I2CRead (0x%08X, [sub@0x%02x] %d) failed.", tUnit, (uSubAddress16[1] << 8)|uSubAddress16[0], ReadLen);
	        err = OM3971_ERR_HW;
	    }
	}
/* ...*/
/* End of Customer code here */

   return err;
}

/*---------------------------------------------------------------------------*/
/* Function Name       : UserWrittenI2CWrite                                 */
/* Object              :                                                     */
/* Input Parameters    : tmUnitSelect_t tUnit                                */
/*                          UInt32 AddrSize,                                 */
/*                          UInt8* pAddr,                                    */
/*                          UInt32 WriteLen,                                 */
/*                          UInt8* pData                                     */
/* Output Parameters   : tmErrorCode_t.                                      */
/*---------------------------------------------------------------------------*/
tmErrorCode_t UserWrittenI2CWrite (tmUnitSelect_t tUnit,     UInt32 AddrSize, UInt8* pAddr,
UInt32 WriteLen, UInt8* pData)
{
   /* Variable declarations */
   tmErrorCode_t err = TM_OK;

/* Customer code here */
/* ...*/
	ptmOM3971Object_t  pObj = Null;
    Int      errI2C = 0;
    //Bool               bIncSubAddress = True;
    UInt8              uSubAddress = 0;
    UInt16             uSubAddress16 = 0;
    UInt32             uCounter = 0;
    UInt8              WriteBuffer[I2C_BUFFER_SIZE+1] = {0};
    UInt32             count = 0;
    UInt32             remain = 0;
    tmUnitSelect_t     tUnitOM3949 = GET_SYSTEM_TUNIT(tUnit);
	struct i2c_msg 					msgs[2];
	struct i2c_rdwr_ioctl_data 		ioctl_data;

    if(GET_XFER_DISABLED_FLAG_TUNIT(tUnit) == False)
    {
        err = OM3971GetInstance(tUnitOM3949, &pObj);
        tmASSERTExT(err, TM_OK, (DEBUGLVL_ERROR, "OM3971GetInstance(0x%08X) failed.", tUnit));

        if(err == TM_OK)
        {
            //if(pObj->uHwAddress[UNIT_PATH_TYPE_GET(tUnit)] != OM3949_INVALID_DEVICE_ADDRESS)
            {
	            switch(AddrSize)
	            {
		            default:
		            case 0:
		                uSubAddress = 0;
		                break;
		            case 1:
		                //uSubAddress = (*pAddr)&0xFF;//pAddr[0];
		                //WriteBuffer[0] = (*pAddr)&0xFF;
		                break;
		            case 2:
		                //uSubAddress16 = pAddr[1]<<8 | pAddr[0];
		                WriteBuffer[0] = pAddr[1];
						WriteBuffer[1] = pAddr[0];
		                break;
            	}
                for(count=0; count<WriteLen; count += remain)
                {
                    //remain = min(WriteLen-count, I2C_BUFFER_SIZE);
					if ( (WriteLen - count) > 127 )
					    remain = 127;
					else
						remain = WriteLen - count;
                    switch(AddrSize)
                    {
	                    default:
	                    case 0:
	                    case 1:
	                        /* copy I2CMap data in WriteBuffer */
	                        for (uCounter = 0; uCounter < remain; uCounter++)
	                        {
	                            WriteBuffer[uCounter] = pData[count+uCounter];
	                        }
	                        break;
	                    case 2:
	                        /* copy I2CMap data in WriteBuffer */
	                        for (uCounter = 0; uCounter < remain; uCounter+=2)
	                        {
	                            WriteBuffer[uCounter+2] = pData[count+uCounter+1];
	                            WriteBuffer[uCounter+3] = pData[count+uCounter];
	                        }
	                        break;
                    }

                    switch(UNIT_PATH_TYPE_GET(tUnit))
                    {
                        case tmOM3971UnitDeviceTypeTDA18250:
							errI2C = DRV_I2C_Write( DeviceTDA18250Addr, pAddr, AddrSize, WriteBuffer, remain);
                            break;

                        case tmOM3971UnitDeviceTypeTDA10025:							
							errI2C = DRV_I2C_Write( DeviceTDA10025Addr, pAddr, 0, WriteBuffer, (remain + AddrSize));
                            break;
                        default:
                            tmDBGPRINTEx(DEBUGLVL_ERROR, "tmsysOM3971I2CWrite(0x%08X) failed. Unknown Device Type.", tUnit);
                            err = OM3971_ERR_BAD_PARAMETER;
                            break;
                    }

                    if( errI2C != 0 )
                    {
                        tmDBGPRINTEx(DEBUGLVL_ERROR, "tmsysOM3949I2CWrite(0x%08X, [sub@0x%02x] %d/%d) failed.", tUnit, (pAddr[1] << 8)|pAddr[0], count, WriteLen);
                        err = OM3971_ERR_HW;
                    }

                    if(err != TM_OK)
                        break;
                    #if  0
                    if(bIncSubAddress)
                    {
                        uSubAddress = (UInt8)(uSubAddress+remain);
                        uSubAddress16 = (UInt16)(uSubAddress16+remain);
                    }
					#endif /* #if 0 */
                }
            }
        }
    }
/* ...*/
/* End of Customer code here */

   return err;
}

/*----------------------------------------------------------------------------*/
/* Function Name       : UserWrittenWait                                      */
/* Object              :                                                      */
/* Input Parameters    : tmUnitSelect_t tUnit                                 */
/*                          UInt32 tms                                        */
/* Output Parameters   : tmErrorCode_t.                                       */
/*----------------------------------------------------------------------------*/
tmErrorCode_t UserWrittenWait(tmUnitSelect_t tUnit, UInt32 tms)
{
   /* Variable declarations */
   tmErrorCode_t err = TM_OK;

/* Customer code here */
/* ...*/
	usleep( tms * 1000 );
/* ...*/
/* End of Customer code here */

   return err;
}

/*----------------------------------------------------------------------------*/
/* Function Name       : UserWrittenPrint                                     */
/* Object              :                                                      */
/* Input Parameters    : UInt32 level, const char* format, ...                */
/*                                                                            */
/* Output Parameters   : tmErrorCode_t.                                       */
/*----------------------------------------------------------------------------*/
tmErrorCode_t             UserWrittenPrint(UInt32 level, const char* format, ...)
{
   /* Variable declarations */
   tmErrorCode_t err = TM_OK;
   
/* Customer code here */
/* ...*/
	va_list args;
	
	va_start(args,format);
	vprintf(format, args);
	va_end(args);
/* ...*/
/* End of Customer code here */

   return err;
}

/*----------------------------------------------------------------------------*/
/* Function Name       : UserWrittenMutexInit                                 */
/* Object              :                                                      */
/* Input Parameters    : ptmbslFrontEndMutexHandle *ppMutexHandle             */
/* Output Parameters   : tmErrorCode_t.                                       */
/*----------------------------------------------------------------------------*/
tmErrorCode_t  UserWrittenMutexInit(ptmbslFrontEndMutexHandle *ppMutexHandle)
{
   /* Variable declarations */
   int err = TM_OK;

/* Customer code here */
/* ...*/
	Int32 iI = 0;
	for ( iI = 0 ; iI < OM3971_MAX_SEMAPHORES; iI++ )
	{
	    if ( OM3971_sem_table[iI].free == 1 )
	    {
	        break;
	    }
	}
	/* It isn't seem  happen any more */
    if((iI >= OM3971_MAX_SEMAPHORES) || ( OM3971_sem_table[iI].free != 1 ))
        return OM3971_ERR_NOT_SUPPORTED;
	
	err = sem_init( &(OM3971_sem_table[iI].id), 0, 1 );
	iOM3971_FUCHECK(err);

	OM3971_sem_table[iI].free = 0;
	
	*ppMutexHandle = iI;
/* ...*/
/* End of Customer code here */

   return TM_OK;
}


/*----------------------------------------------------------------------------*/
/* Function Name       : UserWrittenMutexDeInit                               */
/* Object              :                                                      */
/* Input Parameters    : ptmbslFrontEndMutexHandle pMutex                     */
/* Output Parameters   : tmErrorCode_t.                                       */
/*----------------------------------------------------------------------------*/
tmErrorCode_t  UserWrittenMutexDeInit( ptmbslFrontEndMutexHandle pMutex)
{
   /* Variable declarations */
   int err = TM_OK;

/* Customer code here */
/* ...*/
	Int32 iI = 0;

	for ( iI = 0 ; iI < OM3971_MAX_SEMAPHORES ; iI++ )
	{
		if ( OM3971_sem_table[iI].free == 0 )
		{
		    err = sem_destroy( &(OM3971_sem_table[iI].id) );
			iOM3971_FUCHECK(err);
		}
	}
/* ...*/
/* End of Customer code here */

   return TM_OK;
}



/*----------------------------------------------------------------------------*/
/* Function Name       : UserWrittenMutexAcquire                              */
/* Object              :                                                      */
/* Input Parameters    : ptmbslFrontEndMutexHandle pMutex, UInt32 timeOut     */
/* Output Parameters   : tmErrorCode_t.                                       */
/*----------------------------------------------------------------------------*/
tmErrorCode_t  UserWrittenMutexAcquire(ptmbslFrontEndMutexHandle pMutex, UInt32 timeOut)
{
   /* Variable declarations */
	int err = TM_OK;
   	Int32 iI = 0;
	struct timeval tv;
	struct timespec abs_timeout;
/* Customer code here */
/* ...*/
	iI = pMutex;
	if( ( OM3971_sem_table[iI].free == 1) || ( iI >= OM3971_MAX_SEMAPHORES ))
    	return OM3971_ERR_NOT_SUPPORTED;
    

    gettimeofday(&tv, NULL);
    abs_timeout.tv_sec = tv.tv_sec;
    abs_timeout.tv_nsec = tv.tv_usec * 1000;

    /* add the delay to the current time */
    abs_timeout.tv_sec  += (time_t) (timeOut / 1000) ;
    /* convert residue ( milli seconds)  to nano second */
    abs_timeout.tv_nsec +=  (timeOut % 1000) * 1000000 ;

    if(abs_timeout.tv_nsec > 999999999 )
    {
        abs_timeout.tv_nsec -= 1000000000 ;
        abs_timeout.tv_sec ++ ;
    }
	
	err = sem_timedwait( &OM3971_sem_table[iI].id, &abs_timeout );
	iOM3971_FUCHECK(err);
/* ...*/
/* End of Customer code here */

   return TM_OK;
}

/*----------------------------------------------------------------------------*/
/* Function Name       : UserWrittenMutexRelease                              */
/* Object              :                                                      */
/* Input Parameters    : ptmbslFrontEndMutexHandle pMutex                     */
/* Output Parameters   : tmErrorCode_t.                                       */
/*----------------------------------------------------------------------------*/
tmErrorCode_t  UserWrittenMutexRelease(ptmbslFrontEndMutexHandle pMutex)
{
   /* Variable declarations */
   tmErrorCode_t err = TM_OK;
   Int32 iI = 0;
/* Customer code here */
/* ...*/

	iI = pMutex;
    if(( iI>= OM3971_MAX_SEMAPHORES ) || (OM3971_sem_table[iI].free == 1 ))
        return OM3971_ERR_NOT_SUPPORTED;
	
	sem_post( &OM3971_sem_table[iI].id );

/* ...*/
/* End of Customer code here */

   return err;
}


