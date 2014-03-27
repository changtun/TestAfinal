/*****************************************************************************
*    (c)    Copyright Pro. Broadband Inc. PVware
*
* File name : os_os21.c
* Description :  This file  contains some of the OS APIs abstraction layer 
**                   implementation for OS21.
* History :
*    Date               Modification                                Name
*    ----------         ------------                                ----------
*    2008/3/25        Created                                     Dongsheng Zhang
*   
*
*
******************************************************************************/

/****************************************************************************************
                                    INCLUDE FILES
****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#ifdef QUEUE_MQ
#include <mqueue.h>
#endif
#include <sched.h>
#include <time.h>
#include <signal.h>
/* BEGIN: Added by zhwu, 2013/3/13 */
#include <linux/i2c.h>
/* END:   Added by zhwu, 2013/3/13 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>     
#include <sys/select.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h> 

#include "pbitrace.h"
//#include "glib.h"
#include "osapi.h"

#ifdef  OS_MEMORY_TRACE  
static void OS_MemInit(void );
#endif

/****************************************************************************************
                                     DEFINES
****************************************************************************************/
#define DB_OSAPI_MSG 0

#if DB_OSAPI_MSG
	#define DB_OSAPI(__statement__)  TRC_DBMSG(__statement__)
	TRC_DBMSG_SET_MODULE(OSA);
#else
	#define DB_OSAPI(__statement__)
#endif


#define QUEUE_SELF


#define OS_BASE_PORT 8000
#define UNINITIALIZED 0
#define MAX_PRIORITY 99

/* 
** this define is need to run on cygwin. It should not affect the 
** real linux version at all 
*/
#ifndef PTHREAD_STACK_MIN
   #define PTHREAD_STACK_MIN  0 /* set to any non negative value */
#endif


/****************************************************************************************
                                   GLOBAL DATA
****************************************************************************************/


/*******************************************************/
/*               Private Types                           */
/*******************************************************/
/*  tables for the properties of objects */
#ifdef QUEUE_SELF
typedef struct message_queue_s
{   U16  				Index;
    sem_t 				MsgSemaphore_p;
    sem_t             	ClaimSemaphore_p;
} message_queue_t;
#endif
/*tasks */
typedef struct
{
    int free;
    pthread_t id;
    char name [OS_MAX_API_NAME];
    pthread_t creator;
    U32 stack_size;
    U32 priority;
}OS_task_record_t;
    
/* queues */
typedef struct
{
    int free;
#ifdef QUEUE_SOCKET	
    int id;
#endif
#ifdef QUEUE_MQ
    mqd_t id;
#endif
#ifdef QUEUE_SELF
   message_queue_t *id;
   U32 size;
   U32 elemnum;
#endif 
    char name [OS_MAX_API_NAME];
    pthread_t creator;
    U32   count;  
}OS_queue_record_t;

/* Binary Semaphores */
typedef struct
{
    int free;
    sem_t id;
    char name [OS_MAX_API_NAME];
    pthread_t creator;
}OS_bin_sem_record_t;

/* Mutexes */
typedef struct
{
    int free;
    pthread_mutex_t id;
    char name [OS_MAX_API_NAME];
    pthread_t creator;
}OS_mut_sem_record_t;

struct i2c_rdwr_ioctl_data {
	struct i2c_msg __user *msgs;	/* pointers to i2c_msgs */
	__u32 nmsgs;			/* number of i2c_msgs */
};

//typedef void(*function_pointer)(void*)    funcpoint;


/*******************************************************/
/*               Private Constants                     */
/*******************************************************/

/*******************************************************/
/*               Global Variables                       */
/*******************************************************/


/*******************************************************/
/*               External Variables                       */
/*******************************************************/


/*******************************************************/
/*               Private Variables (static)               */
/*******************************************************/

/* Tables where the OS object information is stored */
OS_task_record_t    OS_task_table      [OS_MAX_TASKS];
OS_queue_record_t   OS_queue_table     [OS_MAX_QUEUES];
OS_bin_sem_record_t OS_bin_sem_table   [OS_MAX_SEMAPHORES];
OS_mut_sem_record_t OS_mut_sem_table   [OS_MAX_MUTEXES];

pthread_key_t    thread_key;

sem_t OS_task_table_sem;
sem_t OS_queue_table_sem;
sem_t OS_bin_sem_table_sem;
sem_t OS_mut_sem_table_sem;


#ifdef  OS_MEMORY_TRACE 
	//{creat by dszhang in 2007.9.11 for modify memory trace
	
	typedef struct
	{
	    char	 const *taskname;
	    char	 const *filename;
	}traverse_par;

	static U8 mem_init=FALSE;
	static sem_t OS_mem_sem;
	GTree *mem_trace_tree=NULL;
	//}creat by dszhang in 2007.9.11 for modify memory trace
#endif


/*******************************************************/
/*               global  Function prototypes           */
/*******************************************************/
extern int sem_timedwait (sem_t *,const struct timespec *);
#ifdef QUEUE_MQ
extern ssize_t mq_timedreceive(mqd_t , char *,
			       size_t ,
			       unsigned int *,
			       const struct timespec *);
extern int mq_timedsend(mqd_t , const char *,
			size_t , unsigned int ,
			const struct timespec *);
#endif

/*******************************************************/
/*               Private Function prototypes           */
/*******************************************************/

static pthread_t OS_FindCreator(void);
static U32  OS_CompAbsDelayedTime( U32 milli_second , struct timespec * tm);
#ifdef QUEUE_SELF
static S8 Init_Allqueues(void);
static message_queue_t * message_create_queue_timeout(size_t ElementSize, unsigned int NoElements);
static S8 message_delete_queue(message_queue_t * MessageQueue);
static int message_claim_timeout(message_queue_t * MessageQueue, U32);
static int message_claim(message_queue_t * MessageQueue);
static void message_release (message_queue_t* MessageQueue, void* Message);
static int message_send(message_queue_t * MessageQueue, void * message);
static void * message_receive_timeout (message_queue_t* MessageQueue, U32);
static void * message_receive (message_queue_t* MessageQueue);
#endif



/*******************************************************/
/*               Functions                               */
/*******************************************************/
/*---------------------------------------------------------------------------------------
   Name: OS_API_Init

   Purpose: Initialize the tables that the OS API uses to keep track of information
            about objects

   returns: nothing
---------------------------------------------------------------------------------------*/
void OS_API_Init(void)
{
   int i;
   int ret;

    /* Initialize Task Table */
   
    for(i = 0; i < OS_MAX_TASKS; i++)
    {
        OS_task_table[i].free        = TRUE;
        OS_task_table[i].creator     = UNINITIALIZED;
        strcpy(OS_task_table[i].name,"");    
    }

    /* Initialize Message Queue Table */

    for(i = 0; i < OS_MAX_QUEUES; i++)
    {
        OS_queue_table[i].free        = TRUE;
        OS_queue_table[i].id          = UNINITIALIZED;
        OS_queue_table[i].creator     = UNINITIALIZED;
        strcpy(OS_queue_table[i].name,""); 
    }

    /* Initialize Binary Semaphore Table */

    for(i = 0; i < OS_MAX_SEMAPHORES; i++)
    {
        OS_bin_sem_table[i].free        = TRUE;
        OS_bin_sem_table[i].creator     = UNINITIALIZED;;
        strcpy(OS_bin_sem_table[i].name,"");
    }

    /* Initialize Mutex Semaphore Table */

    for(i = 0; i < OS_MAX_MUTEXES; i++)
    {
        OS_mut_sem_table[i].free        = TRUE;
        OS_mut_sem_table[i].creator     = UNINITIALIZED;;
        strcpy(OS_mut_sem_table[i].name,"");
    }
   
   ret = pthread_key_create(&thread_key, NULL );
   if ( ret != 0 )
   {
      printf("Error creating thread key\n");
   }
   
   sem_init(&(OS_task_table_sem),
            0 , /* no process sharing */
            OS_SEM_FULL) ; /* initial value */

   sem_init(&(OS_queue_table_sem),
            0 , /* no process sharing */
            OS_SEM_FULL) ; /* initial value */

   sem_init(&(OS_bin_sem_table_sem),
            0 , /* no process sharing */
            OS_SEM_FULL) ; /* initial value */

   sem_init(&(OS_mut_sem_table_sem),
            0 , /* no process sharing */
            OS_SEM_FULL) ; /* initial value */

#ifdef QUEUE_SELF
   if(0 != Init_Allqueues())
		printf(" Init_Allqueues error in OS_API_Init\n ");
#endif


#if defined(OS_MEMORY_TRACE)
	
	OS_MemInit();
	mem_init=TRUE;
#endif
   
}/* end OS_API_Init */

/*
**********************************************************************************
**          TASK API
**********************************************************************************
*/



/*---------------------------------------------------------------------------------------
  Function: OS_TaskCreate
   
   Parameters:	        task_id		:address where the index in the OS_task_table[OS_MAX_TASKS] of the created task saved in  
   				task_name :the name of task 
   				function_pointer:the function of task
   				 Param:  param pass to task function 
   				stack_pointer    :not used ,exist  here just for compatible 
   				stack_size		:the size of task ,should be large than OS21_DEF_MIN_STACK_SIZE( 16*1024 BYTE) and align to 4 BYTE
   				priority			:priority of task (0-255)
   				flags			:should be 1 or 2
   							(1)    task_flags_suspended      :after the task is created,it's state been set to suspend 
							(2)     task_flags_no_min_stack_size  :if set this flag ,donn't return error when stak_size 
								small than  OS21_DEF_MIN_STACK_SIZE  
   				
   
   Description: Creates a task and starts running it.

    returns	: OS_INVALID_POINTER:        if any of the necessary pointers are NULL
                     OS_ERR_NAME_TOO_LONG: if the name of the task is too long to be copied
                     OS_ERR_INVALID_PRIORITY: if the priority is bad
                     OS_ERR_NO_FREE_IDS		: if there can be no more tasks created
            	    OS_ERR_NAME_TAKEN 		:if the name specified is already used by a task
            	    OS_ERROR 					:if the operating system calls fail
            	    OS_SUCCESS 				:if success

    Author : 			ZDS    2008/3/25


---------------------------------------------------------------------------------------*/
S32             OS_TaskCreate (U32 *task_id, const char *task_name,
                                                void* (* function_pointer)(void* Param),void* Param,
                                                void *stack_pointer, U32 stack_size, U32 priority,U32 flags)
{
    int                return_code = 0;
    int                possible_taskid;
#if 0
    pthread_attr_t     custom_attr ;
    struct sched_param priority_holder ;
    U32                local_stack_size;
    int                ret;
#endif
    int                i;

    /* we don't want to allow names too long*/
    /* if truncated, two names might be the same */
    
    /* Check for NULL pointers */
    
    if( (task_name == NULL) || (function_pointer == NULL) || (task_id == NULL) )
        return OS_INVALID_POINTER;
    
    if (strlen((char *)task_name) > OS_MAX_API_NAME)
            return OS_ERR_NAME_TOO_LONG;

    /* Check for bad priority */

    if (priority > MAX_PRIORITY)
        return OS_ERR_INVALID_PRIORITY;

    
    /* Check Parameters */

    sem_wait( & (OS_task_table_sem));
    for(possible_taskid = 0; possible_taskid < OS_MAX_TASKS; possible_taskid++)
    {
        if (OS_task_table[possible_taskid].free  == 1)
        {
            break;
        }
    }
    sem_post( & (OS_task_table_sem));


    
    /* Check to see if the id is out of bounds */
    
    if( possible_taskid >= OS_MAX_TASKS || OS_task_table[possible_taskid].free != TRUE)
        return OS_ERR_NO_FREE_IDS;

    /* Check to see if the name is already taken */

    sem_wait( & (OS_task_table_sem));

    for (i = 0; i < OS_MAX_TASKS; i++)
    {
        if ((OS_task_table[i].free == 0) &&
           ( strcmp((char*) task_name, OS_task_table[i].name) == 0)) 
        {        
            sem_post( & (OS_task_table_sem));
            return OS_ERR_NAME_TAKEN;
        }
    }
  
    sem_post( & (OS_task_table_sem));

#if 0
    if ( stack_size < PTHREAD_STACK_MIN )
       local_stack_size = PTHREAD_STACK_MIN;
    else
        local_stack_size = stack_size;


    /*
    ** Set stack size
    */

    /* this statement is need for cygwin. If it is not here, cygwin give a device or resource
     * busy error. So we clear the memory of the custom attribute
    */

    memset(&custom_attr, 0, sizeof(pthread_attr_t));

    if(pthread_attr_init(&custom_attr))
    {
        printf("pthread_attr_init error in OS_TaskCreate, Task ID = %d\n",possible_taskid);
        return(OS_ERROR);
    }

    
    if (pthread_attr_setstacksize(&custom_attr, (size_t)local_stack_size ))
    {
        printf("pthread_attr_setstacksize error in OS_TaskCreate, Task ID = %d\n",possible_taskid);
        /* return(OS_ERROR);*/
    }
        
    /* 
    ** Set priority 
    */
   

	ret =pthread_attr_setschedpolicy(&custom_attr, SCHED_RR);

	priority_holder.sched_priority = sched_get_priority_max(SCHED_RR) - priority ;
    ret |= pthread_attr_setschedparam(&custom_attr,&priority_holder);

    if(ret !=0)
    {
    
       // There are some problems with the pthread implementation in Linux.. Some of 
       // these directives fail.
  /*         printf("pthread_attr_setschedparam error in OS_TaskCreate, Task ID= %d ",possible_taskid);
       if(ret == EINVAL)
           printf("EINVAL\n");
       if(ret == ENOTSUP)
           printf("ENOTSUP\n");
  
       return(OS_ERROR);*/
    }
#endif
        /*
    ** Create thread
    */
    return_code = pthread_create(&(OS_task_table[possible_taskid].id),
                                 NULL,//&custom_attr,
                                 function_pointer,
                                 (void*)Param);
    if (return_code != 0)
    {
        printf("pthread_create error in OS_TaskCreate, Task ID = %d\n",possible_taskid);
        return(OS_ERROR);
    }

	printf("\n\n======== OS_TaskCreate: create task [%s] success ========\n", task_name);

    *task_id = possible_taskid;

    /* this Id no longer free */
    

    sem_wait( & (OS_task_table_sem));

    strcpy(OS_task_table[*task_id].name, (char*) task_name);
    OS_task_table[possible_taskid].free = FALSE;
    OS_task_table[possible_taskid].creator = OS_FindCreator();
    OS_task_table[possible_taskid].stack_size = stack_size;
    OS_task_table[possible_taskid].priority = MAX_PRIORITY - priority;

    sem_post( & (OS_task_table_sem));

    return OS_SUCCESS;
}/* end OS_TaskCreate */


/*--------------------------------------------------------------------------------------
    Function: OS_TaskDelete
   
   Parameters:	task_id		:The index of the task in  OS_task_table[OS_MAX_TASKS],which you want to delete
   				
   				
   
   Description: Deletes the specified Task and removes it from the OS_task_table.

    returns	:  OS_ERR_INVALID_ID		:if the ID given to it is invali
            	    OS_ERROR 					:if the OS delete call fails
            	    OS_SUCCESS 				:if success

    Author : 			ZDS    2008/3/21
---------------------------------------------------------------------------------------*/
S32 OS_TaskDelete (U32 task_id)
{    
    /* Check to see if the task_id given is valid */
    int ret;
    if (task_id >= OS_MAX_TASKS || OS_task_table[task_id].free == TRUE)
    {
        return OS_ERR_INVALID_ID;
    }

    /* Try to delete the task */
    ret = pthread_join(OS_task_table[task_id].id,NULL);
    if (ret != 0)
    {
         printf("task_dalete error");
        return OS_ERROR;
    }  
   
     
    /*
     * Now that the task is deleted, remove its 
     * "presence" in OS_task_table
    */

    sem_wait( & (OS_task_table_sem));
    OS_task_table[task_id].free = TRUE;
    OS_task_table[task_id].id = UNINITIALIZED;
    strcpy(OS_task_table[task_id].name, "");
    OS_task_table[task_id].creator = UNINITIALIZED;
    OS_task_table[task_id].stack_size = UNINITIALIZED;
    OS_task_table[task_id].priority = UNINITIALIZED;
    sem_post( & (OS_task_table_sem));

    
    return OS_SUCCESS;
    
}/* end OS_TaskDelete */

/* {{{   add by zyliu */
S32 OS_DeleteTaskRecord (U32 task_id)
{
    /* Check to see if the task_id given is valid */
    int ret;
    if (task_id >= OS_MAX_TASKS || OS_task_table[task_id].free == TRUE)
            return OS_ERR_INVALID_ID;

    sem_wait( & (OS_task_table_sem));
    OS_task_table[task_id].free = TRUE;
    OS_task_table[task_id].id = UNINITIALIZED;
    strcpy(OS_task_table[task_id].name, "");
    OS_task_table[task_id].creator = UNINITIALIZED;
    OS_task_table[task_id].stack_size = UNINITIALIZED;
    OS_task_table[task_id].priority = UNINITIALIZED;
    sem_post( & (OS_task_table_sem));
	return OS_SUCCESS;
}

/*  add by zyliu }}}*/

S32 OS_TaskDelete_Android (U32 task_id)
{    
    int ret;
    if (task_id >= OS_MAX_TASKS || OS_task_table[task_id].free == TRUE)
    {
        return OS_ERR_INVALID_ID;
    }
    sem_wait( & (OS_task_table_sem));
    OS_task_table[task_id].free = TRUE;
    OS_task_table[task_id].id = UNINITIALIZED;
    strcpy(OS_task_table[task_id].name, "");
    OS_task_table[task_id].creator = UNINITIALIZED;
    OS_task_table[task_id].stack_size = UNINITIALIZED;
    OS_task_table[task_id].priority = UNINITIALIZED;
    sem_post( & (OS_task_table_sem));
    return OS_SUCCESS;
}/* end OS_TaskDelete */
/*=============================================================================
   Function: OS_TaskDataSet
   
   Parameters:	        task_id:		The index of the task in  OS_task_table[OS_MAX_TASKS],which you want to set to
   				data :		the point of data want to set
   				
   				
   
    Description: because the libthread not support set thread private data from other thread,so this function 
    is illegal on linux platform

    returns	:OS_ERROR

    Author : 			ZDS    2008/3/25
*===========================================================================*/
S32 OS_TaskDataSet (U32 task_id, void *data)
{
    return OS_ERROR;
   
}/* end OS_TaskDataSet */


/*=============================================================================
   Function: OS_TaskDataGet
   
   Parameters:	        task_id:		The index of the task in  OS_task_table[OS_MAX_TASKS],which you want to get from

    Description: because the libthread not support set thread private data from other thread,so this function 
    			is illegal on linux platform

    returns	:OS_ERROR

    Author : 			ZDS    2008/3/25
*===========================================================================*/
void * OS_TaskDataGet (U32 task_id)
{
     return NULL;
   
}/* end OS_TaskDataGet */
/*---------------------------------------------------------------------------------------
    Function: OS_TaskDelay
   
   Parameters:	millisecond		:how many millisecond you want to the task delay
   				
   				
   
   Description:Delay a task for specified amount of milliseconds

    returns	:  OS_ERROR 					:if sleep fails
            	    OS_SUCCESS 				:if success

    Author : 			ZDS    2008/2/21
---------------------------------------------------------------------------------------*/
S32 OS_TaskDelay(U32 millisecond )
{
	if(millisecond < 1000)
	{
		if (usleep(millisecond * 1000 ) != 0)
		{
			return OS_ERROR;
		}
		else
		{
			return OS_SUCCESS;
		}
	}
	else
	{
		if (sleep(millisecond / 1000 ) != 0)
		{
			return OS_ERROR;
		}
		else
		{
			return OS_SUCCESS;
		}
	}

} /* end OS_TaskDelay */
/*=============================================================================
   Function: OS_Reschedule
   
   Parameters:	NONE
   
   Description:the  task  voluntarily give up control of the CPU

    returns	:  NONE

    Author : 			ZDS    2008/3/25
*===========================================================================*/
void OS_Reschedule(void)
{
	sched_yield();
}
/*---------------------------------------------------------------------------------------
  Function: OS_TaskSetPriority
   
   Parameters:	task_id		:the index of the given task in OS_task_table[]
   				new_priority:the new priority you want to set to the task
   				
   
   Description:Sets the given task to a new priority

    returns	: OS_ERR_INVALID_ID 			:if the ID passed to it is invalid
            	  OS__ERR_INVALID_PRIORITY		:if the priority is greater than the max allowed
            	  >=0  					: the preview priority of the task

    Author : 			ZDS    2008/2/21
---------------------------------------------------------------------------------------*/
S32 OS_TaskSetPriority (U32 task_id, U32 new_priority)
{
    pthread_attr_t     custom_attr ;
    struct sched_param priority_holder ;

    if(task_id >= OS_MAX_TASKS || OS_task_table[task_id].free == TRUE)
        return OS_ERR_INVALID_ID;

    /* Maybe check for priority >MAX_PRIORITY, but I don;t know if there isa constant for that */
    
    if (new_priority > MAX_PRIORITY)
        return OS_ERR_INVALID_PRIORITY;
    
    /* 
    ** Set priority -- This is currently incomplete ..
    */

    /* this statement is need for cygwin. If it is not here, cygwin give a device or resource
     * busy error. So we clear the memory of the custom attribute
    */
    memset(&custom_attr, 0, sizeof(pthread_attr_t));

    priority_holder.sched_priority = MAX_PRIORITY - new_priority ;
    if(pthread_attr_setschedparam(&custom_attr,&priority_holder))
    {
       printf("pthread_attr_setschedparam error in OS_TaskSetPriority, Task ID = %d\n",task_id);
       return(OS_ERROR);
    }

    /* Change the priority in the table as well */
    OS_task_table[task_id].priority = MAX_PRIORITY - new_priority;

   return OS_SUCCESS;
}/* end OS_TaskSetPriority */
/*--------------------------------------------------------------------------------------
    Function: OS_TaskGetIdByName
   
   Parameters:	task_id			:the address where found task id to be copy into
   				task_name       :the task name of the task you want find
   
   Description:This function tries to find a task Id given the name of a task

    Returns: OS_INVALID_POINTER 	:if the pointers passed in are NULL
             OS_ERR_NAME_TOO_LONG 	:if th ename to found is too long to begin with
             OS_ERR_NAME_NOT_FOUND	: if the name wasn't found in the table
             OS_SUCCESS					: if SUCCESS

    Author : 			ZDS    2008/3/25
---------------------------------------------------------------------------------------*/

S32 OS_TaskGetIdByName (U32 *task_id, const char *task_name)
{
    U32 i;

    if (task_id == NULL || task_name == NULL)
        return OS_INVALID_POINTER;
    
    /* we don't want to allow names too long because they won't be found at all */
    
    if (strlen((char*)task_name) > OS_MAX_API_NAME)
            return OS_ERR_NAME_TOO_LONG;

    for (i = 0; i < OS_MAX_TASKS; i++)
    {
        if((OS_task_table[i].free != TRUE) &&
          (strcmp(OS_task_table[i].name,(char*) task_name) == 0 ) )
        {
            *task_id = i;
            return OS_SUCCESS;
        }
    }
    /* The name was not found in the table,
     *  or it was, and the task_id isn't valid anymore */
    return OS_ERR_NAME_NOT_FOUND;

}/* end OS_TaskGetIdByName */       

/*---------------------------------------------------------------------------------------
    Function: OS_TaskGetCurrentId
   
   Parameters:	        task_id			:the address where found task id to be copy into
   				
   
   Description:This function tries to find a task Id running

    Returns: OS_ERROR	: if some error occur
             	  OS_SUCCESS	: if SUCCESS

    Author : 			ZDS    2008/2/21
---------------------------------------------------------------------------------------*/
S32 OS_TaskGetCurrentId (U32 *taskid)
{ 
	pthread_t    pthread_id;
	U16 i;
	
	pthread_id = pthread_self();

	/*
	** Look our task ID in table 
	*/
	for(i = 0; i < OS_MAX_TASKS; i++)
	{
		if(OS_task_table[i].free != TRUE&&OS_task_table[i].id == pthread_id ) 
		{
			*taskid = i;
			return OS_SUCCESS;
		}
	}
	return OS_ERROR;
	
} /* end OS_TaskGetId */

/*---------------------------------------------------------------------------------------
    Function: OS_TaskGetInfo
   
   Parameters:	task_id			:the index in OS_task_table[] of the given task 
   				task_prop       :the address where the info of the task copy into
   
   Description:his function will pass back a pointer to structure that contains 
             		all of the relevant info (creator, stack size, priority, name) about the 
            		specified task. 

    Returns: OS_ERR_INVALID_ID 			:if the ID passed to it is invalid
             	OS_INVALID_POINTER 			:if the task_prop pointer is NULL
             	OS_SUCCESS 					:if it copied all of the relevant info over

    Author : 			ZDS    2008/3/25
---------------------------------------------------------------------------------------*/
S32 OS_TaskGetInfo (U32 task_id, OS_task_prop_t *task_prop)  
{
    /* Check to see that the id given is valid */
    
    if (task_id >= OS_MAX_TASKS || OS_task_table[task_id].free == TRUE)
        return OS_ERR_INVALID_ID;

    if( task_prop == NULL)
        return OS_INVALID_POINTER;

    /* put the info into the stucture */
    sem_wait( & (OS_task_table_sem));
    task_prop->creator =    OS_task_table[task_id].creator;
    task_prop->stack_size = OS_task_table[task_id].stack_size;
    task_prop->priority =   OS_task_table[task_id].priority;
    
    strcpy(task_prop->name, OS_task_table[task_id].name);
    sem_post( & (OS_task_table_sem));

    return OS_SUCCESS;
    
} /* end OS_TaskGetInfo */
/*=============================================================================
   Function: OS_TaskSttbxInfo
   
   Parameters: 	taskname			:the name of task you want to get information 
   				     
   
   Description:this function will print the information about task,include  
             		stack base , stack size, stack used (if valid) ,task time (if valid ), state). 
             		if you pass the  parameter taskname NULL,it will print all of task's inforamtion ,
             		else ,it will print information of the task in which you interesting .
    Returns:          NULL;
    Author : 			ZDS    2008/3/25
*===========================================================================*/
void OS_TaskSttbxInfo (char * taskname)  
{
   return;
    
} /* end OS_TaskSttbxInfo */

/****************************************************************************************
                                MESSAGE QUEUE API
****************************************************************************************/


/*---------------------------------------------------------------------------------------
  Function: OS_MessageQueueCreate
   
   Parameters:	queue_id			:address where the index in OS_queue_table[] of 
  									 the created queue 
   				queue_name       	:the name of the created queue 
   				queue_depth		:deepth of the created queue
   				data_size			:data size of the created queue
   				flags				:additional flag of the created,in current 
   										version it should be set to NULL
   
   Description: Create a message queue which can be refered to by name or ID

    Returns: OS_INVALID_POINTER 		:if a pointer passed in is NULL
            	OS_ERR_NAME_TOO_LONG	: if the name passed in is too long
            	OS_ERR_NO_FREE_IDS 		:if there are already the max queues created
            	OS_ERR_NAME_TAKEN 		:if the name is already being used on another queue
            	OS_ERROR 					:if the OS create call fails
            	OS_SUCCESS 					:if success

    Author : 			ZDS    2008/3/25             
*
*NOTE: this function should not call direct,to use this funtion should use the macro
*		which define in osapi.h for this function. 
---------------------------------------------------------------------------------------*/
S32 OS_MessageQueueCreate (U32 *queue_id, const char *queue_name, U32 queue_depth,
                       U32 data_size, U32 flags)
{
#ifdef QUEUE_SOCKET
   int                     tmpSkt;
   struct sockaddr_in      servaddr;	
    int                     returnStat;
  
#endif
#ifdef QUEUE_MQ
   int		          tmpSkt;
   struct mq_attr        mqattr;

#endif
#ifdef QUEUE_SELF
   message_queue_t     *  tmpSkt=NULL;
#endif
 
  
   int                     i;
   U32                  possible_qid;

    if ( queue_id == NULL || queue_name == NULL)
        return OS_INVALID_POINTER;

    /* we don't want to allow names too long*/
    /* if truncated, two names might be the same */

    if (strlen((char*)queue_name) > OS_MAX_API_NAME)
            return OS_ERR_NAME_TOO_LONG;

   /* Check Parameters */

    sem_wait( & (OS_queue_table_sem));

    for(possible_qid = 0; possible_qid < OS_MAX_QUEUES; possible_qid++)
    {
        if (OS_queue_table[possible_qid].free == TRUE)
            break;
    }      
    sem_post( & (OS_queue_table_sem));

    if( possible_qid >= OS_MAX_QUEUES || OS_queue_table[possible_qid].free != TRUE)
        return OS_ERR_NO_FREE_IDS;

    /* Check to see if the name is already taken */

    sem_wait( & (OS_queue_table_sem));

    for (i = 0; i < OS_MAX_QUEUES; i++)
    {
        if (strcmp ((char*) queue_name, OS_queue_table[i].name) == 0)
        {
            sem_post( & (OS_queue_table_sem));
            return OS_ERR_NAME_TAKEN;
        }
    }  

    sem_post( & (OS_queue_table_sem));

#ifdef QUEUE_SOCKET	
 
    tmpSkt = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(0> tmpSkt)
    {
	printf("socket failed on OS_QueueCreate. errno = %d\n",errno);
       return OS_ERROR;
    }
   printf("spossible_qid = %d\n",possible_qid);
   memset(&servaddr, 0, sizeof(servaddr));
   servaddr.sin_family      = AF_INET;
   servaddr.sin_port        = htons(OS_BASE_PORT + possible_qid);
   servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 

   /* 
   ** bind the input socket to a pipe
   ** port numbers are OS_BASE_PORT + queue_id
   */
   returnStat = bind(tmpSkt,(struct sockaddr *)&servaddr, sizeof(servaddr));
   
   if ( returnStat == -1 )
   {
      printf("bind failed on OS_QueueCreate. errno = %d\n",errno);
      return OS_ERROR;
   }
#endif 
#ifdef QUEUE_MQ
   memset(&mqattr,0,sizeof(mqattr));
   mqattr.mq_maxmsg=queue_depth;
   mqattr.mq_msgsize=data_size;
   tmpSkt = mq_open(queue_name, O_RDWR|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR, &mqattr);
   if ( tmpSkt == -1 )
   {
      printf("mq_open failed on OS_QueueCreate. errno = %d\n",errno);
      return OS_ERROR;
   }
#endif
#ifdef QUEUE_SELF
   tmpSkt=message_create_queue_timeout(data_size,queue_depth);
   if(tmpSkt == NULL)
   {
      printf("message_create_queue_timeout failed on OS_QueueCreate\n");
      return OS_ERROR;
   }
   
#endif
   
   /*
   ** store socket handle
   */
   *queue_id = possible_qid;
   sem_wait( & (OS_queue_table_sem));

#ifdef QUEUE_SELF
   OS_queue_table[*queue_id].size = data_size;
   OS_queue_table[*queue_id].elemnum = queue_depth;   
#endif

   OS_queue_table[*queue_id].id = tmpSkt;
   OS_queue_table[*queue_id].free = FALSE;
   strcpy( OS_queue_table[*queue_id].name, (char*) queue_name);
   OS_queue_table[*queue_id].creator = OS_FindCreator();
   sem_post( & (OS_queue_table_sem));
   
   return OS_SUCCESS;
    
} /* end OS_MessageQueueCreate */


/*--------------------------------------------------------------------------------------
    Function: OS_MessageQueueDelete
   
   Parameters:	queue_id		:index in OS_queue_table[] of the given queue
  									
   
   Description: Deletes the specified message queue.

    Returns: OS_ERR_INVALID_ID		 if the id passed in does not exist
             	OS_ERROR 				if the OS call to delete the queue fails 
             	OS_SUCCESS 				if success

    Author : 			ZDS    2008/3/25
*
*NOTE: this function should not call direct,to use this funtion should use the macro
*		which define in osapi.h for this function. 
---------------------------------------------------------------------------------------*/

S32 OS_MessageQueueDelete (U32 queue_id)
{
    /* Check to see if the queue_id given is valid */

    if (queue_id >= OS_MAX_QUEUES || OS_queue_table[queue_id].free == TRUE)
            return OS_ERR_INVALID_ID;

    /* Try to delete the queue */
#ifdef QUEUE_SOCKET
    if(close(OS_queue_table[queue_id].id) !=0)   
    {
        return OS_ERROR;
    }
#endif
#ifdef QUEUE_MQ
    if(mq_close(OS_queue_table[queue_id].id) !=0)   
    {
        return OS_ERROR;
    }
#endif
#ifdef QUEUE_SELF
     if(message_delete_queue(OS_queue_table[queue_id].id) !=0)   
    {
        return OS_ERROR;
    }
#endif
        
    /* 
     * Now that the queue is deleted, remove its "presence"
     * in OS_message_q_table and OS_message_q_name_table 
    */
        
    sem_wait( & (OS_queue_table_sem));

    OS_queue_table[queue_id].free = TRUE;
    strcpy(OS_queue_table[queue_id].name, "");
    OS_queue_table[queue_id].creator = UNINITIALIZED;
    OS_queue_table[queue_id].id = UNINITIALIZED;

#ifdef QUEUE_SELF
   OS_queue_table[queue_id].size = UNINITIALIZED;
   OS_queue_table[queue_id].elemnum = UNINITIALIZED;   
#endif
    
    sem_post( & (OS_queue_table_sem));
   
    return OS_SUCCESS;

} /* end OS_MessageQueueDelete */

/*---------------------------------------------------------------------------------------
    Function: OS_MessageQueueReceive
   
   Parameters:		queue_id		:index in OS_queue_table[] of the given queue
   				data			:address where got data will be copy into
   				size				:the size of data want to get
   				size_copied		:adress where size of real got data will be 
   									copy into
   				timeout			:in current version it have three 
   									optional:OS_PEND or OS_CHECK or other values,OS_PEND means 
   									wait infinity untile get the message,OS_CKECK means no wait,
   									other values means wait the value milisecend time
  									
   
   Description:  Receive a message on a message queue.  Will pend or timeout on the receive.

    Returns: OS_ERR_INVALID_ID 			if the given ID does not exist
               	OS_ERR_INVALID_POINTER 	if a pointer passed in is NULL
            	OS_QUEUE_EMPTY				 if the Queue has no messages on it to be recieved
            	
            	n (n>=0) 					if success ,return the number of message remain in the queue 
            	
   NOTICE:   because OS20 haven't support timeout tick for MESSAGE_QUEUE, so in this version,the time out argument 
            is simplely support OS_PEND and OS_CHECK,but not support timeout ticks. 
            besides, OS20 haven't support variable size  byte of got message in message_receive_timeout() function,
            so in this version,argument size and size_copied is not in use

    Author : 			ZDS    2008/3/25
    		
*
*NOTE: this function should not call direct,to use this funtion should use the macro
*		which define in osapi.h for this function. 
---------------------------------------------------------------------------------------*/
S32 OS_MessageQueueReceive (U32 queue_id, void *data, U32 size, U32 *size_copied, S32 timeout)
{

    int sizeCopied;
#ifdef QUEUE_SOCKET   
    int flags;
#endif
#ifdef QUEUE_MQ
    struct mq_attr mqattr;
#endif
#ifdef QUEUE_SELF
    char * msg=NULL;
#endif
    U32 return_num=0;

    /*
    ** Check Parameters 
    */
    /* Check Parameters */

    if(queue_id >= OS_MAX_QUEUES || OS_queue_table[queue_id].free == TRUE)
    {
        return OS_ERR_INVALID_ID;
    }
    else
    {
        if( (data == NULL) || (size_copied == NULL) )
        {
            return OS_INVALID_POINTER;
        }
    }

#ifdef QUEUE_SELF
    if (size != OS_queue_table[queue_id].size)
    {
        return OS_QUEUE_INVALID_SIZE;
    }
#endif

    /*
    ** Read the socket for data
    */
    if (timeout == OS_PEND) 
    {   
#ifdef QUEUE_SOCKET   
        fcntl(OS_queue_table[queue_id].id,F_SETFL,0);
        sizeCopied = recvfrom(OS_queue_table[queue_id].id, data, size, 0, NULL, NULL);
        if(sizeCopied != size )
        {
            *size_copied = 0;
            return(OS_QUEUE_INVALID_SIZE);
        }
#endif
#ifdef QUEUE_MQ
        sizeCopied = mq_receive(OS_queue_table[queue_id].id, data, size, NULL);
        if(sizeCopied != size )
        {
            *size_copied = 0;
            return(OS_QUEUE_INVALID_SIZE);
        }
#endif
#ifdef  QUEUE_SELF
        msg=(char *)message_receive(OS_queue_table[queue_id].id);
        if(NULL == msg )
        {
            *size_copied = 0;
            return(OS_QUEUE_INVALID_SIZE);
        }
#endif
    }
    else if (timeout == OS_CHECK)
    {    
#ifdef QUEUE_SOCKET   
        flags = fcntl(OS_queue_table[queue_id].id, F_GETFL, 0);
        fcntl(OS_queue_table[queue_id].id,F_SETFL,flags|O_NONBLOCK);

        sizeCopied = recvfrom(OS_queue_table[queue_id].id, data, size, 0, NULL, NULL);

        fcntl(OS_queue_table[queue_id].id,F_SETFL,flags);

        if (sizeCopied == -1 && errno == EWOULDBLOCK )
        {
            *size_copied = 0;
            printf("empty\n");
            return OS_QUEUE_EMPTY;
        }
        else if(sizeCopied != size )
        {
            *size_copied = 0;
            return(OS_QUEUE_INVALID_SIZE);
        }
#endif
#ifdef QUEUE_MQ
        if(0 != mq_getattr(OS_queue_table[queue_id].id,&mqattr))
        {
            *size_copied = 0;
            return(OS_QUEUE_INVALID_SIZE);
        }
        mqattr.mq_flags|=O_NONBLOCK;
        if(0 != mq_setattr(OS_queue_table[queue_id].id,&mqattr,NULL))
        {
            *size_copied = 0;
            return(OS_QUEUE_INVALID_SIZE);
        }
        mqattr.mq_flags&=~O_NONBLOCK;
        sizeCopied = mq_receive(OS_queue_table[queue_id].id, data, size, NULL);
        if(sizeCopied != size )
        {
            mq_setattr(OS_queue_table[queue_id].id,&mqattr,NULL);
            *size_copied = 0;
            return(OS_QUEUE_INVALID_SIZE);
        }
        mq_setattr(OS_queue_table[queue_id].id,&mqattr,NULL);

#endif
#ifdef QUEUE_SELF
        msg = (char *) message_receive_timeout(OS_queue_table[queue_id].id,0);
        if(msg == NULL)
        {
            *size_copied = 0;
            return OS_QUEUE_EMPTY;
        }

#endif
    }
    else if(timeout >0) /* timeout */ 
    {
#ifdef QUEUE_SOCKET 
        int timeloop;

        flags = fcntl(OS_queue_table[queue_id].id, F_GETFL, 0);
        fcntl(OS_queue_table[queue_id].id,F_SETFL,flags|O_NONBLOCK);

        /*
        ** This "timeout" will check the socket for data every 10 milliseconds
        ** up until the timeout value. Although this works fine for a desktop environment,
        ** it should be written more efficiently for a flight system.
        ** The proper way will be to use select or poll with a timeout
        */
        for ( timeloop = timeout; timeloop > 0; timeloop = timeloop - 10 )
        {
            sizeCopied = recvfrom(OS_queue_table[queue_id].id, data, size, 0, NULL, NULL);

            if ( sizeCopied == size )
            {
                *size_copied = sizeCopied;
                fcntl(OS_queue_table[queue_id].id,F_SETFL,flags);
                sem_wait( & (OS_queue_table_sem));
                OS_queue_table[queue_id].count--; 
                return_num=OS_queue_table[queue_id].count;
                sem_post( & (OS_queue_table_sem));
                return return_num;

            }
            else if (sizeCopied == -1 && errno == EWOULDBLOCK )
            {
                /*
                ** Sleep for 10 milliseconds
                */
                usleep(10 * 1000);
            }
            else
            {
                *size_copied = 0;
                fcntl(OS_queue_table[queue_id].id,F_SETFL,flags);
                return OS_QUEUE_INVALID_SIZE;
            }
        }
        fcntl(OS_queue_table[queue_id].id,F_SETFL,flags);
        return(OS_QUEUE_TIMEOUT);
#endif
#ifdef QUEUE_MQ
        struct timespec  temp_timespec ;
        /*
        ** Compute an absolute time for the delay
        */
        OS_CompAbsDelayedTime( timeout , &temp_timespec) ;
        sizeCopied = mq_timedreceive(OS_queue_table[queue_id].id, data, size, NULL,&temp_timespec);
        if(sizeCopied != size )
        {
            if(-1 == sizeCopied && errno == ETIMEDOUT)
            {
                return OS_SEM_TIMEOUT;
            }
            else
            {
                return OS_ERROR;
            }
        }


#endif
#ifdef QUEUE_SELF
        msg = (char *) message_receive_timeout(OS_queue_table[queue_id].id,timeout);
        if(msg == NULL)
        {
            *size_copied = 0;
            return OS_QUEUE_EMPTY;
        }
#endif
    } /* END timeout */
    else
    {
        *size_copied = 0;
        return OS_ERROR;
    }

#ifdef QUEUE_SELF
    memcpy(data,msg,OS_queue_table[queue_id].size);
    sizeCopied=OS_queue_table[queue_id].size;
    message_release(OS_queue_table[queue_id].id, msg);  
#endif
    /*
    ** Should never really get here.
    */
    *size_copied = sizeCopied;
    sem_wait( & (OS_queue_table_sem));
    OS_queue_table[queue_id].count--; 
    return_num=OS_queue_table[queue_id].count;
    sem_post( & (OS_queue_table_sem));

    return return_num;

}/* end OS_MessageQueueReceive */


/*---------------------------------------------------------------------------------------
   Function: OS_MessageQueueSend
   
   Parameters:	queue_id		:index in OS_queue_table[] of the given queue
   				data			:address where the  data want to be put
   				size				:the size of data want to put
   				timeout			:in current version it have three 
   									optional:OS_PEND or OS_CHECK or other values,OS_PEND means 
   									wait infinity untile get the message,OS_CKECK means no wait,
   									other values means wait the value ticks time
   				flags			:no mean in current version,reserved for future used
  									
   
   Description: Put a message on a message queue.

    Returns: 	OS_ERR_INVALID_ID 		if the queue id passed in is not a valid queue
            		OS_INVALID_POINTER 		if the data pointer is NULL
            		OS_QUEUE_FULL 			if the queue cannot accept another message
           		OS_SUCCESS 				if SUCCESS   
            	
   NOTICE: The flags parameter is not used.  The message put is always configured to
            immediately return an error if the receiving message queue is full.

    Author : 			ZDS    2008/3/25
    			
*
*NOTE: this function should not call direct,to use thi
---------------------------------------------------------------------------------------*/
S32 OS_MessageQueueSend(U32 queue_id, void *data, U32 size, S32 timeout,U32 flags)
{

#ifdef QUEUE_SOCKET   
   struct sockaddr_in serva;
   static int socketFlags = 0;
    int bytesSent    = 0;
   int tempSkt      = 0;

#endif
#ifdef QUEUE_MQ
   struct mq_attr mqattr;
#endif
#ifdef QUEUE_SELF
   char *msg=NULL;
#endif


   /*
   ** Check Parameters 
   */
    if(queue_id >= OS_MAX_QUEUES || OS_queue_table[queue_id].free == TRUE)
        return OS_ERR_INVALID_ID;

    if (data == NULL)
        return OS_INVALID_POINTER;

#ifdef QUEUE_SELF
   if (size != OS_queue_table[queue_id].size)
        return OS_QUEUE_INVALID_SIZE;
#endif

#ifdef QUEUE_SOCKET 
   /* 
   ** specify the IP addres and port number of destination
   */
   memset(&serva, 0, sizeof(serva));
   serva.sin_family      = AF_INET;
   serva.sin_port        = htons(OS_BASE_PORT + queue_id);
   serva.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    /*
    ** open a temporary socket to transfer the packet to MR
    */
    tempSkt = socket(AF_INET, SOCK_DGRAM, 0);
#endif
    /* 
    ** send the packet to the message router task (MR)
    */
  
    if (timeout == OS_PEND) 
   {      
#ifdef QUEUE_SOCKET 
           bytesSent = sendto(tempSkt,(char *)data, size, socketFlags,
                     				(struct sockaddr *)&serva, sizeof(serva));
	    if( bytesSent != size )
	    {
                 printf("error errno %x\n",errno);
	    	close(tempSkt);
	       return(OS_QUEUE_FULL);
	    }
	    else
          {
                printf("success");
              close(tempSkt);
		 sem_wait( & (OS_queue_table_sem));
		OS_queue_table[queue_id].count++; 
		 sem_post( & (OS_queue_table_sem));
             return OS_SUCCESS;
         
          }
#endif
#ifdef QUEUE_MQ
	             				
	    if( 0 != mq_send(OS_queue_table[queue_id].id,(char *)data, size, 1))
	    {
	    	return(OS_QUEUE_FULL);
	    }
	    else
            {
               sem_wait( & (OS_queue_table_sem));
	       OS_queue_table[queue_id].count++; 
	       sem_post( & (OS_queue_table_sem));
               return OS_SUCCESS;
         
           }
#endif
#ifdef QUEUE_SELF
         msg = (char *)message_claim(OS_queue_table[queue_id].id);
	 if(msg == NULL)
         {
  		return OS_QUEUE_FULL;
         }
#endif
   }
   else if (timeout == OS_CHECK)
   {  
#ifdef QUEUE_SOCKET     
      fcntl(OS_queue_table[queue_id].id,F_SETFL,O_NONBLOCK|fcntl(tempSkt, F_GETFL, 0));
      
       bytesSent = sendto(tempSkt,(char *)data, size, socketFlags,
                     				(struct sockaddr *)&serva, sizeof(serva));

    	if ( bytesSent == -1 )  
      {
      	    close(tempSkt);
          return OS_QUEUE_FULL;
      }
      else
      {
          close(tempSkt);
	     sem_wait( & (OS_queue_table_sem));	  
	    OS_queue_table[queue_id].count++; 
	    sem_post( & (OS_queue_table_sem));	
         return OS_SUCCESS;
     
      }
#endif
#ifdef QUEUE_MQ
     if(0 != mq_getattr(OS_queue_table[queue_id].id,&mqattr))
     {
         return(OS_ERROR);
     }
     mqattr.mq_flags|=O_NONBLOCK;
     if(0 != mq_setattr(OS_queue_table[queue_id].id,&mqattr,NULL))
     {
         return(OS_ERROR);
     }
     mqattr.mq_flags&=~O_NONBLOCK;
     if(0 != mq_send(OS_queue_table[queue_id].id, data, size, 1) )
     {
         mq_setattr(OS_queue_table[queue_id].id,&mqattr,NULL);
         return(OS_QUEUE_FULL);
     }
    else
    {
          mq_setattr(OS_queue_table[queue_id].id,&mqattr,NULL);
	     sem_wait( & (OS_queue_table_sem));	  
	    OS_queue_table[queue_id].count++; 
	    sem_post( & (OS_queue_table_sem));	
         return OS_SUCCESS;
     
      }
     

#endif
#ifdef QUEUE_SELF
     msg = (char *) message_claim_timeout(OS_queue_table[queue_id].id,0);
     if(msg == NULL)
     {
	return OS_QUEUE_FULL;
     }
#endif
     

   }
   else  if (timeout > 0 )
   {
#ifdef QUEUE_SOCKET 
      int timeloop;
      
      fcntl(OS_queue_table[queue_id].id,F_SETFL,O_NONBLOCK|fcntl(tempSkt, F_GETFL, 0));

      /*
      ** This "timeout" will check the socket for data every 10 milliseconds
      ** up until the timeout value. Although this works fine for a desktop environment,
      ** it should be written more efficiently for a flight system.
      ** The proper way will be to use select or poll with a timeout
      */
      for ( timeloop = timeout; timeloop > 0; timeloop = timeloop - 10 )
      {
          bytesSent = sendto(tempSkt,(char *)data, size, socketFlags,
                     				(struct sockaddr *)&serva, sizeof(serva));

         if ( bytesSent == size )
         {
              close(tempSkt);
		 sem_wait( & (OS_queue_table_sem));	  
		OS_queue_table[queue_id].count++; 
		 sem_post( & (OS_queue_table_sem));
             return OS_SUCCESS;
         
         }
         else if (bytesSent == -1 && errno == EWOULDBLOCK )
         {
            /*
            ** Sleep for 10 milliseconds
            */
            usleep(10 * 1000);
         }
         else
         {
              close(tempSkt);
              return OS_QUEUE_FULL;
         }
      }

	close(tempSkt);
      return(OS_QUEUE_TIMEOUT);
#endif
#ifdef QUEUE_MQ
      struct timespec  temp_timespec ;
    /*
     ** Compute an absolute time for the delay
    */
    OS_CompAbsDelayedTime( timeout , &temp_timespec) ;
    
     if(0 != mq_timedsend(OS_queue_table[queue_id].id, data, size, 1,&temp_timespec) )
     {
         if( errno == ETIMEDOUT)
         {
            return OS_SEM_TIMEOUT;
         }
         else
         {
            return OS_ERROR;
         }
     }
     else
     {
            sem_wait( & (OS_queue_table_sem));	  
	    OS_queue_table[queue_id].count++; 
            sem_post( & (OS_queue_table_sem));
            return OS_SUCCESS;
      }
     

#endif
#ifdef QUEUE_SELF
     msg = (char*)message_claim_timeout(OS_queue_table[queue_id].id, timeout);
     if(msg == NULL)
     {
        return OS_QUEUE_FULL;
     }
#endif

   } /* END timeout */
	else
	{
		return OS_ERROR;
	}

#ifdef QUEUE_SELF
      memcpy(msg,data, size);

     sem_wait(&OS_queue_table_sem);
        OS_queue_table[queue_id].count++; 
     sem_post(&OS_queue_table_sem);
    
     message_send(OS_queue_table[queue_id].id, (void*) msg);
     return OS_SUCCESS;
#endif
   
} /* end OS_MessageQueueSend */







#if 1
S32 OS_MessageQueueRelease_UTI (U32 queue_id, void *data)
{

    int sizeCopied;
#ifdef QUEUE_SELF
    char * msg=NULL;
#endif
    U32 return_num=0;


    if(queue_id >= OS_MAX_QUEUES || OS_queue_table[queue_id].free == TRUE)
    {
        return OS_ERR_INVALID_ID;
    }
    else
    {
        if( data == NULL )
        {
            return OS_INVALID_POINTER;
        }
    }
    msg= data;
    
#ifdef QUEUE_SELF
    message_release(OS_queue_table[queue_id].id, msg);  
#endif
    /*
    ** Should never really get here.
    */
    sem_wait( & (OS_queue_table_sem));
    OS_queue_table[queue_id].count--; 
    return_num=OS_queue_table[queue_id].count;
    sem_post( & (OS_queue_table_sem));

    return return_num;

}/* end OS_MessageQueueReceive */


char*   OS_MessageQueueReceive_UTI (U32 queue_id, U32 size, U32 *size_copied, S32 timeout)
{

    int sizeCopied;
#ifdef QUEUE_SELF
    char * msg=NULL;
#endif
    U32 return_num=0;

    /*
    ** Check Parameters 
    */
    /* Check Parameters */

    if(queue_id >= OS_MAX_QUEUES || OS_queue_table[queue_id].free == TRUE)
    {
        return NULL;
    }
    else
    {
        if(size_copied == NULL) 
        {
            return NULL;
        }
    }

#ifdef QUEUE_SELF
    if (size != OS_queue_table[queue_id].size)
    {
        return NULL;
    }
#endif

    /*
    ** Read the socket for data
    */
    if (timeout == OS_PEND) 
    {   
#ifdef  QUEUE_SELF
		pbierror("%s.   line %d \n", __FUNCTION__,__LINE__);
        msg=(char *)message_receive(OS_queue_table[queue_id].id);
        if(NULL == msg )
        {
            *size_copied = 0;
			pbierror("%s.   line %d \n", __FUNCTION__,__LINE__);
            return NULL;
        }
#endif
    }
    else if (timeout == OS_CHECK)
    {    
#ifdef QUEUE_SELF
        msg = (char *) message_receive_timeout(OS_queue_table[queue_id].id,0);
        if(msg == NULL)
        {
            *size_copied = 0;
            return NULL;
        }

#endif
    }
    else if(timeout >0) /* timeout */ 
    {
#ifdef QUEUE_SELF
        msg = (char *) message_receive_timeout(OS_queue_table[queue_id].id,timeout);
        if(msg == NULL)
        {
            *size_copied = 0;
            return NULL;
        }
#endif
    } /* END timeout */
    else
    {
    	pbierror("%s.   line %d \n", __FUNCTION__,__LINE__);
        *size_copied = 0;
        return NULL;
    }

#ifdef QUEUE_SELF
	pbierror("%s.	line %d \n", __FUNCTION__,__LINE__);

    *size_copied = OS_queue_table[queue_id].size;
	return msg;
#endif

}/* end OS_MessageQueueReceive */


S32 OS_MessageQueueSend_UTI(U32 queue_id, void *data, U32 size,U32 flags)
{

#ifdef QUEUE_SELF
   char *msg=NULL;
#endif

    if(queue_id >= OS_MAX_QUEUES || OS_queue_table[queue_id].free == TRUE)
        return OS_ERR_INVALID_ID;

    if (data == NULL)
        return OS_INVALID_POINTER;

	msg =data;

#ifdef QUEUE_SELF
   if (size != OS_queue_table[queue_id].size)
        return OS_QUEUE_INVALID_SIZE;
#endif

#ifdef QUEUE_SELF

     sem_wait(&OS_queue_table_sem);
        OS_queue_table[queue_id].count++; 
     sem_post(&OS_queue_table_sem);
    
     message_send(OS_queue_table[queue_id].id, (void*) msg);
     return OS_SUCCESS;
#endif
   
} /* end OS_MessageQueueSend */

/*---------------------------------------------------------------------------------------
   Function: OS_MessageQueueclaim
   
*NOTE: this function should not call direct,to use thi
---------------------------------------------------------------------------------------*/
char * OS_MessageQueueclaim_UTI(U32 queue_id,U32 size, S32 timeout,U32 flags)
{

#ifdef QUEUE_SELF
   char *msg=NULL;
#endif

    if(queue_id >= OS_MAX_QUEUES || OS_queue_table[queue_id].free == TRUE)
        return NULL;


#ifdef QUEUE_SELF
   if (size != OS_queue_table[queue_id].size)
        return NULL;
#endif

  
    if (timeout == OS_PEND) 
   {      
#ifdef QUEUE_SELF

         msg = (char *)message_claim(OS_queue_table[queue_id].id);
	 	if(msg == NULL)
         {
  			return NULL;
         }
		
#endif
   }
   else if (timeout == OS_CHECK)
   {  
#ifdef QUEUE_SELF
     msg = (char *) message_claim_timeout(OS_queue_table[queue_id].id,0);
     if(msg == NULL)
     {
		return NULL;
     }
#endif
     

   }
   else  if (timeout > 0 )
   {
#ifdef QUEUE_SELF
     msg = (char*)message_claim_timeout(OS_queue_table[queue_id].id, timeout);
     if(msg == NULL)
     {
        return NULL;
     }
#endif

   } /* END timeout */
	else
	{
		return NULL;
	}
	
	
	//*data =(void *)msg;
	pbiinfo("%s.   line %d \n", __FUNCTION__,__LINE__);
	return msg;
   
} /* end OS_MessageQueueSend */







#endif


/*--------------------------------------------------------------------------------------
     Function: OS_MessageQueueGetIdByName
   
   Parameters:	queue_id		:The  id of the queue is passed back in
   				queue_name	:the name of the given id
   				
   
   Description: This function tries to find a queue Id given the name of the queue. 
   					The  id of the queue is passed back in queue_id

    Returns:    OS_INVALID_POINTER			 if the name or id pointers are NULL
             	OS_ERR_NAME_TOO_LONG 	 the name passed in is too long
             	OS_ERR_NAME_NOT_FOUND 	 the name was not found in the table
            	       OS_SUCCESS 					 if success

    Author : 			ZDS    2008/3/25
             
---------------------------------------------------------------------------------------*/

S32 OS_MessageQueueGetIdByName (U32 *queue_id, const char *queue_name)
{
    U32 i;

    if(queue_id == NULL || queue_name == NULL)
        return OS_INVALID_POINTER;

    
    /* a name too long wouldn't have been allowed in the first place
     * so we definitely won't find a name too long*/
 
    if (strlen((char*)queue_name) > OS_MAX_API_NAME)
            return OS_ERR_NAME_TOO_LONG;


    for (i = 0; i < OS_MAX_QUEUES; i++)
    {
        if (OS_queue_table[i].free != TRUE &&
                (strcmp(OS_queue_table[i].name, (char*) queue_name) == 0 ))
        {
            *queue_id = i;
            return OS_SUCCESS;
        }
    }

    /* The name was not found in the table,
     *  or it was, and the queue_id isn't valid anymore */
    return OS_ERR_NAME_NOT_FOUND;

}/* end OS_MessageQueueGetIdByName */

/*---------------------------------------------------------------------------------------
   Function: OS_MessageQueueGetInfo
   
   Parameters:	queue_id		:The  id of the given queue
   				queue_prop	:where the info of given queue will copy into
   				
   
   Description: This function will pass back a pointer to structure that contains 
             		all of the relevant info (name and creator) about the specified queue. 

    Returns:OS_INVALID_POINTER 		if queue_prop is NULL
            	 OS_ERR_INVALID_ID 		if the ID given is not  a valid queue
             	OS_SUCCESS 				if the info was copied over correctly

    Author : 			ZDS    2008/3/25
---------------------------------------------------------------------------------------*/

S32 OS_QueueGetInfo (U32 queue_id, OS_queue_prop_t *queue_prop)  
{
    /* Check to see that the id given is valid */
    
    if (queue_prop == NULL)
        return OS_INVALID_POINTER;
    
    if (queue_id >= OS_MAX_QUEUES || OS_queue_table[queue_id].free == TRUE)
        return OS_ERR_INVALID_ID;

    /* put the info into the stucture */
    sem_wait( & (OS_queue_table_sem));

    queue_prop->creator =   OS_queue_table[queue_id].creator;
    strcpy(queue_prop->name, OS_queue_table[queue_id].name);
    sem_post( & (OS_queue_table_sem));

    return OS_SUCCESS;
    
} /* end OS_MessageQueueGetInfo */

/****************************************************************************************
                                  SEMAPHORE API
****************************************************************************************/

/*---------------------------------------------------------------------------------------
  Function: OS_SemCreate
   
   Parameters:	sem_id			:address where The  id in OS_bin_sem_table[] of 
   									creating semaphore will copy into
   				sem_name		:name of the creating semaphore
   				sem_initial_value: the  initial_value of taked number of 
   									creating semaphore
   				options			 :no mean in current version,should be set to 
   									NULL
   				
   
   Description:  Creates a binary semaphore with initial value specified by
            		sem_initial_value and name specified by sem_name. sem_id will be 
            		returned to the caller

    Returns:OS_INVALID_POINTER 			if sen name or sem_id are NULL
            	OS_ERR_NAME_TOO_LONG	 if the name given is too long
            	OS_ERR_NO_FREE_IDS		 if all of the semaphore ids are taken
            	OS_ERR_NAME_TAKEN 		if this is already the name of a binary semaphore
            	OS_SEM_FAILURE				 if the OS call failed
           		 OS_SUCCESS 					if success

    Author : 			ZDS    2008/3/25
---------------------------------------------------------------------------------------*/
S32 OS_SemCreate (U32 *sem_id, const char *sem_name, U32 sem_initial_value,
                        U32 options)
{
    int                ret;
    U32 possible_semid;
    U32 i;

    if (sem_id == NULL || sem_name == NULL)
    {
        return OS_INVALID_POINTER;
    }

    /* we don't want to allow names too long*/
    /* if truncated, two names might be the same */

    if (strlen((char*)sem_name) > OS_MAX_API_NAME)
    {
        return OS_ERR_NAME_TOO_LONG;
    }

    sem_wait( & (OS_bin_sem_table_sem));

    /* Check Parameters */
    for (possible_semid = 0; possible_semid < OS_MAX_SEMAPHORES; possible_semid++)
    {
        if (OS_bin_sem_table[possible_semid].free == TRUE)    
        {
            break;
        }
    }

    if((possible_semid >= OS_MAX_SEMAPHORES) || (OS_bin_sem_table[possible_semid].free != TRUE))
    {
        sem_post( & (OS_bin_sem_table_sem));
        return OS_ERR_NO_FREE_IDS;
    }

    /* Check to see if the name is already taken */
    for (i = 0; i < OS_MAX_SEMAPHORES; i++)
    {
        if (strcmp ((char*) sem_name, OS_bin_sem_table[i].name) == 0)
        {
            sem_post( & (OS_bin_sem_table_sem));
            return OS_ERR_NAME_TAKEN;
        }
    }   

    ret = sem_init(&(OS_bin_sem_table[possible_semid].id ),
                    0 , /* no process sharing */
                    sem_initial_value) ; /* initial value */

    *sem_id = possible_semid;

    OS_bin_sem_table[*sem_id].free = FALSE;
    strcpy(OS_bin_sem_table[*sem_id].name , (char*) sem_name);
    OS_bin_sem_table[*sem_id].creator = OS_FindCreator();


    sem_post( & (OS_bin_sem_table_sem));

    return OS_SUCCESS;
}/* end OS_SemCreate */

/*--------------------------------------------------------------------------------------
     Function: OS_SemDelete
   
   Parameters:	sem_id			: id in OS_bin_sem_table[] of 
   									given semaphore want to delete
   				
   
   Description:   Deletes the specified Binary Semaphore.

    Returns:OS_ERR_INVALID_ID 		if the id passed in is not a valid binary semaphore
             	OS_ERR_SEM_NOT_FULL 	if the semahore is taken and cannot be deleted
             	OS_SUCCESS 				if success

    Author : 			ZDS    2008/3/25
---------------------------------------------------------------------------------------*/
S32 OS_SemDelete (U32 sem_id)
{
    struct timespec abstime;
    abstime.tv_sec  = 0;
    abstime.tv_nsec = 40000;
    
    /* Check to see if this sem_id is valid */
    if (sem_id >= OS_MAX_SEMAPHORES || OS_bin_sem_table[sem_id].free == TRUE)
    {
        return OS_ERR_INVALID_ID;
    }
    
    /* To make sure it is safe to delete a semaphore, we have to make sure that the 
    * semaphore is full. Therefore, if OS_BinSemDelete can successfully take the 
    * semaphore, it must have been free, and we can delete it, otherwise return OS_ERROR
    */

    if (sem_trywait( &(OS_bin_sem_table[sem_id].id)) != 0) /* 0 is success in posix */
    {
        return OS_ERR_SEM_NOT_FULL;
    }


    /* now we have successfully taken the semaphore */
    /* we must give the semaphore back to delete it */


    if (sem_destroy( &(OS_bin_sem_table[sem_id].id)) != 0) /* 0 = success */ 
    {
		printf("OS_SemDelete  line %d \n",__LINE__);
        return OS_SEM_FAILURE;
    }
    sem_wait( & (OS_bin_sem_table_sem));
    OS_bin_sem_table[sem_id].free = TRUE;
    strcpy(OS_bin_sem_table[sem_id].name , "");
    OS_bin_sem_table[sem_id].creator = UNINITIALIZED;
    sem_post( & (OS_bin_sem_table_sem));
    return OS_SUCCESS;
}/* end OS_SemDelete */
S32 OS_SemDelete1 (U32 sem_id)
{
    struct timespec abstime;
    abstime.tv_sec  = 0;
    abstime.tv_nsec = 40000;
    if (sem_id >= OS_MAX_SEMAPHORES || OS_bin_sem_table[sem_id].free == TRUE)
    {
    printf("OS_SemDelete  line %d \n",__LINE__);
        return OS_ERR_INVALID_ID;
    }
    if (sem_destroy( &(OS_bin_sem_table[sem_id].id)) != 0) /* 0 = success */ 
    {
		printf("OS_SemDelete  line %d \n",__LINE__);
        return OS_SEM_FAILURE;
    }



    /* Remove the Id from the table, and its name, so that it cannot be found again */

    sem_wait( & (OS_bin_sem_table_sem));
    OS_bin_sem_table[sem_id].free = TRUE;
    strcpy(OS_bin_sem_table[sem_id].name , "");
    OS_bin_sem_table[sem_id].creator = UNINITIALIZED;
    sem_post( & (OS_bin_sem_table_sem));

    return OS_SUCCESS;

}/* end OS_SemDelete */


/*---------------------------------------------------------------------------------------
    Function: OS_SemSignal
   
   Parameters:	sem_id			: id in OS_bin_sem_table[] of 
   							 given semaphore want to give
   				
   
   Description:   the function  unlocks the semaphore referenced by sem_id by performing
             a semaphore unlock operation on that semaphore.If the semaphore value 
             resulting from this operation is positive, then no threads were blocked             
             waiting for the semaphore to become unlocked; the semaphore value is
             simply incremented for this semaphore.


    Returns:
             OS_ERR_INVALID_ID 		if the id passed in is not a binary semaphore
             OS_SUCCESS 			if success

    Author : 			ZDS    2008/3/25
                
---------------------------------------------------------------------------------------*/

S32 OS_SemSignal( U32 sem_id )
{
    S32 ret_val ;
    int    ret;
    
    /* Check Parameters */

    if(sem_id >= OS_MAX_SEMAPHORES || OS_bin_sem_table[sem_id].free == TRUE)
        return OS_ERR_INVALID_ID;
    
    /*
    ** Get a lock on the mutex, then signal the 
    ** condition variable
    */
    ret = sem_post(&OS_bin_sem_table[sem_id].id);

    
    if ( ret != 0 )
    {
       ret_val = OS_SEM_FAILURE;
    }
    else
    {
       ret_val = OS_SUCCESS ;
    }
    
    return ret_val;
}/* end OS_SemSignal */

/*---------------------------------------------------------------------------------------
      Function: OS_SemWait
   
   Parameters:	sem_id			: 		id in OS_bin_sem_table[] of 
   									given semaphore want to take
   				
   
   Description:  The locks the semaphore referenced by sem_id by performing a 
             semaphore lock operation on that semaphore.If the semaphore value 
             is currently zero, then the calling thread shall not return from 
             the call until it either locks the semaphore or the call is 
             interrupted by a signal.


    Returns:
             OS_ERR_INVALID_ID 			the Id passed in is not a valid binar semaphore
             OS_SEM_FAILURE 			if the OS call failed
             OS_SUCCESS					 if success

    Author : 			ZDS    2008/3/25
             
----------------------------------------------------------------------------------------*/
S32 OS_SemWait ( U32 sem_id )
{
    S32 ret_val ;
    int    ret;

    if(sem_id >= OS_MAX_SEMAPHORES  || OS_bin_sem_table[sem_id].free == TRUE)
        return OS_ERR_INVALID_ID;

    /*
    ** Get a lock on the mutex, then wait for the 
    ** condition variable
    */

    ret = sem_wait(&OS_bin_sem_table[sem_id].id );

    if ( ret == 0 )
    {
       ret_val = OS_SUCCESS;
    }
    else
    {
       ret_val = OS_SEM_FAILURE;
    }

    return ret_val;
} /* end OS_SemWait */

S32 OS_SemGetValue(U32 sem_id)
{
    S32 ret_val ;
    int    ret =0;
	
	pbiinfo("%s    %d start\n",__FUNCTION__,ret);
    if(sem_id >= OS_MAX_SEMAPHORES  || OS_bin_sem_table[sem_id].free == TRUE)
    {
    	pbiinfo("%s    %d error\n",__FUNCTION__,ret);
    	return OS_ERR_INVALID_ID;
	}


    ret_val = sem_getvalue(&OS_bin_sem_table[sem_id].id,&ret);

    if ( ret_val == 0 )
    {
       ret_val = OS_SUCCESS;
    }
    else
    {
       ret_val = OS_SEM_FAILURE;
	   ret =-1;
    }
	pbiinfo("%s     line%d    value:%d \n",__FUNCTION__,__LINE__,ret);
    return ret_val;


}

/*---------------------------------------------------------------------------------------
    Function: OS_SemWait_Timeout
   
   Parameters:	sem_id			: id in OS_bin_sem_table[] of 
   									given semaphore want to take
   				msecs			:millisecond the take function should wait for
   				
   
   Description:  The function locks the semaphore referenced by sem_id . owever,
             if the semaphore cannot be locked without waiting for another process
             or thread to unlock the semaphore , this wait shall be terminated when 
             the specified timeout ,msecs, expires.


    Returns:OS_SEM_TIMEOUT 			if semaphore was not relinquished in time
            	 OS_SUCCESS 				if success
             OS_ERR_INVALID_ID 		if the ID passed in is not a valid semaphore ID

    Author : 			ZDS    2008/3/25
----------------------------------------------------------------------------------------*/


S32 OS_SemWait_Timeout ( U32 sem_id, U32 msecs )
{
    int              ret;
    struct timespec  temp_timespec ;

    if( (sem_id >= OS_MAX_SEMAPHORES) || (OS_bin_sem_table[sem_id].free == TRUE) )
        return OS_ERR_INVALID_ID;   

#if 1
    if(0 != msecs)
    {	
		/*
		** Compute an absolute time for the delay
		*/
		OS_CompAbsDelayedTime( msecs , &temp_timespec) ;
		/*
		** Get a lock on the condition variable, then wait until a specified time for the
		** semaphore
		*/
		ret = sem_timedwait(&OS_bin_sem_table[sem_id].id,&temp_timespec); 
		
	   
		if ( ret == 0 )
		{
		   return OS_SUCCESS;
		}
		else
		{
		   return OS_SEM_TIMEOUT;
		}
	}
	else
	{
		
		ret = sem_trywait(&OS_bin_sem_table[sem_id].id); 
		
	   
		if ( ret == 0 )
		{
		   return OS_SUCCESS;
		}
		else
		{
		   return OS_SEM_FAILURE;
		}

	}
    
    
  
#else


    
    /* try it this way */

    for (timeloop = msecs; timeloop >0; timeloop -= 100)
    {
        if (sem_trywait(&OS_bin_sem_table[sem_id].id) == -1 && errno == EAGAIN)
        {
            /* sleep for 100 msecs */
            usleep(100*1000);
        }
    
        else
        {   /* something besides the sem being taken made it fail */
            if(sem_trywait(&OS_bin_sem_table[sem_id].id) == -1)
                return OS_SEM_FAILURE;
        
            /* took the sem successfully */
            else
                return OS_SUCCESS;
        }
    }


    return OS_SEM_TIMEOUT;
#endif
            
}/* end OS_SemWait_Timeout */

/*--------------------------------------------------------------------------------------
   Function: OS_SemGetIdByName
   
   Parameters:	sem_id			: adress where the id in OS_bin_sem_table[] of 
   									given semaphore will copy into
   				sem_name		:name of the given semaphore
   				
   
   Description:  This function tries to find a binary sem Id given the name of a bin_sem
             		The id is returned through sem_id


    Returns:OS_INVALID_POINTER 			is semid or sem_name are NULL pointers
             	OS_ERR_NAME_TOO_LONG	 if the name given is to long to have been stored
             	OS_ERR_NAME_NOT_FOUND	 if the name was not found in the table
             	OS_SUCCESS 					if success

    Author : 			ZDS    2008/3/25
             
---------------------------------------------------------------------------------------*/
S32 OS_SemGetIdByName (U32 *sem_id, const char *sem_name)
{
    U32 i;

    if (sem_id == NULL || sem_name == NULL)
        return OS_INVALID_POINTER;

    
    /* a name too long wouldn't have been allowed in the first place
     * so we definitely won't find a name too long*/
    
    if (strlen((char*)sem_name) > OS_MAX_API_NAME)
            return OS_ERR_NAME_TOO_LONG;

    for (i = 0; i < OS_MAX_SEMAPHORES; i++)
    {
        if ( OS_bin_sem_table[i].free != TRUE &&
           (strcmp (OS_bin_sem_table[i].name , (char*) sem_name) == 0))
        {
            *sem_id = i;
            return OS_SUCCESS;
        }
    }
    /* The name was not found in the table,
     *  or it was, and the sem_id isn't valid anymore */

    return OS_ERR_NAME_NOT_FOUND;
    
}/* end OS_SemGetIdByName */
/*---------------------------------------------------------------------------------------
  Function: OS_SemGetInfo
   
   Parameters:		sem_id			:  the id in OS_bin_sem_table[] of given semaphore
   				sem_prop		:address where the info of given semaphore will 
   									copy into
   				
   
   Description:  This function will pass back a pointer to structure that contains 
            		 all of the relevant info( name and creator) about the specified binary
             		semaphore.


    Returns:	OS_ERR_INVALID_ID	 if the id passed in is not a valid semaphore 
             	OS_INVALID_POINTER	 if the sem_prop pointer is null
             	OS_SUCCESS 			if success

    Author : 			ZDS    2008/3/25
---------------------------------------------------------------------------------------*/

S32 OS_SemGetInfo (U32 sem_id, OS_bin_sem_prop_t *sem_prop)  
{
    /* Check to see that the id given is valid */
    
    if (sem_id >= OS_MAX_SEMAPHORES || OS_bin_sem_table[sem_id].free == TRUE)
        return OS_ERR_INVALID_ID;

    if (sem_prop == NULL)
        return OS_INVALID_POINTER;

    /* put the info into the stucture */
    sem_wait( & (OS_bin_sem_table_sem));
    sem_prop ->creator =    OS_bin_sem_table[sem_id].creator;
    strcpy(sem_prop-> name, OS_bin_sem_table[sem_id].name);
    sem_post( & (OS_bin_sem_table_sem));
    return OS_SUCCESS;
    
} /* end OS_SemGetInfo */
/****************************************************************************************
                                  MUTEX API
****************************************************************************************/

/*---------------------------------------------------------------------------------------
     Function: OS_MutexCreate
   
   Parameters:		sem_id			:  address where the id in OS_mut_sem_table[] of mutex
   									creating will copy into
   				sem_name		:name of creating mutex
   				options			:no mean in current version ,reserved for 
   									future use
   				
   
   Description:  Creates a mutex


    Returns:    OS_INVALID_POINTER 			if sem_id or sem_name are NULL
             	OS_ERR_NAME_TOO_LONG 	if the sem_name is too long to be stored
             	OS_ERR_NO_FREE_IDS		 if there are no more free mutex Ids
             	OS_ERR_NAME_TAKEN 		if there is already a mutex with the same name
             	OS_SEM_FAILURE 				if the OS call failed
             	OS_SUCCESS 					if success

    Author : 			ZDS    2008/3/25

---------------------------------------------------------------------------------------*/
S32 OS_MutexCreate (U32 *sem_id, const char *sem_name, U32 options)
{
    int                 return_code;
    int                 mutex_init_attr_status;
    /*    int                 mutex_setprotocol_status ;*/
    pthread_mutexattr_t mutex_attr ;    
    U32              possible_semid;
    U32              i;      

    /* Check Parameters */

    if (sem_id == NULL || sem_name == NULL)
        return OS_INVALID_POINTER;
    
    /* we don't want to allow names too long*/
    /* if truncated, two names might be the same */
    
    if (strlen((char*)sem_name) > OS_MAX_API_NAME)
            return OS_ERR_NAME_TOO_LONG;

    sem_wait( & (OS_mut_sem_table_sem));
    for (possible_semid = 0; possible_semid < OS_MAX_MUTEXES; possible_semid++)
    {
        if (OS_mut_sem_table[possible_semid].free == TRUE)    
            break;
    }
    sem_post( & (OS_mut_sem_table_sem));
    
    if( (possible_semid >= OS_MAX_MUTEXES) ||
        (OS_mut_sem_table[possible_semid].free != TRUE) )
        return OS_ERR_NO_FREE_IDS;
    

    /* Check to see if the name is already taken */

    sem_wait( & (OS_mut_sem_table_sem));

    for (i = 0; i < OS_MAX_MUTEXES; i++)
    {
        if (strcmp ((char*) sem_name, OS_mut_sem_table[i].name) == 0)
        {
            sem_post( & (OS_mut_sem_table_sem));
            return OS_ERR_NAME_TAKEN;
        }
    }
    sem_post( & (OS_mut_sem_table_sem));

    /* 
    ** initialize the attribute with default values 
    */
    mutex_init_attr_status = pthread_mutexattr_init( &mutex_attr) ; 
    /* Linux does not support ? mutex_setprotocol_status = pthread_mutexattr_setprotocol(&mutex_attr,PTHREAD_PRIO_INHERIT) ; */

    /* 
    ** create the mutex 
    ** upon successful initialization, the state of the mutex becomes initialized and ulocked 
    */
    return_code =  pthread_mutex_init((pthread_mutex_t *) &OS_mut_sem_table[possible_semid].id,&mutex_attr); 
    if ( return_code != 0 )
    {
       printf("Error: Mutex could not be created. ID = %d\n",possible_semid);
       return OS_ERROR;
    }
    else
    {
       /*
       ** Mark mutex as initialized
       */
/*     printf("Mutex created, mutex_id = %d \n" ,possible_semid) ;*/
       
    *sem_id = possible_semid;
    sem_wait( & (OS_mut_sem_table_sem));
    strcpy(OS_mut_sem_table[*sem_id].name, (char*) sem_name);
    OS_mut_sem_table[*sem_id].free = FALSE;
    OS_mut_sem_table[*sem_id].creator = OS_FindCreator();
    sem_post( & (OS_mut_sem_table_sem));


       return OS_SUCCESS;
    }

}/* end OS_MutexCreate */


/*--------------------------------------------------------------------------------------
    Function: OS_MutexDelete
   
   Parameters:		sem_id			:   id in OS_mut_sem_table[] of the given mutex
   
   Description:  Deletes the specified Mutex.


    Returns:OS_ERR_INVALID_ID 		if the id passed in is not a valid mutex
            	 OS_ERR_SEM_NOT_FULL 	if the mutex is empty 
             	OS_SEM_FAILURE 			if the OS call failed
             	OS_SUCCESS 				if success
     Notes: The mutex must be full to take it, so we have to check for fullness

    Author : 			ZDS    2008/3/25

---------------------------------------------------------------------------------------*/

S32 OS_MutexDelete(U32 sem_id)
{
    int status=-1;
    /* Check to see if this sem_id is valid   */
    if (sem_id >= OS_MAX_MUTEXES || OS_mut_sem_table[sem_id].free == TRUE)
        return OS_ERR_INVALID_ID;

    /*
     * To make sure it is safe to delete a semaphore, we have to make sure that the 
     * semaphore is full. Therefore, if OS_MutSemDelete can successfully take the 
     * semaphore, it must have been free, and we can delete it, otherwise return 
     * OS_ERR_SEM_NOT_FULL*/
    
    status = pthread_mutex_trylock ( &(OS_mut_sem_table[sem_id].id)); 
   
    if(status !=0)       /*0 is success in posix */
        return OS_ERR_SEM_NOT_FULL;
    
    /* now we have successfully taken the semaphore */
    /* we can't delete a taken semaphore, so we must release it now. */

    status = pthread_mutex_unlock( &(OS_mut_sem_table[sem_id].id));
    if (status != 0 )
        return OS_SEM_FAILURE;

    
    status = pthread_mutex_destroy( &(OS_mut_sem_table[sem_id].id)); /* 0 = success */   
    
    if( status != 0)
        return OS_SEM_FAILURE;
    /* Delete its presence in the table */
   
    sem_wait( & (OS_mut_sem_table_sem));
    OS_mut_sem_table[sem_id].free =TRUE;
    strcpy(OS_mut_sem_table[sem_id].name , "");
    OS_mut_sem_table[sem_id].creator = UNINITIALIZED;
    sem_post( & (OS_mut_sem_table_sem));
        
    return OS_SUCCESS;

}/* end OS_MutexDelete */

/*---------------------------------------------------------------------------------------
    Function: OS_MutexRelease
   
   Parameters:	sem_id			:   id in OS_mut_sem_table[] of the given mutex
   
   Description:  The function releases the mutex object referenced by sem_id.
   				If there are threads blocked on the mutex object referenced by 
           			 mutex when this function is called, resulting in the mutex becoming 
             		available, the scheduling policy shall determine which thread shall 
            		acquire the mutex.


     Returns: OS_SUCCESS 		if success
             	OS_SEM_FAILURE 		if the semaphore was not previously  initialized 
             	OS_ERR_INVALID_ID 	if the id passed in is not a valid mutex
     

    Author : 			ZDS    2008/3/25

---------------------------------------------------------------------------------------*/

S32 OS_MutexRelease ( U32 sem_id )
{
    S32 ret_val ;

    /* Check Parameters */

    if(sem_id >= OS_MAX_MUTEXES || OS_mut_sem_table[sem_id].free == TRUE)
        return OS_ERR_INVALID_ID;

    /*
    ** Unlock the mutex
    */
    if(pthread_mutex_unlock(&(OS_mut_sem_table[sem_id].id)))
    {
        ret_val = OS_SEM_FAILURE ;
    }
    else
    {
        ret_val = OS_SUCCESS ;
    }
    
    return ret_val;
}/* end OS_MutexRelease */


/*---------------------------------------------------------------------------------------
    Function: OS_MutexLock
   
   Parameters:	sem_id			:   id in OS_mut_sem_table[] of the given mutex
   
   Description:  The mutex object referenced by sem_id shall be locked by calling this
             function. If the mutex is already locked, the calling thread shall
             block until the mutex becomes available. This operation shall return
             with the mutex object referenced by mutex in the locked state with the             
             calling thread as its owner.


     Returns: OS_SUCCESS 		if success
             OS_ERR_INVALID_ID 		the id passed in is not a valid mutex
     

    Author : 			ZDS    2008/3/25
---------------------------------------------------------------------------------------*/

S32 OS_MutexLock ( U32 sem_id )
{
    int ret_val ;

    /* 
    ** Check Parameters
    */  
   if(sem_id >= OS_MAX_MUTEXES || OS_mut_sem_table[sem_id].free == TRUE)
        return OS_ERR_INVALID_ID;
 
    /*
    ** Lock the mutex
    */
    if( pthread_mutex_lock(&(OS_mut_sem_table[sem_id].id) ))
    {
        ret_val = OS_SEM_FAILURE ;
    }
    else
    {
        ret_val = OS_SUCCESS ;
    }
    
    return ret_val;
}/* end OS_MutexLock */
/*=============================================================================
   Function: OS_MutexTrylock
   
   Parameters:	sem_id			:   id in OS_mut_sem_table[] of the given mutex
   
   Description:  try to lock The mutex object referenced by sem_id .
   		whether If the mutex is already  or not locked 
    		by another task, return immediately, if current able to lock the mutex ,
    		return OS_SUCCESS,else return OS_SEM_FAILURE.


     Returns: OS_SUCCESS 		if lock the mutex success
                 OS_ERR_INVALID_ID 		the id passed in is not a valid mutex
                 OS_ERROR             if can not lock the mutex,return OS_ERROR immediately ,donn't wait 
     

    Author : 	add  by	ZDS    2008/3/25
    		
*===========================================================================*/

S32 OS_MutexTrylock ( U32 sem_id )
{
     int ret_val ;

    /* 
    ** Check Parameters
    */  
   if(sem_id >= OS_MAX_MUTEXES || OS_mut_sem_table[sem_id].free == TRUE)
        return OS_ERR_INVALID_ID;
 
    /*
    ** tryLock the mutex
    */
    if( pthread_mutex_trylock(&(OS_mut_sem_table[sem_id].id) ))
    {
        ret_val = OS_SEM_FAILURE ;
    }
    else
    {
        ret_val = OS_SUCCESS ;
    }
    
    return ret_val;
}/* end OS_MutexTrylock */

/*--------------------------------------------------------------------------------------
   Function: OS_MutexGetIdByName
   
   Parameters:		sem_id			:   address where id in OS_mut_sem_table[] of the given mutex
   									will copy into
   				sem_name		:	name of given mutex
   
   Description:  This function tries to find a mutex sem Id given the name of a bin_sem
             		The id is returned through sem_id


     Returns: OS_INVALID_POINTER 		is semid or sem_name are NULL pointers
             	OS_ERR_NAME_TOO_LONG 	if the name given is to long to have been stored
             	OS_ERR_NAME_NOT_FOUND	 if the name was not found in the table
             	OS_SUCCESS 					if success
     

    Author : 			ZDS    2008/3/25
             
---------------------------------------------------------------------------------------*/

S32 OS_MutexGetIdByName (U32 *sem_id, const char *sem_name)
{
    U32 i;

    if(sem_id == NULL || sem_name == NULL)
        return OS_INVALID_POINTER;

    /* a name too long wouldn't have been allowed in the first place
     * so we definitely won't find a name too long*/
    
    if (strlen((char*)sem_name) > OS_MAX_API_NAME)
            return OS_ERR_NAME_TOO_LONG;

    for (i = 0; i < OS_MAX_MUTEXES; i++)
    {
        if ((OS_mut_sem_table[i].free != TRUE)&& 
            (strcmp (OS_mut_sem_table[i].name, (char*) sem_name) == 0) )
        {
            *sem_id = i;
            return OS_SUCCESS;
        }
    }
    
    /* The name was not found in the table,
     *  or it was, and the sem_id isn't valid anymore */
    return OS_ERR_NAME_NOT_FOUND;

}/* end OS_MutexGetIdByName */

/*---------------------------------------------------------------------------------------
    Function: OS_MutexGetInfo
   
   Parameters:	sem_id			:    id in OS_mut_sem_table[] of the given mutex
   									
   				sem_prop		:	address where the info of given  mutex will 
   									copy into
   
   Description:  This function will pass back a pointer to structure that contains 
             all of the relevant info( name and creator) about the specified mutex
             semaphore.


     Returns: 	 	OS_ERR_INVALID_ID 		if the id passed in is not a valid semaphore 
             		OS_INVALID_POINTER 		if the sem_prop pointer is null
             		OS_SUCCESS 				if success
     

    Author : 			ZDS    2008/3/25
---------------------------------------------------------------------------------------*/

S32 OS_MutexGetInfo (U32 sem_id, OS_mut_sem_prop_t *sem_prop)  
{
    /* Check to see that the id given is valid */
    
    if (sem_id >= OS_MAX_MUTEXES || OS_mut_sem_table[sem_id].free == TRUE)
        return OS_ERR_INVALID_ID;

    if (sem_prop == NULL)
        return OS_INVALID_POINTER;
    
    /* put the info into the stucture */ 
    
    sem_wait( & (OS_mut_sem_table_sem));
    sem_prop -> creator =   OS_mut_sem_table[sem_id].creator;
    strcpy(sem_prop-> name, OS_mut_sem_table[sem_id].name);
    sem_post( & (OS_mut_sem_table_sem));

    return OS_SUCCESS;
    
} /* end OS_MutexGetInfo */

/****************************************************************************************
                                    Memorry API
****************************************************************************************/

#define MAX_LINE_NUMBER		(8000)
static S32 log[MAX_LINE_NUMBER] = {0,};
static U32 alloc_count = 0;
static U32 waste_size = 0;

void show_mem_state( void )
{
	int i;
	for( i=0; i<MAX_LINE_NUMBER; i++ )
		if( log[i] != 0 )
			printf("------- Line = %d, allocated memory = %d -------\n", i, log[i]);
	printf("system_p memory_allocate status: allocate size %d, waste size %u\n\n", alloc_count - waste_size, waste_size);
}



/*=============================================================================
* 
* OS_MemInit
*
* Parameters:
*				NONE
*	
*
* Description:
*				Create the memory partition for the block allocation control list and
*				Initialize the MemHeader 
*
* Returns:		NONE
*				
* Author : 			ZDS    2007/9/11
*===========================================================================*/
#ifdef  OS_MEMORY_TRACE  
	 
	 
	
/*=============================================================================
* 
* FileName_Cmp
*
* Parameters:
*				filename1			the filename1  ,the filename1 may be include the full path ,
*									so before compare ,should get rid of the path.
*									example:  " F:\pvware-1.1_2\src\thirdparty\flash_fs\fs_inner.c"
*									
*				filename2                     the filename1  ,the filename1 must not be include  path.
*									example: "fs_inner.c"
* Description:		compare two name of file is indentical.
*				call by OS_MemTraceInfo only.
*
* Returns:		0:      	the two filename  is refer to same file
*				other :    the two filename  isn't  refer to same file
*				
* Author : 			ZDS    2008/10/16
*===========================================================================*/

static int  FileName_Cmp(char * filename1,char *filename2)
{
	char *pt1=NULL;
	char *pt2=NULL;

	pt1=pt2=filename1;
	while(1)
	{
		pt2++;
		if(*pt2=='\\')
			pt1=pt2+1;  //forward one byte to skip '\\'
		else if(*pt2=='\0')
			break;
	}
	return strcmp((char*)pt1,(char*)filename2);
	
}

static gint __mem_comparefunc(gconstpointer	a, gconstpointer	b)
{

	if((U32)a >(U32) b)
		return 1;
	if((U32)a <(U32) b)
		return -1;
	return 0;


}
static  gint	 __mem_searchfunc(gpointer	key,gpointer	data)
{

	if((U32)key >(U32) data)
		return -1;
	if((U32)key <(U32) data)
		return 1;
	return 0;


}

static  gint	 __mem_traverse_0(gpointer	key,gpointer	value, gpointer data)
{
	traverse_par *parame=(traverse_par *)data;
	t_MemList  *mem_ptr=(t_MemList  *)value;

	if(NULL == parame->taskname ||parame->taskname == mem_ptr->taskname )
	{
		if(NULL == parame->filename)
			printf( "==address: %8x ||size: %8x ||Line: %d ||File: %s ||task:%s  \n",
						mem_ptr->Ptr,mem_ptr->SegtSize,mem_ptr->Line,mem_ptr->File,mem_ptr->taskname);
		else if(FileName_Cmp((char *)mem_ptr->File,parame->filename)==0)
			{
				printf( "==address: %8x ||size: %8x ||Line: %d ||File: %s ||task:%s  \n",
					mem_ptr->Ptr,mem_ptr->SegtSize,mem_ptr->Line,mem_ptr->File,mem_ptr->taskname);
			}		
	}
			
	return 0;
}



static void OS_MemInit(void )
{
	 int                ret;
	 
	   ret = sem_init(&OS_mem_sem, 0 , 1) ;
  	 if(-1 == ret)
   		DB_OSAPI((TRC_CRITICAL,"OS_mem_sem creat error \n"));
	 
	

	mem_trace_tree=g_tree_new (__mem_comparefunc);
	
	return;
}

/******************************************************************************
* Function :OS_MemSizeDump
* Parameters : 
*
* Return :
*
* Description :
* 
* Author : jxZheng     2008/05/28
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
typedef struct memdumpsize
{
	char taskname[OS_MAX_API_NAME];
	U32 taskID;
	U32  size;
	struct memdumpsize *next;
}memdumpsize_t;


static  gint	 __mem_traverse_1(gpointer	key,gpointer	value, gpointer data)
{
	memdumpsize_t  *temp=NULL, *newnode=NULL;
	memdumpsize_t *dump_list=(memdumpsize_t *)data;
	t_MemList  *mem_ptr=(t_MemList  *)value;

	temp = dump_list;
		while((temp!=NULL)&&(mem_ptr->taskID!=temp->taskID))
		{
			temp = temp->next;
		}

		if(temp==NULL)
		{
			
			newnode = malloc(sizeof(memdumpsize_t));
			memset(newnode, 0 , sizeof(memdumpsize_t));
		temp = dump_list;
			while(temp->next!=NULL)
				temp=temp->next;
			temp->next = newnode;
			temp = temp->next;
		memcpy(temp->taskname ,mem_ptr->taskname, 20);
		temp->taskID = mem_ptr->taskID;
		}
		temp->size+=mem_ptr->SegtSize;

			
	return 0;
}


void OS_MemSizeDump(void)
{
	t_MemList  *mem_ptr=NULL;
	memdumpsize_t *dumplist = NULL, *temp=NULL, *newnode=NULL;
	U32 tal=0;
	

	dumplist = malloc(sizeof(memdumpsize_t));
	memset(dumplist, 0 , sizeof(memdumpsize_t));
	dumplist->taskID=0xffffffff; //oxffffffff is invalid value
	g_tree_traverse(mem_trace_tree, __mem_traverse_1, G_IN_ORDER, (gpointer*)dumplist);
	temp = dumplist;
	while(temp != NULL)
	{
		

		tal +=dumplist->size;
		temp = temp->next;
	}
	
	printf( "========================================\n");
	temp = dumplist;
	while(temp!=NULL)
	{
		printf("--taskid[%d] taskname[%s]  size[%d]\n", temp->taskID, temp->taskname, temp->size);
		newnode = temp;
		temp = temp->next;
		free(newnode);
	}
	printf( "[%d]========================================\n",tal);
}

/*=============================================================================
* 
* OS_MemTraceInfo
*
* Parameters:
*				filename				the name of  the file ,where the memory block you want to 
*									output have been allocate in .if set the parameter to NULL ,
*									it'll output all of the allocated memory block information  
*	
*
* Description:		out put the memory block information allocate by the user
*				
*
* Returns:		NONE
*				
* Author : 			ZDS    2007/9/29
*===========================================================================*/
void OS_MemTraceInfo(char*taskname,char *filename )
{
	traverse_par parame;
	
	if(mem_init==FALSE)
	{
		OS_MemInit();
		mem_init=TRUE;
	}
	sem_wait(&OS_mem_sem);

	parame.taskname=taskname;
	parame.filename=filename;

	printf( "========================================\n");
	printf( "memory full information ouput start\n");
	g_tree_traverse(mem_trace_tree, __mem_traverse_0, G_IN_ORDER, (gpointer*)&parame);
	printf( "memory full information ouput end\n");
	printf( "========================================\n");

	sem_post(&OS_mem_sem);
	
	return;
}
/*=============================================================================
   Function: OS_MemTraceCtrl
   
   Parameters:	 
   					
   
   Description:   this funtion have no means in OS20


     Returns:  None 
     

    Author : 			ZDS    2008/2/21         add for memory trace    
   
    NOTE:  this funtion have no means in OS20
*===========================================================================*/

void OS_MemTraceCtrl(trace_option  option )
{
	return ;
}
#endif

/*=============================================================================
   Function: OS_MemAlloc
   
   Parameters:	size:    the number in byte want to allocate
   			
   
   Description: allocate a number byte of memorry


     Returns:   NULL:   some error occur
                     other:   the point of the allocated memorry	 
     

    Author : 			ZDS    2007/9/12         add for memory trace    
   
    NOTE: this function should not call direct,to use this funtion should use the macro
		which define in osapi.h for this function. 
*===========================================================================*/
#ifdef  OS_MEMORY_TRACE 
char t_name[]="unknow_task";
void *OS_MemAlloc (partition_type type,U32 size,U16 Line,char *File) 
{
	
	void			*PtrTmp;
	S8			*PtrTmpChar;
	s_MemoryFlag	*IntTmp;
	t_MemList		*NewMemHeader;
	U32			RealSegtSize;
	U32 i=0;

	if(mem_init==FALSE)
	{
		OS_MemInit();
		mem_init=TRUE;
	}
		
	
	sem_wait(&OS_mem_sem);
	if(size==0)
	{
		DB_OSAPI((TRC_WARNING,"size=0 in OS_MemAllocate LINE:%d ||FILE:%s\n",Line,File));
		sem_post(&OS_mem_sem);
		return NULL;
	}
	
	RealSegtSize = (size + (2 * sizeof(s_MemoryFlag)));
	RealSegtSize=(RealSegtSize+3)&0xFFFFFFFC;
	
	if(type==system_p)
		{
		
			 if((PtrTmp =malloc(RealSegtSize)) == NULL)
			 {
			 	DB_OSAPI((TRC_CRITICAL,"no free space in OS_MemAllocate LINE:%d ||FILE:%s\n",Line,File));
				sem_post(&OS_mem_sem);
				return NULL;
			 }
		}
	else
		{
			 if((PtrTmp =malloc( RealSegtSize)) == NULL)
			 {
			 	DB_OSAPI((TRC_CRITICAL,"no free space in OS_MemAllocate LINE:%d ||FILE:%s\n",Line,File));
				sem_post(&OS_mem_sem);
				return NULL;
			 }
		}
	PtrTmpChar = (S8 *)PtrTmp;
	IntTmp = (s_MemoryFlag *)PtrTmp;
	*(IntTmp) = MEMORY_FLAG_VALUE;
	IntTmp = (s_MemoryFlag *)(PtrTmpChar + RealSegtSize - sizeof(s_MemoryFlag));
	*(IntTmp) = MEMORY_FLAG_VALUE;

	/*--------------------------*/
	/* critical section entry	*/
	/*--------------------------*/
	if((NewMemHeader = (t_MemList *)malloc(sizeof(t_MemList))) == NULL)
	{
		sem_post(&OS_mem_sem);
		return NULL;
	}

	memset(NewMemHeader, 0, sizeof(t_MemList));
	NewMemHeader->NextMemHeader = NULL;
	NewMemHeader->Line          = Line;
	NewMemHeader->File          = File;
	for( i=0;i<OS_MAX_TASKS;i++)
	{
		if(FALSE == OS_task_table[i].free && OS_task_table[i].id == pthread_self())
		{
			NewMemHeader->taskname=OS_task_table[i].name;
			break;
		}
		
	}
	if(NULL==NewMemHeader->taskname)
		NewMemHeader->taskname=t_name;
	NewMemHeader->SegtSize      = RealSegtSize - (2 * sizeof(s_MemoryFlag))/*@@@SegtSize*/;
	NewMemHeader->Ptr           = (void *)(PtrTmpChar + sizeof(s_MemoryFlag));
	NewMemHeader->taskID 	= pthread_self();

	
	
	g_tree_insert (mem_trace_tree,
	       NewMemHeader->Ptr,
	       NewMemHeader);

	sem_post(&OS_mem_sem);
	return (NewMemHeader->Ptr);
			
}
#else
/*=============================================================================
   Function: OS_MemAlloc
   
   Parameters:	size:    the number in byte want to allocate
   			
   
   Description: allocate a number byte of memorry


     Returns:   NULL:   some error occur
                     other:   the point of the allocated memorry	 
     

    Author : 			ZDS    2008/2/21
   
   
   NOTE: this function should not call direct,to use this funtion should use the macro
		which define in osapi.h for this function. 
*===========================================================================*/
void *OS_MemAlloc (partition_type type,U32 size, U16 Line, char * File) 
{
	U32 realsize = 0;
	void * PtrTmpChar = NULL;

	if( size == 0 )
	{
		printf("warning!!!!! allocate size = 0! file = %s.\n", File);
		return NULL;
	}

	if( Line >= MAX_LINE_NUMBER )
	{
		printf("line >= MAX_LINE_NUMBER %d. file: %s.\n", MAX_LINE_NUMBER, File);
	}

	realsize = size + (2 * sizeof(U32));
	realsize = (realsize+3)&0xFFFFFFFC;
	PtrTmpChar = malloc(realsize);
	if (PtrTmpChar == NULL)
	{
		printf("\n osal_MemAlloc failed. size [%d]\n\n", size);
	}
	else
	{
		U32 * head = (U32 *)PtrTmpChar;
		char * pTmp = (char *)PtrTmpChar;
		if( Line < MAX_LINE_NUMBER )
			log[Line] += realsize;
		else
			log[0] += realsize;
		*head = (U32)Line;
		head = (U32 *)(pTmp + sizeof(U32));
		*head = realsize;
		alloc_count += realsize;
		waste_size += (2 * sizeof(U32));
		PtrTmpChar = (void *)(pTmp + (2 *sizeof(U32)));
	}

	return PtrTmpChar;
}

#endif
/*=============================================================================
   Function: OS_MemRealloc
   
   Parameters:	Block:  The current memory block
   			size:    the number in byte want to allocate
   			
   
   Description: reallocate a number byte of memorry


     Returns:   NULL:   some error occur
                     other:   the point of the reallocated memorry	 
     

    Author : 			ZDS    2008/2/21          add for  memory trace

     NOTE: this function should not call direct,to use this funtion should use the macro
		which define in osapi.h for this function. 
    				
*===========================================================================*/
#ifdef  OS_MEMORY_TRACE 
void *OS_MemRealloc (partition_type type,void* Block,U32 size,U16 Line,char *File) 
{
	
	
	void *PtrTmp;
	  
	if((PtrTmp = OS_MemAlloc(type,size,Line,File)) == NULL)
	{
		DB_OSAPI((TRC_CRITICAL,"no free space in OS_MemReallocate LINE:%d ||FILE:%s\n",Line,File));
		 return(NULL);
	}
	memcpy(PtrTmp,Block,size);
	//DB_OSAPI((TRC_CRITICAL,"************************************* free space in OS_MemReallocate LINE:%d ||FILE:%s\n",Line,File));
	OS_MemDealloc(type,Block,Line,File);
 
	return(PtrTmp);
	
}
#else

/*=============================================================================
   Function: OS_MemRealloc
   
   Parameters:	Block:  The current memory block
   			size:    the number in byte want to allocate
   			
   
   Description: reallocate a number byte of memorry


     Returns:   NULL:   some error occur
                     other:   the point of the reallocated memorry	 
     

    Author : 			ZDS    2008/2/21
     

 NOTE: this function should not call direct,to use this funtion should use the macro
	which define in osapi.h for this function. 
*===========================================================================*/

void *OS_MemRealloc (partition_type type,void* Block,U32 size, U16 Line, char * File)
{
	void *PtrTmp;
	  
	if(size==0)
	{
		printf("size=0 in osal_MemRealloc \n");
		return NULL;
	}
	if(Block==NULL)
	{
		printf("Block==NULL in osal_MemRealloc  \n");
	}

	if((PtrTmp = OS_MemAlloc(type,size,Line,File)) == NULL)
	{
		printf("no free space in osal_MemRealloc.\n");
		return(NULL);
	}

	if( Block != NULL )
	{
		memcpy(PtrTmp,Block,size);
		OS_MemDealloc(type,Block);
	}
	return(PtrTmp);

}
#endif

/*=============================================================================
   Function: OS_MemDeallocate
   
   Parameters:	Block:    the number in byte want to deallocate
   			
   
   Description: deallocate a number byte of memorry


     Returns:   NONE	 
     

    Author : 			ZDS    2007/9/12  
    
     NOTE: this function should not call direct,to use this funtion should use the macro
		which define in osapi.h for this function. 
*===========================================================================*/
#ifdef  OS_MEMORY_TRACE 
void OS_MemDealloc (partition_type type, void* Block,U16 Line,char *File) 
{
	t_MemList		*CrtMemHeader,*PrevMemHeader;
	U8			Find = FALSE;
	s_MemoryFlag	*Tmp1,*Tmp2;
	U8			*PtrChar;

	
	PtrChar       = (U8 *)Block;
	

	sem_wait(&OS_mem_sem);
	if(Block==NULL)
	{
		DB_OSAPI((TRC_WARNING,"Block==NULL in OS_MemDeallocate : Line:%d ||File %s\n",Line,File));
		sem_post(&OS_mem_sem);
		return;
	}
	CrtMemHeader= g_tree_search(mem_trace_tree,__mem_searchfunc,Block);
	if(NULL != CrtMemHeader)
		{
			Find = TRUE;
			
			// Memory Integrity check
			
			Tmp1 = (s_MemoryFlag *)(PtrChar - sizeof(s_MemoryFlag));
			Tmp2 = (s_MemoryFlag *)(PtrChar + CrtMemHeader->SegtSize);
			if(*Tmp1 != MEMORY_FLAG_VALUE)
			{
				DB_OSAPI((TRC_WARNING,"Corrupted 1  module at the begining : Line:%d ||File %s\n",Line,File));
			}
			if(*Tmp2 != MEMORY_FLAG_VALUE)
			{
				DB_OSAPI((TRC_WARNING,"Corrupted  2 module at the begining : Line:%d ||File %s\n",Line,File));
			}
			
			// Memory free	
			PtrChar -= sizeof(s_MemoryFlag);
			if(type==system_p)
			{
				free( PtrChar);
			}
			else
			{
				free( PtrChar);
				
			}
		g_tree_remove(mem_trace_tree, Block);
		//DB_OSAPI((TRC_CRITICAL,"************************************* OS_MemDealloc  free space in OS_MemReallocate LINE:%d ||FILE:%s\n",Line,File));
		free(CrtMemHeader);

					
	}

	if(Find == FALSE)
	{
		DB_OSAPI((TRC_WARNING,"memory already free : Line [%d] File [%s]\n",Line,File));
	}	
	sem_post(&OS_mem_sem);
	
}
#else
/*=============================================================================
   Function: OS_MemDealloc
   
   Parameters:	Block:    the number in byte want to deallocate
   			
   
   Description: deallocate a number byte of memorry


     Returns:   NONE	 
     

    Author : 			ZDS    2008/2/21
  
     NOTE: this function should not call direct,to use this funtion should use the macro
		which define in osapi.h for this function. 
*===========================================================================*/
void OS_MemDealloc (partition_type type, void* Block) 
{
	U32 * head = NULL;
	U16 Line = 0;
	char * pTmp = (char *)Block;

	if(Block==NULL)
	{
		printf("Block==NULL in osal_MemDealloc  \n");
		return;
	}

	Block = (void *)(pTmp - (2 * sizeof(U32)));
	head = (U32 *)Block;
	Line = (U16)(*head);
	head = (U32 *)(pTmp - sizeof(U32));
	log[Line] -= *head;
	alloc_count -= *head;
	waste_size -= (2 * sizeof(U32));
	free(Block);

	return;

}
#endif

/****************************************************************************************
                                    INFO API
****************************************************************************************/
/*---------------------------------------------------------------------------------------
 Function: OS_IntAttachHandler
   
   Parameters:	InterruptNumber	:    
   									
   				InerruptHandler		:	
   				parameter			:
   
   Description:  in current version this function is null operation


     Returns: 	 
     

    Author : 			ZDS    2008/3/25
---------------------------------------------------------------------------------------*/

S32 OS_IntAttachHandler( U32 InterruptNumber,
            void * InerruptHandler , S32 parameter ) 
{

    return(OS_SUCCESS) ;
}
         
/*---------------------------------------------------------------------------------------
Function: OS_IntEnableAll
   
   Parameters:	None
   
   Description: Enable the interrupts, in current version this function is null operation


     Returns: 	 
     

    Author : 			ZDS    2008/3/25
---------------------------------------------------------------------------------------*/
S32 OS_IntEnableAll ( void ) 
{

    return(OS_SUCCESS) ;
}

/*---------------------------------------------------------------------------------------
 Function: OS_IntDisableAll
   
   Parameters:	None
   
   Description:  Disable the interrupts,in current version this function is null operation


     Returns: 	 
     

    Author : 			ZDS    2008/3/25
---------------------------------------------------------------------------------------*/
S32 OS_IntDisableAll ( void ) 
{

    return(OS_SUCCESS) ;
}

/*---------------------------------------------------------------------------------------
   Function: OS_GetLocalTime
   
   Parameters:		ticks :	 address where the local time(milisecond from Epoch) will copy into
   
   Description: 		This functions get the local time of the machine its on
   				


     Returns: 	OS_SUCCESS:    if get local time success
     

    Author : 			ZDS    2008/3/25
 * ------------------------------------------------------------------------------------*/

S32 OS_GetLocalTime(U64 *millisecond)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    *millisecond=(tv.tv_sec*1000)+(tv.tv_usec/1000);
    return OS_SUCCESS;


}
/*=============================================================================
   Function: OS_TimeMinus
   
   Parameters:		result :	 address where the result(system ticks) will copy into
   				time1:	
   				time2:
   
   Description: 	Subtract time2 from time1  				


   Returns: 	OS_SUCCESS:    if the operation success
     

    Author : 			LL     		2008/4/2
*===========================================================================*/
S32 OS_TimeMinus( U64 *ms_result, U64 ms_from, U64 ms_by)
{
	*ms_result = ms_from-ms_by;
	return OS_SUCCESS;
}
 
 /*=============================================================================
   Function: OS_TimePlus
   
   Parameters:		result :	 address where the result(system ticks) will copy into
   				time1:	
   				time2:
   
   Description: 	add time1 to time2				


   Returns: 	OS_SUCCESS:    if the operation success
     

    Author : 			LL     		2008/4/2
*===========================================================================*/
S32 OS_TimePlus( U64 *ms_result, U64 ms_time1, U64 ms_time2)
{
	*ms_result = ms_time1+ms_time2;
	return OS_SUCCESS;
}
                                                         
/*--------------------------------------------------------------------------------------
 * int FindCreator
 * purpose: Finds the creator of the calling thread
---------------------------------------------------------------------------------------*/
static pthread_t OS_FindCreator(void)
{
  
    return pthread_self();

}

/*---------------------------------------------------------------------------------------
** Name: OS_CompAbsDelayedTime
**
** Purpose:
** This function accept time interval, milli_second, as an input and 
** computes the absolute time at which this time interval will expire. 
** The absolute time is programmed into a struct.
**
** Assumptions and Notes:
**
** Parameters:
**
** Global Inputs: None
**
** Global Outputs: None
**
**
** Return Values: OS_SUCCESS, 
---------------------------------------------------------------------------------------*/
static U32  OS_CompAbsDelayedTime( U32 milli_second , struct timespec * tm)
{

    /* 
    ** get the current time 
    */
    /* Note: this is broken at the moment! */
    /*clock_gettime( CLOCK_REALTIME,  tm ); */
    
    /* Using gettimeofday instead of clock_gettime because clock_gettime is not
     * implemented in the linux posix */
    struct timeval tv;

    gettimeofday(&tv, NULL);
    tm->tv_sec = tv.tv_sec;
    tm->tv_nsec = tv.tv_usec * 1000;



    
    /* add the delay to the current time */
    tm->tv_sec  += (time_t) (milli_second / 1000) ;
    /* convert residue ( milli seconds)  to nano second */
    tm->tv_nsec +=  (milli_second % 1000) * 1000000 ;
    
    if(tm->tv_nsec > 999999999 )
    {
        tm->tv_nsec -= 1000000000 ;
        tm->tv_sec ++ ;
    }
    
    return(OS_SUCCESS) ;    
}


#ifdef QUEUE_SELF

#define PrintTrace(x)					//printf x
#define PrintMessageQ_Debug(x)			//printf x		
#define PrintMessageQ_FullDebug(x)		//printf x	
#define QUEUE_NB_MAX  20

#define MESSAGE_MEMSIZE_QUEUE(x,y)    ((x)*(y))
/*******************************************************/
/*******************************************************/
typedef struct MessageSend_s
{
    struct MessageSend_s  *	Next_p;
    void     			  *	Message_p;
} MessageSend_t;

typedef struct
{
	void 			  * Memory_p;
	U8		  * Used_p;
	MessageSend_t	  *	Pending_p;
	size_t 				ElementSize;
	unsigned int 		NoElements;
} MessageElem_t;

typedef struct MessageQueueList_s
{
    struct MessageQueueList_s   * Next_p;
    message_queue_t             * MessageQueue_p;
    void                        * Memory_p;
} MessageQueueList_t;


/*******************************************************/
/*******************************************************/
static MessageElem_t	MessageArray[QUEUE_NB_MAX];
static U8   			MessageInitDone = FALSE;

static MessageQueueList_t    * MessageQueueList_p = NULL;

static pthread_mutex_t message_mutex ;
/*******************************************************/
/*******************************************************/
static S8 Init_Allqueues(void)
{
    MessageElem_t * Elem_p;
    int				Index;
    static pthread_mutexattr_t  mutex_attr;

    if (! MessageInitDone)
    {
        if(pthread_mutexattr_init(&mutex_attr) != 0)
        {
            return -1;
        }

        if(pthread_mutex_init(&message_mutex,&mutex_attr) != 0)
        {
            return -1;
        }

        for (Index=0 , Elem_p = MessageArray ; Index<QUEUE_NB_MAX ; Index++ , Elem_p++)
        {
            Elem_p->Memory_p = NULL;
            Elem_p->Used_p = NULL;
            Elem_p->Pending_p = NULL;
            Elem_p->ElementSize = 0;
            Elem_p->NoElements = 0;
        }

        MessageInitDone=TRUE;
        PrintMessageQ_Debug(("COMPAT: message initialization done !!!\n"));
    }
    return 0;
}

/*******************************************************/
/*******************************************************/
static S8 message_init_queue_timeout(message_queue_t * MessageQueue,
                                void * Memory_p,
								size_t ElementSize,
                                unsigned int NoElements)
{
    MessageElem_t * Elem_p;
    int				Index;

    pthread_mutex_lock(&message_mutex);

    /* Seeking for unused queue */
	Index = 0;
	Elem_p = MessageArray;
	while ((Index<QUEUE_NB_MAX) && (Elem_p->Memory_p != NULL))
	{
		Index++;
		Elem_p++;;
	}

    MessageQueue->Index = Index;

    if (Index < QUEUE_NB_MAX)   /* if valid unused queue has been found */
	{
		Elem_p = &MessageArray[ MessageQueue->Index ];

    		Elem_p->Used_p = (U8*)malloc(NoElements*sizeof(U8));    /* Allocates used/unused element array */

       	 if (Elem_p->Used_p != NULL)
		{
            		memset((void *)Elem_p->Used_p, 0, NoElements*sizeof(U8));     /* Sets all elements to unused */

    			Elem_p->Memory_p = Memory_p;
			Elem_p->Pending_p = NULL;
    			Elem_p->ElementSize = ElementSize;
    			Elem_p->NoElements = NoElements;
			if(-1 == sem_init(&(MessageQueue->MsgSemaphore_p),0,0))
			{
   				PrintTrace(("COMPAT: message_init_queue_timeout, OS_SemCreate fail 1 !!!\n"));
            	pthread_mutex_unlock(&message_mutex);
				return -1;

			}
			if(-1 == sem_init(&(MessageQueue->ClaimSemaphore_p),0,NoElements))
			{
				PrintTrace(("COMPAT: message_init_queue_timeout, OS_SemCreate fail 2 !!!\n"));
            	pthread_mutex_unlock(&message_mutex);
				return -1;

			}
                   

			PrintMessageQ_Debug(("COMPAT: Message_queue initialized: %d\n", MessageQueue->Index));
		}
		else
		{
			PrintTrace(("COMPAT: message_init_queue_timeout, No memory !!!\n"));
			pthread_mutex_unlock(&message_mutex);
			return -1;
	    }
	}
	else
	{
		PrintTrace(("COMPAT: message_init_queue_timeout, No memory !!!\n"));
		pthread_mutex_unlock(&message_mutex);
		return -1;
	}
    pthread_mutex_unlock(&message_mutex);
	return 0;
}

/*******************************************************/
/*******************************************************/
static message_queue_t * message_create_queue_timeout(size_t ElementSize, unsigned int NoElements)
{
    MessageQueueList_t  * Current_p=NULL;
    MessageQueueList_t  * New_p=NULL;
    message_queue_t     * MesQ_p = NULL;
	 


    if ((New_p = (MessageQueueList_t*)malloc(sizeof(MessageQueueList_t))) != NULL)
    {
        if ((New_p->MessageQueue_p = (message_queue_t*)malloc(sizeof(message_queue_t))) != NULL)
        {
            if ((New_p->Memory_p = malloc( MESSAGE_MEMSIZE_QUEUE(ElementSize, NoElements))) == NULL)
            {
                /* Memory allocation pb, deallocate list element */
                free( New_p->MessageQueue_p);
                free( New_p);
                New_p = NULL;
            }
        }
        else
        {
            /* Memory allocation pb, deallocate list element */
            free( New_p);
            New_p = NULL;
        }
    }

    if (New_p != NULL)
    {
        if(0 == message_init_queue_timeout( New_p->MessageQueue_p, New_p->Memory_p,ElementSize, NoElements))
		{

			if (MessageArray[ New_p->MessageQueue_p->Index ].Used_p != NULL)
			{
				/* At this stage, we know that all allocations have been correctly done */
				New_p->Next_p = NULL;

				pthread_mutex_lock(&message_mutex);
				Current_p = MessageQueueList_p;
				if (Current_p != NULL)
				{
				    while (Current_p->Next_p != NULL)
				        Current_p = Current_p->Next_p;

				    Current_p->Next_p = New_p;
				}
				else
				{
				    MessageQueueList_p = New_p;
				}
				pthread_mutex_unlock(&message_mutex);

				MesQ_p = New_p->MessageQueue_p;

				PrintMessageQ_Debug(("COMPAT: message_create_queue_timeout done (0x%x)!!!\n", (int)MesQ_p));
			}
			else
			{
				message_delete_queue(New_p->MessageQueue_p);

				free( New_p->MessageQueue_p);
				free( New_p->Memory_p);
				free( New_p);

				PrintTrace(("COMPAT: message_create_queue_timeout, message queue init problem !!!\n"));
			}
		}
    }
    else
    {
        PrintTrace(("COMPAT: message_create_queue_timeout, no memory !!!\n"));
    }

    /* NULL if Message Queue creation problem, message queue pointer otherwise */
    return(MesQ_p);
}

/*******************************************************/
/*******************************************************/
static S8 message_delete_queue(message_queue_t * MessageQueue)
{
    MessageQueueList_t  * Current_p = NULL; /* to avoid warning */
    MessageQueueList_t  * Deleted_p = NULL;
	MessageSend_t       * Pending_p;
    MessageElem_t       * Elem_p;

	if (! MessageInitDone)
	{
		PrintTrace(("COMPAT: message_delete_queue, not initialized !!!\n"));
		return -1;
	}
    if (NULL == MessageQueue)
	{
		PrintTrace(("COMPAT: message_delete_queue, invalid addr !!!\n"));
		return -1;
	}

    if (MessageQueue->Index < QUEUE_NB_MAX)
	{
        Elem_p = &MessageArray[ MessageQueue->Index ];
		
		

		
        /* Delete associated semaphores */
        sem_post(&(MessageQueue->MsgSemaphore_p));
        if(0 != sem_destroy(&(MessageQueue->MsgSemaphore_p)))
		{
			PrintTrace(("COMPAT: message_delete_queue, OS_SemDelete fail 1 !!!\n"));
			return -1;
		}
        sem_post(&(MessageQueue->ClaimSemaphore_p));
        if(0 != sem_destroy(&(MessageQueue->ClaimSemaphore_p)))
		{
			PrintTrace(("COMPAT: message_delete_queue, OS_SemDelete fail 2 !!!\n"));
			return -1;
		}
		

        pthread_mutex_lock(&message_mutex);

	       Elem_p->Memory_p = NULL;
		Pending_p = Elem_p->Pending_p;
		while (Pending_p != NULL)
		{
			Elem_p->Pending_p = Elem_p->Pending_p->Next_p;
	    	      free( Pending_p);
			Pending_p = Elem_p->Pending_p;
		}
    	Elem_p->ElementSize = 0;
    	Elem_p->NoElements = 0;
    	if (Elem_p->Used_p != NULL)
    	{
    	    free( Elem_p->Used_p);
    	    Elem_p->Used_p = NULL;
        }

        pthread_mutex_unlock(&message_mutex);
	}
	else
	{
		return -1;
	}
	MessageQueue->Index = QUEUE_NB_MAX;

    /* Frees the message queue in the list */
    pthread_mutex_lock(&message_mutex);
    if (MessageQueueList_p != NULL)
    {
        if (MessageQueueList_p->MessageQueue_p == MessageQueue)
        {
            Deleted_p = MessageQueueList_p;
        }
        else
        {
            Current_p = MessageQueueList_p;
            while (   (Current_p->Next_p != NULL)
                   && (Deleted_p == NULL))
            {
                if (Current_p->Next_p->MessageQueue_p == MessageQueue)
                {
                    /* Element to destroy has been found */
                    Deleted_p = Current_p->Next_p;
                }
                else
                {
                    /* Goes to next element */
                    Current_p = Current_p->Next_p;
                }
            }
        }

        if (Deleted_p != NULL)
        {
            if (Deleted_p == MessageQueueList_p)
            {
                MessageQueueList_p = Deleted_p->Next_p;
            }
            else
            {
                Current_p->Next_p = Deleted_p->Next_p;
            }

            free( Deleted_p->Memory_p);
            free( Deleted_p->MessageQueue_p);
            free( Deleted_p);
        }
        else
        {
            PrintTrace(("COMPAT: message_delete problem (0x%x), not found !!!\n", (U32)MessageQueue));
			pthread_mutex_unlock(&message_mutex);
			return -1;
        }
    }
    pthread_mutex_unlock(&message_mutex);
    return 0;

    PrintMessageQ_Debug(("COMPAT: message_delete_queue done !!!\n"));
}

/*******************************************************/
/*******************************************************/
static int message_claim_timeout(message_queue_t * MessageQueue, U32 milisecond)
{
    MessageElem_t * Elem_p;
    unsigned int		i;
    struct timespec temp_timespec;

	if (! MessageInitDone)
	{
		PrintTrace(("COMPAT: message_claim_timeout, not initialized !!!\n"));
		return((int)NULL);
	}

    if (MessageQueue->Index < QUEUE_NB_MAX)
	{
		PrintMessageQ_Debug(("COMPAT: Claiming (%d)\n", MessageQueue->Index));

        Elem_p = &MessageArray[ MessageQueue->Index ];
        if(0 == milisecond)
		{
			if(0 != sem_trywait(&(MessageQueue->ClaimSemaphore_p)))
			{
				PrintMessageQ_FullDebug(("COMPAT: Claiming timeout 1\n"));
				return((int)NULL);
			}

		}
		else
		{		
			OS_CompAbsDelayedTime( milisecond , &temp_timespec) ;
		
			if(0 != sem_timedwait(&(MessageQueue->ClaimSemaphore_p),&temp_timespec))
		   	{
				PrintMessageQ_FullDebug(("COMPAT: Claiming timeout 2 \n"));
				return((int)NULL);
			}
		}
		  

        /* Scan every element to find first free one */
        pthread_mutex_lock(&message_mutex);
		for (i=0 ; i<Elem_p->NoElements ; i++)
		{
			if (! Elem_p->Used_p[i])
			{
	            PrintMessageQ_Debug(("COMPAT: Claiming done, elem:%d\n", i));

				Elem_p->Used_p[i] = TRUE;

                pthread_mutex_unlock(&message_mutex);
				return( (int)(Elem_p->Memory_p) + (int)(i * Elem_p->ElementSize) );
			}
		}
        pthread_mutex_unlock(&message_mutex);
       
        PrintMessageQ_FullDebug(("COMPAT: message_clean, No memory, waiting ...\n"));

	}

	return((int)NULL);
}

/*******************************************************/
/*******************************************************/
static int message_claim(message_queue_t * MessageQueue)
{
	MessageElem_t * Elem_p;
	unsigned int	i;


	if (! MessageInitDone)
	{
		PrintTrace(("COMPAT: message_claim_timeout, not initialized !!!\n"));
		return((int)NULL);
	}

    if (MessageQueue->Index < QUEUE_NB_MAX)
	{
		PrintMessageQ_Debug(("COMPAT: Claiming (%d)\n", MessageQueue->Index));

        Elem_p = &MessageArray[ MessageQueue->Index ];
        
		if(sem_wait(&(MessageQueue->ClaimSemaphore_p)) == 0)
		{

	        /* Scan every element to find first free one */
	        pthread_mutex_lock(&message_mutex);

			for (i=0 ; i<Elem_p->NoElements ; i++)
			{
				if (! Elem_p->Used_p[i])
				{
		            PrintMessageQ_Debug(("COMPAT: Claiming done, elem:%d\n", i));

					Elem_p->Used_p[i] = TRUE;

                    pthread_mutex_unlock(&message_mutex);
					return( (int)(Elem_p->Memory_p) + (int)(i * Elem_p->ElementSize) );
				}
			}

            pthread_mutex_unlock(&message_mutex);

           /* No free message has been found, waiting for message release */
           PrintMessageQ_FullDebug(("COMPAT: message_claim, No free message, waiting ...\n"));
		}

		PrintMessageQ_FullDebug(("COMPAT: Claiming timeout\n"));
	}

	return((int)NULL);
}

/*******************************************************/
/*******************************************************/
static void message_release (message_queue_t* MessageQueue, void* Message)
{
    MessageElem_t * Elem_p;
	U32				Index;

	if (! MessageInitDone)
	{
		PrintTrace(("COMPAT: message_release, not initialized !!!\n"));
		return;
	}

    if (MessageQueue->Index < QUEUE_NB_MAX)
	{
		PrintMessageQ_Debug(("COMPAT: Releasing (%d)\n", MessageQueue->Index));

        Elem_p = &MessageArray[ MessageQueue->Index ];

        pthread_mutex_lock(&message_mutex);

        if (   ((U32)Message >= (U32)(Elem_p->Memory_p))
		    && ((U32)Message < (U32)(Elem_p->Memory_p) + (Elem_p->ElementSize*Elem_p->NoElements)) )
		{
			Index = ((U32)Message - (U32)(Elem_p->Memory_p))/((U32)(Elem_p->ElementSize));
			Elem_p->Used_p[Index] = FALSE;

			PrintMessageQ_Debug(("COMPAT: %d released\n", Index));

                  sem_post(&(MessageQueue->ClaimSemaphore_p));
		}
		else
		{
			PrintTrace(("COMPAT: message_release pb !!!\n"));
		}
        pthread_mutex_unlock(&message_mutex);
	}
}


/*******************************************************/
/*******************************************************/
static int message_send(message_queue_t * MessageQueue, void * message)
{
	MessageSend_t  * Pending_p;
	MessageSend_t  * New_p;

	if (! MessageInitDone)
	{
		PrintTrace(("COMPAT: message_send, not initialized !!!\n"));
		return 0;
	}

    if (MessageQueue->Index < QUEUE_NB_MAX)
	{
		PrintMessageQ_Debug(("COMPAT: Sending (%d)\n", MessageQueue->Index));

        New_p = (MessageSend_t*)malloc( sizeof(MessageSend_t));

        if (New_p != NULL)
		{
			New_p->Next_p = NULL;
			New_p->Message_p = message;

	    	PrintMessageQ_Debug(("COMPAT: Sending (0x%x)\n", (int)New_p->Message_p));

            pthread_mutex_lock(&message_mutex);
			Pending_p = MessageArray[ MessageQueue->Index ].Pending_p;
	        if (Pending_p != NULL)
			{
				while (Pending_p->Next_p != NULL)
					Pending_p = Pending_p->Next_p;

				Pending_p->Next_p = New_p;
			}
			else
			{
				MessageArray[ MessageQueue->Index ].Pending_p = New_p;
			}
            pthread_mutex_unlock(&message_mutex);

	    	sem_post(&(MessageQueue->MsgSemaphore_p));
		}
		else
		{
			PrintTrace(("COMPAT: message_send, no memory !!!\n"));
		}
    }

	return 0;
}

/*******************************************************/
/*******************************************************/
static void * message_receive_timeout (message_queue_t* MessageQueue, U32  milisecond)
{
	MessageSend_t  * Pending_p;
    MessageElem_t  * Elem_p;
    void 		   * Message_p = NULL;
    struct  timespec  temp_timespec;

	if (! MessageInitDone)
	{
		PrintTrace(("COMPAT: message_receive_timeout, not initialized !!!\n"));
		return (Message_p);
	}

    if (MessageQueue->Index < QUEUE_NB_MAX)
	{
		PrintMessageQ_FullDebug(("COMPAT: Receiving (%d)\n", MessageQueue->Index));

#ifdef STATIC_MESSAGE_QUEUE_DEBUG
		Elem_p = &MessageArray[ MessageQueue->Index ];

        pthread_mutex_lock(&message_mutex);
		Pending_p = Elem_p->Pending_p;
		if (Pending_p != NULL)
		{
			PrintMessageQ_Debug(("COMPAT: Queue %d: ", MessageQueue->Index));
			while (Pending_p != NULL)
			{
			   PrintMessageQ_Debug(("-> 0x%x ", Pending_p->Message_p));
			   Pending_p = Pending_p->Next_p;
			}
			PrintMessageQ_Debug(("-> NULL\n"));
		}
        pthread_mutex_unlock(&message_mutex);
#endif
        if(0 == milisecond)
		{
			if(0 != sem_trywait(&(MessageQueue->MsgSemaphore_p)))
			{
				 PrintMessageQ_FullDebug(("COMPAT: Receive timeout  1 (%d) \n", MessageQueue->Index));
				return((void *)NULL);
			}

		}
		else
		{		
			OS_CompAbsDelayedTime( milisecond , &temp_timespec) ;
		
			if(0 != sem_timedwait(&(MessageQueue->MsgSemaphore_p),&temp_timespec))
		   	{
				 PrintMessageQ_FullDebug(("COMPAT: Receive timeout  2 (%d) \n", MessageQueue->Index));
				return((void *)NULL);
			}
		}

	 

        Elem_p = &MessageArray[ MessageQueue->Index ];

        pthread_mutex_lock(&message_mutex);

		Pending_p = Elem_p->Pending_p;
		if (Pending_p != NULL)
		{
			Elem_p->Pending_p = Pending_p->Next_p;
			Message_p = Pending_p->Message_p;
				free( Pending_p);

			PrintMessageQ_Debug(("COMPAT: Queue %d: Received 0x%x \n", MessageQueue->Index, (int)Message_p));

			pthread_mutex_unlock(&message_mutex);
			return (Message_p);
		}

        pthread_mutex_unlock(&message_mutex);

       	PrintMessageQ_FullDebug(("COMPAT: message_receive (%d), No message available, waiting ...\n", MessageQueue->Index));
		
    }

	return (Message_p);
}

/*******************************************************/
/*******************************************************/
static void * message_receive (message_queue_t* MessageQueue)
{
	MessageSend_t  * Pending_p;
    MessageElem_t  * Elem_p;
    void 		   * Message_p = NULL;

	if (! MessageInitDone)
	{
		PrintTrace(("COMPAT: message_receive_timeout, not initialized !!!\n"));
		return (Message_p);
	}

    if (MessageQueue->Index < QUEUE_NB_MAX)
	{
		PrintMessageQ_FullDebug(("COMPAT: Receiving (%d)\n", MessageQueue->Index));

#ifdef STATIC_MESSAGE_QUEUE_DEBUG
		Elem_p = &MessageArray[ MessageQueue->Index ];

        pthread_mutex_lock(&message_mutex);
		Pending_p = Elem_p->Pending_p;
		if (Pending_p != NULL)
		{
			PrintMessageQ_Debug(("COMPAT: Queue %d: ", MessageQueue->Index));
			while (Pending_p != NULL)
			{
			   PrintMessageQ_Debug(("-> 0x%x ", Pending_p->Message_p));
			   Pending_p = Pending_p->Next_p;
			}
			PrintMessageQ_Debug(("-> NULL\n"));
		}
        pthread_mutex_unlock(&message_mutex);
#endif

	    if (sem_wait(&(MessageQueue->MsgSemaphore_p)) == 0)
		{
		   

		    Elem_p = &MessageArray[ MessageQueue->Index ];

		    pthread_mutex_lock(&message_mutex);

			Pending_p = Elem_p->Pending_p;
			if (Pending_p != NULL)
			{
				Elem_p->Pending_p = Pending_p->Next_p;
				Message_p = Pending_p->Message_p;
					free( Pending_p);

				PrintMessageQ_Debug(("COMPAT: Queue %d: Received 0x%x \n", MessageQueue->Index, (int)Message_p));

				pthread_mutex_unlock(&message_mutex);
				return (Message_p);
			}
		    pthread_mutex_unlock(&message_mutex);

		    PrintMessageQ_FullDebug(("COMPAT: message_receive (%d), No message available, waiting ...\n", MessageQueue->Index));
		}
		

        PrintMessageQ_FullDebug(("COMPAT: Receive timeout (%d) \n", MessageQueue->Index));
    }

	return (Message_p);
}

#endif
/* BEGIN: Added by zhwu, 2013/3/13 */
static int i2c_fd = -1;
static sem_t i2c_id;
#define DRV_I2C_OPEN_ERR                        (S32)(0x80440001)
#define DRV_I2C_CLOSE_ERR                       (S32)(0x80440002)
#define DRV_I2C_NOT_INIT                        (S32)(0x80440003)
#define DRV_I2C_INVALID_PARA                    (S32)(0x80440004)
#define DRV_I2C_NULL_PTR                        (S32)(0x80440005)
#define DRV_I2C_DATA_TOOLARGE_ERR               (S32)(0x80440006)
#define DRV_I2C_MALLOC_ERR                      (S32)(0x80440007)
#define DRV_I2C_WRITE_ERR						(S32)(0x80440008)
#define DRV_I2C_READ_ERR                     	(S32)(0x80440009)

#undef DBG_DRV_IIC

#ifdef DBG_DRV_IIC
#define DRV_I2C_DBG(res)		\
	do{							\
		if(res < 0) {			\
			printf("FUN:%s,LINE:%d,errno:%d,%s\n",				\
				__FUNCTION__,__LINE__,errno,strerror(errno));	\
			goto DRV_I2C_ERROR;	}								\
	}while( 0 != 0)			
#else
#define DRV_I2C_DBG(res)		\
	do{							\
		if(res < 0) {			\
			pbierror("FUN:%s,LINE:%d,errno:%d,%s\n",			\
				__FUNCTION__,__LINE__,errno,strerror(errno));	\
			goto DRV_I2C_ERROR;	}								\
	}while( 0 != 0)	
#endif /* DBG_DRV_IIC */
/*****************************************************************************
 函 数 名  : DRV_I2C_Open
 功能描述  : 打开IIC设备文件，并将地址配置成7bit模式
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年3月13日
    作    者   : zhwu
    修改内容   : 新生成函数

*****************************************************************************/
S32 DRV_I2C_Open(void)
{
	i2c_fd = open("/dev/i2c-0", O_RDWR );
	DRV_I2C_DBG(i2c_fd);

	DRV_I2C_DBG(ioctl( i2c_fd, I2C_TENBIT, 0 ));
	DRV_I2C_DBG(ioctl( i2c_fd, I2C_RETRIES, 0 ));

	DRV_I2C_DBG(sem_init( &i2c_id, 0, 1 ));
	
    return 0;
	
DRV_I2C_ERROR:
	return DRV_I2C_OPEN_ERR;
}
/*****************************************************************************
 函 数 名  : DRV_I2C_Read
 功能描述  : 读取IIC操作
 输入参数  : U8 u8SlaveAddr      			设备地址
             U8 *pu8ReAddr       			偏移地址
             U16 u16ReAddrCount  			偏移地址长度
             U8 *pu8Buff         			待读取数据buf
             U16 u16Length       			读取数据长度
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年3月13日
    作    者   : zhwu
    修改内容   : 新生成函数

*****************************************************************************/
S32 DRV_I2C_Read( U8 u8SlaveAddr, U8 *pu8ReAddr, U16 u16ReAddrCount, U8 *pu8Buff, U16 u16Length )
{
	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data ioctl_data;

	if ( i2c_fd < 0 )
	    return DRV_I2C_NOT_INIT;
	
	if ( !pu8Buff )
		return DRV_I2C_NULL_PTR;
	    
	if ( (0 == u16Length) || ( u16Length > 8192 ) || (u16ReAddrCount > 8192 ))
		return DRV_I2C_INVALID_PARA;
	    
	msgs[0].addr = u8SlaveAddr >> 1;
	msgs[0].flags = 0;
	msgs[0].buf = pu8ReAddr;
	msgs[0].len = u16ReAddrCount;
	msgs[1].addr = u8SlaveAddr >> 1;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf = pu8Buff;
	msgs[1].len = u16Length;

	ioctl_data.nmsgs = 2;
	ioctl_data.msgs = &msgs[0];
	sem_wait( &i2c_id );
	DRV_I2C_DBG(ioctl( i2c_fd, I2C_RDWR, (unsigned long)&ioctl_data ));
	sem_post( &i2c_id );
	
    return 0;
DRV_I2C_ERROR:
	sem_post( &i2c_id );
	return DRV_I2C_READ_ERR;
}
/*****************************************************************************
 函 数 名  : DRV_I2C_Write
 功能描述  : 写IIC操作
 输入参数  : U8 u8SlaveAddr    				设备地址  
             U8 *pu8ReAddr       			偏移地址
             U16 u16ReAddrCount  			偏移地址长度
             U8 *pu8Buff        			待写入数据
             U16 u16Length       			待写入数据长度
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年3月13日
    作    者   : zhwu
    修改内容   : 新生成函数

*****************************************************************************/
S32 DRV_I2C_Write( U8 u8SlaveAddr, U8 *pu8ReAddr, U16 u16ReAddrCount, U8 *pu8Buff, U16 u16Length )
{
    U8 ulI = 0;
	U8 buff[256] = {0};
	struct i2c_msg msgs[1];
	struct i2c_rdwr_ioctl_data ioctl_data;

	memset( buff, 0xFF, sizeof(buff) );

	if ( i2c_fd < 0 )
	    return DRV_I2C_NOT_INIT;
	
	if ( !pu8Buff )
		return DRV_I2C_NULL_PTR;
	    
	if ( (0 == u16Length) || ( u16Length > 8192 ))
		return DRV_I2C_INVALID_PARA;
	
	if ( (u16ReAddrCount + u16Length) > 256 )
		return DRV_I2C_DATA_TOOLARGE_ERR;
	
	if ( u16ReAddrCount != 0 )
	{
		for ( ulI = 0; ulI < u16ReAddrCount ; ulI++ )
		    buff[ulI] = pu8ReAddr[ulI];
	}
	for ( ulI = 0; ulI < u16Length ; ulI++ )
	    buff[u16ReAddrCount + ulI] = pu8Buff[ulI];

	msgs[0].addr = u8SlaveAddr >> 1;
	msgs[0].flags = 0;
	msgs[0].len = u16ReAddrCount + u16Length;
	msgs[0].buf = buff;

	ioctl_data.nmsgs = 1;
	ioctl_data.msgs = &msgs[0];
	sem_wait( &i2c_id );
	DRV_I2C_DBG(ioctl( i2c_fd, I2C_RDWR, (unsigned long)&ioctl_data ));
	sem_post( &i2c_id );
	return 0;
	
DRV_I2C_ERROR:
	sem_post( &i2c_id );
	return DRV_I2C_WRITE_ERR;
}
/*****************************************************************************
 函 数 名  : DRV_I2C_Close
 功能描述  : 关闭IIC设备
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年3月13日
    作    者   : zhwu
    修改内容   : 新生成函数

*****************************************************************************/
S32 DRV_I2C_Close(void)
{
	if ( i2c_fd < 0 )
		return 0;

	sem_destroy(&i2c_id);
	DRV_I2C_DBG(close(i2c_fd));

	return 0;
DRV_I2C_ERROR:
	return DRV_I2C_CLOSE_ERR;
}

/* END:   Added by zhwu, 2013/3/13 */




