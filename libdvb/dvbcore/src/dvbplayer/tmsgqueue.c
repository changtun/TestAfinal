/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name : dvbplayer_video_operation.c
* Description : player������Ƶ�Ĳ���ģ��. 
* History :
*	Date               Modification                                Name
*	----------         ------------                                ----------
*	2007/12/28        Created                                     LSZ
******************************************************************************/
/*******************************************************/
/*              Includes				                   */
/*******************************************************/
/* Standard headers */


/* include ANSI C .h file */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* include OS, STAPI file */
#include "pvddefs.h"
#include "osapi.h"
/* include Driver .h file */
/* include System layer .h file */
#include "pbitrace.h"

/* include Middle layer .h file */
/* include CA .h file */
/* include Local .h file */
//#include "systemdata.h"
#include "dvbplayer.h"
#include "tmsgqueue.h"
#include "tlock.h"

#define DB_TMSGQUEUE_MSG 0

#if DB_TMSGQUEUE_MSG
	#define DB_TMSGQUEUE(__statement__)  TRC_DBMSG(__statement__)
	TRC_DBMSG_SET_MODULE(SYS);
#else
	#define DB_TMSGQUEUE(__statement__)
#endif

static int sg_serial_num = 0;

int MsgQueue_create(MsgQueue* msg_q, int max_count)
{
	//int i;
	U8 sem_name[OS_MAX_API_NAME];
	if( NULL == msg_q)
	{
		DB_TMSGQUEUE((TRC_CRITICAL, "MsgQueue_create"));
		return 1;
	}
	if(TLock_create(&msg_q->lock))
	{
		return 1;
	}
	memset(sem_name,0,sizeof(sem_name));
	sprintf((char *)sem_name,"read_sem+%x", (U32)msg_q);
	if(OS_SUCCESS  != OS_SemCreate(&msg_q->read_sem, (const char *) sem_name, 0,0))
	{
		TLock_destroy(&msg_q->lock);
		return 1;
	}
	memset(sem_name,0,sizeof(sem_name));
	sprintf((char *)sem_name,"write_sem+%x", (U32)msg_q);
	if(OS_SUCCESS  != OS_SemCreate(&msg_q->write_sem, (const char *) sem_name, max_count,0))
	{
		OS_SemDelete(msg_q->read_sem);
		TLock_destroy(&msg_q->lock);
		return 1;
	}
	msg_q->msgs = (QueueMsg*)OS_MemAllocate(system_p,sizeof(QueueMsg)*max_count);
	if(msg_q->msgs == NULL) 
	{
		OS_SemDelete(msg_q->write_sem);
		OS_SemDelete(msg_q->read_sem);
		TLock_destroy(&msg_q->lock);
		return 1;
	}
	msg_q->max_count = max_count;
	msg_q->count = 0;
	msg_q->write_index = 0;
	msg_q->read_index = 0;
	return 0;
}

void MsgQueue_destroy(MsgQueue* msg_q)
{
	//int j;
	if(NULL == msg_q)
		return;
	OS_MemDeallocate(system_p,msg_q->msgs);
	OS_SemDelete(msg_q->write_sem);
	OS_SemDelete(msg_q->read_sem);
	TLock_destroy(&msg_q->lock);
}

void MsgQueue_post(MsgQueue* msg_q, DVBPlayer_Action_Type_e type, DVBPlayer_Action_Param_u param1, void* param2)
{
	if(NULL == msg_q)
	{
		return;
	}
	OS_SemWait(msg_q->write_sem);
	TLock_lock(&msg_q->lock);
	msg_q->msgs[msg_q->write_index].type = type;
	msg_q->msgs[msg_q->write_index].param1 = param1;
	msg_q->msgs[msg_q->write_index].param2 = param2;
	msg_q->msgs[msg_q->write_index].serial_num = sg_serial_num;
	sg_serial_num++;
	msg_q->write_index++;
	if(msg_q->write_index == msg_q->max_count)
		msg_q->write_index = 0;
	msg_q->count++;
	TLock_unlock(&msg_q->lock);
	OS_SemSignal(msg_q->read_sem);
}

void MsgQueue_clean(MsgQueue* msg_q, DVBPlayer_Action_Type_e type)
{
	int i = 0, j = 0;
	if(NULL == msg_q)
	{
		return;
	}
	//OS_SemWait(msg_q->write_sem);
	TLock_lock(&msg_q->lock);
	for(i = msg_q->read_index; j < msg_q->count; i++)
	{
		if(msg_q->msgs[i].type == type)
		{
			msg_q->msgs[i].type = eDVBPLAYER_ACTION_INVALID;
		}
		if(i == msg_q->max_count) i = 0;		
		j++;
	}	
	TLock_unlock(&msg_q->lock);
	//OS_SemSignal(msg_q->read_sem);
}

void MsgQueue_post_front(MsgQueue* msg_q, DVBPlayer_Action_Type_e type, int serial_num,  DVBPlayer_Action_Param_u param1, void* param2)
{
	if(NULL == msg_q)
		return;
	OS_SemWait(msg_q->write_sem);
	TLock_lock(&msg_q->lock);
	msg_q->read_index--;
	if(-1 == msg_q->read_index)
		msg_q->read_index = msg_q->max_count - 1;
	msg_q->msgs[msg_q->read_index].type = type;
	msg_q->msgs[msg_q->read_index].param1 = param1;
	msg_q->msgs[msg_q->read_index].param2 = param2;
	msg_q->msgs[msg_q->read_index].serial_num = serial_num;
	msg_q->count++;
	TLock_unlock(&msg_q->lock);
	OS_SemSignal(msg_q->read_sem);
}

int MsgQueue_take(MsgQueue* msg_q,
				   DVBPlayer_Action_Type_e* type,
				   DVBPlayer_Action_Param_u* param1,
				   void** param2,
				   int* serial_num)
{
	if(MsgQueue_try_take(msg_q, 0xFFFFFFFF, type, param1, param2, serial_num))
		return 1;
	else
		return 0;
}

/*****************************************************************************
 �� �� ��  : MsgQueue_CompareMsgType
 ��������  : �жϴ�����Ϣ�������һ����Ϣ�Ƿ���ͬ����ͬ����?
 �������? : MsgQueue* msg_q  
             int type         
 �������? : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��2��2��
    ��    ��   : zxguan
    �޸�����   : ����ɺ���?

*****************************************************************************/
int MsgQueue_CompareMsgType(MsgQueue* msg_q,int type)
{
    int ret = 0;

    if( NULL == msg_q )
    {
        return -1;
    }
    
    TLock_lock(&msg_q->lock);
    pbiinfo("[%s %d] msgCount =%d Type = %d V %d.\n",DEBUG_LOG, msg_q->count, type,msg_q->msgs[0].type );
    if( 0 != msg_q->count )
    {
        if( type == msg_q->msgs[0].type)
        {
           ret = 1;
        }
        else
        {
           ret = 0;
        }
    }
    TLock_unlock(&msg_q->lock);

    return ret;
}



int MsgQueue_try_take(MsgQueue* msg_q,
					   U32 milisecond,
					   DVBPlayer_Action_Type_e* type,
					   DVBPlayer_Action_Param_u* param1,
					   void** param2,
					   int* serial_num)
{

	if(NULL == msg_q)
		return 0;
	 if(0xFFFFFFFF == milisecond)
	{
		DB_TMSGQUEUE((TRC_CRITICAL, "DVBplayer MSGQ signal wait TIMEOUT_INFINITY\n"));
		OS_SemWait(msg_q->read_sem);
	}
	else
	{
		if(OS_SUCCESS != OS_SemWait_Timeout(msg_q->read_sem, milisecond))
			return 0;

	}

	TLock_lock(&msg_q->lock);

	if(type)
		*type = (DVBPlayer_Action_Type_e)msg_q->msgs[msg_q->read_index].type;
	if(param1)
		*param1 = msg_q->msgs[msg_q->read_index].param1;
	if(param2)
		*param2 = msg_q->msgs[msg_q->read_index].param2;
	if(serial_num)
		*serial_num = msg_q->msgs[msg_q->read_index].serial_num;

	msg_q->read_index++;
	if(msg_q->read_index == msg_q->max_count)
		msg_q->read_index = 0;
	msg_q->count--;
	TLock_unlock(&msg_q->lock);

	OS_SemSignal(msg_q->write_sem);
	return 1;
}

