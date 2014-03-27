#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl.h>
#include <types.h>
#include <easy.h>
#include <pthread.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "pbitrace.h"

//#if defined(USE_DAL_MULTICAST)

//{{{add by LYN ,for multicast , 2011-08-22
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h> 	
#include <netinet/in.h> 	
#include <net/if_arp.h> 

//}}}add by LYN ,for multicast , 2011-08-22
//#endif
#include "osapi.h"
#include "pvddefs.h"

#include "ring_buffer.h"


#include "dal_common.h"
#include "dal_wrap.h"
#include "dal.h"

#include "dal_message.h"

#include "dal_download.h"
#include "dal_inject.h"
#include "dal_parse.h"
#include "dal_core.h"
	
#define MAXINTERFACES   16

#define DAL_DOWNLOAD_TASK_STACK_SIZE		1024*8
#define DAL_DOWNLOAD_TASK_PRIOITY			4
#define DAL_DOWNLOAD_QUEUE_DEPTH			20
#define DAL_DOWNLOAD_QUEUE_DATA_SIZE		DAL_MESSAGE_SIZE


#define DAL_CURL_TASK_STACK_SIZE			1024*8
#define DAL_CURL_TASK_PRIOITY				5

#define DAL_MIN_FREESIZE					0x40000 //256kb

#define DAL_DOWNLOAD_BUFFER_SIZE			(0xa00000 + DAL_MIN_FREESIZE)
#define DAL_DATA_REV_TIMEOUT				16

#define DAL_HDD_READ_BLOCK_SIZE				32*1024




static void* dal_download_task( void* param);
static void* curl_preform_task_sync( void* param);
static void* curl_preform_task_async( void* param);

static void* curl_check_task( void* param);




static size_t dal_download_write_func(void *buffer, size_t size, size_t nmemb, void *stream);

static void download_start_process( DAL_Message_t* pMsg);
static void download_stop_process( DAL_Message_t* pMsg);
static void download_curl_error_process( DAL_Message_t* pMsg);
static void download_hdd_read_cont_process( DAL_Message_t* pMsg);
static void data_request_process( DAL_Message_t* pMsg);
static void check_parse_data_ready_process( DAL_Message_t* pMsg);
static void curl_recv_timeout_process( DAL_Message_t* pMsg);
static void curl_recv_resume_process( DAL_Message_t* pMsg);



static int curl_preform_task_create( DAL_DownloadControl_t* p_control);
static int curl_preform_task_delete( DAL_DownloadControl_t* p_control);
#if 0
static int curl_check_task_create( DAL_DownloadControl_t* p_control);
static int curl_check_task_delete( DAL_DownloadControl_t* p_control);
static int curl_check_task_pause( DAL_DownloadControl_t* p_control);
static int curl_check_task_resume( DAL_DownloadControl_t* p_control);
#endif
#if defined(USE_DAL_MULTICAST)

//{{{ add by LYN, for multicast , 2011-08-22
static void* multicast_perform_task_async( void* param);
static void Multicast_start_process(DAL_Message_t* pMsg);
static void Multicast_stop_process(DAL_Message_t* pMsg);
static void Multicast_error_process(DAL_Message_t* pMsg);
static void Multicast_recv_timeout_process(DAL_Message_t* pMsg);
static void Multicast_recv_resume_process(DAL_Message_t* pMsg);
static int Multicast_perform_task_create(DAL_DownloadControl_t* p_control);
static int Multicast_perform_task_delete(DAL_DownloadControl_t* p_control);
static size_t my_fwrite(char *buffer,size_t size,size_t nitems,void *userp);

//}}} add by LYN, for multicast , 2011-08-22

#endif
static void M3u8_start_process( DAL_Message_t* pMsg);
static void M3u8_stop_process( DAL_Message_t* pMsg);
static size_t dal_m3u8_download_write_func(void *buffer, size_t size, size_t nmemb, void *stream);
static int m3u8_curl_preform_task_create( DAL_DownloadControl_t* p_control);
static void* m3u8_curl_preform_task_async( void* param);
static void timeshift_ts_stop_process( DAL_Message_t* pMsg);
static void timeshift_ts_start_process( DAL_Message_t* pMsg);
static void* time_shift_curl_preform_task_async( void* param);
static int Timeshift_curl_preform_task_delete( DAL_DownloadControl_t* p_control);
static int Timeshift_curl_preform_task_create( DAL_DownloadControl_t* p_control);

DAL_DownloadControl_t 			g_DownloadControl;
extern U32 p2v_movie_length;
extern U32 p2v_movie_duration;
extern U32 ts_rate;

int DAL_DownloadTaskCreate(void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	int							iret = 0;

	if( p_control->status != DOWNLOAD_IDEL)
	{
		//DAL_ERROR(("NOT DOWNLOAD_IDEL"));
		DebugMessage("[dal]DAL_DownloadTaskCreate ->NOT DOWNLOAD_IDEL");
		return -1;
	}
	iret = DAL_MessageQueueCreate( &(p_control->queueID), "DAL_DownloadQueue", DAL_DOWNLOAD_QUEUE_DEPTH, DAL_DOWNLOAD_QUEUE_DATA_SIZE, 0);
	if( iret != 0)
	{
		//DAL_ERROR(("DAL_MessageQueueCreate failed\n"));
		DebugMessage("[dal]DAL_DownloadTaskCreate ->DAL_MessageQueueCreate failed");
		return -2;
	}
	iret = DAL_SemCreate(&p_control->curl_exit_sem, "dal curl exit sem", 1, 0);
	if( iret != 0)
	{
		//DAL_ERROR(("DAL_SemCreate curl_exit_sem failed\n"));
		DebugMessage("[dal]DAL_DownloadTaskCreate ->DAL_SemCreate curl_exit_sem failed");
		DAL_MessageQueueDelete( p_control->queueID);
		return -3;
	}

	//add by LYN, for multicast , 2011-08-22
	/*create sem for the udp download task*/
    iret = DAL_SemCreate(&p_control->multicast_exit_sem,"dal mul exit sem", 1, 0);
    if( iret != 0)
	{
	  //  DAL_ERROR(("DAL_SemCreate multicast_exit_sem failed\n"));
	  	DebugMessage("[dal]DAL_DownloadTaskCreate ->DAL_SemCreate multicast_exit_sem failed");
		DAL_MessageQueueDelete( p_control->queueID);
		return -3;
	}
	//add by LYN, for multicast , 2011=08-22
	iret = DAL_TaskCreate( &(p_control->taskID), "DAL_DownloadTask", dal_download_task, NULL, NULL, DAL_DOWNLOAD_TASK_STACK_SIZE, DAL_DOWNLOAD_TASK_PRIOITY, 0);
	if( iret != 0)
	{
	//	DAL_ERROR(("DAL_TaskCreate failed\n"));
		DebugMessage("[dal]DAL_DownloadTaskCreate ->DAL_TaskCreate failed");
		DAL_SemDelete(p_control->curl_exit_sem);
		DAL_MessageQueueDelete( p_control->queueID);
		return -4;
	}
	/*
	iret = curl_check_task_create(p_control);
	if( iret != 0)
	{
		DAL_ERROR(("curl_check_task_create failed\n"));
		DAL_SemDelete(p_control->curl_exit_sem);
		DAL_TaskDelete(p_control->taskID);
		DAL_MessageQueueDelete( p_control->queueID);
		return -5;
	}
	*/
	p_control->status = DOWNLOAD_STOP;
	return 0;
}

int DAL_DownloadTaskDelete(void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;

	if( p_control->status == DOWNLOAD_IDEL)
	{
		//DAL_ERROR(("DOWNLOAD_IDEL\n"));
		DebugMessage("[dal]DAL_DownloadTaskCreate ->DOWNLOAD_IDEL");
		return -1;
	}
	DAL_TaskDelete( p_control->taskID);
	DAL_MessageQueueDelete( p_control->queueID);
	//curl_check_task_delete(p_control);
	p_control->status = DOWNLOAD_IDEL;
	return 0;
}
size_t ParseHttpHeader(char *buffer,
               size_t size,
               size_t nitems,
               void *userp)
{

		C8 cString[512];
		
		memset(cString, 0, sizeof(cString));
		sprintf(cString,"%s",buffer);
		
		if(!strncmp(cString,"Content-Duration:",17))
		{
			C8*pstr=NULL;
			
			pstr = strchr(cString, ':');
			if(pstr!=NULL)
			{
				pstr+=2;
				p2v_movie_duration=atoi(pstr);
			}
			else
			{
				DebugMessage("[ParseHttpHeader]Content-Duration: point NULL!");
				return -1;
			}
		}
		else if(!strncmp(cString,"Content-Length:",15))
		{
			C8*pstr=NULL;
			
			pstr = strchr(cString, ':');
			if(pstr!=NULL)
			{
				pstr=pstr+2;
				p2v_movie_length=atoi(pstr);
			}
			else
			{
				DebugMessage("[ParseHttpHeader]Content-Length: point NULL!");
				
			}
		}
	DebugMessage("[ParseHttpHeader] duration %d length:%d!",p2v_movie_duration,p2v_movie_length);
	
	return nitems;
}

int DAL_DownloadStart( dal_download_param_t* param)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pMsg = NULL;

	//DAL_DEBUG(("entry\n"));
	DebugMessage("[dal]DAL_DownloadStart ->entry");

	if( p_control->status == DOWNLOAD_IDEL)
	{
		//DAL_ERROR(("DOWNLOAD_IDEL"));
		DebugMessage("[dal]DAL_DownloadStart ->DOWNLOAD_IDEL");
		return -1;
	}

	/* 清空可能堆积的消息*/
	download_queue_clear();	

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_DOWNLOAD_START;
	memset( pMsg->MsgData.DownloadStart.url, 0, sizeof(pMsg->MsgData.DownloadStart.url));
	strcpy( pMsg->MsgData.DownloadStart.url, param->url);
	DAL_MessageSend( p_control->queueID, &pMsg);
	return 0;
}

int DAL_DownloadStop( void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pMsg = NULL;

	if( p_control->status == DOWNLOAD_IDEL)
	{
		//DAL_ERROR(("DOWNLOAD_IDEL"));
		DebugMessage("[dal]DAL_DownloadStop ->DOWNLOAD_IDEL");
		return -1;
	}	

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_DOWNLOAD_STOP;
	DAL_MessageSend( p_control->queueID, &pMsg);

	while( DAL_DownloadIsStop() != TRUE)
	{
		//DAL_DEBUG(("waiting for DAL_DownloadStop\n"));
		DebugMessage("[dal]DAL_DownloadStop ->waiting for DAL_DownloadStop");
		DAL_TaskDelay(200);
	}	
	return 0;
}

int DAL_DownloadBufferInit(void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	int							iret = 0;

	if( ring_buffer_is_init( &(p_control->download_buffer)) == 1)
	{
		//DAL_DEBUG(("BUFFER ALREADY INIT\n"));
		DebugMessage("BUFFER ALREADY INIT\n");
		return 0;//return -1; 
	}
	else
	{
		p_control->buffer_addr = DAL_Malloc(DAL_DOWNLOAD_BUFFER_SIZE);
		if( p_control->buffer_addr == NULL)
		{
			//DAL_ERROR(("DAL_Malloc failed\n"));
			DebugMessage("DAL_Malloc failed\n");
			return -2;
		}
		
		iret = ring_buffer_init_with_mem( &(p_control->download_buffer), p_control->buffer_addr, DAL_DOWNLOAD_BUFFER_SIZE);
		if( iret != 0)
		{
		
			DebugMessage("ring_buffer_init_with_mem failed\n");
			//DAL_ERROR(("ring_buffer_init_with_mem failed\n"));
			DAL_Free(p_control->buffer_addr);
			p_control->buffer_addr = NULL;
			return -2;
		}
	}
	return 0;
}

int DAL_DownloadBufferRelease(void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	int							iret = 0;

	if( ring_buffer_is_init( &(p_control->download_buffer)) == 0)
	{
		//DAL_DEBUG(("BUFFER NOT INIT\n"));
		DebugMessage("[DAL]DAL_DownloadBufferRelease ->BUFFER NOT INIT");
		return -1;
	}
	else
	{
		iret = ring_buffer_release_with_mem( &(p_control->download_buffer));
		if( iret != 0)
		{
			//DAL_DEBUG(("ring_buffer_release failed\n"));	
			DebugMessage("[DAL]DAL_DownloadBufferRelease ->ring_buffer_release failed");
			return -1;
		}
		if( p_control->buffer_addr != NULL)
		{
			DAL_Free(p_control->buffer_addr);
			p_control->buffer_addr = NULL;
		}
	}
	return 0;
}

int DAL_DownloadBufferGet( ring_buffer_t** ppbuffer)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;

	if( ring_buffer_is_init( &(p_control->download_buffer)) == 0)
	{
		//DAL_DEBUG(("BUFFER NOT INIT\n"));
		DebugMessage("[DAL]DAL_DownloadBufferGet ->BUFFER NOT INIT");
		*ppbuffer = NULL;
		return -1;
	}
	else
	{
		*ppbuffer = &(p_control->download_buffer);
	}
	return 0;
}

DAL_QueueID_t DAL_GetDownloadQueueID(void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;

	if( p_control->status == DOWNLOAD_IDEL)
	{
		//DAL_ERROR(("DOWNLOAD_IDEL"));
		DebugMessage("[DAL]DAL_GetDownloadQueueID ->DOWNLOAD_IDEL");
		return -1;
	}

	return p_control->queueID;
}

BOOL DAL_DownloadIsStop( void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	return (p_control->status == DOWNLOAD_STOP) ? 1 : 0;
}


static void* dal_download_task( void* param)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pMsg = NULL;
	U32							SizeCopy;
	int							iret = 0;

	//DAL_DEBUG(("wait queue %d\n", p_control->queueID));
	while(1)
	{
		iret = DAL_MessageQueueReceive( p_control->queueID, &pMsg, sizeof(DAL_Message_t*), &SizeCopy, DAL_PEND);
		if( iret < 0)
		{
			//DAL_DEBUG(("DAL_MessageQueueReceive no message\n"));
			continue;
		}
		else
		{
		
		DebugMessage("[DAL]dal_download_task -> 	 Recv MSG: %s	%d\n", DAL_MessageString( pMsg->MsgType), iret);
			if(  DALMSG_CHECK_PARSE_DATA_READY != pMsg->MsgType
				&& DALMSG_CURL_REV_TIMEOUT != pMsg->MsgType
				&& DALMSG_HDD_READ_CONT != pMsg->MsgType)
			{
		//		DebugMessage("[DAL]dal_download_task ->		 Recv MSG: %s   %d\n", DAL_MessageString( pMsg->MsgType), iret);
				//DAL_DEBUG(("                              Recv MSG: %s   %d\n", DAL_MessageString( pMsg->MsgType), iret ));
			}
			switch( pMsg->MsgType)
			{
				case DALMSG_DOWNLOAD_START: download_start_process( pMsg); break;
				case DALMSG_DOWNLOAD_STOP: download_stop_process( pMsg); break;
				case DALMSG_CURL_ERROR: download_curl_error_process(pMsg); break;
				case DALMSG_HDD_READ_CONT: download_hdd_read_cont_process(pMsg); break;
				case DALMSG_DATA_REQUEST: data_request_process(pMsg);break;
				case DALMSG_CHECK_PARSE_DATA_READY: check_parse_data_ready_process(pMsg); break;
				case DALMSG_CURL_REV_TIMEOUT: curl_recv_timeout_process(pMsg); break;
				case DALMSG_CURL_REV_RESUME: curl_recv_resume_process(pMsg); break;
				
#if defined(USE_DAL_MULTICAST)
				//{{{ add by LYN , for multicast , 2011-08-22
				case DALMSG_MULTICAST_START:        Multicast_start_process( pMsg); break;
				case DALMSG_MULTICAST_STOP:         Multicast_stop_process(pMsg);break;
				case DALMSG_MULTICAST_ERROR:        Multicast_error_process(pMsg);break;
				case DALMSG_MULTICAST_REV_TIMEOUT:  Multicast_recv_timeout_process(pMsg);break;
				case DALMSG_MULTICAST_REV_RESUME:   Multicast_recv_resume_process(pMsg);break;		
				//}}} add by LYN , for multicast , 2011-08-22
#endif

				case DALMSG_M3U8_TSDOWNLOAD:		M3u8_start_process(pMsg);break;
				case DALMSG_M3U8_TSSTOP: 			M3u8_stop_process( pMsg); break;
				//case DALMSG_M3U8_ERROR: 			//M3u8_curl_error_process(pMsg); break;

				case DALMSG_TIMESHIFT_TSDOWNLOAD_START:	timeshift_ts_start_process(pMsg);break;
				case DALMSG_TIMESHIFT_TSDOWNLOAD_STOP:	timeshift_ts_stop_process(pMsg);break;
				case DALMSG_TIMESHIFT_DOWNLOAD:			dal_timeshift_download_start(pMsg);break;
				case DALMSG_TIMESHIFT_STOP:				dal_timeshift_download_stop(pMsg);break;

				default: 				break;
			}
			DAL_MessageFree(pMsg);
		}
		DAL_TaskDelay(100);
	}
	return (void*)0;
}

extern char p2vheader[255];
static void download_start_process( DAL_Message_t* pMsg)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				p_msg = NULL;
	int							iret = 0;

	if( p_control->status == DOWNLOAD_IDEL)
	{
		//DAL_ERROR(("DOWNLOAD_IDEL\n"));
		DebugMessage("DOWNLOAD_IDEL\n");
		return ;
	}

	if( strncmp( pMsg->MsgData.DownloadStart.url, "http:", strlen("http:"))== 0)
	{
		p_control->type = DAL_DL_HTTP;
	}
	else
	{
		p_control->type = DAL_DL_LOCAL;
	}

	if( p_control->status == DOWNLOAD_START)
	{
		if( DAL_DL_HTTP == p_control->type)
		{
			//DAL_DEBUG(("ALREADY DOWNLOAD_START , STOP AND RESTART CURL\n"));
			DebugMessage("ALREADY DOWNLOAD_START , STOP AND RESTART CURL\n");
			//curl_check_task_pause(p_control);
			iret = curl_preform_task_delete( p_control);
			if( iret != 0)
			{
				//DAL_ERROR(("curl_preform_task_delete failed\n"));
				DebugMessage("curl_preform_task_delete failed\n");
				return ;
			}
		}
		else
		{
			//DAL_ERROR(("error operation!\n"));
			DebugMessage("error operation!\n");
			return ;
		}
	}

	// start new download
	if( ring_buffer_is_init( &(p_control->download_buffer)) == 0)
	{
		//DAL_ERROR(("DOWNLOAD BUFFER NOT INIT\n"));
		DebugMessage("DOWNLOAD BUFFER NOT INIT\n");
		return ;
	}
	
	ring_buffer_flush(&(p_control->download_buffer));
	DebugMessage("DOWNLOAD start url:%s\n",pMsg->MsgData.DownloadStart.url);

	memset( p_control->url, 0, sizeof(p_control->url));
	strcpy( p_control->url, pMsg->MsgData.DownloadStart.url);

	/*每次重新开始CURL  下载, 重新设置此标志*/
	p_control->parse_pids_flag = PPF_NO_REQUEST;

	if( DAL_DL_HTTP == p_control->type)
	{
		curl_off_t cByteS = 200 * 1024;
		DebugMessage("DOWNLOAD libcurl start");
		p_control->curl_rev_data = 0;
		p_control->data_timeout_flag = 0;
		p_control->curl_handle = curl_easy_init();
		p_control->curl_exit_flag = 0;
		p_control->no_data_times = 0;

		curl_easy_setopt( p_control->curl_handle, CURLOPT_URL, p_control->url); 
		curl_easy_setopt( p_control->curl_handle, CURLOPT_FOLLOWLOCATION,1);
		if(GetP2vResumeflag() ==0)
		{
			curl_easy_setopt( p_control->curl_handle, CURLOPT_HEADERFUNCTION, ParseHttpHeader);//xzhou add for p2v FF&rew
		}
		curl_easy_setopt( p_control->curl_handle, CURLOPT_WRITEFUNCTION, dal_download_write_func);
		curl_easy_setopt( p_control->curl_handle, CURLOPT_MAX_RECV_SPEED_LARGE, cByteS);
		//curl_easy_setopt( p_control->curl_handle, CURLOPT_VERBOSE, 1L);
		iret = curl_preform_task_create( p_control);
		if( iret != 0)
		{
			//DAL_ERROR(("curl_preform_task_create failed with %d\n", iret));
			DebugMessage("curl_preform_task_create failed with %d\n", iret);
			curl_easy_cleanup( p_control->curl_handle);
			p_control->status = DOWNLOAD_STOP;
			return ;
		}
		//curl_check_task_resume(p_control);
	}
	else
	{
	#if 1
		if( p_control->parse_pids_flag != PPF_WAIT_READY)
		{
			/*
			DAL_DEBUG(("wait fot parse data ready\n"));
			p_msg = DAL_MessageAlloc();
			p_msg->MsgType = DALMSG_DOWNLOAD_START;
			memcpy(p_msg,pMsg,sizeof(DAL_Message_t));
			DAL_MessageSend( p_control->queueID, &p_msg);
			DAL_TaskDelay(500);
			return ;
			*/
			DAL_TaskDelay(1000);
		}
		
		p_control->hdd_ts_file_handle = fopen(p_control->url,"rb");
		if( NULL == p_control->hdd_ts_file_handle)
		{
			//DAL_ERROR(("fopen error!\n"));
			DebugMessage("fopen error!\n");
			return ;
		}
		p_control->p_hdd_read_buf = DAL_Malloc(DAL_HDD_READ_BLOCK_SIZE);
		if( NULL == p_control->p_hdd_read_buf)
		{			
			//DAL_ERROR(("DAL_Malloc error!\n"));
			DebugMessage("DAL_Malloc error!\n");
			fclose(p_control->hdd_ts_file_handle);
			return ;
		}
		p_msg = DAL_MessageAlloc();
		p_msg->MsgType = DALMSG_HDD_READ_CONT;
		DAL_MessageSend( p_control->queueID, &p_msg);
		//DAL_DEBUG(("dal hdd ts read start ok\n"));
		DebugMessage("dal hdd ts read start ok\n");
	#endif	
	}
	p_control->status = DOWNLOAD_START;
}

static void download_stop_process( DAL_Message_t* pMsg)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	int							iret = 0;

	if( p_control->status == DOWNLOAD_IDEL)
	{
		//DAL_ERROR(("DOWNLOAD_IDEL\n"));
		DebugMessage("DOWNLOAD_IDEL\n");
		return ;
	}

	if( p_control->status != DOWNLOAD_START)
	{
		//DAL_ERROR(("NOT DOWNLOAD_ING\n"));
		DebugMessage("DOWNLOAD_IDEL\n");
		return ;
	}

	if( DAL_DL_HTTP == p_control->type)
	{
		/*for p2s , delete p2s data download task*/
		//curl_check_task_pause(p_control);
		//DAL_DEBUG(("before curl_preform_task_delete, waiting...\n"));
		DebugMessage("before curl_preform_task_delete, waiting...\n");
		iret = curl_preform_task_delete( p_control);
		if( iret != 0)
		{
			//DAL_ERROR(("curl_preform_task_delete failed\n"));
			DebugMessage("before curl_preform_task_delete, waiting...\n");
			return ;
		}
		//DAL_DEBUG(("curl_preform_task_delete ok\n"));
		DebugMessage("before curl_preform_task_delete, waiting...\n");
	}
	else
	{
		/*for local ts play, close the file*/
		DAL_Free(p_control->p_hdd_read_buf);
		fclose(p_control->hdd_ts_file_handle);
		p_control->hdd_ts_file_handle = NULL;
	}
	p_control->status = DOWNLOAD_STOP;
}

static void download_curl_error_process( DAL_Message_t* pMsg)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pmsg = NULL;
	int							iret = 0;

	/*curl perform 自己退出了*/
//	DAL_ERROR(("CURL ERROR  %s\n", curl_easy_strerror(p_control->curl_error)));
	DebugMessage("CURL ERROR  %s\n", curl_easy_strerror(p_control->curl_error));

	//curl_check_task_pause(p_control);
	iret = curl_preform_task_delete( p_control);
	if( iret != 0)
	{
	//	DAL_ERROR(("curl_preform_task_delete failed\n"));
		DebugMessage("curl_preform_task_delete failed\n");
		return ;
	}
	//DAL_ERROR(("curl_preform_task_delete success\n"));
	DebugMessage("curl_preform_task_delete success\n");
	p_control->status = DOWNLOAD_STOP;

//	Get_play_time();
	Reset_inject_totalnum();//nlv

         dal_core_event_notify(DAL_CORE_EVENT_PLAYEND,NULL);
	/*自动重新开始下载*/
#if 1
	pmsg = DAL_MessageAlloc();
	pmsg->MsgType = DALMSG_DOWNLOAD_START;
	memset(pmsg->MsgData.DownloadStart.url, 0, sizeof(MsgData_DownloadStart_t));

	
	{
		char *str=NULL;
		char tmp[255],tmp1[255];
		U32 length=0,Slen=0,Slen1=0;

		memset(tmp,0,sizeof(tmp));
		memset(tmp1,0,sizeof(tmp1));
		strcpy(tmp,p_control->url);
		length=strlen(tmp);
	
		DebugMessage("[delete offset]:^^^^^%s^^^^%d",tmp,length);
		str=tmp+length-1;
		while(*str!='/')
		{
			str=str-1;
		}
		Slen=strlen(str);
		strncpy(tmp1,str,Slen);
		
		DebugMessage("[delete offset]:^^^^^%s^^^^%d",str,Slen);
		str=str-1;
		while(*str!='/')
		{
			str=str-1;
		}
		Slen1=strlen(str);
		
		DebugMessage("[delete offset]:^^^^^%s^^^^%d",str,Slen1);
		if(!strncmp(str,"/offset=", 8))
		{
			strncpy(pmsg->MsgData.DownloadStart.url,tmp,length-Slen1);
			strcat(pmsg->MsgData.DownloadStart.url,tmp1);
		}
		else
		{
			strcpy(pmsg->MsgData.DownloadStart.url, p_control->url);

		}
		
		DebugMessage("[delete offset]:^^^^^%s",pmsg->MsgData.DownloadStart.url);
	}
	DAL_MessageSend( p_control->queueID, &pmsg);
#endif
}

static void download_hdd_read_cont_process( DAL_Message_t* pMsg)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				p_msg = NULL;
	unsigned int 				p_freesize = 0;
	unsigned int				read_size = 0;
	unsigned int				data_request = 0;
	int							iret = 0;
	
	if( p_control->status != DOWNLOAD_START)
	{
		//DAL_ERROR(("NOT STATUS DOWNLOAD_START\n"));
		DebugMessage("NOT STATUS DOWNLOAD_START\n");
		return ;
	}

/*
	if( p_control->parse_pids_flag != PPF_WAIT_READY)
	{
		DAL_DEBUG(("wait fot parse data ready\n"));
		goto send_cont;
	}
*/

	iret = ring_buffer_freesize( &(p_control->download_buffer), &p_freesize);
	if( iret != 0)
	{
		//DAL_ERROR(("ring_buffer_freesize failed\n"));
		DebugMessage("[dal]ring_buffer_freesize failed\n");
		goto send_cont;
	}
	
	//if( p_freesize < (DAL_DOWNLOAD_BUFFER_SIZE-(64*1024)))
	if( p_freesize < (DAL_DOWNLOAD_BUFFER_SIZE-(128*1024)))
	{
	
		DebugMessage("[dal]p_freesize:  %u  < %u  \n", p_freesize, (DAL_DOWNLOAD_BUFFER_SIZE-(64*1024)));
		//DAL_DEBUG(("p_freesize:  %u  < %u  \n", p_freesize, (DAL_DOWNLOAD_BUFFER_SIZE-(64*1024))));
		DAL_TaskDelay(500);
		goto send_cont;
	}

	if( p_control->p_hdd_read_buf != NULL)
	{
		memset(p_control->p_hdd_read_buf,0,DAL_HDD_READ_BLOCK_SIZE);
		read_size = fread( p_control->p_hdd_read_buf, 1, DAL_HDD_READ_BLOCK_SIZE, p_control->hdd_ts_file_handle);
		if( read_size < 0)
		{
			//DAL_ERROR(("fread failed with %d\n", read_size));	
			DebugMessage("[dal]fread failed with %d\n", read_size);
			fseek(p_control->hdd_ts_file_handle, 0, SEEK_SET);
			goto send_cont;
		}
		if( read_size < DAL_HDD_READ_BLOCK_SIZE)
		{
			//DAL_ERROR(("fread data %d\n", read_size));
			DebugMessage("[dal]fread data %d\n", read_size);
			fseek(p_control->hdd_ts_file_handle, 0, SEEK_SET);
		}
		//DAL_DEBUG(("fread OK %d\n", read_size));
		DebugMessage("[dal]fread OK %d\n", read_size);
	}
	iret = ring_buffer_write( &(p_control->download_buffer), (unsigned char*)p_control->p_hdd_read_buf, read_size);
	if( iret != 0)
	{
		//DAL_ERROR(("ring_buffer_write failed\n"));
		DebugMessage("[dal]ring_buffer_write failed");
		goto send_cont;
	}

	/*提供数据给parse task*/
	if( p_control->parse_pids_flag == PPF_WAIT_READY)
	{
		if( p_control->parse_buf_size == p_control->parse_data_size)
		{
			p_control->parse_pids_flag = PPF_DATA_READY;
			//DAL_DEBUG((" PPF_DATA_READY\n"));
			DebugMessage("[dal]PPF_DATA_READY");
		}
		else
		{
			data_request = p_control->parse_buf_size - p_control->parse_data_size;
			if( data_request >= DAL_HDD_READ_BLOCK_SIZE)
			{
				memcpy(p_control->p_parse_buffer + p_control->parse_data_size, p_control->p_hdd_read_buf, DAL_HDD_READ_BLOCK_SIZE);
				p_control->parse_data_size += DAL_HDD_READ_BLOCK_SIZE;
			}
			else
			{
				memcpy(p_control->p_parse_buffer + p_control->parse_data_size, p_control->p_hdd_read_buf, data_request);
				p_control->parse_data_size += data_request;
				DebugMessage("[dal]parse_data_size: %d   data_request: %d\n", p_control->parse_data_size, data_request);
				//DAL_DEBUG((" parse_data_size: %d   data_request: %d\n", p_control->parse_data_size, data_request));
			}
			//DAL_DEBUG((" parse_data_size: %d   data_request: %d\n", p_control->parse_data_size, data_request));
		}
		
		if( p_control->parse_buf_size == p_control->parse_data_size)
		{
			p_control->parse_pids_flag = PPF_DATA_READY;
			//DAL_DEBUG((" PPF_DATA_READY\n"));
			
			DebugMessage("[dal]PPF_DATA_READY");
		}		
	}

send_cont:
	p_msg = DAL_MessageAlloc();
	p_msg->MsgType = DALMSG_HDD_READ_CONT;
	DAL_MessageSend( p_control->queueID, &p_msg);
}

static void data_request_process( DAL_Message_t* pMsg)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pmsg = NULL;
	DebugMessage("~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	if( p_control->status != DOWNLOAD_START)
	{
		//DAL_ERROR(("NOT DOWNLOAD_START\n"));
		DebugMessage("NOT DOWNLOAD_START\n");
		return ;
	}
	DebugMessage("[data_request_process]parse_pids_flag:%d",p_control->parse_pids_flag);

	if( p_control->parse_pids_flag == PPF_WAIT_READY)
	{
		//DAL_ERROR(("ALREADY PPF_WAIT_READY\n"));
		DebugMessage("ALREADY PPF_WAIT_READY\n");
		return ;
	}
	
	/*设置参数，在curl 写函数中接收数据*/
	p_control->p_parse_buffer = pMsg->MsgData.DataRequest.p_parse_buffer;
	p_control->parse_buf_size = pMsg->MsgData.DataRequest.parse_buf_size;
	p_control->parse_data_size = 0;	
	p_control->parse_pids_flag = PPF_WAIT_READY;

	//DAL_DEBUG(("parse_buf_size: %u    parse_data_size: %d\n", p_control->parse_buf_size, p_control->parse_data_size));
	DebugMessage("parse_buf_size: 0x%x    parse_data_size: %d\n", p_control->parse_buf_size, p_control->parse_data_size);

	/*发消息检测数据是否准备就绪*/
	pmsg = DAL_MessageAlloc();
	pmsg->MsgType = DALMSG_CHECK_PARSE_DATA_READY;
	DAL_MessageSend( p_control->queueID, &pmsg);	
}

static void check_parse_data_ready_process( DAL_Message_t* pMsg)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pmsg = NULL;
	
	if( p_control->parse_pids_flag == PPF_WAIT_READY && p_control->status != DOWNLOAD_STOP)
	{
		DAL_TaskDelay(1000);							// 以后最好换成定时器
		/*这里的延时，会导致download task 不能更快的处理队列中的消息
		连续的消息会堆积在队列中*/
		DebugMessage("1 check_parse_data_ready_process %d",p_control->parse_pids_flag );
		DebugMessage("2 check_parse_data_ready_process %d",p_control->parse_pids_flag );

		/*延时后重新检查状态*/
		if( p_control->parse_pids_flag == PPF_WAIT_READY && p_control->status != DOWNLOAD_STOP)
		{
		pmsg = DAL_MessageAlloc();
		pmsg->MsgType = DALMSG_CHECK_PARSE_DATA_READY;
		DAL_MessageSend( p_control->queueID, &pmsg);
			return ;
		}
	}
	
	DebugMessage("3 check_parse_data_ready_process %d",p_control->parse_pids_flag );
	DebugMessage("4 check_parse_data_ready_process %d",p_control->parse_pids_flag );
	if( p_control->parse_pids_flag == PPF_DATA_READY)
	{
		p_control->parse_pids_flag = PPF_NO_REQUEST;
		pmsg = DAL_MessageAlloc();
		pmsg->MsgType = DALMSG_PARSE_DATA_READY;
		DAL_MessageSend( DAL_GetParseQueueID(), &pmsg);
		DebugMessage("Send DALMSG_PARSE_DATA_READY to ParseTask\n");
		return ;
	}
		DebugMessage("ALREADY PPF_NO_REQUEST\n");
	}

static void curl_recv_timeout_process( DAL_Message_t* pMsg)
{
	DAL_Message_t*				pmsg = NULL;
	
	//DAL_DEBUG(("   CURL RECEIVE DATA TIMEOUT !!!\n"));
	pmsg = DAL_MessageAlloc();
	pmsg->MsgType = DALMSG_DATA_TIMEOUT;
	DAL_MessageSend( DAL_GetCoreQueueID(), &pmsg);
}

static void curl_recv_resume_process( DAL_Message_t* pMsg)
{
	DAL_Message_t*				pmsg = NULL;
	
	//DAL_DEBUG(("   CURL RECEIVE DATA TIMEOUT !!!\n"));
	pmsg = DAL_MessageAlloc();
	pmsg->MsgType = DALMSG_DATA_RESUME;
	DAL_MessageSend( DAL_GetCoreQueueID(), &pmsg);
}

void download_queue_clear( void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	int							iret = 0;
	DAL_Message_t*				pMsg = NULL;
	U32							SizeCopy = 0;
	U32							msg_clear = 0;

//	DAL_DEBUG(("entry\n"));
	DebugMessage("entry\n");

	while( (iret = DAL_MessageQueueReceive( p_control->queueID, &pMsg, sizeof(DAL_Message_t*), &SizeCopy, DAL_CHECK)) >= 0)
	{
		msg_clear++;
		DAL_DEBUG(("clear a message  %s    %d\n", DAL_MessageString( pMsg->MsgType),iret));
		DAL_MessageFree(pMsg);
	}
//	DAL_DEBUG(("^^^^^^^^^^^^^^^^^^^^^^^^  clear %d message!!!\n", msg_clear));
	DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^  clear %d message!!!\n", msg_clear);

}


static int curl_preform_task_create( DAL_DownloadControl_t* p_control)
{
	int						iret = 0;

	iret = DAL_SemWait_Timeout( p_control->curl_exit_sem, 2000);
	if( iret != 0)
	{
		//DAL_ERROR(("DAL_SemWait_Timeout failed\n"));
		DebugMessage("DAL_SemWait_Timeout failed\n");
		return iret;
	}
	
	//iret = DAL_TaskCreate( &(p_control->curl_taskID), "curl preform task", curl_preform_task_sync, NULL, NULL, DAL_CURL_TASK_STACK_SIZE, DAL_CURL_TASK_PRIOITY,0);
	iret = DAL_TaskCreate( &(p_control->curl_taskID), "curl preform task", curl_preform_task_async, NULL, NULL, DAL_CURL_TASK_STACK_SIZE, DAL_CURL_TASK_PRIOITY,0);
	if( iret != 0)
	{
		return iret;
	}
	return 0;
}

static int curl_preform_task_delete( DAL_DownloadControl_t* p_control)
{
	int 						iret = 0;
	
	p_control->curl_exit_flag = 1;
	//DAL_DEBUG(("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ set flag to stop perform task\n"));
	DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ set flag to stop perform task\n");

	iret = DAL_SemWait_Timeout(p_control->curl_exit_sem, 5000);
	if( iret != 0)
	{
		//DAL_ERROR(("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DAL_SemWait_Timeout failed\n"));
		//DAL_ERROR(("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ perform task delete failed\n"));
		DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DAL_SemWait_Timeout failed\n");
		DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ perform task delete failed\n");
	}
	else
	{
		DAL_SemSignal(p_control->curl_exit_sem);
		//DAL_DEBUG(("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ perform task delete success\n"));
		DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ perform task delete success\n");
		return 0;
	}
	return -1;
}
#if 0
static int curl_check_task_create( DAL_DownloadControl_t* p_control)
{
	int						iret = 0;

	iret = DAL_SemCreate( &(p_control->check_sem), "curl chekc sem", 0, 0);
	if( iret != 0)
	{
		return iret;
	}
	iret = DAL_TaskCreate( &(p_control->check_taskID), "curl check task", curl_check_task, NULL, NULL, DAL_CURL_TASK_STACK_SIZE, DAL_CURL_TASK_PRIOITY,0);
	if( iret != 0)
	{
		DAL_SemDelete(p_control->check_sem);
		return iret;
	}
	return 0;
}

static int curl_check_task_delete( DAL_DownloadControl_t* p_control)
{
	int						iret = 0;
	
	iret = DAL_TaskDelete( p_control->check_taskID);
	if( iret != 0)
	{
		return iret;
	}
	iret = DAL_SemDelete(p_control->check_sem);
	{
		return iret;
	}
	return 0;
}

static int curl_check_task_pause( DAL_DownloadControl_t* p_control)
{	
	p_control->no_data_times = 0;
	DAL_SemWait(p_control->check_sem);
	return 0;
}

static int curl_check_task_resume( DAL_DownloadControl_t* p_control)
{
	DAL_SemSignal(p_control->check_sem);
	return 0;
}
#endif
#if 0
static size_t  my_fwrite(char *buffer,
               size_t size,
               size_t nitems,
               void *userp)
{
	FILE *fp = (FILE *)userp;
	int iRet=0;
	if(fp == NULL)
		return -1;
	
	//iRet = fwrite(fp, buffer, size*nitems);
	iRet =fwrite(buffer,size,nitems,fp);
	
	DebugMessage("[dal] my_fwrit %c%c%c%c%c%c%c size:%d! (%x)",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],size*nitems,*fp);
	if ( 0 >= iRet )
	{
		pbiinfo("[%s %d] write error.\n",DEBUG_LOG);
		return -1;
	}
	return nitems;
}

static size_t  my2_fwrite(char *buffer,
               size_t size,
               size_t nitems,
               void *userp)
{
	FILE *fp = (FILE *)userp;
	int iRet=0;
	if(fp == NULL)
		return -1;
	
	//iRet = fwrite(fp, buffer, size*nitems);
	iRet =fwrite(buffer,size,nitems,fp);
	
	DebugMessage("[dal] my_fwrit %c%c%c%c%c%c%c size:%d! (%x)",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],size*nitems,*fp);
	if ( 0 >= iRet )
	{
		pbiinfo("[%s %d] write error.\n",DEBUG_LOG);
		return -1;
	}
	return nitems;
}



S32 dal_dowload_m3u8(void)
{
		CURL* curl;
		CURLcode res;
		FILE* filefp;
		FILE* headerfp;
		
		//U8 fbuffer[2048];
//	char *url = "http://so.52itv.cn/live/72676552616B34796E4E6C73484E516B726634376F67767953356E73.m3u8";
//	char *url ="http://sw.live.cntv.cn/cctv_p2p_hdcctv6.m3u8";
		char *url ="http://221.224.106.86:80/160/15597/4568/1.m3u8?swax_k=_ver_tm_ar_sig&swax_v=_1.0_1368263985_197_9d0aa2fc2e";

		curl_global_init(CURL_GLOBAL_DEFAULT);

		curl = curl_easy_init();
		if(curl == NULL)
		{
			DebugMessage("curl init failed\n");
			return -1;
		}
		/*open file which we want to get*/
		filefp = fopen("/mnt/sda/sda1/text.txt","w+");
		if(filefp == NULL)
		{
			DebugMessage("LibAdpt_GetHttpFile:open get  text file error\n");
			curl_easy_cleanup(curl);
			return -1;
		}

		/*open a temp file which store the http header */	
		headerfp = fopen("/mnt/sda/sda1/header.txt","w+");
		if(headerfp == NULL)
		{
			DebugMessage("LibAdpt_GetHttpFile:open head  file error\n");
			fclose(filefp);
			curl_easy_cleanup(curl);
			return -1;
		}

	//	memset(header,0,sizeof(HttpResponseHead_t));

		/*set URL for a link*/
		curl_easy_setopt(curl, CURLOPT_URL,url);

		curl_easy_setopt (curl, CURLOPT_HEADER,0); 

		/*set callback function and filepointer for the http header*/
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, my_fwrite);	
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, headerfp);

		/*set callback function and filepointer for the download file*/
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my2_fwrite);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, filefp);

		curl_easy_setopt(curl,CURLOPT_TIMEOUT,20);		//operate outtime is 5 sencond
		
		res = curl_easy_perform(curl);
#if 0
		switch(res)
		{
			case CURLE_OK:/*download header and file successful,so parse the http header */
			case CURLE_PARTIAL_FILE:
			case CURLE_OPERATION_TIMEDOUT: /*operate time out */
				{
					S32 fsize =0 ;
					/*close file pointer*/
					if(filefp)
						fclose(filefp);
					
					fseek(headerfp,0,SEEK_SET);
					memset(fbuffer,0,2048);
					fsize =fread(fbuffer,1,2048,headerfp);
					
					/*parse http header*/
					ParseHttpResponseHead(fbuffer,fsize,header);
					fclose(headerfp);	

					if(res == CURLE_OK
						||CURLE_PARTIAL_FILE==res)
					{
						header->result = 0;
						res = 0;
					}
					else
					{
						header->result = 1;
						res = 1;
					}
					break;
				}
			
			default:
				{
					res = 2;
					header->result = 2;
					if(filefp)
						fclose(filefp);
					if(headerfp)
						fclose(headerfp);
						break;
				}

		}

	curl_easy_cleanup(curl);
#endif	
	fclose(filefp);
	fclose(headerfp);

	return res;
}

S32 dal_dowload_m3u8_pro(void)
{
		CURL* curl;
		CURLcode res;
		FILE* filefp;
		FILE* headerfp;
		
		//U8 fbuffer[2048];
//	char *url = "http://so.52itv.cn/live/72676552616B34796E4E6C73484E516B726634376F67767953356E73.m3u8";
	char *url ="http://sw.live.cntv.cn/cctv_p2p_hdcctv6.m3u8";
//		char *url ="http://221.224.106.86:80/160/15597/4568/1.m3u8?swax_k=_ver_tm_ar_sig&swax_v=_1.0_1368263985_197_9d0aa2fc2e";

		curl_global_init(CURL_GLOBAL_DEFAULT);

		curl = curl_easy_init();
		if(curl == NULL)
		{
			DebugMessage("curl init failed\n");
			return -1;
		}
		/*open file which we want to get*/
		filefp = fopen("/mnt/sda/sda1/text_pro.txt","w+");
		if(filefp == NULL)
		{
			DebugMessage("LibAdpt_GetHttpFile:open get  text file error\n");
			curl_easy_cleanup(curl);
			return -1;
		}

		/*open a temp file which store the http header */	
		headerfp = fopen("/mnt/sda/sda1/header_pro.txt","w+");
		if(headerfp == NULL)
		{
			DebugMessage("LibAdpt_GetHttpFile:open head  file error\n");
			fclose(filefp);
			curl_easy_cleanup(curl);
			return -1;
		}

	//	memset(header,0,sizeof(HttpResponseHead_t));

		/*set URL for a link*/
		curl_easy_setopt(curl, CURLOPT_URL,url);

		curl_easy_setopt (curl, CURLOPT_HEADER,0); 

		/*set callback function and filepointer for the http header*/
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, my_fwrite);	
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, headerfp);

		/*set callback function and filepointer for the download file*/
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my2_fwrite);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, filefp);

		curl_easy_setopt(curl,CURLOPT_TIMEOUT,20);		//operate outtime is 5 sencond
		
		res = curl_easy_perform(curl);
#if 0
		switch(res)
		{
			case CURLE_OK:/*download header and file successful,so parse the http header */
			case CURLE_PARTIAL_FILE:
			case CURLE_OPERATION_TIMEDOUT: /*operate time out */
				{
					S32 fsize =0 ;
					/*close file pointer*/
					if(filefp)
						fclose(filefp);
					
					fseek(headerfp,0,SEEK_SET);
					memset(fbuffer,0,2048);
					fsize =fread(fbuffer,1,2048,headerfp);
					
					/*parse http header*/
					ParseHttpResponseHead(fbuffer,fsize,header);
					fclose(headerfp);	

					if(res == CURLE_OK
						||CURLE_PARTIAL_FILE==res)
					{
						header->result = 0;
						res = 0;
					}
					else
					{
						header->result = 1;
						res = 1;
					}
					break;
				}
			
			default:
				{
					res = 2;
					header->result = 2;
					if(filefp)
						fclose(filefp);
					if(headerfp)
						fclose(headerfp);
						break;
				}

		}

	curl_easy_cleanup(curl);
#endif	
	fclose(filefp);
	fclose(headerfp);

	return res;
}

S32 dal_dowload_m3u8_first(void)
{
		CURL* curl;
		CURLcode res;
		FILE* filefp;
		FILE* headerfp;
		
		//U8 fbuffer[2048];
	char *url = "http://so.52itv.cn/live/72676552616B34796E4E6C73484E516B726634376F67767953356E73.m3u8";
//	char *url ="http://sw.live.cntv.cn/cctv_p2p_hdcctv6.m3u8";
//		char *url ="http://221.224.106.86:80/160/15597/4568/1.m3u8?swax_k=_ver_tm_ar_sig&swax_v=_1.0_1368263985_197_9d0aa2fc2e";

		curl_global_init(CURL_GLOBAL_DEFAULT);

		curl = curl_easy_init();
		if(curl == NULL)
		{
			DebugMessage("curl init failed\n");
			return -1;
		}
		/*open file which we want to get*/
		filefp = fopen("/mnt/sda/sda1/text_first.txt","w+");
		if(filefp == NULL)
		{
			DebugMessage("LibAdpt_GetHttpFile:open get  text file error\n");
			curl_easy_cleanup(curl);
			return -1;
		}

		/*open a temp file which store the http header */	
		headerfp = fopen("/mnt/sda/sda1/header_first.txt","w+");
		if(headerfp == NULL)
		{
			DebugMessage("LibAdpt_GetHttpFile:open head  file error\n");
			fclose(filefp);
			curl_easy_cleanup(curl);
			return -1;
		}

	//	memset(header,0,sizeof(HttpResponseHead_t));

		/*set URL for a link*/
		curl_easy_setopt(curl, CURLOPT_URL,url);

		curl_easy_setopt (curl, CURLOPT_HEADER,0); 

		/*set callback function and filepointer for the http header*/
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, my_fwrite);	
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, headerfp);

		/*set callback function and filepointer for the download file*/
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my2_fwrite);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, filefp);

		curl_easy_setopt(curl,CURLOPT_TIMEOUT,20);		//operate outtime is 5 sencond
		
		res = curl_easy_perform(curl);
#if 0
		switch(res)
		{
			case CURLE_OK:/*download header and file successful,so parse the http header */
			case CURLE_PARTIAL_FILE:
			case CURLE_OPERATION_TIMEDOUT: /*operate time out */
				{
					S32 fsize =0 ;
					/*close file pointer*/
					if(filefp)
						fclose(filefp);
					
					fseek(headerfp,0,SEEK_SET);
					memset(fbuffer,0,2048);
					fsize =fread(fbuffer,1,2048,headerfp);
					
					/*parse http header*/
					ParseHttpResponseHead(fbuffer,fsize,header);
					fclose(headerfp);	

					if(res == CURLE_OK
						||CURLE_PARTIAL_FILE==res)
					{
						header->result = 0;
						res = 0;
					}
					else
					{
						header->result = 1;
						res = 1;
					}
					break;
				}
			
			default:
				{
					res = 2;
					header->result = 2;
					if(filefp)
						fclose(filefp);
					if(headerfp)
						fclose(headerfp);
						break;
				}

		}

	curl_easy_cleanup(curl);
#endif	
	fclose(filefp);
	fclose(headerfp);

	return res;

}
void mWrite( void *pBuffer, int Size)
{
	return ;
	int iRet = 0,fp=0;
	static int aaa = 0;
	pbiinfo("[%s %d]  size = %d.\n",DEBUG_LOG, Size);
	if( NULL == pBuffer || 0 >= Size)
	{
		pbiinfo("[%s %d]  buffer is null size = %d.\n",DEBUG_LOG, Size);
		return;
	}
	if( 0 == aaa )
	{
		aaa++;
		//system("chmod 777 /data/data/com.pbi.dvb");
		fp = open("/mnt/sda/sda1/text.txt",O_RDWR|O_CREAT|O_SYNC, 777);
		if( -1 == fp )
		{
			pbiinfo("[%s %d] open error.\n",DEBUG_LOG);
			return;
		}
		//system("chmod 777 /data/data/com.pbi.dvb/a.ts");
	}
	iRet = write(fp, pBuffer, Size);
	if ( 0 >= iRet )
	{
		pbiinfo("[%s %d] write error.\n",DEBUG_LOG);
		return;
	}
}

#endif
static size_t dal_download_write_func(void *buffer, size_t size, size_t nmemb, void *stream)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	unsigned int 				p_freesize;
	int							iret = 0;	
	unsigned int				data_request;



/*
	static unsigned int				dl_total=0;
	static	int						cbtimes;
	
	dl_total+=size*nmemb;
	if( cbtimes++ % 100 == 0)
	{
		//DAL_DEBUG(("CURL RECEIVE DATA %d %d\n", cbtimes, dl_total));
	}
	if( cbtimes == 1000)
	{
		DAL_DEBUG(("--------------------------------------------\n"));
		DAL_DEBUG(("--------------------------------------------\n"));
		DAL_DEBUG(("--------------------------------------------\n"));
		return -1;
	}
*/
		
	//DebugMessage("--  %d\n",size*nmemb);

	if( 0 == p_control->curl_rev_data){
		//sys_print_time(2, "data arrive");
	}
	
	/* curl 由数据接收的标志*/
	p_control->curl_rev_data = 1;

#if 1
	iret = ring_buffer_freesize( &(p_control->download_buffer), &p_freesize);
	if( iret != 0)
	{
		//DAL_ERROR(("ring_buffer_freesize failed\n"));	
		DebugMessage("ring_buffer_freesize failed\n");
		return size*nmemb;
	}

	if( p_freesize < size*nmemb)
	{
		//DAL_ERROR((" BUFFER FULL, data will to give up!!!!!!!!!!!!!\n"));
		DebugMessage(" BUFFER FULL, data will to give up!!!!!!!!!!!!!\n");
		return size*nmemb;
	}

//	mWrite(buffer,size*nmemb );
	iret = ring_buffer_write( &(p_control->download_buffer), buffer, size*nmemb);
	if( iret != 0)
	{
		//DAL_ERROR(("ring_buffer_write failed\n"));
		DebugMessage("ring_buffer_write failed\n");
		return 0;
	}
//	DebugMessage("[DAL]dal_download_write_func->ring_buffer_write 0x%x",size*nmemb);

	
#endif	

	/*提供数据给parse task*/
	if( p_control->parse_pids_flag == PPF_WAIT_READY)
	{
		if( p_control->parse_buf_size == p_control->parse_data_size)
		{
			p_control->parse_pids_flag = PPF_DATA_READY;
			//DAL_DEBUG((" PPF_DATA_READY\n"));
			DebugMessage(" PPF_DATA_READY\n");
		}
		else
		{
			data_request = p_control->parse_buf_size - p_control->parse_data_size;
			if( data_request >= size*nmemb)
			{
				memcpy(p_control->p_parse_buffer + p_control->parse_data_size, buffer, size*nmemb);
				p_control->parse_data_size += size*nmemb;
			}
			else
			{
				memcpy(p_control->p_parse_buffer + p_control->parse_data_size, buffer, data_request);
				p_control->parse_data_size += data_request;
				//DAL_DEBUG((" parse_data_size: %d   data_request: %d\n", p_control->parse_data_size, data_request));
				DebugMessage(" parse_data_size: %d   data_request: %d\n", p_control->parse_data_size, data_request);
			}
		}
	}
	
	return size*nmemb;
}

static void* curl_preform_task_sync( void* param)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pMsg = NULL;

#ifdef USE_ST_PLAYREC
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
#endif
	DAL_DEBUG(("perform %s\n", p_control->url));
	p_control->curl_error = curl_easy_perform( p_control->curl_handle);
	switch( p_control->curl_error)
	{
		case CURLE_OK:
		{
			DAL_DEBUG(("curl_easy_perform return CURLE_OK\n"));
		}
		break;
		default:
		{
			DAL_ERROR(("curl_easy_perform return %d  %s\n", p_control->curl_error, curl_easy_strerror(p_control->curl_error)));

			curl_easy_cleanup(p_control->curl_handle);
			DAL_DeleteTaskRecord(p_control->curl_taskID);
			p_control->curl_taskID = 0;
			return (void*)0;
			
		}
		break;
	}
	curl_easy_cleanup(p_control->curl_handle);
	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_CURL_ERROR;
	DAL_MessageSend( p_control->queueID, &pMsg);
	return (void*)0;	
}

static void* curl_preform_task_async( void* param)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pMsg = NULL;
	fd_set 						fdread;
	fd_set 						fdwrite;
	fd_set 						fdexcep;
	int 						maxfd;
	int							stillRunning;
	struct timeval 				timeout;
	CURLMsg*					curl_msg;
	int							nbMsg = 0;
	int							iret = 0;
	int							data_ready = 0;
	int							iFreeSize = 0;
	int							iErrorCount = 0;
#ifdef USE_ST_PLAYREC
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
#endif

	p_control->curl_multi_handle = curl_multi_init();
	curl_multi_add_handle(p_control->curl_multi_handle, p_control->curl_handle);

	DebugMessage("curl_multi_perform return curl_multi_init\n");

	//sys_print_time(1, "download start");

	data_ready = 1;
	for(;;)
	{
		if( 1 == p_control->curl_exit_flag)
		{
			goto TASK_EXIT;
		}

		if( 1 == data_ready)
		{
			for(;;)
			{
				//DAL_DEBUG(("before curl_multi_perform \n"));
				if( curl_multi_perform(p_control->curl_multi_handle, &stillRunning) == CURLM_CALL_MULTI_PERFORM)
				{
					//DAL_DEBUG(("curl_multi_perform return CURLM_CALL_MULTI_PERFORM\n"));
					DebugMessage("curl_multi_perform return CURLM_CALL_MULTI_PERFORM\n");
				}
				else
				{
					//DAL_DEBUG(("curl_multi_perform ok\n"));
					break;
				}
			}
		}
		while (( curl_msg = curl_multi_info_read(p_control->curl_multi_handle, &nbMsg)))
		{
			if ( curl_msg->easy_handle == p_control->curl_handle)
			{
				//DAL_DEBUG(("found easy_handle 0x%x\n", (unsigned int)p_control->curl_handle));
				DebugMessage("found easy_handle 0x%x\n", (unsigned int)p_control->curl_handle);
				if( curl_msg->data.result != CURLM_OK)
				{
					//DAL_DEBUG(("curl_handle result != CURLM_OK	 %s\n", curl_easy_strerror(curl_msg->data.result)));
					DebugMessage("curl_handle result != CURLM_OK	 %s\n", curl_easy_strerror(curl_msg->data.result));
					return NULL;
				}
				else if (curl_msg->msg == CURLMSG_DONE)
				{
					/* 直播数据传输不应该有结束的时候*/
					DebugMessage("curl_handle CURLMSG_DONE ???????????????\n");
					//DAL_DEBUG(("curl_handle CURLMSG_DONE ???????????????\n"));
					pMsg = DAL_MessageAlloc();
					pMsg->MsgType = DALMSG_CURL_ERROR;
					DAL_MessageSend( p_control->queueID, &pMsg);
					goto TASK_EXIT;
				}
			}
		}		

		while(ring_buffer_freesize(&(p_control->download_buffer), &iFreeSize) == 0 
			&& iFreeSize < DAL_MIN_FREESIZE
			&& iErrorCount < 25 //2s
			)
		{
			DebugMessage("download_buffer is full, delay 80 ms!!!!\n");
			DAL_TaskDelay(80);
			iErrorCount++;
		}
		iErrorCount = 0;
		
		FD_ZERO(&fdread);
		FD_ZERO(&fdwrite);
		FD_ZERO(&fdexcep);
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		iret = curl_multi_fdset( p_control->curl_multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);
		if( iret != CURLM_OK || maxfd < 0)
		{
			//DAL_DEBUG((" curl_multi_fdset - fdset Failed with %d\n", iret));
			
			DebugMessage(" curl_multi_fdset - fdset Failed with %d\n", iret);
			data_ready = 0;
			continue;
		}
		
		iret = select( maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
		if( iret == -1)
		{
			//DAL_DEBUG(("Select Failed)\n"));
			
			DebugMessage("Select Failed");
			data_ready = 0;
		}
		else if( iret == 0)
		{
			//DAL_DEBUG(("Select Timeout - %d times\n",p_control->no_data_times));
			
			DebugMessage("Select Timeout - %d times\n",p_control->no_data_times);
			data_ready = 0;

			/* 一定时间内没有数据接收，通知上层*/
			p_control->no_data_times++;
			if( p_control->no_data_times >= DAL_DATA_REV_TIMEOUT)
			{
				pMsg = DAL_MessageAlloc();
				pMsg->MsgType = DALMSG_CURL_REV_TIMEOUT;
				DAL_MessageSend( p_control->queueID, &pMsg);
				DAL_DEBUG(("Send --> DALMSG_CURL_REV_TIMEOUT\n"));
				p_control->data_timeout_flag = 1;
				p_control->no_data_times = 0;
			}
		}
		else 
		{
			//DAL_DEBUG(("Select OK  %d\n", iret));
			data_ready = 1;
			p_control->no_data_times = 0;

			/* 数据接收恢复时通知上层*/
			if( p_control->data_timeout_flag == 1)
			{
				pMsg = DAL_MessageAlloc();
				pMsg->MsgType = DALMSG_CURL_REV_RESUME;
				DAL_MessageSend( p_control->queueID, &pMsg);
				p_control->data_timeout_flag = 0;
				
			}
		}
		
		if(strstr(p_control->url,"http://127.")!=NULL)
		{
			DAL_TaskDelay(90);
		}
		else
		{
			if(ts_rate==0)
				DAL_TaskDelay(80);
			else if(ts_rate==1)
				DAL_TaskDelay(70);//测试服务器，网络环境太好，下载数据task占用所有cpu资源
			else if(ts_rate==3)
				DAL_TaskDelay(30);
			else if(ts_rate==4)
				DAL_TaskDelay(20);
		}
	}

TASK_EXIT:
	//DAL_DEBUG(("curl async perform task will stop\n"));
	DebugMessage("curl async perform task will stop\n");

	curl_multi_remove_handle(p_control->curl_multi_handle,p_control->curl_handle);
	curl_easy_cleanup(p_control->curl_handle);
	DAL_DeleteTaskRecord(p_control->curl_taskID);
	p_control->curl_taskID = 0;
	DAL_SemSignal(p_control->curl_exit_sem);
	return 0;
}
#if 0
static void* curl_check_task( void* param)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pMsg = NULL;

	while(1)
	{
		DAL_SemWait(p_control->check_sem);
		if( p_control->curl_rev_data == 1)
		{
			if( p_control->data_timeout_flag == 1)
			{
				pMsg = DAL_MessageAlloc();
				pMsg->MsgType = DALMSG_CURL_REV_RESUME;
				DAL_MessageSend( p_control->queueID, &pMsg);
				p_control->data_timeout_flag = 0;
			}
			p_control->no_data_times = 0;
		}
		else
		{
			p_control->no_data_times++;
			//DAL_DEBUG(("CURL NOT RECEIVE DATA %d SECONDS\n", p_control->no_data_times));
			DebugMessage("CURL NOT RECEIVE DATA %d SECONDS\n", p_control->no_data_times);
		}
		p_control->curl_rev_data = 0;

		if( p_control->no_data_times >= DAL_DATA_REV_TIMEOUT && p_control->data_timeout_flag == 0)
		{
			pMsg = DAL_MessageAlloc();
			pMsg->MsgType = DALMSG_CURL_REV_TIMEOUT;
			DAL_MessageSend( p_control->queueID, &pMsg);
			//DAL_DEBUG(("Send --> DALMSG_CURL_REV_TIMEOUT\n"));
			DebugMessage("Send --> DALMSG_CURL_REV_TIMEOUT\n");
			p_control->data_timeout_flag = 1;
			p_control->no_data_times = 0;
		}
		DAL_SemSignal(p_control->check_sem);
		DAL_TaskDelay(1000);
	}
	return 0;
}

#endif

#if defined(USE_DAL_MULTICAST)
//{{{ add by LYN , for multicast , 2011-08-22
int DAL_MulticastStart( dal_download_param_t* param)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pMsg = NULL;

	DebugMessage("entry\n");

	if( p_control->status == DOWNLOAD_IDEL)
	{
		DebugMessage("DOWNLOAD_IDEL");
		return -1;
	}

	/* 清空可能堆积的消息*/
	download_queue_clear();	

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_MULTICAST_START;
	memset( pMsg->MsgData.DownloadStart.url, 0, sizeof(pMsg->MsgData.DownloadStart.url));
	strcpy( pMsg->MsgData.DownloadStart.url, param->url);
	DAL_MessageSend( p_control->queueID, &pMsg);
	return 0;
}

int DAL_MulticastStop( void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pMsg = NULL;

	if( p_control->status == DOWNLOAD_IDEL)
	{
		DebugMessage("DOWNLOAD_IDEL");
		return -1;
	}	

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_MULTICAST_STOP;
	DAL_MessageSend( p_control->queueID, &pMsg);

	while( DAL_DownloadIsStop() != TRUE)
	{
		DebugMessage("waiting for DAL_MulticastStop\n");
		DAL_TaskDelay(200);
	}	
	return 0;
}

static void Multicast_start_process(DAL_Message_t* pMsg)
{
    DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				p_msg = NULL;
	int							iret = 0;

    // stop the exist connection 
    if( p_control->status == DOWNLOAD_START)
	{
	    
		if( DAL_DL_HTTP == p_control->type)
		{
			DebugMessage("ALREADY DOWNLOAD_START , STOP  CURL\n");
			//curl_check_task_pause(p_control);
			iret = curl_preform_task_delete( p_control);
			if( iret != 0)
			{
				DebugMessage("curl_preform_task_delete failed\n");
				return ;
			}
		}
		else if(DAL_DL_UDP == p_control->type)
		{
		    DebugMessage("ALREADY DOWNLOAD_START , STOP UDP \n");

		    iret = Multicast_perform_task_delete(p_control);
		    if( iret != 0)
		    {
		        DebugMessage("multicast_perform_task_delete failed\n");
		        return;
		    }
		}
		else
		{
			DebugMessage("error operation!\n");
			return ;
		}
	}

    //set DL type
    DebugMessage("%s !\n",pMsg->MsgData.DownloadStart.url);
	if(( 0 == strncmp( pMsg->MsgData.DownloadStart.url, "udp:", strlen("udp:")))
	    ||(0 == strncmp(pMsg->MsgData.DownloadStart.url,"UDP:", strlen("UDP:"))))
	{
		p_control->type = DAL_DL_UDP;
	}
	else
	{
	   DebugMessage("error url params!\n");
	    return;
	}

	// start new download
	if( ring_buffer_is_init( &(p_control->download_buffer)) == 0)
	{
		DebugMessage("DOWNLOAD BUFFER NOT INIT\n");
		return ;
	}
	ring_buffer_flush(&(p_control->download_buffer));
	
	memset( p_control->url, 0, sizeof(p_control->url));
	strcpy( p_control->url, pMsg->MsgData.DownloadStart.url);

	/*每次重新开始 下载, 重新设置此标志*/
	p_control->parse_pids_flag = PPF_NO_REQUEST;


    /*set global var for udp download*/
    p_control->multicast_rev_data= 0;
	p_control->multicast_timeout_flag= 0;
	p_control->multicast_exit_flag = 0;
	p_control->multicast_no_data_times = 0;
    p_control->multicast_socket_exist = 0;


    /*create the udp download task*/
    iret = Multicast_perform_task_create( p_control);
	if( iret != 0)
	{
		DebugMessage("Multicast_perform_task_create failed with %d\n", iret);
		p_control->status = DOWNLOAD_STOP;
		return ;
	}

    p_control->status = DOWNLOAD_START;
    return;
}


static void Multicast_stop_process(DAL_Message_t* pMsg)
{
    DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	int							iret = 0;

	if( DOWNLOAD_IDEL == p_control->status)
	{
		DebugMessage("DOWNLOAD_IDEL\n");
		return ;
	}

	if( p_control->status != DOWNLOAD_START)
	{
		DebugMessage("NOT DOWNLOAD_ING\n");
		return ;
	}

	if( DAL_DL_UDP == p_control->type)
	{
		DebugMessage("before Multicast_perform_task_delete, waiting...\n");
		iret = Multicast_perform_task_delete( p_control);
		if( iret != 0)
		{
			DebugMessage("Multicast_perform_task_delete failed\n");
			return ;
		}
		DebugMessage("Multicast_perform_task_delete ok\n");
	}
	
	p_control->status = DOWNLOAD_STOP;
    return;
}

static void Multicast_error_process(DAL_Message_t* pMsg)
{
    DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pmsg = NULL;
	int							iret = 0;

	iret = Multicast_perform_task_delete( p_control);
	if( iret != 0)
	{
		DebugMessage("Multicast_perform_task_delete failed\n");
		return ;
	}
	DebugMessage("Multicast_perform_task_delete success\n");
	p_control->status = DOWNLOAD_STOP;

	/*自动重新开始下载*/
	pmsg = DAL_MessageAlloc();
	pmsg->MsgType = DALMSG_DOWNLOAD_START;
	memset(pmsg->MsgData.DownloadStart.url, 0, sizeof(MsgData_DownloadStart_t));
	strcpy(pmsg->MsgData.DownloadStart.url, p_control->url);
	DAL_MessageSend( p_control->queueID, &pmsg);
}

static void Multicast_recv_timeout_process(DAL_Message_t* pMsg)
{
    DAL_Message_t*				pmsg = NULL;
	
	pmsg = DAL_MessageAlloc();
	pmsg->MsgType = DALMSG_DATA_TIMEOUT;
	DAL_MessageSend( DAL_GetCoreQueueID(), &pmsg);
}

static void Multicast_recv_resume_process(DAL_Message_t* pMsg)
{
    DAL_Message_t*				pmsg = NULL;
	
	pmsg = DAL_MessageAlloc();
	pmsg->MsgType = DALMSG_DATA_RESUME;
	DAL_MessageSend( DAL_GetCoreQueueID(), &pmsg);
    return;
}

static int Multicast_perform_task_create(DAL_DownloadControl_t* p_control)
{
    int						iret = 0;

	iret = DAL_SemWait_Timeout( p_control->multicast_exit_sem, 2000);
	if( iret != 0)
	{
		DebugMessage("DAL_SemWait_Timeout failed\n");
		return iret;
	}
	
	iret = DAL_TaskCreate( &(p_control->multicast_taskID), "curl preform task", multicast_perform_task_async, NULL, NULL, DAL_CURL_TASK_STACK_SIZE*2, DAL_CURL_TASK_PRIOITY,0);
	if( iret != 0)
	{
		DebugMessage("Multicast_perform_task_create  DAL_TaskCreate failed\n");
		return iret;
	}
	return 0;
}
static int Multicast_perform_task_delete(DAL_DownloadControl_t* p_control)
{
    int 						iret = 0;
	
	p_control->multicast_exit_flag = 1;
	DebugMessage("^^multicast set flag to stop perform task\n");

	iret = DAL_SemWait_Timeout(p_control->multicast_exit_sem, 5000);
	if( iret != 0)
	{
		DebugMessage("^^ multicast DAL_SemWait_Timeout failed\n");
		DebugMessage("^^ multicast perform task delete failed\n");
	}
	else
	{
		DAL_SemSignal(p_control->multicast_exit_sem);
		DebugMessage("^^multicast perform task delete success\n");
		return 0;
	}
	return -1;
}
static struct ip_mreq mulit_req;
S32 GET_Multicast(S8*siServerIP ,S32 *siSocket)
{
	S32 fd, iret;
	struct sockaddr_in Server;
	
	S8 *siStart = NULL;
	S8 *sitmp = NULL;
	U8 Port[6] = {0};
	U8 Addr[20] = {0};
	U8 cIp_Addr[16] = {0};/*IP address*/
	U8 cNet_Mask[16] = {0};
	U8 cGateway[16] = {0};
	//U32 buffer_len = 512*1024;
	U32 buffer_len = 0;

	memset(&Server, 0, sizeof(Server));
	memset(&Port, 0, sizeof(Port));
	memset(&Addr, 0, sizeof(Addr));
	memset( cIp_Addr,0,sizeof(cIp_Addr) );
	memset( cNet_Mask,0,sizeof(cNet_Mask) );
	memset( cGateway,0,sizeof(cGateway) );
//	Network_Get_Current_Ip( cIp_Addr, cNet_Mask, cGateway );
	get_ip_addr(cIp_Addr);



	sitmp = strstr(siServerIP,":"); 
	if(strncpy(Addr,siServerIP,(sitmp-siServerIP))==NULL)
		return (-1);
	DebugMessage("[Multicast] len: %d\n",sitmp-siServerIP);

	sitmp+=1;
	if(strcpy(Port,sitmp) ==NULL)
		return (-1);

	DebugMessage("[Multicast] server:%s -ip:%s-port: %d\n",siServerIP,Addr,atoi(Port));

	Server.sin_family = AF_INET;
	Server.sin_port = htons(atoi(Port));
	inet_aton(Addr, &Server.sin_addr );

	if( (fd = socket( AF_INET, SOCK_DGRAM, 0) ) < 0 )
	{
		DebugMessage("[Multicast] socket error!!!\n");
		return (-1);
	}

	iret=bind(fd, (struct sockaddr *)&Server, sizeof(Server));

	if( iret < 0 )
	{
		DebugMessage("[Multicast] bind error %d!!!\n",iret);
		return (-1);
	}

	memset(&mulit_req,0,sizeof(mulit_req));
	mulit_req.imr_multiaddr.s_addr=inet_addr(Addr);
	mulit_req.imr_interface.s_addr=inet_addr(cIp_Addr);
	if( setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mulit_req,sizeof(mulit_req)) < 0 )
	{
		iret = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mulit_req,sizeof(mulit_req));
		DebugMessage(" iret = %d,Caddr = %s,[Multicast] setsockopt error !!!\n",iret,cIp_Addr);
		return (-1);
	}
	
	DebugMessage(" Caddr = %s,[Multicast] setsockopt OK !!!\n",cIp_Addr);
	if(siSocket == NULL)
	{
		close(fd);
		return (-1);
	}
	else
	{
		*siSocket= fd;
		DebugMessage("--------siSocket=%d------------\n", siSocket);
	}
	// printf("[Multicast] socked:%d\n",fd);

	return 0;
}

#define BUFFER_LEN   5000*3 
static void* multicast_perform_task_async( void* param)
{
    DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pMsg = NULL;
    S8 *sitmp = NULL;
    S8					tempUrl[36];
    S32						iret = 0,ret=0;
    fd_set	selfd;
    struct timeval tv; 
	struct sockaddr_in Server;
	U32 addrlen = 0;
  //  U8 temBuffer[BUFFER_LEN];
    unsigned int 				p_freesize;
    U32 				data_request;
    U8 SocFlag = 0;//add by LYN,0902

    U8* temBuffer = malloc(BUFFER_LEN);
  //  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
//	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    
    memset(tempUrl,0,36);

    strcpy(tempUrl,p_control->url);
    sitmp = strstr(tempUrl,"://");
    sitmp += 3;
    iret = GET_Multicast(sitmp,&(p_control->multicast_socket_fd));
    if(iret != 0)
    {
        pMsg = DAL_MessageAlloc();
		pMsg->MsgType = DALMSG_MULTICAST_ERROR;
		DAL_MessageSend( p_control->queueID, &pMsg);
		DebugMessage("[Multicast] Send --> DALMSG_MULTICAST_ERROR\n");
	//	goto TASK_EXIT;
	SocFlag = 1;
    }
    p_control->multicast_socket_exist = 1;
    p_control->multicast_buffer_flag = 0;
    while(1)
    {
    	//DAL_TaskDelay(10);/*udp 无法控制码率*/
       if( 1 == p_control->multicast_exit_flag)
		{
			goto TASK_EXIT;
		}

	if(1 == SocFlag)
		continue;
#if 0
       if(1 == p_control->multicast_buffer_flag)
       {
            iret = ring_buffer_freesize( &(p_control->download_buffer), &p_freesize);
            if( iret != 0)
            {
            	DAL_ERROR(("ring_buffer_freesize failed\n"));
            	pMsg = DAL_MessageAlloc();
        		pMsg->MsgType = DALMSG_MULTICAST_ERROR;
        		DAL_MessageSend( p_control->queueID, &pMsg);
        		goto TASK_EXIT;
            }
            if(p_freesize >= BUFFER_LEN)
            {
                iret = ring_buffer_write( &(p_control->download_buffer), temBuffer,BUFFER_LEN);
                p_control->multicast_buffer_flag = 0;
            	if( iret != 0)
            	{
            		DAL_ERROR(("ring_buffer_write failed\n"));
            	}
            }
            else
           {
               DAL_TaskDelay(10);
               continue;
           }
       }
#endif     
            memset( &Server, 0, sizeof( Server ));
			addrlen = sizeof(Server);
			FD_ZERO(&selfd); 
			FD_SET(p_control->multicast_socket_fd, &selfd); 
			tv.tv_sec = 1; 
			tv.tv_usec = 0;  //5ms

			ret = select(p_control->multicast_socket_fd + 1, &selfd, NULL, NULL, &tv);
			if(ret == 0)
			{
			    
				/* 一定时间内没有数据接收，通知上层*/
    			p_control->multicast_no_data_times++;
    			if( p_control->multicast_no_data_times >= DAL_DATA_REV_TIMEOUT)
    			{
    				pMsg = DAL_MessageAlloc();
    				pMsg->MsgType = DALMSG_CURL_REV_TIMEOUT;
    				DAL_MessageSend( p_control->queueID, &pMsg);
    				DebugMessage("[Multicast] Send --> DALMSG_CURL_REV_TIMEOUT\n");
    				p_control->multicast_timeout_flag = 1;
    				p_control->multicast_no_data_times = 0;
    			}
			}
			else if(ret == -1)
			{
				DebugMessage("[Multicast] ReceiveTsData select error\n");
				ret = -1;
			}
			else if(FD_ISSET(p_control->multicast_socket_fd, &selfd))
			{
			    p_control->multicast_rev_data = 1;
			    p_control->multicast_no_data_times = 0;
                	/* 数据接收恢复时通知上层*/
    			if( p_control->multicast_timeout_flag == 1)
    			{
    				pMsg = DAL_MessageAlloc();
    				pMsg->MsgType = DALMSG_CURL_REV_RESUME;
					DebugMessage("[Multicast] Send --> DALMSG_CURL_REV_RESUME\n");
    				DAL_MessageSend( p_control->queueID, &pMsg);
    				p_control->multicast_timeout_flag = 0;
  
    			}
    			addrlen = 0;
				ret = recvfrom(p_control->multicast_socket_fd, temBuffer, BUFFER_LEN, 0,(struct sockaddr *)&Server,&addrlen);
				if( ret  > 0 )
				{
				    p_control->multicast_buffer_flag = 1;
			        iret = ring_buffer_freesize( &(p_control->download_buffer), &p_freesize);
                    if( iret != 0)
                    {
                    	DebugMessage("[Multicast] ring_buffer_freesize failed\n");
                    	pMsg = DAL_MessageAlloc();
                		pMsg->MsgType = DALMSG_MULTICAST_ERROR;
                		DAL_MessageSend( p_control->queueID, &pMsg);
                		goto TASK_EXIT;
                    }
 
                    if(p_freesize < ret)
                    {
                        DebugMessage(" [Multicast] BUFFER FULL, data will to give up!!!!!!!!!!!!!\n");
                      //  DAL_TaskDelay(500);
                       
                    }
                    else
                    {
                        iret = ring_buffer_write( &(p_control->download_buffer), temBuffer,ret);
						// DebugMessage(" ########################\n");
                    	if( iret != 0)
                    	{
                    		DebugMessage("[Multicast] ring_buffer_write failed\n");
                    	} 
						p_control->multicast_buffer_flag = 0;
                    }
                    /*提供数据给parse task*/
                	if( p_control->parse_pids_flag == PPF_WAIT_READY)
                	{
                		if( p_control->parse_buf_size == p_control->parse_data_size)
                		{
                			p_control->parse_pids_flag = PPF_DATA_READY;
                			DebugMessage(" [Multicast] PPF_DATA_READY ************************\n");
                		}
                		else
                		{
                			data_request = p_control->parse_buf_size - p_control->parse_data_size;
                			if( data_request >= ret)
                			{
                				memcpy(p_control->p_parse_buffer + p_control->parse_data_size, temBuffer, ret);
                				p_control->parse_data_size += ret;
                			}
                			else
                			{
                				memcpy(p_control->p_parse_buffer + p_control->parse_data_size, temBuffer, data_request);
                				p_control->parse_data_size += data_request;
                		//		DebugMessage("[Multicast] parse_data_size: %x   data_request: %x\n", p_control->parse_data_size, data_request);
                			}
                			
                		}
	                }
				}
				
			}
    }


TASK_EXIT:
	DebugMessage("[Multicast] multicast async perform task will stop\n");

    if(1 == p_control->multicast_socket_exist)
    {
    	setsockopt(p_control->multicast_socket_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mulit_req,sizeof(mulit_req));
	    close(p_control->multicast_socket_fd);
        p_control->multicast_socket_exist = 0;
	}
	DAL_DeleteTaskRecord(p_control->multicast_taskID);
	p_control->multicast_taskID = 0;

//	SYS_DVBSubt_Stop();
//	Set_SubFlag(0);
	DAL_SemSignal(p_control->multicast_exit_sem);

	free(temBuffer);
	return 0;
	
}
//}}} add by LYN , for multicast , 2011-08-22
#endif
void init_ring_buffer(void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;

	if( ring_buffer_is_init( &(p_control->download_buffer)) == 0)
	{
		DebugMessage("[init_ring_buffer]DOWNLOAD BUFFER NOT INIT\n");
		return ;
	}
	ring_buffer_flush(&(p_control->download_buffer));
}

static size_t dal_m3u8_download_write_func(void *buffer, size_t size, size_t nmemb, void *stream)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	unsigned int 				p_freesize;
	int							iret = 0;	
	unsigned int				data_request;


	
	//DebugMessage("[dal_m3u8_download_write_func]enter nmemb*size=%d",size*nmemb);
	/* curl 由数据接收的标志*/
	p_control->curl_rev_data = 1;

	iret = ring_buffer_freesize( &(p_control->download_buffer), &p_freesize);
	if( iret != 0)
	{
		DebugMessage("ring_buffer_freesize failed\n");
		return size*nmemb;
	}

	if( p_freesize < size*nmemb)
	{
		DebugMessage(" BUFFER FULL, data will to give up!!!!!!!!!!!!!\n");
		return size*nmemb;
	}

	iret = ring_buffer_write( &(p_control->download_buffer), buffer, size*nmemb);
	if( iret != 0)
	{
		DebugMessage("ring_buffer_write failed\n");
		return 0;
	}

	
	DebugMessage("[dal_m3u8_download_write_func] ring_buffer_write:%d",size*nmemb);
	/*提供数据给parse task*/
	if( p_control->parse_pids_flag == PPF_WAIT_READY)
	{
		if( p_control->parse_buf_size == p_control->parse_data_size)
		{
			p_control->parse_pids_flag = PPF_DATA_READY;
			DebugMessage(" PPF_DATA_READY\n");
		}
		else
		{
			data_request = p_control->parse_buf_size - p_control->parse_data_size;
			if( data_request >= size*nmemb)
			{
				memcpy(p_control->p_parse_buffer + p_control->parse_data_size, buffer, size*nmemb);
				p_control->parse_data_size += size*nmemb;
			}
			else
			{
				memcpy(p_control->p_parse_buffer + p_control->parse_data_size, buffer, data_request);
				p_control->parse_data_size += data_request;
				//DAL_DEBUG((" parse_data_size: %d   data_request: %d\n", p_control->parse_data_size, data_request));
				DebugMessage(" parse_data_size: %d   data_request: %d\n", p_control->parse_data_size, data_request);
			}
		}
	}
	
	return size*nmemb;
}

static void M3u8_start_process( DAL_Message_t* pMsg)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				p_msg = NULL;
	int							iret = 0;

	if( p_control->status == DOWNLOAD_IDEL)
	{
		DebugMessage("[M3u8_start_process]DOWNLOAD_IDEL\n");
		return ;
	}


	memset( p_control->url, 0, sizeof(p_control->url));
	strcpy( p_control->url, pMsg->MsgData.DownloadStart.url);
	DebugMessage("DOWNLOAD start url:%s\n",pMsg->MsgData.DownloadStart.url);


	
	//nlv p_control->parse_pids_flag = PPF_NO_REQUEST;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	
	DebugMessage("M3u8 DOWNLOAD libcurl start");
	p_control->m3u8_curl_rev_data = 0;
	p_control->m3u8_data_timeout_flag = 0;
	p_control->m3u8_curl_handle = curl_easy_init();
	p_control->m3u8_curl_exit_flag = 0;
	p_control->m3u8_no_data_times = 0;

	curl_easy_setopt( p_control->m3u8_curl_handle, CURLOPT_URL, p_control->url); 
	curl_easy_setopt( p_control->m3u8_curl_handle, CURLOPT_WRITEFUNCTION, dal_m3u8_download_write_func);
	iret = m3u8_curl_preform_task_create( p_control);
	if( iret != 0)
	{
		DebugMessage("curl_preform_task_create failed with %d\n", iret);
		curl_easy_cleanup( p_control->m3u8_curl_handle);
		p_control->status = DOWNLOAD_STOP;
		return ;
	}	
	p_control->status = DOWNLOAD_START;
}

static int m3u8_curl_preform_task_create( DAL_DownloadControl_t* p_control)
{
	int						iret = 0;

	iret = DAL_SemWait_Timeout( p_control->m3u8_curl_exit_sem, 2000);
	if( iret != 0)
	{
		DebugMessage("DAL_SemWait_Timeout failed\n");
		return iret;
	}
	
	iret = DAL_TaskCreate( &(p_control->m3u8_curl_taskID), "m3u8 curl preform task", m3u8_curl_preform_task_async, NULL, NULL, DAL_CURL_TASK_STACK_SIZE, DAL_CURL_TASK_PRIOITY,0);
	if( iret != 0)
	{
		return iret;
	}
	return 0;
}
static void* m3u8_curl_preform_task_async( void* param)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pMsg = NULL;
	fd_set 						fdread;
	fd_set 						fdwrite;
	fd_set 						fdexcep;
	int 						maxfd;
	int							stillRunning;
	struct timeval 				timeout;
	CURLMsg*					curl_msg;
	int							nbMsg = 0;
	int							iret = 0;
	int							data_ready = 0;
	long 						T=0;
#ifdef USE_ST_PLAYREC
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
#endif

	p_control->m3u8_curl_multi_handle = curl_multi_init();
	curl_multi_add_handle(p_control->m3u8_curl_multi_handle, p_control->m3u8_curl_handle);

	data_ready = 1;
	for(;;)
	{
		if( 1 == p_control->m3u8_curl_exit_flag)
		{
			goto TASK_EXIT;
		}

		if( 1 == data_ready)
		{
			for(;;)
			{
				if( curl_multi_perform(p_control->m3u8_curl_multi_handle, &stillRunning) == CURLM_CALL_MULTI_PERFORM)
				{
					DebugMessage("m3u8_curl_multi_perform return CURLM_CALL_MULTI_PERFORM\n");
				}
				else
				{
					break;
				}
			}
		}
		while (( curl_msg = curl_multi_info_read(p_control->m3u8_curl_multi_handle, &nbMsg)))
		{
			if ( curl_msg->easy_handle == p_control->m3u8_curl_handle)
			{
				DebugMessage("found easy_handle 0x%x\n", (unsigned int)p_control->m3u8_curl_handle);
				if( curl_msg->data.result != CURLM_OK)
				{
					DebugMessage("curl_handle result != CURLM_OK	 %s\n", curl_easy_strerror(curl_msg->data.result));
					//return NULL;
					
					goto TASK_EXIT;
				}
				else if (curl_msg->msg == CURLMSG_DONE)
				{
				
					DebugMessage("curl_handle CURLMSG_DONE ???????????????\n");
					/* 直播数据传输不应该有结束的时候*/
					goto TASK_EXIT;
				}
			}
		}		

		FD_ZERO(&fdread);
		FD_ZERO(&fdwrite);
		FD_ZERO(&fdexcep);
		
		timeout.tv_sec = 1;
		timeout.tv_usec = 500;

		iret = curl_multi_fdset( p_control->m3u8_curl_multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);
		if( iret != CURLM_OK || maxfd < 0)
		{			
			DebugMessage(" curl_multi_fdset - fdset Failed with %d\n", iret);
			data_ready = 0;
			continue; //nlv test maxfd<0后不能恢复播放
		}
	/*	iret =curl_multi_timeout( p_control->m3u8_curl_multi_handle,&T);
		if( T==-1 ||T>5000)
		{
			DebugMessage(" curl_multi_fdset - timeout %d\n", T);
			T=2000;

			
			if(maxfd < 0)
			{
			
				DebugMessage(" curl_multi_fdset - timeout %d\n", T);
				DAL_TaskDelay(T);
			continue;
		}
		
		}*/
		
		iret = select( maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
		if( iret == -1)
		{
			DebugMessage("Select Failed");
			data_ready = 0;
		}
		else if( iret == 0)
		{
			DebugMessage("Select Timeout - %d times\n",p_control->m3u8_no_data_times);
			data_ready = 0;

			/* 一定时间内没有数据接收，通知上层*/
			p_control->m3u8_no_data_times++;
			if( p_control->m3u8_no_data_times == 6)
			{
				pMsg = DAL_MessageAlloc();
				pMsg->MsgType = DALMSG_CURL_REV_TIMEOUT;
				DAL_MessageSend( p_control->queueID, &pMsg);

			}
			if( p_control->m3u8_no_data_times >= DAL_DATA_REV_TIMEOUT)
			{				
				p_control->m3u8_data_timeout_flag = 1;
				p_control->m3u8_no_data_times = 0;
				goto TASK_EXIT;
			}
		}
		else 
		{
			data_ready = 1;
			p_control->m3u8_no_data_times = 0;

			/* 数据接收恢复时通知上层*/
			if( p_control->m3u8_data_timeout_flag == 1)
			{
				pMsg = DAL_MessageAlloc();
				pMsg->MsgType = DALMSG_CURL_REV_RESUME;
				DAL_MessageSend( p_control->queueID, &pMsg);
				p_control->m3u8_data_timeout_flag = 0;
				
			}
		}
		if(get_live_state()==1||get_catch_state()==1)
		{
			if(Get_hls_resume_state()==0)
				DAL_TaskDelay(240);//测试服务器，网络环境太好，下载数据task占用所有cpu资源
			else
				DAL_TaskDelay(80);
		}
		else	
			DAL_TaskDelay(20);
	}

TASK_EXIT:
	DebugMessage("curl async perform task will stop\n");

	curl_multi_remove_handle(p_control->m3u8_curl_multi_handle,p_control->m3u8_curl_handle);
	curl_easy_cleanup(p_control->m3u8_curl_handle);
	curl_multi_cleanup(p_control->m3u8_curl_multi_handle);
	
	curl_global_cleanup();
	
	DAL_DeleteTaskRecord(p_control->m3u8_curl_taskID);
	p_control->m3u8_curl_taskID = 0;
	
	DAL_SemSignal(p_control->m3u8_curl_exit_sem);
/*拿下一段数据m3u8*/	
	DebugMessage("m3u8 ts download message request next part data\n");
	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_M3U8_TSDOWNLOAD_START;
	DAL_MessageSend(DAL_GetM3u8QueueID(), &pMsg);
	return 0;
}
static int m3u8_curl_preform_task_delete( DAL_DownloadControl_t* p_control)
{
	int 						iret = 0;
	
	p_control->m3u8_curl_exit_flag = 1;
	DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ set flag to stop perform task\n");

	iret = DAL_SemWait_Timeout(p_control->m3u8_curl_exit_sem, 5000);
	if( iret != 0)
	{
		DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DAL_SemWait_Timeout failed\n");
		DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ perform task delete failed\n");
	}
	else
	{
		DAL_SemSignal(p_control->m3u8_curl_exit_sem);
		DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ perform task delete success\n");
		return 0;
	}
	return -1;
}

static void M3u8_stop_process( DAL_Message_t* pMsg)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	int							iret = 0;

	if(( p_control->status == DOWNLOAD_IDEL)||( p_control->status != DOWNLOAD_START))
	{
		DebugMessage("DOWNLOAD_IDEL\n");
		return ;
	}

	iret = m3u8_curl_preform_task_delete( p_control);
	if( iret != 0)
	{
		DebugMessage("before m3u8_curl_preform_task_delete, waiting...\n");
		return ;
	}
	DebugMessage("before m3u8_curl_preform_task_delete, waiting...\n");
	p_control->status = DOWNLOAD_STOP;
}


int DAL_DownloadTest( void)
{
	dal_download_param_t	param;
	int						iret = 0;

	DAL_DEBUG(("ENTRY\n"));
	
	iret = DAL_DownloadTaskCreate();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_DownloadTaskCreate failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_DownloadTaskCreate success\n"));


	iret = DAL_DownloadBufferInit();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_DownloadBufferInit failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_DownloadBufferInit success\n"));
	
	memset( param.url, 0, sizeof(param.url));
	strcpy( param.url, "http://192.168.2.125/ss.mp4");
	iret = DAL_DownloadStart(&param);
	if( iret != 0)
	{
		DAL_ERROR(("DAL_DownloadStart failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_DownloadStart success\n"));

	DAL_TaskDelay(1000*10);

	iret = DAL_DownloadStop();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_DownloadStop failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_DownloadStop success\n"));


	iret = DAL_DownloadBufferRelease();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_DownloadBufferRelease failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_DownloadBufferRelease success\n"));	


	iret = DAL_DownloadTaskDelete();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_DownloadTaskDelete failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_DownloadTaskDelete success\n"));

	DAL_DEBUG(("DAL_DownloadTest success\n"));
	return 0;
}
 
int DAL_Download_Parse_Test( void)
{
	dal_download_param_t	param;
	int						iret = 0;

	DAL_DEBUG(("ENTRY\n"));

	iret = DAL_MessageInit();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_MessageInit failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_MessageInit success\n"));
	
	iret = DAL_DownloadTaskCreate();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_DownloadTaskCreate failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_DownloadTaskCreate success\n"));
	
	iret = DAL_DownloadBufferInit();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_DownloadBufferInit failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_DownloadBufferInit success\n"));
	iret = DAL_ParseTaskCreate();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_ParseTaskCreate failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_ParseTaskCreate success\n"));



	memset( param.url, 0, sizeof(param.url));
	strcpy( param.url, "http://192.168.2.125/p2s.ts");
	iret = DAL_DownloadStart(&param);
	if( iret != 0)
	{
		DAL_ERROR(("DAL_DownloadStart failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_DownloadStart success\n"));

	iret = DAL_ParseStart();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_ParseStart failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_ParseStart success\n"));


	while(1)
	{
		DAL_TaskDelay(1000);
	}
}



int DAL_Download_Inject_Test( void)
{
	dal_download_param_t	param;
	int						iret = 0;

	DAL_DEBUG(("ENTRY\n"));

	iret = DAL_MessageInit();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_MessageInit failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_MessageInit success\n"));
	
	iret = DAL_DownloadTaskCreate();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_DownloadTaskCreate failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_DownloadTaskCreate success\n"));
	
	iret = DAL_DownloadBufferInit();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_DownloadBufferInit failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_DownloadBufferInit success\n"));

	iret = DAL_InjectTaskCreate();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_InjectTaskCreate failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_InjectTaskCreate success\n"));



	memset( param.url, 0, sizeof(param.url));
	strcpy( param.url, "http://192.168.2.125/p2s.ts");
	iret = DAL_DownloadStart(&param);
	if( iret != 0)
	{
		DAL_ERROR(("DAL_DownloadStart failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_DownloadStart success\n"));

	iret = DAL_InjectStart();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_InjectStart failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_InjectStart success\n"));

	while(1)
	{
		DAL_TaskDelay(1000);
	}

}


int MessageQueueTest( void)
{
	int					iret = 0;
	U32					queue_id;
	DAL_Message_t		Msg;
	DAL_Message_t*		pMsg = &Msg;
	DAL_Message_t*		premsg;
	U32					size_copied;
	
	iret = OS_MessageQueueCreate( &queue_id, "321", 50, sizeof(DAL_Message_t*), 0);
	if( iret != 0)
	{
		DAL_ERROR(("OS_MessageQueueCreate failed\n"));
		return -1;
	}
	DAL_DEBUG(("OS_MessageQueueCreate ok\n"));

	
	iret = OS_MessageQueueSend( queue_id, &pMsg, sizeof(DAL_Message_t*), -1, 0);
	if( iret != 0)
	{
		DAL_ERROR(("OS_MessageQueueSend failed\n"));
	}
	DAL_DEBUG(("OS_MessageQueueSend ok\n"));

	iret = OS_MessageQueueSend( queue_id, &pMsg, sizeof(DAL_Message_t*), -1, 0);
	if( iret != 0)
	{
		DAL_ERROR(("OS_MessageQueueSend failed\n"));
	}
	DAL_DEBUG(("OS_MessageQueueSend ok\n"));

	iret = OS_MessageQueueSend( queue_id, &pMsg, sizeof(DAL_Message_t*), -1, 0);
	if( iret != 0)
	{
		DAL_ERROR(("OS_MessageQueueSend failed\n"));
	}
	DAL_DEBUG(("OS_MessageQueueSend ok\n"));

	iret = OS_MessageQueueSend( queue_id, &pMsg, sizeof(DAL_Message_t*), -1, 0);
	if( iret != 0)
	{
		DAL_ERROR(("OS_MessageQueueSend failed\n"));
	}
	DAL_DEBUG(("OS_MessageQueueSend ok\n"));


	while(1)
	{
		iret = OS_MessageQueueReceive( queue_id, &premsg, sizeof(DAL_Message_t*), &size_copied, -1);
		if( iret >= 0)
		{
			DAL_DEBUG(("OS_MessageQueueReceive ok %d\n", iret));
		}
		else
		{
			DAL_DEBUG(("OS_MessageQueueReceive no msg\n"));
		}
		DAL_TaskDelay(1000*10);
	}
	return 0;
}


 
int get_ip_addr(U8*ipaddr)
{
	int fd, intrface, retn = 0; 
	char *ip=NULL;/*IP address*/
	struct ifreq buf[MAXINTERFACES]; 
	struct arpreq arp; 
	struct ifconf ifc; 
	
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) 
	{ 
		  ifc.ifc_len = sizeof buf; 
		  ifc.ifc_buf = (caddr_t) buf; 
		  if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) 
		  { 
			    //获取接口信息
			    intrface = ifc.ifc_len / sizeof (struct ifreq); 
			 	DebugMessage("interface num is intrface=%d\n\n\n",intrface); 
	
	   			//根据借口信息循环获取设备IP和MAC地址
	
	  			   while (intrface-- > 0) 
	  			   { 
	  				//获取设备名称
		  				DebugMessage("net device %s\n", buf[intrface].ifr_name); 
							
						if(strncmp(buf[intrface].ifr_name,"eth0",4))
							continue;
		  				//判断网卡类型 
		  				if (!(ioctl (fd, SIOCGIFFLAGS, (char *) &buf[intrface]))) 
		  				{ 
			  				 if (buf[intrface].ifr_flags & IFF_PROMISC) 
			  				 { 
			  				 	 DebugMessage("the interface is PROMISC"); 
			  				 	 retn++; 
			  				 } 
		  				} 
		  				else 
		  				{ 
			  				 char str[256]; 
			  				 sprintf (str, "cpm: ioctl device %s", buf[intrface].ifr_name); 
			  				 DebugMessage("%s",str); 
		  			
		  				} 
		  			
			  			 //判断网卡状态 
		  				if (buf[intrface].ifr_flags & IFF_UP) 
		  			   	{ 
	  						DebugMessage("the interface status is UP"); 
	  					} 
	  					else 
	  			   		{ 
	  						DebugMessage("the interface status is DOWN"); 
	  					} 
	  			
	  			  		 //获取当前网卡的IP地址 
  						if (!(ioctl(fd, SIOCGIFADDR, (char *) &buf[intrface]))) 
  						{ 
  							 memcpy(ipaddr,inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr),15);
							// memcpy(ipaddr,ip,16);
  							 DebugMessage("IP address is:%s",ipaddr); 
  						}  			
  						else 			
	  			   		{ 
	  						   char str[256]; 
	  						   sprintf(str, "cpm: ioctl device %s", buf[intrface].ifr_name); 
	  						   DebugMessage("%s",str); 
	  					 } 
	  			
	  			/* this section can't get Hardware Address,I don't know whether the reason is module driver*/ 

	  					if (!(ioctl(fd, SIOCGIFHWADDR, (char *) &buf[intrface]))) 	  			
	  					{ 
	  			
  							 DebugMessage("HW address is:"); 	
  							 DebugMessage("%02x:%02x:%02x:%02x:%02x:%02x\n", 
  									(unsigned char)buf[intrface].ifr_hwaddr.sa_data[0], 
  									(unsigned char)buf[intrface].ifr_hwaddr.sa_data[1], 
  									(unsigned char)buf[intrface].ifr_hwaddr.sa_data[2], 
  									(unsigned char)buf[intrface].ifr_hwaddr.sa_data[3], 
  									(unsigned char)buf[intrface].ifr_hwaddr.sa_data[4], 
  									(unsigned char)buf[intrface].ifr_hwaddr.sa_data[5]); 
 	  					} 
						else 
	  			   		{ 
	  						  char str[256]; 
	  						  sprintf (str, "cpm: ioctl device %s", buf[intrface].ifr_name); 
	  						  DebugMessage("%s",str); 
	  					 } 
	  				} //while
		  } 
		  else 
			 DebugMessage("cpm: error ioctl"); 
	   } 
	 
		close (fd);
		return retn; 
		
}
/*------------------------------------------------------------------*/
/* 0: 成功														*/
/* ret:错误码													*/
/* 													*/
/*------------------------------------------------------------------*/
static int Timeshift_curl_preform_task_create( DAL_DownloadControl_t* p_control)
{
		int 					iret = 0;
	
		iret = DAL_SemWait_Timeout( p_control->timeshift_curl_exit_sem, 2000);
		if( iret != 0)
		{
			DebugMessage("[Timeshift_curl_preform_task_create] DAL_SemWait_Timeout failed\n");
			return iret;
		}
		
		iret = DAL_TaskCreate( &(p_control->timeshift_curl_taskID), "time shift curl preform task", time_shift_curl_preform_task_async, NULL, NULL, DAL_CURL_TASK_STACK_SIZE, DAL_CURL_TASK_PRIOITY,0);
		if( iret != 0)
		{
			return iret;
		}
		return 0;
}

/*------------------------------------------------------------------*/
/* 0: 成功														*/
/* -1: sem wait 失败													*/
/* 												*/
/*------------------------------------------------------------------*/
static int Timeshift_curl_preform_task_delete( DAL_DownloadControl_t* p_control)
{
	int 						iret = 0;
	
	p_control->timeshift_curl_exit_flag = 1;

	iret = DAL_SemWait_Timeout(p_control->timeshift_curl_exit_sem, 5000);
	if( iret != 0)
	{
		DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Timeshift_curl_preform_task_delete sem wait failed\n");
	}
	else
	{
		DAL_SemSignal(p_control->timeshift_curl_exit_sem);
		DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ perform task delete success\n");
		return 0;
	}
	return -1;
}

/*------------------------------------------------------------------*/
/* void																*/
/* void																*/
/* 																	*/
/*------------------------------------------------------------------*/
static void* time_shift_curl_preform_task_async( void* param)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pMsg = NULL;
	fd_set 						fdread;
	fd_set 						fdwrite;
	fd_set 						fdexcep;
	int 						maxfd;
	int							stillRunning;
	struct timeval 				timeout;
	CURLMsg*					curl_msg;
	int							nbMsg = 0;
	int							iret = 0;
	int							data_ready = 0;

	p_control->timeshift_curl_multi_handle = curl_multi_init();
	curl_multi_add_handle(p_control->timeshift_curl_multi_handle, p_control->timeshift_curl_handle);

	DebugMessage("time_shift_curl_multi_perform return curl_multi_init\n");


	data_ready = 1;
	for(;;)
	{
		if( 1 == p_control->timeshift_curl_exit_flag)
		{
			goto TASK_EXIT;
		}

		if( 1 == data_ready)
		{
			for(;;)
			{
				if( curl_multi_perform(p_control->timeshift_curl_multi_handle, &stillRunning) == CURLM_CALL_MULTI_PERFORM)
				{
					DebugMessage("timeshift_curl_multi_perform return CURLM_CALL_MULTI_PERFORM\n");
				}
				else
				{
					break;
				}
			}
		}
		while (( curl_msg = curl_multi_info_read(p_control->timeshift_curl_multi_handle, &nbMsg)))
		{
			if ( curl_msg->easy_handle == p_control->timeshift_curl_handle)
			{
				DebugMessage("timeshift found easy_handle 0x%x\n", (unsigned int)p_control->timeshift_curl_handle);
				if( curl_msg->data.result != CURLM_OK)
				{
					DebugMessage("timeshift_curl_handle result != CURLM_OK	 %s\n", curl_easy_strerror(curl_msg->data.result));
					return NULL;
				}
				else if (curl_msg->msg == CURLMSG_DONE)
				{
					/* 直播数据传输不应该有结束的时候*/
					DebugMessage("timeshift_curl_handle CURLMSG_DONE ???????????????\n");
					goto TASK_EXIT;
				}
			}
		}		

		FD_ZERO(&fdread);
		FD_ZERO(&fdwrite);
		FD_ZERO(&fdexcep);
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		iret = curl_multi_fdset( p_control->timeshift_curl_multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);
		if( iret != CURLM_OK || maxfd < 0)
		{
			DebugMessage("timeshift curl_multi_fdset - fdset Failed with %d\n", iret);
			data_ready = 0;
			continue;
		}
		
		iret = select( maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
		if( iret == -1)
		{
			DebugMessage("timeshift Select Failed");
			data_ready = 0;
		}
		else if( iret == 0)
		{
			
			DebugMessage("timeshift Select Timeout - %d times\n",p_control->timeshift_no_data_times);
			data_ready = 0;

			/* 一定时间内没有数据接收，通知上层*/
			p_control->timeshift_no_data_times++;
			if( p_control->timeshift_no_data_times >= DAL_DATA_REV_TIMEOUT)
			{
				pMsg = DAL_MessageAlloc();
				pMsg->MsgType = DALMSG_CURL_REV_TIMEOUT;
				DAL_MessageSend( p_control->queueID, &pMsg);
				DebugMessage("Send --> timeshift DALMSG_CURL_REV_TIMEOUT\n");
				p_control->timeshift_data_timeout_flag = 1;
				p_control->timeshift_no_data_times = 0;
			}
		}
		else 
		{
			data_ready = 1;
			p_control->timeshift_no_data_times = 0;

			/* 数据接收恢复时通知上层*/
			if( p_control->timeshift_data_timeout_flag == 1)
			{
				pMsg = DAL_MessageAlloc();
				pMsg->MsgType = DALMSG_CURL_REV_RESUME;
				DAL_MessageSend( p_control->queueID, &pMsg);
				p_control->timeshift_data_timeout_flag = 0;
				
			}
		}
		
		DAL_TaskDelay(80);
	}

TASK_EXIT:
	DebugMessage("time shift curl async perform task will stop\n");

	curl_multi_remove_handle(p_control->timeshift_curl_multi_handle,p_control->timeshift_curl_handle);
	curl_easy_cleanup(p_control->timeshift_curl_handle);
	
	curl_multi_cleanup(p_control->timeshift_curl_multi_handle);
	
	curl_global_cleanup();
	DAL_DeleteTaskRecord(p_control->timeshift_curl_taskID);
	p_control->timeshift_curl_taskID = 0;
	DAL_SemSignal(p_control->timeshift_curl_exit_sem);

	
/*拿下一段数据timeshift*/	
	DebugMessage("timeshift ts download message request next part data\n");
	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_TIMESHIFT_DOWNLOAD;
	DAL_MessageSend(p_control->queueID, &pMsg);
	return 0;

}

/*------------------------------------------------------------------*/
/* size:write size																*/
/* 															*/
/* 																	*/
/*------------------------------------------------------------------*/
static size_t dal_timeshift_download_write_func(void *buffer, size_t size, size_t nmemb, void *stream)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	unsigned int 				p_freesize;
	int							iret = 0;	
	unsigned int				data_request;


	
	/* curl 由数据接收的标志*/
	p_control->curl_rev_data = 1;

	iret = ring_buffer_freesize( &(p_control->download_buffer), &p_freesize);
	if( iret != 0)
	{
		DebugMessage("[dal_timeshift_download_write_func]ring_buffer_freesize failed\n");
		return size*nmemb;
	}

	if( p_freesize < size*nmemb)
	{
		DebugMessage("[dal_timeshift_download_write_func] BUFFER FULL, data will to give up!!!!!!!!!!!!!\n");
		return size*nmemb;
	}

	iret = ring_buffer_write( &(p_control->download_buffer), buffer, size*nmemb);
	if( iret != 0)
	{
		DebugMessage("[dal_timeshift_download_write_func]ring_buffer_write failed\n");
		return 0;
	}

	
//	DebugMessage("[dal_timeshift_download_write_func] ring_buffer_write:%d",size*nmemb);
	/*提供数据给parse task*/
	if( p_control->parse_pids_flag == PPF_WAIT_READY)
	{
		if( p_control->parse_buf_size == p_control->parse_data_size)
		{
			p_control->parse_pids_flag = PPF_DATA_READY;
			DebugMessage("[dal_timeshift_download_write_func] PPF_DATA_READY\n");
		}
		else
		{
			data_request = p_control->parse_buf_size - p_control->parse_data_size;
			if( data_request >= size*nmemb)
			{
				memcpy(p_control->p_parse_buffer + p_control->parse_data_size, buffer, size*nmemb);
				p_control->parse_data_size += size*nmemb;
			}
			else
			{
				memcpy(p_control->p_parse_buffer + p_control->parse_data_size, buffer, data_request);
				p_control->parse_data_size += data_request;
				DebugMessage("[dal_timeshift_download_write_func] parse_data_size: %d   data_request: %d\n", p_control->parse_data_size, data_request);
			}
		}
	}
	
	return size*nmemb;
}

static void timeshift_ts_start_process( DAL_Message_t* pMsg)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				p_msg = NULL;
	int							iret = 0;

	if( p_control->status == DOWNLOAD_IDEL)
	{
		DebugMessage("[timeshift_ts_start_process]DOWNLOAD_IDEL\n");
		return ;
	}


	memset( p_control->url, 0, sizeof(p_control->url));
	strcpy( p_control->url, pMsg->MsgData.DownloadStart.url);
	DebugMessage("DOWNLOAD start url:%s\n",pMsg->MsgData.DownloadStart.url);


	
	//nlv p_control->parse_pids_flag = PPF_NO_REQUEST;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	
	DebugMessage("timeshift DOWNLOAD libcurl start");
	p_control->timeshift_curl_rev_data = 0;
	p_control->timeshift_data_timeout_flag = 0;
	p_control->timeshift_curl_handle = curl_easy_init();
	p_control->timeshift_curl_exit_flag = 0;
	p_control->timeshift_no_data_times = 0;

	curl_easy_setopt( p_control->timeshift_curl_handle, CURLOPT_URL, p_control->url); 
	curl_easy_setopt( p_control->timeshift_curl_handle, CURLOPT_FOLLOWLOCATION,1);
	//curl_easy_setopt( p_control->timeshift_curl_handle, CURLOPT_MAX_RECV_SPEED_LARGE,125);
	curl_easy_setopt( p_control->timeshift_curl_handle, CURLOPT_WRITEFUNCTION, dal_timeshift_download_write_func);
	
	iret = Timeshift_curl_preform_task_create( p_control);
	if( iret != 0)
	{
		DebugMessage("curl_preform_task_create failed with %d\n", iret);
		curl_easy_cleanup( p_control->timeshift_curl_handle);
		p_control->status = DOWNLOAD_STOP;
		return ;
	}	
	p_control->status = DOWNLOAD_START;
}

static void timeshift_ts_stop_process( DAL_Message_t* pMsg)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	int							iret = 0;

	if( p_control->status != DOWNLOAD_START)
	{
		DebugMessage("DOWNLOAD_IDEL\n");
		return ;
	}
	DebugMessage("before Timeshift_curl_preform_task_delete~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	iret = Timeshift_curl_preform_task_delete( p_control);
	if( iret != 0)
	{
		DebugMessage("before Timeshift_curl_preform_task_delete, waiting...\n");
		return ;
	}
	DebugMessage("after Timeshift_curl_preform_task_delete, waiting...\n");
	p_control->status = DOWNLOAD_STOP;
}


