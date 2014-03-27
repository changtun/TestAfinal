/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name : dal_timeshift.c
* Description : 
* History : 
*	Date				Modification				Name
*	----------			------------			----------
*	2013/10/15			Created					nlv	
******************************************************************************/
/*-----------------------------------------------------*/
/*              Includes												*/
/*-----------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl.h>
#include <types.h>
#include <easy.h>
#include "pbitrace.h"

#include "pvlist.h"
#include "osapi.h"
#include "pvddefs.h"
#include "ring_buffer.h"
#include "dal_common.h"
#include "dal_wrap.h"
#include "dal_message.h"
#include "dal_download.h"
#include "dal_timeshift.h"
//#include "dal_hls.h"
#include "dal.h"
#include "dal_core.h"
#include "anpdebug.h"


/*-----------------------------------------------------*/
/*              Global												*/
/*-----------------------------------------------------*/

static TSHIFT_struct_t 		g_TIMESHIFT;
static U32 					Timeshift_Num=0;

extern DAL_DownloadControl_t	g_DownloadControl;



/*-----------------------------------------------------*/
/*              Exported Types									*/
/*-----------------------------------------------------*/





/*-----------------------------------------------------*/
/*              Local Function Prototypes					*/
/*-----------------------------------------------------*/

/*------------------------------------------------------------------*/
/* void																*/
/*------------------------------------------------------------------*/
void INIT_TIMESHIFT(void)
{
	PV_INIT_LIST_HEAD(&g_TIMESHIFT.tTIMESHIFT_HEADER);
	
	g_TIMESHIFT.total_num =0;

	DAL_SemCreate(&g_TIMESHIFT.sem_request, "time shift struct sem", 1, 0);
}

/*------------------------------------------------------------------*/
/* 0: 成功														*/
/* -1: 失败													*/
/*------------------------------------------------------------------*/
S32 TIMESHIFT_ADD_ONE(Current_timeshift_t node)
{
	struct pvlist_head *PvlistHeader;
	TSHIFT_Addr_Node_t *Newnode;

	
	DebugMessage("timeshift add new member !!!!");

	PvlistHeader = &g_TIMESHIFT.tTIMESHIFT_HEADER;
	Newnode = DAL_Malloc(sizeof(TSHIFT_Addr_Node_t));
	if(Newnode == NULL)
	{
		DebugMessage("[TIMESHIFT_ADD] malloc error!");
		return -1;
	}
	
	memset(Newnode,0,sizeof(TSHIFT_Addr_Node_t));
	strcpy(Newnode->http_address,node.http_address);
	Newnode->time_info = node.time_info;	
	pvlist_add_tail(&Newnode->tNode, PvlistHeader);
	
	g_TIMESHIFT.total_num++;
	DebugMessage("TIMESHIFT_ADD new address:%s ,total num:%d",Newnode->http_address,g_TIMESHIFT.total_num);
	
	return 0;

}
/*------------------------------------------------------------------*/
/* void																*/
/* void																*/
/*------------------------------------------------------------------*/
void TIMESHIFT_DEL_ALL(void)
{
	struct pvlist_head *PvlistHeader,*pdelNode,*pNode;
	TSHIFT_Addr_Node_t *AddressNode;
	
	
	PvlistHeader = &g_TIMESHIFT.tTIMESHIFT_HEADER;
	pdelNode = PvlistHeader->next;

	while(pdelNode != PvlistHeader)
	{
		pNode = pdelNode->next;
		AddressNode=pvlist_entry(pdelNode,TSHIFT_Addr_Node_t,tNode);
		pvlist_del(pdelNode);
		DAL_Free(AddressNode);
		pdelNode = pNode;
	}
	g_TIMESHIFT.total_num =0;
	DAL_SemDelete(g_TIMESHIFT.sem_request);

}

void dal_timeshift_download_start(DAL_Message_t* pMsg)
{
	 struct pvlist_head   *tTemp = &g_TIMESHIFT.tTIMESHIFT_HEADER;
	 DAL_DownloadControl_t* 	 p_control = &g_DownloadControl;
	 TSHIFT_Addr_Node_t   *Anode;
	 DAL_Message_t* 	  Msg = NULL;
	 dal_evt_param_t			 dal_evt_param;
	 S32		iret=0;
	 
	 DebugMessage("[dal_timeshift_download_start]entry total_num:%d,num:%d\n",g_TIMESHIFT.total_num,Timeshift_Num);
	
	 if(p_control->status== DOWNLOAD_STOP)
	 	return;
	 
	if(Timeshift_Num < g_TIMESHIFT.total_num)
	{
		int i=0;
		for(i=0;i<=Timeshift_Num;i++)
		{
			tTemp = tTemp->next;
		}
		Anode = pvlist_entry(tTemp, TSHIFT_Addr_Node_t, tNode);
		Timeshift_Num++;
		Msg = DAL_MessageAlloc();
		if( NULL != Msg)
		{
			Msg->MsgType = DALMSG_TIMESHIFT_TSDOWNLOAD_START;
			memset( Msg->MsgData.DownloadStart.url, 0, sizeof(Msg->MsgData.DownloadStart.url));
			strcpy( Msg->MsgData.DownloadStart.url,Anode->http_address);
			DebugMessage("[dal_timeshift_download_start]entry url:%s\n",Msg->MsgData.DownloadStart.url);
			DAL_MessageSend(DAL_GetDownloadQueueID(), &Msg);
			
			DebugMessage("[dal_timeshift_download_start]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
		}
		else
		{
			DebugMessage("[dal]dal_timeshift_download_start -> DAL_MessageAlloc failed 1 \n");
		}
	}
	else
	{	
		
		iret=check_refresh_timeshift_list();
		if(iret==1)
		{
			//未更新列表
			//DAL_TaskDelay(300);
			Msg = DAL_MessageAlloc();
			if( NULL != Msg)
			{
				Msg->MsgType = DALMSG_STREAM_STOP;
				DAL_MessageSend( DAL_GetCoreQueueID(), &Msg);
			}
			else
			{
				DebugMessage("[dal]dal_timeshift_download_start -> DAL_MessageAlloc failed 33 \n");
			}
			dal_core_event_notify(DAL_CORE_EVENT_TIMESHIFT_PLAYEND,&dal_evt_param);
				
		}
		else
		{
			//列表已经更新
		
			Msg = DAL_MessageAlloc();
			if( NULL != Msg){
				Msg->MsgType = DALMSG_TIMESHIFT_DOWNLOAD;
				DAL_MessageSend(DAL_GetM3u8QueueID(), &Msg);
			}
			else
			{
				DebugMessage("[dal]dal_timeshift_download_start -> DAL_MessageAlloc failed 2 \n");
			}
		}
		
	}
	
//	 DebugMessage("[m3u8_perpare_parse] 2--> p_control %d ",get_core_controlstatus());
//	if(get_core_controlstatus() == 5||get_core_controlstatus() == 4)
//		 m3u8_perpare_parse();
	
//	DebugMessage("[m3u8_perpare_parse] 2-->  p_control %d ",get_core_controlstatus());
	
}

S32 dal_timeshift_download_stop(void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pMsg = NULL;
	dal_evt_param_t				dal_evt_param;	
	C8 str[256] = {0};


	if( p_control->status == DOWNLOAD_IDEL)
	{
		DebugMessage("[dal_timeshift_download_stop]dal_timeshift_download_stop ->DOWNLOAD_IDEL");
		dal_core_event_notify(DAL_CORE_EVENT_TIMESHIFT_STOP_BAD_STATUS,&dal_evt_param);
		return -1;
	}	

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_TIMESHIFT_TSDOWNLOAD_STOP;
	DAL_MessageSend( p_control->queueID, &pMsg);

	while( DAL_DownloadIsStop() != TRUE)
	{
		DebugMessage("[dal_timeshift_download_stop]dal_timeshift_download_stop ->waiting for ts_DownloadStop");
		DAL_TaskDelay(200);
	}
	
	DAL_SemWait(g_TIMESHIFT.sem_request);
	TIMESHIFT_DEL_ALL();
	
	
	DAL_SemSignal(g_TIMESHIFT.sem_request);
	DAL_SemDelete(g_TIMESHIFT.sem_request);
	Timeshift_Num=0;
//	set_hls_resume_state(0);
	return 0;
}


/*------------------------------------------------------------------*/
/* 0:有更新列表											*/
/* 1:没有更新列表											*/
/* 																	*/
/*------------------------------------------------------------------*/
int check_refresh_timeshift_list(void)
{
	if(Timeshift_Num!=g_TIMESHIFT.total_num)
		return 0;
	else
		return 1;
}
/*------------------------------------------------------------------*/
/* 0:允许seek										*/
/* 1:不允许seek										*/
/* 																	*/
/*------------------------------------------------------------------*/
int check_timeshift_listhead(void)
{
	if(Timeshift_Num>1)
		return 0;
	else
		return 1;
}

/*------------------------------------------------------------------*/
/* -1:错误										*/
/* 0:成功										*/
/* 																	*/
/*------------------------------------------------------------------*/
S32 DAL_preDownloadStart(void)
{
	DAL_DownloadControl_t*	down_control = &g_DownloadControl;

	DAL_Message_t*				pMsg = NULL;

	DebugMessage("[DAL_preDownloadStart]- ->entry");

	if( down_control->status == DOWNLOAD_IDEL)
	{
		DebugMessage("[DAL_preDownloadStart]status ->DOWNLOAD_IDEL");
		return -1;
	}


	if( ring_buffer_is_init( &(down_control->download_buffer)) == 0)
	{
		DebugMessage("[DAL_preDownloadStart]ring_buffer_is_init -> DOWNLOAD BUFFER NOT INIT\n");
		return -1;
	}
	ring_buffer_flush(&(down_control->download_buffer));
	
//	INIT_TIMESHIFT();

	/* 清空可能堆积的消息*/
	download_queue_clear();

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_TIMESHIFT_DOWNLOAD;
	DAL_MessageSend( down_control->queueID, &pMsg);
	

	 down_control->parse_pids_flag = PPF_NO_REQUEST;
	
	down_control->status = DOWNLOAD_START;
	return 0;
}
S32 timeshift_Download_Pause(void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	DAL_Message_t*				pMsg = NULL;
	dal_evt_param_t 			dal_evt_param;	

	if( p_control->status == DOWNLOAD_IDEL || p_control->status == DOWNLOAD_STOP)
	{	
		dal_core_event_notify(DAL_CORE_EVENT_TIMESHIFT_STOP_BAD_STATUS,&dal_evt_param);
		DebugMessage("[dal]timeshift_Download_Pause--->TIMESHIFT_IDEL or TIMESHIFT_STOP");
		return -1;
	}
	
		

	pMsg = DAL_MessageAlloc();
	pMsg->MsgType = DALMSG_TIMESHIFT_TSDOWNLOAD_STOP;
	DAL_MessageSend( p_control->queueID, &pMsg);

	while( DAL_DownloadIsStop() != TRUE)
	{
		DebugMessage("[dal]timeshift_Download_Pause ->waiting for ts_DownloadStop");
		DAL_TaskDelay(200);
	}
//	set_hls_resume_state(0);
	
	return 0;
}

S32 timeshift_Download_Resume(void)
{
	DAL_DownloadControl_t*		p_control = &g_DownloadControl;
	dal_evt_param_t 			dal_evt_param;	
	DAL_Message_t*				pMsg = NULL;

	if(p_control->status == DOWNLOAD_STOP)
	{
		ring_buffer_flush(&(p_control->download_buffer));

		download_queue_clear();
		
		pMsg = DAL_MessageAlloc();
		pMsg->MsgType = DALMSG_TIMESHIFT_DOWNLOAD;
		DAL_MessageSend( p_control->queueID, &pMsg);
		
		p_control->parse_pids_flag = PPF_NO_REQUEST;
		p_control->status = DOWNLOAD_START;
		
		
		DebugMessage("[dal]timeshift_Download_Resume");
		return 0;
	}
	else
	{
		dal_core_event_notify(DAL_CORE_EVENT_TIMESHIFT_STOP_BAD_STATUS,&dal_evt_param);
		DebugMessage("[dal]timeshift_Download_Resume--->DOWNLOAD_IDEL or DOWNLOAD_START");
		return -1;
	}

}

void test_timeshift_url(void)
{
	int i=0;
	C8 times[8][256]={			
		"http://192.168.30.210:8080/group1/M00/1C/47/wKge9FKEnSvY0wkTAEzy4P9iN2w1934187",
		"http://192.168.30.210:8080/group1/M00/1C/47/wKge9FKEnWeua1nhAE0W3PuR-508722542",
		"http://192.168.30.210:8080/group1/M00/1C/47/wKge9FKEnaKRkSNWAEup4Okibpk6052294",
		"http://192.168.30.210:8080/group3/M00/07/D8/wKge-1KELVa2SLsaAEz4BKL4n2I1895343",
		"http://192.168.30.210:8080/group3/M00/07/D8/wKge-1KELZK8UcHsAEy_eL-WfaQ2701007",
		"http://192.168.30.210:8080/group3/M00/07/D8/wKge-1KELc7TWDQdAEvnkE1zYoU1736728",
		"http://192.168.30.210:8080/group1/M00/1C/47/wKge9FKEnpK6xkAwAE2mzPt8mNw2605124",
		"http://192.168.30.210:8080/group1/M00/1C/47/wKge9FKEns_TUoXuAEvstLFRR4g5217472"
		};
INIT_TIMESHIFT();

	for(i=0;i<8;i++)
	{
		Current_timeshift_t xs;

		memset(&xs,0,sizeof(Current_timeshift_t));
		strcpy(xs.http_address,times[i]);
		xs.time_info=60;
		TIMESHIFT_ADD_ONE(xs);
	}

}


void timeshift_prev_num(void)
{
	Timeshift_Num=Timeshift_Num-2;
}





