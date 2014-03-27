/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : 
* Description : 
* Author : zyliu
* History :
*	2011/4/11 : Initial Version
******************************************************************************/
#ifndef __DAL_MESSAGE_H
#define __DAL_MESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************/
/*              Includes											*/
/*******************************************************/

/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/
#define DAL_MESSAGE_SIZE		sizeof(DAL_Message_t*)
/*******************************************************/
/*              Exported Types			                */
/*******************************************************/
typedef struct MsgData_DownloadStart_s
{
	char					url[256];
}MsgData_DownloadStart_t;

typedef struct MsgData_StreamParam_s
{
	DAL_Player_Input_t		input;
	DAL_Player_Output_t		output;
}MsgData_StreamParam_t;

typedef struct MsgData_PidsParam_s
{
	unsigned int			PidsNum;
	DAL_StreamData_t		Pids[MAX_STREAM_PIDS];
}MsgData_PidsParam_t;

typedef struct MsgData_DataRequest_s
{
	unsigned char*			p_parse_buffer;
	unsigned int			parse_buf_size;
}MsgData_DataRequest_t;

typedef struct MsgData_Common_s
{
	U32			param1;
	U32			param2;
}MsgData_Common_t;

typedef enum DAL_Msg_Type_e
{
	/*MP --> DAL*/
	DALMSG_STREAM_START = 0,
	DALMSG_STREAM_RESTART,
	DALMSG_STREAM_STOP,
	DALMSG_STREAM_PAUSE,
	DALMSG_STREAM_RESUME,
	DALMSG_STREAM_SETSPEED,
	DALMSG_STREAM_SEEK,
	DALMSG_STREAM_SEEK_PREV,
	DALMSG_STREAM_SEEK_NEXT,
	DALMSG_STREAM_GETINFO,
	DALMSG_STREAM_NEXT,

	DALMSG_STREAM_PVRSTART = 11,
	DALMSG_STREAM_PVRSTOP,
	DALMSG_STREAM_TIMESHIFT_START_RECORD,
	DALMSG_STREAM_TIMESHIFT_START_PLAY,
	DALMSG_STREAM_TIMESHIFT_STOP,
	
	/*DAL --> Download*/
	DALMSG_DOWNLOAD_START,
	DALMSG_DOWNLOAD_STOP,
	/*Download --> DAL*/
	/*Download --> Download*/
	DALMSG_CURL_ERROR,
	DALMSG_CURL_REV_TIMEOUT,
	DALMSG_CURL_REV_RESUME,
	DALMSG_CHECK_PARSE_DATA_READY,
//#if defined(USE_DAL_MULTICAST)
	//{{{ add by LYN, for multicast , 2011-08-22
	DALMSG_MULTICAST_START,
	DALMSG_MULTICAST_STOP,
	DALMSG_MULTICAST_ERROR,
	DALMSG_MULTICAST_REV_TIMEOUT,
	DALMSG_MULTICAST_REV_RESUME,
	//}}} add by LYN, for multicast , 2011-08-22
//#endif	
	/*DAL --> INJECT*/
	DALMSG_INJECT_START,
	DALMSG_INJECT_CONT,
	DALMSG_INJECT_STOP,
	/*INJECT --> DAL*/

	/*DAL --> PARSE*/
	DALMSG_PARSE_START,
	DALMSG_PARSE_CONT,
	DALMSG_PARSE_STOP,
	/*PARSE --> DAL*/
	DALMSG_NEW_PIDS,
	DALMSG_PIDS_CHANGED,

	/* PARSE --> Download*/
	DALMSG_DATA_REQUEST,
	/* Download --> PARSE*/
	DALMSG_PARSE_DATA_READY,

	/*download --> core*/
	DALMSG_DATA_TIMEOUT,
	DALMSG_DATA_RESUME,

	/*inject --> core*/
	DALMSG_FIRST_FRAME_DISPLAY,
	DALMSG_CHECK_FIRST_FRAME,

	DALMSG_HDD_READ_CONT,
#ifdef FLV
	DALMSG_FLVLIST_PARSE_OK,
	DALMSG_FLVLIST_PARSE_ERROR,
	DALMSG_FLVLIST_PARSE_ERROR_LATER_RETRY,
	DALMSG_FLVLIST_PARSE_LATER_OK,
	DALMSG_FLVLIST_SEEK_ACTION_DONE,
#endif
	DALMSG_DVB_CHECK_FRIST_FRAME,
	DALMSG_P2S_RECOVERY,
	
	/*HLS*/

	DALMSG_M3U8_ADDRESSDOWNLOAD_START,
	DALMSG_M3U8_TSDOWNLOAD_START,
	DALMSG_M3U8_TSDOWNLOAD,
	DALMSG_M3U8_TSSTOP,
	DALMSG_M3U8_TSDOWNLOAD_STOP,	
	DALMSG_FIRST_M3U8PARSE_START,
	DALMSG_SECOND_M3U8PARSE_START,
	DALMSG_M3U8_REQUEST_STOP,
	DALMSG_PARSE_TS_PREPARE,
	DALMSG_P2V_SEEK_PREV,
	DALMSG_P2V_SEEK_NEXT,
	DALMSG_HLS_SEEK_PREV,
	DALMSG_HLS_SEEK_NEXT,
	DALMSG_M3U8_RESUME_PARSE,
	
//timeshift
	DALMSG_TIMESHIFT_TSDOWNLOAD_START,
	DALMSG_TIMESHIFT_TSDOWNLOAD_STOP,
	DALMSG_TIMESHIFT_DOWNLOAD,
	DALMSG_TIMESHIFT_STOP,
	DALMSG_TIMESHIFT_SEEK_NEXT,
	DALMSG_TIMESHIFT_SEEK_PREV,

	DALMSG_FLUSH_RINGBUFFER,


	
	DALMSG_WIAT_RETRY_FLVPLAY,
	DALMSG_Total,
}DAL_Msg_Type_t;

typedef union DAL_Msg_Data_u
{
	MsgData_Common_t			Common;
	MsgData_StreamParam_t		StreamParam;
	MsgData_PidsParam_t			PidsParam;
	MsgData_DownloadStart_t		DownloadStart;
	MsgData_DataRequest_t		DataRequest;
}DAL_Msg_Data_t;

typedef struct DAL_Message_s
{
	DAL_Msg_Type_t		MsgType;
	DAL_Msg_Data_t		MsgData;
}DAL_Message_t;

/*
typedef void (*DAL_MsgProc_t)(DAL_Message_t*);
typedef struct DAL_MsgCallTab_s
{
	DAL_Msg_Type_t		MsgType;
	DAL_MsgProc_t		MsgCall;
}DAL_MsgCallTab_t;
*/

/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/


int DAL_MessageInit(void);
int DAL_MessageRelease(void);

DAL_Message_t* DAL_MessageAlloc( void);
int DAL_MessageFree( DAL_Message_t* pMsg);
int DAL_MessageSend( DAL_QueueID_t queueID, DAL_Message_t** ppMsg);


char* DAL_MessageString( DAL_Msg_Type_t type);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __XXXXXXX_H */
/* End of __DAL_MESSAGE_H.h  --------------------------------------------------------- */


