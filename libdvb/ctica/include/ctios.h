/*******************************************************************

  Source file name : ctios.h

  Description:
	OS support APIs for all applications of integrating CTI's CA.

  COPYRIGHT (C) Compunicate Technologies, Inc. 2003-2005

*********************************************************************/

#ifndef _OS_H_
#define _OS_H_

#include "ctidef.h"

typedef struct
{
	WORD        msg_length;
	WORD        reserved;
	BYTE*       pData;
	UINT         type;
}CTI_OsMessage_t;

PVOID CTIOS_MemoryAllocate(ULONG uSize);
VOID  CTIOS_MemoryFree(PVOID pvMemory);
CTI_MailBoxOfTask_t CTIOS_TaskCreate(VOID(*task)(PVOID arg),INT iPriority,INT iStackSize);
CTI_OsSemaphore_t CTIOS_CreateSemaphore(ULONG uInitialValue);
VOID CTIOS_SignalSemaphore(CTI_OsSemaphore_t uSemaphore);
VOID CTIOS_WaitSemaphore(CTI_OsSemaphore_t uSemaphore);
VOID CTIOS_TaskDelay(INT iTimeInMilliSeconds);
BYTE CTIOS_SendMessage(CTI_MailBoxOfTask_t uMailbox, CTI_OsMessage_t *pstMsg);
CTI_OsMessage_t * CTIOS_ReceiveMessage(CTI_MailBoxOfTask_t uMailbox);
VOID CTIOS_Print(CHAR *Format_p, ...);
void CTICAclient_GetSTBipAddr(BYTE* ipaddr);

#endif	/*_OS_H_*/
