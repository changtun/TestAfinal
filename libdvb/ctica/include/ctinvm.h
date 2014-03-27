/*******************************************************************

  Source file name : ctinvm.h

  Description:
	NVRAM driver header file for CTI-CA.

  COPYRIGHT (C) Compunicate Technologies, Inc. 2003-2005

*********************************************************************/

#ifndef NVM_H_
#define NVM_H_

#include "ctidef.h"

BYTE CTINVM_Init(void);
BYTE CTINVM_Read(BYTE *pdata, WORD len, WORD position);
BYTE CTINVM_Write(BYTE *pdata, WORD len, WORD position);

#endif /* NVM_H_ */
