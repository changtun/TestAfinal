/********************************************************************************
** File name: osapi.h
**
**
**Description :Contains functions prototype definitions and variables declarations
**                 for the OS Abstraction Layer, Core OS module
**
** Author:  Dongsheng Zhang
**
**version:1.0   2006/12/25
**           2.0   2008/2/21
**
**History : 2006/12/25 : Initial Version
**	      Modified:  
**		2008/2/21  :by dszhang  add os_os21.c to the module to support os21 .
**
** 
*********************************************************************************/

#ifndef _osapi_
#define _osapi_
/*******************************************************/
/*            Global define of osapi		                */
/*******************************************************/
 #undef  OS_MEMORY_TRACE 
 #undef  OS_QUEUE_TRACE 
 
#if defined(ST_OS20)
#define OS_OS20
#else 
#if defined(ST_OS21)
#define OS_OS20
#else 
#define OS_LINUX
#endif
#endif

#include "pvddefs.h"
#ifdef OS_OS21
#include <os21/task.h>
#include <stddefs.h>
#else 
#ifdef OS_OS20
#include <task.h>
#include <stddefs.h>
#else 
#ifdef OS_LINUX
#include <pthread.h>

#endif

#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif


#define OS_SUCCESS                     (0)
#define OS_ERROR                       (-1)
#define OS_INVALID_POINTER             (-2)
#define OS_ERROR_ADDRESS_MISALIGNED    (-3)
#define OS_ERROR_TIMEOUT               (-4)
#define OS_INVALID_INT_NUM             (-5)
#define OS_SEM_FAILURE                 (-6)
#define OS_SEM_TIMEOUT                 (-7)
#define OS_QUEUE_EMPTY                 (-8)
#define OS_QUEUE_FULL                  (-9)
#define OS_QUEUE_TIMEOUT               (-10)
#define OS_QUEUE_INVALID_SIZE          (-11)
#define OS_QUEUE_ID_ERROR              (-12)
#define OS_ERR_NAME_TOO_LONG           (-13)
#define OS_ERR_NO_FREE_IDS             (-14)
#define OS_ERR_NAME_TAKEN              (-15)
#define OS_ERR_INVALID_ID              (-16)
#define OS_ERR_NAME_NOT_FOUND          (-17)
#define OS_ERR_SEM_NOT_FULL            (-18)
#define OS_ERR_INVALID_PRIORITY        (-19)

#define OS_MAJOR_VERSION (2)
#define OS_MINOR_VERSION (0)

#define OS_MAX_TASKS            40
#define OS_MAX_QUEUES           100
#define OS_MAX_SEMAPHORES     100
#define OS_MAX_MUTEXES          100

/*difines constants for OS_BinSemCreate for state of semaphore  */
#define OS_SEM_FULL     1
#define OS_SEM_EMPTY    0

#define OS_TIMEOUT_INFINITY	0xFFFFFFFF
#define OS_TIMEOUT_IMMEDIATE	0

/* The maxium length allowed for a object (task,queue....) name */
#define OS_MAX_API_NAME     80

/*
** Defines for Queue Timeout parameters
*/
#define OS_PEND   (-1)
#define OS_CHECK (0)

#ifndef ONE_MILLISECOND
#define ONE_MILLISECOND 1 
#endif

#ifndef ONESEC
#define ONESEC 1000 /*Add by LSZ 2008.04.17*/
#endif


/*----- other macros (NOTE: these may be removed eventually) -----*/

#define OS_ABS(x)    ( ((x) <  0 ) ? -(x) : (x) )

#define OS_PACK

#ifdef OS_OS20
typedef clock_t          OS_TIME;
#define OS_DEF_MIN_STACK_SIZE		1024*4

#else
#ifdef OS_OS21
typedef osclock_t   OS_TIME;
#define OS_DEF_MIN_STACK_SIZE		1024*16

#else
#ifdef OS_LINUX
typedef long long int   OS_TIME;
#define OS_DEF_MIN_STACK_SIZE		1024*16
#endif
#endif
#endif

/*******************************************************/
/*              Exported Types			                */
/*******************************************************/
/*  tables for the properties of objects */

/*tasks */
typedef struct
{
    char name[OS_MAX_API_NAME];
#ifdef OS_LINUX
    pthread_t creator;
#else
    task_t* creator;
#endif
    U32 stack_size;
    U32 priority;
}OS_task_prop_t;
    
/* queues */
typedef struct
{
    char name[OS_MAX_API_NAME];
 #ifdef OS_LINUX
    pthread_t creator;
#else
    task_t* creator;
#endif
}OS_queue_prop_t;

/* Binary Semaphores */
typedef struct
{                     
    char name[OS_MAX_API_NAME];
#ifdef OS_LINUX
    pthread_t creator;
#else
    task_t* creator;
#endif
}OS_bin_sem_prop_t;

/* Mutexes */
typedef struct
{
    char name[OS_MAX_API_NAME];
 #ifdef OS_LINUX
    pthread_t creator;
#else
    task_t* creator;
#endif
}OS_mut_sem_prop_t;


/* struct for OS_GetLocalTime() */

typedef struct 
{ 
    U32 seconds; 
    U32 microsecs;
}OS_time_t; 

//{creat by dszhang in 2007.9.7 for modify memory op
 typedef enum 
{ 
    	system_p=0,
	other_p		
}partition_type; 
//}creat by dszhang in 2007.9.7 for modify memory op

//{creat by dszhang in 2007.9.7 for modify memory op
 typedef enum 
{ 
    	trace_off=0,
	trace_on		
}trace_option; 
//}creat by dszhang in 2007.9.7 for modify memory op


#if (defined(OS_QUEUE_TRACE) ||defined(OS_MEMORY_TRACE))
//{creat by dszhang in 2007.9.11 for modify memory trace
/* Block allocated control structure	*/
typedef struct s_MemList
{
	void				*Ptr;			/* allocated block address					*/
	U16				Line;			/* allocation request source line 			*/
	char				*File;			/* allocation request file		 			*/
	char	 const			*taskname;			/* allocation request task		 			*/
	U32				taskID;
#ifdef OS_OS21
	osclock_t			time;                  /*allocate time */
#else

#endif
	U32				SegtSize;		/* allocation size							*/
	struct s_MemList	*NextMemHeader;	/* next block control structure				*/
} t_MemList;

#define MEMORY_CTRL_SIZE	640000		/* (16*4000 blocks)	*/

#define	MEMORY_FLAG_VALUE	(unsigned int)0xFEDC

/*--------------------------------------------------------------------------*/
/* Integrity block flag														*/
/* This flag is added at the begining and the end of each allcated block	*/
/* in order to check its integrity in the PBI_Mem_freeing call						*/
/*--------------------------------------------------------------------------*/
typedef U32  s_MemoryFlag;
//}creat by dszhang in 2007.9.11 for modify memory trace

#endif


#if defined(OS_QUEUE_TRACE) && !defined(OS_LINUX)
//{creat by dszhang in 2007.9.19 for for message queue trace
/* Block allocated control structure	*/
typedef struct t_QueueHeader
{
	 U16		         Line;			/* allocation request source line 			*/
	char				*File;			/* allocation request file		 			*/
	 struct t_QueueHeader	*PrevQueueHeader;	/* next block control structure				*/
	 struct t_QueueHeader	*NextQueueHeader;	/* next block control structure				*/
} s_QueueHeader;

#define QUEUE_CTRL_SIZE	4000		/* (16*4000 blocks)	*/

//}creat by dszhang in 2007.9.19 for for message queue trace

#endif



/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*               Functions		Declarations                */
/*******************************************************/

void OS_GetVersion(void);


/*
** Initialization of API
*/

void 	OS_API_Init (void);


/*
** Task API
*/
#ifdef OS_LINUX
S32 		OS_TaskCreate(U32 *task_id, const char *task_name,
                          			void*(* function_pointer)(void* Param),void* Param,
                      				void *stack_pointer, U32 stack_size, U32 priority,U32 flags);
#else
S32 		OS_TaskCreate (U32 *task_id, const char *task_name,
                          			void (* function_pointer)(void* Param),void* Param,
                      				void *stack_pointer, U32 stack_size, U32 priority,U32 flags);
#endif
S32 		OS_TaskDataSet (U32 task_id, void *data);
void * 	OS_TaskDataGet (U32 task_id);
S32 		OS_TaskDelete (U32 task_id); 
S32 		OS_TaskDelay  (U32 millisecond);
void  	OS_TaskReschedule (void);
S32 		OS_TaskSetPriority (U32 task_id, U32 new_priority);
S32 		OS_TaskGetIdByName (U32 *task_id, const char *task_name);
S32 		OS_TaskGetCurrentId (U32 *taskid);
S32 		OS_TaskGetInfo (U32 task_id, OS_task_prop_t *task_prop);      
void 	OS_TaskSttbxInfo (char * taskname);



/*
** Message Queue API
*/

/*
** Queue Create now has the Queue ID returned to the caller.
*/

#if defined(OS_QUEUE_TRACE) && !defined(OS_LINUX)
void		OS_MessageQueueInfo(const char *queuename );
S32 		OS_MessageQueueCreate(U32 *queue_id, const char *queue_name,
                                			U32 queue_depth, U32 data_size, U32 flags);
S32 		OS_MessageQueueDelete(U32 queue_id);
S32 		OS_MessageQueueReceive(U32 queue_id, void *data, U32 size, 
                                	U32 *size_copied, S32 timeout);
S32 		OS_MessageQueueSend(U32 queue_id, void *data, U32 size, S32 timeout,U16 Line,char *File,U32 flags);
S32 		OS_MessageQueueGetIdByName(U32 *queue_id, const char *queue_name);
S32 		OS_MessageQueueGetInfo(U32 queue_id, OS_queue_prop_t *queue_prop);
#else
S32 		OS_MessageQueueCreate(U32 *queue_id, const char *queue_name,
                                	U32 queue_depth, U32 data_size, U32 flags);
S32 		OS_MessageQueueDelete(U32 queue_id);
S32 		OS_MessageQueueReceive(U32 queue_id, void *data, U32 size, 
                                	U32 *size_copied, S32 timeout);
S32 		OS_MessageQueueSend (U32 queue_id, void *data, U32 size, S32 timeout,U32 flags);
S32 		OS_MessageQueueGetIdByName(U32 *queue_id, const char *queue_name);
S32 		OS_MessageQueueGetInfo(U32 queue_id, OS_queue_prop_t *queue_prop);
//liqian 20120725 UtiCA 接口使用
S32 		OS_MessageQueueRelease_UTI (U32 queue_id, void *data);
char*   	OS_MessageQueueReceive_UTI (U32 queue_id, U32 size, U32 *size_copied, S32 timeout);
S32 		OS_MessageQueueSend_UTI(U32 queue_id, void *data, U32 size,U32 flags);
char* 		OS_MessageQueueclaim_UTI(U32 queue_id, U32 size, S32 timeout,U32 flags);

#endif



/*
** Semaphore API
*/

S32 		OS_SemCreate(U32 *sem_id, const char *sem_name, 
                                U32 sem_initial_value, U32 options);
S32 		OS_SemSignal(U32 sem_id);
S32 		OS_SemWait(U32 sem_id);
S32 		OS_SemWait_Timeout(U32 sem_id, U32 msecs);
S32 		OS_SemDelete(U32 sem_id);
S32 		OS_SemGetIdByName(U32 *sem_id, const char *sem_name);
S32 		OS_SemGetInfo(U32 sem_id, OS_bin_sem_prop_t *bin_prop);
S32 		OS_SemGetValue(U32 sem_id);

/*
** Mutex API
*/

S32 		OS_MutexCreate (U32 *sem_id, const char *sem_name, U32 options);
S32 		OS_MutexRelease(U32 sem_id);
S32 		OS_MutexLock (U32 sem_id);
S32 		OS_MutexTrylock ( U32 sem_id );
S32 		OS_MutexDelete (U32 sem_id);  
S32 		OS_MutexGetIdByName (U32 *sem_id, const char *sem_name); 
S32 		OS_MutexGetInfo (U32 sem_id, OS_mut_sem_prop_t *mut_prop);

/*
** OS Time/Tick related API
*/


S32 		OS_GetLocalTime( U64 *millisecond);

/**Add by LL 2008-4-2*/
S32		OS_TimeMinus( U64  *ms_result, U64 ms_from, U64 ms_by);
S32		OS_TimePlus( U64  *ms_result, U64 ms_time1, U64 ms_time2);
/**Add by LL 2008-4-2*/

/*
** OS Memorry API
*/

#if defined(OS_MEMORY_TRACE) 
void *	OS_MemAlloc (partition_type type,U32 size,U16 Line,char *File) ;  //add on 2007.9.12  by dszhang for memory trace
void *	OS_MemRealloc (partition_type type,void* Block,U32 size,U16 Line,char *File) ;  //add on 2007.9.12  by dszhang for memory trace
void 	OS_MemDealloc (partition_type type, void* Block,U16 Line,char *File) ;  //add on 2007.9.12  by dszhang for memory trace
 void 	OS_MemTraceInfo(char*taskname,char *filename ); //add by dszhang 2007.9.29
 void 	OS_MemTraceCtrl(trace_option  option ); //add by dszhang 2008.2.29
#else
void *	OS_MemAlloc (partition_type type,U32 size,U16 Line,char *File) ;  //add on 2007.9.12  by dszhang for memory trace
void *	OS_MemRealloc (partition_type type,void* Block,U32 size,U16 Line,char *File) ;  //add on 2007.9.12  by dszhang for memory trace
void 	OS_MemDealloc(partition_type type,void* Block) ;
#endif

/*
** Interrupt API
*/

S32 		OS_IntAttachHandler(U32 InterruptNumber, void * InerruptHandler , S32 parameter);
S32 		OS_IntEnableAll(void);
S32 		OS_IntDisableAll(void);

/* BEGIN: Added by zhwu, 2013/3/13 */

/*
** IIC API
*/

extern S32 DRV_I2C_Open(void);
extern S32 DRV_I2C_Read( U8 u8SlaveAddr, U8 *pu8ReAddr, U16 u16ReAddrCount, U8 *pu8Buff, U16 u16Length );
extern S32 DRV_I2C_Write( U8 u8SlaveAddr, U8 *pu8ReAddr, U16 u16ReAddrCount, U8 *pu8Buff, U16 u16Length );
extern S32 DRV_I2C_Close(void);
/* END:   Added by zhwu, 2013/3/13 */



//{creat by dszhang in 2007.9.20 for  message queue trace
#if defined(OS_QUEUE_TRACE) && !defined(OS_LINUX)
#define OS_QueueCreate(p_id,name,depth,size,flag)			OS_MessageQueueCreate(p_id,name,depth,size,flag)
#define OS_QueueDelete(id)								OS_MessageQueueDelete(id)
#define OS_QueueReceive(id,p_data,size,p_copied,time)		OS_MessageQueueReceive(id,p_data,size,p_copied,time)
#define OS_QueueSend(id,p_data,size,time,flag) 			OS_MessageQueueSend(id,p_data,size,time,__LINE__,__FILE__,flag)
#define OS_QueueGetIdByName(p_id,p_name)				OS_MessageQueueGetIdByName(p_id,p_name)
#define OS_QueueGetInfo(id,p_prop) 						OS_MessageQueueGetInfo(id,p_prop)
#else
#define OS_QueueCreate(p_id,name,depth,size,flag)			OS_MessageQueueCreate(p_id,name,depth,size,flag)
#define OS_QueueDelete(id)								OS_MessageQueueDelete(id)
#define OS_QueueReceive(id,p_data,size,p_copied,time)		OS_MessageQueueReceive(id,p_data,size,p_copied,time)
#define OS_QueueSend(id,p_data,size,time,flag) 			OS_MessageQueueSend(id,p_data,size,time,flag)
#define OS_QueueGetIdByName(p_id,p_name)				OS_MessageQueueGetIdByName(p_id,p_name)
#define OS_QueueGetInfo(id,p_prop) 						OS_MessageQueueGetInfo(id,p_prop)
#endif
//}creat by dszhang in 2007.9.12 for  message queue trace




//{creat by dszhang in 2007.9.12 for  memory trace
#if defined(OS_MEMORY_TRACE)
#define  OS_MemAllocate(type,size)    			OS_MemAlloc(type,size,__LINE__,__FILE__) 
#define  OS_MemReallocate(type,point,size) 	OS_MemRealloc(type,point,size,__LINE__,__FILE__)  
#define  OS_MemDeallocate(type,point) 		OS_MemDealloc(type,point,__LINE__,__FILE__)  
#else
#define  OS_MemAllocate(type,size)    			OS_MemAlloc(type,size,__LINE__,__FILE__) 
#define  OS_MemReallocate(type,point,size) 	OS_MemRealloc(type,point,size,__LINE__,__FILE__) 
#define  OS_MemDeallocate(type,point) 		OS_MemDealloc(type,point) 
#endif
//}creat by dszhang in 2007.9.12 for  memory trace

#ifdef __cplusplus
}
#endif



#endif

