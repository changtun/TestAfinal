/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name : tlock.c
* Description : player对音视频的操作模块. 
* History :
*	Date               Modification                                Name
*	----------         ------------                                ----------
*	2007/12/28        Created                                     LSZ
******************************************************************************/
/*******************************************************/
/*              Includes				                   */
/*******************************************************/
/* Standard headers */

/* ST_API headers */
/* include ANSI C .h file */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* include OS, STAPI file */
#include "pvddefs.h"	
/* include Driver .h file */
/* include System layer .h file */
#include "pbitrace.h"	
#include "osapi.h"	

/* include Middle layer .h file */
/* include CA .h file */
/* include Local .h file */
//#include "dvbplayer.h"
#include "tlock.h"

#define DB_TLOCK_MSG 0

#if DB_TLOCK_MSG
	#define DB_TLOCK(__statement__)  TRC_DBMSG(__statement__)
	TRC_DBMSG_SET_MODULE(SYS);
#else
	#define DB_TLOCK(__statement__)
#endif

U32 g_SemCount = 0;/*BY SCZ 08-04-11 原因:为了保证创建semaphore时名字不同.*/
int TLock_create(TLock* lock)
{
	char cStr[20];

	memset(cStr, 0, 20);
	sprintf( cStr, "%s%d", "Tlock Sem", g_SemCount);
	OS_SemCreate(&(lock->uSemId), cStr, 1, 0);
	if((lock->uSemId >= OS_MAX_SEMAPHORES) || lock->uSemId < 0)
	{
		return -1;
	}
	lock->tid = 0xFFFFFFFF;
	lock->count = 0;
	g_SemCount++;
	return 0;
}

void TLock_destroy(TLock* lock)
{
	OS_SemDelete(lock->uSemId);
}

int TLock_try_lock(TLock* lock, S32 timeout)
{
	U32 tid;

	OS_TaskGetCurrentId(&tid);
	if(lock->tid != tid)
	{
		U32 r;
		if(-1 == timeout)
		{
			r = OS_SemWait(lock->uSemId);
		}
		else
		{
			r = OS_SemWait_Timeout(lock->uSemId, timeout);
		}

		if(0 != r)
		{
			return r;
		}

		lock->tid = tid;
		lock->count = 1;
	}
	else
	{
		lock->count++;
	}

	return 0;
}

void TLock_lock(TLock* lock)
{
	(void)TLock_try_lock(lock, -1 /*TIMEOUT_INFINITY*/);
}

void TLock_unlock(TLock* lock)
{
	lock->count--;
	if(0 == lock->count)
	{
		lock->tid = 0xFFFFFFFF;
		OS_SemSignal(lock->uSemId);
	}
}

