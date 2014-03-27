#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include "osapi.h"
#include "pvddefs.h"

#include "dal_wrap.h"
#include "flv_parse.h"
#include "flv_list_play.h"

#define MAX_REQUEST_STRING_LEN		256
#define MAX_REPLY_LEN				256
//#define FLV_LIST_TIME_DAT_FILE		"flv_list_time.txt"
//#define FLV_LIST_URL_FILE			"flv_list_url.txt"

typedef size_t (*curl_write_func_t)(void *buffer, size_t size, size_t nmemb, void *stream);

typedef struct curl_data_s
{
	char*			p_data;
	unsigned int	size;
}curl_data_t;

typedef enum flvlist_parse_status_e
{
	FLVLIST_PARSE_IDEL,					//	
	FLVLIST_PARSE_DOWNLOADING,			//	parse_task 未结束，正在下载数据
	FLVLIST_PARSE_PARSEING,				// 	parse_task 未结束, 下载已经结束，正在解析数据
}flvlist_parse_status_t;

typedef struct flvlist_parse_control_s
{
	BOOL					status;							//	解析运行中标识, 只允许一个解析动作在运行
	flv_play_list_t*		p_flvlist;						//	需要解析的列表, 其中包含了每个flv片段的url地址
	DAL_TaskID_t			parse_task_id;					//	该task 包含下载数据和解析过程
	unsigned char*			p_parse_buffer;					//	用于解析flvlist 的数据缓存
	int						parse_buf_data_size;			// 	parse_buffer中的可用数据量
	BOOL					parse_data_enough;				// 	用于flvlist解析的数据是否接受充足
	flv_parse_input_t		parse_input;					//	在解析过程中用于数据输入的接口
	CURL*					curl_handle;
	CURLcode				curl_error;
	parse_callback_func		result_callback;				//	用于返回解析执行结构的回调函数
	DAL_Sem_t				task_exit_sem;					//	用于确定parse task 是否已经退出
	BOOL					curl_interrupt_flag;			//	curl perform task 中判断是否终止标志
	BOOL					parse_interrupt_flag;			//	parse task 中判断是否终止标志
	int						parse_mode;
	BOOL					parse_exit_flag;				// prase_thread exit flag					
}flvlist_parse_control_t;
static flvlist_parse_control_t		g_flvlist_parse_control;


static flv_play_list_t		g_flv_list;
static flv_play_list_t		g_flv_list_1;

/* global for flvlist parse sync mode*/
static unsigned char*		g_p_parse_buffer = NULL;
static int 					g_parse_buf_data_size = 0;
static BOOL					g_data_enough = FALSE;


#if 0
#endif

static unsigned int flv_list_parse_cur_time( curl_data_t* p_data);
static unsigned int flv_list_get_cur_time( void);
static int flv_list_parse_url( curl_data_t* p_data, char* url);
static int flv_list_get_url( char* request, char* url);
static char* flv_list_get_request_no(time_t cur_time);
static size_t flv_list_curl_write_func(void *buffer, size_t size, size_t nmemb, void *stream);


static int flv_parse_input_func_sync(unsigned int offset, unsigned char* p_buf, unsigned int size);
static size_t flv_list_parse_receive_sync(void *buffer, size_t size, size_t nmemb, void *stream);

static int flv_parse_input_func_async(unsigned int offset, unsigned char* p_buf, unsigned int size);
static size_t flv_list_parse_receive_async(void *buffer, size_t size, size_t nmemb, void *stream);

static int flv_parse_task_create(void);
static void* flv_parse_task(void* param);


int curl_download_url123( const char* url, const char* password, curl_write_func_t func, void* param);


#if 0
#endif

int flv_list_create_list(void)
{
	int			iret = 0;
	memset( &g_flv_list, 0, sizeof(flv_play_list_t));
	iret = DAL_SemCreate( &(g_flv_list.sem), "flvlist sem", 1, 0);
	if( iret != 0)
	{
		DAL_ERROR(("DAL_SemCreate failed with %d\n"));
		return -1;
	}
	return 0;
}

int flv_list_create_list_1(void)
{
	int			iret = 0;
	memset( &g_flv_list_1, 0, sizeof(flv_play_list_t));
	iret = DAL_SemCreate( &(g_flv_list.sem), "flvlist1 sem", 1, 0);
	if( iret != 0)
	{
		return -1;
	}	
	return 0;
}

flv_play_list_t* flv_list_get_list(void)
{
	return &g_flv_list;
}

flv_play_list_t* flv_list_get_list_1(void)
{
	return &g_flv_list_1;
}

/*
int flv_list_clear(void)
{
	DAL_SemWait(g_flv_list.sem);
	memset( g_flv_list.list, 0, sizeof(flv_media_info_t)*FLV_LIST_MAX_SIZE);
	g_flv_list.num = 0;
	DAL_SemSignal(g_flv_list.sem);
	return 0;
}

int flv_list_clear_1(void)
{
	DAL_SemWait(g_flv_list_1.sem);
	memset( g_flv_list_1.list, 0, sizeof(flv_media_info_t)*FLV_LIST_MAX_SIZE);
	g_flv_list_1.num = 0;
	DAL_SemSignal(g_flv_list_1.sem);
	return 0;
}
*/

int flv_list_set_url(flv_play_list_t* p_list, int index, char* url)
{
	int		iret = 0;
	
	if( p_list == NULL || url == NULL || strlen(url) >= FLV_URL_MAX_LEN)
	{
		return -1;
	}
	if( index >= p_list->num)
	{
		return -2;
	}
	
	iret = DAL_SemWait_Timeout(p_list->sem,5000);
	if( iret != 0)
	{
		return -1;
	}	
	memset(p_list->list[index].url,0,sizeof(p_list->list[index].url));
	strcpy(p_list->list[index].url,url);
	DAL_SemSignal(p_list->sem);
	return 0;
}

int flv_list_add_url( flv_play_list_t* p_list, char* url)
{
	int		iret = 0;
	
	if( p_list == NULL || url == NULL || strlen(url) >= FLV_URL_MAX_LEN)
	{
		return -1;
	}
	
	iret = DAL_SemWait_Timeout(p_list->sem,5000);
	if( iret != 0)
	{
		return -1;
	}
	memset( p_list->list[p_list->num].url, 0, FLV_URL_MAX_LEN);
	strcpy( p_list->list[p_list->num].url, url);
	p_list->num++;
	DAL_SemSignal(p_list->sem);
	return 0;
}

int flv_list_clear( flv_play_list_t* p_list)
{
	int		iret = 0;
	
	if( p_list == NULL)
	{
		return -1;
	}
	iret = DAL_SemWait_Timeout(p_list->sem,5000);
	if( iret != 0)
	{
		return -1;
	}
	memset( p_list->list, 0, sizeof(flv_media_info_t)*FLV_LIST_MAX_SIZE);
	p_list->cur_play = 0;
	p_list->num = 0;
	DAL_SemSignal(p_list->sem);
	return 0;
}

int flv_list_get_url_by_index(flv_play_list_t* p_list,int index, char* url)
{
	int		iret = 0;
	
	if( p_list == NULL || url == NULL)
	{
		return -1;
	}
	if( index >= p_list->num)
	{
		return -1;
	}
	iret = DAL_SemWait_Timeout(p_list->sem,10000);
	if( iret != 0)
	{
		return -1;
	}	
	strcpy(url,p_list->list[index].url);
	DAL_SemSignal(p_list->sem);
	return 0;
}

int flv_list_get_seek_info(flv_play_list_t* p_list, double msec_in_whole, unsigned int* p_index, double* p_msec_in_part)
{
	int				i = 0;

	DAL_DEBUG(("msec_in_whole: %f   \n", msec_in_whole));
	
	if( msec_in_whole >= p_list->list[p_list->num-1].time_end)
	{
		return -1;
	}
	for( i = 0; i < p_list->num; i++)
	{
		if( msec_in_whole >= p_list->list[i].time_start && msec_in_whole < p_list->list[i].time_end)
		{
			*p_index = i;
			*p_msec_in_part = msec_in_whole - p_list->list[i].time_start;
			return 0;
		}
	}
	return -1;
}

void flv_list_debug( flv_play_list_t* p_list)
{
	int			i = 0;

	for( i = 0; i < p_list->num; i++)
	{
		printf("\n(%d)  %s\n", i, p_list->list[i].url);
		printf("duration:   %f   ",p_list->list[i].duration);
		printf("time_start: %f   ",p_list->list[i].time_start);
		printf("time_end:   %f\n",p_list->list[i].time_end);
	}
}

#if 0
#endif

/*
	flv_list_convert 
	用于将奇异网原始的flv 点播链接转化为可播放的实际链接
*/
int flv_list_convert(flv_play_list_t* p_list)
{
	char				request_string[MAX_REQUEST_STRING_LEN];
	char				url[FLV_URL_MAX_LEN];
	char*				p = NULL;
	unsigned int		cur_time = 0;
	int					iret = 0;
	int 				i = 0;

	
	for( i = 0; i < g_flv_list_1.num; i++)
	{
		memset(request_string,0,sizeof(request_string));
		p = strrchr(g_flv_list_1.list[i].url, '.');
		strncpy(request_string, g_flv_list_1.list[i].url, (unsigned int)p - (unsigned int)(g_flv_list_1.list[i].url));
		strcat(request_string,".hml?v=");
		cur_time = flv_list_get_cur_time();
		if( cur_time == 0)
		{
			DAL_ERROR(("flv_list_get_cur_time failed\n"));
			return -1;
		}
		strcat(request_string,flv_list_get_request_no(cur_time));

		DAL_DEBUG(("Request: %s\n", request_string));
		
		memset(url,0,sizeof(url));
		iret = flv_list_get_url(request_string,url);
		if( iret != 0)
		{
			DAL_ERROR(("flv_list_get_url failed\n"));
			return -1;
		}
		
		memset(p_list->list[i].url, 0, FLV_URL_MAX_LEN);
		strcpy(p_list->list[i].url, url);
		p_list->num = g_flv_list_1.num;
		
		DAL_DEBUG(("url: %s\n", url));
	}
	return 0;
}

/*
	flv_list_get_cur_time
	通过固定的请求，获取奇艺网的时间值
*/
static unsigned int flv_list_get_cur_time( void)
{
	#if 0
	return (unsigned int)time(NULL);
	#else
	char			reply[MAX_REPLY_LEN];
	char			request[MAX_REQUEST_STRING_LEN];
//	unsigned int	cur_time = 0;
	int				iret = 0;
	curl_data_t		curl_data;

	curl_data.p_data = reply;
	curl_data.size = 0;

	memset(request,0,sizeof(request));
	strcat(request,"http://data.video.qiyi.com/t");
	memset(reply,0,sizeof(reply));
	
	iret = curl_download_url123(request, NULL, flv_list_curl_write_func, (void*)&curl_data);
	if( iret != 0)
	{
		DAL_ERROR(("curl_download_url123 failed\n"));
		return 0;
	}

	return flv_list_parse_cur_time(&curl_data);
	#endif
}

static unsigned int flv_list_parse_cur_time( curl_data_t* p_data)
{
	char*			p = NULL;
	char*			q = NULL;
	unsigned int	cur_time = 0;
	char			time[32];

	memset(time,0,sizeof(time));
	p = p_data->p_data;
	p = strchr(p, '"');
	p = strchr(p+1, '"');
	p = strchr(p+1, '"');
	q = strchr(p+1, '"');
	strncpy(time, p+1, q-p);
	cur_time = atoi(time);
	return cur_time;
}

static char* flv_list_get_request_no(time_t cur_time)
{
	unsigned long 	m = cur_time ^ 2519219136UL;
	unsigned long 	n = 0xFFFFFFFF;		//    4294967296 - 1  = 4294967296 = 0xFFFFFFFF
	unsigned long 	p = 0;
	unsigned long 	q = 0;
	int 			add = 1;		// n本来应该是0x100000000的，因为越界了，这里当做进位来计算
	int 			i = 0;
	static char		value[11];
	char 			ch;
	
	memset(value,0,sizeof(value));
	for( i = 1; i <= 10; i++)
	{
		p = m % 10;
		q = n % 10;
		m = m /10;
		n = n /10;
		if( p+q+add >=10)
		{
			ch = '0' + ((p+q+add)%10);
			add = 1;	
		}
		else
		{
			ch = '0' + (p+q+add);
			add = 0;	
		}
		value[sizeof(value)-i-1] = ch;
	}
	return value;
}

static int flv_list_parse_url( curl_data_t* p_data, char* url)
{
	char*			p = NULL;
	char*			q = NULL;
	
	memset(url,0,FLV_URL_MAX_LEN);
	p = p_data->p_data;
	p = strchr(p, '"');
	p = strchr(p+1, '"');
	p = strchr(p+1, '"');
	q = strchr(p+1, '"');
	strncpy(url,p+1,q-(p+1));
	return 0;
}

static int flv_list_get_url( char* request, char* url)
{
	char			reply[MAX_REPLY_LEN];
	int				iret = 0;
	curl_data_t		curl_data;
	
	curl_data.p_data = reply;
	curl_data.size = 0;
	
	memset(reply,0,sizeof(reply));
	iret = curl_download_url123(request, NULL, flv_list_curl_write_func, (void*)&curl_data);
	if( iret != 0)
	{
		DAL_ERROR(("curl_download_url123 failed\n"));
		return -1;
	}

	iret = flv_list_parse_url( &curl_data, url);
	if( iret != 0)
	{
		DAL_ERROR(("flv_list_parse_url failed\n"));
		return -1;
	}
	return 0;
}

static size_t flv_list_curl_write_func(void *buffer, size_t size, size_t nmemb, void *stream)
{
	curl_data_t*		p_curl_data = (curl_data_t*)stream;
	
	memcpy(p_curl_data->p_data + p_curl_data->size, buffer, size*nmemb);
	p_curl_data->size += size*nmemb;
	
	return size*nmemb;
}


#if 0
#endif

/*
		flv_list_parse_start_sync
		同步方式，获取一个flv列表中所有片段的信息
*/
int flv_list_parse_start_sync( flv_play_list_t* p_list)
{
	CURL*					curl_handle;
	CURLcode				curl_error;
	flv_info_t 				flv_info;
	int						i = 0;
	int						j = 0;
	int						iret = 0;


	/*malloc memory to save flv 5K data in head to parse flv part info*/
	g_p_parse_buffer = (unsigned char*)DAL_Malloc(FLV_PARSE_INFO_MAX_DATA_SIZE);
	if( g_p_parse_buffer == NULL)
	{
		DAL_DEBUG(("alloc buffer failed\n"));
		return -1;
	}
	memset(g_p_parse_buffer,0,FLV_PARSE_INFO_MAX_DATA_SIZE);

	/*set input func to provide data when parse process need flv data*/
	flv_parse_set_input( flv_parse_input_func_sync);

	/*use curl perform to get flv data*/
	curl_handle = curl_easy_init();
	curl_easy_setopt( curl_handle, CURLOPT_WRITEFUNCTION, flv_list_parse_receive_sync);
	//curl_easy_setopt( curl_handle, CURLOPT_VERBOSE, 1L);
	for( i = 0; i < p_list->num; i++)
	{
		g_parse_buf_data_size = 0;
		g_data_enough = 0;
		curl_easy_setopt( curl_handle, CURLOPT_URL, p_list->list[i].url);
		curl_easy_setopt( curl_handle, CURLOPT_TIMEOUT, (long)15);
		curl_error = curl_easy_perform( curl_handle); 
		switch( curl_error)
		{
			case CURLE_OK:
			{
				DAL_DEBUG(("curl_easy_perform return CURLE_OK\n"));
			}
			break;
			case CURLE_WRITE_ERROR:
			{
				if(g_data_enough)
				{
					//DAL_DEBUG(("curl has received enough data\n"));
				}
				else
				{
					DAL_DEBUG(("curl_easy_perform return %d	%s\n", curl_error, curl_easy_strerror(curl_error)));
				}
			}
			break;
			default:
			{
				DAL_DEBUG(("curl_easy_perform return %d	%s\n", curl_error, curl_easy_strerror(curl_error)));
			}
			break;
		}

		/*once we got enough data (5K) , we start the flv info parse process , and save the result to global flvlist*/
		if( g_data_enough)
		{
			iret = flv_parse_perform( &flv_info);
			p_list->list[i].duration = flv_info.duration;
			p_list->list[i].time_start = 0;
			for( j = 0; j < i; j++)
			{
				p_list->list[i].time_start += p_list->list[j].duration;
			}
			p_list->list[i].time_end = p_list->list[i].time_start + p_list->list[i].duration;
		}
		else
		{
			/*if we can't get enough , stop the flvlist parse and return failed*/
			DAL_Free(g_p_parse_buffer);
			curl_easy_cleanup(curl_handle);
			return -1;
		}

	}

	DAL_Free(g_p_parse_buffer);
	curl_easy_cleanup(curl_handle);
	return 0;	
}

static int flv_parse_input_func_sync(unsigned int offset, unsigned char* p_buf, unsigned int size)
{
	if(offset+size>FLV_PARSE_INFO_MAX_DATA_SIZE)
	{
		return -1;
	}
	memcpy( p_buf, g_p_parse_buffer+offset, size);
	return size;
}

static size_t flv_list_parse_receive_sync(void *buffer, size_t size, size_t nmemb, void *stream)
{
	unsigned int			buffer_free = 0;
	unsigned int			copy_size = 0;

	buffer_free = FLV_PARSE_INFO_MAX_DATA_SIZE - g_parse_buf_data_size;
	if( buffer_free <= size*nmemb)
	{
		copy_size = buffer_free;
	}
	else
	{
		copy_size = size*nmemb;
		
	}
	memcpy( g_p_parse_buffer+g_parse_buf_data_size, buffer, copy_size);
	g_parse_buf_data_size += copy_size;
	if( g_parse_buf_data_size == FLV_PARSE_INFO_MAX_DATA_SIZE)
	{
		g_data_enough = TRUE;
		return 0;
	}
	return size*nmemb;
}

#if 0
#endif
static int flv_parse_input_func_async(unsigned int offset, unsigned char* p_buf, unsigned int size)
{
	flvlist_parse_control_t*	p_control = &g_flvlist_parse_control;
	
	if(p_control->status != FLVLIST_PARSE_IDEL && NULL != p_control->p_parse_buffer)
	{
		if(offset+size>FLV_PARSE_INFO_MAX_DATA_SIZE)
		{
			return -1;
		}
		memcpy( p_buf, p_control->p_parse_buffer+offset, size);
		return size;
	}
	else
	{
		return 0;
	}
}

static size_t flv_list_parse_receive_async(void *buffer, size_t size, size_t nmemb, void *stream)
{
	flvlist_parse_control_t*	p_control = &g_flvlist_parse_control;
	unsigned int				buffer_free = 0;
	unsigned int				copy_size = 0;

	buffer_free = FLV_PARSE_INFO_MAX_DATA_SIZE - p_control->parse_buf_data_size;
	if( buffer_free <= size*nmemb)
	{
		copy_size = buffer_free;
	}
	else
	{
		copy_size = size*nmemb;
		
	}
	memcpy( p_control->p_parse_buffer + p_control->parse_buf_data_size, buffer, copy_size);
	p_control->parse_buf_data_size += copy_size;
	if( p_control->parse_buf_data_size == FLV_PARSE_INFO_MAX_DATA_SIZE)
	{
		/*if we buffer enough data to parse , return 0 to stop the curl perform*/
		p_control->parse_data_enough = TRUE;
		return 0;
	}
	return size*nmemb;
}

/* 启动flvlist 解析thread , 
		flag为0  表示只尝试一次，如果中间有获取信息失败，就直接返回解析失败，并退出thread
		flag为1 表示，如果中间失败，会通知暂时失败，但是还会一直尝试获取数据去解析，直到调用停止thread 接口*/
int flv_list_parse_start_async( flv_play_list_t* p_list, parse_callback_func callback, int parse_mode)
{
	flvlist_parse_control_t*	p_control = &g_flvlist_parse_control;
	int							iret = 0;

	/*if task exit sem not create ,create it*/
	if( 0 == p_control->task_exit_sem)
	{
		iret = DAL_SemCreate(&p_control->task_exit_sem, "flvlist parse sem", 1, 0);
		if(iret != 0)
		{
			DAL_ERROR(("DAL_SemCreate failed with 0x%x\n", iret));
			return -1;
		}
	}
	
	/*if a parse task is running*/
	if( p_control->status != FLVLIST_PARSE_IDEL 
		|| NULL != p_control->parse_task_id )
	{
		DAL_DEBUG(("One parse task is running, stop it frist\n"));

		/*stop the running parse task*/
		p_control->curl_interrupt_flag = TRUE;
		p_control->parse_interrupt_flag = TRUE;
		iret = DAL_SemWait_Timeout( p_control->task_exit_sem, 5000);
		if( iret != 0)
		{
			DAL_ERROR(("parse task stop failed\n"));
			p_control->curl_interrupt_flag = FALSE;
			p_control->parse_interrupt_flag = FALSE;			
			return -1;
		}
		DAL_DEBUG(("parse task stop ok\n"));
		DAL_SemSignal( p_control->task_exit_sem);
	}
	
	/*malloc memory to buffer  flv 5K data in head to parse flv part info*/
	if( NULL != p_control->p_parse_buffer)
	{
		DAL_DEBUG(("parse_buffer has already malloc\n"));
	}
	else
	{
		p_control->p_parse_buffer = (unsigned char*)DAL_Malloc(FLV_PARSE_INFO_MAX_DATA_SIZE);
		if( NULL == p_control->p_parse_buffer)
		{	
			DAL_ERROR(("DAL_Malloc %d failed\n", FLV_PARSE_INFO_MAX_DATA_SIZE));
			return -1;
		}
		memset(p_control->p_parse_buffer,0,FLV_PARSE_INFO_MAX_DATA_SIZE);
	}
	
	/*set input func to provide data when parse process need flv data*/
	p_control->parse_input = flv_parse_input_func_async;
	flv_parse_set_input(p_control->parse_input);

	p_control->parse_mode = parse_mode;
	p_control->parse_exit_flag = FALSE;
	p_control->result_callback = callback;
	p_control->p_flvlist = p_list;
	p_control->curl_interrupt_flag = FALSE;
	p_control->parse_interrupt_flag = FALSE;

	iret = flv_parse_task_create();
	if( iret != 0)
	{
		DAL_ERROR(("flv_parse_task_create failed\n"));
		DAL_Free(p_control->p_parse_buffer);
		p_control->p_parse_buffer = NULL;
		return -1;
	}
	DAL_DEBUG(("flv_parse_task_create OK\n"));
	return 0;	
}

int flv_list_parse_stop( void)
{
	flvlist_parse_control_t*	p_control = &g_flvlist_parse_control;

	
	p_control->parse_exit_flag = TRUE;
	DAL_DEBUG(("^^^^^^^^^^^^^^^^^^^^^^^^^^^ set parse_exit_flag = TRUE\n"));
	if( DAL_SemWait_Timeout(p_control->task_exit_sem, 5000) != 0){
		DAL_ERROR(("DAL_SemWait_Timeout failed\n"));
	}else{
		DAL_SemSignal(p_control->task_exit_sem);
		DAL_Free(p_control->p_parse_buffer);
		p_control->p_parse_buffer = NULL;
	}
	return 0;
}

static int flv_parse_task_create(void)
{
	flvlist_parse_control_t*	p_control = &g_flvlist_parse_control;
	int							iret = 0;

	iret = DAL_SemWait_Timeout(p_control->task_exit_sem,5000);
	if( iret != 0)
	{
		DAL_DEBUG(("DAL_SemWait_Timeout task_exit_sem  failed\n"));
		return -1;		
	}	
	iret = DAL_TaskCreate( &p_control->parse_task_id, "flv_parse_task", flv_parse_task, NULL, NULL, 1024*64, 4, 0);
	if( iret != 0)
	{
		DAL_ERROR(("DAL_TaskCreate failed with %d\n", iret));
		return -1;
	}
	return 0;
}

static void* flv_parse_task(void* param)
{
	flvlist_parse_control_t*	p_control = &g_flvlist_parse_control;
	flv_play_list_t*			p_list = p_control->p_flvlist;
	flv_info_t 					flv_info;
	int							iret = 0;
	int							i = 0;
	int							j = 0;
	int							cur_parse = 0;
	int							send_later_retry_flag = 0;	// 在所有解析完成之前，是否已经发送过later retry消息

	/* 不用等着flvlist 解析，直接播放*/
	p_control->result_callback(FLVLIST_PARSE_FAILED_LATER_RETRY);
	/* 等待一小会，节省带宽，然播放优先出图 */
	DAL_TaskDelay(5000);

	/*use curl perform to get flv data*/
	p_control->curl_handle = curl_easy_init();
	curl_easy_setopt( p_control->curl_handle, CURLOPT_WRITEFUNCTION, flv_list_parse_receive_async);
	//curl_easy_setopt( curl_handle, CURLOPT_VERBOSE, 1L);

	while(1)
	//for( i = 0; i < p_list->num; i++)
	{
		if( TRUE == p_control->parse_exit_flag){
			DAL_DEBUG(("parse will exit!!\n"));
			goto PARSE_EXIT;
		}
		
		if( cur_parse == p_list->num){
			DAL_DEBUG(("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  all part parse ok, total %d\n", cur_parse));
			goto PARSE_OK;
		}else{
			DAL_DEBUG(("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  flv_parse_task parse %d\n", cur_parse));
		}
		
		/*reset the parse buffer*/
		p_control->parse_buf_data_size = 0;
		p_control->parse_data_enough = 0;

		/*reset the next flv partition url to curl handle*/
		curl_easy_setopt( p_control->curl_handle, CURLOPT_URL, p_list->list[cur_parse].url);
		curl_easy_setopt( p_control->curl_handle, CURLOPT_TIMEOUT, (long)15);
		//curl_easy_setopt( p_control->curl_handle, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt( p_control->curl_handle, CURLOPT_FOLLOWLOCATION, 1);

		//curl_easy_setopt( p_control->curl_handle, CURLOPT_USERAGENT, "VLC media player - version 0.8.6h Janus - (c) 1996-2008");
		/*perform the curl*/
		p_control->status = FLVLIST_PARSE_DOWNLOADING;
		p_control->curl_error = curl_easy_perform( p_control->curl_handle); 
		switch( p_control->curl_error)
		{
			case CURLE_OK:
			{
				DAL_DEBUG(("curl_easy_perform return CURLE_OK\n"));
			}
			break;
			case CURLE_WRITE_ERROR:
			{
				if(p_control->parse_data_enough)
				{
					//DAL_DEBUG(("curl has received enough data\n"));
				}
				else
				{
					DAL_DEBUG(("curl_easy_perform return %d	%s\n", p_control->curl_error, curl_easy_strerror(p_control->curl_error)));
				}
			}
			break;
			default:
			{
				DAL_DEBUG(("curl_easy_perform return %d	%s\n", p_control->curl_error, curl_easy_strerror(p_control->curl_error)));
			}
			break;
		}

		/*once we got enough data (5K) , we start the flv info parse process , and save the result to global flvlist*/
		if( TRUE == p_control->parse_data_enough)
		{
			p_control->status = FLVLIST_PARSE_PARSEING;
			iret = flv_parse_perform( &flv_info);
			if( iret != 0)
			{
				DAL_ERROR(("flv_parse_perform failed\n"));
			}
			p_list->list[cur_parse].duration = flv_info.duration;
			p_list->list[cur_parse].time_start = 0;
			for( j = 0; j < cur_parse; j++)
			{
				p_list->list[cur_parse].time_start += p_list->list[j].duration;
			}
			p_list->list[cur_parse].time_end = p_list->list[cur_parse].time_start + p_list->list[cur_parse].duration;
			DAL_DEBUG(("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  flv_parse_task parse %d  ok\n", cur_parse));
			cur_parse++;
		}
		else
		{
			/*if we can't get enough , stop the flvlist parse and return failed*/
			//curl_easy_cleanup(p_control->curl_handle);
			//p_control->status = FLVLIST_PARSE_IDEL;
			
			DAL_DEBUG(("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  flv_parse_task parse %d  failed\n", cur_parse));
			DAL_DEBUG(("failed url:  %s  \n", p_list->list[cur_parse].url));

			if( p_control->parse_mode == 0){
				/*use callback notify result */
				if( NULL != p_control->result_callback){
					DAL_ERROR(("result_callback FLVLIST_PARSE_FAILED\n"));
					p_control->result_callback(FLVLIST_PARSE_FAILED);
				}
				goto PARSE_EXIT;
			}else{
				/*use callback notify result */
				if( NULL != p_control->result_callback){
					DAL_DEBUG(("later will retry to parse this part info\n"));
					p_control->result_callback(FLVLIST_PARSE_FAILED_LATER_RETRY);
					send_later_retry_flag = 1;
				}			
				DAL_TaskDelay(3000); //这里之后应该修改为定时器
			}
			/*这里没有cur_parse++ ， 表示将重新尝试解析当前flv 段的信息*/
		}
	}

PARSE_OK:	
	/*use callback notify result */
	if( NULL != p_control->result_callback) 
	{
		if( send_later_retry_flag == 0){
			DAL_DEBUG(("result_callback FLVLIST_PARSE_SUCCESS\n"));
			p_control->result_callback(FLVLIST_PARSE_SUCCESS);
		}else{
			DAL_DEBUG(("result_callback FLVLIST_PARSE_SUCCESS\n"));
			p_control->result_callback(FLVLIST_PARSE_RETRY_SUCCESS);
		}
	}
	
PARSE_EXIT:
	curl_easy_cleanup(p_control->curl_handle);
	p_control->status = FLVLIST_PARSE_IDEL;
	DAL_SemSignal(p_control->task_exit_sem);
	DAL_DeleteTaskRecord(p_control->parse_task_id);
	DAL_DEBUG(("task exit!\n"));
	return (void*)0;
}

int flv_list_parse_free_buffer(void)
{
	flvlist_parse_control_t*	p_control = &g_flvlist_parse_control;
	if( p_control->p_parse_buffer != NULL)
	{
		DAL_Free( p_control->p_parse_buffer);
		p_control->p_parse_buffer = NULL;
	}
	return 0;
}


#if 0
#endif

int flv_list_play_test(void)
{
	flv_play_list_t			flv_play_list;
	int						iret = 0;
	unsigned int			index;
	double					msec_in_part;

	memset(&flv_play_list,0,sizeof(flv_play_list_t));
	flv_list_add_url( &flv_play_list, "http://192.168.2.125/0.flv");
	flv_list_add_url( &flv_play_list, "http://192.168.2.125/1.flv");
	flv_list_add_url( &flv_play_list, "http://192.168.2.125/2.flv");
	flv_list_add_url( &flv_play_list, "http://192.168.2.125/3.flv");
	flv_list_add_url( &flv_play_list, "http://192.168.2.125/4.flv");
	flv_list_add_url( &flv_play_list, "http://192.168.2.125/5.flv");
	flv_list_add_url( &flv_play_list, "http://192.168.2.125/6.flv");
	flv_list_add_url( &flv_play_list, "http://192.168.2.125/7.flv");
	flv_list_add_url( &flv_play_list, "http://192.168.2.125/8.flv");

	iret = flv_list_parse_start_sync( &flv_play_list);
	if( iret != 0)
	{
		DAL_DEBUG(("flv_list_parse_start_sync failed\n"));
		return -1;
	}

	flv_list_debug(&flv_play_list);

	flv_list_get_seek_info( &flv_play_list, 0.0, &index, &msec_in_part);
	printf("0.0   %d   %f\n", index, msec_in_part);

	flv_list_get_seek_info( &flv_play_list, 362.44, &index, &msec_in_part);
	printf("362.44   %d   %f\n", index, msec_in_part);

	flv_list_get_seek_info( &flv_play_list, 1440.477, &index, &msec_in_part);
	printf("1440.477   %d   %f\n", index, msec_in_part);

	flv_list_get_seek_info( &flv_play_list, 2520.0, &index, &msec_in_part);
	printf("2520.0   %d   %f\n", index, msec_in_part);	

	flv_list_get_seek_info( &flv_play_list, 3232.331, &index, &msec_in_part);
	printf("3232.331   %d   %f\n", index, msec_in_part);
	
	return 0;
}


int curl_download_url123( const char* url, const char* password, curl_write_func_t func, void* param)
{
	CURL*					curl_handle;
	CURLcode				curl_error;

	DAL_DEBUG(("url: %s\n",url));
	
	curl_handle = curl_easy_init();
	curl_easy_setopt( curl_handle, CURLOPT_URL, url);
	if( password != NULL)
	curl_easy_setopt( curl_handle, CURLOPT_USERPWD, password);
	curl_easy_setopt( curl_handle, CURLOPT_WRITEFUNCTION, func);
	curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, param);
	curl_easy_setopt( curl_handle, CURLOPT_VERBOSE, 1L);

	curl_error = curl_easy_perform( curl_handle);
	curl_easy_cleanup(curl_handle);
	switch( curl_error)
	{
		case CURLE_OK:
		{
			DAL_DEBUG(("curl_easy_perform return CURLE_OK\n"));
			return 0;
		}
		break;
		default:
		{
			DAL_DEBUG(("curl_easy_perform return %d  %s\n", curl_error, curl_easy_strerror(curl_error)));
			return -1;
		}
		break;
	}
}

