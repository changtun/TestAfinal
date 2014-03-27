#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>


#include "osapi.h"
#include "pvddefs.h"

#include "dal_wrap.h"
#include "dal_common.h"
#include "dal_message.h"
#include "dal_mms.h"

#include "modify_include/uri.h"
#include "modify_include/mmsio.h"
#include "modify_include/mms-common.h"
#include "modify_include/mmsh.h"
#include "modify_include/mms.h"
#include "modify_include/mmsx.h"



#if 0
#endif

#define MMS_DEBUG(_x_)		{ printf("                       [MMS_DEBUG] %s: ", __FUNCTION__); printf _x_ ; }

#define MMS_DOWNLOAD_TASK_STACK_SIZE	1024*32

#define MMS_URL_TAG						"mms://"
#define MMS_READ_TIMEOUT 				10 
#define MMS_CONNECT_TIMEOUT				5			// mms_connect 时调用connect 函数的timeout				
#define MMS_MAX_READ_HEADER_LEN			1024*8		//  由于少数mms 信息返回的file_len 特别大，而实际的header 没有file_len 这么长，用于控制不要读取过多数据	
#define MMS_READ_CHUNK_SIZE				1024*8
#define MMS_DOWNLOAD_BUFFER_SIZE		1024*16
#define MMS_PLAYING_BUFFER_SIZE			(MMS_DOWNLOAD_BUFFER_SIZE + (1024*2))

#define MMS_RECORD_NAME_LEN				32
#define MMS_DATA_MANAGER_MAX_BLOCK		10				// 在flash中最多保存的数据块数
#define MMS_TIMESHIT_MAX_BLOCK			5				// 时移最多保存的数据块数

#define MMS_TRY_CONNECT_MAX_TIMES		3

typedef enum dal_mms_status_e
{
	MMS_STATUS_IDEL,
	MMS_STATUS_CONNECTED,
	MMS_STATUS_READING,
}dal_mms_status_t;

typedef struct dal_mms_buffer_s
{
	unsigned char*		p_data;
	unsigned int		buf_size;				// 
	unsigned int		data_size;				// 有效数据量，不会因为读取而减少
	unsigned int		read_off;				// 读取位置
	unsigned int		off_of_mms_file;		// buffer 中开始的数据在整个mms 文件中的偏移
}dal_mms_buffer_t;

typedef struct dal_mms_private_data_s
{
				// 用于在mms 的io 接口中指示是否处于mms_connect 动作中
	mms_download_event_func event_callback;
}dal_mms_private_data_t;

typedef struct mms_data_record_s
{
	BOOL					valid;				// 是否包含有效数据
	unsigned int			mms_offset;			// 有效数据起始位置在mms_file 中对应位置
	unsigned int			data_size;			// 有效数据量
	char					record_name[MMS_RECORD_NAME_LEN];	// 数据文件名称
}mms_data_record_t;

typedef struct mms_data_manager_s
{
	unsigned int			record_total;		// 数据几率块总数
	int						start_index;		// 存在时间最长的数据块索引，以为所有数据块循环利用
	int						last_copy_index;	// 上次获取数据时的数据块索引，认为是当前播放块
	int						next_use;			// 下一个即将被存入数据的块
	mms_data_record_t*		p_data_record;		// 所有快数据列表
	DAL_Sem_t				sem;				// 操作同步保护
}mms_data_manager_t;


typedef struct dal_mms_control_s
{
	dal_mms_status_t		status;
	int						auto_timeshift;			//
	mms_download_event_func event_notify;
	char					url[MMS_MAX_URL_LEN];
	mmsx_t*					mmsx_handle;
	mmsh_t*					mmsh_handle;
	mms_t*					mms_handle;
	mms_io_t				mms_io;					// libmms 的io 接口

	int 					is_in_connect;			// 记录mms 是否在connect 过程中

	dal_mms_buffer_t		header_buffer;
	dal_mms_buffer_t		download_buffer;		//当前 mms_read 下载数据的buffer
	dal_mms_buffer_t		playing_buffer;		// 当前用于给asf 播放器提供数据的buffer

	DAL_TaskID_t			download_task_id;
	DAL_Sem_t				download_sem;
	DAL_Sem_t				interface_sem;			//用于同步提供给ststream的数据接口和下载task的信号量
	int						read_thread_exit_flag;
	int						connect_interrupt_flag;
	DAL_Sem_t				read_thead_exit_sem;		// task 运行时占有该信号量，结束后释放		
	

	unsigned int			header_len;				// mms 中读取header_len 用于解析asf info 使用
	unsigned int			data_reference;			// mms 中data 数据块的起始位置
	unsigned int			read_offset;			// 记录ststream_mms_read 读取位置，相对于整个mms 文件计算
	unsigned int*			p_st_asf_read_offset;	// st 底层解析asf 文件时记录偏移的变量指针, 通过ststream_mms_set_file_offset_ptr 接口设置
	int						data_reference_found;	// 标识asf 解析时是否找到data 数据块的起始位置, 0未发现，1已发现，2已发现并已经copy header中多余数据到playing buffer
	DAL_Sem_t				data_reference_sem;		// 用于同步data_reference_found = 2 之前st_mms_read 要等一下


	mms_data_manager_t		mms_data_manager;	
}dal_mms_control_t;

static dal_mms_control_t	g_mms_control;

#if 0
#endif

static int dal_mms_io_select_func(void *data, int fd, int state, int timeout_msec);
static int dal_mms_io_tcp_connect_func(void *data, const char *host, int port);
static off_t dal_mms_io_write_func(void *data, int socket, char *buf, off_t num);
static off_t dal_mms_io_read_func(void *data, int socket, char *buf, off_t num);

static void dal_mms_status(dal_mms_status_t status);
static int dal_mms_buffer_init(dal_mms_buffer_t* p_mms_buffer,unsigned int size);
static int dal_mms_buffer_release(dal_mms_buffer_t* p_mms_buffer);
static int dal_mms_download_copy_to_playing(dal_mms_control_t* p_control);

static int dal_mms_get_reocrd_data( dal_mms_control_t* p_control);

static void dal_mms_connect_debug( dal_mms_control_t* p_control);


static int dal_mms_read_header_data(dal_mms_control_t* p_control);
static int dal_mms_read_data_thread_create(dal_mms_control_t* p_control);
static int dal_mms_read_data_thread_cancel(dal_mms_control_t* p_control);

static void* dal_mms_read_data_thread(void* param);



static int mms_data_manager_init(mms_data_manager_t* p_manager);
static int mms_data_manager_release(mms_data_manager_t* p_manager);
static int mms_data_manager_add_record(mms_data_manager_t* p_manager, char* p_data, unsigned int size, unsigned int mms_off);
static int mms_data_manager_get_record(mms_data_manager_t* p_manager, int index, char* p_data, unsigned int buf_size, unsigned int* copy_size, unsigned int* p_mms_off);
static unsigned int mms_data_manager_last_copy_index(mms_data_manager_t* p_manager);

static int mms_lockapi(void);
static int mms_unlockapi(void);



#if 0
#endif

int dal_mms_init(mms_init_param_t* param)
{
	dal_mms_control_t*		p_control = &g_mms_control;

	if( NULL == param){
		DAL_ERROR(("param error\n"));
		return -1;
	}
	
	memset( p_control,0,sizeof(dal_mms_control_t));	
	p_control->mms_io.connect = dal_mms_io_tcp_connect_func;
	p_control->mms_io.read = (mms_io_read_func)dal_mms_io_read_func;
	p_control->mms_io.write = (mms_io_write_func)dal_mms_io_write_func;
	p_control->mms_io.select = dal_mms_io_select_func;
	p_control->mms_io.connect_data = p_control->mms_io.read_data = p_control->mms_io.write_data = p_control->mms_io.select_data = p_control;
	p_control->auto_timeshift = param->auto_timeshift;
	if( NULL != param->dl_notify){
		p_control->event_notify = param->dl_notify; 
	}
	if( DAL_SemCreate( &(p_control->read_thead_exit_sem), "readtheadexit_sem", 1, 0) != 0){
		DAL_ERROR(("readtheadexit_sem create failed!!!\n"));
		return -1;
	}
	if( DAL_SemCreate( &(p_control->download_sem), "mms dl sem", 1, 0) != 0){
		DAL_ERROR(("download_sem create failed!!!\n"));
		DAL_SemDelete(p_control->read_thead_exit_sem);
		return -1;
	}
	if( DAL_SemCreate( &(p_control->data_reference_sem), "data refer sem", 0, 0) != 0){
		DAL_ERROR(("data_reference_sem create failed!!!\n"));
		DAL_SemDelete(p_control->read_thead_exit_sem);
		DAL_SemDelete(p_control->download_sem);
		return -1;
	}
	if( DAL_SemCreate( &(p_control->interface_sem), "interface_sem sem", 1, 0) != 0){
		DAL_ERROR(("interface_sem create failed!!!\n"));
		DAL_SemDelete(p_control->read_thead_exit_sem);
		DAL_SemDelete(p_control->download_sem);
		DAL_SemDelete(p_control->data_reference_sem);
		return -1;
	}
	dal_mms_status(MMS_STATUS_IDEL);
	return 0;
}
int dal_mms_connect(const char* url)
{
	dal_mms_control_t*		p_control = &g_mms_control;
	int						try_time = 0;

	if( mms_lockapi() != 0){
		DAL_ERROR(("mms_lockapi failed\n"));
		return -1;
	}
	
	if( MMS_STATUS_IDEL != p_control->status){
		DAL_ERROR(("mms already connected!!!\n"));
		mms_unlockapi();
		return -1;
	}
	if( NULL == url || strncmp(url,MMS_URL_TAG,strlen(MMS_URL_TAG)) || strlen(url)>(sizeof(p_control->url)-1)){
		DAL_ERROR(("mms url param error!!!\n"));
		mms_unlockapi();
		return -1;
	}

	/* init mms_data_manager */
	if( 1 == p_control->auto_timeshift){
		if( mms_data_manager_init(&(p_control->mms_data_manager)) != 0){
			DAL_ERROR(("mms_data_manager_init create failed!!!\n"));
			mms_unlockapi();
			return -1;
		}
	}

	memset(p_control->url,0,sizeof(p_control->url));
	strncpy(p_control->url,url,sizeof(p_control->url)-1);

	p_control->connect_interrupt_flag = 0;

	try_time = 0;
	p_control->mms_handle = NULL;
	p_control->mmsh_handle = NULL;
	while(1){

		if( p_control->connect_interrupt_flag == 1){
			/* user interrupt connect*/
			DAL_ERROR(("user interrupt mms connect\n"));
			dal_mms_status(MMS_STATUS_IDEL);
			mms_unlockapi();
			return 1;
		}
	
		if( try_time >= MMS_TRY_CONNECT_MAX_TIMES){
			if( 1 == p_control->auto_timeshift){
				mms_data_manager_release(&(p_control->mms_data_manager));
			}
			DAL_ERROR(("connect times > %d\n",MMS_TRY_CONNECT_MAX_TIMES));
			break;
		}

		
		p_control->is_in_connect = 1;				// 给mms_io 指示已经进入connect 中
		p_control->mmsx_handle = mmsx_connect( &(p_control->mms_io), NULL, p_control->url, 1024*1024);
		p_control->is_in_connect = 0;
		
		if( NULL == p_control->mmsx_handle){
			DAL_ERROR(("############################################# ##  mmsx_connect failed   %d  times\n", try_time));
			//if( 1 == p_control->auto_timeshift){
				//mms_data_manager_release(&(p_control->mms_data_manager));
			//}
			//mms_unlockapi();
			//return -1;

			try_time++;
			DAL_TaskDelay(1000);
			continue;
		}else{
			break;
		}
	}

	if( NULL != p_control->mmsx_handle){
		if( NULL != p_control->mmsx_handle->connection_h){
			p_control->mmsh_handle = p_control->mmsx_handle->connection_h;
		}
		else if(NULL != p_control->mmsx_handle->connection){
			p_control->mms_handle = p_control->mmsx_handle->connection;
		}
		else{
			DAL_ERROR(("p_control->mmsx_handle != NULL, but mms_connect failed ???\n"));
		}
	}
	else{
		DAL_ERROR(("dal_connect failed\n"));
		dal_mms_status(MMS_STATUS_IDEL);
		mms_unlockapi();
		return -1;
	}
		
	p_control->read_offset = 0;
	p_control->data_reference_found = 0;
	dal_mms_connect_debug(p_control);

	if( dal_mms_read_header_data(p_control) != 0){
		DAL_ERROR(("dal_mms_read_header_data failed\n"));
		dal_mms_status(MMS_STATUS_IDEL);
		mms_unlockapi();
		return -1;
	}
	dal_mms_status(MMS_STATUS_CONNECTED);
	
	if( dal_mms_read_data_thread_create(p_control) != 0){
		DAL_ERROR(("dal_mms_create_read_data_thread failed\n"));
		dal_mms_status(MMS_STATUS_IDEL);
		mms_unlockapi();
		return -1;
	}
	dal_mms_status(MMS_STATUS_READING);
	DAL_DEBUG(("----------------------------------------MMS_STATUS_READING\n"));
	mms_unlockapi();
	return 0;
}

int dal_mms_close(void)
{
	dal_mms_control_t*		p_control = &g_mms_control;

	if(mms_lockapi()!= 0){
		DAL_ERROR(("mms_lockapi failed\n"));
		return -1;
	}

	if( p_control->status != MMS_STATUS_READING && p_control->status != MMS_STATUS_CONNECTED ){
		DAL_ERROR(("mms is not MMS_STATUS_READING or MMS_STATUS_CONNECTED  %d\n", p_control->status));
		dal_mms_status(MMS_STATUS_IDEL);
		mms_unlockapi();
		return -1;
	}

	if( dal_mms_read_data_thread_cancel(p_control) != 0){
		DAL_ERROR(("dal_mms_read_data_thread_cancel failed\n"));
		DAL_ERROR(("dal_mms_read_data_thread_cancel failed\n"));
		DAL_ERROR(("dal_mms_read_data_thread_cancel failed\n"));
		//mms_unlockapi();
		//return -1;
	}
	
	mmsx_close(p_control->mmsx_handle);
	p_control->mmsx_handle = NULL;

	dal_mms_buffer_release( &(p_control->header_buffer));

	if( 1 == p_control->auto_timeshift){
		if( mms_data_manager_release(&(p_control->mms_data_manager)) != 0){
			DAL_ERROR(("mms_data_manager_release create failed!!!\n"));
			dal_mms_status(MMS_STATUS_IDEL);
			mms_unlockapi();
			return -1;
		}
	}
	dal_mms_status(MMS_STATUS_IDEL);
	DAL_DEBUG(("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  MMS_STATUS_IDEL\n"));
	mms_unlockapi();
	return 0;
}

int dal_mms_interrupt(void)
{
	dal_mms_control_t*		p_control = &g_mms_control;

	if( p_control->is_in_connect == 1){
		DAL_DEBUG(("dal mms is in connect, set the interrupt flag = 1 \n"));
		p_control->connect_interrupt_flag = 1;
	}else{
		DAL_DEBUG(("dal mms is not in connect\n"));
	}
	return 0;	
}

#if 0
#endif

int ststream_mms_read(unsigned char* buffer, unsigned int size)
{
	dal_mms_control_t*		p_control = &g_mms_control;
	unsigned int			data_avail = 0;
	unsigned int			copy_size1 = 0;
	unsigned int			copy_size2 = 0;//int i;

	if( mms_lockapi() != 0){
		DAL_ERROR(("mms_lockapi failed\n"));
		return -1;
	}

	if( p_control->status != MMS_STATUS_READING)
	{
		DAL_DEBUG(("status NOT MMS_STATUS_READING\n"));
		mms_unlockapi();
		return 0x0000ffff;
	}
	
	//DAL_DEBUG(("p_control->header_buffer: 0x%x	p_control->playing_buffer: 0x%x\n", p_control->header_buffer.p_data,p_control->playing_buffer.p_data));

	if( p_control->data_reference_found == 0){
		/*read data from header buffer*/
		if( p_control->read_offset < 0 || p_control->read_offset > p_control->header_buffer.data_size){
			DAL_ERROR(("read_offset error when read header! read_offset: %u  data_size: %d\n", p_control->read_offset, p_control->header_buffer.data_size));
			mms_unlockapi();
			return -1;
		}
		if( size > p_control->header_buffer.data_size - p_control->header_buffer.read_off){
			DAL_ERROR(("need size > header size left!!!   size: %u  data_size: %u  read_off: %u\n", size, p_control->header_buffer.data_size, p_control->header_buffer.read_off));
			mms_unlockapi();
			return -1;
		}
		memcpy(buffer, p_control->header_buffer.p_data + p_control->header_buffer.read_off, size);
		p_control->header_buffer.read_off += size;
		//p_control->read_offset += size;
		//DAL_DEBUG(("read header data %u ok ,  read_offset: %u\n", size, p_control->read_offset));
	}else if(p_control->data_reference_found == 2){
		//DAL_DEBUG(("will read from playing buffer, read_offset: %u  "));
		/*read data from playing buffer*/
		if( p_control->read_offset < p_control->playing_buffer.off_of_mms_file 
			|| p_control->read_offset > p_control->playing_buffer.off_of_mms_file + p_control->playing_buffer.data_size){
			DAL_ERROR(("read_offset match playing buffer offset failed, read_offset: %u  playing_buffer.off_of_mms_file: %u  data_size: %u\n",p_control->read_offset,p_control->playing_buffer.off_of_mms_file,p_control->playing_buffer.data_size));
			mms_unlockapi();
			return -1;
		}
		if( size > p_control->download_buffer.buf_size){
			DAL_ERROR(("can't read so much data one time\n"));
			mms_unlockapi();
			return -1;
		}		
		/*correct the playing buffer read_off*/
		if( p_control->read_offset != p_control->playing_buffer.off_of_mms_file + p_control->playing_buffer.read_off){
			DAL_DEBUG(("mms read_offset: %u  off_of_mms_file: %u  buf_read_off: %u\n",p_control->read_offset,p_control->playing_buffer.off_of_mms_file,p_control->playing_buffer.read_off));
			p_control->playing_buffer.read_off = p_control->read_offset - p_control->playing_buffer.off_of_mms_file;
			DAL_DEBUG(("!!!!!!!!!!!!!!! correct the playing buffer read_off to %u\n", p_control->playing_buffer.read_off));
		}
		data_avail = p_control->playing_buffer.data_size - p_control->playing_buffer.read_off;
		if( size <= data_avail){
			/*
			if( size == 34){
				for( i = 0; i< size; i++){
					printf("0x%02x  ",p_control->playing_buffer.p_data[i]);
				}
				printf("p_control->playing_buffer.read_off: %u\n", p_control->playing_buffer.read_off);
			}*/
			memcpy(buffer, p_control->playing_buffer.p_data+p_control->playing_buffer.read_off, size);
			p_control->playing_buffer.read_off += size;

			/*
			if( size == 34){
				for( i = 0; i< size; i++){
					printf("0x%02x  ",buffer[i]);
				}
				printf("buffer: 0x%x\n", buffer);
			}*/
			//p_control->playing_buffer.data_size -= size;
		}else{
			copy_size1 = data_avail;
			copy_size2 = size - copy_size1;
			memcpy(buffer, p_control->playing_buffer.p_data+p_control->playing_buffer.read_off, copy_size1);
			p_control->playing_buffer.read_off += copy_size1;

			if( 1 == p_control->auto_timeshift){
				/* auto_timeshift 的情况，需要从mms_data_manager中获取下一块数据*/
				while( 0 != dal_mms_get_reocrd_data(p_control)){
					if( p_control->mmsx_handle == NULL || p_control->read_thread_exit_flag == 1){
						DAL_DEBUG(("mms has closed!!!\n"));
						mms_unlockapi();
						return -1;
					}
					DAL_TaskDelay(200);
				}
			}else{
				/* not auto_timeshift 情况，则直接从download buffer 中获取下一块数据*/
				while( 0 != dal_mms_download_copy_to_playing(p_control)){
					if( p_control->mmsx_handle == NULL || p_control->read_thread_exit_flag == 1){
						DAL_DEBUG(("mms has closed!!!\n"));
						mms_unlockapi();
						return -1;
					}					
					DAL_TaskDelay(200);
				}
			}
			
			memcpy(buffer+copy_size1, p_control->playing_buffer.p_data+p_control->playing_buffer.read_off, copy_size2);
			p_control->playing_buffer.read_off += copy_size2;
			//DAL_DEBUG(("1 data_size: %u\n", p_control->playing_buffer.data_size));
			//DAL_DEBUG(("2 data_size: %u\n", p_control->playing_buffer.data_size));
		}
		//p_control->read_offset += size;
		
		//DAL_DEBUG(("read asf chunk data %u ok, off_of_mms_file: %u read_off: %u  data_size: %u \n",
			//size, p_control->playing_buffer.off_of_mms_file, p_control->playing_buffer.read_off, p_control->playing_buffer.data_size));
			
	}else{
		DAL_DEBUG(("wait copy header last some data\n"));
		mms_unlockapi();
		return -1;
	}
	p_control->read_offset += size;
	//DAL_DEBUG(("p_control->read_offset: %u\n", p_control->read_offset));

	mms_unlockapi();
	return size;
}

int ststream_mms_seek(unsigned int offset)
{
	dal_mms_control_t*		p_control = &g_mms_control;

	if( mms_lockapi() != 0){
		DAL_ERROR(("mms_lockapi failed\n"));
		return -1;
	}


	if( p_control->status != MMS_STATUS_READING)
	{
		DAL_DEBUG(("status MMS_STATUS_IDEL\n"));
		mms_unlockapi();
		return 0;
	}

	//DAL_DEBUG(("p_control->header_buffer: 0x%x  p_control->playing_buffer: 0x%x\n", p_control->header_buffer.p_data,p_control->playing_buffer.p_data));
	
	if( p_control->data_reference_found == 0){
		/* seek in header buffer*/
		DAL_ERROR((" p_control->data_reference_found == 0 ???????????????????\n"));
	}
	else if(p_control->data_reference_found == 2){ 
		/* seek in playing buffer*/
		if( offset < p_control->playing_buffer.off_of_mms_file || offset > p_control->playing_buffer.off_of_mms_file + p_control->playing_buffer.data_size){
			DAL_ERROR((" offset < p_control->playing_buffer.off_of_mms_file  offset: %u  off_of_mms_file: %u  data_size: %u\n",offset,p_control->playing_buffer.off_of_mms_file,p_control->playing_buffer.data_size));
			mms_unlockapi();
			return -1;
		}
		p_control->playing_buffer.read_off = offset - p_control->playing_buffer.off_of_mms_file;
		p_control->read_offset = p_control->playing_buffer.off_of_mms_file + p_control->playing_buffer.read_off;
		if( NULL != p_control->p_st_asf_read_offset){
			/*设置底层asf fileoffset , 保证asf fileoffset 与上层dal 中一致*/
			*(p_control->p_st_asf_read_offset) = p_control->read_offset;
		}
	}
	mms_unlockapi();
	return 0;
}
unsigned int ststream_mms_tell( void)
{
	dal_mms_control_t*		p_control = &g_mms_control;

	if( mms_lockapi() != 0){
		DAL_ERROR(("mms_lockapi failed\n"));
		return -1;
	}

	if( p_control->status != MMS_STATUS_READING)
	{
		DAL_DEBUG(("status NOT MMS_STATUS_READING\n"));
		mms_unlockapi();
		return 0;
	}
	//DAL_DEBUG(("p_control->header_buffer: 0x%x  p_control->playing_buffer: 0x%x\n", p_control->header_buffer.p_data,p_control->playing_buffer.p_data));

	if( p_control->data_reference_found == 0){
		//DAL_DEBUG(("1 ------------------------------------------------Tello: %u\n", p_control->header_buffer.off_of_mms_file + p_control->header_buffer.read_off));
		mms_unlockapi();
		return p_control->header_buffer.off_of_mms_file + p_control->header_buffer.read_off;
	}else{
		//DAL_DEBUG(("2 ------------------------------------------------Tello: %u\n", p_control->playing_buffer.off_of_mms_file + p_control->playing_buffer.read_off));
		mms_unlockapi();
		return p_control->playing_buffer.off_of_mms_file + p_control->playing_buffer.read_off;
	}	
}
int ststream_mms_found_data_reference(unsigned int data_reference)
{
	dal_mms_control_t*		p_control = &g_mms_control;

	if( mms_lockapi() != 0){
		DAL_ERROR(("mms_lockapi failed\n"));
		return -1;
	}
	p_control->data_reference = data_reference;
	p_control->data_reference_found = 1;
	DAL_DEBUG(("set data_reference_found 1   data_reference: %u\n",data_reference ));
	DAL_SemWait( p_control->data_reference_sem);
	DAL_DEBUG(("DAL_SemWait  data_reference_sem ok\n"));
	mms_unlockapi();
	return 0;
}
int ststream_mms_set_file_offset_ptr(unsigned int* p_ststeam_file_offset)
{
	dal_mms_control_t*		p_control = &g_mms_control;

	if( mms_lockapi() != 0){
		DAL_ERROR(("mms_lockapi failed\n"));
		return -1;
	}	
	p_control->p_st_asf_read_offset = p_ststeam_file_offset;
	mms_unlockapi();
	return 0;
}




#if 0
#endif

static int mms_lockapi(void)
{/*
	dal_mms_control_t*		p_control = &g_mms_control;
	if( p_control->interface_sem == 0) return -1;
	return DAL_SemWait_Timeout( p_control->interface_sem, 5000);
	*/
	return 0;
}

static int mms_unlockapi(void)
{/*
	dal_mms_control_t*		p_control = &g_mms_control;
	if( p_control->interface_sem == 0) return -1;
	return DAL_SemSignal( p_control->interface_sem);
	*/
	return 0;
}

static void dal_mms_status(dal_mms_status_t status)
{
	dal_mms_control_t*		p_control = &g_mms_control;
	p_control->status = status;
}


static int dal_mms_download_copy_to_playing(dal_mms_control_t* p_control)
{
	if( DAL_SemWait_Timeout( p_control->download_sem, 100) != 0){
		return -1;
	}
	if( p_control->status != MMS_STATUS_READING){
		DAL_SemSignal( p_control->download_sem);
		return -1;
	}
	if( p_control->download_buffer.data_size != p_control->download_buffer.buf_size){
		//DAL_ERROR(("download buffer is not fill full, can't to provide data!\n"));
		DAL_SemSignal( p_control->download_sem);
		return -1;
	}else{
		DAL_DEBUG(("will copy a data chunk, asf file offset: %u  data_size: %u\n", p_control->download_buffer.off_of_mms_file, p_control->download_buffer.data_size));
		memcpy(p_control->playing_buffer.p_data, p_control->download_buffer.p_data, p_control->download_buffer.data_size);
		p_control->playing_buffer.data_size = p_control->download_buffer.data_size;
		p_control->playing_buffer.off_of_mms_file = p_control->download_buffer.off_of_mms_file;
		p_control->playing_buffer.read_off = 0;		
		p_control->download_buffer.data_size = 0;
	}
	DAL_SemSignal( p_control->download_sem);
	//DAL_DEBUG(("copy a data chunk success, data_size: %u\n", p_control->playing_buffer.data_size));

	//DAL_DEBUG(("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ Signal"));
	return 0;
}

static int dal_mms_get_reocrd_data( dal_mms_control_t* p_control)
{
	unsigned int		copy_size = 0;
	unsigned int		mms_off = 0;
	
	if( 0 != mms_data_manager_get_record( &p_control->mms_data_manager, -1, 
										(char*)p_control->playing_buffer.p_data, p_control->playing_buffer.buf_size, &copy_size, &mms_off)){
		//DAL_ERROR(("mms_data_manager_get_record failed\n"));
		return -1;
	}
	p_control->playing_buffer.off_of_mms_file = mms_off;
	p_control->playing_buffer.data_size = copy_size;
	p_control->playing_buffer.read_off = 0;
	return 0;
}

static int dal_mms_read_header_data(dal_mms_control_t* p_control)
{
	unsigned int		need_size = 0;
	unsigned int		read_size = 0;

	if( p_control->mmsh_handle || p_control->mms_handle){
		if( NULL != p_control->mmsh_handle){
			p_control->header_len = DAL_MIN(p_control->mmsh_handle->file_len,MMS_MAX_READ_HEADER_LEN);
		}else if( NULL != p_control->mms_handle){
			p_control->header_len = DAL_MIN(p_control->mms_handle->asf_header_len,MMS_MAX_READ_HEADER_LEN);
		}else{
			DAL_ERROR(("no mmsh_handle or mms_handle \n"));
			return -1;
		}
		DAL_DEBUG(("will read file len: %u\n", p_control->header_len));
	}
	if( NULL == p_control->header_buffer.p_data){
		if( dal_mms_buffer_init(&(p_control->header_buffer), MMS_MAX_READ_HEADER_LEN) != 0){
			DAL_ERROR(("header_buffer init failed\n"));
			return -1;
		}
	}
	p_control->header_buffer.off_of_mms_file = 0;
	while( p_control->header_buffer.data_size < p_control->header_len)
	{
		if( 0 == p_control->header_buffer.data_size){
			p_control->header_buffer.off_of_mms_file = (unsigned int)mmsx_get_current_pos(p_control->mmsx_handle);
			DAL_DEBUG(("set header_buffer.off_of_mms_file %u\n", p_control->header_buffer.off_of_mms_file));
		}
		need_size = p_control->header_len - p_control->header_buffer.data_size;
		need_size = need_size > MMS_READ_CHUNK_SIZE ? MMS_READ_CHUNK_SIZE : need_size;
		DAL_DEBUG(("header_len: %u  data_size: %u  need_size: %u\n", p_control->header_len,p_control->header_buffer.data_size,need_size));
		
		read_size = mmsx_read( &(p_control->mms_io), p_control->mmsx_handle, (char*)(p_control->header_buffer.p_data + p_control->header_buffer.data_size), need_size);
		if( read_size < 0){
			DAL_ERROR(("mmsx_read failed\n"));
			dal_mms_buffer_release( &(p_control->header_buffer));
			return -1;
		}else{
			DAL_DEBUG(("read_size :  %d\n", read_size));
			p_control->header_buffer.data_size += read_size;	
		}
	}
	DAL_DEBUG(("mms header read ok!\n"));
	DAL_DEBUG(("data_size: %u    mms_off: %ld \n", p_control->header_buffer.data_size, (off_t)mmsx_get_current_pos(p_control->mmsx_handle)));

	{
		FILE*		fp = fopen("header.asf", "w");
		fwrite(p_control->header_buffer.p_data, 1, p_control->header_buffer.data_size, fp);
		fclose(fp);
		system("sync");
	}
	/*{
		int i = 0;
		for( i = 2579; i < 2579 + 50; i++){
			printf("0x%x  ", p_control->header_buffer.p_data[i]);
		}
		printf("\n");
	}*/
	
	return 0;
}

static int dal_mms_read_data_thread_create(dal_mms_control_t* p_control)
{
	p_control->read_thread_exit_flag = 0;

	if( NULL == p_control->download_buffer.p_data){
		if( 0 != dal_mms_buffer_init(&(p_control->download_buffer),MMS_DOWNLOAD_BUFFER_SIZE)){
			DAL_ERROR(("mms download_buffer init failed\n"));
			return -1;
		}
	}
	if( NULL == p_control->playing_buffer.p_data){
		if( 0 != dal_mms_buffer_init(&(p_control->playing_buffer),MMS_PLAYING_BUFFER_SIZE)){
			DAL_ERROR(("mms playing_buffer init failed\n"));
			return -1;
		}
	}

	
	if( 0 != DAL_TaskCreate( &(p_control->download_task_id), "mms dl task", dal_mms_read_data_thread, (void*)p_control, NULL, MMS_DOWNLOAD_TASK_STACK_SIZE, 5, 0)){
		DAL_ERROR(("mms download task create failed\n"));
		dal_mms_buffer_release(&(p_control->download_buffer));	
		dal_mms_buffer_release(&(p_control->playing_buffer));		
		return -1;
	}
	return 0;
}

static int dal_mms_read_data_thread_cancel(dal_mms_control_t* p_control)
{
	
	p_control->read_thread_exit_flag = 1;
	DAL_DEBUG(("set read_thread_exit_flag = 1\n"));
	
	if( 0 != DAL_SemWait_Timeout( p_control->read_thead_exit_sem, 8000)){
		DAL_ERROR(("DAL_SemWait_Timeout DAL_SemWait_Timeout failed\n"));
		//return -1;
	}else{
		DAL_SemSignal( p_control->read_thead_exit_sem);
	}

	if( NULL != p_control->download_buffer.p_data){
		DAL_Free(p_control->download_buffer.p_data); 
		p_control->download_buffer.p_data = NULL;
	}
	if( NULL != p_control->playing_buffer.p_data){
		DAL_Free(p_control->playing_buffer.p_data); 
		p_control->playing_buffer.p_data = NULL;
	}
	return 0;
}

static void* dal_mms_read_data_thread(void* param)
{
	dal_mms_control_t*		p_control = (dal_mms_control_t*)param;
	unsigned int			need_size = 0;
	unsigned int			read_size = 0;
	unsigned int			copy_size = 0;
	int						i = 0;
	int						first_block_flag = 0;

	if( 0 != DAL_SemWait_Timeout( p_control->read_thead_exit_sem, 2000)){
		DAL_ERROR(("DAL_SemWait_Timeout read_thead_exit_sem failed\n"));
		goto task_exit;
	}

	if( p_control->event_notify){
		p_control->event_notify(MMS_DOWNLOAD_START,(void*)0);
	}

	while(1)
	{
		if( p_control->read_thread_exit_flag == 1){
			//DAL_SemWait_Timeout( p_control->download_sem, 500);
			DAL_DEBUG(("read_thread_exit_flag == 1, thread will exit\n"));
			break;
		}
		if( p_control->data_reference_found == 1){
			DAL_DEBUG(("data_reference_found !!!\n"));
			/* copy header buffer 中处于data_reference 位置之后的数据到playing buffer
			因为asf 开始播放时，需要seek 到 data_reference 的位置*/
			if( p_control->header_len < p_control->data_reference){
				DAL_ERROR(("header_len < data_reference ???\n"));
			}else{
				if( 0 != p_control->playing_buffer.data_size){
					DAL_ERROR(("will copy header last some data, but playing buffer is not empty"));
				}
				copy_size = p_control->header_buffer.data_size - p_control->data_reference;
				memcpy(p_control->playing_buffer.p_data + p_control->playing_buffer.data_size, 
							p_control->header_buffer.p_data + p_control->data_reference, copy_size);
				p_control->playing_buffer.data_size += copy_size;
				p_control->playing_buffer.off_of_mms_file = p_control->data_reference;	//第一次设置playing buffer  的off_of_mms_file
				//p_control->header_buffer.data_size -= copy_size;
				DAL_DEBUG(("header_buffer.data_size: %u  data_reference: %u\n", p_control->header_buffer.data_size, p_control->data_reference));
				DAL_DEBUG(("copy header last some data to playing buffer, copy_size: %d  off_of_mms_file: %u\n", copy_size, p_control->playing_buffer.off_of_mms_file));
/*
				for( i = 0; i< 50; i++){
					printf("0x%02x  ",p_control->playing_buffer.p_data[i]);
				}
				*/
				printf("p_control->playing_buffer.data_size: %u\n", p_control->playing_buffer.data_size);
			}
			p_control->data_reference_found = 2;
			DAL_SemSignal( p_control->data_reference_sem);
		}
		if( 0 != DAL_SemWait_Timeout( p_control->download_sem, 100)){
			continue;
		}
		if( p_control->download_buffer.data_size < p_control->download_buffer.buf_size)
		{
			if( NULL == p_control->mmsx_handle ){
				DAL_DEBUG(("NULL == p_control->mmsx_handle, thread will exit\n"));
				break;
			}
			if( 0 == p_control->download_buffer.data_size){
				/* 每次重新开始填充download buffer 时设置一下mms 的offset*/
				unsigned int off = (unsigned int)mmsx_get_current_pos(p_control->mmsx_handle);
				if(  off - p_control->download_buffer.off_of_mms_file > MMS_DOWNLOAD_BUFFER_SIZE){
					/* 之前发现有个电台，mmsx_read 16K 数据后，mmsx_get_current_pos 却增加了18K ，
					导致后面数据位置对不上，这里临时修改一下
					mms://widget.cbs.co.kr/cbs939_live 		*/
					SYS_DEBUG(SD_WARNING,MODULE_DAL,"mmsx_get_current_pos maybe some error??\n");
					SYS_DEBUG(SD_WARNING,MODULE_DAL,"mmsx_get_current_pos maybe some error??\n");
					p_control->download_buffer.off_of_mms_file += MMS_DOWNLOAD_BUFFER_SIZE;
				}else{
					p_control->download_buffer.off_of_mms_file = off;
				}
				DAL_DEBUG(("set download buffer off_of_mms_file: %u\n", p_control->download_buffer.off_of_mms_file));
			}
			need_size = p_control->download_buffer.buf_size - p_control->download_buffer.data_size;
			need_size = need_size > MMS_READ_CHUNK_SIZE ? MMS_READ_CHUNK_SIZE : need_size;
			read_size = mmsx_read( &(p_control->mms_io), p_control->mmsx_handle, (char*)(p_control->download_buffer.p_data+p_control->download_buffer.data_size), need_size);
			if( read_size < 0){
				DAL_ERROR(("mmsx_read failed with %u\n", read_size));
			}else{
				p_control->download_buffer.data_size += read_size;
				DAL_DEBUG(("mms_read ok , download_buffer.data_size: %u K\n", p_control->download_buffer.data_size/1024));
			}

			if( 0 == first_block_flag && p_control->download_buffer.data_size == p_control->download_buffer.buf_size){
				if( p_control->event_notify){
					p_control->event_notify(MMS_FRIST_DATA_BLOCK_OK,(void*)0);
				}
				first_block_flag = 1;
			}

			if( 1 == p_control->auto_timeshift){
				/*如果是自动timeshift 模式的情况，则立即把装满的buffer 存到mms_data_manager 中*/
				if( p_control->download_buffer.data_size == p_control->download_buffer.buf_size){
					DAL_DEBUG(("will mms_data_manager_add_record\n"));
					if( 0 != mms_data_manager_add_record(&(p_control->mms_data_manager), (char*)p_control->download_buffer.p_data, p_control->download_buffer.data_size, p_control->download_buffer.off_of_mms_file)){
						DAL_ERROR(("mms_data_manager_add_record failed	off_of_mms_file: %u\n ",p_control->download_buffer.off_of_mms_file));
					}else{
						DAL_DEBUG(("mms_data_manager_add_record ok\n"));
						p_control->download_buffer.data_size = 0;
						p_control->download_buffer.read_off = 0;
					}
				}
			}
	
		}else{
			//DAL_DEBUG(("download_buffer.data_size: %u K  buf_size: %u K\n", p_control->download_buffer.data_size/1024,p_control->download_buffer.buf_size/1024));
		}
		DAL_SemSignal( p_control->download_sem);
	}

task_exit:
	DAL_DeleteTaskRecord( p_control->download_task_id);
	p_control->download_task_id = -1;
	DAL_SemSignal(p_control->read_thead_exit_sem);
	DAL_DEBUG(("threa exit\n"));
	return (void*)0;
}
#if 0
#endif

static int dal_mms_buffer_init(dal_mms_buffer_t* p_mms_buffer, unsigned int size)
{
	if( NULL == p_mms_buffer || NULL != p_mms_buffer->p_data){
		return -1;
	}
	if( (p_mms_buffer->p_data = (unsigned char*)DAL_Malloc(size)) == NULL){
		return -1;
	}
	p_mms_buffer->buf_size = size;
	p_mms_buffer->data_size = 0;
	p_mms_buffer->read_off = 0;
	p_mms_buffer->off_of_mms_file = 0;
	return 0;
}

static int dal_mms_buffer_release(dal_mms_buffer_t* p_mms_buffer)
{
	if( NULL == p_mms_buffer || p_mms_buffer->p_data == NULL){
		return -1;
	}
	DAL_Free( p_mms_buffer->p_data);
	memset(p_mms_buffer, 0, sizeof(dal_mms_buffer_t));
	return 0;
}

#if 0
#endif

static int mms_data_manager_init(mms_data_manager_t* p_manager)
{
	int		i = 0;
	
	if( NULL == p_manager){
		return -1;
	}
	p_manager->record_total = MMS_DATA_MANAGER_MAX_BLOCK;
	if( (p_manager->p_data_record = (mms_data_record_t*)DAL_Malloc(sizeof(mms_data_record_t)*p_manager->record_total)) == NULL){
		DAL_ERROR(("p_data_record malloc failed\n"));
		return -1;
	}
	memset(p_manager->p_data_record,0,sizeof(mms_data_record_t)*p_manager->record_total);
	for( i = 0; i < MMS_DATA_MANAGER_MAX_BLOCK; i++){
		sprintf(p_manager->p_data_record[i].record_name,"./mms_data/%d.asf", i);
	}
	if( 0!= DAL_SemCreate( &(p_manager->sem), "mms manager sem", 1, 0)){
		DAL_ERROR(("mms manager sem create failed\n"));
		DAL_Free(p_manager->p_data_record);
		return -1;
	}
	p_manager->next_use = 0;
	p_manager->last_copy_index = -1;
	p_manager->start_index = 0;
	return 0;
}

static int mms_data_manager_release(mms_data_manager_t* p_manager)
{
	if( NULL == p_manager){
		return -1;
	}
	DAL_Free(p_manager->p_data_record);
	if( 0 != DAL_SemDelete(p_manager->sem)){
		DAL_ERROR(("mms manager sem delete failed\n"));
		return -1;
	}
	return 0;
}

static int mms_data_manager_add_record(mms_data_manager_t* p_manager, char* p_data, unsigned int size, unsigned int mms_off)
{
	FILE*		fp = NULL;
	int			iret = 0;
	int			old_valid;
	
	if( NULL == p_manager){
		return -1;
	}
	if( 0 != DAL_SemWait_Timeout( p_manager->sem, 2000)){
		DAL_ERROR(("DAL_SemWait_Timeout sem\n"));
		return -1;
	}	
	if( p_manager->next_use < 0 || p_manager->next_use >= p_manager->record_total){
		DAL_ERROR(("next_use error\n"));
		DAL_SemSignal( p_manager->sem);
		return -1;
	}
	fp = fopen(p_manager->p_data_record[p_manager->next_use].record_name,"w");
	if( NULL == fp){
		DAL_ERROR(("fopen %s  error\n",p_manager->p_data_record[p_manager->next_use].record_name));
		DAL_SemSignal( p_manager->sem);
		return -1;
	}
	if( size != fwrite( p_data, 1, size, fp)){
		DAL_ERROR(("fwrite error %d\n", size));
		fclose(fp);
		DAL_SemSignal( p_manager->sem);
		return -1;
	}
	fclose(fp);
	system("sync");
	old_valid = p_manager->p_data_record[p_manager->next_use].valid;
	p_manager->p_data_record[p_manager->next_use].data_size = size;
	p_manager->p_data_record[p_manager->next_use].mms_offset = mms_off;
	p_manager->p_data_record[p_manager->next_use].valid = TRUE;

	
	
	MMS_DEBUG(("new mms data block record success!  block index: %u  file: %s   mms_off: %u  data_size: %u K  \n",
		p_manager->next_use, p_manager->p_data_record[p_manager->next_use].record_name
		,p_manager->p_data_record[p_manager->next_use].mms_offset, size/1024));
	


	if( old_valid == TRUE ){
		/*如果start_index 块被覆盖，则start_index 要变化*/
		p_manager->start_index = (p_manager->start_index + 1) % p_manager->record_total;
		//p_manager->start_index = p_manager->next_use;
	}

	
	p_manager->next_use = (p_manager->next_use + 1) % p_manager->record_total;
	MMS_DEBUG(("start_index: %u   next_use: %u\n", p_manager->start_index, p_manager->next_use));
	
	DAL_SemSignal( p_manager->sem);
	return 0;
}

static int mms_data_manager_get_record(mms_data_manager_t* p_manager, int index, char* p_data, unsigned int buf_size, unsigned int* copy_size, unsigned int* p_mms_off)
{
	FILE*			fp = NULL;
	int				iret = 0;
	int				record_index;
	
	if( NULL == p_manager){
		return -1;
	}

	if( 0 != DAL_SemWait_Timeout( p_manager->sem, 2000)){
		DAL_ERROR(("DAL_SemWait_Timeout sem\n"));
		return -1;
	}
	/*
	if( index == 0xFFFFFFFF &&  p_manager->last_copy_index == 0xFFFFFFFF){
		record_index = ( p_manager->last_copy_index + 1) % p_manager->record_total;
		DAL_DEBUG(("1 record_index %u  %x  %x \n", record_index, index, p_manager->last_copy_index));
	}else{
		record_index = index;
		DAL_DEBUG(("2 record_index %u  %x  %x \n", record_index, index, p_manager->last_copy_index));
	}
	*/

	if( index == -1){
		record_index = ( p_manager->last_copy_index + 1) % p_manager->record_total;
	}else{
		record_index = index;
	}

	if( FALSE == p_manager->p_data_record[record_index].valid){
		//DAL_ERROR((" %d block invalid  start_index: %u  next_use: %u  last_copy_index: %u\n", record_index, p_manager->start_index, p_manager->next_use, p_manager->last_copy_index));
		DAL_SemSignal( p_manager->sem);
		return -1;
	}
	if( buf_size < p_manager->p_data_record[record_index].data_size){
		DAL_ERROR((" %u block data_size: %u > buf_size: %u\n", record_index, p_manager->p_data_record[record_index].data_size, buf_size));
		DAL_SemSignal( p_manager->sem);
		return -1;
	}
	fp = fopen(p_manager->p_data_record[record_index].record_name, "r");
	if( NULL == fp){
		DAL_ERROR(("fopen %s  error\n",p_manager->p_data_record[record_index].record_name));
		DAL_SemSignal( p_manager->sem);
		return -1;
	}
	if( p_manager->p_data_record[record_index].data_size != fread(p_data, 1, p_manager->p_data_record[record_index].data_size, fp)){
		DAL_ERROR(("fread error %d\n", p_manager->p_data_record[record_index].data_size));
		DAL_SemSignal( p_manager->sem);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	*copy_size = p_manager->p_data_record[record_index].data_size;
	*p_mms_off = p_manager->p_data_record[record_index].mms_offset;
	p_manager->last_copy_index = record_index;
	DAL_SemSignal( p_manager->sem);
	
	MMS_DEBUG((" get data ok      record_index: %u   mms_off: %u  data_size: %u     \n", record_index, *p_mms_off, *copy_size));
	return 0;
}

static unsigned int mms_data_manager_last_copy_index(mms_data_manager_t* p_manager)
{
	unsigned int	last_copy_index= 0;
	if( NULL == p_manager){
		return -1;
	}
	if( 0 != DAL_SemWait_Timeout( p_manager->sem, 2000)){
		DAL_ERROR(("DAL_SemWait_Timeout sem\n"));
		return -1;
	}
	last_copy_index = p_manager->last_copy_index;
	DAL_SemSignal( p_manager->sem);
	return last_copy_index;
}

#if 0
#endif

#define mms_io_debug(_x_)		{ printf("										[MMS_IO] %s: ", __FUNCTION__); printf _x_ ; }

static int dal_mms_io_select_func(void *data, int fd, int state, int timeout_msec)
{
	DAL_ERROR(("we don't need the select now!!!\n"));return 0;
}
static int dal_mms_io_tcp_connect_func(void *data, const char *host, int port)
{
  struct addrinfo *r, *res, hints;
  char port_str[16];
  int i, s;
  int flags,ret,select_times = 0;
  fd_set fdr, fdw;
  struct timeval timeout;
  dal_mms_control_t*	p_control = (dal_mms_control_t*)data;

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_ADDRCONFIG | AI_NUMERICSERV;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  sprintf(port_str, "%d", port);
  i = getaddrinfo(host, port_str, &hints, &res);
  if (i != 0) {
	mms_io_debug(("unable to resolve host: %s\n", host));
	return -1;
  }
  for (r = res; r != NULL; r = r->ai_next) {
	s = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
	if (s != -1) {
		
	   /* {{{  modify by zyliu , set the connect timeout*/
	   if( (flags = fcntl(s, F_GETFL, 0)) < 0){
		 perror("fcntl F_GETFL: ");
		 freeaddrinfo(res);
		 return -1;
	   }
	   if( (fcntl(s, F_SETFL, flags | O_NONBLOCK)) < 0){
		 perror("fcntl F_SETFL O_NONBLOCK: ");
		 freeaddrinfo(res);
		 close(s);
		 return -1;
	   }
	   if( connect(s, r->ai_addr, r->ai_addrlen) != 0){
		 if( errno != EINPROGRESS){
		   perror("connect:  ");
		   freeaddrinfo(res);
		   close(s);
		   return -1;
		 }
	   }

	   #if 0
	   FD_ZERO(&fdr);
	   FD_ZERO(&fdw);
	   FD_SET(s,&fdr);
	   FD_SET(s,&fdw);
	   timeout.tv_sec = MMS_CONNECT_TIMEOUT;
	   timeout.tv_usec = 0;
	   mms_io_debug(("connect select ......\n"));
	   ret = select(s + 1, &fdr, &fdw, NULL, &timeout);
	   if(ret < 0) {
		 mms_io_debug(("connected failed 1\n"));
		 perror("select: ");
		  freeaddrinfo(res);
		 close(s);
		 return -1;
	   }
	   if(ret == 0) {
		 mms_io_debug(("connected failed 2\n"));
		 perror("select: ");
		  freeaddrinfo(res);
		 close(s);
		 return -1;
	   }
	   if(ret == 1) {
		 if( FD_ISSET(s, &fdw)){
		   mms_io_debug(("connected\n"));
		   freeaddrinfo(res);
		   return s;
		 }
	   }
	   #else
	   
	   select_times = 0;
		while( select_times < MMS_CONNECT_TIMEOUT){

			if( p_control->connect_interrupt_flag == 1){
				mms_io_debug(("user interrpt connect\n"));
				break;
			}
			
			FD_ZERO(&fdr);
			FD_ZERO(&fdw);
			FD_SET(s,&fdr);
			FD_SET(s,&fdw);
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;
			mms_io_debug(("connect select 1 second   %d  times\n", select_times));
			ret = select(s + 1, &fdr, &fdw, NULL, &timeout);
			if(ret < 0) {
			  //mms_io_debug(("connected failed 1\n"));
			  perror("select: ");
			  select_times++;
			  continue;
			}
			if(ret == 0) {
			  //mms_io_debug(("connected failed 2\n"));
			  perror("select: ");
			  select_times++;
				continue;
			}
			//if(ret == 1) {
			else{
			  if( FD_ISSET(s, &fdw)){
				mms_io_debug(("connected  socket: %d\n", s));
				freeaddrinfo(res);
				return s;
			  }
			  else{
				  mms_io_debug(("connected failed 3\n"));
				  perror("select: ");
				  select_times++;
				  continue;
			  }
			}
		}
		
		mms_io_debug(("connect to server failed!!\n"));
		freeaddrinfo(res);
		close(s);
		return -1;

	   #endif
	   
	}
  }
  freeaddrinfo(res);
  return -1;
}
static off_t dal_mms_io_write_func(void *data, int socket, char *buf, off_t num)
{
	int		iret = 0;

	mms_io_debug(("socket: %d  num: %lu  siezof(mms_off_t): %u\n",socket,num,sizeof(off_t)));
	iret = (off_t)write(socket, buf, num);
	mms_io_debug(("iret: %d socket: %d  num: %lu \n",iret,socket,num));
	return iret;
}
static off_t dal_mms_io_read_func(void *data, int socket, char *buf, off_t num)
{
  off_t len = 0, read_ret;
  fd_set 			set;
  int 				iret = 0;
  int 				i = 0;
  struct timeval 	tv;
  dal_mms_control_t*	p_control = (dal_mms_control_t*)data;
  static unsigned int  timeout_count = 0;

  set_errno(0);
  while (len < num)
  {
  	for( i = 0; i < MMS_READ_TIMEOUT; i++){
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		FD_ZERO(&set);
		FD_SET(socket, &set);
		iret = select(socket+1, &set, NULL, NULL, &tv);
		if( iret > 0){
			break;
		}
		if( 0 == p_control->is_in_connect 
			&& 1 == p_control->read_thread_exit_flag){
			mms_io_debug(("not in connect and 1 == p_control->read_thread_exit_flag, should return\n"));
			return 0;
		}
	}
	if( i >= MMS_READ_TIMEOUT){
		mms_io_debug(("select times >= %d \n",MMS_READ_TIMEOUT));
		return -1;
	}

    if( iret > 0 && FD_ISSET(socket,&set)){
      read_ret = (off_t)read(socket, buf + len, num - len);
	  if( timeout_count != 0 && NULL != p_control->event_notify){
	  	mms_io_debug(("data resume  !!!\n"));
		timeout_count = 0;
		p_control->event_notify(MMS_DATA_RECEIVE_RESUME, NULL);
	  }
    }else{
      if( 1 == p_control->is_in_connect){
	  	/*if it's in mms_connect operate to read header data,return eof,
	  		because some mms link can't return eof when read to header eof*/
	  	mms_io_debug(("is_in_connect == 1, return 0\n"));
	  	return 0;
	  }
    }
    if(read_ret == 0){
		mms_io_debug(("select timeout !!!!  %d  times\n",++timeout_count));
		if( timeout_count == 1 &&  0 == p_control->is_in_connect && NULL != p_control->event_notify ){
			p_control->event_notify(MMS_DATA_RECEIVE_TIMEOUT, (void*)timeout_count);
		}
		break; /* EOF */
	}
      
    if(read_ret < 0) {
      mms_io_debug(("read error @ len = %lld: %s\n", (long long int) len, strerror(get_errno())));
      switch(get_errno())
      {
          case EAGAIN:
            continue;
          default:
            /* if already read something, return it, we will fail next time */
            return len ? len : read_ret; 
      }
    }
    len += read_ret;
  }
  return len;	
}

#if 0
#endif
static void dal_mms_connect_debug( dal_mms_control_t* p_control)
{
	if( NULL == p_control )
	{
		DAL_ERROR(("p_control is null\n"))
		return ;
	}
	if( NULL != p_control->mmsh_handle)
	{
		DAL_PRINT(("--------------------------------------------------------------------------------\n"));
		DAL_PRINT(("						mmsh  connect  success  \n"))
		DAL_PRINT(("--------------------------------------------------------------------------------\n"));
		DAL_DEBUG(("##### bandwidth        #### %d\n",p_control->mmsh_handle->bandwidth));
		DAL_DEBUG(("##### current_pos      #### %d\n",(int)p_control->mmsh_handle->current_pos));
		DAL_DEBUG(("##### stream type      #### %d\n",p_control->mmsh_handle->stream_type));
		DAL_DEBUG(("##### chunk length     ####	%d\n",(int)p_control->mmsh_handle->chunk_length));
		DAL_DEBUG(("##### chunk seqNum     ####	%d\n",p_control->mmsh_handle->chunk_seq_number));
		DAL_DEBUG(("##### asf num packets  #### %llu\n",p_control->mmsh_handle->asf_num_packets));
		DAL_DEBUG(("##### chunk type       #### 0x%x\n",p_control->mmsh_handle->chunk_type));
		DAL_DEBUG(("##### seekable         #### %d\n",(int)p_control->mmsh_handle->seekable));
		DAL_DEBUG(("##### time length      #### %llu\n",p_control->mmsh_handle->time_len));
		DAL_DEBUG(("##### uri              #### %s	\n",p_control->mmsh_handle->uri));
		DAL_DEBUG(("##### url              #### %s	\n",p_control->mmsh_handle->url));
		DAL_DEBUG(("##### proto            #### %s  \n",p_control->mmsh_handle->proto));
		DAL_DEBUG(("##### num stream ids   #### %d\n",p_control->mmsh_handle->num_stream_ids));
		DAL_DEBUG(("##### file time        ####	%llu\n",p_control->mmsh_handle->file_time));
		DAL_DEBUG(("##### file length      #### %lld\n",p_control->mmsh_handle->file_len));
		DAL_DEBUG(("##### guid             #### %s\n",p_control->mmsh_handle->guid));
		DAL_DEBUG(("##### has audio        #### %d\n",p_control->mmsh_handle->has_audio));
		DAL_DEBUG(("##### has video        #### %d\n",p_control->mmsh_handle->has_video));
		DAL_DEBUG(("##### http host        ####	%s \n",p_control->mmsh_handle->http_host));
		DAL_DEBUG(("##### http prot        ####	%d\n",p_control->mmsh_handle->http_port));
		DAL_DEBUG(("##### proxy url        ####	%s \n",p_control->mmsh_handle->proxy_url));
		DAL_DEBUG(("##### proxy password   #### %s\n",p_control->mmsh_handle->proxy_password));
	    DAL_DEBUG(("##### asf_header_len   #### %u\n",p_control->mmsh_handle->asf_header_len));
	
		DAL_DEBUG(("mmsx_get_seekable return %d\n",mmsx_get_seekable(p_control->mmsx_handle))); 
		DAL_DEBUG(("mmsx_get_length return %u\n",mmsx_get_length(p_control->mmsx_handle))); 
		DAL_DEBUG(("mmsx_get_file_time return %ld\n",(off_t)mmsx_get_file_time(p_control->mmsx_handle))); 
		DAL_DEBUG(("mmsx_get_asf_header_len return %ld\n",(off_t)mmsx_get_asf_header_len(p_control->mmsx_handle))); 
		DAL_DEBUG(("mmsx_get_asf_packet_len return %ld\n",(off_t)mmsx_get_asf_packet_len(p_control->mmsx_handle)));
	}
	else if( NULL != p_control->mms_handle)
	{
		DAL_PRINT(("--------------------------------------------------------------------------------\n"));
		DAL_PRINT(("						mms  connect  success  \n"))
		DAL_PRINT(("--------------------------------------------------------------------------------\n"));

		DAL_DEBUG(("##### guri        #### %s\n",p_control->mms_handle->guri));
		DAL_DEBUG(("##### url         #### %s\n",p_control->mms_handle->url));
		DAL_DEBUG(("##### proto      	  #### %s\n",p_control->mms_handle->proto));
		DAL_DEBUG(("##### connect_host	  #### %s\n",p_control->mms_handle->connect_host));
		DAL_DEBUG(("##### connect_port    #### %d\n",p_control->mms_handle->connect_port));
		DAL_DEBUG(("##### user   		  #### %s\n",p_control->mms_handle->user));
		DAL_DEBUG(("##### password        #### %s\n",p_control->mms_handle->password));
		DAL_DEBUG(("##### buf_size        #### %d\n",p_control->mms_handle->buf_size));
		DAL_DEBUG(("##### buf_read        #### %d\n",p_control->mms_handle->buf_read));
		DAL_DEBUG(("##### buf_packet_seq_offset #### %lu\n",p_control->mms_handle->buf_packet_seq_offset));

		//
		DAL_DEBUG(("##### asf_header_len  #### %u\n",p_control->mms_handle->asf_header_len));
		DAL_DEBUG(("##### asf_header_read #### %u\n",p_control->mms_handle->asf_header_read));
		DAL_DEBUG(("##### num_stream_ids  #### %d\n",p_control->mms_handle->num_stream_ids));
		DAL_DEBUG(("##### packet_id_type  #### %d\n",p_control->mms_handle->packet_id_type));
		DAL_DEBUG(("##### need_discont    #### %d\n",p_control->mms_handle->need_discont));
		DAL_DEBUG(("##### asf_packet_len  #### %u\n",p_control->mms_handle->asf_packet_len));
		DAL_DEBUG(("##### file_time       #### %llu\n",p_control->mms_handle->file_time));
		DAL_DEBUG(("##### file_len        #### %lld\n",p_control->mms_handle->file_len));
		DAL_DEBUG(("##### time_len   	  #### %lld\n",p_control->mms_handle->time_len));
		DAL_DEBUG(("##### preroll		  #### %lld\n",p_control->mms_handle->preroll));
		DAL_DEBUG(("##### asf_num_packets #### %lld\n",p_control->mms_handle->asf_num_packets));
		DAL_DEBUG(("##### guid            #### %s\n",p_control->mms_handle->guid));
		DAL_DEBUG(("##### bandwidth 	  #### %d\n",p_control->mms_handle->bandwidth));
		DAL_DEBUG(("##### has_audio 	  #### %d\n",p_control->mms_handle->has_audio));
		DAL_DEBUG(("##### has_video 	  #### %d\n",p_control->mms_handle->has_video));
		DAL_DEBUG(("##### seekable 	      #### %d\n",p_control->mms_handle->seekable));
		DAL_DEBUG(("##### current_pos     #### %lu\n",p_control->mms_handle->current_pos));
		DAL_DEBUG(("##### eos             #### %lu\n",p_control->mms_handle->eos));
	}
	printf("\n\n");
}


#if 0
#endif



