
/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name :trcas_NVRAM.c
* Description : 
* History :
* Date               Modification                                Name
* ----------         ------------                        ----------
* 2012/06/21         Created                                Li qian 
******************************************************************************/

/*******************************************************/
/*              Includes				                                        */
/*******************************************************/
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "sys/stat.h"
#include <fcntl.h>
#include "sys/types.h"
#include <unistd.h>
#include "osapi.h" 
//#include "Tr_Cas.h"
#include "ctinvm.h"
#include "pbitrace.h"


/*******************************************************/
/*               Private Defines and Macros			                    */
/*******************************************************/

#define CA_FILE "/mnt/expand/ctica_file"//"data/data/com.pbi.dvb/ca_file"



#define OS_SEEK_SET         0
#define OS_SEEK_CUR         1
#define OS_SEEK_END         2


static  S32  ca_file;

/*******************************************************/
/*               Private Constants                                                    */
/*******************************************************/

/*******************************************************/
/*               Global Variables					                          */
/*******************************************************/

/*******************************************************/
/*               Private Variables (static)			                          */
/*******************************************************/




/*******************************************************/
/*               Private Function prototypes		                          */
/*******************************************************/

/*******************************************************/
/*               Functions							                   */
/*******************************************************/



/******************************************************************************
* Function :TRDRV_NVRAM_Initialise
* Parameters : 
*			none
* Return :
*			0:successful
*			1: failure
* Description :
*                   The function is to initialize the nvram drvier
* Author : 
*                   Liqian     20120627
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/
//S16   TRDRV_NVRAM_Initialise(void)
BYTE   CTINVM_Init(void)
{
	

	
	U32 mode =777;
	S32 ret = 0;
	char cmd[50];
    
	printf( " %s:%d  \n",__FILE__,__LINE__);

    
	ret = access(CA_FILE, F_OK);
	if( 0 == ret )
	{
	    	memset( cmd, 0, 50 );
		sprintf( cmd,"chmod %d %s",mode,CA_FILE);
		system(cmd);
		printf("\n--------CTINVM_Init access %d----------------\n",__LINE__);
	}
	
	printf("\n#################3--------CTINVM_Init access %d----------------\n",__LINE__);
	
	ca_file=open(CA_FILE,O_CREAT|O_RDWR|O_SYNC,mode);
	if((S32)ca_file == -1)
	{
		 printf( "error %s:%d  \n",__FILE__,__LINE__);
		 return 1;
	}
	return 0;
}

/******************************************************************************
* Function :TRDRV_NVRAM_Read
* Parameters : 
*			ulOffset:offset from the nvram start address
*			pbData:memory location to which read data should be copied
*			wLength:number of bytes to be read
* Return :
*			0:successful
*			1: failure
* Description :
*                   The function is to read nvram data to memory
* Author : 
*                   Liqian     20120627
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/
//S16   TRDRV_NVRAM_Read(U32 ulOffset, OUT U8 *pbData, U16 wLength)
BYTE CTINVM_Read(BYTE *pdata, WORD len, WORD position)
{
	S32 status;

    
    
	if((pdata ==NULL)||(len <=0))
	{
		printf("%s param error %d \n",__FUNCTION__,__LINE__);
		return 1;
	}

	printf("n\n CA file read!!!\n\n");
	status = lseek(  ca_file, (off_t) position,  SEEK_SET );

	if (  status < 0)
	{
	 	printf("%s  error %d \n",__FUNCTION__,__LINE__);
		return 1;
	}

	status = read(ca_file, pdata, len);

	if (  status < 0)
	{
		printf("%s	error %d \n",__FUNCTION__,__LINE__);
		return 1;
	}

	return 0;
 

}


/******************************************************************************
* Function :TRDRV_NVRAM_Write
* Parameters : 
*			ulOffset:offset from the nvram start address
*			pbData:memory location to which  data should be copied
*			wLength:number of bytes to be read
* Return :
*			0:successful
*			1: failure
* Description :
*                   The function is to write wLength bytes starting at address pbData to NVRAM
* Author : 
*                   Liqian     20120627
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/
//S16   TRDRV_NVRAM_Write(U32 ulOffset, IN U8 *pbData,  U16 wLength)
BYTE CTINVM_Write(BYTE *pdata, WORD len, WORD position)
{
	S32 status;
		
	if((pdata ==NULL)||(len <=0))
	{
		printf("%s param error %d \n",__FUNCTION__,__LINE__);
		return 1;
	}

	printf("n\n CA file write!!!\n\n");
	status = lseek(  ca_file, (off_t) position,  SEEK_SET );

	if (  status < 0)
	{
		printf("%s	error %d \n",__FUNCTION__,__LINE__);
		return 1;
	}

	status = write(ca_file, pdata, len);
 
	if (  status < 0)
	{
		printf("%s	error %d \n",__FUNCTION__,__LINE__);
		return 1;
	}

	return 0;	 


}

