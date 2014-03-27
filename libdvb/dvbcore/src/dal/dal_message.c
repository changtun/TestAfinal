#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "osapi.h"
#include "pvddefs.h"

#include "mempool.h"
#include "dal_wrap.h"
#include "dal_common.h"
#include "dal_message.h"

#define DAL_MESSAGE_POOL_SIZE			(30)

#define USE_MEMPOOL
  
typedef struct MessageBlock_s
{
	DAL_Message_t		msg;
	BOOL				avail;
}MessageBlock_t;

typedef struct MessagePool_s
{
	MessageBlock_t*		p_block_list;
	unsigned int		pool_size;
	unsigned int		used_block;
	DAL_Sem_t			sem;
	BOOL				init;
}MessagePool_t;

typedef struct dal_msg_control_s
{
	BOOL			init;
	MemPool			msg_pool;
}dal_msg_control_t;


MessagePool_t		g_dal_message_pool;




#ifndef USE_MEMPOOL

int DAL_MessageInit(void)
{
	int				iret = 0;
	
	if( g_dal_message_pool.init == 1)
	{
		DAL_ERROR(("MessagePool already init\n"));
		return -1;
	}

	iret = DAL_SemCreate( &g_dal_message_pool.sem, "MessagePool Sem", 1, 0);
	if( iret != 0)
	{
		DAL_ERROR(("DAL_SemCreate failed\n"));
		return -1;
	}
	
	g_dal_message_pool.pool_size = DAL_MESSAGE_POOL_SIZE;
	g_dal_message_pool.p_block_list = DAL_Malloc(sizeof(MessageBlock_t)*g_dal_message_pool.pool_size);
	if( g_dal_message_pool.p_block_list == NULL)
	{
		DAL_ERROR(("DAL_Malloc MessagePool failed\n"));
		DAL_SemDelete(g_dal_message_pool.sem);
		return -1;
	}
	memset( g_dal_message_pool.p_block_list, 0, sizeof(MessageBlock_t)*g_dal_message_pool.pool_size);
	g_dal_message_pool.used_block = 0;

	DAL_DEBUG(("DAL MessagePool size: %u  use mem: %u\n", g_dal_message_pool.pool_size, sizeof(MessageBlock_t)*g_dal_message_pool.pool_size));
	g_dal_message_pool.init = 1;
	return 0;
}

int DAL_MessageRelease(void)
{
	if( g_dal_message_pool.init == 0)
	{
		DAL_ERROR(("MessagePool not init\n"));
		return -1;
	}
	DAL_Free(g_dal_message_pool.p_block_list);
	DAL_SemDelete(g_dal_message_pool.sem);
	memset( &g_dal_message_pool,0,sizeof(MessagePool_t));
	return 0;
}

DAL_Message_t* DAL_MessageAlloc( void)
{
	int		i,iret;

	if( g_dal_message_pool.init == 0)
	{
		DAL_ERROR(("MessagePool not init\n"));
		return NULL;
	}

	iret = DAL_SemWait_Timeout( g_dal_message_pool.sem, 1000);
	if( iret != 0)
	{
		DAL_ERROR(("DAL_SemWait_Timeout\n"));
		return NULL;
	}
	
	for( i=0; i < g_dal_message_pool.pool_size; i++)
	{
		if( g_dal_message_pool.p_block_list[i].avail == 0)
		{
			g_dal_message_pool.p_block_list[i].avail = 1;
			g_dal_message_pool.used_block++;
			break;
		}
	}
	if( i < g_dal_message_pool.pool_size)
	{
		DAL_SemSignal(g_dal_message_pool.sem);
		//DAL_DEBUG(("cur_use: %d  used_block: %d  \n", i, g_dal_message_pool.used_block));
		return &(g_dal_message_pool.p_block_list[i].msg);
	}
	else
	{
		DAL_ERROR(("MessagePool have no free space!!!\n"));
		DAL_ERROR(("MessagePool have no free space!!!\n"));
		DAL_ERROR(("MessagePool have no free space!!!\n"));
		DAL_SemSignal(g_dal_message_pool.sem);
		return NULL;
	}
}

int DAL_MessageFree( DAL_Message_t* pMsg)
{
	int		i,iret;

	if( g_dal_message_pool.init == 0)
	{
		DAL_ERROR(("MessagePool not init\n"));
		return -1;
	}

	iret = DAL_SemWait_Timeout( g_dal_message_pool.sem, 1000);
	if( iret != 0)
	{
		DAL_ERROR(("DAL_SemWait_Timeout\n"));
		return -1;
	}
	
	for( i=0; i < g_dal_message_pool.pool_size; i++)
	{
		if( &(g_dal_message_pool.p_block_list[i].msg) == pMsg)
		{
			g_dal_message_pool.p_block_list[i].avail = 0;
			g_dal_message_pool.used_block--;
			break;
		}
	}
	if( i < g_dal_message_pool.pool_size)
	{
		DAL_SemSignal(g_dal_message_pool.sem);
		//DAL_DEBUG(("cur_free: %d  used_block: %d  \n", i, g_dal_message_pool.used_block));
		return 0;
	}
	else
	{
		DAL_ERROR((" free msg failed !!!\n"));
		DAL_SemSignal(g_dal_message_pool.sem);
		return -1;
	}
}
#else

dal_msg_control_t	g_dal_msg_contaol = {.init = FALSE};

int DAL_MessageInit(void)
{
	dal_msg_control_t*		p_control = &g_dal_msg_contaol;
	int						iret = 0;
	
	if( p_control->init )
	{
		DAL_ERROR(("already init\n"));
		return -1;
	}
	iret = mempool_init( &(p_control->msg_pool), "dal_mempool", DAL_MESSAGE_POOL_SIZE, sizeof(DAL_Message_t));
	if( 0 != iret)
	{
		DAL_ERROR(("mempool_init failed\n"));
		return -1;	
	}
	p_control->init = TRUE;
	return 0;
}

int DAL_MessageRelease(void)
{
	dal_msg_control_t*		p_control = &g_dal_msg_contaol;
	int						iret = 0;

	if( !(p_control->init) )
	{
		DAL_ERROR(("not init\n"));
		return -1;
	}
	iret = mempool_destroy( &(p_control->msg_pool));
	if( 0 != iret)
	{
		DAL_ERROR(("mempool_destroy failed\n"));
		return -1;	
	}
	p_control->init = FALSE;	
	return 0;
}

DAL_Message_t* DAL_MessageAlloc( void)
{
	dal_msg_control_t*		p_control = &g_dal_msg_contaol;
	DAL_Message_t*			p_msg = NULL;
	
	if( !(p_control->init) )
	{
		DAL_ERROR(("not init\n"));
		return NULL;
	}
	p_msg = (DAL_Message_t*)mempool_alloc( &(p_control->msg_pool));
	if( NULL == p_msg)
	{
		DAL_ERROR(("mempool_alloc failed\n"));
		return NULL;	
	}
	return p_msg;
}

int DAL_MessageFree( DAL_Message_t* pMsg)
{
	dal_msg_control_t*		p_control = &g_dal_msg_contaol;
	int						iret = 0;

	if( !(p_control->init) )
	{
		DAL_ERROR(("not init\n"));
		return -1;
	}
	iret = mempool_free( &(p_control->msg_pool), (void*)pMsg);
	if( iret != 0)
	{
		DAL_ERROR(("mempool_free failed\n"));
		return -1;
	}
	return 0;
}

#endif


int DAL_MessageSend( DAL_QueueID_t queueID, DAL_Message_t** ppMsg)
{
	int			iret = 0;

	iret = DAL_MessageQueueSend( queueID, ppMsg, sizeof(DAL_Message_t*), DAL_PEND, 0);
	if( iret != 0)
	{
		DAL_ERROR(("DAL_MessageQueueSend failed with %d\n",iret));
		return -1;
	}
	//DAL_DEBUG(("Send  <%s>  to queue %d OK\n", DAL_MessageString((*ppMsg)->MsgType),queueID));
	return 0;
}

char* DAL_MessageString( DAL_Msg_Type_t type)
{
#define E(e) case e: return #e
	switch(type)
	{
		E(DALMSG_STREAM_START);
		E(DALMSG_STREAM_STOP);
		E(DALMSG_STREAM_RESTART);
		E(DALMSG_STREAM_PAUSE);
		E(DALMSG_STREAM_RESUME);
		E(DALMSG_STREAM_SETSPEED);
		E(DALMSG_STREAM_SEEK);
		E(DALMSG_STREAM_SEEK_PREV);
		E(DALMSG_STREAM_SEEK_NEXT);
		E(DALMSG_STREAM_GETINFO);
		E(DALMSG_STREAM_NEXT);
		E(DALMSG_STREAM_PVRSTART);
		E(DALMSG_STREAM_PVRSTOP);
		E(DALMSG_STREAM_TIMESHIFT_START_RECORD);
		E(DALMSG_STREAM_TIMESHIFT_START_PLAY);
		E(DALMSG_STREAM_TIMESHIFT_STOP);
		E(DALMSG_DOWNLOAD_START);
		E(DALMSG_DOWNLOAD_STOP);
		E(DALMSG_CURL_ERROR);
		E(DALMSG_CURL_REV_TIMEOUT);
		E(DALMSG_CURL_REV_RESUME);
		E(DALMSG_CHECK_PARSE_DATA_READY);
		E(DALMSG_INJECT_START);
		E(DALMSG_INJECT_CONT);
		E(DALMSG_INJECT_STOP);
		E(DALMSG_PARSE_START);
		E(DALMSG_PARSE_CONT);
		E(DALMSG_PARSE_STOP);
		E(DALMSG_NEW_PIDS);
		E(DALMSG_PIDS_CHANGED);
		E(DALMSG_DATA_REQUEST);
		E(DALMSG_PARSE_DATA_READY);
		E(DALMSG_DATA_TIMEOUT);
		E(DALMSG_DATA_RESUME);
		E(DALMSG_FIRST_FRAME_DISPLAY);
		E(DALMSG_CHECK_FIRST_FRAME);
		E(DALMSG_HDD_READ_CONT);
#ifdef FLV	
		E(DALMSG_FLVLIST_PARSE_OK);
		E(DALMSG_FLVLIST_PARSE_ERROR);
#endif		
		E(DALMSG_DVB_CHECK_FRIST_FRAME);
		E(DALMSG_WIAT_RETRY_FLVPLAY);
		E(DALMSG_M3U8_ADDRESSDOWNLOAD_START);
				
		E(DALMSG_FIRST_M3U8PARSE_START);
		E(DALMSG_SECOND_M3U8PARSE_START);
		E(DALMSG_M3U8_REQUEST_STOP);
				
		E(DALMSG_M3U8_TSDOWNLOAD_START);				
		E(DALMSG_M3U8_TSDOWNLOAD_STOP);
		E(DALMSG_PARSE_TS_PREPARE);
		E(DALMSG_P2V_SEEK_PREV);
		E(DALMSG_P2V_SEEK_NEXT);

		E(DALMSG_TIMESHIFT_TSDOWNLOAD_START);
		E(DALMSG_TIMESHIFT_TSDOWNLOAD_STOP);
		E(DALMSG_TIMESHIFT_DOWNLOAD);
		E(DALMSG_TIMESHIFT_STOP);
		E(DALMSG_TIMESHIFT_SEEK_NEXT);
		E(DALMSG_TIMESHIFT_SEEK_PREV);
		default: return "unknow dal msg type";
	}
}

