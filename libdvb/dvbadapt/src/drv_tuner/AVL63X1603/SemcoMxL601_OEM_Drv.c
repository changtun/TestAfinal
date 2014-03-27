/*****************************************************************************************
 *
 * FILE NAME          : MxL601_OEM_Drv.c
 * 
 * AUTHOR             : Dong Liu 
 *
 * DATE CREATED       : 01/23/2011  
 *
 * DESCRIPTION        : This file contains I2C driver and Sleep functins that 
 *                      OEM should implement for MxL601 APIs
 *                             
 *****************************************************************************************
 *                Copyright (c) 2010, MaxLinear, Inc.
 ****************************************************************************************/

#include "SemcoMxL601_OEM_Drv.h"
#include "IBSP.h"
#include "AVL63X1_include.h"

extern struct AVL63X1_Chip * pAVL_Semco_Chip;

/*----------------------------------------------------------------------------------------
--| FUNCTION NAME : SemcoCtrl_WriteRegister
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 7/30/2009
--|
--| DESCRIPTION   : This function does I2C write operation.
--|
--| RETURN VALUE  : True or False
--|
--|-------------------------------------------------------------------------------------*/

MXL_STATUS SemcoCtrl_WriteRegister(UINT8 I2cSlaveAddr, UINT8 RegAddr, UINT8 RegData)
{
	MXL_STATUS status = MXL_TRUE;
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	// OEM should implement I2C write protocol that complies with MxL601 I2C
	// format.

	// 8 bit Register Write Protocol:
	// +------+-+-----+-+-+----------+-+----------+-+-+
	// |MASTER|S|SADDR|W| |RegAddr   | |RegData(L)| |P|
	// +------+-+-----+-+-+----------+-+----------+-+-+
	// |SLAVE |         |A|          |A|          |A| |
	// +------+---------+-+----------+-+----------+-+-+
	// Legends: SADDR (I2c slave address), S (Start condition), A (Ack), N(NACK), 
	// P(Stop condition)

	UINT8 i2c_addr;
	AVL_uchar buffer[2]={0};
	AVL_uint16 size=0;
	#if  1
	i2c_addr=0x60;//write
	buffer[0]=RegAddr;
	buffer[1]=RegData;
	size=2;
	r = AVL63X1_II2C_Repeater_WriteData(i2c_addr,buffer,size, pAVL_Semco_Chip);
	if (r != AVL63X1_EC_OK)
	{
		return MXL_FALSE;
	}
	#endif /* #if 0 */

	return MXL_TRUE;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : SemcoCtrl_ReadRegister
--| 
--| AUTHOR        : Brenndon Lee
--|
--| DATE CREATED  : 7/30/2009
--|
--| DESCRIPTION   : This function does I2C read operation.
--|
--| RETURN VALUE  : True or False
--|
--|---------------------------------------------------------------------------*/

MXL_STATUS SemcoCtrl_ReadRegister(UINT8 I2cSlaveAddr, UINT8 RegAddr, UINT8 *DataPtr)
{
	MXL_STATUS status = MXL_TRUE;
	AVL63X1_ErrorCode r = AVL63X1_EC_OK;

	// OEM should implement I2C read protocol that complies with MxL601 I2C
	// format.

	// 8 bit Register Read Protocol:
	// +------+-+-----+-+-+----+-+----------+-+-+
	// |MASTER|S|SADDR|W| |0xFB| |RegAddr   | |P|
	// +------+-+-----+-+-+----+-+----------+-+-+
	// |SLAVE |         |A|    |A|          |A| |
	// +------+-+-----+-+-+----+-+----------+-+-+
	// +------+-+-----+-+-+-----+--+-+
	// |MASTER|S|SADDR|R| |     |MN|P|
	// +------+-+-----+-+-+-----+--+-+
	// |SLAVE |         |A|Data |  | |
	// +------+---------+-+-----+--+-+
	// Legends: SADDR(I2c slave address), S(Start condition), MA(Master Ack), MN(Master NACK), 
	// P(Stop condition)

	AVL_uchar buffer[3]={0};
	UINT8 i2c_addr;
	AVL_uint16 size=0;

	i2c_addr=I2cSlaveAddr+0x00;//write
	buffer[0]=0xFB;
	buffer[1]=RegAddr;
	size=2;
	r=AVL63X1_II2C_Repeater_WriteData(i2c_addr,buffer,size,pAVL_Semco_Chip);
	if(r!=AVL63X1_EC_OK)
	{
		return MXL_FALSE;
	}

	i2c_addr=I2cSlaveAddr;
	size=1;
	r=AVL63X1_II2C_Repeater_ReadData(i2c_addr, DataPtr, size,pAVL_Semco_Chip);
	if (r != AVL63X1_EC_OK)
	{
		return MXL_FALSE;
	}

	return MXL_TRUE;
}

/*------------------------------------------------------------------------------
--| FUNCTION NAME : MXL_Sleep
--| 
--| AUTHOR        : Dong Liu
--|
--| DATE CREATED  : 01/10/2010
--|
--| DESCRIPTION   : This function complete sleep operation. WaitTime is in ms unit
--|
--| RETURN VALUE  : None
--|
--|-------------------------------------------------------------------------------------*/

void SemcoMxL_Sleep(UINT16 DelayTimeInMs)
{
  // OEM should implement sleep operation 
	AVL63X1_IBSP_Delay(DelayTimeInMs);
}