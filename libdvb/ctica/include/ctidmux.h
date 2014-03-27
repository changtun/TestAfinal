/*******************************************************************

  Source file name : ctidmux.h

  Description: 
	Demultiplexer Driver API module header of integrating CTI's CA.

   COPYRIGHT (C) Compunicate Technologies, Inc. 2003-2005

*********************************************************************/

#ifndef __DMUX_H__
#define __DMUX_H__

#include "ctidef.h"

/*---------------------------- Variables Definitions -----------------------------------------------*/
typedef WORD DMUX_FILTER_M;
typedef WORD DMUX_CHANNEL_M;
typedef UINT DMUX_DESCRAM_M;

#define DMUX_INVALID_CHANNEL_ID      0xffff
#define DMUX_INVALID_FILTER_ID       0xffff
#define DMUX_INVALID_DESCRAMBLER_ID  0xffff 

#define DMUX_STOP_CHANNEL		0
#define DMUX_START_CHANNEL		1
#define DMUX_RESET_CHANNEL		2

/*------------------------------- API functions ----------------------------------------------------*/
BYTE CTIDMUX_Init(void);

DMUX_DESCRAM_M  CTIDMUX_OpenDescrambler(BYTE bConnection);
DMUX_CHANNEL_M 	CTIDMUX_OpenSectionChannel(INT max_filter_number, INT max_filter_size, WORD buffer_size, BYTE bConnection);

DMUX_FILTER_M 	CTIDMUX_OpenFilter(DMUX_CHANNEL_M SecChannelID);
void CTIDMUX_GetDataSection(DMUX_CHANNEL_M SecChannelID, BYTE **section_ptr, INT *pSecLen);
void CTIDMUX_RegisterChannelCallback(DMUX_CHANNEL_M SecChannelID, void (*upfct)(DMUX_CHANNEL_M SecChannelID));

void CTIDMUX_ControlChannelState(DMUX_CHANNEL_M SecChannelID,BYTE WorkStatus);
void CTIDMUX_SetChannelPID(DMUX_CHANNEL_M SecChannelID, WORD pid);
BYTE CTIDMUX_SetFilterValue(DMUX_CHANNEL_M SecChannelID,DMUX_FILTER_M FilterID, PBYTE pMask, PBYTE pMatch);

void CTIDMUX_CloseSectionChannel(DMUX_CHANNEL_M SecChannelID);
void CTIDMUX_CloseSection(DMUX_CHANNEL_M SecChannelID, BYTE *pSection, INT SecLen);
void CTIDMUX_CloseFilter(DMUX_CHANNEL_M SecChannelID,DMUX_FILTER_M FilterID);
void  CTIDMUX_CloseDescrambler(DMUX_DESCRAM_M DescId);

void  CTIDMUX_SetDescramblerPID(DMUX_DESCRAM_M DescId, INT Pid);
void  CTIDMUX_SetDescramblerOdd_Key(DMUX_DESCRAM_M DescId, PBYTE pKey);
void  CTIDMUX_SetDescramblerEven_Key(DMUX_DESCRAM_M DescId, PBYTE pKey);


#endif	/*__DMUX_H__*/
