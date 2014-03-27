/*******************************************************************

  Source file name : ctisc.h

  Description: 
	SC Driver API Interfaces for the Compunicate Smart Card driver   

   COPYRIGHT (C) Compunicate Technologies, Inc. 2003-2005

*********************************************************************/

#ifndef __SC_H__
#define __SC_H__

#include "ctidef.h"

BYTE CTISC_Init(void);
BYTE CTISC_Command(BYTE bLength, PBYTE pabMessage, PBYTE pabReplay);
void CTISC_Reset(void);

#endif	/*__SC_H__*/
