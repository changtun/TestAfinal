/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : 
* Description : 
* Author :
* History :
*	2013/5/11 : Initial Version
******************************************************************************/
#ifndef __DAL_HLS_H
#define __DAL_HLS_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************/
/*              Includes											*/
/*******************************************************/

/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/

/*******************************************************/
/*              Exported Types			                */
/*******************************************************/



#define HTTP_HEADER_TAG				"HTTP/1."

#define URL_TAG 					"Location:"
#define M3U8_TAG 					"#EXTM3U"/*m3u8标示*/
#define M3U8_CATCHE					"#EXT-X-ALLOW-CACHE:"/*是否可以缓存下载*/
#define	M3U8_TARGETDURATION 		"#EXT-X-TARGETDURATION:"/*大致播放时长seconds*/
#define	M3U8_SEQUENCE		 		"#EXT-X-MEDIA-SEQUENCE:"/*文件序列号*/
#define	M3U8_STREAMINFO				"#EXT-X-STREAM-INF:"/*流的属性*/
#define	M3U8_KEY					"#EXT-X-KEY:"/*密钥*/
#define	M3U8_DISCONTINUITY			"#EXT-X-DISCONTINUITY"/*表征其后的视频文件和之前的不连续，这意味着文件格式，时间戳顺序，编码参数等的变化*/
#define	M3U8_EXTINF					"#EXTINF:"
#define	M3U8_ENDLIST				"#EXT-X-ENDLIST"/*是否有更多的媒体文件*/
#define	PROGRAM_ID					"PROGRAM-ID"
#define	PROGRAM_BANDWIDTH			"BANDWIDTH"
#define	PROGRAM_CODECS				"CODECS"
#define	HTTP_URL					"http:"


#define DAL_M3U8_TASK_STACK_SIZE		1024*8
#define DAL_M3U8_TASK_PRIOITY			5
#define DAL_M3U8_QUEUE_DEPTH			20
#define DAL_M3U8_QUEUE_DATA_SIZE		DAL_MESSAGE_SIZE

#define P2P_M3U8_DATA_CHUNK_SIZE		188*128		//23.5 k

typedef enum M3u8_Element_Type{
	eM3U8_ELEMENT_TAG,
	eM3U8_ELEMENT_CATCHE,	
	eM3U8_ELEMENT_TARGETDURATION,
	eM3U8_ELEMENT_SEQUENCE,
	eM3U8_ELEMENT_STREAMINFO,
	eM3U8_ELEMENT_KEY,
	eM3U8_ELEMENT_DISCONTINUITY,
	eM3U8_ELEMENT_EXTINF,
	eM3U8_ELEMENT_ENDLIST,
	eM3U8_ELEMENT_HTTP,
	eM3U8_ELEMENT_UNKNOW,
}M3u8_Element_Type;

typedef struct M3U8_struct_s{
	struct pvlist_head tM3U8_HEADER;
	U32	total_num;			/*ts stream total num*/
	U8 	catche;				/*0:不可以缓存，1:可以缓存*/
	U8	live_state;			/*0:live stream; 1:no live stream*/	
	U8  discontinuity;			/*1:continuity  stream; 0:no continuity stream*/	
	U32	duration;
	U32 sequence;
	U32	xkey;
	U32	program_id[10];
	U32	bandwidth[10];	
	U32 total_time;
	DAL_Sem_t sem_request;
}M3U8_struct_t;


typedef struct M3u8_Addr_Node_s{
	struct 	pvlist_head tNode;
	U32		segement_info;
	C8		http_address[256];
}M3u8_Addr_Node_t;

typedef struct Current_hls_s{
	U32 	segement_info;
	C8 	http_address[256];
}Current_hls_t;

typedef struct M3u8_address_s{
C8 	Url[256];
}M3u8_address_t;

typedef enum M3u8_down_Status_e
{
	M3U8DOWN_IDEL,
	M3U8DOWN_STOP,
	M3U8DOWN_PAUSE,
	M3U8DOWN_START,
}M3u8_down_Status_t;


typedef struct DAL_M3u8Control_s
{
	M3u8_down_Status_t		status;
	DAL_TaskID_t			taskID;
	DAL_QueueID_t			queueID;
	ring_buffer_t*			p_download_buffer;
}DAL_M3u8Control_t;
U32 Get_HLS_file_info(void);
S32 DAL_M3u8TaskCreate(void);
S32 DAL_M3u8DownloadStart(M3u8_address_t* param);
DAL_QueueID_t DAL_GetM3u8QueueID(void);
void Set_Http_header_addr(C8* cstring);
void Set_Http_File_addr(C8* cstring);



#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __XXXXXXX_H */
/* End of __DAL_hls_H.h  --------------------------------------------------------- */


