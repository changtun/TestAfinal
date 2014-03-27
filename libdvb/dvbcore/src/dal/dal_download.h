/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : 
* Description : 
* Author : zyliu
* History :
*	2011/4/11 : Initial Version
******************************************************************************/
#ifndef __DAL_DOWNLOAD_H
#define __DAL_DOWNLOAD_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************/
/*              Includes											*/
/*******************************************************/

/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/
#define DAL_DOWNLOAD_MAX_URL_LEN			255

/*******************************************************/
/*              Exported Types			                */
/*******************************************************/
typedef struct dal_download_param_s
{
	char				url[DAL_DOWNLOAD_MAX_URL_LEN];		
}dal_download_param_t;
/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/
typedef enum DownloadStatus_s
{
	DOWNLOAD_IDEL,
	DOWNLOAD_START,
	//DOWNLOAD_ING,
	DOWNLOAD_STOP,
}DownloadStatus_t;

typedef enum ParsePidsFlag_s
{
	PPF_NO_REQUEST,
	PPF_WAIT_READY,
	PPF_DATA_READY,
}ParsePidsFlag_t;

typedef enum DownloadType_e
{
	DAL_DL_HTTP,
	DAL_DL_LOCAL,
//{{{ add by LYN , for multicast , 2011-08-22
    DAL_DL_UDP,
//}}} add by LYN , for multicast , 2011-08-22
}DownloadType_t;

typedef struct DAL_DownloadControl_s
{
	DownloadStatus_t		status;
	DownloadType_t			type;
	DAL_TaskID_t			taskID;
	DAL_QueueID_t			queueID;
	char					url[DAL_DOWNLOAD_MAX_URL_LEN];	// 当前下载URL
	ring_buffer_t			download_buffer;				// 下载缓冲
	void*					buffer_addr;					// 下载缓冲内存首地址

	DAL_TaskID_t			curl_taskID;
	DAL_TaskID_t			check_taskID;
	DAL_Sem_t				check_sem;	

	CURL*					curl_handle;
	CURLM*					curl_multi_handle;
	CURLcode				curl_error;
	BOOL					curl_exit_flag;				// 用于标识curl perform task 时候退要终止退出
	DAL_Sem_t				curl_exit_sem;				// 用于检测curl perform task 是否已经退出
	BOOL					curl_rev_data;				// check task 用于检测单位时间内是否接收到数据的标志
	unsigned int			no_data_times;				// 标识curl 在多少个单位时间内没有接收到数据	
	BOOL					data_timeout_flag;			// 用于标识curl download 是否已经处于超时状态

	ParsePidsFlag_t			parse_pids_flag;
	unsigned char*			p_parse_buffer;
	unsigned int			parse_buf_size;
	unsigned int			parse_data_size;

	FILE*					hdd_ts_file_handle;
	char*					p_hdd_read_buf;	

	//{{{add by LYN , for multicast , 2011-08-22
	DAL_TaskID_t			multicast_taskID;
	BOOL					multicast_exit_flag;	
	DAL_Sem_t				multicast_exit_sem;
	BOOL					multicast_rev_data;
	BOOL					multicast_timeout_flag;	
	unsigned int			multicast_no_data_times;
	S32                     multicast_socket_fd;
	BOOL                    multicast_socket_exist;
	BOOL                    multicast_buffer_flag;
	//}}}add by LYN , for multicast , 2011-08-22


	//m3u8
	DAL_Sem_t				m3u8_curl_exit_sem;				
	DAL_TaskID_t            m3u8_curl_taskID;
	CURLM*					m3u8_curl_multi_handle;
	CURL*					m3u8_curl_handle;
	BOOL					m3u8_curl_exit_flag;
	unsigned int			m3u8_no_data_times;
	BOOL					m3u8_data_timeout_flag;
	BOOL					m3u8_curl_rev_data;

	//timeshift	
	DAL_Sem_t				timeshift_curl_exit_sem;				
	DAL_TaskID_t            timeshift_curl_taskID;
	CURLM*					timeshift_curl_multi_handle;
	CURL*					timeshift_curl_handle;
	BOOL					timeshift_curl_exit_flag;
	unsigned int			timeshift_no_data_times;
	BOOL					timeshift_data_timeout_flag;
	BOOL					timeshift_curl_rev_data;
}DAL_DownloadControl_t;

int DAL_DownloadTaskCreate(void);
int DAL_DownloadTaskDelete(void);

int DAL_DownloadBufferInit(void);
int DAL_DownloadBufferRelease(void);
int DAL_DownloadBufferGet( ring_buffer_t** ppbuffer);

int DAL_DownloadStart( dal_download_param_t* param);
int DAL_DownloadStop( void);

DAL_QueueID_t DAL_GetDownloadQueueID(void);
BOOL DAL_DownloadIsStop( void);
long int Qjy_Get_testcount();//xzhou add for p2v FF&rew 12-3-17
void init_ring_buffer(void);
void download_queue_clear( void);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __XXXXXXX_H */
/* End of __DAL_DOWNLOAD_H.h  --------------------------------------------------------- */

