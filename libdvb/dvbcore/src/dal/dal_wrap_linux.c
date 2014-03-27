/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name : dal_wrap_linux.c
* Description : 
* History : 
*	Date				Modification				Name
*	----------			------------			----------
*	2011/4/11			Created					zyliu
******************************************************************************/
/*-----------------------------------------------------*/
/*              Includes												*/
/*-----------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "osapi.h"
#include "pvddefs.h"

#include "dal_wrap.h"
/*-----------------------------------------------------*/
/*              Defines												*/
/*-----------------------------------------------------*/

/*-----------------------------------------------------*/
/*              Global												*/
/*-----------------------------------------------------*/

/*-----------------------------------------------------*/
/*              Exported Types									*/
/*-----------------------------------------------------*/

/*-----------------------------------------------------*/
/*              Local Function Prototypes								*/
/*-----------------------------------------------------*/

/*-----------------------------------------------------*/
/*              External Function  Definition								*/
/*-----------------------------------------------------*/
S32 DAL_TaskCreate( DAL_TaskID_t* TaskID, const char *TaskName, void*(*func)(void* Param), void* pParam, void* pStockPointer, U32 StockSize, U32 Prioity,U32 Flag)
{
	return OS_TaskCreate( TaskID, TaskName, func, pParam, pStockPointer, StockSize, Prioity, Flag);
}

S32 DAL_TaskDelete( DAL_TaskID_t TaskID)
{
	//return OS_TaskDelete_new(TaskID);
	return OS_TaskDelete(TaskID);//再android系统可以重写一下，存在隐患
}

S32 DAL_TaskDelay( U32 millisecond)
{
	return OS_TaskDelay(millisecond);
}

S32 DAL_DeleteTaskRecord( DAL_TaskID_t TaskID)
{
	return OS_DeleteTaskRecord(TaskID);
}


S32 DAL_SemCreate( DAL_Sem_t *SemID, const C8* SemName, U32 SemInitValue, U32 Options)
{
	return OS_SemCreate( SemID, SemName, SemInitValue, Options);
}

S32 DAL_SemDelete( DAL_Sem_t SemID)
{
	return OS_SemDelete( SemID);
}

S32 DAL_SemSignal( DAL_Sem_t SemID)
{
	return OS_SemSignal( SemID);
}

S32 DAL_SemWait( DAL_Sem_t SemID)
{
	return OS_SemWait( SemID);
}

S32 DAL_SemWait_Timeout( DAL_Sem_t SemID, U32 msecs)
{
	return OS_SemWait_Timeout( SemID, msecs);
}

S32 DAL_MessageQueueCreate( DAL_QueueID_t* QueueID, const C8* QueueName, U32 QueueDepth, U32 DataSize, U32 Flags)
{
	return OS_MessageQueueCreate( QueueID, QueueName, QueueDepth, DataSize, Flags);
}

S32 DAL_MessageQueueDelete( DAL_QueueID_t QueueID)
{
	return OS_MessageQueueDelete( QueueID);
}

S32 DAL_MessageQueueSend( DAL_QueueID_t QueueID, void* pData, U32 DataSize, S32 Timeout, U32 Flags)
{
	return OS_MessageQueueSend( QueueID, pData, DataSize, Timeout, Flags);
}

S32 DAL_MessageQueueReceive( DAL_QueueID_t QueueID, void* pData, U32 DataSize, U32* SizeCopy, S32 Timeout)
{
	return OS_MessageQueueReceive( QueueID, pData, DataSize, SizeCopy, Timeout);
}

void* DAL_Malloc( U32 size)
{
#if 0
	static int		dal_mem_alloc = 0;
	void*			p = NULL;

	p = OSAL_MemAllocate(system_p,size);
	if( p != NULL)
	{
		dal_mem_alloc += size;
		DAL_DEBUG((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> DAL_Malloc   0x%x   %d\n", p, dal_mem_alloc));
	}
	return p;
#else
	return OS_MemAllocate(system_p,size);
#endif

}

void DAL_Free( void* p)
{
#if 0
	DAL_DEBUG(("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< DAL_Free:   0x%x\n", p));
#else
	OS_MemDeallocate(system_p,p);
#endif
}



/*-----------------------------------------------------*/
/*              Internal Function	 Definition							*/
/*-----------------------------------------------------*/



