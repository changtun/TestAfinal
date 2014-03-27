#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include "osapi.h"
#include "pvddefs.h"
#include <curl.h>
#include <types.h>
#include <easy.h>

#ifdef USE_ST_PLAYREC
#include "stapp_sysini.h"
#include "stfdma.h"
#include "stpti4.h"
#endif
#include "pbitrace.h"

#include "ring_buffer.h"
#include "dal_wrap.h"
#include "dal_common.h"
#include "dal_message.h"
#include "dal_download.h"
#include "dal_inject.h"


#define DAL_INJECT_TASK_STACK_SIZE		1024*8
#define DAL_INJECT_TASK_PRIOITY			5
#define DAL_INJECT_QUEUE_DEPTH			20
#define DAL_INJECT_QUEUE_DATA_SIZE		DAL_MESSAGE_SIZE

#define P2P_INJECT_DATA_CHUNK_SIZE		188*128		//23.5 k
//#define P2P_INJECT_DATA_CHUNK_SIZE		188*256		//47 k
#define P2P_TS_BUFFER_SIZE				(P2P_INJECT_DATA_CHUNK_SIZE*2)
#define FILTERTASKINTERVAL				15  /*handle 50 ms date*/


typedef enum InjectStatus_e
{
	INJECT_IDEL,
	INJECT_STOP,
	INJECT_PAUSE,
	INJECT_START,
}InjectStatus_t;


typedef struct DAL_InjectControl_s
{
	InjectStatus_t			status;
	DAL_TaskID_t			taskID;
	DAL_QueueID_t			queueID;
	ring_buffer_t*			p_download_buffer;
	unsigned int			inject_total;
	int						notify_first_frame_displayed_flag;
}DAL_InjectControl_t;

#define PTS_MAX_PID 8192
#define PTS_TS_PACKET_SIZE 188
#define PTS_TS_HEADER_SIZE 4
#define PTS_TIME_STAMP_SIZE 5

const long long unsigned system_frequency = 27000000;

static DAL_InjectControl_t 			g_InjectControl;


/*dma inject 使用的全局变量*/
static unsigned int						TS_PacketSize = 188;
static unsigned int						TS_PacketHeaderSize = 0;
static unsigned int						SWTS_Identifier = 1;
static unsigned int 					TS_BufferSrcBase;
static unsigned int						TS_BufferSrcSize;
static unsigned int						TS_BufferMemoryType;
static unsigned int						TS_BufferSrcBaseDevice;
//static unsigned int						TS_BufferDstBaseDevice;
static unsigned int						TS_BufferSrcBaseKernel;
#ifdef USE_ST_PLAYREC
static STFDMA_GenericNode_t*			FDMA_Node,*FDMA_NodeKernel,*FDMA_NodeDevice;
static STFDMA_ChannelId_t	  			FDMA_ChannelId;
static STFDMA_Pool_t		  			FDMA_Pool;
static STFDMA_Block_t		  			FDMA_Block;
static unsigned int		  				FDMA_MemoryType;
#endif

/*stprm 内部自己添加的全局变量*/
extern int						g_first_video_frame_display;
FILE               *g_pTsFile = NULL;
U32 ts_rate=0;
static U32 first_pts=0;
static	char*				Inject_buf[P2P_INJECT_DATA_CHUNK_SIZE]={0};
static int					leng = P2P_INJECT_DATA_CHUNK_SIZE;
static int 					inject_letf=0;
static int					inject_send=0;



/*add by zyliu to count video frame displayed*/
void reset_video_frame_display_count(void);
int	 get_video_frame_display_count(void);
void new_video_frame_display(void);
/*add by zyliu 2011-7-28}}}*/



void* inject_parse_task( void* param);
void inject_start_process( DAL_Message_t* pMsg);
void inject_stop_process( DAL_Message_t* pMsg);
void inject_cont_process( DAL_Message_t* pMsg);

#ifdef USE_ST_PLAYREC
int dal_dma_inject_init( void);
int dal_dma_do_inject( void);
int dal_dma_inject_release(void);
#endif
int dal_inject_is_stop( void);

static U64 PCR_Base(U8*pPkt, int n);   
static unsigned int PCR_Extension(U8*pPkt, int n);	
static U64 CalcuteRate(U8* Buffer,U32 n,U32 uiPacketNum,U64* PCR);
U32 check_and_caculate_pcr(U32 PacketNum,U8* buffer );
U8 check_sync_buffer(U8 *ts_buffer,U8 *sync_buffer,U32* ts_num);
U32 check_and_caculate_pts(void);
U32 Get_play_time(void);
U32 CheckPTSTime(char* buffer,int length);
U64 parse_timestamp(unsigned char *buf);
U32	calc_ts_rate(void);

int DAL_InjectTaskCreate( void)
{
	DAL_InjectControl_t*		p_control = &g_InjectControl;
	int							iret = 0;

	iret = DAL_MessageQueueCreate( &(p_control->queueID), "dal_inject_queue", DAL_INJECT_QUEUE_DEPTH, DAL_INJECT_QUEUE_DATA_SIZE, 0);
	if( iret != 0)
	{
		return -1;
	}
	iret = DAL_TaskCreate( &(p_control->taskID), "dal_inject_task", inject_parse_task, NULL, NULL, DAL_INJECT_TASK_STACK_SIZE, DAL_INJECT_TASK_PRIOITY, 0);
	if( iret != 0)
	{
		DAL_MessageQueueDelete( p_control->queueID);
		return -2;
	}

	p_control->status = INJECT_STOP;
	return 0;
}

int DAL_InjectTaskDelete( void)
{
	DAL_InjectControl_t*		p_control = &g_InjectControl;
	
	DAL_TaskDelete( p_control->taskID);
	DAL_MessageQueueDelete( p_control->queueID);
	p_control->status = INJECT_IDEL;
	return 0;
}

int DAL_InjectStart( void)
{
	DAL_InjectControl_t*		p_control = &g_InjectControl;
	DAL_Message_t*				pMsg = NULL;

	if( p_control->status == INJECT_IDEL)
	{
		//DAL_ERROR(("INJECT_IDEL\n"));
		DebugMessage("[dal]DAL_InjectStart->INJECT_IDEL");
		return -1;
	}

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_INJECT_START;
	DAL_MessageSend( p_control->queueID, &pMsg);
	return 0;
}

int DAL_InjectStop( void)
{
	DAL_InjectControl_t*		p_control = &g_InjectControl;
	DAL_Message_t*				pMsg = NULL;

	if( p_control->status == INJECT_IDEL || p_control->status == INJECT_STOP)
	{
		//DAL_ERROR(("INJECT_IDEL or INJECT_STOP\n"));
		DebugMessage("[dal]DAL_InjectStart->INJECT_IDEL or INJECT_STOP");
		return -1;
	}

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_INJECT_STOP;
	DAL_MessageSend( p_control->queueID, &pMsg);


	while( dal_inject_is_stop() != TRUE)
	{
		//DAL_DEBUG(("waiting for DAL_InjectStop\n"));
		DebugMessage("[dal]DAL_InjectStop->waiting for DAL_InjectStop");
		DAL_TaskDelay(200);
	}
		
	return 0;
}

BOOL DAL_InjectIsPause(void)
{
	DAL_InjectControl_t*		p_control = &g_InjectControl;
	
	return ( p_control->status == INJECT_PAUSE ? 1 : 0 );
}


int DAL_InjectPause( void)
{
	DAL_InjectControl_t*		p_control = &g_InjectControl;

	if( p_control->status != INJECT_START)
	{
		//DAL_ERROR(("NOT INJECT_START\n"));
		DebugMessage("[dal]DAL_InjectPause->NOT INJECT_START");
		return -1;
	}
	p_control->status = INJECT_PAUSE;
	return 0;
}

int DAL_InjectResume( void)
{
	DAL_InjectControl_t*		p_control = &g_InjectControl;
	DAL_Message_t*				pMsg = NULL;

	if( p_control->status != INJECT_PAUSE)
	{
		//DAL_ERROR(("NOT INJECT_PAUSE\n"));
		DebugMessage("[dal]DAL_InjectResume->NOT INJECT_PAUSE");
		return -1;
	}
	p_control->inject_total = 0;
	p_control->status = INJECT_START;
	
	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_INJECT_CONT;
	DAL_MessageSend( p_control->queueID, &pMsg);
	return 0;
}


DAL_QueueID_t DAL_GetInjectQueueID(void)
{
	DAL_InjectControl_t*		p_control = &g_InjectControl;

	if( p_control->status == INJECT_IDEL)
	{
		//DAL_ERROR(("INJECT_IDEL\n"));
		DebugMessage("[dal]DAL_GetInjectQueueID->INJECT_IDEL");
		return -1;
	}
	return p_control->queueID;
}

//extern HI_UNF_STREAM_BUF_S   StreamBuf;

void* inject_parse_task( void* param)
{
	DAL_InjectControl_t*	p_control = &g_InjectControl;
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
		
			if( pMsg->MsgType != DALMSG_INJECT_CONT)
			{

				DebugMessage("[dal]inject_parse_task ->    Recv MSG: %s\n", DAL_MessageString(pMsg->MsgType));
				//DAL_DEBUG(("                              Recv MSG: %s\n", DAL_MessageString(pMsg->MsgType)));
			}
			switch(pMsg->MsgType)
			{
				case DALMSG_INJECT_START:	inject_start_process(pMsg); break;
				case DALMSG_INJECT_STOP:	inject_stop_process(pMsg);  break;
				case DALMSG_INJECT_CONT: 	inject_cont_process(pMsg);  break;
				default: break;
			}
			DAL_MessageFree(pMsg);
		}
		//DAL_TaskDelay(200);/*udp 无法控制码率*/
	
	}
	return (void*)0;
}


void inject_start_process( DAL_Message_t* pMsg)
{
	DAL_InjectControl_t*		p_control = &g_InjectControl;
	DAL_Message_t*				pmsg = NULL;
	int							iret = 0;

	if( p_control->status == INJECT_STOP)
	{
		iret = DAL_DownloadBufferGet(&(p_control->p_download_buffer));
		if( iret != 0)
		{
			//DAL_ERROR(("DAL_DownloadBufferGet failed\n"));
			DebugMessage("[dal]inject_start_process ->DAL_DownloadBufferGet failed");
			return ;
		}


#if 0		
		g_pTsFile = fopen("/mnt/sda/sda1/3000v_ts_out.ts", "rb");
		if (!g_pTsFile)
		{
			DebugMessage("open file  error!");
			return -1;
		}
#endif		

		
#ifdef USE_ST_PLAYREC
		
		dal_dma_inject_init();
#else
		iret = Amlogic_Inject_start();
		if(iret!=0)
			DebugMessage("[dal]inject_start_process ->Amlogic_Inject_start failed");
#endif
		pmsg = DAL_MessageAlloc();
		pmsg->MsgType = DALMSG_INJECT_CONT;
		DAL_MessageSend( p_control->queueID, &pmsg);
		p_control->inject_total = 0;
		p_control->status = INJECT_START;
		p_control->notify_first_frame_displayed_flag = 0;
		leng = P2P_INJECT_DATA_CHUNK_SIZE;
 		inject_letf=0;
		inject_send=0;
	}
	else if( p_control->status == INJECT_START)
	{
		//DAL_DEBUG(("ALREADY INJECT_START\n"));
		DebugMessage("[dal]inject_start_process ->ALREADY INJECT_START");
	}
}



void inject_stop_process( DAL_Message_t* pMsg)
{
	DAL_InjectControl_t*		p_control = &g_InjectControl;
	int							iret = 0;

	if( p_control->status == INJECT_START)
	{
	
#ifdef USE_ST_PLAYREC
		dal_dma_inject_release();
#else
		iret=Amlogic_Inject_stop();
		if(iret!=0)
		{
			//DAL_DEBUG(("DRV_AVCtrl_Reset_TS_Buffer failed!\n"));
			DebugMessage("[dal]inject_stop_process ->DRV_AVCtrl_Reset_TS_Buffer failed!");
		}

//?hisi
#endif
		leng = P2P_INJECT_DATA_CHUNK_SIZE;

		Reset_inject_totalnum();
		first_pts=0;

		p_control->status = INJECT_STOP;
	}
	else if( p_control->status == INJECT_STOP)
	{
		//DAL_DEBUG(("ALREADY INJECT_STOP\n"));
		DebugMessage("[dal]inject_stop_process ->ALREADY INJECT_STOP");
	}
}
void inject_cont_process( DAL_Message_t* pMsg)
{
	DAL_InjectControl_t*		p_control = &g_InjectControl;
	DAL_Message_t*				pmsg = NULL;
	unsigned int				datasize = 0;
	int							iret = 0,len=0;
	
	char  buffer[P2P_INJECT_DATA_CHUNK_SIZE];
	char  sync_buffer[P2P_INJECT_DATA_CHUNK_SIZE];
	char  temp_buffer[P2P_INJECT_DATA_CHUNK_SIZE];
//	U32 ts_pack_num=0;
	
	U32 found=0,length=0,ts_num=0, start_of_sync=0,packet_size=188;


	
	if( p_control->status == INJECT_START)
	{
		ring_buffer_datasize( p_control->p_download_buffer, &datasize);
		if( datasize >= P2P_INJECT_DATA_CHUNK_SIZE)
		{
#ifdef USE_ST_PLAYREC
			iret = ring_buffer_read( p_control->p_download_buffer, (unsigned char*)TS_BufferSrcBase, P2P_INJECT_DATA_CHUNK_SIZE);
			if( iret != 0)
			{
				DAL_ERROR(("ring_buffer_read failed\n"));
			}
			dal_dma_do_inject();
#else


			if( Get_hls_resume_state() == 1)
			{

				 ring_buffer_read( p_control->p_download_buffer,temp_buffer, P2P_INJECT_DATA_CHUNK_SIZE);
#if 1
				for(start_of_sync=0;start_of_sync<P2P_INJECT_DATA_CHUNK_SIZE;start_of_sync++)
				{
					if(temp_buffer[start_of_sync]==0x47)
					{		
						ts_num = (P2P_INJECT_DATA_CHUNK_SIZE-start_of_sync)/packet_size;
						if((temp_buffer[start_of_sync+packet_size]==0x47)
							&&(temp_buffer[start_of_sync+1*packet_size]==0x47)
							&&(temp_buffer[start_of_sync+2*packet_size]==0x47))
						{
							char*  current_packet;
							U8	   pes_header_size = 0,adapt = 0;
							U16    pid=0,firstpid = 0;
							int    ts_header_size = 0,i = 0,firstFlag = 0;
							U8	   timestamp[PTS_TIME_STAMP_SIZE];
							U64    time = 0,firstTime = 0,lastTime = 0;
							U32    ftime=0;
							memcpy(sync_buffer,&temp_buffer[start_of_sync],ts_num*packet_size);
						
							length= PTS_TS_PACKET_SIZE*ts_num;
							for (i= 0; i<length;i += PTS_TS_PACKET_SIZE)
							{
								  current_packet = sync_buffer + i;
								  memcpy(&pid, current_packet + 1, 2);		 
								  pid = ((current_packet[1] & 0x1f) << 8) | current_packet[2];
						
								  if(1 == firstFlag && pid != firstpid)
									 continue;
						
								  /* check PTS and DTS */
								  adapt = (current_packet[3] >> 4) & 0x03;
								  if(adapt == 0)
								  {
									 ts_header_size = PTS_TS_PACKET_SIZE; /* the packet is invalid ?*/
								  }
								  else if(adapt == 1)
								  {
									ts_header_size = PTS_TS_HEADER_SIZE; /* only payload */
								  }
								  else if(adapt == 2)
								  { 
									 ts_header_size = PTS_TS_PACKET_SIZE; /* only adaptation field */
								  }
								  else if(adapt == 3)
								  {
									 ts_header_size = PTS_TS_HEADER_SIZE + current_packet[4] + 1; /* jump the adaptation field */
								  }
								  else
								  {
									 ts_header_size = PTS_TS_PACKET_SIZE; /* not managed */
								  }
						
								  if(ts_header_size + 20 < PTS_TS_PACKET_SIZE && pid < PTS_MAX_PID)
								  {
									 if((current_packet[ts_header_size] == 0x00) && (current_packet[ts_header_size + 1] == 0x00) && (current_packet[ts_header_size + 2] == 0x01))
									 {
										  pes_header_size = current_packet[ts_header_size + 8];
										  U8   videoflag = (unsigned char)current_packet[ts_header_size+3]>>4;
										
										   //		if ((unsigned char)(current_packet[ts_header_size + 3] >> 4) == 0x0E)//PES video
										   if(0x0e == videoflag)		 
										   {
						
											 memcpy(timestamp, current_packet + ts_header_size + 9, PTS_TIME_STAMP_SIZE);
											   
											  time = parse_timestamp(timestamp);
											  if(0 == firstFlag)
											  {
												  DebugMessage("FIND FIND FIND!!!");
												   firstFlag = 1;
												   firstpid = pid;
												   firstTime = time;
											  }
											  break;
										 }
									   }
								  }
							}
				
							ftime=firstTime/(system_frequency/300/1000);

							if(( ftime-first_pts >= Get_hls_pause_time())&&(ftime>first_pts))
							{
								found =1;
								stop_hls_seek_proc();
								DebugMessage("[check_sync_buffer]found the pause time data %d --%d!!!",ftime-first_pts,Get_hls_pause_time());
							   break;
							}
							else
							{
							   start_of_sync+=i;
							
							//	DebugMessage("[check_sync_buffer]found sync ts buffer");
							}
						}
					}
				 }
#else

			if(Get_av_play_time()>=Get_hls_pause_time())
			{
				found =1;
				stop_hls_seek_proc();
				DebugMessage("[check_sync_buffer]found the pause time data %d -%d!!!",Get_av_play_time(),Get_hls_pause_time());
			}
#endif		
				if(found ==1)
				{	
					U32 uResumelive=P2P_INJECT_DATA_CHUNK_SIZE-start_of_sync;
					memset(Inject_buf,0,sizeof(Inject_buf));
					memcpy( Inject_buf,temp_buffer+start_of_sync,uResumelive);
					ring_buffer_read( p_control->p_download_buffer, Inject_buf+uResumelive, start_of_sync);
					
					iret = Amlogic_Inject_Data(Inject_buf,&inject_send);
					if( iret != 0)
					{
						DebugMessage("[dal]inject_cont_process ->Amlogic_Inject_Data failed %d",__LINE__);
					}
					
					set_hls_resume_state(0);
					dal_core_check_frame();

				}
				
				pmsg = DAL_MessageAlloc();
				pmsg->MsgType = DALMSG_INJECT_CONT;
				DAL_MessageSend( p_control->queueID, &pmsg);
				return;
			}
						
			leng=P2P_INJECT_DATA_CHUNK_SIZE -inject_letf;


		
			iret = ring_buffer_read( p_control->p_download_buffer, Inject_buf+inject_letf, leng);
			if( iret != 0)
			{
				DebugMessage("[dal]inject_cont_process ->ring_buffer_read failed");
			}
		//	DebugMessage("[dal]inject_cont_process ->111111111111111111111111");
			inject_letf+=leng;
			inject_send=inject_letf;
			iret = Amlogic_Inject_Data(Inject_buf,&inject_send);
			if( iret != 0)
			{
				DebugMessage("[dal]inject_cont_process ->Amlogic_Inject_Data failed");
			}
			inject_letf-=inject_send;
			if(inject_letf>0)
			{
				memmove(Inject_buf,Inject_buf+inject_send,inject_letf);
			}
			
		//	DebugMessage("[dal]inject_cont_process ---->inject leng:0x%x",inject_send);
		
#endif
		
			p_control->inject_total += inject_send;
			

			//if(g_first_video_frame_display == 0)
			if( 0 == get_video_frame_display_count())
			{
				//DAL_DEBUG(("download avail: %u k   availinject total: %u B == %u k\n", datasize/1024, p_control->inject_total,p_control->inject_total/1024));
				//DebugMessage("[DAL ]inject_cont_process->download avail: 0x%x k   availinject total: 0x%x B == 0x%x k\n", datasize/1024, p_control->inject_total,p_control->inject_total/1024);
				p_control->notify_first_frame_displayed_flag = 0;
			}
#if 1
			//PCR and rate caculate
			if(ts_rate == 0)
			{
				ts_rate=calc_ts_rate();
					
			}
#endif
				if(first_pts == 0)
				{
					first_pts = check_and_caculate_pts();
					pbiinfo("[DAL_inject_cont_process] ->first pts time [%d]ms",first_pts);
			
				}
			

			

		#if 0
			//else if( g_first_video_frame_display == 1)
			else if( get_video_frame_display_count() >= 0 && 0 == p_control->notify_first_frame_displayed_flag)
			{
				//p2p_event_callback(P2P_EVENT_FIRST_VIDEO_PRESENT,NULL);
				// 这里可以返回一个事件，通知上层，视频第一帧已经显示
				// 这个事件也可以由MPlayer 层通过video callback 得知，再由Mplayer 通知上层
				pmsg = DAL_MessageAlloc();
				pmsg->MsgType = DALMSG_FIRST_FRAME_DISPLAY;
				DAL_MessageSend( DAL_GetCoreQueueID(), &pmsg);
				DAL_DEBUG(("notify dal core task first frame has been displayed!!!\n"));
				//g_first_video_frame_display = 2;
				p_control->notify_first_frame_displayed_flag = 1;
			}
		#endif
		 
		}
	
		pmsg = DAL_MessageAlloc();
		pmsg->MsgType = DALMSG_INJECT_CONT;
		DAL_MessageSend( p_control->queueID, &pmsg);
	}
	else if( p_control->status == INJECT_PAUSE)
	{
		//DAL_DEBUG(("INJECT_PAUSE!!!\n"));
		DebugMessage("[DAL]inject_cont_process->INJECT_PAUSE!!!\n");
	}
	else if( p_control->status == INJECT_STOP)
	{
		//DAL_DEBUG(("INJECT_STOP!!!\n"));
		DebugMessage("[DAL]inject_cont_process->INJECT_STOP!!!\n");
	}
}
U32 inject_time_now(void)
{
	DAL_InjectControl_t*		p_control = &g_InjectControl;
	U32 play_seconds=(p_control->inject_total *8)/(ts_rate*1000*1000);

	pbiinfo("\n[inject_time_now]inject_total=%d  time = %d  s !\n",p_control->inject_total,play_seconds);
	return play_seconds;
}

void Reset_inject_totalnum(void)
{
	DAL_InjectControl_t*		p_control = &g_InjectControl;
	pbiinfo("\n[Reset_inject_totalnum] inject_total=%d s !\n",p_control->inject_total);

	p_control->inject_total=0;
	ts_rate=0;

}


/******************************************************************************
* Function : PCR_Base
* parameters :
*		pPkt		    :		(IN)the packet related buffer
*		n		    :		(IN)begin location
* Return :	
*		PCR'base value(only 32bits)
*
* Description :
*		This function count pcr base value,return low 32bits
*
* Author : CK	2009/3/11
******************************************************************************/
static U64 PCR_Base(U8*pPkt, int n)   
{   
    U8 PCRbase_1, PCRbase_2, PCRbase_3, PCRbase_4, PCRbase_5;   
	U32 x,y;
    U64 PCR_basefun;   
   
    /*33 bit PCR_base 以90kHz为单位*/   
    memcpy(&PCRbase_1, &pPkt[n+2],1);   
    memcpy(&PCRbase_2, &pPkt[n+3], 1);   
    memcpy(&PCRbase_3, &pPkt[n+4], 1);   
    memcpy(&PCRbase_4, &pPkt[n+5], 1);   
    memcpy(&PCRbase_5, &pPkt[n+6], 1);  
	x=(PCRbase_1>>7);
	y=(PCRbase_1<<25)|(PCRbase_2 <<17)|(PCRbase_3 <<9)|(PCRbase_4 <<1)|(PCRbase_5 >>7);
 //  PCR_basefun = (PCRbase_1 <<25) | (PCRbase_2 <<17) | (PCRbase_3 <<9) | (PCRbase_4 <<1) |(PCRbase_5 >>7);   
 	PCR_basefun=x*0x100000000+y; 
	   
	pbiinfo("[PCR_Base 1]%x %x %x %x %x\n",PCRbase_1,PCRbase_2,PCRbase_3,PCRbase_4,PCRbase_5);
	pbiinfo("[PCR_Base 2]%llx ",PCR_basefun);
	
    return (PCR_basefun);   

}   

/******************************************************************************
* Function : PCR_Extension
* parameters :
*		pPkt		    :		(IN)the packet related buffer
*		n		    :		(IN)begin location
* Return :	
*		PCR'Extension value(only 9bits)
*
* Description :
*		This function count pcr extension value
*
* Author : CK	2009/3/11
******************************************************************************/
static unsigned int PCR_Extension(U8*pPkt, int n)   
{   
    unsigned char PCRext_h, PCRext_l;   
    unsigned int PCR_extenfun;   
   
    /*9 bit PCR_extension以27MHz为单位*/   
    memcpy(&PCRext_h, &pPkt[n+6],1);   
    memcpy(&PCRext_l, &pPkt[n+7],1);   
  
    PCR_extenfun = ((PCRext_h & 0x01) <<8) |PCRext_l;   
   
   pbiinfo("[PCR_Extension]%x %x \n",PCRext_h,PCRext_l);
   
   pbiinfo("[PCR_Extension]%x ",PCR_extenfun);
    return (PCR_extenfun);   
} 

/******************************************************************************
* Function : CalcuteRate
* parameters :
*		Buffer	:		(IN)the packet related buffer
*		n		:		(IN)begin location
*		uiPacketNum	:	(IN)packet number between two pcrs
*		PCR		:		(IN/OUT)PCR value
* Return :	
*		TS Rate
*
* Description :
*		This function counts ts strem rate
*
* Author : CK	2009/3/11
******************************************************************************/
static U64 CalcuteRate(U8* Buffer,U32 n,U32 uiPacketNum,U64* PCR)
{
	U64 PCRBase,PCRExt, uiDiff=0;
	U64 Rate = 0;
	U64 BytesNum = 0;
	U64 x=0,y=0;
	
	if(PCR[0] ==0)
	{
		PCRBase = PCR_Base(Buffer, n); 
		PCRExt = PCR_Extension(Buffer,n);
		PCR[0] = PCRBase * 300 + PCRExt;
		pbiinfo("[CalcuteRate]PCR[0]%llx \n",PCR[0]);
		return 0; 
	}
	else
	{
		PCRBase = PCR_Base(Buffer, n); 
		PCRExt = PCR_Extension(Buffer,n);
		PCR[1] = PCRBase * 300 + PCRExt;
		if(PCR[1] > PCR[0])
		{
			uiDiff = PCR[1]-PCR[0]; /*ticks between two PCR*/
			BytesNum = (uiPacketNum) * 188 * 8 ;  /*bits between two PCR  188,204*/
			Rate = ((BytesNum* 27) / uiDiff);
			Rate = Rate;
		}
		pbiinfo("[CalcuteRate]PCR[1]%llx \n",PCR[1]);
		pbiinfo("[CalcuteRate]BytesNum %llx \n",BytesNum);
		pbiinfo("[CalcuteRate]Rate%llx \n",Rate);

		//*uiPacketNum = 0;
		//PCR[0] = PCR[1];
		return Rate;
	}

}

U32 check_and_caculate_pcr(U32 PacketNum,U8* buffer )
{
	U32	PCRIntervalPacket = 0;
	U32 iJ=0,packet_size=188,uiPcrPid=0;
	U8 *Bufferbase=NULL,*BufferPointer=NULL;
	U8  TS_Error_Indicator = 0;
	U16	PID = 0;
	U8	ucAdaptField = 0;
	U8	ucTransportScrambling = 0;
	U8	ucPCRflag = 0;
	
	U64 TSRateNew = 0;
	U64  PCR[2]={0,0};
	U32 TsMRate=0;
	
	Bufferbase=	buffer;	
	for(iJ=0; iJ < PacketNum;iJ++)
	{
		BufferPointer = Bufferbase + iJ*packet_size;/*get one packet's head pointer*/
		if(BufferPointer[0] != 0x47)
		{
			continue;
		}

		if(uiPcrPid != 0)
		{
			PCRIntervalPacket++;
		}
		TS_Error_Indicator = (BufferPointer[1] >> 7) & 0x01;
		PID = ((BufferPointer[1] & 0x1f) << 8) | BufferPointer[2];
		ucTransportScrambling = (BufferPointer[3] >> 6);
		ucAdaptField = (BufferPointer[3] >>4) & 0x03;
		
		if( PID == 0x1fff || TS_Error_Indicator == 1 ||ucTransportScrambling!=0)/*NULL packet or error packet ignore it*/
		{
			continue;
		}

		if(ucAdaptField == 0x02 || ucAdaptField == 0x03)
		{
			
			ucPCRflag = (BufferPointer[5] >> 4) & 0x01;
			if(BufferPointer[4] != 0 && ucPCRflag == 0x01)
			{
				if(uiPcrPid == 0 || uiPcrPid == PID)
				{		
					uiPcrPid = PID;
					pbiinfo("[check_and_caculate_pcr] uiPcrPid=%x PID=%x PCRIntervalPacket=%x",uiPcrPid,PID,PCRIntervalPacket);

					TSRateNew= CalcuteRate(BufferPointer, 4,PCRIntervalPacket,PCR);
					if(TSRateNew !=0)
					{
					
						pbiinfo("[check_and_caculate_pcr] TSRateNew=%llx ",TSRateNew);
						TsMRate=(U32)TSRateNew;//1024;

						pbiinfo("[check_and_caculate_pcr] TSRateNew=%d ",TsMRate);						
						break;

					}
					else
					{
						TsMRate	=0;
						pbiinfo("[check_and_caculate_pcr] TSRateNew zero");

					}
				}
			}
		}
	}
	return TsMRate;
}
U8 check_sync_buffer(U8 *ts_buffer,U8 *sync_buffer,U32* ts_num)
{
	U32 	 start_of_sync=0,packet_size=188;
	U8 		found=0;	
	
	for(start_of_sync=0,found=0;(start_of_sync<P2P_INJECT_DATA_CHUNK_SIZE)&&(found==0);start_of_sync++)
	{
	  	if(ts_buffer[start_of_sync]==0x47)
	    {		
	   		*ts_num = (P2P_INJECT_DATA_CHUNK_SIZE-start_of_sync)/packet_size;
	  		if((ts_buffer[start_of_sync+packet_size]==0x47)
	  			&&(ts_buffer[start_of_sync+1*packet_size]==0x47)
	  			&&(ts_buffer[start_of_sync+2*packet_size]==0x47))
	   		{
				found=1;
				memcpy(sync_buffer,&ts_buffer[start_of_sync],(*ts_num)*packet_size);
				DebugMessage("[check_sync_buffer]found sync ts buffer");
				break;
		    }
			else
			{
				continue;	
				
				DebugMessage("[check_sync_buffer] continue ");
			}
		 }
	 }	
return found;
}

U32	calc_ts_rate(void)
{
	char  buffer[P2P_INJECT_DATA_CHUNK_SIZE];
	char  sync_buffer[P2P_INJECT_DATA_CHUNK_SIZE];
	U32  pack_num=0,length=0;
	U32  ts_rate=0;
	
	memset(buffer,0,P2P_INJECT_DATA_CHUNK_SIZE);
	memset(sync_buffer,0,P2P_INJECT_DATA_CHUNK_SIZE);

	memcpy(buffer,Inject_buf,P2P_INJECT_DATA_CHUNK_SIZE);
		
	if(check_sync_buffer(buffer,sync_buffer,&pack_num)==1)	
	{
		ts_rate=check_and_caculate_pcr(pack_num,buffer);
		DebugMessage("[calc_ts_rate]---------->check_sync_buffer found !ts_rate=%d,",ts_rate);
	}
	else
	{
		DebugMessage("[calc_ts_rate]---------->check_sync_buffer failed .");
	
	}
	return ts_rate;

}
#define	DEFAULTTSRATE			38.1 	/*Mbps*/

#ifdef USE_ST_PLAYREC

int dal_dma_inject_init( void)
{
	ST_ErrorCode_t 	   			ErrCode = ST_NO_ERROR;


	TS_PacketSize		= 188;
	TS_PacketHeaderSize = 0;

	/* Allocate a node for FDMA transfer */
	/* --------------------------------- */
	FDMA_MemoryType=SYS_MEMORY_NCACHED|SYS_MEMORY_CONTIGUOUS|SYS_MEMORY_SECURED;
	FDMA_Node=(STFDMA_GenericNode_t *)SYS_Memory_Allocate(FDMA_MemoryType,sizeof(STFDMA_GenericNode_t),256);
	if (FDMA_Node==NULL)
	{
		DAL_DEBUG(("--> Unable to allocate memory for a FDMA node !\n"));
		return -1;
	}
	memset(FDMA_Node,0,sizeof(STFDMA_Node_t));
	FDMA_NodeDevice = (STFDMA_GenericNode_t *)(SYS_Memory_UserToDevice(FDMA_MemoryType,(U32)FDMA_Node));
	FDMA_NodeKernel = (STFDMA_GenericNode_t *)(SYS_Memory_UserToKernel(FDMA_MemoryType,(U32)FDMA_Node));

	/* Lock a dedicated channel */
	/* ------------------------ */
	FDMA_Pool	= STFDMA_DEFAULT_POOL;
	FDMA_Block = STFDMA_2;

	ErrCode=STFDMA_LockChannelInPool(FDMA_Pool,&FDMA_ChannelId,FDMA_Block);
	if (ErrCode != ST_NO_ERROR)
	{
		SYS_Memory_Deallocate(FDMA_MemoryType,(U32)FDMA_Node);
		DAL_DEBUG(("--> Unable to lock a FDMA channel in the pool !\n"));
		return -1;
	}

	/* Allocate TS buffer for source */
	/* ============================= */
	TS_BufferMemoryType = SYS_MEMORY_NCACHED|SYS_MEMORY_CONTIGUOUS|SYS_MEMORY_SECURED;
	TS_BufferSrcBase = (U32)SYS_Memory_Allocate( TS_BufferMemoryType, P2P_TS_BUFFER_SIZE, 512);
	if (TS_BufferSrcBase==0)
	{
		//DAL_DEBUG(("--> Unable to allocate memory for the TS source buffer !\n"));
		return -1;
	}
	TS_BufferSrcBaseDevice = (U32)SYS_Memory_UserToDevice(TS_BufferMemoryType,TS_BufferSrcBase);
	TS_BufferSrcBaseKernel = (U32)SYS_Memory_UserToKernel(TS_BufferMemoryType,TS_BufferSrcBase);
	TS_BufferSrcSize	   = P2P_INJECT_DATA_CHUNK_SIZE;

	return 0;
}


int dal_dma_do_inject(void)
{
	STFDMA_TransferGenericParams_t 		FDMA_TransferParams;
	STFDMA_TransferId_t 		   		FDMA_TransferId;
	ST_ErrorCode_t 	   					ErrCode = ST_NO_ERROR;

	/* Fill the standard node */
	FDMA_Node->Node.SourceAddress_p 				 = (void *)TS_BufferSrcBaseDevice; 
	FDMA_Node->Node.DestinationAddress_p			 = (void *)(ST7105_TSMERGE2_BASE_ADDRESS+0x20*SWTS_Identifier);   // 这里修改swts的index
	FDMA_Node->Node.SourceStride					 = TS_PacketSize+TS_PacketHeaderSize;
	FDMA_Node->Node.DestinationStride				 = 0;
	FDMA_Node->Node.Length							 = TS_PacketSize;
	FDMA_Node->Node.NumberBytes 					 = P2P_INJECT_DATA_CHUNK_SIZE;   //lzy
	FDMA_Node->Node.Next_p							 = NULL;
	FDMA_Node->Node.NodeControl.SourceDirection 	 = STFDMA_DIRECTION_INCREMENTING;
	FDMA_Node->Node.NodeControl.DestinationDirection = STFDMA_DIRECTION_STATIC;
	FDMA_Node->Node.NodeControl.NodeCompleteNotify	 = TRUE;
	FDMA_Node->Node.NodeControl.NodeCompletePause	 = FALSE;
	
	switch(SWTS_Identifier)
	{
		case 0	: FDMA_Node->Node.NodeControl.PaceSignal = STFDMA_REQUEST_SIGNAL_SWTS0; break;
		case 1	: FDMA_Node->Node.NodeControl.PaceSignal = STFDMA_REQUEST_SIGNAL_SWTS1; break;
		case 2	: FDMA_Node->Node.NodeControl.PaceSignal = STFDMA_REQUEST_SIGNAL_SWTS2; break;
		default : FDMA_Node->Node.NodeControl.PaceSignal = STFDMA_REQUEST_SIGNAL_SWTS0; break;
	}

	/* Do the transfer now */
	FDMA_TransferParams.ApplicationData_p		  = NULL;
	FDMA_TransferParams.BlockingTimeout 		  = 0;
	FDMA_TransferParams.CallbackFunction		  = NULL;
	FDMA_TransferParams.Pool					  = FDMA_Pool;
	FDMA_TransferParams.ChannelId				  = FDMA_ChannelId;
	FDMA_TransferParams.NodeAddress_p			  = (void *)(FDMA_NodeDevice);
	FDMA_TransferParams.NodeAddress_Vp			  = (void *)(FDMA_NodeKernel);
	FDMA_TransferParams.FDMABlock				  = FDMA_Block;
	
	ErrCode=STFDMA_StartGenericTransfer(&FDMA_TransferParams,&FDMA_TransferId,TRUE);
	if (ErrCode!=ST_NO_ERROR)
	{
		DAL_DEBUG(("--> Unable to perform the fdma transfer !\n"));
		SYS_Memory_Deallocate(TS_BufferMemoryType,(U32)TS_BufferSrcBase);
		return -1;
	}
	//DAL_DEBUG(("dal_dma_do_inject: inject ok!!!  playrec_buffer_level: %u\n", g_pti0_buffer_level));
	return 0;
}

int dal_dma_inject_release(void)
{
	SYS_Memory_Deallocate(FDMA_MemoryType,(U32)FDMA_Node);
	STFDMA_UnlockChannel(FDMA_ChannelId,FDMA_Block);
	SYS_Memory_Deallocate(TS_BufferMemoryType,(U32)TS_BufferSrcBase);
	return 0;
}
#endif
int dal_inject_is_stop( void)
{
	DAL_InjectControl_t*		p_control = &g_InjectControl;
	return (p_control->status == INJECT_STOP)?1:0;
}




U32 Get_play_time(void)
{
	U32 time_now=0;
    U32 time_temp=check_and_caculate_pts();;
	
	if(time_temp == 0)
	{
		DAL_TaskDelay(500);
		time_temp=check_and_caculate_pts();
		
	}
	time_now = time_temp-first_pts;

	pbiinfo("[Get_play_time] time now :%d ms",time_now);
	return time_now;
}
//find pts to acculate time stamp
U32 check_and_caculate_pts(void)
{
	char  buffer[P2P_INJECT_DATA_CHUNK_SIZE];
	char  sync_buffer[P2P_INJECT_DATA_CHUNK_SIZE];
	U32  pack_num=0,length=0;
	U32  time_stamp=0;

	memset(buffer,0,P2P_INJECT_DATA_CHUNK_SIZE);
	memset(sync_buffer,0,P2P_INJECT_DATA_CHUNK_SIZE);

	
	memcpy(buffer,Inject_buf,P2P_INJECT_DATA_CHUNK_SIZE);
	
	if(check_sync_buffer(buffer,sync_buffer,&pack_num)==1)	
	{
		length= PTS_TS_PACKET_SIZE*pack_num;
		time_stamp=CheckPTSTime(sync_buffer,length);	
		DebugMessage("[check_and_caculate_pts]---------->check_sync_buffer found !length=%x,first_time_stamp=%d ms",length,time_stamp);
	}
	else
	{
		DebugMessage("[check_and_caculate_pts]---------->check_sync_buffer failed .");
	
	}
	return time_stamp;
}


U64 parse_timestamp(unsigned char *buf)
{
     U8 a1=0;
     U16 a2=0,a3=0;
	 U32 a4=0;
     U64 pts;
	 
     a1 = (buf[0] & 0x0F) >>3;
	 a2 = ((buf[1] << 8) | buf[2]) >> 1;
     a3 = ((buf[3] << 8) | buf[4]) >> 1;
     a4 = (buf[0] & 0x06)<<29|(a2 << 15)|(a3);
	 
     pts = a1 *0x100000000 +a4;

	// pbiinfo("[parse_timestamp] buf[%x][%x][%x][%x][%x] pts [%llx]",buf[0],buf[1],buf[2],buf[3],buf[4],pts);
     return pts;
}

U32 CheckPTSTime(char* buffer,int length)
{

  //   unsigned long long  pts_table[PTS_MAX_PID]; /* PTS last value */
  //   unsigned long long  pts_delta_table[PTS_MAX_PID]; /* PTS delta increment */
  //   unsigned char pts_index_table[PTS_MAX_PID];/* PTS index table for the TS packets */
     char* 	current_packet;
     U8 	pes_header_size = 0,adapt = 0;
	 U16 	pid=0,firstpid = 0;
     int 	ts_header_size = 0,i = 0,firstFlag = 0;
     U8 	timestamp[PTS_TIME_STAMP_SIZE];
     U64 	time = 0,firstTime = 0,lastTime = 0;
	 
//     memset(pts_delta_table, 0,  PTS_MAX_PID*(sizeof(long long int)));
//     memset(pts_table, 0,  PTS_MAX_PID*(sizeof(long long int)));
//     memset(pts_index_table, 0,  PTS_MAX_PID*(sizeof(unsigned char)));

     for (i= 0; i<length;i += PTS_TS_PACKET_SIZE)
 	 {
           current_packet = buffer + i;
           memcpy(&pid, current_packet + 1, 2);       
		   pid = ((current_packet[1] & 0x1f) << 8) | current_packet[2];

           if(1 == firstFlag && pid != firstpid)
              continue;

           /* check PTS and DTS */
           adapt = (current_packet[3] >> 4) & 0x03;
           if(adapt == 0)
		   {
              ts_header_size = PTS_TS_PACKET_SIZE; /* the packet is invalid ?*/
           }
		   else if(adapt == 1)
           {
             ts_header_size = PTS_TS_HEADER_SIZE; /* only payload */
           }
		   else if(adapt == 2)
		   { 
              ts_header_size = PTS_TS_PACKET_SIZE; /* only adaptation field */
           }
		   else if(adapt == 3)
		   {
              ts_header_size = PTS_TS_HEADER_SIZE + current_packet[4] + 1; /* jump the adaptation field */
           }
		   else
		   {
              ts_header_size = PTS_TS_PACKET_SIZE; /* not managed */
           }

           if(ts_header_size + 20 < PTS_TS_PACKET_SIZE && pid < PTS_MAX_PID)
           {
              if((current_packet[ts_header_size] == 0x00) && (current_packet[ts_header_size + 1] == 0x00) && (current_packet[ts_header_size + 2] == 0x01))
              {
                   pes_header_size = current_packet[ts_header_size + 8];
                   U8	videoflag = (unsigned char)current_packet[ts_header_size+3]>>4;
				 
                    //       if ((unsigned char)(current_packet[ts_header_size + 3] >> 4) == 0x0E)//PES video
                    if(0x0e == videoflag)         
    				{
						DebugMessage("HAHAHAHAHAHAHAHAHAHAHAHA  find video");

                      memcpy(timestamp, current_packet + ts_header_size + 9, PTS_TIME_STAMP_SIZE);
						
                       time = parse_timestamp(timestamp);
                       if(0 == firstFlag)
                       {
						   DebugMessage("HAHAHAHAHAHAHAHAHAHAHAHA");
                            firstFlag = 1;
                            firstpid = pid;
                            firstTime = time;
                       }
                       lastTime = time;
					   if(time <= 0)
					   {
					   	firstFlag = 0;
					   	continue;
						}

					   
					   break;//不算last了，只取1个
    				}

			  }
           }
		   
 }
  pbiinfo("time Pts %llx\n",firstTime);
 // int tim = (lastTime-firstTime)/(system_frequency/300);
  U32 tim = firstTime/(system_frequency/300/1000);

  pbiinfo("tim %d ms\n",tim);
  return tim;

}



