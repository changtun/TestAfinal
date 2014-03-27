/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : 
* Description : 
* Author : zyliu
* History :
*	2011/4/11 : Initial Version
******************************************************************************/
#ifndef __DAL_WRAP_H
#define __DAL_WRAP_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************/
/*              Includes											*/
/*******************************************************/

/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/
#define DAL_PEND	OS_PEND
#define	DAL_CHECK 	OS_CHECK

/*******************************************************/
/*              Exported Types			                */
/*******************************************************/
typedef U32		DAL_Sem_t;
typedef U32		DAL_TaskID_t;
typedef U32		DAL_QueueID_t;

#ifndef DEFINED_BOOL
#define DEFINED_BOOL
typedef int BOOL;
#endif


/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/
#if 1
#define DAL_DEBUG(_x_)		{ printf("                            [DAL_DEBUG] %s: ", __FUNCTION__); printf _x_ ; }
#define DAL_ERROR(_x_)		{ printf("            ?????[DAL_ERROR] %s: ", __FUNCTION__); printf _x_ ; }
#define DAL_PRINT(_x_)		{ printf _x_ ; }
#else
#define DAL_DEBUG(_x_)		{  }
#define DAL_ERROR(_x_)		{  }
#define DAL_PRINT(_x_)		{  }

#endif

#define DAL_MAX(x,y)	((x)>(y)?(x):(y))
#define DAL_MIN(x,y)	((x)<(y)?(x):(y))

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/
S32 DAL_TaskCreate( DAL_TaskID_t* TaskID, const char *TaskName, void*(*func)(void* Param), void* pParam, void* pStockPointer, U32 StockSize, U32 Prioity,U32 Flag);
S32 DAL_TaskDelete( DAL_TaskID_t TaskID);
S32 DAL_TaskDelay( U32 millisecond);
S32 DAL_DeleteTaskRecord( DAL_TaskID_t TaskID);

S32 DAL_SemCreate( DAL_Sem_t *SemID, const C8* SemName, U32 SemInitValue, U32 Options);
S32 DAL_SemDelete( DAL_Sem_t SemID);
S32 DAL_SemSignal( DAL_Sem_t SemID);
S32 DAL_SemWait( DAL_Sem_t SemID);
S32 DAL_SemWait_Timeout( DAL_Sem_t SemID, U32 msecs);

S32 DAL_MessageQueueCreate( DAL_QueueID_t* QueueID, const C8* QueueName, U32 QueueDepth, U32 DataSize, U32 Flags);
S32 DAL_MessageQueueDelete( DAL_QueueID_t QueueID);
S32 DAL_MessageQueueSend( DAL_QueueID_t QueueID, void* pData, U32 DataSize, S32 Timeout, U32 Flags);
S32 DAL_MessageQueueReceive( DAL_QueueID_t QueueID, void* pData, U32 DataSize, U32* SizeCopy, S32 Timeout);

void* DAL_Malloc( U32 size);
void DAL_Free( void* p);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __XXXXXXX_H */
/* End of __DAL_WRAP_H  --------------------------------------------------------- */

