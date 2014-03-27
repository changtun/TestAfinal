#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "osapi.h"
#include "pvddefs.h"
#include <curl.h>
#include <types.h>
#include <easy.h>

#include "ring_buffer.h"
#include "dal_wrap.h"
#include "dal_common.h"
#include "dal_message.h"
#include "dal.h"

#include "dal_core.h"
#include "dal_download.h"
#include "dal_parse.h"

#ifdef USE_ST_PLAYREC
#include "playrec.h"
#include "drv_filter.h"
#include "stpti4.h"

#endif


/* Enum to avoid to filter some particular fields */
/* ---------------------------------------------- */
#define PLAYRECi_STTAPI_FILTER_DONT_CARE 0x80000000
#define PLAYRECi_STTAPI_FILTER_AUTOMATIC 0x40000000
/* Table buffer size */
/* ----------------- */
#define PLAYRECi_TAPI_MAX_BUFFER_SIZE 	256*1024

#define DAL_PARSE_TASK_STACK_SIZE		1024*32
#define DAL_PARSE_TASK_PRIOITY			5
#define DAL_PARSE_QUEUE_DEPTH			20
#define DAL_PARSE_QUEUE_DATA_SIZE		DAL_MESSAGE_SIZE

#define DAL_PARSE_BUFFER_SIZE			1024*128//1024*64


#define LOOP_PIDS_CHECK					(1)				// 是否在首次播放成功之后定期检测PIDS
//#ifdef LOOP_PIDS_CHECK
//#undef LOOP_PIDS_CHECK
//#endif


typedef enum ParseStatus_e
{
	PARSE_IDEL,
	PARSE_STOP,
	PARSE_START,
	PARSE_WAIT_DATA,
	PARSE_WAIT_REPARSE,
}ParseStatus_t;

typedef enum
{
 PLAYRECi_STTAPI_STREAM_RESERVED         = 0x00,
 PLAYRECi_STTAPI_STREAM_VIDEO_MPEG1      = 0x01,
 PLAYRECi_STTAPI_STREAM_VIDEO_MPEG2      = 0x02,
 PLAYRECi_STTAPI_STREAM_AUDIO_MPEG1      = 0x03,
 PLAYRECi_STTAPI_STREAM_AUDIO_MPEG2      = 0x04,
 PLAYRECi_STTAPI_STREAM_PRIVATE_SECTIONS = 0x05,
 PLAYRECi_STTAPI_STREAM_PRIVATE_PES      = 0x06,
 PLAYRECi_STTAPI_STREAM_MHEG             = 0x07,
 PLAYRECi_STTAPI_STREAM_DSM_CC           = 0x08,
 PLAYRECi_STTAPI_STREAM_TYPE_H2221       = 0x09,
 PLAYRECi_STTAPI_STREAM_TYPE_A           = 0x0A,
 PLAYRECi_STTAPI_STREAM_TYPE_B           = 0x0B,
 PLAYRECi_STTAPI_STREAM_TYPE_C           = 0x0C,
 PLAYRECi_STTAPI_STREAM_TYPE_D           = 0x0D,
 PLAYRECi_STTAPI_STREAM_TYPE_AUX         = 0x0E,
 PLAYRECi_STTAPI_STREAM_AUDIO_AAC_ADTS   = 0x0F,
 PLAYRECi_STTAPI_STREAM_AUDIO_AAC_LATM   = 0x11,
 PLAYRECi_STTAPI_STREAM_AUDIO_AAC_RAW1   = 0x12,
 PLAYRECi_STTAPI_STREAM_AUDIO_AAC_RAW2   = 0x13,
 PLAYRECi_STTAPI_STREAM_VIDEO_MPEG4      = 0x1B,
 PLAYRECi_STTAPI_STREAM_VIDEO_AVS        = 0x42,
 PLAYRECi_STTAPI_STREAM_AUDIO_LPCM       = 0x80,
 PLAYRECi_STTAPI_STREAM_AUDIO_AC3        = 0x81,
 PLAYRECi_STTAPI_STREAM_AUDIO_DTS        = 0x82,
 PLAYRECi_STTAPI_STREAM_AUDIO_MLP        = 0x83,
 PLAYRECi_STTAPI_STREAM_AUDIO_DDPLUS     = 0x84,
 PLAYRECi_STTAPI_STREAM_AUDIO_DTSHD      = 0x85,
 PLAYRECi_STTAPI_STREAM_AUDIO_DTSHD_XLL  = 0x86,
 PLAYRECi_STTAPI_STREAM_AUDIO_DDPLUS_2   = 0xA1,
 PLAYRECi_STTAPI_STREAM_AUDIO_DTSHD_2    = 0xA2,
 PLAYRECi_STTAPI_STREAM_VIDEO_VC1        = 0xEA,
 PLAYRECi_STTAPI_STREAM_AUDIO_WMA        = 0xE6
} PLAYRECi_STTAPI_StreamType_t;

typedef enum
{
 PLAYREC_STREAMTYPE_INVALID     =   0, /* None     : Invalid type                         */
 PLAYREC_STREAMTYPE_MP1V        =   1, /* Video    : MPEG1                                */
 PLAYREC_STREAMTYPE_MP2V        =   2, /* Video    : MPEG2                                */
 PLAYREC_STREAMTYPE_MP4V        =   3, /* Video    : H264                                 */
 PLAYREC_STREAMTYPE_MP1A        =   4, /* Audio    : MPEG 1 Layer I                       */
 PLAYREC_STREAMTYPE_MP2A        =   5, /* Audio    : MPEG 1 Layer II                      */
 PLAYREC_STREAMTYPE_MP4A        =   6, /* Audio    : like HEAAC,Decoder LOAS / LATM - AAC */
 PLAYREC_STREAMTYPE_TTXT        =   7, /* Teletext : Teletext pid                         */
 PLAYREC_STREAMTYPE_SUBT        =   8, /* Subtitle : Subtitle pid                         */
 PLAYREC_STREAMTYPE_PCR         =   9, /* Synchro  : PCR pid                              */
 PLAYREC_STREAMTYPE_AC3         =  10, /* Audio    : AC3                                  */
 PLAYREC_STREAMTYPE_H264        =  11, /* Video    : H264                                 */
 PLAYREC_STREAMTYPE_MPEG4P2     =  12, /* Video    : MPEG4 Part II                        */
 PLAYREC_STREAMTYPE_VC1         =  13, /* Video    : Decode Simple/Main/Advanced profile  */
 PLAYREC_STREAMTYPE_AAC         =  14, /* Audio    : Decode ADTS - AAC                    */
 PLAYREC_STREAMTYPE_HEAAC       =  15, /* Audio    : Decoder LOAS / LATM - AAC            */
 PLAYREC_STREAMTYPE_WMA         =  16, /* Audio    : WMA,WMAPRO                           */
 PLAYREC_STREAMTYPE_DDPLUS      =  17, /* Audio    : DD+ Dolby digital                    */
 PLAYREC_STREAMTYPE_DTS         =  18, /* Audio    : DTS                                  */
 PLAYREC_STREAMTYPE_MMV         =  19, /* Video    : Multimedia content                   */
 PLAYREC_STREAMTYPE_MMA         =  20, /* Audio    : Multimedia content                   */
 PLAYREC_STREAMTYPE_AVS         =  21, /* Video    : AVS Video format                     */
 PLAYREC_STREAMTYPE_MP1A_AD     =  22, /* AudioDes : MPEG 1 Layer I                       */
 PLAYREC_STREAMTYPE_MP2A_AD     =  23, /* AudioDes : MPEG 1 Layer II                      */
 PLAYREC_STREAMTYPE_AC3_AD      =  24, /* AudioDes : AC3                                  */
 PLAYREC_STREAMTYPE_HEAAC_AD    =  25, /* AudioDes : HEAAC                                */
 PLAYREC_STREAMTYPE_LPCM        =  26, /* Audio    : LPCM                                 */
 PLAYREC_STREAMTYPE_MP1A_AUX    =  27, /* AuxAudio : MPEG 1 Layer I                       */
 PLAYREC_STREAMTYPE_MP2A_AUX    =  28, /* AuxAudio : MPEG 1 Layer II                      */
 PLAYREC_STREAMTYPE_MP4A_AUX    =  29, /* AuxAudio : like HEAAC,Decoder LOAS / LATM - AAC */
 PLAYREC_STREAMTYPE_AC3_AUX     =  30, /* AuxAudio : AC3                                  */
 PLAYREC_STREAMTYPE_AAC_AUX     =  31, /* AuxAudio : Decode ADTS - AAC                    */
 PLAYREC_STREAMTYPE_HEAAC_AUX   =  32, /* AuxAudio : Decoder LOAS / LATM - AAC            */
 PLAYREC_STREAMTYPE_WMA_AUX     =  33, /* AuxAudio : WMA,WMAPRO                           */
 PLAYREC_STREAMTYPE_DDPLUS_AUX  =  34, /* AuxAudio : DD+ Dolby digital                    */
 PLAYREC_STREAMTYPE_DTS_AUX     =  35, /* AuxAudio : DTS                                  */
 PLAYREC_STREAMTYPE_DDPULSE     =  36, /* Audio    : Dolby PULSE                          */
 PLAYREC_STREAMTYPE_DDPLUS_AD   =  37, /* AudioDes : e-AC3                                */
 PLAYREC_STREAMTYPE_DDPULSE_AD  =  38, /* AudioDes : Dolby Pulse                          */
 PLAYREC_STREAMTYPE_DDPULSE_AUX =  39, /* AuxAudio : Dolby Pulse                          */
 PLAYREC_STREAMTYPE_LPCM_AUX    =  40, /* AuxAudio : LPCM                                 */
 PLAYREC_STREAMTYPE_AUD_ID      =  41, /* Audio    : Independent Sub-stream ID            */
 PLAYREC_STREAMTYPE_DRA         =  42, /* Audio    : DRA                                  */
 PLAYREC_STREAMTYPE_DRA_AD      =  43, /* AudioDes : DRA                                  */
 PLAYREC_STREAMTYPE_DRA_AUX     =  44, /* AuxAudio : DRA                                  */
 PLAYREC_STREAMTYPE_DTS_AD      =  45, /* AudioDes : DTS                                  */
 PLAYREC_STREAMTYPE_OTHER       = 255, /* Misc     : Non identified pid                   */
} PLAYREC_StreamType_t;

/* Descriptor Types */
/* ---------------- */
typedef enum
{
 PLAYRECi_STTAPI_DESCRIPTOR_REGISTRATION           = 0x05,
 PLAYRECi_STTAPI_DESCRIPTOR_TARGET_BACKGROUND_GRID = 0x07,
 PLAYRECi_STTAPI_DESCRIPTOR_LANGUAGE_NAME          = 0x0A,
 PLAYRECi_STTAPI_DESCRIPTOR_MPEG4_VIDEO            = 0x1B,
 PLAYRECi_STTAPI_DESCRIPTOR_MPEG4_AUDIO            = 0x1C,
 PLAYRECi_STTAPI_DESCRIPTOR_TELETEXT               = 0x56,
 PLAYRECi_STTAPI_DESCRIPTOR_SUBTITLING             = 0x59,
 PLAYRECi_STTAPI_DESCRIPTOR_AC3                    = 0x6A,
 PLAYRECi_STTAPI_DESCRIPTOR_ENHANCED_AC3           = 0x7A,
 PLAYRECi_STTAPI_DESCRIPTOR_DTS                    = 0x7B,
 PLAYRECi_STTAPI_DESCRIPTOR_AAC                    = 0x7C
} PLAYRECi_STTAPI_DescriptorType_t;

/* Pid type */
/* -------- */
typedef enum
{
 PLAYRECi_PID_UNDEFINED = 0x00,
 PLAYRECi_PID_AUDIO     = 0x01,
 PLAYRECi_PID_VIDEO     = 0x02,
 PLAYRECi_PID_SUBTITLE  = 0x03,
 PLAYRECi_PID_TELETEXT  = 0x04,
 PLAYRECi_PID_PCR       = 0x05
} PLAYRECi_PidType_t;


/* Descriptor Element */
/* ------------------ */
typedef struct
{
 PLAYRECi_STTAPI_DescriptorType_t Tag;
 U8                               Size;
 U8                              *Data;
} PLAYRECi_STTAPI_Descriptor_t;

/* PMT Element */
/* ----------- */
typedef struct
{
 U16                           Pid;
 PLAYRECi_STTAPI_StreamType_t  Type;
 U32                           NbDescriptors;
 PLAYRECi_STTAPI_Descriptor_t *Descriptor;
} PLAYRECi_STTAPI_PMTElement_t;


/* PAT Element */
/* ----------- */
typedef struct
{
 U16 ProgramNumber;
 U16 Pid;
} PLAYRECi_STTAPI_PATElement_t;

/* PAT */
/* --- */
typedef struct
{
 U16                           TransportStreamId;
 U32                           NbElements;
 PLAYRECi_STTAPI_PATElement_t *Element;
} PLAYRECi_STTAPI_PATData_t;

/* PMT */
/* --- */
typedef struct
{
 U16                           PcrPid;
 U16                           ProgramNumber;
 U32                           NbDescriptors;
 PLAYRECi_STTAPI_Descriptor_t *Descriptor;
 U32                           NbElements;
 PLAYRECi_STTAPI_PMTElement_t *Element;
 BOOL                          Scrambled;
} PLAYRECi_STTAPI_PMTData_t;

typedef struct dal_psi_parse_s
{
	dal_parse_buffer_t	parse_buffer;

	int			 		(*read_data)(unsigned char* pbuffer, unsigned int offset, unsigned int get_size);
	void*				(*mem_alloc)(unsigned int size);
	void				(*mem_free)(void* pdata);	
}dal_psi_parse_t;

typedef struct PidParam_s
{
	U32						PidsNum;
	DAL_StreamData_t		Pids[MAX_STREAM_PIDS];
}PidParam_t;

typedef struct DAL_ParseControl_s
{
	ParseStatus_t			status;
	DAL_TaskID_t			taskID;
	DAL_QueueID_t			queueID;

	PidParam_t				cur_pids;
	BOOL					new_parse_flag;
	unsigned int			reparse_time_left;
}DAL_ParseControl_t;


static DAL_ParseControl_t		g_ParseControl;
static dal_psi_parse_t 			g_parse_handle;

unsigned int					g_section_found_read;
extern U8 multicast_flag;


static void* dal_parse_task( void* param);
static void parse_start_process( DAL_Message_t* pMessage);
static void parse_cont_process( DAL_Message_t* pMessage);
static void parse_stop_process( DAL_Message_t* pMessage);
static void parse_data_ready_process( DAL_Message_t* pMessage);
//static void parse_request_data_process( DAL_Message_t* pMessage);
static int  read_data_from_buffer(unsigned char* pbuffer, unsigned int offset, unsigned int get_size);
static int  parse_pids( U32* pPidsNum, DAL_StreamData_t* Pids);
static int PLAYRECi_STTAPI_PAT_Acquire(dal_psi_parse_t* handle,U16 Pid,PLAYRECi_STTAPI_PATData_t *TableData);
static int PLAYRECi_STTAPI_PAT_Delete(dal_psi_parse_t* handle,PLAYRECi_STTAPI_PATData_t *TableData);
static int PLAYRECi_STTAPI_PMT_Acquire(dal_psi_parse_t* handle,U16 Pid,PLAYRECi_STTAPI_PMTData_t *TableData);
static int PLAYRECi_STTAPI_PMT_Delete(dal_psi_parse_t* handle,PLAYRECi_STTAPI_PMTData_t *TableData);
static int PLAYRECi_STTAPI_PMT_GetInfo(PLAYRECi_STTAPI_PMTElement_t *PMTElement,PLAYREC_StreamType_t *StreamType,U16 *Pid,PLAYRECi_PidType_t *PidType,char *StringType);
static int PLAYRECi_STTAPI_DescriptorRegistration(PLAYRECi_STTAPI_Descriptor_t *Descriptor,U8 *FormatIdentifier);
static int PLAYRECi_STTAPI_XXX_AcquireFile(dal_psi_parse_t* handle,U16 Pid,U8 TableId,U32 ProgramNumber,U32 SectionNumber,U32 CurrentNotNext,U8 *Buffer,U32 BufferSize,U32 *ReadSize);



int DAL_ParseTaskCreate( void)
{
	DAL_ParseControl_t*		p_control = &g_ParseControl;
	int						iret = 0;

	iret = DAL_MessageQueueCreate( &(p_control->queueID), "dal_parse_queue", DAL_PARSE_QUEUE_DEPTH, DAL_PARSE_QUEUE_DATA_SIZE, 0);
	if( iret != 0)
	{
		return -1;
	}
	iret = DAL_TaskCreate( &(p_control->taskID), "dal_parse_task", dal_parse_task, NULL, NULL, DAL_PARSE_TASK_STACK_SIZE, DAL_PARSE_TASK_PRIOITY, 0);
	if( iret != 0)
	{
		DAL_MessageQueueDelete( p_control->queueID);
		return -2;
	}

	g_parse_handle.read_data = read_data_from_buffer;
	g_parse_handle.mem_alloc = DAL_Malloc;
	g_parse_handle.mem_free = DAL_Free;

	g_parse_handle.parse_buffer.buf_size = DAL_PARSE_BUFFER_SIZE;
	g_parse_handle.parse_buffer.pbuffer = g_parse_handle.mem_alloc(g_parse_handle.parse_buffer.buf_size);
	if( g_parse_handle.parse_buffer.pbuffer == NULL)
	{
		//DAL_ERROR(("Parse Buffer failed\n"));
		DebugMessage("[DAL]DAL_ParseTaskCreate->Parse Buffer failed!!!\n");
		DAL_MessageQueueDelete( p_control->queueID);
		DAL_TaskDelete(p_control->taskID);
		return -3;
	}
	
	p_control->status = PARSE_STOP;
	return 0;
}

int DAL_ParseTaskDelete( void)
{
	DAL_ParseControl_t*		p_control = &g_ParseControl;
	int 					iret = 0;

	iret = DAL_TaskDelete( p_control->taskID);
	iret = DAL_MessageQueueDelete( p_control->queueID);

	g_parse_handle.mem_free(g_parse_handle.parse_buffer.pbuffer);
	
	p_control->status = PARSE_IDEL;
	return 0;
}

int DAL_ParseStart( void)
{
	DAL_ParseControl_t*		p_control = &g_ParseControl;
	DAL_Message_t*			pMsg = NULL;

	if( p_control->status == PARSE_IDEL)
	{
		//DAL_DEBUG(("PARSE_IDEL\n"));
		DebugMessage("PARSE_IDEL\n");
		return -1;
	}

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_PARSE_START;
	DAL_MessageSend( p_control->queueID, &pMsg);
	return 0;
}

int DAL_ParseStop( void)
{
	DAL_ParseControl_t*		p_control = &g_ParseControl;
	DAL_Message_t*			pMsg = NULL;
	
	if( p_control->status == PARSE_IDEL)
	{
		//DAL_DEBUG(("PARSE_IDEL\n"));
		DebugMessage("[DAL]DAL_ParseStop->PARSE_IDEL\n");
		return -1;
	}	
	p_control->reparse_time_left=0;//nlv hls 停止慢
	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_PARSE_STOP;
	DAL_MessageSend( p_control->queueID, &pMsg);

	while( DAL_ParseIsStop() != TRUE)
	{
		//DAL_DEBUG(("waiting for DAL_ParseStop\n"));
		DebugMessage("[DAL]DAL_ParseStop->waiting for DAL_ParseStop\n");
		DAL_TaskDelay(200);
	}	
	return 0;
}

DAL_QueueID_t DAL_GetParseQueueID(void)
{
	DAL_ParseControl_t*		p_control = &g_ParseControl;

	if( p_control->status == PARSE_IDEL)
	{
		//DAL_DEBUG(("PARSE_IDEL\n"));
		DebugMessage("[DAL]DAL_ParseStop->waiting for DAL_ParseStop\n");
		return -1;
	}

	return p_control->queueID;
}

BOOL DAL_ParseIsStop( void)
{
	DAL_ParseControl_t*		p_control = &g_ParseControl;
	return (p_control->status == PARSE_STOP) ? 1 : 0;
}


static void* dal_parse_task( void* param)
{
	DAL_ParseControl_t*		p_control = &g_ParseControl;
	DAL_Message_t*			pMsg = NULL;
	unsigned int			SizeCopy = 0;
	int						iret = 0;
	
	while(1)
	{
		iret = DAL_MessageQueueReceive( p_control->queueID, &pMsg, sizeof(DAL_Message_t*), &SizeCopy, DAL_PEND);
		if( iret < 0)
		{
			continue;	
		}
		else
		{
		
	//	DebugMessage("[DAL]dal_parse_task-> 							 Recv MSG: %s\n", DAL_MessageString(pMsg->MsgType));
			if( DALMSG_PARSE_CONT != pMsg->MsgType)
			{
			//	DebugMessage("[DAL]dal_parse_task->                              Recv MSG: %s\n", DAL_MessageString(pMsg->MsgType));
				//DAL_DEBUG(("                              Recv MSG: %s\n", DAL_MessageString(pMsg->MsgType)));
			}
			switch(pMsg->MsgType)
			{
				case DALMSG_PARSE_START: parse_start_process(pMsg); break;
				case DALMSG_PARSE_CONT: parse_cont_process(pMsg); break;
				case DALMSG_PARSE_STOP: parse_stop_process(pMsg); break;
				case DALMSG_PARSE_DATA_READY: parse_data_ready_process(pMsg); break;
				//case DALMSG_DATA_REQUEST: parse_request_data_process(pMsg); break;
				default: break;
			}
			DAL_MessageFree(pMsg);
		}
	//	DAL_TaskDelay(10);
	}
	return (void*)0;
}

static void parse_start_process( DAL_Message_t* pMessage)
{
	DAL_ParseControl_t*		p_control = &g_ParseControl;
	DAL_Message_t*			pMsg = NULL;

	if( p_control->status == PARSE_IDEL)
	{
		//DAL_ERROR(("PARSE_IDEL\n"));
		DebugMessage("[DAL]parse_start_process->PARSE_IDEL");
		return ;
	}
		
	p_control->status = PARSE_START;
	p_control->new_parse_flag = 1;
	
	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_DATA_REQUEST;
	pMsg->MsgData.DataRequest.p_parse_buffer = g_parse_handle.parse_buffer.pbuffer;
	pMsg->MsgData.DataRequest.parse_buf_size = g_parse_handle.parse_buffer.buf_size;
	DAL_MessageSend( DAL_GetDownloadQueueID(), &pMsg);
	p_control->status = PARSE_WAIT_DATA;
}

static void parse_cont_process( DAL_Message_t* pMessage)
{
	DAL_ParseControl_t*		p_control = &g_ParseControl;
	DAL_Message_t*			pMsg = NULL;

//	DebugMessage("[DAL]parse_cont_process status->%d",p_control->status);


	if( p_control->status != PARSE_WAIT_REPARSE)
	{
		//DAL_DEBUG(("NOT PARSE_WAIT_REPARSE\n"));
		DebugMessage("[DAL]parse_cont_process->NOT PARSE_WAIT_REPARSE");
		return ;
	}
		
	if( p_control->reparse_time_left == 0)
	{
		/*重新向download 申请数据解析pids*/
		pMsg = DAL_MessageAlloc();
		pMsg->MsgType = DALMSG_DATA_REQUEST;
		pMsg->MsgData.DataRequest.p_parse_buffer = g_parse_handle.parse_buffer.pbuffer;
		pMsg->MsgData.DataRequest.parse_buf_size = g_parse_handle.parse_buffer.buf_size;
		DAL_MessageSend( DAL_GetDownloadQueueID(), &pMsg);
		p_control->status = PARSE_WAIT_DATA;		
	}
	else
	{
		p_control->reparse_time_left--;
		DebugMessage("[DAL]parse_cont_process->NOT reparse_time_left :%d^^^^^^^^^^^^^^^^^^^^^^^",p_control->reparse_time_left);

		DAL_TaskDelay(500);
		pMsg = DAL_MessageAlloc();
		pMsg->MsgType = DALMSG_PARSE_CONT;
		DAL_MessageSend( p_control->queueID, &pMsg);	
	}
}

static void parse_stop_process( DAL_Message_t* pMessage)
{
	DAL_ParseControl_t*		p_control = &g_ParseControl;	
	p_control->status = PARSE_STOP;
}

static void parse_data_ready_process( DAL_Message_t* pMessage)
{

	DAL_ParseControl_t*		p_control = &g_ParseControl;
	U32						PidsNum = 0;
	DAL_StreamData_t		Pids[MAX_STREAM_PIDS];
	DAL_Message_t*			pMsg = NULL;
	BOOL					pids_changed = 0;
	int						iret = 0;
	int						i = 0;

	

	if( p_control->status != PARSE_WAIT_DATA)
	{
		return ;
	}

	iret = parse_pids( &PidsNum, Pids);
	if( iret != 0 || PidsNum == 0)
	{
		//DAL_DEBUG(("parse_pids failed, reparse after 2 seconds\n"));
		DebugMessage("parse_pids failed, reparse after 2 seconds\n");

		p_control->reparse_time_left = 4;
		pMsg = DAL_MessageAlloc();
		pMsg->MsgType = DALMSG_PARSE_CONT;
		DAL_MessageSend( p_control->queueID, &pMsg);

		p_control->status = PARSE_WAIT_REPARSE;
		return ;
	} 

	if( p_control->new_parse_flag == 1)
	{
		DebugMessage("parse_pids success\n");
		//DAL_DEBUG(("parse_pids success\n"));
		for( i=0;i<PidsNum;i++)
		{
			#if 0//nlv	
			{
			STPTI_Descrambler_t DescramblerHandle;
			STPTI_SlotOrPid_t SlotOrPid;
			//char key[16] = {0x93, 0x4a ,0x8a ,0x67 ,0xfa ,0x78 ,0x29 ,0x9b};// ,0x9f ,0x00 ,0x70,0x0f ,0xb1 ,0xdf ,0x67 ,0xf7 };
			
			char key[16] = {0x01, 0x01 ,0x01 ,0x03 ,0x01 ,0x01 ,0x01 ,0x03};// ,0x9f ,0x00 ,0x70,0x0f ,0xb1 ,0xdf ,0x67 ,0xf7 };
			
			STPTI_DescramblerAllocate(PTI_HANDLE,&DescramblerHandle,STPTI_DESCRAMBLER_TYPE_DVB_DESCRAMBLER);
			SlotOrPid.Pid = Pids[i].pid;
			STPTI_DescramblerAssociate(DescramblerHandle,SlotOrPid);
	 		STPTI_DescramblerSet( DescramblerHandle,  STPTI_KEY_PARITY_ODD_PARITY, STPTI_KEY_USAGE_VALID_FOR_ALL,key);
			}
			printf("LYN---------------------------------------\n");
			#endif
			DebugMessage("(%d)   pid: %d	 type: %d\n", i, Pids[i].pid, Pids[i].type);
			//DAL_DEBUG(("(%d)   pid: %d	 type: %d\n", i, Pids[i].pid, Pids[i].type));
		}
		
		pMsg = DAL_MessageAlloc();
		pMsg->MsgType = DALMSG_NEW_PIDS;
		p_control->cur_pids.PidsNum = pMsg->MsgData.PidsParam.PidsNum = PidsNum;
		for( i=0;i<PidsNum;i++)
		{
			p_control->cur_pids.Pids[i].type = pMsg->MsgData.PidsParam.Pids[i].type = Pids[i].type;
			p_control->cur_pids.Pids[i].pid = pMsg->MsgData.PidsParam.Pids[i].pid = Pids[i].pid;
		}
		DAL_MessageSend( DAL_GetCoreQueueID(),&pMsg);
		
		p_control->new_parse_flag = 0;
		p_control->status = PARSE_START;
	}
#if defined(LOOP_PIDS_CHECK)
	else
	{
		DebugMessage("Reparse_pids success\n");
		//DAL_DEBUG(("Reparse_pids success\n"));
		for( i=0;i<PidsNum;i++)
		{
			DebugMessage("(%d)   pid: %d	 type: %d\n", i, Pids[i].pid, Pids[i].type);
			//DAL_DEBUG(("(%d)   pid: %d	 type: %d\n", i, Pids[i].pid, Pids[i].type));
		}

		/*和原先的pids 比较*/
		pids_changed = 0;
		if( PidsNum != p_control->cur_pids.PidsNum)
		{
			pids_changed = 1;
		}
		else
		{
			for( i = 0; i < PidsNum; i++)
			{
				if( p_control->cur_pids.Pids[i].type == Pids[i].type
					&& p_control->cur_pids.Pids[i].pid != Pids[i].pid)//????
				{
					pids_changed = 1;
					break;
				}
			}
		}
		
		/**  测试PIDS CHANGED的情
		{
			static int				pids_changed_test = 0;
			if( ++pids_changed_test == 5)
			{
				pids_changed = 1;
			}
		}
		*/
		
		if( pids_changed == 1)
		{
			//DAL_DEBUG(("PIDS CHANGED !!!\n"));
			DebugMessage("PIDS CHANGED !!!\n");
			pMsg = DAL_MessageAlloc();
			pMsg->MsgType = DALMSG_PIDS_CHANGED;
			p_control->cur_pids.PidsNum = pMsg->MsgData.PidsParam.PidsNum = PidsNum;
			for( i=0;i<PidsNum;i++)
			{
				p_control->cur_pids.Pids[i].type = pMsg->MsgData.PidsParam.Pids[i].type = Pids[i].type;
				p_control->cur_pids.Pids[i].pid = pMsg->MsgData.PidsParam.Pids[i].pid = Pids[i].pid;
			}
			DAL_MessageSend( DAL_GetCoreQueueID(),&pMsg);
		}
		p_control->status = PARSE_START;
	}

	
	/*设置下次重新检测pids 的剩余时间*/
	p_control->reparse_time_left = 20;
	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_PARSE_CONT;
	DAL_MessageSend( p_control->queueID, &pMsg);
	
	p_control->status = PARSE_WAIT_REPARSE;
	
#endif
}

#if 0
static void parse_request_data_process( DAL_Message_t* pMessage)
{
	DAL_ParseControl_t*		p_control = &g_ParseControl;
	DAL_Message_t*			pMsg = NULL;
	unsigned int			readsize = 0;
	FILE*					fp = NULL;
	
	fp = fopen("0.ts","r");
	if( fp == NULL)
	{
		DAL_ERROR(("fopen failed\n"));
		return ;		
	}

	readsize = fread( g_parse_handle.parse_buffer.pbuffer, 1, g_parse_handle.parse_buffer.buf_size, fp);
	if( readsize != g_parse_handle.parse_buffer.buf_size)
	{
		DAL_ERROR(("fread failed\n"));
		fclose(fp);
		return ;
	}
	fclose(fp);

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_PARSE_DATA_READY;
	DAL_MessageSend( p_control->queueID, &pMsg);
	return ;
}
#endif

static int read_data_from_buffer(unsigned char* pbuffer, unsigned int offset, unsigned int get_size)
{
	if( g_parse_handle.parse_buffer.buf_size >= offset + get_size)
	{
		memcpy( pbuffer, g_parse_handle.parse_buffer.pbuffer + offset, get_size);
		g_section_found_read += get_size;
		return get_size;
	}
	else
	{
		return 0;
	}
}

static int parse_pids( U32* pPidsNum, DAL_StreamData_t* Pids)
{


	PLAYRECi_STTAPI_PATData_t		PAT;
	PLAYRECi_STTAPI_PMTData_t 		PMT;
	int 							ErrCode = 0;
	U32                     		i,j,k;
	U32								PidsNum;

	PidsNum = 0;
	for( i=0;i<MAX_STREAM_PIDS;i++)
	{
		Pids[i].type = DAL_STREAMTYPE_INVALID;
		Pids[i].pid = 0;
	}
	g_section_found_read = 0;

	/* Try to identify the pids */
	ErrCode=PLAYRECi_STTAPI_PAT_Acquire(&g_parse_handle,0,&PAT);
	//DAL_DEBUG(("----------------------------PLAYRECi_STTAPI_PAT_Acquire return %d\n", ErrCode));
	if (ErrCode == 0)
	{
		BOOL VID_Found=FALSE;
		BOOL AUD_Found=FALSE;
		/* Look for the program requested into the PAT */
		for (i=0;(i<PAT.NbElements)&&(VID_Found==FALSE)&&(AUD_Found==FALSE);i++)
		{
			/* Get the PMT for this program */
			if (PAT.Element[i].ProgramNumber!=0)
			{
				memset(&PMT,0,sizeof(PLAYRECi_STTAPI_PMTData_t));
				PMT.ProgramNumber=PAT.Element[i].ProgramNumber;
				ErrCode=PLAYRECi_STTAPI_PMT_Acquire(&g_parse_handle,PAT.Element[i].Pid,&PMT);
				if (ErrCode==0)
				{
					/* Get the list of pids */
					for (j=0;j<PMT.NbElements;j++)
					{
						PLAYRECi_PidType_t PidType;
						char               StringType[512+1];

						/* Check if number of pids has reach maximum value */
						if( PidsNum==MAX_STREAM_PIDS)
						{
							//PLAYRECi_TraceERR(("%s(%d):**ERROR** !!! Maximum number of pids detected reached !!!\n",__FUNCTION__,PlayId));
						//	DAL_ERROR(("!!! Maximum number of pids detected reached !!!\n"));
							DebugMessage("[DAL]parse_pids->!!! Maximum number of pids detected reached !!!");

							/* Clean-up all the pids entries */
							for (k=0;k<PidsNum;k++)
							{
								Pids[k].type= DAL_STREAMTYPE_INVALID;
								Pids[k].pid= 0;
							}
							PidsNum=0;
							PLAYRECi_STTAPI_PMT_Delete(&g_parse_handle,&PMT);
							PLAYRECi_STTAPI_PAT_Delete(&g_parse_handle,&PAT);
							return -1;
						}
						
						/* Analysis of each pmt element */
						ErrCode=PLAYRECi_STTAPI_PMT_GetInfo(&PMT.Element[j],(PLAYREC_StreamType_t*)&(Pids[PidsNum].type),&(Pids[PidsNum].pid),&PidType,StringType);
						if (PidType==PLAYRECi_PID_VIDEO    ) VID_Found = TRUE;
						if (PidType==PLAYRECi_PID_AUDIO    ) AUD_Found = TRUE;
						if (PidType!=PLAYRECi_PID_UNDEFINED) PidsNum++;
					}


					/**********/
					#if 0
					/* Add the PCR pid */
					PlayInitParams->Pids[PlayInitParams->PidsNum].Type = PLAYREC_STREAMTYPE_PCR;
					PlayInitParams->Pids[PlayInitParams->PidsNum].Pid  = PMT.PcrPid;
					PlayInitParams->PidsNum++;
					#endif
					/**********/
	
					/* Delete the PMT entry now */
					ErrCode=PLAYRECi_STTAPI_PMT_Delete(&g_parse_handle,&PMT);
					if (ErrCode!=0)
					{
						//PLAYRECi_TraceERR(("%s(%d):**ERROR** !!! Unable to delete the internal PMT found !!!\n",__FUNCTION__,PlayId));
						//DAL_ERROR(("!!! Unable to delete the internal PMT found !!!\n"));
						DebugMessage("[DAL]parse_pids->!!! Unable to delete the internal PMT found !!!");

						/* Clean-up all the pids entries */
						for (k=0;k<PidsNum;k++)
						{
							Pids[k].type= PLAYREC_STREAMTYPE_INVALID;
							Pids[k].pid= 0;
						}
						PidsNum=0;
						PLAYRECi_STTAPI_PAT_Delete(&g_parse_handle,&PAT);
						return -1;
					}
				}
			}
		}
		/* Delete the PAT */
		ErrCode=PLAYRECi_STTAPI_PAT_Delete(&g_parse_handle,&PAT);
		if (ErrCode!=0)
		{
			//PLAYRECi_TraceERR(("%s(%d):**ERROR** !!! Unable to delete the internal PAT found !!!\n",__FUNCTION__,PlayId));
			//DAL_ERROR(("!!! Unable to delete the internal PAT found !!!\n"));
			DebugMessage("[DAL]parse_pids->!!! Unable to delete the internal PAT found !!!");
			/* Clean-up all the pids entries */
			for (k=0;k<PidsNum;k++)
			{
				Pids[k].type= PLAYREC_STREAMTYPE_INVALID;
				Pids[k].pid= 0;
			}
			PidsNum=0;
			return -1;
		}
	}
	else
	{
		return -1;
	}


	*pPidsNum = PidsNum;	
	return 0;
}
//{{{ add by LYN , for parse subtitle , 2011-08-25
static U32 gSubtitle_language[16];
static U16 gSubtitle_composition_pid[16];
static U16 gSubtitle_ancillary_pid[16];
static U8 gSubtitle_flag = 0;
static U16 gSubtitle_pid;
static U8 gSubtNum = 0;

U8 Get_subtNum(void)
{
    return gSubtNum;
}
void Set_SubtPid(U16 pid)
{
    gSubtitle_pid = pid;
}
U16 Get_SubtPid(void)
{
    return gSubtitle_pid;
}
U8 Get_SubFlag(void)
{
    return gSubtitle_flag;
}

void Set_SubFlag(U8 flag)
{
    gSubtitle_flag = flag;
    if(0 == flag)
     gSubtNum = 0;
}

void Set_SubData(U8 loop, U32 language, U16 compPid, U16 anciPid )
{
    gSubtitle_language[loop] = language;
    gSubtitle_composition_pid[loop] = compPid;
    gSubtitle_ancillary_pid[loop] = anciPid;
}

void Get_SubData(U8 loop, U32* language, U16* comPid, U16* anciPid)
{
    *language   = gSubtitle_language[loop];
    *comPid     = gSubtitle_composition_pid[loop];
    *anciPid    = gSubtitle_ancillary_pid[loop];
}
void Parse_SubData(U8* data, U8 length)
{
    U8 i;

    printf("--------------------------------Parse SUBTDATA------%d--\n",length);
    for(i =0 ; i<length;i+=8)
    {
        gSubtitle_language[i%8] = (data[i]<<16)+(data[i+1]<<8)+(data[i+2]);
        gSubtitle_composition_pid[i%8] = (data[i+4]<<8)+(data[i+5]);
        gSubtitle_ancillary_pid[i%8] = (data[i+6]<<8)+(data[i+7]);
        Set_SubFlag(1);
    }
}
//}}} add by LYN , for parse subtitle , 2011-08-25

static int PLAYRECi_STTAPI_PAT_Acquire(dal_psi_parse_t* handle,U16 Pid,PLAYRECi_STTAPI_PATData_t *TableData)
{
 U8            *Buffer;
 U32            BufferSize,ReadSize,NumberOfEntries,SectionLength,i,j;
 
 //ST_ErrorCode_t ErrCode=ST_NO_ERROR;
 int			ErrCode = 0;

 /* Clear the structure */
 /* =================== */
 memset(TableData,0,sizeof(PLAYRECi_STTAPI_PATData_t));





 /*_______________________________________________________________________________________________*/
 /* Allocate buffer to receive the PAT section */
 /* ========================================== */
 BufferSize = PLAYRECi_TAPI_MAX_BUFFER_SIZE;
 #if 0
 Buffer     = (U8 *)PLAYRECi_Context.InitParams.MEM_Allocate(PLAYREC_MEMORY_CACHED,BufferSize,0);
 if (Buffer==NULL)
  {
   TableData->NbElements=0;
   PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to allocate memory !!!\n",__FUNCTION__));
   return(ST_ERROR_NO_MEMORY);
  }
 #endif
 Buffer = handle->mem_alloc(BufferSize);
 if( Buffer==NULL)
 {
	TableData->NbElements=0;
	//DAL_ERROR(("!!! Unable to allocate memory !!!\n"));
	DebugMessage("[DAL]PLAYRECi_STTAPI_PAT_Acquire-> !!! Unable to allocate memory !!!");
	
	return -1;
 }
 /*_______________________________________________________________________________________________*/




 
 /* Filter the complete section */
 /* =========================== */
 ErrCode=PLAYRECi_STTAPI_XXX_AcquireFile(handle,Pid,0/*STTAPI_PAT_TABLE_ID*/,PLAYRECi_STTAPI_FILTER_DONT_CARE,PLAYRECi_STTAPI_FILTER_AUTOMATIC,1,Buffer,BufferSize,&ReadSize);
 if (ErrCode!=0)
  {
   TableData->NbElements=0;
   /*_______________________________________________________________________________________________*/
   /*PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to filter the section !!!\n",__FUNCTION__));*/
   //PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)Buffer);
   //return(ST_ERROR_NO_MEMORY);
   
	//DAL_ERROR(("!!! Unable to filter the section !!!\n"));
   DebugMessage("[DAL]PLAYRECi_STTAPI_PAT_Acquire-> !!! Unable to filter the section !!!");
	handle->mem_free((void*)Buffer);
	//DAL_DEBUG(("-------------------------------will return -1\n"));
	return -1;   
   /*_______________________________________________________________________________________________*/
  }



 /* Parse the complete buffer */
 /* ========================= */

 /* Get the transport stream id */
 /* --------------------------- */
 TableData->TransportStreamId = ((Buffer[3]<<8)|(Buffer[4]));

 /* Get the number of entries */
 /* ------------------------- */
 NumberOfEntries = i = 0;
 while(i<ReadSize)
  {
   SectionLength    = (((Buffer[i+1]&0xF)<<8)|(Buffer[i+2]))+3;
   NumberOfEntries += (SectionLength-12)/4;
   i               += SectionLength;
  }

 /* If the PAT is empty, go out now */
 /* ------------------------------- */
 if (NumberOfEntries==0)
  {
   TableData->NbElements = 0;
   TableData->Element    = NULL;
   /*_______________________________________________________________________________________________*/
   //PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)Buffer);
   //return(ST_NO_ERROR);
   handle->mem_free((void*)Buffer);
   return -1;
   /*_______________________________________________________________________________________________*/
  }

 /*_______________________________________________________________________________________________*/
 /* Allocate the PAT elements */
 /* ------------------------- */
 #if 0
 TableData->Element = (PLAYRECi_STTAPI_PATElement_t *)PLAYRECi_Context.InitParams.MEM_Allocate(PLAYREC_MEMORY_CACHED,NumberOfEntries*sizeof(PLAYRECi_STTAPI_PATElement_t),0);
 if (TableData->Element==NULL)
  {
   TableData->NbElements=0;
   
   PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to allocate memory for elements !!!\n",__FUNCTION__));
   PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)Buffer);
   return(ST_ERROR_NO_MEMORY);

  }
 #endif
 TableData->Element = handle->mem_alloc(NumberOfEntries*sizeof(PLAYRECi_STTAPI_PATElement_t));
 if (TableData->Element==NULL)
 {
	 //DAL_ERROR(("!!! Unable to allocate memory for elements !!!\n"));
	 DebugMessage("[DAL]PLAYRECi_STTAPI_PAT_Acquire-> !!! Unable to allocate memory for elements !!!");
	 handle->mem_free((void*)Buffer);
	 return -1;
 }
 /*_______________________________________________________________________________________________*/




 /* Fill up the elements */
 /* -------------------- */
 NumberOfEntries = i = 0;
 while(i<ReadSize)
  {
   j             = 8; /* 9 bytes not used in the PAT */
   SectionLength = (((Buffer[i+1]&0xF)<<8)|(Buffer[i+2]))+3;
   while(j<(SectionLength-4))
    {
     TableData->Element[NumberOfEntries].ProgramNumber = (((Buffer[i+j  ]<<8))|(Buffer[i+j+1]));
     TableData->Element[NumberOfEntries].Pid           = (((Buffer[i+j+2]&0x1F)<<8)|(Buffer[i+j+3]));
     NumberOfEntries++; j+=4;
    }
   i += SectionLength;
  }
 TableData->NbElements=NumberOfEntries;


 /*_______________________________________________________________________________________________*/
 /* Free the section buffer */
 /* ======================= */
 #if 0
 PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)Buffer);
 #endif
 handle->mem_free((void*)Buffer);
 /*_______________________________________________________________________________________________*/


 /* Return no errors */
 /* ================ */
 return 0;
}

static int PLAYRECi_STTAPI_PAT_Delete(dal_psi_parse_t* handle,PLAYRECi_STTAPI_PATData_t *TableData)
{
 /* Check if something to delete */
 /* ============================ */
 if (TableData==NULL)
  {
   /*_______________________________________________________________________________________________*/
   //return(ST_NO_ERROR);
   return 0;
    /*_______________________________________________________________________________________________*/
  }
 if (TableData->Element==NULL)
  {
   /*_______________________________________________________________________________________________*/
   //return(ST_NO_ERROR);
   return 0;
    /*_______________________________________________________________________________________________*/
  }

 /*_______________________________________________________________________________________________*/
 /* Free the memory */
 /* =============== */
 //PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)TableData->Element);
 handle->mem_free((void *)TableData->Element);
  /*_______________________________________________________________________________________________*/

 /* Reset the structure */
 /* =================== */
 TableData->NbElements = 0;
 TableData->Element    = NULL;

 /* Return no errors */
 /* ================ */
 return 0;
}


static int PLAYRECi_STTAPI_PMT_Acquire(dal_psi_parse_t* handle,U16 Pid,PLAYRECi_STTAPI_PMTData_t *TableData)
{
 U8            *Buffer;
 U32            BufferSize,ReadSize,NumberOfEntries;
 U32            Program_Info_Length,ES_Info_Length,SectionLength,i,j,k;
 
 //ST_ErrorCode_t ErrCode=ST_NO_ERROR;
 int ErrCode=0;

 /* Clear the structure by keeping ProgramNumber */
 /* ============================================ */
 i=TableData->ProgramNumber;
 memset(TableData,0,sizeof(PLAYRECi_STTAPI_PMTData_t));
 TableData->ProgramNumber=i;


 /*_______________________________________________________________________________________________*/
 /* Allocate buffer to receive the PMT section */
 /* ========================================== */
 BufferSize = PLAYRECi_TAPI_MAX_BUFFER_SIZE;
 #if 0
 Buffer     = (U8 *)PLAYRECi_Context.InitParams.MEM_Allocate(PLAYREC_MEMORY_CACHED,BufferSize,0);
 if (Buffer==NULL)
  {
   PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to allocate memory !!!\n",__FUNCTION__));
   return(ST_ERROR_NO_MEMORY);
  }
 #endif
 Buffer = handle->mem_alloc(BufferSize);
 if( Buffer==NULL)
 {
	// DAL_ERROR(("!!! Unable to allocate memory for elements !!!\n"));
	DebugMessage("[DAL]PLAYRECi_STTAPI_PMT_Acquire-> !!! Unable to allocate memory for elements !!!");
	 handle->mem_free((void*)Buffer);
 }
 /*_______________________________________________________________________________________________*/



 /*_______________________________________________________________________________________________*/
 /* Filter the complete section */
 /* =========================== */
 ErrCode=PLAYRECi_STTAPI_XXX_AcquireFile(handle,Pid,2/*STTAPI_PMT_TABLE_ID*/,TableData->ProgramNumber,PLAYRECi_STTAPI_FILTER_AUTOMATIC,1,Buffer,BufferSize,&ReadSize);
 #if 0
 if (ErrCode!=ST_NO_ERROR)
  {
   PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to filter the section !!!\n",__FUNCTION__));
   PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)Buffer);
   return(ST_ERROR_NO_MEMORY);
  }
 #endif
 if( ErrCode != 0)
 {
	//DAL_ERROR(("!!! Unable to filter the section !!!\n"));
	DebugMessage("[DAL]PLAYRECi_STTAPI_PMT_Acquire-> !!! Unable to filter the section !!");
	handle->mem_free((void*)Buffer);
	return -1;
 }
 /*_______________________________________________________________________________________________*/



 /* Parse the complete buffer */
 /* ========================= */

 /* Get the program number */
 /* ---------------------- */
 TableData->ProgramNumber = ((Buffer[3]<<8)|(Buffer[4]));

 /* Get the pcr pid */
 /* --------------- */
 TableData->PcrPid = (((Buffer[8]&0x1F)<<8)|(Buffer[9]));

 /* Get the number of elements */
 /* -------------------------- */
 NumberOfEntries = i = 0;
 while(i<ReadSize)
  {
   SectionLength           = (((Buffer[i+1]&0xF)<<8)|(Buffer[i+2]))+3;
   Program_Info_Length     = (((Buffer[i+10]&0xF)<<8)|(Buffer[i+11]));
   j                       = Program_Info_Length+12;
   while(j<(SectionLength-4))
    {
     ES_Info_Length = (((Buffer[i+j+3]&0xF)<<8)|(Buffer[i+j+4]));
     NumberOfEntries++; j+=ES_Info_Length+5;
    }
   i += SectionLength;
  }


 /*_______________________________________________________________________________________________*/
 /* If the PMT is empty, go out now */
 /* ------------------------------- */
 #if 0
 if (NumberOfEntries==0)
  {
   TableData->NbElements = 0;
   TableData->Element    = NULL;
   PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)Buffer);
   return(ST_NO_ERROR);
  }
 #endif
 if (NumberOfEntries==0)
  {
	TableData->NbElements = 0;
	TableData->Element    = NULL;
	handle->mem_free((void*)Buffer);
	return -1;
  }

  /*_______________________________________________________________________________________________*/


  /*_______________________________________________________________________________________________*/
 /* Allocate the element memory */
 /* --------------------------- */
  #if 0
 TableData->Element=(PLAYRECi_STTAPI_PMTElement_t *)PLAYRECi_Context.InitParams.MEM_Allocate(PLAYREC_MEMORY_CACHED,NumberOfEntries*sizeof(PLAYRECi_STTAPI_PMTElement_t),0);
 if (TableData->Element==NULL)
  {
   PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to allocate memory for elements !!!\n",__FUNCTION__));
   PLAYRECi_STTAPI_PMT_Delete(TableData);
   PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)Buffer);
   return(ST_ERROR_NO_MEMORY);
  }
 #endif
 TableData->Element = handle->mem_alloc(NumberOfEntries*sizeof(PLAYRECi_STTAPI_PMTElement_t));
 if( TableData->Element == NULL)
 {
	//DAL_ERROR(("!!! Unable to allocate memory for elements !!!\n"));
	DebugMessage("[DAL]PLAYRECi_STTAPI_PMT_Acquire-> !!! Unable to allocate memory for elements !!!");
	PLAYRECi_STTAPI_PMT_Delete(handle,TableData);
	handle->mem_free((void *)Buffer);
	return -1;	
 }
 /*_______________________________________________________________________________________________*/




 /* Get the number of byte to allocate for general descriptors */
 /* ---------------------------------------------------------- */
 NumberOfEntries = i = 0;
 while(i<ReadSize)
  {
   SectionLength           = (((Buffer[i+1]&0xF)<<8)|(Buffer[i+2]))+3;
   Program_Info_Length     = (((Buffer[i+10]&0xF)<<8)|(Buffer[i+11]));
   j = 0;
   while(j<Program_Info_Length)
    {
     NumberOfEntries++;
     j+= Buffer[i+j+13]+2;
    }
   i += SectionLength;
  }
 
 /*_______________________________________________________________________________________________*/
 /* Allocate the general descriptor memory */
 /* -------------------------------------- */
 #if 0
 if (NumberOfEntries==0)
  {
   TableData->Descriptor    = NULL;
   TableData->NbDescriptors = 0;
  }
 else
  {
   TableData->Descriptor=(PLAYRECi_STTAPI_Descriptor_t *)PLAYRECi_Context.InitParams.MEM_Allocate(PLAYREC_MEMORY_CACHED,NumberOfEntries*sizeof(PLAYRECi_STTAPI_Descriptor_t),0);
   if (TableData->Descriptor==NULL)
    {
     PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to allocate memory for descriptors !!!\n",__FUNCTION__));
     PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)Buffer);
     return(ST_ERROR_NO_MEMORY);
    }
  }
 #endif
  if (NumberOfEntries==0)
  {
   TableData->Descriptor    = NULL;
   TableData->NbDescriptors = 0;
  }
 else
  {
	TableData->Descriptor = handle->mem_alloc(NumberOfEntries*sizeof(PLAYRECi_STTAPI_Descriptor_t));
   if (TableData->Descriptor==NULL)
    {
		//DAL_ERROR(("!!! Unable to allocate memory for elements !!!\n"));
		DebugMessage("[DAL]PLAYRECi_STTAPI_PMT_Acquire-> !!! Unable to allocate memory for elements !!!");
		handle->mem_free((void *)Buffer);
		return -1;
    }
  }
 /*_______________________________________________________________________________________________*/



 /* Fill up the general descriptors */
 /* ------------------------------- */
 if (TableData->Descriptor!=NULL)
  {
   TableData->NbDescriptors = i = 0;
   while(i<ReadSize)
    {
     SectionLength       = (((Buffer[i+1]&0xF)<<8)|(Buffer[i+2]))+3;
     Program_Info_Length = (((Buffer[i+10]&0xF)<<8)|(Buffer[i+11]));
     j = 0;
     while(j<Program_Info_Length)
      {
       TableData->Descriptor[TableData->NbDescriptors].Tag  = Buffer[i+j+12];
       TableData->Descriptor[TableData->NbDescriptors].Size = Buffer[i+j+13];
       TableData->Descriptor[TableData->NbDescriptors].Data = NULL;
       if (TableData->Descriptor[TableData->NbDescriptors].Size!=0)
        {
        /*_______________________________________________________________________________________________*/
         //TableData->Descriptor[TableData->NbDescriptors].Data = (U8 *)PLAYRECi_Context.InitParams.MEM_Allocate(PLAYREC_MEMORY_CACHED,TableData->Descriptor[TableData->NbDescriptors].Size,0);
			TableData->Descriptor[TableData->NbDescriptors].Data = handle->mem_alloc(TableData->Descriptor[TableData->NbDescriptors].Size);
		/*_______________________________________________________________________________________________*/
		 if (TableData->Descriptor[TableData->NbDescriptors].Data == NULL)
          {
 /*_______________________________________________________________________________________________*/
			#if 0
			PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to allocate memory for descriptors !!!\n",__FUNCTION__));
			PLAYRECi_STTAPI_PMT_Delete(TableData);
			PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)Buffer);
			return(ST_ERROR_NO_MEMORY);
			#endif
		//	DAL_ERROR(("!!! Unable to allocate memory for descriptors !!!\n"));
			DebugMessage("[DAL]PLAYRECi_STTAPI_PMT_Acquire-> !!! Unable to allocate memory for descriptors !!!");
			PLAYRECi_STTAPI_PMT_Delete(handle,TableData);
			handle->mem_free((void *)Buffer);
			return -1;		   
 /*_______________________________________________________________________________________________*/
          }
         memcpy(TableData->Descriptor[TableData->NbDescriptors].Data,&Buffer[i+j+14],TableData->Descriptor[TableData->NbDescriptors].Size);
        }
       j+=TableData->Descriptor[TableData->NbDescriptors].Size+2;
       TableData->NbDescriptors++;
      }
     i += SectionLength;
    }
  }

 /* Fill up the elements list */
 /* ------------------------- */
 TableData->NbElements = i = 0;
 while(i<ReadSize)
  {
   SectionLength           = (((Buffer[i+1]&0xF)<<8)|(Buffer[i+2]))+3;
   Program_Info_Length     = (((Buffer[i+10]&0xF)<<8)|(Buffer[i+11]));
   j                       = Program_Info_Length+12;
   while(j<(SectionLength-4))
    {
     TableData->Element[TableData->NbElements].Type          = Buffer[i+j];
     TableData->Element[TableData->NbElements].Pid           = (((Buffer[i+j+1]&0x1F)<<8)|(Buffer[i+j+2]));
     TableData->Element[TableData->NbElements].NbDescriptors = 0;
     TableData->Element[TableData->NbElements].Descriptor    = NULL;
     ES_Info_Length = (((Buffer[i+j+3]&0xF)<<8)|(Buffer[i+j+4]));
     /* Get the number of descriptors to allocate */
     for (k=0,NumberOfEntries=0;k<ES_Info_Length;)
      {
       k=k+Buffer[i+j+5+k+1]+2;
       NumberOfEntries++;
      }
     /* If there are some descriptors */
     if (ES_Info_Length!=0)
      {
       /*_______________________________________________________________________________________________*/
       //TableData->Element[TableData->NbElements].Descriptor=(PLAYRECi_STTAPI_Descriptor_t *)PLAYRECi_Context.InitParams.MEM_Allocate(PLAYREC_MEMORY_CACHED,NumberOfEntries*sizeof(PLAYRECi_STTAPI_Descriptor_t),0);
		TableData->Element[TableData->NbElements].Descriptor = handle->mem_alloc(NumberOfEntries*sizeof(PLAYRECi_STTAPI_Descriptor_t));
	   /*_______________________________________________________________________________________________*/
	   if (TableData->Element[TableData->NbElements].Descriptor==NULL)
        {
 /*_______________________________________________________________________________________________*/
			#if 0
			PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to allocate memory for descriptors !!!\n",__FUNCTION__));
			PLAYRECi_STTAPI_PMT_Delete(TableData);
			PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)Buffer);
			return(ST_ERROR_NO_MEMORY);
			#endif
		//	DAL_ERROR(("!!! Unable to allocate memory for descriptors !!!\n"));
			DebugMessage("[DAL]PLAYRECi_STTAPI_PMT_Acquire-> !!! Unable to allocate memory for descriptors !!!");
			PLAYRECi_STTAPI_PMT_Delete(handle,TableData);
			handle->mem_free((void *)Buffer);
			return -1;		 
 /*_______________________________________________________________________________________________*/

        }
       k=0;
       while(k<ES_Info_Length)
        {
         TableData->Element[TableData->NbElements].Descriptor[TableData->Element[TableData->NbElements].NbDescriptors].Tag  = Buffer[i+j+5+k];
         TableData->Element[TableData->NbElements].Descriptor[TableData->Element[TableData->NbElements].NbDescriptors].Size = Buffer[i+j+5+k+1];
         TableData->Element[TableData->NbElements].Descriptor[TableData->Element[TableData->NbElements].NbDescriptors].Data = NULL;
         if (TableData->Element[TableData->NbElements].Descriptor[TableData->Element[TableData->NbElements].NbDescriptors].Size!=0)
          {
          /*_______________________________________________________________________________________________*/
           //TableData->Element[TableData->NbElements].Descriptor[TableData->Element[TableData->NbElements].NbDescriptors].Data = (U8 *)PLAYRECi_Context.InitParams.MEM_Allocate(PLAYREC_MEMORY_CACHED,TableData->Element[TableData->NbElements].Descriptor[TableData->Element[TableData->NbElements].NbDescriptors].Size,0);
		  TableData->Element[TableData->NbElements].Descriptor[TableData->Element[TableData->NbElements].NbDescriptors].Data = handle->mem_alloc(TableData->Element[TableData->NbElements].Descriptor[TableData->Element[TableData->NbElements].NbDescriptors].Size);
		  /*_______________________________________________________________________________________________*/
		   if (TableData->Element[TableData->NbElements].Descriptor[TableData->Element[TableData->NbElements].NbDescriptors].Data == NULL)
            {
 /*_______________________________________________________________________________________________*/
			#if 0
			PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to allocate memory for descriptors !!!\n",__FUNCTION__));
			PLAYRECi_STTAPI_PMT_Delete(TableData);
			PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)Buffer);
			return(ST_ERROR_NO_MEMORY);
			#endif
		//	DAL_ERROR(("!!! Unable to allocate memory for descriptors !!!\n"));
			DebugMessage("[DAL]PLAYRECi_STTAPI_PMT_Acquire-> !!! Unable to allocate memory for descriptors !!!");
			PLAYRECi_STTAPI_PMT_Delete(handle,TableData);
			handle->mem_free((void *)Buffer);
			return -1;	   
 /*_______________________________________________________________________________________________*/

            }
           memcpy(TableData->Element[TableData->NbElements].Descriptor[TableData->Element[TableData->NbElements].NbDescriptors].Data,&Buffer[i+j+5+k+2],TableData->Element[TableData->NbElements].Descriptor[TableData->Element[TableData->NbElements].NbDescriptors].Size);
          }
         k=k+Buffer[i+j+5+k+1]+2;
         TableData->Element[TableData->NbElements].NbDescriptors++;
        }
      }
     TableData->NbElements++; j+=ES_Info_Length+5;
    }
   i += SectionLength;
  }


 /*_______________________________________________________________________________________________*/
 /* Free the section buffer */
 /* ======================= */
 #if 0
 PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)Buffer);
 #endif
 handle->mem_free((void *)Buffer);
 /*_______________________________________________________________________________________________*/

 /* Return no errors */
 /* ================ */
 return 0;
}


static int PLAYRECi_STTAPI_PMT_Delete(dal_psi_parse_t* handle,PLAYRECi_STTAPI_PMTData_t *TableData)
{
 U32 i,j;

 /* Check if something to delete */
 /* ============================ */
 if (TableData==NULL)
  {
  /*_______________________________________________________________________________________________*/
   //return(ST_NO_ERROR);
  return 0;
  /*_______________________________________________________________________________________________*/
  }

 /* Free the global descriptors */
 /* =========================== */
 if (TableData->Descriptor!=NULL)
  {
   /* For each global descriptors */
   /* --------------------------- */
   for (i=0;i<TableData->NbDescriptors;i++)
    {
     if (TableData->Descriptor[i].Data!=NULL)
      {
      /*_______________________________________________________________________________________________*/
       //PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)TableData->Descriptor[i].Data);
       handle->mem_free((void *)TableData->Descriptor[i].Data);
	  /*_______________________________________________________________________________________________*/
      }
    }
   /*_______________________________________________________________________________________________*/
   //PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)TableData->Descriptor);
   handle->mem_free((void *)TableData->Descriptor);
   /*_______________________________________________________________________________________________*/
   TableData->NbDescriptors = 0;
   TableData->Descriptor    = NULL;
  }

 /* Free the elements */
 /* ================= */
 if (TableData->Element!=NULL)
  {
   /* For each element */
   /* ---------------- */
   for (i=0;i<TableData->NbElements;i++)
    {
     if (TableData->Element[i].Descriptor!=NULL)
      {
       for (j=0;j<TableData->Element[i].NbDescriptors;j++)
        {
         if (TableData->Element[i].Descriptor[j].Data!=NULL)
          {
          /*_______________________________________________________________________________________________*/
           //PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)TableData->Element[i].Descriptor[j].Data);
		   handle->mem_free((void *)TableData->Element[i].Descriptor[j].Data);
		  /*_______________________________________________________________________________________________*/
          }
        }
	   /*_______________________________________________________________________________________________*/
       //PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)TableData->Element[i].Descriptor);
	   handle->mem_free((void *)TableData->Element[i].Descriptor);
	   /*_______________________________________________________________________________________________*/
      }
    }

   /*_______________________________________________________________________________________________*/
   /* Clear the element structure */
   /* --------------------------- */
   //PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)TableData->Element);
   handle->mem_free((void *)TableData->Element);
   /*_______________________________________________________________________________________________*/

   TableData->NbElements = 0;
   TableData->Element    = NULL;
  }

 /* Return no errors */
 /* ================ */
 return 0;
}

static int PLAYRECi_STTAPI_PMT_GetInfo(PLAYRECi_STTAPI_PMTElement_t *PMTElement,PLAYREC_StreamType_t *StreamType,U16 *Pid,PLAYRECi_PidType_t *PidType,char *StringType)
{
 U32 i;

 /* Check parameters */
  /* ================ */
 if ((PMTElement==NULL)||(StreamType==NULL)||(PidType==NULL)||(Pid==NULL)||(StringType==NULL))
  {
   return -1;
  }

 /* Reset fields */
 /* ============ */
 *StreamType = PLAYREC_STREAMTYPE_MP1V;
 *PidType    = PLAYRECi_PID_UNDEFINED;
 *Pid        = 0;
 strcpy(StringType,"NONE");

 /* Analysis of each pmt element */
 /* ============================ */
 switch(PMTElement->Type)
  {
   case PLAYRECi_STTAPI_STREAM_VIDEO_MPEG1 :
    *StreamType = PLAYREC_STREAMTYPE_MP1V;
    *PidType    = PLAYRECi_PID_VIDEO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"MP1V");
    break;
   case PLAYRECi_STTAPI_STREAM_VIDEO_MPEG2 :
    *StreamType = PLAYREC_STREAMTYPE_MP2V;
    *PidType    = PLAYRECi_PID_VIDEO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"MP2V");
    break;
   case PLAYRECi_STTAPI_STREAM_VIDEO_MPEG4 :
    *StreamType = PLAYREC_STREAMTYPE_H264;
    *PidType    = PLAYRECi_PID_VIDEO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"MP4V");
    break;
   case PLAYRECi_STTAPI_STREAM_VIDEO_VC1 :
    *StreamType = PLAYREC_STREAMTYPE_VC1;
    *PidType    = PLAYRECi_PID_VIDEO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"VC-1");
    break;
   case PLAYRECi_STTAPI_STREAM_VIDEO_AVS :
    *StreamType = PLAYREC_STREAMTYPE_AVS;
    *PidType    = PLAYRECi_PID_VIDEO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"AVS");
    break;
   case PLAYRECi_STTAPI_STREAM_AUDIO_MPEG1 :
    *StreamType = PLAYREC_STREAMTYPE_MP1A;
    *PidType    = PLAYRECi_PID_AUDIO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"MP1A");
    break;
   case PLAYRECi_STTAPI_STREAM_AUDIO_MPEG2 :
    *StreamType = PLAYREC_STREAMTYPE_MP2A;
    *PidType    = PLAYRECi_PID_AUDIO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"MP2A");
    break;
   case PLAYRECi_STTAPI_STREAM_AUDIO_WMA :
    *StreamType = PLAYREC_STREAMTYPE_WMA;
    *PidType    = PLAYRECi_PID_AUDIO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"WMA");
    break;
   case PLAYRECi_STTAPI_STREAM_AUDIO_AAC_ADTS :
    *StreamType = PLAYREC_STREAMTYPE_AAC;
    *PidType    = PLAYRECi_PID_AUDIO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"AAC");
    break;
   case PLAYRECi_STTAPI_STREAM_AUDIO_AAC_LATM :
    *StreamType = PLAYREC_STREAMTYPE_HEAAC;
    *PidType    = PLAYRECi_PID_AUDIO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"HEAAC");
    break;
   case PLAYRECi_STTAPI_STREAM_AUDIO_AAC_RAW1 :
    *StreamType = PLAYREC_STREAMTYPE_HEAAC;
    *PidType    = PLAYRECi_PID_AUDIO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"HEAAC");
    break;
   case PLAYRECi_STTAPI_STREAM_AUDIO_AAC_RAW2 :
    *StreamType = PLAYREC_STREAMTYPE_HEAAC;
    *PidType    = PLAYRECi_PID_AUDIO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"HEAAC");
    break;
   case PLAYRECi_STTAPI_STREAM_AUDIO_LPCM :
    break;
   case PLAYRECi_STTAPI_STREAM_AUDIO_AC3 :
    *StreamType = PLAYREC_STREAMTYPE_AC3;
    *PidType    = PLAYRECi_PID_AUDIO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"AC3");
    break;
   case PLAYRECi_STTAPI_STREAM_AUDIO_DTS :
    *StreamType = PLAYREC_STREAMTYPE_DTS;
    *PidType    = PLAYRECi_PID_AUDIO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"DTS");
    break;
   case PLAYRECi_STTAPI_STREAM_AUDIO_MLP :
    break;
   case PLAYRECi_STTAPI_STREAM_AUDIO_DTSHD     :
   case PLAYRECi_STTAPI_STREAM_AUDIO_DTSHD_XLL :
   case PLAYRECi_STTAPI_STREAM_AUDIO_DTSHD_2   :
    *StreamType = PLAYREC_STREAMTYPE_DTS;
    *PidType    = PLAYRECi_PID_AUDIO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"DTS");
    break;
   case PLAYRECi_STTAPI_STREAM_AUDIO_DDPLUS   :
   case PLAYRECi_STTAPI_STREAM_AUDIO_DDPLUS_2 :
    *StreamType = PLAYREC_STREAMTYPE_DDPLUS;
    *PidType    = PLAYRECi_PID_AUDIO;
    *Pid        = PMTElement->Pid;
    strcpy(StringType,"DDPLUS");
    break;
   default :
    break;
  }

 /* Look for sub descriptors */
 /* ======================== */
 for (i=0;i<PMTElement->NbDescriptors;i++)
  {
   if (PMTElement->Descriptor[i].Tag==PLAYRECi_STTAPI_DESCRIPTOR_REGISTRATION)
    {
     U8 RegistrationString[16];
     PLAYRECi_STTAPI_DescriptorRegistration(&(PMTElement->Descriptor[i]),RegistrationString);
     if (strcmp((char *)RegistrationString,"VC-1")==0)
      {
       *StreamType = PLAYREC_STREAMTYPE_VC1;
       *PidType    = PLAYRECi_PID_VIDEO;
       *Pid        = PMTElement->Pid;
       strcpy(StringType,"VC1");
      }
#if 0
     if (strcmp((char *)RegistrationString,"HDMV")==0)
      {
       *StreamType = PLAYREC_STREAMTYPE_H264;
       *PidType    = PLAYRECi_PID_VIDEO;
       *Pid        = PMTElement->Pid;
       strcpy(StringType,"H264");
      }
#endif
     if (strcmp((char *)RegistrationString,"WMA9")==0)
      {
       *StreamType = PLAYREC_STREAMTYPE_WMA;
       *PidType    = PLAYRECi_PID_AUDIO;
       *Pid        = PMTElement->Pid;
       strcpy(StringType,"WMA");
      }
     if (strcmp((char *)RegistrationString,"AC-3")==0)
      {
       *StreamType = PLAYREC_STREAMTYPE_AC3;
       *PidType    = PLAYRECi_PID_AUDIO;
       *Pid        = PMTElement->Pid;
       strcpy(StringType,"AC3");
      }
    }
   if (PMTElement->Descriptor[i].Tag==PLAYRECi_STTAPI_DESCRIPTOR_MPEG4_VIDEO)
    {
     *StreamType = PLAYREC_STREAMTYPE_H264;
     *PidType    = PLAYRECi_PID_VIDEO;
     *Pid        = PMTElement->Pid;
     strcpy(StringType,"H264");
    }
   if (PMTElement->Descriptor[i].Tag==PLAYRECi_STTAPI_DESCRIPTOR_MPEG4_AUDIO)
    {
     /* /!\ If stream type is telling us it's ADTS or LATM AAC type, the information is clear, no need to parse descriptors /!\ */
     if ((PMTElement->Type!=PLAYRECi_STTAPI_STREAM_AUDIO_AAC_ADTS)&&(PMTElement->Type!=PLAYRECi_STTAPI_STREAM_AUDIO_AAC_LATM))
      {
       *StreamType = PLAYREC_STREAMTYPE_MP4A;
       *PidType    = PLAYRECi_PID_AUDIO;
       *Pid        = PMTElement->Pid;
       strcpy(StringType,"MP4A");
      }
    }
   if (PMTElement->Descriptor[i].Tag==PLAYRECi_STTAPI_DESCRIPTOR_AAC)
    {
     /* /!\ If stream type is telling us it's ADTS or LATM AAC type, the information is clear, no need to parse descriptors /!\ */
     if ((PMTElement->Type!=PLAYRECi_STTAPI_STREAM_AUDIO_AAC_ADTS)&&(PMTElement->Type!=PLAYRECi_STTAPI_STREAM_AUDIO_AAC_LATM))
      {
       *StreamType = PLAYREC_STREAMTYPE_AAC;
       *PidType    = PLAYRECi_PID_AUDIO;
       *Pid        = PMTElement->Pid;
       strcpy(StringType,"AAC");
      }
    }
   if (PMTElement->Descriptor[i].Tag==PLAYRECi_STTAPI_DESCRIPTOR_AC3)
    {
     *StreamType = PLAYREC_STREAMTYPE_AC3;
     *PidType    = PLAYRECi_PID_AUDIO;
     *Pid        = PMTElement->Pid;
     strcpy(StringType,"AC3");
    }
   if (PMTElement->Descriptor[i].Tag==PLAYRECi_STTAPI_DESCRIPTOR_ENHANCED_AC3)
    {
     *StreamType = PLAYREC_STREAMTYPE_DDPLUS;
     *PidType    = PLAYRECi_PID_AUDIO;
     *Pid        = PMTElement->Pid;
     strcpy(StringType,"DDPLUS");
    }
   if (PMTElement->Descriptor[i].Tag==PLAYRECi_STTAPI_DESCRIPTOR_DTS)
    {
     *StreamType = PLAYREC_STREAMTYPE_DTS;
     *PidType    = PLAYRECi_PID_AUDIO;
     *Pid        = PMTElement->Pid;
     strcpy(StringType,"DTS");
    }
   if (PMTElement->Descriptor[i].Tag==PLAYRECi_STTAPI_DESCRIPTOR_TELETEXT)
    {
     *StreamType = PLAYREC_STREAMTYPE_TTXT;
     *PidType    = PLAYRECi_PID_TELETEXT;
     *Pid        = PMTElement->Pid;
     strcpy(StringType,"TTXT");
    }
   if (PMTElement->Descriptor[i].Tag==PLAYRECi_STTAPI_DESCRIPTOR_SUBTITLING)
    {
     *StreamType = PLAYREC_STREAMTYPE_SUBT;
     *PidType    = PLAYRECi_PID_SUBTITLE;
     *Pid        = PMTElement->Pid;
//{{{add by LYN, for subtitle , 2011-08-24
    
    Set_SubtPid(PMTElement->Pid);
    Parse_SubData(PMTElement->Descriptor[i].Data,PMTElement->Descriptor[i].Size);
//}}}add by LYN, for subtitle , 2011-08-24
     
     strcpy(StringType,"SUBT");
    }
  }

 /* Return no errors */
 /* ================ */
 return(0);
}

static int PLAYRECi_STTAPI_DescriptorRegistration(PLAYRECi_STTAPI_Descriptor_t *Descriptor,U8 *FormatIdentifier)
{
 /* Check parameters */
 /* ---------------- */
 if ((Descriptor==NULL)||(FormatIdentifier==NULL))
  {
   return(-1);
  }
 FormatIdentifier[0]=0;

 /* If descriptor is not correct */
 /* ---------------------------- */
 if ((Descriptor->Tag!=PLAYRECi_STTAPI_DESCRIPTOR_REGISTRATION) || (Descriptor->Size==0))
  {
   return(-1);
  }

 /* Get the descriptor details */
 /* -------------------------- */
 FormatIdentifier[0] = (Descriptor->Data[0]);
 FormatIdentifier[1] = (Descriptor->Data[1]);
 FormatIdentifier[2] = (Descriptor->Data[2]);
 FormatIdentifier[3] = (Descriptor->Data[3]);
 FormatIdentifier[4] = 0;

 /* Return no errors */
 /* ---------------- */
 return(0);
}


static int PLAYRECi_STTAPI_XXX_AcquireFile(dal_psi_parse_t* handle,U16 Pid,U8 TableId,U32 ProgramNumber,U32 SectionNumber,U32 CurrentNotNext,U8 *Buffer,U32 BufferSize,U32 *ReadSize)
{
 U8      *TS_Buffer=NULL;
// void    *FILE_Handle=NULL;
 U32      TS_NbBytes=0;
 BOOL     TS_SectionFound=FALSE;
 U32      TS_NbBytesReadFromFile=0,i=0,j=0;
 U8       sync_byte=0;
 U8       transport_error_indicator=0;
 U8       payload_unit_start_indicator=0;
 U16      PID=0;
 U8       adaptation_field_control=0;
 U16      adaptation_field_length=0;
 U16      offset=0;
 U8       table_id=0;
 U8       section_syntax_indicator=0;
 U16      section_length=0;
 U16      program_number=0;
 U8       version_number=0;
 U8       current_next_indicator=0;
 U8       section_number=0;
 U8       last_section_number=0;
 U32      nb_bytes_to_copy_by_section=0;
 U8       section_number_to_filter=0;
 U8       payload_unit_start_indicator_to_filter=0;
 U32      size_to_copy=0;
 BOOL     topology_found;
 U32      packet_size=188;
 U32      packet_header_size=0;
 U32      start_of_sync=0;

 /*_______________________________________________________________________________________________*/
 unsigned int		read_offset=0;
 unsigned int		search_size=16*packet_size;
 /*_______________________________________________________________________________________________*/

 


 /*_______________________________________________________________________________________________*/
 /* We try first to open the file for topology analysis */
 /* =================================================== */
  #if 0
 FILE_Handle=PLAYRECi_Context.InitParams.FS_Open((char *)FileName,"rb");
 if (FILE_Handle==NULL)
  {
   PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to open the file for topology analysis \"%s\" !!!\n",__FUNCTION__,FileName));
   return(ST_ERROR_BAD_PARAMETER);
  }
 #endif
 /*_______________________________________________________________________________________________*/

 /*_______________________________________________________________________________________________*/
 /* Allocate a small buffer for topology analysis */
 /* ============================================= */
 #if 0
 TS_Buffer=(U8 *)PLAYRECi_Context.InitParams.MEM_Allocate(PLAYREC_MEMORY_CACHED,2048,0);
 if (TS_Buffer==NULL)
  {
   PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to allocate memory to read ts datas for topology analysis !!!\n",__FUNCTION__));
   PLAYRECi_Context.InitParams.FS_Close(FILE_Handle);
   return(ST_ERROR_NO_MEMORY);
  }
 #endif
 TS_Buffer = handle->mem_alloc(2048);
 if( TS_Buffer == NULL)
 {
	 DebugMessage("[DAL]PLAYRECi_STTAPI_XXX_AcquireFile-> !!! Unable to allocate memory to read ts datas for topology analysis !!!");
	//DAL_ERROR(("!!! Unable to allocate memory to read ts datas for topology analysis !!!\n"));
	return -1;
 }
 /*_______________________________________________________________________________________________*/


 /*_______________________________________________________________________________________________*/
 /* Read small quantity of datas for topology analysis */
 /* ================================================== */
 #if 0
 TS_NbBytes=PLAYRECi_Context.InitParams.FS_Read(TS_Buffer,1,2048,FILE_Handle);
 if (TS_NbBytes!=2048)
  {
   PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to read enough datas for topology analysis !!!\n",__FUNCTION__));
   PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)TS_Buffer);
   PLAYRECi_Context.InitParams.FS_Close(FILE_Handle);
   return(ST_ERROR_NO_MEMORY);
  }
 #endif
 TS_NbBytes = handle->read_data(TS_Buffer,read_offset,2048);
 if( TS_NbBytes != 2048)
 {
	//DAL_ERROR(("!!! Unable to read enough datas for topology analysis !!!\n"));
	DebugMessage("[DAL]PLAYRECi_STTAPI_XXX_AcquireFile-> !!! Unable to read enough datas for topology analysis !!!");
	handle->mem_free((void*)TS_Buffer);
	return -1;
 }
 /*_______________________________________________________________________________________________*/


 /*_______________________________________________________________________________________________*/
 /* Close the file */
 /* ============== */
 #if 0
 if (PLAYRECi_Context.InitParams.FS_Close(FILE_Handle)!=0)
  {
   PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to close the file for topology analysis !!!\n",__FUNCTION__));
   PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)TS_Buffer);
   return(ST_ERROR_BAD_PARAMETER);
  }
 #endif
 /*_______________________________________________________________________________________________*/




 /* Try to find topology */
 /* ==================== */
 for (start_of_sync=0,topology_found=FALSE;(start_of_sync<1024)&&(topology_found==FALSE);start_of_sync++)
  {
   if (TS_Buffer[start_of_sync]==0x47)
    {
     for (packet_size=188;packet_size<=204;packet_size++)
      {
       if ((TS_Buffer[start_of_sync+packet_size]==0x47)&&(TS_Buffer[start_of_sync+2*packet_size]==0x47)&&(TS_Buffer[start_of_sync+3*packet_size]==0x47))
        {
         packet_header_size=packet_size-188;
         if (start_of_sync<packet_header_size)
          {
           start_of_sync=start_of_sync+packet_size-packet_header_size-1;
          }
         else
          {
           start_of_sync=start_of_sync-packet_header_size-1;
          }
         topology_found=TRUE;
         break;
        }
      }
    }
  }


 /*_______________________________________________________________________________________________*/
 /* Deallocate the intermediate buffer for topology analysis */
 /* ======================================================== */
 #if 0
 PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)TS_Buffer);
 #endif
 handle->mem_free((void*)TS_Buffer);
 /*_______________________________________________________________________________________________*/


 /*_______________________________________________________________________________________________*/
 /* If topology is not found, go out as this is not a ts stream known */
 /* ================================================================= */
 #if 0
 if (topology_found==FALSE)
  {

  /*PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to find the topology of this ts stream !!!\n",__FUNCTION__));*/
	 return(ST_ERROR_BAD_PARAMETER);
	}
 #endif
   if(topology_found==FALSE)
   {
	 // DAL_ERROR(("!!! Unable to find the topology of this ts stream !!!\n"));
	 DebugMessage("[DAL]PLAYRECi_STTAPI_XXX_AcquireFile-> !!! Unable to find the topology of this ts stream !!!");
	  return -1;
   }
   /*_______________________________________________________________________________________________*/
  
  
   /*_______________________________________________________________________________________________*/
   /* Now we open the file for section analysis */
   /* ========================================= */
 #if 0
   FILE_Handle=PLAYRECi_Context.InitParams.FS_Open((char *)FileName,"rb");
   if (FILE_Handle==NULL)
	{
	 PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to open the file \"%s\" !!!\n",__FUNCTION__,FileName));
	 return(ST_ERROR_BAD_PARAMETER);
	}
  
   /* We perform a seek to be aligned to start of sync */
   /* ================================================ */
   if (PLAYRECi_Context.InitParams.FS_Seek(FILE_Handle,start_of_sync,SEEK_SET)!=0)
	{
	 PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to perform a seek to be aligned to start of synchro !!!\n",__FUNCTION__));
	 PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)TS_Buffer);
	 PLAYRECi_Context.InitParams.FS_Close(FILE_Handle);
	 return(ST_ERROR_BAD_PARAMETER);
	}
  
   /* Then, we allocate a piece of buffer to read packets */
   /* =================================================== */
   TS_Buffer=(U8 *)PLAYRECi_Context.InitParams.MEM_Allocate(PLAYREC_MEMORY_CACHED,packet_size*1024,0);
   if (TS_Buffer==NULL)
	{
	 PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to allocate memory to read ts datas !!!\n",__FUNCTION__));
	 PLAYRECi_Context.InitParams.FS_Close(FILE_Handle);
	 return(ST_ERROR_NO_MEMORY);
	}
 #endif
   
   if( start_of_sync != 0)
   {
	   DebugMessage("[DAL]PLAYRECi_STTAPI_XXX_AcquireFile-> ???? start_of_sync: 0x%x\n", start_of_sync);
	  // DAL_DEBUG(("-----------------------------------------------???? start_of_sync: %u\n", start_of_sync));
   }
  
   read_offset = start_of_sync;
   TS_Buffer = handle->mem_alloc(search_size);
   if(TS_Buffer==NULL)
   {
	 // DAL_ERROR(("!!! Unable to allocate memory to read ts datas !!!\n"));
	 DebugMessage("[DAL]PLAYRECi_STTAPI_XXX_AcquireFile->!!! Unable to allocate memory to read ts datas !!!");
	  return -1;
   }
   /*_______________________________________________________________________________________________*/
  
  
  
  
   /* Now, we loop until we reach the end of the file or find the complete section needed */
   /* =================================================================================== */
   TS_SectionFound						  = FALSE;
   TS_NbBytesReadFromFile				  = 0;
   section_number_to_filter 			  = (SectionNumber==PLAYRECi_STTAPI_FILTER_AUTOMATIC)?0:SectionNumber;
   payload_unit_start_indicator_to_filter = 1;
   nb_bytes_to_copy_by_section			  = 0;
   *ReadSize							  = 0;
  
  
  
   
   /* Look for section in the first 23.5Mbytes of datas (packet_size*128*1024) */
   while((TS_SectionFound==FALSE)&&(TS_NbBytesReadFromFile<=(packet_size*128*1024)))
	{
  
	 /*_______________________________________________________________________________________________*/
	 /* Read 128 ts packets */
	 /* ------------------- */
   #if 0
	 TS_NbBytes=PLAYRECi_Context.InitParams.FS_Read(TS_Buffer,1,packet_size*1024,FILE_Handle);
	 if (TS_NbBytes>packet_size*1024)
	  {
	   PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to read datas from the file !!!\n",__FUNCTION__));
	   break;
	  }
   #endif
	 TS_NbBytes = handle->read_data(TS_Buffer,read_offset,search_size);

/*addd to test reparse*/
#if 0
{
	static unsigned int   g_test_reparse = 0;
	if( ++g_test_reparse == 50)
	{
		TS_NbBytes = 0;
	}
	DAL_DEBUG(("-----------------------------------g_test_reparse: %d\n", g_test_reparse));
}
#endif	
/**/

	 if( TS_NbBytes != search_size)
	 {
		//  DAL_ERROR(("!!! Unable to read datas from the file !!! read_offset: %d\n", read_offset));
		DebugMessage("[DAL]PLAYRECi_STTAPI_XXX_AcquireFile->!!! Unable to read datas from the file !!! buffersize:%d\n",g_parse_handle.parse_buffer.buf_size);
		DebugMessage("[DAL]PLAYRECi_STTAPI_XXX_AcquireFile->!!! Unable to read datas from the file !!! read_offset: %d\n", read_offset);
		DebugMessage("[DAL]PLAYRECi_STTAPI_XXX_AcquireFile->!!! Unable to read datas from the file !!! searchsize:%d\n",search_size);


		return -1;
	 }
	 read_offset += TS_NbBytes;
	 /*_______________________________________________________________________________________________*/
	 
	 TS_NbBytes=((TS_NbBytes/packet_size)*packet_size);
	 if (TS_NbBytes==0) break;
	 TS_NbBytesReadFromFile=TS_NbBytesReadFromFile+TS_NbBytes;
  
	 /* Parse the packets */
	 /* ----------------- */
	 for (i=0;((i<TS_NbBytes)&&(TS_SectionFound==FALSE));i+=packet_size)
	  {
	   j							= i+packet_header_size;
	   sync_byte					= TS_Buffer[j];
	   transport_error_indicator	= (TS_Buffer[j+1]>>7)&0x1;
	   payload_unit_start_indicator = (TS_Buffer[j+1]>>6)&0x1;
	   PID							= ((TS_Buffer[j+1]&0x1F)<<8)|(TS_Buffer[j+2]);
	   adaptation_field_control 	= (TS_Buffer[j+3]>>4)&0x3;
	   adaptation_field_length		= 0;
	   offset						= 0;
	   /* If we detect a sync byte */
	   if (sync_byte==0x47)
		{
		 /* If there is no error inside the packet */
		 if (transport_error_indicator==0)
		  {
		   /* If the pid matchs and the packet contain a payload */
		   if ((PID==Pid)&&(adaptation_field_control!=0)&&(adaptation_field_control!=2))
			{
			 /* If there is an adapation field, we need to bypass it */
			 if ((adaptation_field_control==2)||(adaptation_field_control==3))
			  {
			   adaptation_field_length = (TS_Buffer[j+4])+1;
			  }
			 /* Offset to point to the beginning of the payload */
			 offset=adaptation_field_length+4;
			 /* Pointer field incremented when PUSI is there */
			 if (payload_unit_start_indicator==1)
			  {
			   /* If we were waiting for a real pusi indicator we want to filter */
			   if (payload_unit_start_indicator_to_filter==1)
				{
				 offset+=TS_Buffer[j+offset]+1;
				}
			   else
				{
				 /* Just bypass pointer field increment, as we are just dumping the section now */
				 offset++;
				}
			  }
			 /* This while is done here to parse several sections inside a single packet */
			 while(offset<=((packet_size-packet_header_size)-8))
			  {
			   /* Section not yet matched, look for it */
			   if ((nb_bytes_to_copy_by_section==0)&&(payload_unit_start_indicator==1))
				{
				 /* We now process the section */
				 table_id				  = TS_Buffer[j+offset];
				 section_syntax_indicator = (TS_Buffer[j+offset+1]>>7)&0x1;
				 section_length 		  = ((TS_Buffer[j+offset+1]&0xF)<<8)|(TS_Buffer[j+offset+2]);
				 program_number 		  = ((TS_Buffer[j+offset+3])<<8)|(TS_Buffer[j+offset+4]);
				 version_number 		  = (TS_Buffer[j+offset+5]>>1)&0x1F;
				 current_next_indicator   = TS_Buffer[j+offset+5]&0x1;
				 section_number 		  = TS_Buffer[j+offset+6];
				 last_section_number	  = TS_Buffer[j+offset+7];
				 /* If there is the syntax indicator for PSI and table id matchs */
				 if ((table_id==TableId)&&(section_syntax_indicator==1))
				  {
				   /* If this is the current psi applied */
				   if ((CurrentNotNext==PLAYRECi_STTAPI_FILTER_DONT_CARE)||((CurrentNotNext!=PLAYRECi_STTAPI_FILTER_DONT_CARE)&&(current_next_indicator==CurrentNotNext)))
					{
					 /* If we filter the program number, match it ? */
					 if ((ProgramNumber==PLAYRECi_STTAPI_FILTER_DONT_CARE)||((ProgramNumber!=PLAYRECi_STTAPI_FILTER_DONT_CARE)&&(program_number==ProgramNumber)))
					  {
					   /* Look for the correct section number */
					   if ((SectionNumber==PLAYRECi_STTAPI_FILTER_DONT_CARE)||((SectionNumber!=PLAYRECi_STTAPI_FILTER_DONT_CARE)&&(section_number_to_filter==section_number)))
						{
						 nb_bytes_to_copy_by_section=section_length+3;
						 payload_unit_start_indicator_to_filter=0;
						}
					  }
					}
				  }
				}
			   /* Section reception in progress, just copy packet */
			   if (nb_bytes_to_copy_by_section!=0)
				{
				 size_to_copy=((packet_size-packet_header_size-offset)>nb_bytes_to_copy_by_section)?nb_bytes_to_copy_by_section:(packet_size-packet_header_size-offset);
				 /* If not enough space in the buffer, go out */
				 if ((*ReadSize+size_to_copy)>BufferSize)
				  {
				   //PLAYRECi_TraceERR(("%s():**ERROR** !!! User section buffer too small !!!\n",__FUNCTION__));
				   break;
				  }
				 /* Copy the datas */
				 memcpy(Buffer+*ReadSize,&TS_Buffer[j+offset],size_to_copy);
				 *ReadSize+=size_to_copy;
				 nb_bytes_to_copy_by_section-=size_to_copy;
				 /* If we reach the end of the section */
				 if (nb_bytes_to_copy_by_section==0)
				  {
				   if (((SectionNumber==PLAYRECi_STTAPI_FILTER_AUTOMATIC)&&(last_section_number==section_number))||(SectionNumber==PLAYRECi_STTAPI_FILTER_DONT_CARE))
					{
					 TS_SectionFound=TRUE;
					// DAL_DEBUG((" >>>>>TS_SectionFound!!! %u Bytes == %u K\n", g_section_found_read, g_section_found_read/1024));
					DebugMessage("[DAL]PLAYRECi_STTAPI_XXX_AcquireFile->>>>>>TS_SectionFound!!! 0x%x Bytes == 0x%x K\n", g_section_found_read, g_section_found_read/1024);
					 break;
					}
				   else
					{
					 payload_unit_start_indicator_to_filter=1;
					 section_number_to_filter++;
					}
				  }
				}
			   /* Manage offset increment, offset is the current position in the current ts packet */
			   if ((nb_bytes_to_copy_by_section==0)&&(section_syntax_indicator==1))
				{
				 /* Here, we have found nothing, so try next section in the same ts packet */
				 offset+=section_length+3;
				}
			   else
				{
				 /* Here, two cases :													 */
				 /* 1: The section parsed is not a valid section, try next ts packet now */
				 /* 2: Current section reception in progressed, need next ts packet 	 */
				 offset=packet_size-packet_header_size;
				}
			  }
			}
		  }
		}
	  }
	}
  
   /*_______________________________________________________________________________________________*/
  
   /* Deallocate the ts buffer from memory */
   /* ==================================== */
 #if 0
   PLAYRECi_Context.InitParams.MEM_Deallocate(PLAYREC_MEMORY_CACHED,(void *)TS_Buffer);
 #endif
  
   handle->mem_free((void*)TS_Buffer);
  
   /* Close the file */
   /* ============== */
 #if 0
   if (PLAYRECi_Context.InitParams.FS_Close(FILE_Handle)!=0)
	{
	 PLAYRECi_TraceERR(("%s():**ERROR** !!! Unable to close the file !!!\n",__FUNCTION__));
	 return(ST_ERROR_BAD_PARAMETER);
	}
 #endif
   /*_______________________________________________________________________________________________*/
  
   /* Return no errors */
   /* ================ */
   if (TS_SectionFound==TRUE) 
	  return (0); 
   else 
	  return (-1);
 }


int DAL_ParseTest(void)
{
	int			iret = 0;

	iret = DAL_ParseTaskCreate();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_ParseTaskCreate failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_ParseTaskCreate OK\n"));

	iret = DAL_ParseStart();
	if( iret != 0)
	{
		DAL_ERROR(("DAL_ParseStart failed\n"));
		return -1;
	}
	DAL_DEBUG(("DAL_ParseStart OK\n"));


	DAL_TaskDelay(5000);
	
	iret = DAL_ParseStop();
	if( iret != 0)
	{
		return -1;
		DAL_ERROR(("DAL_ParseStop failed\n"));
	}
	DAL_DEBUG(("DAL_ParseStop OK\n"));

	iret = DAL_ParseTaskDelete();
	if( iret != 0)
	{
		return -1;
		DAL_ERROR(("DAL_ParseTaskDelete failed\n"));
	}
	DAL_DEBUG(("DAL_ParseTaskDelete OK\n"));

	return 0;
}

void parse_queue_clear( void)
{
	DAL_ParseControl_t*		p_control = &g_ParseControl;
	int							iret = 0;
	DAL_Message_t*				pMsg = NULL;
	U32							SizeCopy = 0;
	U32							msg_clear = 0;

	DebugMessage("parse_queue_clear enter!\n");

	while( (iret = DAL_MessageQueueReceive( p_control->queueID, &pMsg, sizeof(DAL_Message_t*), &SizeCopy, DAL_CHECK)) >= 0)
	{
		msg_clear++;
		DAL_DEBUG(("clear a message  %s    %d\n", DAL_MessageString( pMsg->MsgType),iret));
		DAL_MessageFree(pMsg);
	}
	DebugMessage("^^^^^^^^^^^^^^^^^^^^^^^^parse_queue_clear  clear %d message!!!\n", msg_clear);

}


