/******************************************************************************
* FILENAME: $Id: bsp_i2c.c,v 1.3 2009/05/20 08:56:01 carlo Exp $
*
* DESCRIPTION:
* Includes a Trident-provided I2C implementation.
* Also serves as the template for a target-specific I2C implementation.
*
* USAGE:
* Outcomment and fill-out the template implementation
* OR include target-specific I2C source files, surrounded by a compiler flag.
*
* NOTES:
* $(c) 2004-2009 Trident Microsystems, Inc. - All rights reserved.
*
* This software and related documentation (the 'Software') are intellectual
* property owned by Trident and are copyright of Trident, unless specifically
* noted otherwise.
*
* Any use of the Software is permitted only pursuant to the terms of the
* license agreement, if any, which accompanies, is included with or applicable
* to the Software ('License Agreement') or upon express written consent of
* Trident. Any copying, reproduction or redistribution of the Software in
* whole or in part by any means not in accordance with the License Agreement
* or as agreed in writing by Trident is expressly prohibited.
*
* THE SOFTWARE IS WARRANTED, IF AT ALL, ONLY ACCORDING TO THE TERMS OF THE
* LICENSE AGREEMENT. EXCEPT AS WARRANTED IN THE LICENSE AGREEMENT THE SOFTWARE
* IS DELIVERED 'AS IS' AND TRIDENT HEREBY DISCLAIMS ALL WARRANTIES AND
* CONDITIONS WITH REGARD TO THE SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
* AND CONDITIONS OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIT
* ENJOYMENT, TITLE AND NON-INFRINGEMENT OF ANY THIRD PARTY INTELLECTUAL
* PROPERTY OR OTHER RIGHTS WHICH MAY RESULT FROM THE USE OR THE INABILITY
* TO USE THE SOFTWARE.
*
* IN NO EVENT SHALL TRIDENT BE LIABLE FOR INDIRECT, INCIDENTAL, CONSEQUENTIAL,
* PUNITIVE, SPECIAL OR OTHER DAMAGES WHATSOEVER INCLUDING WITHOUT LIMITATION,
* DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS
* INFORMATION, AND THE LIKE, ARISING OUT OF OR RELATING TO THE USE OF OR THE
* INABILITY TO USE THE SOFTWARE, EVEN IF TRIDENT HAS BEEN ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGES, EXCEPT PERSONAL INJURY OR DEATH RESULTING FROM
* TRIDENT'S NEGLIGENCE.                                                        $
*
******************************************************************************/

/******************************************************************************
*
* If no BSP_I2C implementation is selected, select a default
*
*****************************************************************************/
/******************************************************************************
*
* Below, a template is provided for a custom BSP_I2C implementation
*
*****************************************************************************/


#if 1

/*
* To create a custom BSP_I2C implementation:
*
* - Disable the default (Trident-supplied) I2C interface,
*    e.g. by removing (or out-commenting) all lines above this section.
*
* - Also remove (or out-comment) the '#if 0' (above)
*     and the matching '#endif'(at the end of this file).
*
* - Implement the BSP_I2C functions for your target system.
*    Empty function bodies and brief explanations of the required
*    functionality are included below. Please refer to Driver Manual for
*    further details of these functions.
*
*/

/* Necessary includes go here */
#include "stdio.h"
#include "stdlib.h"
#include "pbitrace.h"
#include "./include/bsp_i2c.h"

#ifdef STPLANTFORM
#include "drv_i2c.h"
#endif /* STPLANTFORM */

#ifdef HISIPLANTFORM
#include "hi_type.h"
#include "hi_unf_ecs.h"
#endif /* HISIPLANTFORM */

#include "unistd.h"
#include "errno.h"
#include "osapi.h"

/* Store errors from your I2C subsystem as-is in this variable */
int DRX_I2C_Error_g;

#ifdef STPLANTFORM
static unsigned int  I2C_DRXHandle_D=0;
static unsigned int  I2C_DRXHandle_T=0;
u32_t	Handle_RW = 0;
#endif /* STPLANTFORM */

/* BEGIN: Added by zhwu, 2013/2/21 */
#define TUNI2C_DEBUG

#ifdef TUNI2C_DEBUG
#define CHECK_I2C(res) \
		do{ \
            if ( (res) > 0 ) \
            { \
               pbierror( \
                        "ERROR[\n file        : %s\n function    : %s\n line        : %d\n errcode     : %d,%s\n]\n ",\
                        __FILE__,__FUNCTION__,__LINE__,errno,strerror(errno)); \
               goto i2c_error; }; \
            } \
        while (0 != 0)
#else
#define CHECK_I2C(res) \
		do{ \
            if ( (res) > 0 ) \
            { \
               fprintf( stderr, \
                        "ERROR[\n file        : %s\n function    : %s\n line        : %d\n errcode     : %d,%s\n]\n ",\
                        __FILE__,__FUNCTION__,__LINE__,errno,strerror(errno)); \
               goto i2c_error; }; \
            } \
        while (0 != 0)
#endif /* TUNI2C_DEBUG */
/* END:   Added by zhwu, 2013/2/21 */
/******************************
*
* DRXStatus_t DRXBSP_I2C_Init (void)
*
* Prepare the platform's I2C subsystem for usage.
* Should be called before any other I2C function is called.
*
* Output:
* - DRX_STS_OK    system is now ready for use
* - other         something wrong
*
******************************/
DRXStatus_t DRXBSP_I2C_Init( unsigned char Tuner_ID )
{
   /* TODO: initialize your I2C subsystem */
#if defined(STPLANTFORM)
	unsigned char				Error = 0;
	I2C_OpenParams_t		I2cOpenParams;
	memset(&I2cOpenParams, '\0', sizeof( I2C_OpenParams_t ) );
	/* Initialize the I2C interface for DEMOD*/
	I2cOpenParams.BusAccessTimeOut= 5000;
	I2cOpenParams.AddrType= eI2C_ADDRESS_7_BITS;
	I2cOpenParams.I2cAddr=DRX_DEMOD_ADDR;
	Error = DRV_I2C_Open(/*DRV_ADP_TUNER_I2cIndex(Tuner_ID)*/1, I2cOpenParams, &I2C_DRXHandle_D);
	if ( 0 !=  Error)
	{
		pbierror("NCB : Failed to open I2C access for DRX\n");
		//( void ) HAL_I2C_Close( &I2C_DRXHandle_D );
		return DRX_STS_ERROR;
	}
	else
	{
		pbiinfo("demod DRX i2c open successful\n");
	}

	/* Initialize the I2C interface for Tuenr*/
	I2cOpenParams.BusAccessTimeOut= 5000;
	I2cOpenParams.AddrType= eI2C_ADDRESS_7_BITS;
	I2cOpenParams.I2cAddr= DRX_TUNER_ADDR;
	Error = DRV_I2C_Open(/*DRV_ADP_TUNER_I2cIndex(Tuner_ID)*/1, I2cOpenParams, &I2C_DRXHandle_T);
	if (  0 !=  Error )
	{
		pbierror("NCB : Failed to open I2C access for TUENR\n");
		( void ) DRV_I2C_Close( &I2C_DRXHandle_D ); /*modify by hujun 2010.05.25*/
		return DRX_STS_ERROR;
	}
	else
	{
		pbiinfo("tuner DHF_84000_K i2c open successful\n");
	}
#endif /* #if define STPLANTFORM */

	DRV_I2C_Open();
	
	system("echo w:d:8:1 > /sys/class/gpio/cmd ");
	usleep(10*1000);
	system("echo w:d:8:0 > /sys/class/gpio/cmd ");
	usleep(300*1000);
	system("echo w:d:8:1 > /sys/class/gpio/cmd ");
	usleep(10*1000);

   	return DRX_STS_OK;
}


/******************************
*
* DRXStatus_t DRXBSP_I2C_Term (void)
*
* Shut down the I2C subsystem of the target platform.
* After this, I2C functions should no longer be called, until the system is
* initialized again.
*
* Output:
* - DRX_STS_OK     I2C subsystem shut down
* - other          something wrong
*
******************************/

DRXStatus_t DRXBSP_I2C_Term( void )
{
	/* TODO: terminate your I2C subsystem */

	return DRX_STS_OK;
}


/******************************
*
* DRXStatus_t DRXBSP_I2C_WriteRead(
*             pI2CDeviceAddr_t wDevAddr,     -- Device to write to
*             u16_t            wCount,       -- nr of bytes to write
*             pu8_t            wData,        -- the data to write
*             pI2CDeviceAddr_t rDevAddr,     -- Device to read from
*             u16_t            rCount,       -- nr of bytes to read
*             pu8_t            rData     )   -- buffer receiving the data
*
* Write data to I2C device, then receive back specified number of bytes.
* Must also support write-only (when rDevAddr is NULL)
*               and read-only  (when wDevAddr is NULL).
*
* Output:
* - DRX_STS_OK           data successfully transferred
*                        in that case: data received, if any, is in rData
* - DRX_STS_INVALID_ARG  invalid arguments passed to this function
* - DRX_STS_ERROR        something wrong in the transfer
*
******************************/

DRXStatus_t DRXBSP_I2C_WriteRead(   pI2CDeviceAddr_t wDevAddr,
                                    u16_t            wCount,
                                    pu8_t            wData,
                                    pI2CDeviceAddr_t rDevAddr,
                                    u16_t            rCount,
                                    pu8_t            rData    )
{
#if defined(STPLANTFORM)
	u8_t		ucError=0;
	u8_t		ucRetry=0;
	u32_t	uiAct_length=0;
	if (!wDevAddr && !rDevAddr)
	{
			 /* Nothing to do! */
			return DRX_STS_INVALID_ARG;
	}
	if (wDevAddr)
	{
		if (!wData && wCount)
		{
			/* Buffer error! */
			return DRX_STS_INVALID_ARG;
		}
			/* TODO: prepare for write (e.g. set-up data structures) */
	}
   	if (rDevAddr)
   	{
		if (!rData && rCount)
		{
		/* Buffer error! */
			return DRX_STS_INVALID_ARG;
		}
		/* TODO: prepare for read (e.g. set-up data structures) */
   	}
	/* TODO: generate I2C-start signal */
	if(rDevAddr)
	{
		if(DRX_DEMOD_ADDR==rDevAddr->i2cAddr)
		{
			Handle_RW=I2C_DRXHandle_D;	//drxk
		}
		else
		{
			Handle_RW=I2C_DRXHandle_T;	//tuenr
		}	
	}
	else 
	{
		if(DRX_DEMOD_ADDR==wDevAddr->i2cAddr)
		{
			Handle_RW=I2C_DRXHandle_D;	//drxk
		}
		else
		{
			Handle_RW=I2C_DRXHandle_T;	//tuenr
		}	
	}

	DRV_I2C_Lock(Handle_RW);
	 
   	if (wDevAddr)
   	{
		/* TODO: write data */
  		/* Note: if wCount is zero, only the I2C-address must be written */
		while(1)	
		{
			//HAL_I2C_Lock(Handle_RW);
			if (rDevAddr)
			{
				ucError|=DRV_I2C_WriteNoStop(Handle_RW,wData,wCount,5000,&uiAct_length);
			}
			else
			{
				ucError|= DRV_I2C_Write (Handle_RW,wData,wCount,5000,&uiAct_length);
				//HAL_I2C_Unlock(Handle_RW);//if only write
			}
			if ( 0!=ucError )
			{
				if (++ucRetry > 10)
				break;
				printf("drx write address wait!!!\n");
			}
			else
			{
				break;
			}	
		}
   	}

  	 if (rDevAddr)
   	{
      /* TODO: read data */
      /* Note: if rCount is zero, only the I2C-address must be written,
               no data must be read */
		ucRetry=0;
		if(DRX_DEMOD_ADDR==rDevAddr->i2cAddr)
		{
			Handle_RW=I2C_DRXHandle_D;	//drxk
		}
		else
		{
			Handle_RW=I2C_DRXHandle_T;	//tuenr
		}	
		while(1)	
		{

	   		if (!wDevAddr)
   			{	
				//HAL_I2C_Lock(Handle_RW);//if read ,  call HAL_I2C_Lock befor write address
	   		}
			ucError|= DRV_I2C_Read (Handle_RW,rData,rCount,5000,&uiAct_length);
			//HAL_I2C_Unlock(Handle_RW);
		
			if ( 0!=ucError)
			{
				if (++ucRetry > 10)
				break;
				printf("drx read address wait!!!\n");
			}
			else
			{
				break;
			}
		}
  	 }

	DRV_I2C_Unlock(Handle_RW);
	if(0==ucError)
	{
		return DRX_STS_OK;
	}
	else
	{
		return DRX_STS_ERROR;
	}
#endif /* #if define STPLANTFORM */

#ifdef HISIPLANTFORM
	HI_S32 res = 0;

	if (!wDevAddr && !rDevAddr)
	{
		/* Nothing to do! */
		pbierror("FUN:%s,LINE:%d,wDevAddr:0x%X,rDevAddr:0x%X\n",__FUNCTION__,__LINE__,wDevAddr->i2cAddr,rDevAddr->i2cAddr);
		return DRX_STS_INVALID_ARG;
	}
	if (wDevAddr)
	{
		if (!wData && wCount)
		{
			/* Buffer error! */
			pbierror("FUN:%s,LINE:%d,wCount:%d,\n",__FUNCTION__,__LINE__,wCount);
			return DRX_STS_INVALID_ARG;
		}
		
		/* TODO: prepare for write (e.g. set-up data structures) */
		res = HI_UNF_I2C_Write( 0, wDevAddr->i2cAddr, 0, 0, wData, (HI_U32)wCount );
		if ( 0 != res )
		{
		    pbierror("FUN:%s, Write Error! DevAddr:0x%x,DataLength:%d \n",__FUNCTION__,wDevAddr->i2cAddr,wCount);
		}
	}
   	if (rDevAddr)
   	{
		if (!rData && rCount)
		{
			/* Buffer error! */
			pbierror("FUN:%s,LINE:%d,rCount:%d,\n",__FUNCTION__,__LINE__,rCount);
			return DRX_STS_INVALID_ARG;
		}
		//HI_S8 iI = 0;
		HI_U32 rAddr=0x0;
		HI_U32 rLength = 0;//wCount;
		if ( wCount != 0 )
		{
 			rLength = wCount;
			if ( wCount == 2 )
			{
			    rAddr = (wData[0] << 8) + wData[1];
			}
			else if ( wCount == 4 )
			{
			    rAddr = (wData[0]<<24) + (wData[1]<<16) + (wData[2]<<8) + wData[3];
			}
		 }
		
		/* TODO: prepare for read (e.g. set-up data structures) */
		res = HI_UNF_I2C_Read( 0, rDevAddr->i2cAddr, rAddr, rLength, rData, (HI_U32)rCount );
		if ( 0 != res )
		{
		    pbierror("FUN:%s, Read Error! DevAddr:0x%x,DataLength:%d \n",__FUNCTION__,rDevAddr->i2cAddr,rCount);
		}
   	}

	if ( 0 != res )
	    return DRX_STS_ERROR;
#endif /* HISIPLANTFORM */

	if (!wDevAddr && !rDevAddr)
	{
		/* Nothing to do! */
		pbierror("FUN:%s,LINE:%d,wDevAddr:0x%X,rDevAddr:0x%X\n",__FUNCTION__,__LINE__,wDevAddr->i2cAddr,rDevAddr->i2cAddr);
		return DRX_STS_INVALID_ARG;
	}
	if (wDevAddr && !rDevAddr)
	{
		if (!wData && wCount)
		{
			/* Buffer error! */
			pbierror("FUN:%s,LINE:%d,wCount:%d,\n",__FUNCTION__,__LINE__,wCount);
			return DRX_STS_INVALID_ARG;
		}
		/* TODO: prepare for write (e.g. set-up data structures) */
		//printf("wData[0]:0x%X,wData[1]:0x%X,wData[2]:0x%X,wData[3]:0x%X,wCount: %d\n",wData[0],wData[1],wData[2],wData[3],wCount);
		CHECK_I2C(DRV_I2C_Write( wDevAddr->i2cAddr, NULL, 0, wData, wCount ));

	}
  	if (rDevAddr)
   	{
		if (!rData && rCount)
		{
			/* Buffer error! */
			pbierror("FUN:%s,LINE:%d,rCount:%d,\n",__FUNCTION__,__LINE__,rCount);
			return DRX_STS_INVALID_ARG;
		}
		/* TODO: prepare for write (e.g. set-up data structures) */
		{
			CHECK_I2C(DRV_I2C_Read( rDevAddr->i2cAddr, wData, wCount, rData, rCount ));
            //printf("FUN:%s,rData[0]:0x%X,rData[1]:0x%X,rData[2]:0x%X,rData[3]:0x%X,rCount: %d\n",__FUNCTION__,
						//rData[0],rData[1],rData[2],rData[3],rCount);
		}
	}
	
	return DRX_STS_OK;

i2c_error:
	return DRX_STS_ERROR;
}

/******************************
*
* char* DRXBSP_I2C_ErrorText( void )
*
* Returns a human readable error.
* Counter part of numerical DRX_I2C_Error_g.
*
* Output:
* - Pointer to string containing error description.
*
******************************/

char* DRXBSP_I2C_ErrorText( void )
{
   /* TODO: explain your I2C subsystem-specific error code */
   switch ( DRX_I2C_Error_g ) {
      case 0:
         return ("No error");
         break;
      /*
      case SOME_ERROR:
         return ("Some error");
         break;
      */
      default:
         return("Unknown error");
         break;
   } /* switch */

   return("DRXBSP_I2C_ErrorText error") ;
}
#endif
