/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name :trcas_sys.c
* Description :   the  trcas sys funcs include the cas init and cas info func
* History :
* Date               Modification                                Name
* ----------         ------------                        ----------
* 2012/06/28        Created                                Li qian 
******************************************************************************/

/*******************************************************/
/*              Includes				                                        */
/*******************************************************/
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "pvddefs.h"
//#include "Tr_Cas.h"
#include "osapi.h" 
#include "ctiapi.h"
#include "ctios.h"
#include "dvbplayer.h"
#include "ca.h"
#include "pbitrace.h"
#include <systime.h>
#include "ctisys.h"
#include "ctico.h"
/********************************************************/
/*               Private Defines and Macros			                    */
/*******************************************************/
typedef struct
{
	U32							eEventType;
	U32							u32EventLen;
	unsigned char*				pEventData;
	unsigned char* 				pPrivateData;
} CTICA_Event_t;

typedef struct cas_task_msg{
	U32 task_id;	
	U32 msgque_id;
	BOOL use_flag;
}cas_task_msg_t;

#define DEBUGPF pbiinfo 
/*******************************************************/
/*               Private Constants                                                    */
/*******************************************************/

/*******************************************************/
/*               Global Variables					                          */
/*******************************************************/

/*******************************************************/
/*               Private Variables (static)			                          */
/*******************************************************/
static CTI_OsSemaphore_t	g_tCTICA_Lock;
static CTI_OsSemaphore_t	g_tCTICA_Msg;
static U32				g_uCTICATask = 0xFFFFFFFF;
static const int				CTICA_Priority = 5;
static const int				CTICA_Size = 1024 * 16;
static CTICA_Event_t 		g_tCallMsg;
static U32  g_CA_task_id;
static CACORE_CAMsg_Notify CTI_Notif_CallBack = NULL;

static U8 drv_task_num = 0;
#define MAX_CAS_TASK_NUM     (2)
#define MAX_CAS_MSG_NUM		 (10)

static cas_task_msg_t g_drv_ca_task_info[2] ;

static unsigned char CAT_DATA[1152];
static unsigned char PMT_DATA[1152];
static unsigned char PMT_CA_EXT_DATA[1152];
static unsigned char CTI_SEND_Data[1152];
static unsigned char NIT_DATA[1152];
static U32				g_uCTICAIPTask = 0xFFFFFFFF;
static const int				CTICAIP_Priority = 5;
static const int				CTICAIP_Size = 1024 * 64;

static U32				g_uCTICAIPSendTask = 0xFFFFFFFF;
static const int				CTICAIPSend_Priority = 5;
static const int				CTICAIPSend_Size = 1024 * 16;


#define		CA_DESC		0x09
static CAPMT_QueryData QueryData;
U32 CTICA_SendMsg(CTICA_Event_t tEvent, CTICA_Event_t *tCallMsg, U32 waitTime);

/*******************************************************/
/*               Private Function prototypes		                          */
/*******************************************************/

/*******************************************************/
/*               Functions							                   */
/*******************************************************/

U32 DRV_OS_TaskCreate(VOID(*task)(PVOID arg),INT iPriority,INT iStackSize)
{
	U32 task_id;
	U8  name[20] ={0};
	U8  i;  
	U32  msg_id;
	U32  os_err;
	
	if((task ==NULL)||(iStackSize <=0))
	{
		pbierror("TRDRV_OS_CreateTask-------> param error statcksize %d \n",iStackSize);
		return 0;
	}

	sprintf(name,"%s_%d","castask",drv_task_num);
	
	os_err = OS_TaskCreate(&task_id,&name,(&(*task)),NULL,NULL,iStackSize,iPriority,0);
		                 //&task_id,(U8*)(&name),(void *(* task)),NULL,NULL,ulStackSize,iPriority,0);
	if(os_err!=OS_SUCCESS)
	{
		pbierror("TRDRV_OS_CreateTask--->create task error [%d]\n", os_err);
		return 0;
	}
	drv_task_num++;

	for(i=0; i<MAX_CAS_TASK_NUM; i++)
	{
		if(g_drv_ca_task_info[i].use_flag ==FALSE)
		{
			break;
		}
	}
	if(i == MAX_CAS_TASK_NUM)
	{
		pbierror("TRDRV_OS_CreateTask---error ---task full\n");
		return (task_id+1);		
	}

	sprintf(name,"casmsg_%d",drv_task_num-1);
	
	os_err = OS_QueueCreate(&msg_id, name,MAX_CAS_MSG_NUM, sizeof(OS_MESSAGE), 0);
	if(OS_SUCCESS != os_err)
	{
		pbierror("TRDRV_OS_CreateTask, create message queue failed! Errcode = %d.\n", os_err);
		g_drv_ca_task_info[i].use_flag =TRUE;
		g_drv_ca_task_info[i].task_id =(task_id);
		return (task_id+1); 
	}
	
	g_drv_ca_task_info[i].use_flag =TRUE;
	g_drv_ca_task_info[i].task_id =(task_id);
	g_drv_ca_task_info[i].msgque_id = msg_id;
	
	
	return (task_id);
}

BYTE DRV_OS_SendMessage(U32 uMailbox, OS_MESSAGE *pstMsg)
{
	int  i = 0;
	U32  os_err;

	pbiinfo("\nTRDRV_OS_SendMessage--------wait......taskid[%x]\n",uMailbox);
//	OS_SemWait(msg_protect_sem);
	
	//pbierror("--------------------TRDRV_OS_SendMessage------------------start----task id [%x]\n",tTaskId);
	if(( pstMsg == NULL)||(uMailbox ==0))
	{
		pbierror("%s  param error \n",__FUNCTION__);
	//	OS_SemSignal(msg_protect_sem);
		return 1;
	}

	for(i =0;i<MAX_CAS_TASK_NUM;i++)
	{
		if((g_drv_ca_task_info[i].task_id == uMailbox)&&(g_drv_ca_task_info[i].msgque_id != 0xffffffff))
		{
			break;
		}
	}

	if( i == MAX_CAS_TASK_NUM)
	{
		pbierror("%s   error  line %d \n",__FUNCTION__,__LINE__);
	//	OS_SemSignal(msg_protect_sem);
		return 1;
	}
	
	os_err = OS_QueueSend(g_drv_ca_task_info[i].msgque_id, pstMsg, sizeof(OS_MESSAGE),200, 0);
	if(os_err != OS_SUCCESS)
	{
		pbierror("TRDRV_OS_SendMessage: send message to queue failed.---[0x%x]\n",os_err);
	//	OS_SemSignal(msg_protect_sem);
		return 1;
	}
	
	pbiinfo("---------------------------TRDRV_OS_SendMessage------------------ok\n");
	//OS_SemSignal(msg_protect_sem);
	return 0;

}
OS_MESSAGE * DRV_OS_ReceiveMessage(U32 uMailbox)
{
	U32  i;
	CTI_OsMessage_t *msg_temp  = NULL; /*这个msg buf 如何释放呢????????????????*/
	U32  os_err;
	U32  actual_size = 0;
	
	//OS_SemWait(msg_protect_sem);

//	pbiinfo("TRDRV_OS_ReceiveMessage------------------start task id [%x]\n",tTaskId);
	if(uMailbox ==0)
	{
		pbierror("%s  param error  \n",__FUNCTION__);
	//	OS_SemSignal(msg_protect_sem);
		return NULL;
	}

	for(i =0;i<MAX_CAS_TASK_NUM;i++)
	{
		if((g_drv_ca_task_info[i].task_id == uMailbox)&&(g_drv_ca_task_info[i].msgque_id != 0xffffffff))
		{
			break;
		}
	}
	//printf("%s   line %d\n",__FUNCTION__,__LINE__);
	if( i == MAX_CAS_TASK_NUM)
	{
		pbierror("%s   error  line %d   id %x\n\n",__FUNCTION__,__LINE__,uMailbox);
	//	OS_SemSignal(msg_protect_sem);
		return NULL;
	}
	// printf("%s   line %d\n",__FUNCTION__,__LINE__);
	msg_temp =(OS_MESSAGE *)OS_MemAllocate(system_p, sizeof(OS_MESSAGE));
	if(msg_temp == NULL)
	{
		pbierror("%s   error  line %d \n",__FUNCTION__,__LINE__);
		//OS_SemSignal(msg_protect_sem);
		return NULL;
	}
	//printf("%s   line %d\n",__FUNCTION__,__LINE__);
	os_err = OS_QueueReceive(g_drv_ca_task_info[i].msgque_id, msg_temp, sizeof(CTI_OsMessage_t),&actual_size, -1 );
	if( os_err < 0 )
	{
//		pbierror("%s   error  line %d \n",__FUNCTION__,__LINE__);
		OS_MemDeallocate(system_p,msg_temp);
	//	OS_SemSignal(msg_protect_sem);
		return NULL;
	}
	if(actual_size>0)
	{
		pbierror("TRDRV_OS_ReceiveMessage-----------------tasid %x -actualsize %d   ok\n",uMailbox,actual_size);
	}
	else
	{
	//	pbierror("%s   error  line %d \n",__FUNCTION__,__LINE__);
		OS_MemDeallocate(system_p,msg_temp);
		msg_temp =NULL;
	}
	//OS_SemSignal(msg_protect_sem);

	return (OS_MESSAGE*)msg_temp;


}




void get_current_frequency(CAPMT_QueryData *data)
{
	data->frequency = QueryData.frequency;
	data->network_id = QueryData.network_id;
	data->service_id = QueryData.service_id;
	data->ts_id = QueryData.ts_id;
printf("\n send data fre = %d,network_id = %d,service_id = %d,ts_id = %d\n",QueryData.frequency,QueryData.network_id,QueryData.service_id,QueryData.ts_id); 	
	return 0;
}

void Build_CA_CLIENT_PMT_QueryData(unsigned char *PMT,unsigned int arg )
{	

	unsigned short FULL_Lengh = 0;	
	unsigned char  *pTmp;
	unsigned int   length = 0;
	unsigned int   lengthDEC = 0;
	unsigned int   lengthTable = 0;
	int            CA_DESC_NO = 0;	
	int            i = 0;
	unsigned char  * DEC_TEMP = NULL;
	unsigned int   Curstation = 0;
	unsigned int   Curstation_temp = 0;
	unsigned int   Curstation_temp1= 0;
	unsigned short program_number = 0;
	CAPMT_QueryData data;
	CTICA_Event_t tEvent;
	CTIASSERT(PMT!=NULL);

       memset(PMT_DATA,0,1152);	


	   PMT_DATA[ 0 ] = 0;		//demux_ID
	get_current_frequency(&data);
	  PMT_DATA[ 1 ] = (unsigned char)(data.frequency>>8);
	  PMT_DATA[ 2 ] = (unsigned char)(data.frequency);

	  PMT_DATA[ 3 ] = (unsigned char)(data.network_id>>8);
	  PMT_DATA[ 4 ] = (unsigned char)(data.network_id);

	  PMT_DATA[ 5 ] = (unsigned char)(data.ts_id>>8);
	  PMT_DATA[ 6 ] = (unsigned char)(data.ts_id);

	  PMT_DATA[ 7 ] = (unsigned char)(data.service_id>>8);
	  PMT_DATA[ 8 ] = (unsigned char)(data.service_id);

	program_number = (PMT[3]<<8)|PMT[4];
	  PMT_DATA[ 9 ] = (unsigned char)(program_number>>8);
	  PMT_DATA[ 10 ] = (unsigned char)(program_number);
	
	  
	lengthTable = PMT[1]&0x0f;
	lengthTable = lengthTable<<8 | PMT[2];
#if 0	
	printf("PMT_DATA: ");
	for(i=0; i<lengthTable; i++)
	{
		printf("%02x ", PMT[i]);
	}
	printf("\n");
#endif	
	pTmp = PMT + 3;

	pTmp = PMT + 10;
	length = pTmp[0]&0x0f;
	length = length<<8|pTmp[1] ;		
	
	pTmp = PMT + 12;
	lengthTable = lengthTable - length - 9;
	 PMT_DATA[ 11 ]  = 0;		//global ca desc count
	Curstation = 12;

	while(length>0)
	{				
		lengthDEC = pTmp[1];		
		if(pTmp[0] == CA_DESC)
		{	
			for(i=0;i<lengthDEC+2;i++)
			{
				PMT_DATA[Curstation] = pTmp[i];	/* ca private data */	
				Curstation++;
			}    			
			PMT_DATA[ 11 ] = PMT_DATA[ 11 ] + 1; /* global descriptor count */

			if( lengthDEC>0 )
			{
				printf("PMT--Program--------CA SYSID=%02x%02x\n",pTmp[2],pTmp[3] );	
				CA_DESC_NO++;
			}
						
			DEC_TEMP = DEC_TEMP + lengthDEC + 2;		
			FULL_Lengh = FULL_Lengh  + lengthDEC + 2;
		}
		pTmp = pTmp + lengthDEC + 2;		
		length = length - lengthDEC - 2;				
	}	
	
	length = 0;
	lengthDEC = 0;
	Curstation_temp1 = Curstation;

	PMT_DATA[Curstation_temp1]  = 0;	/*stream count*/
	Curstation ++;
	while((lengthTable-4)>0)
	{
				
		PMT_DATA[Curstation_temp1] = PMT_DATA[Curstation_temp1] + 1;/*stream count*/
		/*特别说明：视频描述要在前*/
		PMT_DATA[Curstation] = pTmp[0];
		DEBUGPF("Stream type=0x%02x\n",pTmp[0]);
		Curstation++;	
		PMT_DATA[Curstation]  =  pTmp[1]&0x1f;		
		Curstation++;				
		PMT_DATA[Curstation]  =  pTmp[2];/*element pid*/
		Curstation++;		 
		DEBUGPF("Element PID = 0x%02x%02x\n",pTmp[1]&0x1f,pTmp[2]);
		
		length = pTmp[3]&0x0f;
		length = (length << 8) | pTmp[4];		
		pTmp = pTmp + 5;
		
		lengthTable = lengthTable - length - 5;		    		    
		Curstation_temp = Curstation;
		PMT_DATA[Curstation_temp]  = 0; //ca des count
		Curstation++;
		
		while(length>0)
		{				
			lengthDEC = pTmp[1];		
			if(pTmp[0] == CA_DESC)
			{	
				PMT_DATA[Curstation_temp] = PMT_DATA[Curstation_temp] + 1 ;  /*stream descriptor count*/       
				
				for(i=0;i<lengthDEC+2;i++)
				{
			    		PMT_DATA[Curstation] = pTmp[i];		/* ca private data */
			    		Curstation++;
				}    		
				if( lengthDEC>0 )
				{
					DEBUGPF("PMT--Component--------CA SYSID=%02x%02x\n",pTmp[2],pTmp[3]  );	
					CA_DESC_NO++;
				}
				DEC_TEMP = DEC_TEMP + lengthDEC + 2;		
				FULL_Lengh = FULL_Lengh  + lengthDEC + 2;	
			}
			pTmp = pTmp + lengthDEC + 2;
			length = length - lengthDEC - 2;						
		}	
		FULL_Lengh = FULL_Lengh + 4; 
				
	}
	FULL_Lengh = FULL_Lengh + 6; 

#if 1	
	DEBUGPF("PMT_DATA: ");
	for(i=0; i<Curstation; i++)
	{
		DEBUGPF("%02x ", PMT_DATA[i]);
	}
	DEBUGPF("\n");
#endif	
#if 1	
	CTICAS_SendMsgToCAcore(PMT_QUERY, PMT_DATA, Curstation);
#else	
	tEvent.eEventType = PMT_QUERY;
	tEvent.pEventData = PMT_DATA;
	tEvent.u32EventLen = Curstation;
	CTICA_SendMsg(tEvent,NULL,0);   	
#endif	
}	



void Build_CA_CLIENT_PMT_QueryDataExt(unsigned char *PMT,unsigned int arg )
{	

	unsigned short FULL_Lengh = 0;	
	unsigned char  *pTmp;
	unsigned int   length = 0;
	unsigned int   lengthDEC = 0;
	unsigned int   lengthTable = 0;
	int            CA_DESC_NO = 0;	
	int            i = 0;
	unsigned char  * DEC_TEMP = NULL;
	unsigned int   Curstation = 0;
	unsigned int   Curstation_temp = 0;
	unsigned int   Curstation_temp1= 0;
	unsigned short program_number = 0;
	CAPMT_QueryData data;
	CTICA_Event_t tEvent;
	CTIASSERT(PMT!=NULL);

       memset(PMT_DATA,0,1152);	

	   PMT_DATA[ 0 ] = 1;		//demux_ID
	get_current_frequency(&data);
	  PMT_DATA[ 1 ] = (unsigned char)(data.frequency>>8);
	  PMT_DATA[ 2 ] = (unsigned char)(data.frequency);

	  PMT_DATA[ 3 ] = (unsigned char)(data.network_id>>8);
	  PMT_DATA[ 4 ] = (unsigned char)(data.network_id);

	  PMT_DATA[ 5 ] = (unsigned char)(data.ts_id>>8);
	  PMT_DATA[ 6 ] = (unsigned char)(data.ts_id);

	  PMT_DATA[ 7 ] = (unsigned char)(data.service_id>>8);
	  PMT_DATA[ 8 ] = (unsigned char)(data.service_id);

	program_number = (PMT[3]<<8)|PMT[4];
	  PMT_DATA[ 9 ] = (unsigned char)(program_number>>8);
	  PMT_DATA[ 10 ] = (unsigned char)(program_number);
	
	  
	lengthTable = PMT[1]&0x0f;
	lengthTable = lengthTable<<8 | PMT[2];
#if 0	
	printf("PMT_DATA: ");
	for(i=0; i<lengthTable; i++)
	{
		printf("%02x ", PMT[i]);
	}
	printf("\n");
#endif	
	pTmp = PMT + 3;

	pTmp = PMT + 10;
	length = pTmp[0]&0x0f;
	length = length<<8|pTmp[1] ;		
	
	pTmp = PMT + 12;
	lengthTable = lengthTable - length - 9;
	 PMT_DATA[ 11 ]  = 0;		//global ca desc count
	Curstation = 12;

	while(length>0)
	{				
		lengthDEC = pTmp[1];		
		if(pTmp[0] == CA_DESC)
		{	
			for(i=0;i<lengthDEC+2;i++)
			{
				PMT_DATA[Curstation] = pTmp[i];	/* ca private data */	
				Curstation++;
			}    			
			PMT_DATA[ 11 ] = PMT_DATA[ 11 ] + 1; /* global descriptor count */

			if( lengthDEC>0 )
			{
				printf("PMT--Program--------CA SYSID=%02x%02x\n",pTmp[2],pTmp[3] );	
				CA_DESC_NO++;
			}
						
			DEC_TEMP = DEC_TEMP + lengthDEC + 2;		
			FULL_Lengh = FULL_Lengh  + lengthDEC + 2;
		}
		pTmp = pTmp + lengthDEC + 2;		
		length = length - lengthDEC - 2;				
	}	
	
	length = 0;
	lengthDEC = 0;
	Curstation_temp1 = Curstation;

	PMT_DATA[Curstation_temp1]  = 0;	/*stream count*/
	Curstation ++;
	while((lengthTable-4)>0)
	{
				
		PMT_DATA[Curstation_temp1] = PMT_DATA[Curstation_temp1] + 1;/*stream count*/
		/*特别说明：视频描述要在前*/
		PMT_DATA[Curstation] = pTmp[0];
		DEBUGPF("Stream type=0x%02x\n",pTmp[0]);
		Curstation++;	
		PMT_DATA[Curstation]  =  pTmp[1]&0x1f;		
		Curstation++;				
		PMT_DATA[Curstation]  =  pTmp[2];/*element pid*/
		Curstation++;		 
		DEBUGPF("Element PID = 0x%02x%02x\n",pTmp[1]&0x1f,pTmp[2]);
		
		length = pTmp[3]&0x0f;
		length = (length << 8) | pTmp[4];		
		pTmp = pTmp + 5;
		
		lengthTable = lengthTable - length - 5;		    		    
		Curstation_temp = Curstation;
		PMT_DATA[Curstation_temp]  = 0; //ca des count
		Curstation++;
		
		while(length>0)
		{				
			lengthDEC = pTmp[1];		
			if(pTmp[0] == CA_DESC)
			{	
				PMT_DATA[Curstation_temp] = PMT_DATA[Curstation_temp] + 1 ;  /*stream descriptor count*/       
				
				for(i=0;i<lengthDEC+2;i++)
				{
			    		PMT_DATA[Curstation] = pTmp[i];		/* ca private data */
			    		Curstation++;
				}    		
				if( lengthDEC>0 )
				{
					DEBUGPF("PMT--Component--------CA SYSID=%02x%02x\n",pTmp[2],pTmp[3]  );	
					CA_DESC_NO++;
				}
				DEC_TEMP = DEC_TEMP + lengthDEC + 2;		
				FULL_Lengh = FULL_Lengh  + lengthDEC + 2;	
			}
			pTmp = pTmp + lengthDEC + 2;
			length = length - lengthDEC - 2;						
		}	
		FULL_Lengh = FULL_Lengh + 4; 
				
	}
	FULL_Lengh = FULL_Lengh + 6; 

#if 1	
	DEBUGPF("PMT_DATA: ");
	for(i=0; i<Curstation; i++)
	{
		DEBUGPF("%02x ", PMT_DATA[i]);
	}
	DEBUGPF("\n");
#endif	
#if 1	
	CTICAS_SendMsgToCAcore(PMT_QUERY, PMT_DATA, Curstation);
#else	
	tEvent.eEventType = PMT_QUERY;
	tEvent.pEventData = PMT_DATA;
	tEvent.u32EventLen = Curstation;
	CTICA_SendMsg(tEvent,NULL,0);   	
#endif	
}	



void Build_CA_CLIENT_CAT_QueryData(unsigned char *CAT,unsigned int arg )
{		
	unsigned short FULL_Lengh = 0;
	int            i=0, iError = 0; 
	int            CA_DESC_NO = 0;		
	unsigned char  *pTmp;
	unsigned int   length = 0;
	unsigned int   lengthDEC = 0;
	unsigned int   Curstation = 0;
	CTICA_Event_t tEvent;
	CTIASSERT(CAT!=NULL);
	
       memset(CAT_DATA,0,1152);	

	/* ca system count */
	CAT_DATA[ 0 ]=0;
	
	Curstation = 1;
	
	pTmp  =  CAT + 8;

	length = CAT[1]&0x0f;
	length = (length << 8) | CAT[2];		
	length = length - 5;
	while((length-4)>0)
	{				
		lengthDEC = pTmp[1];		
		if(pTmp[0] == CA_DESC)
		{
			for(i=0;i<lengthDEC+2;i++)
			{
			    CAT_DATA[Curstation] = pTmp[i];	/* ca private data */	
			    Curstation++;
			}    
			CAT_DATA[0] = CAT_DATA[0]+1;/* ca system count */
			if( lengthDEC>0 )
			{
				CA_DESC_NO++;
			}			
			FULL_Lengh = FULL_Lengh  + lengthDEC + 2;
		}
		pTmp = pTmp + lengthDEC + 2;		
		length = length - lengthDEC - 2;				
	}
	
	FULL_Lengh = FULL_Lengh  + 3;
		
	//DEBUGPF("CAT-----------CA_DESC_NO = %d\n",CA_DESC_NO);
	if(CA_DESC_NO>0)
	{
#if 1	
		DEBUGPF("CAT_DATA: ");
		for(i=0; i<Curstation; i++)
		{
			DEBUGPF("%02x ", CAT_DATA[i]);
		}
		DEBUGPF("\n");
#endif	
#if 1	
		iError = CTICAS_SendMsgToCAcore(CAT_QUERY, CAT_DATA, Curstation);
		if(iError != 0)
		{
			pbierror("CAInit CTICAS_CAcoreInit---->error:%d \n", iError);
			return 1;
		}
#else
		//DEBUGPF("-------Build_CA_CLIENT__CAT_UPDATE\n"); 
		tEvent.eEventType = CAT_QUERY;
		tEvent.pEventData = CAT_DATA;
		tEvent.u32EventLen = Curstation;
		CTICA_SendMsg(tEvent,NULL,0);
#endif		
	}   
}	

#define NIT_MAX_SECTION_LENGTH	1021 
#define NIT_MIN_SECTION_LENGTH	19
#define CTICA_REGION_CONTROL_TAG	0x89

void Build_CA_CLIENT_NIT_QueryData(unsigned char *NIT,unsigned int arg )
{
	int				i, j, k, num=0;
	unsigned short 	usNITServiceNum = 0;
	unsigned short 	section_length_tmp;
	unsigned short 	network_id = 0xffff;
	unsigned short 	version_number;
	unsigned short 	current_next_indicator;
	unsigned short 	section_number;
	unsigned short 	last_section_number;
	unsigned short 	network_descriptors_length;
	unsigned short 	transport_stream_loop_length;
	unsigned short 	transport_stream_id;
	unsigned short 	original_network_id;
	unsigned short 	transport_descriptors_length;
	unsigned char* buffer = NIT;
	int Curstation =0;
	unsigned char find_region_control = 0;

	section_length_tmp=((buffer[1]&0x0f)<<8)+buffer[2];
	if((section_length_tmp > NIT_MAX_SECTION_LENGTH ) 
		||(section_length_tmp < NIT_MIN_SECTION_LENGTH))
	{
		return 1;
	}

	network_id=(buffer[3]<<8)+buffer[4];
	version_number=(buffer[5]>>1)&0x1f;
	current_next_indicator=buffer[5]&0x01;
	section_number=buffer[6];
	last_section_number=buffer[7];
	network_descriptors_length=((buffer[8]&0x0f)<<8)+buffer[9];
	/*network_descriptors*/
	for(i=0;i<network_descriptors_length;)
	{
		unsigned short 	descriptor_length = buffer[i+11];
		switch (buffer[i+10])
		{
			case CTICA_REGION_CONTROL_TAG:
			{
				find_region_control = 1;
				for(j=0;j<descriptor_length+2;j++)
				{
					NIT_DATA[ Curstation ] = buffer[i+10+j];
					Curstation++;
				}
			}
			break;
			default:
			break;
			
		}
		i+= descriptor_length+2;
	}
	
#if 0
	buffer+=(network_descriptors_length+10);

	transport_stream_loop_length=((buffer[0]&0x0f)<<8)+buffer[1];

	for(i=0;i<transport_stream_loop_length;)
	{
		transport_stream_id=(buffer[i+2]<<8)+buffer[i+3];
		original_network_id=(buffer[i+4]<<8)+buffer[i+5];
		transport_descriptors_length=((buffer[i+6]&0x0f)<<8)+buffer[i+7];
		
		for(j=0;j<transport_descriptors_length;)
		{
			unsigned short 	descriptor_length= buffer[i+j+9];
			unsigned int		temp_freq=0,temp_SR=0;

				      
			j+=(descriptor_length+2);
		} 
		i+=(transport_descriptors_length+6);
	}
#endif
//	DEBUGPF("find_region_control[%d] \n", find_region_control);
	if(find_region_control==1)
	{
#if 0
			DEBUGPF("NIT_DATA: ");
			for(i=0; i<Curstation; i++)
			{
				DEBUGPF("%02x ", NIT_DATA[i]);
			}
			DEBUGPF("\n");
#endif			
			CTICAS_SendMsgToCAcore(NIT_QUERY, NIT_DATA, Curstation);	
	}
	return;
}
	


void Build_CA_CLIENT_STOP_PROG_QueryData(unsigned char *PMT,unsigned int arg )
{
	CTICA_Event_t tEvent;
	CTI_SEND_Data[ 0 ] = 1;	//stop emm
	CTI_SEND_Data[ 1 ] = 1;	//stop ecm
	CTI_SEND_Data[ 2 ] = 1;	//stop all program 
	CTI_SEND_Data[ 3 ] = 0xFF;
	CTI_SEND_Data[ 4 ] = 0xFF;	//program number
	CTI_SEND_Data[ 5 ] = 0;
#if 1
CTICAS_SendMsgToCAcore(STOP_PROG_QUERY, CTI_SEND_Data, 5);	
#else
	tEvent.eEventType = STOP_PROG_QUERY;
	tEvent.pEventData = CTI_SEND_Data;
	tEvent.u32EventLen = 6;
	CTICA_SendMsg(tEvent,NULL,0);
#endif
}

void CA_CLIENT_SEND_CAT_QUERY( void* param, unsigned int arg1 )
{
	Build_CA_CLIENT_CAT_QueryData( (unsigned char *)param,arg1);
}

void CA_CLIENT_SEND_PMT_QUERY( void* param, unsigned int arg1 )
{
	Build_CA_CLIENT_PMT_QueryData( (unsigned char *)param,arg1);
}

void CA_CLIENT_SEND_PMT_QUERY_EXT( void* param, unsigned int arg1 )
{
	Build_CA_CLIENT_PMT_QueryDataExt( (unsigned char *)param,arg1);
}

void CA_CLIENT_SEND_NIT_QUERY( void* param, unsigned int arg1 )
{
	Build_CA_CLIENT_NIT_QueryData( (unsigned char *)param,arg1);
}

void CA_CLIENT_SEND_STOP_PROG_QUERY( void* param, unsigned int arg1 )
{		
	Build_CA_CLIENT_STOP_PROG_QueryData( (unsigned char *)param,arg1);
}

void CA_CLIENT_SEND_CHECK_PIN_QUERY( void* param, unsigned int arg1 )
{
	unsigned char* buffer = (char*)param;
	unsigned int length = arg1;
	int i = 0;
	
	if(length!=4)
	{
		pbierror("[CA Client] PIN Code length is error --------\n");
	}
	else
	{
		pbiinfo("[CA Client] PinCode = %x,%x,%x,%x\n",buffer[0],buffer[1],buffer[2],buffer[3]);
		memset(CTI_SEND_Data,0,1024);
		CTI_SEND_Data[ 0 ] = 4;
		for(i=0;i<4;i++)
		{
			CTI_SEND_Data[ 1 + i ] = buffer[ i ] +0x30;
		}
		
		DEBUGPF("check pin: ");
		for(i=0; i<5; i++)
		{
			DEBUGPF("%02x ", CTI_SEND_Data[i]);
		}
		DEBUGPF("\n");
		
		CTICAS_SendMsgToCAcore(CHECK_PIN_QUERY, CTI_SEND_Data, 5);	
	}
}

void CA_CLIENT_SEND_CHANGE_PIN_QUERY( void* param, unsigned int arg1 )
{
	CTI_PinCode *pin = (CTI_PinCode*)param;
	int Curstation = 0;
	int i = 0;

	memset(CTI_SEND_Data,0,1024);
	
	CTI_SEND_Data[ Curstation ] = 4;
	Curstation++;

	for(i=0;i<4;i++)
	{
		CTI_SEND_Data[ Curstation ] = pin->OLD_PinCode[ i ] + 0x30;	//change to assic code
		Curstation++;
	}

	CTI_SEND_Data[ Curstation ] = 4;
	Curstation++;

	for(i=0;i<4;i++)
	{
		CTI_SEND_Data[ Curstation ] = pin->NEW_PinCode[ i ] + 0x30;//change to assic code
		Curstation++;
	}
	
	DEBUGPF("CHANGE_PIN: ");
	for(i=0; i<Curstation; i++)
	{
		DEBUGPF("%02x ", CTI_SEND_Data[i]);
	}
	DEBUGPF("\n");
	
	CTICAS_SendMsgToCAcore(CHANGE_PIN_QUERY, CTI_SEND_Data, Curstation);	
	
}

void CA_CLIENT_SEND_IPPV_QUERY( void* param, unsigned int arg1 )
{
	CTI_IPPV* IPPV = (CTI_IPPV*)param;
	int Curstation = 0;
	int i = 0;

	memset(CTI_SEND_Data,0,1024);

	CTI_SEND_Data[ Curstation ] = IPPV->IPPV_FLag;
	Curstation++;
	CTI_SEND_Data[ Curstation ]  = (unsigned char)(IPPV->IPPV_ProgramNo>>24);
	Curstation++;
	CTI_SEND_Data[ Curstation ]  = (unsigned char)(IPPV->IPPV_ProgramNo>>16);
	Curstation++;
	CTI_SEND_Data[ Curstation ]  = (unsigned char)(IPPV->IPPV_ProgramNo>>8);
	Curstation++;
	CTI_SEND_Data[ Curstation ]  = (unsigned char)(IPPV->IPPV_ProgramNo);
	Curstation++;
	
	CTI_SEND_Data[ Curstation ] = 4;	//pin code length;
	Curstation++;

	for(i=0;i<4;i++)
	{
		CTI_SEND_Data[ Curstation ] = IPPV->PinCode[ i ] + 0x30;//change to asic code
		Curstation++;
	}
 
	DEBUGPF("IPPV: ");
	for(i=0; i<Curstation; i++)
	{
		DEBUGPF("%02x ", CTI_SEND_Data[i]);
	}
	DEBUGPF("\n");
		
	CTICAS_SendMsgToCAcore(IPPV_QUERY, CTI_SEND_Data, Curstation);	
	
}
void CA_CLIENT_SEND_SC_INFO_QUERY( void* param, unsigned int arg1 )
{
	CTICAS_SendMsgToCAcore(SC_INFO_QUERY, NULL, 0);
}
void CA_CLIENT_SEND_PPID_QUERY( void* param, unsigned int arg1 )
{
	CTICAS_SendMsgToCAcore(PPID_QUERY, NULL, 0);
}
void CA_CLIENT_SEND_PPID_INFO_QUERY( void* param, unsigned int arg1 )
{
	CTI_PPID_Info* PPID = (CTI_PPID_Info*)param;
	int Curstation = 0;
	int i = 0;
	
	CTI_SEND_Data[ Curstation ] = 4;	//pin code length;
	Curstation++;

	for(i=0;i<4;i++)
	{
		CTI_SEND_Data[ Curstation ] = PPID->PinCode[ i ] + 0x30;//change to asicc code
		Curstation++;
	}

	CTI_SEND_Data[ Curstation ]  = (unsigned char)(PPID->PPID>>16);
	Curstation++;
	CTI_SEND_Data[ Curstation ]  = (unsigned char)(PPID->PPID>>8);
	Curstation++;
	CTI_SEND_Data[ Curstation ]  = (unsigned char)(PPID->PPID);
	Curstation++;
	
	DEBUGPF("IPPV_INFO: ");
	for(i=0; i<Curstation; i++)
	{
		DEBUGPF("%02x ", CTI_SEND_Data[i]);
	}
	DEBUGPF("\n");
		
	CTICAS_SendMsgToCAcore(PPID_INFO_QUERY, CTI_SEND_Data, Curstation);	
	
}
void CA_CLIENT_SEND_PPV_LIST_INFO_QUERY( void* param, unsigned int arg1 )
{
	CTI_PPV_List* PPV_List = (CTI_PPV_List*)param;
	int Curstation = 0;
	int i = 0;
	
	CTI_SEND_Data[ Curstation ]  = (unsigned char)(PPV_List->PPID_ID>>16);
	Curstation++;
	CTI_SEND_Data[ Curstation ]  = (unsigned char)(PPV_List->PPID_ID>>8);
	Curstation++;
	CTI_SEND_Data[ Curstation ]  = (unsigned char)(PPV_List->PPID_ID);
	Curstation++;

	CTI_SEND_Data[ Curstation ]  = PPV_List->List_Type;
	Curstation++;
	CTI_SEND_Data[ Curstation ]  = PPV_List->List_Page;
	Curstation++;

	
	DEBUGPF("IPPV_LIST: ");
	for(i=0; i<Curstation; i++)
	{
		DEBUGPF("%02x ", CTI_SEND_Data[i]);
	}
	DEBUGPF("\n");

	CTICAS_SendMsgToCAcore(PPV_LIST_INFO_QUERY, CTI_SEND_Data, Curstation);		
}

void CA_CLIENT_SEND_ANTI_RECORD_QUERY( void* param, unsigned int arg1 )
{
	CTICAS_SendMsgToCAcore(ANTI_RECORD_QUERY, NULL, 0);
}

void CA_CLIENT_SEND_SET_PARENTAL_QUERY( void* param, unsigned int arg1 )
{
	CTI_Parental* Parental = (CTI_Parental*)param;
	int Curstation = 0;
	int i = 0;
	
	CTI_SEND_Data[ Curstation ] = 4;	//pin code length;
	Curstation++;

	for(i=0;i<4;i++)
	{
		CTI_SEND_Data[ Curstation ] = Parental->PinCode[ i ] + 0x30;//change to asicc code
		Curstation++;
	}
	
	/****************************/
	/*世新专用父母锁
	/*
	/*等级1--4				*/

	/*		等级2--5			*/

	/*		等级3--6			*/

	/*		等级4--7			*/

	/*		等级5--8			*/
	/*
	/****************************/
	Parental->Parental_rate +=3;
	CTI_SEND_Data[ Curstation ]  = Parental->Parental_rate;
	Curstation++;
	
	DEBUGPF("Parental: ");
	for(i=0; i<Curstation; i++)
	{
		DEBUGPF("%02x ", CTI_SEND_Data[i]);
	}
	DEBUGPF("\n");
		
	CTICAS_SendMsgToCAcore(SET_PARENTAL_QUERY, CTI_SEND_Data, Curstation);	
	
}

void CA_CLIENT_SEND_USER_VIEW_QUERY( void* param, unsigned int arg1 )
{
	CTI_UserView* UserView = (CTI_UserView*)param;
	int Curstation = 0;
	int i = 0;

	CTI_SEND_Data[ Curstation ] = UserView->type;	// 0 读取数据1,设置或取消
	Curstation++;
	
	CTI_SEND_Data[ Curstation ] = 4;	//pin code length;
	Curstation++;

	for(i=0;i<4;i++)
	{
		CTI_SEND_Data[ Curstation ] = UserView->PinCode[ i ] + 0x30;//change to asicc code
		Curstation++;
	}
	
	if(UserView->type>0)
	{
		CTI_SEND_Data[ Curstation ]  = UserView->flag;	/* 0 取消设置1进行设置*/
		Curstation++;
		for(i=0;i<8;i++)
		{
			CTI_SEND_Data[ Curstation ]  = UserView->control_start_time[ i ];
			Curstation++;
		}
		for(i=0;i<8;i++)
		{
			CTI_SEND_Data[ Curstation ]  = UserView->control_end_time[ i ];
			Curstation++;
		}
		for(i=0;i<2;i++)
		{
			CTI_SEND_Data[ Curstation ]  = UserView->control_start_channel[ i ];
			Curstation++;
		}
		for(i=0;i<2;i++)
		{
			CTI_SEND_Data[ Curstation ]  = UserView->control_end_channel[ i ];
			Curstation++;
		}
		
		CTI_SEND_Data[ Curstation ]  = UserView->status;
		Curstation++;
	}

		
	DEBUGPF("USERVIEW: ");
	for(i=0; i<Curstation; i++)
	{
		DEBUGPF("%02x ", CTI_SEND_Data[i]);
	}
	DEBUGPF("\n");
		
	CTICAS_SendMsgToCAcore(USER_VIEW_QUERY, CTI_SEND_Data, Curstation);	
	
}

void CA_CLIENT_SEND_PARENTAL_CHANGE_QUERY( void* param, unsigned int arg1 )
{
	unsigned char* buffer = (unsigned char*)param;
	int Curstation = 0;
	int i = 0;
	
	CTI_SEND_Data[ Curstation ] = 4;	//pin code length;
	Curstation++;

	for(i=0;i<4;i++)
	{
		CTI_SEND_Data[ Curstation ] = buffer[ i ] + 0x30;//change to asicc code
		Curstation++;
	}
	
	DEBUGPF("ParentalChange: ");
	for(i=0; i<Curstation; i++)
	{
		DEBUGPF("%02x ", CTI_SEND_Data[i]);
	}
	DEBUGPF("\n");
		
	CTICAS_SendMsgToCAcore(PARENTAL_CHANGE_QUERY, CTI_SEND_Data, Curstation);	
	
}

void CA_CLIENT_SEND_CHANGE_PID_QUERY( void* param, unsigned int arg1 )
{

}

void CTICA_CLIENT_SEND_CA_MESSAGE_QUERY( void* param, unsigned int arg1 )
{
	BYTE* ptr  = (BYTE*)param;
	int Curstation = arg1;
	int i = 0;
	
	for(i=0;i<Curstation;i++)
	{
		CTI_SEND_Data[ i ] = ptr[i];
	}

	DEBUGPF("Message_code: ");
	for(i=0; i<Curstation; i++)
	{
		DEBUGPF("%02x ", CTI_SEND_Data[i]);
	}
	DEBUGPF("\n");
		
	CTICAS_SendMsgToCAcore(CA_MESSAGE_QUERY, CTI_SEND_Data, Curstation);	

}

void CTICA_CLIENT_SEND_CA_IP_QUERY(BYTE* ipaddr, void* param, unsigned int arg1 )
{
#if 0
	BYTE* ptr  = (BYTE*)param;
	int Curstation = arg1;
	int i = 0;
	
	for(i=0;i<Curstation;i++)
	{
		CTI_SEND_Data[ Curstation ] = ptr[i];
	}

	DEBUGPF("IP: ");
	for(i=0; i<Curstation; i++)
	{
		DEBUGPF("%02x ", CTI_SEND_Data[i]);
	}
	DEBUGPF("\n");
		
	CTICAS_SendMsgToCAcore(CA_IP_DATA_QUERY, CTI_SEND_Data, Curstation);	
#else

	U16 port;
	unsigned char* stb_ipaddr = NULL;
	BYTE buffer[100];
	if(ipaddr[5]==0&&ipaddr[6]==0&&ipaddr[7]==0&&ipaddr[8]==0)	//ipaddr = 0.0.0.0. 广播发送
	{
		//广播发送
		port = (ipaddr[9]<<8)|ipaddr[10];
		sprintf(buffer,"%d.%d.%d.%d",ipaddr[5],ipaddr[6],ipaddr[7],ipaddr[8]);
		DEBUGPF("ipaddr =%s,port = %d\n",buffer,port);
		CTI_Client_SendDataBroadcast(port,param,arg1);
	}
	else
	{
/* commit:
*		子母卡网络认证有的盒子发过来的
*	IP 地址是反序列的，比如ip 地址是192.168.10.2 
*	过来的数据是2.10.168.192,因此要反转处理一下
*	端口也做相应处理。by qiuzw
*/
		stb_ipaddr = CTICASTB_GetAddress();
		if(stb_ipaddr!=NULL)
		{
			if(stb_ipaddr[0] ==ipaddr[8]&&stb_ipaddr[1]==ipaddr[7]&&stb_ipaddr[2]==ipaddr[6]) //x.2.168.192 
			{
				port = (ipaddr[10]<<8)|ipaddr[9];
				sprintf(buffer,"%d.%d.%d.%d",ipaddr[8],ipaddr[7],ipaddr[6],ipaddr[5]);
				DEBUGPF("ipaddr =%s,port = %d\n",buffer,port);
				CTI_Client_SendData(buffer,port,param,arg1);
				return;
			}
		}
		//192.168.2.xx 
		port = (ipaddr[9]<<8)|ipaddr[10];
		sprintf(buffer,"%d.%d.%d.%d",ipaddr[5],ipaddr[6],ipaddr[7],ipaddr[8]);
		DEBUGPF("ipaddr =%s,port = %d\n",buffer,port);
		CTI_Client_SendData(buffer,port,param,arg1);
	}
#endif
}


void CTICA_QueryControl( unsigned int qTyep, void* param, unsigned int arg1 )
{
	switch ( qTyep )
	{
	case CA_CLIENT_CAT_QUERY:
		CA_CLIENT_SEND_CAT_QUERY( param, arg1 );
 		break;
	case CA_CLIENT_PMT_QUERY:
		CA_CLIENT_SEND_PMT_QUERY( param, arg1 );
		break;
	case CA_CLIENT_PMT_QUERY_EXT:
		CA_CLIENT_SEND_PMT_QUERY_EXT( param, arg1 );
	break;
	case CA_CLIENT_CHANGE_PROG:
		CA_CLIENT_SEND_PMT_QUERY( param, arg1 );
		break;
	case CA_CLIENT_STOP_PROG_QUERY:
		CA_CLIENT_SEND_STOP_PROG_QUERY( param, arg1);
		break;		
	case CA_CLIENT_CHECK_PIN_QUERY:
		CA_CLIENT_SEND_CHECK_PIN_QUERY( param, arg1 );
	break;
	
	case CA_CLIENT_CHANGE_PIN_QUERY:
		CA_CLIENT_SEND_CHANGE_PIN_QUERY( param, arg1 );
	break;
	case CA_CLIENT_IPPV_QUERY:
		CA_CLIENT_SEND_IPPV_QUERY( param, arg1 );
	break;
	case CA_CLIENT_SC_INFO_QUERY:
		CA_CLIENT_SEND_SC_INFO_QUERY( param, arg1 );
	break;
	case CA_CLIENT_PPID_QUERY:
		CA_CLIENT_SEND_PPID_QUERY( param, arg1 );
	break;
	case CA_CLIENT_PPID_INFO_QUERY:
		CA_CLIENT_SEND_PPID_INFO_QUERY( param, arg1 );
	break;
	case CA_CLIENT_PPV_LIST_INFO_QUERY:
		CA_CLIENT_SEND_PPV_LIST_INFO_QUERY( param, arg1 );
	break;
	case CA_CLIENT_ANTI_RECORD_QUERY:
		CA_CLIENT_SEND_ANTI_RECORD_QUERY( param, arg1 );
	break;
	case CA_CLIENT_SET_PARENTAL_QUERY:
		CA_CLIENT_SEND_SET_PARENTAL_QUERY( param, arg1 );
	break;
	case CA_CLIENT_USER_VIEW_QUERY:
		CA_CLIENT_SEND_USER_VIEW_QUERY( param, arg1 );
	break;
	case CA_CLIENT_PARENTAL_CHANGE_QUERY:
		CA_CLIENT_SEND_PARENTAL_CHANGE_QUERY( param, arg1 );
	break;
	case CA_CLIENT_NIT_QUERY:
		CA_CLIENT_SEND_NIT_QUERY( param, arg1 );
	break;
	case CA_CLIENT_CHANGE_PID_QUERY:
		CA_CLIENT_SEND_CHANGE_PID_QUERY( param, arg1 );
	break;
	default:
 		break;	  
	}		
}

S32 CA_CallBack( DVBPlayer_Event_t tEvent )
{
	int iRet = 0;
	pbiinfo("[zshang][%s|%d] eEventType:%d\n",__FUNCTION__,__LINE__, tEvent.eEventType);
    switch( tEvent.eEventType )
    {
        case 0:
        {
           // PostChanChange_t *pCa_Post = tEvent.pEventData;
            //CA_PostChanChangeState( pCa_Post );    
        }
        break;
        case 1:		/*cat update*/
        {       
           CTICA_QueryControl(CA_CLIENT_CAT_QUERY,tEvent.pEventData,tEvent.u32EventLen);
        }
        break;
        case 2:		/*pmt update*/
        {
            CTICA_QueryControl(CA_CLIENT_PMT_QUERY,tEvent.pEventData,tEvent.u32EventLen);
        }
        break;
        case 8:
        {
        	CTICA_QueryControl(CA_CLIENT_PMT_QUERY_EXT,tEvent.pEventData,tEvent.u32EventLen);
        }
        break;

        case 7:
        {
        	CTICA_QueryControl(CA_CLIENT_NIT_QUERY,tEvent.pEventData,tEvent.u32EventLen);
        }
        break;
        case 11:
        {
        	memcpy(&QueryData,tEvent.pEventData,sizeof(CAPMT_QueryData));
        }
        break;
		case 3:/*切换多语言*/
		{
			U16  *temp =tEvent.pEventData;
			if(temp ==NULL)
			{
				pbiinfo("%s  error line %d ",__FUNCTION__,__LINE__);
				break;
			}
			iRet=Attach_Audio_Channels(temp[0],temp[1]);
			if(iRet!=0)
			{
				pbiinfo("%s  error line %d ",__FUNCTION__,__LINE__);
				break;
			}
		}
		break;
		case 4:
		{
			U32   *temp = tEvent.pEventData;
			if(temp ==NULL)
			{
				pbiinfo("%s  error line %d ",__FUNCTION__,__LINE__);
				break;
			}
			PVR_Attach_Descrabme(temp[0],temp[1]);
		}
		break;
		case 5:
		{
			U32   *temp = tEvent.pEventData;
			if(temp ==NULL)
			{
				pbiinfo("%s  error line %d ",__FUNCTION__,__LINE__);
				break;
			}
			PVR_Detach_ALL_Descrabme();
		}
		break;
		case 6:
		{
			U32   *temp = tEvent.pEventData;
			if(temp ==NULL)
			{
				pbiinfo("%s  error line %d ",__FUNCTION__,__LINE__);
				break;
			}

			PVR_Retach_PlayerVideoHandle(temp[0]);
			PVR_Retach_PlayerAudioHandle(temp[1]);

		}
		break;
        default:
        {
		 	pbiinfo("\n\nca callback type %d. \n\n",tEvent.eEventType);	
	 	}break;
    }
    if( iRet != 0 )
    {
    	pbierror("\n\nCA CallBack error !!!! err num %d...\n\n",iRet);
    }
    return 0;
}

void cti_set_call_back()
{
	DVBPlayer_RegCACallback( CA_CallBack );
}

/*
 * Description: Main message loop of Embedded CA Task.
 *
 * Parameters : None.
 * 
 * Returns    : None.
 */
static VOID caTaskMain(VOID* argv)
{
#if 0
	OS_MESSAGE* osMsg; 
	CA_SCELL_MESSAGE_STRUCT stCA_Msg;
	WORD wDemux;
#ifdef MULTI_TRANSPORT	
	WORD i;
#endif
	CA_DBG(("+++ CA TASK is running now.\n"));

	/* Initialise all modules here */
	IRD_MSG_Initialise();	   
	CA_SCARD_Initialise();
	CA_MONITOR_Initialise();
	CA_IPPV_Initialise();
	CA_PPT_Initialise();
	CA_LPPV_Inistialise();
	CA_TLV_Initialise();
	CA_SURFLOCK_Initialise();
	CA_OOB_EMMInitialise();
	CA_FlexiFlash_Initialise();
	ClearCaIds();
	
#ifdef MULTI_TRANSPORT	
	wDemuxFirstIndex = 0;
	bIsDefinedSvc = FALSE;
#endif
	for( wDemux=0; wDemux<CA_DEMUX_MAX; wDemux++ )
	{
		/*InitActiveSrvParams(wDemux);*/	
#ifdef MULTI_TRANSPORT	
		for( i=0; i<CA_SERVICES_MAX; i++ )
		{
			astActiveMultiSrv[wDemux][i].fSent = FALSE;
			astActiveMultiSrv[wDemux][i].fClear = TRUE;
			astActiveMultiSrv[wDemux][i].pbParams = NULL;		
			CA_ERROR_Initialise_MultiSvc(wDemux, (BYTE)i);
			ClearLocalMultiSvcInfo(wDemux, (BYTE)i);
		}
#else
		astActiveSrv[wDemux].fSent = FALSE;
		astActiveSrv[wDemux].fClear = TRUE;
		astActiveSrv[wDemux].pbParams = NULL;
		CA_ERROR_Initialise(wDemux);
		ClearLocalInfo(wDemux);
#endif		
		astCatInfo[wDemux].pbParams = NULL;
		awEmmPID[wDemux] = INVALID_EMM_PID;
		
        	CA_SVCINFO_Initialise(wDemux);
	}
	
	/* Let scell settle down */ 
	OS_DRV_DelayTask(500);

	/* After power on, SoftCell doesn't send the Status Msg spontaneously.*/
	CA_DRV_Status_Message();
 #endif
   	OS_MESSAGE* osMsg; 
	/* MAIN CA MESSAGE LOOP */
	for(;;)
	{
		/*CA_DBG_V(("\n\nWaiting for message......\n"));*/

		osMsg = DRV_OS_ReceiveMessage(g_CA_task_id);

		/* Store the message type in the CA Message Structure */
		if (osMsg == NULL) 
			continue; 
#if 0			
		stCA_Msg.enMsgType = osMsg->type;
		switch (stCA_Msg.enMsgType)
		{
			case CADRV__SCELL_STATUS_MESSAGE:
				/* Store the Status byte in the CA message structure */
				stCA_Msg.u.stCAStatus.bStatus = (BYTE)osMsg->CADRV_MSG_STATUS;
				break;
    
			case CADRV__SCELL_CA_MESSAGE:
				/* Store the received CA message info in the CA message structure */
				stCA_Msg.u.stRxTxCAMsg.wLength  = osMsg->CADRV_MSG_CA_LENGTH;
				stCA_Msg.u.stRxTxCAMsg.bChannel = osMsg->CADRV_MSG_CA_CHANNEL;
				stCA_Msg.u.stRxTxCAMsg.pbMsg    = osMsg->CADRV_MSG_CA_POINTER;
				break;

			case CADRV__CA_TASK_MESSAGE:
				stCA_Msg.u.pstCaMsg = osMsg->CADRV_MSG_CATASK_MSG;
				break; 
      
			default:
				CA_ERROR(("Unknown ca msg type.\n"));
				break;
		}
#endif		
		OS_MemDeallocate(system_p, osMsg);
        
	//	caProcessMsg(&stCA_Msg);	
	} /* End Forever */
} 


void CTICA_lock()
{
	CTIOS_WaitSemaphore(g_tCTICA_Lock);
}

void CTICA_unlock()
{
	CTIOS_SignalSemaphore(g_tCTICA_Lock);
}

U32 CTICA_Wait(U32 waitTime)
{
	U32 r;
	if(-1 == waitTime)
	{
		r = OS_SemWait(g_tCTICA_Msg);
	}
	else
	{
		r = OS_SemWait_Timeout(g_tCTICA_Msg, waitTime);
	}
	return r;
}

void CTICA_Signal()
{
	OS_SemSignal(g_tCTICA_Msg);
}

U32 CA_SendEvent( CTICA_Event_t tEvent )
{
	U32 uError = 0;
	if(tEvent.pEventData == NULL || tEvent.u32EventLen <= 0)
	{
		pbierror(" CA_SendEvent type %d. Event parm error \n\n",tEvent.eEventType);
	}
	switch( tEvent.eEventType )
	{
		case CAT_QUERY:
		case PMT_QUERY:
		case CHANGE_PROG:
		case STOP_PROG_QUERY:
		case CHECK_PIN_QUERY:
		case IPPV_QUERY:
		case SC_INFO_QUERY:
		case PPID_QUERY:
		case PPID_INFO_QUERY:
		case PPV_LIST_INFO_QUERY:
		case ANTI_RECORD_QUERY:
		case SET_PARENTAL_QUERY:
		case USER_VIEW_QUERY:
		case PARENTAL_CHANGE_QUERY:
		case NIT_QUERY:
		case CHANGE_PID_QUERY:
		break;		
		default: 
		{
		 	pbiinfo("CA_SendEvent not fount type %d. \n\n",tEvent.eEventType);
			return 1;
		}
	 	break;
	}
	uError = CTICAS_SendMsgToCAcore(tEvent.eEventType, tEvent.pEventData, tEvent.u32EventLen);
	if(uError != 0)
	{
		pbierror("[CTICAS_SendMsgToCAcore]CA_SendEvent type %d. [error %d]\n\n",tEvent.eEventType, uError);
		return uError;
	}
	return 0;
}

U32 CTICA_SendMsg(CTICA_Event_t tEvent, CTICA_Event_t *tCallMsg, U32 waitTime)
{
	U32 uError = 0; 
	CTI_OsMessage_t Message;
	if(tCallMsg == NULL && waitTime != 0)
		return 1;
	CTICA_lock();
	pbiinfo("[CTICA_SendMsg]start!!!!!\n");
	uError = CA_SendEvent(tEvent);
	if(0 != uError)
	{
		pbierror("[CTICA_SendMsg]SendMsg type %d. [error %d]\n",tEvent.eEventType, uError);
		goto error;
	}
	
	if(waitTime == 0)
		goto end;

	Message.type = 0xFF;
	Message.pData = NULL;
	Message.reserved = tEvent.eEventType;
	Message.msg_length = 0;
	uError = DRV_OS_SendMessage(g_uCTICATask, &Message);
	if(uError != 0)
	{
		pbierror("[CTICA_SendMsg]expectMsg type %d send error. [error %d]\n",tEvent.eEventType, uError);
		goto error;
	}
	
	uError = CTICA_Wait(waitTime);
	if(0 != uError)
	{
		pbierror("[CTICA_SendMsg]CTICA_Wait type. [error %d]\n",uError);
		goto error;
	}
	if(g_tCallMsg.eEventType == -1)
	{
		pbierror("[CTICA_SendMsg]CTICAclient_UpcallFunction return error  \n");
		uError = 1;
		goto error;
	}
	if(tCallMsg->eEventType != tEvent.eEventType)
	{
		pbierror("[CTICA_SendMsg]eEventType not same[g = %d | %d]  \n", g_tCallMsg.eEventType, tEvent.eEventType);
	}
	memcpy(tCallMsg, &g_tCallMsg, sizeof(CTICA_Event_t));
end:
	pbiinfo("[CTICA_SendMsg]end!!!!!\n");
	CTICA_unlock();
	return 0;
error:
	CTICA_unlock();
	return uError;
}

U32 CTIAC_AllocatePrivateData(CTICA_Event_t *CallMsg, BYTE* pinfo, U32 size)
{
	if(CallMsg->pPrivateData != NULL)
	{
		CTIOS_MemoryFree(CallMsg->pPrivateData);
		CallMsg->pPrivateData = NULL;
	}
	if(pinfo == NULL)
		return 1;
	CallMsg->pPrivateData = CTIOS_MemoryAllocate(size);
	if(CallMsg->pPrivateData != NULL)
	{
		memcpy(CallMsg->pPrivateData, pinfo, size);
		return 0;
	}
	return 1;
}

U32 CTIAC_AllocateEventData(CTICA_Event_t *CallMsg,CTI_CAInfoNotify_t type, BYTE* pparam, WORD len)
{
	if(CallMsg == NULL)
		return 1;
	CallMsg->eEventType = type;	
	if(CallMsg->pEventData != NULL)
	{
		CTIOS_MemoryFree(CallMsg->pEventData);
		CallMsg->pEventData = NULL;
	}	
	if( pparam != NULL && len > 0)
	{			
	CallMsg->pEventData = CTIOS_MemoryAllocate(len);
	if(CallMsg->pEventData != NULL)
	{
		memcpy(CallMsg->pEventData, pparam, len);
		CallMsg->u32EventLen = len;
		return 0;
	}
	}
	return 1;
}

void CTICA_CleanEvent(CTICA_Event_t* tEvent)
{
	if(tEvent != NULL)
	{
		if(tEvent->pPrivateData != NULL)
		{
			CTIOS_MemoryFree(tEvent->pPrivateData);
			tEvent->pPrivateData = NULL;
		}
		if(tEvent->pEventData != NULL)
		{
			CTIOS_MemoryFree(tEvent->pEventData);
			tEvent->pEventData = NULL;
		}
		CTIOS_MemoryFree(tEvent);
		tEvent = NULL;
	}
}
extern BOOL CTI_Client_ReciveDataBroadcast(void *arg);

void CTICAIP_MsgTask(void *arg)
{
	CTI_Client_ReciveDataBroadcast(arg);
}

void CTICAIP_MsgSendTask(void *arg)
{
	//while(1)
#if 0
	{
		NIT_DATA[0] = 'a';
		NIT_DATA[1] = 'b';
		NIT_DATA[3] = 0;
		CTI_Client_SendDataBroadcast(9999,NIT_DATA,2);
		OS_TaskDelay(10000);
	}
#endif
}


void CTICA_MsgTask(void *arg)
{
	CTI_OsMessage_t *pMsg = NULL;
	CTICA_Event_t*  EventMsg = NULL;
	U16 expectMsg = -1;
	while(1)
	{
		pMsg = DRV_OS_ReceiveMessage(g_uCTICATask);
		if(NULL == pMsg)
		{
			continue;
		}
		pbiinfo("[zshang][%s|%d]type[%d]data[0x%x]\n", __FUNCTION__, __LINE__, pMsg->type, pMsg->pData);
		EventMsg = pMsg->pData;
		if(EventMsg == NULL)
		{
			CTIOS_MemoryFree(pMsg);
			continue;
		}
		if(expectMsg != -1 && expectMsg == pMsg->type && EventMsg->eEventType == expectMsg)
		{
			expectMsg = -1;
			CTIAC_AllocateEventData(&g_tCallMsg, EventMsg->eEventType, EventMsg->pEventData, EventMsg->u32EventLen);
			CTICA_Signal();
		}
		switch(pMsg->type)
		{
			case CAT_QUERY_REPLY:
			case PMT_QUERY_REPLY :
			case STOP_PROG_QUERY_REPLY :
			case CHECK_PIN_QUERY_REPLY:
			case CHANGE_PIN_QUERY_REPLY:
			case IPPV_QUERY_REPLY:
			case NIT_QUERY_REPLY:
			case NVRAM_OP_REPLY:
			case ANTI_RECORD_QUERY_REPLY:
			case SET_PARENTAL_QUERY_REPLY:

			case IPPV_NOTIFY:
			case BMAIL_NOTIFY:
			case CAT_INFO_NOTIFY:
			case PMT_INFO_NOTIFY:
			case SUBTITLE_NOTIFY:
			case OSD_DISPLAY_NOTIFY:
			case EMM_STATUS_NOTIFY:
			case SC_INFO_NOTIFY :
			case PPID_NOTIFY:
			case PPID_INFO_NOTIFY:
			case CA_MESSAGE_CODE:
			case FINGERPRINT_NOTIFY:
			case USER_VIEW_INFO_NOTIFY:
			case PROG_SKIP_NOTIFY:
			case PPV_LIST_INFO_NOTIFY:
			case IPPV_LIST_INFO_NOTIFY:
			case PROG_PARENTAL_RATE_NOTIFY:
			case SECURITY_INIT_ERROR_NOTIFY:

			case COS_SYSTEM_NOTIFY:
			break;
			case 0xFF:
			{
				expectMsg = pMsg->reserved;
			}
			break;
		}
		if(CTI_Notif_CallBack)
		{	
			pbiinfo("[zshang][%s|%d]type[%d]len[%d]pPrivateData[0x%d]\n", __FUNCTION__, __LINE__, EventMsg->eEventType, EventMsg->u32EventLen, EventMsg->pPrivateData);
			CTI_Notif_CallBack(EventMsg->eEventType, EventMsg->u32EventLen, EventMsg->pEventData, EventMsg->pPrivateData);
		}
		CTICA_CleanEvent(EventMsg);
		CTIOS_MemoryFree(pMsg);
		
	}
}

void CTICA_RegMessage_Notify_CallBack(CACORE_CAMsg_Notify CallBack)
{
	if(CallBack)
		CTI_Notif_CallBack = CallBack;
	pbiinfo("[CTICA_RegMessage_Notify_CallBack]RegMessage CallBack addr[0x%x]g_addr[0x%x]\n", CallBack, CTI_Notif_CallBack);
}
void CTICA_CardNumHexToDec(unsigned char *pHex, char *pDec)
{
	CTICAS_HexadecimalToDecimal(pHex, pDec);
}

#if 0
void CTICAclient_UpcallFunction(CTI_CAInfoNotify_t type, BYTE* pparam, WORD len)
{
	DEBUGPF("CTICAclient_UpcallFunction ----------%x\n",type);
	if(type==CA_MESSAGE_CODE)
	{
		if(pparam!=NULL&&len>0)
		{
			DEBUGPF(" CA Message code =========== %d\n",pparam[0]);
		}
	}
	else if(type==CAT_QUERY_REPLY)
	{
		if(pparam!=NULL&&len>0)
		{
			DEBUGPF(" CAT replay --------------------%d\n",pparam[0]);	
		}
	}
	else if(type==CAT_INFO_NOTIFY)
	{
		if(pparam!=NULL&&len>0)
		{
			
		}
		
	}
	else if(type==PMT_QUERY_REPLY)
	{
		if(pparam!=NULL&&len>0)
		{
			DEBUGPF(" pmt replay  --------------------%x %x %x %x \n",pparam[0],pparam[1],pparam[2],pparam[3]);				
		}
		
	}
}
#else

void CTICAclient_UpcallFunction(CTI_CAInfoNotify_t type, BYTE* pparam, WORD len)
{
	int iRet = 0;
	U8 status = 0xff;	
	int i = 0;
CTI_OsMessage_t Message;
CTICA_Event_t* pEventMag = NULL;
	
	pEventMag = CTIOS_MemoryAllocate(sizeof(CTICA_Event_t));
	if(pEventMag == NULL)
	{
		pbierror("[CTICAclient_UpcallFunction]CTIOS_MemoryAllocate pEventMag error\n");
		return;
	}
	memset(pEventMag, 0, sizeof(CTICA_Event_t));
//	CTIAC_CleanEvent(g_tCallMsg);
	DEBUGPF("CTICAclient_UpcallFunction ----------%x\n",type);

	switch( type )
	{
		case CAT_QUERY_REPLY:
		{
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				DEBUGPF("[CAT_QUERY_REPLY] ----------status [%d]\n", status);
			}
		}
		break;
		case PMT_QUERY_REPLY:
		{
			if(pparam != NULL && len > 0)
			{
				U16 program_num = 0;
				U8 Desc_type;
				program_num = (pparam[0] << 8) | pparam[1];
				status = pparam[2];
				Desc_type = pparam[3];
				DEBUGPF("[PMT_QUERY_REPLY] program_num[%d] status[%d] Desc_type[%d]\n", program_num, status, Desc_type);
			}
		}
		break;
		case STOP_PROG_QUERY_REPLY:
		{
			if(pparam != NULL && len > 0)
			{
				U8 ecm_status = 0, emm_status = 0;
				emm_status = pparam[0];
				ecm_status = pparam[1];
				DEBUGPF("[STOP_PROG_QUERY_REPLY] emm_status[%d] ecm_status[%d]\n", emm_status, ecm_status);
			}
		}
		break;
		case CHECK_PIN_QUERY_REPLY:
		{
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				DEBUGPF("[CHECK_PIN_QUERY_REPLY] status[%d]\n", status);
			}
		}
		break;
		case CHANGE_PIN_QUERY_REPLY:
		{
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				DEBUGPF("[CHANGE_PIN_QUERY_REPLY] status[%d]\n", status);
			}
		}
		break;
		case IPPV_QUERY_REPLY:
		{
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				DEBUGPF("[CHANGE_PIN_QUERY_REPLY] status[%d]\n", status);
			}
		}
		break;		
		case NIT_QUERY_REPLY:
		{
			U8 length = 0, i = 0, regionCode[64];
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				length = pparam[1];
				DEBUGPF("[NIT_QUERY_REPLY] status[%d] length[%d]\n", status, length);
				if(status != 0)
					break;
				for(i = 0; i < length; i++)
				{
					regionCode[i] = pparam[2 + i];
				}				
			}
		}
		break;
		case NVRAM_OP_REPLY:
		{
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				DEBUGPF("[NVRAM_OP_REPLY] status[%d]\n", status);
			}
		}
		break;
		case ANTI_RECORD_QUERY_REPLY:
		{
			U8 antirecord_porperty = 1; 
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				antirecord_porperty = pparam[1];
				DEBUGPF("[ANTI_RECORD_QUERY_REPLY] status[%d]antirecord_porperty[%d]\n", status, antirecord_porperty);
			}
		}
		break;	
		case SET_PARENTAL_QUERY_REPLY:				
		{
			U8 parental_rating = 1;
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				parental_rating = pparam[1];
				DEBUGPF("[CTICAclient_UpcallFunction]SET_PARENTAL_QUERY_REPLY status[%d]antirecord_porperty[%d]\n", status, parental_rating);
			}
		}
		break;	
		case EMM_STATUS_NOTIFY:
		{
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				DEBUGPF("[CTICAclient_UpcallFunction]EMM_STATUS_NOTIFY status[%d]\n", status);
			}
		}
		break;
		case FINGERPRINT_NOTIFY:
		{
			U8 fingerprint_property[80];
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				memcpy(fingerprint_property, pparam+1, 80);
				DEBUGPF("[FINGERPRINT_NOTIFY] status[%d]\n", status);
			}
		}
		break;

		case IPPV_NOTIFY:
		{
			CTI_IppvInfoNotify_t IppvInfo;
			iRet = CTICAS_GetCAcoreInfo(type, pparam, len, &IppvInfo);
			if(0 != iRet)
			{
				pbierror("\n[CTICAS_GetCAcoreInfo]type %d. [error %d]\n\n",type, iRet);	
				goto end;
			}
			DEBUGPF("[IPPV_NOTIFY] program number = %x,%x\n",IppvInfo.prog_number[0],IppvInfo.prog_number[1]);
			CTIAC_AllocatePrivateData(pEventMag, &IppvInfo, sizeof(CTI_IppvInfoNotify_t));
		}
		break;
		case BMAIL_NOTIFY:
		{
			CTI_CABmailInfo_t BmailInfo;
			iRet = CTICAS_GetCAcoreInfo(type, pparam, len, &BmailInfo);
			if(0 != iRet)
			{
				pbierror("\n[CTICAS_GetCAcoreInfo]type %d. [error %d]\n\n",type, iRet);	
				goto end;
			}
			DEBUGPF("[BMAIL_NOTIFY] Status = %d,tag = %d\n",BmailInfo.status,BmailInfo.tag);
			CTIAC_AllocatePrivateData(pEventMag, &BmailInfo, sizeof(CTI_CABmailInfo_t));
		}
		break;
		case  SUBTITLE_NOTIFY:
		{
			DEBUGPF("[SUBTITLE_NOTIFY] status = %d\n",pparam[0]);
			CTI_CABmailInfo_t BmailInfo;
			iRet = CTICAS_GetCAcoreInfo(type, pparam, len, &BmailInfo);
			if(0 != iRet)
			{
				pbierror("\n[CTICAS_GetCAcoreInfo]type %d. [error %d]\n\n",type, iRet);	
				goto end;
			}
			DEBUGPF("[BMAIL_NOTIFY] Status = %d,tag = %d\n",BmailInfo.status,BmailInfo.tag);
			CTIAC_AllocatePrivateData(pEventMag, &BmailInfo, sizeof(CTI_CABmailInfo_t));
			
		}
		break;
		case CAT_INFO_NOTIFY:
		{
			CTI_CatInfoNotify_t CatInfo;
			iRet = CTICAS_GetCAcoreInfo(type, pparam, len, (void*)&CatInfo);
			if(0 != iRet )
			{
				pbierror("\n[CTICAS_GetCAcoreInfo]type %d. [error %d]\n\n",type, iRet);	
				goto end;
			}
			if(CatInfo.pid_count==1)
			{
				DEBUGPF("[CAT_INFO_NOTIFY]  EMMPID = %x%x\n",CatInfo.emm_pid[0],CatInfo.emm_pid[1]);
			}
		}
		break;
		case PMT_INFO_NOTIFY:	
		{
			CTI_PmtInfoNotify_t PmtInfo;
			iRet = CTICAS_GetCAcoreInfo(type, pparam, len, (void*)&PmtInfo);
			if(0 != iRet )
			{
				pbierror("\n[CTICAS_GetCAcoreInfo]type %d. [error %d]\n\n",type, iRet);	
				goto end;
			}
			
			DEBUGPF("[PMT_INFO_NOTIFY]  program_number = %x,%x\n",PmtInfo.program_number[0],PmtInfo.program_number[1]);
			DEBUGPF("[PMT_INFO_NOTIFY]  desc_type = %x\n",PmtInfo.desc_type);
			DEBUGPF("[PMT_INFO_NOTIFY]  ecm_pid = %x,%x\n",PmtInfo.ecm_pid[0],PmtInfo.ecm_pid[1]);
		}
		break;
		case OSD_DISPLAY_NOTIFY:
		{
			CTI_CAOsdDisplayStatus_t DisplayInfo;
			iRet = CTICAS_GetCAcoreInfo(type, pparam, len, &DisplayInfo);
			if(0 != iRet )
			{
				pbierror("\n[CTICAS_GetCAcoreInfo]type %d. [error %d]\n\n",type, iRet);	
				goto end;
			}
			DEBUGPF("[OSD_DISPLAY_NOTIFY]  loop count = %d,loop interval = %d\n",DisplayInfo.loop_num,DisplayInfo.loop_interval);
			CTIAC_AllocatePrivateData(pEventMag, &DisplayInfo, sizeof(CTI_CAOsdDisplayStatus_t));
		}
		break;
	#if 1
		case SC_INFO_NOTIFY:
		{
			U8 getinfo = 0;
			DEBUGPF("SC_INFO_NOTIFY coming-----\n");			
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				getinfo = pparam[1];
				DEBUGPF("SC_INFO_NOTIFY status = %d,%d\n",status,getinfo);
				if( getinfo == 1 )
				{
					CTI_SCInfoNotify_t SCInfo;
#if 1	
					iRet = CTICAS_GetCAcoreInfo(type, pparam, len, (void*)&SCInfo);
					if(0 != iRet )
					{
						DEBUGPF("\n[CTICAS_GetCAcoreInfo]type %d. [error %d]\n\n",type, iRet);	
						goto end;
					}
					DEBUGPF("sc number = ");
					for(i=0;i<8;i++)
					{
						DEBUGPF("%02x",SCInfo.cardnumber[i]);
					}
					DEBUGPF("\n");					
					DEBUGPF("parentalrating = %d",SCInfo.parentalrating);
					CTIAC_AllocatePrivateData(pEventMag, &SCInfo, sizeof(CTI_SCInfoNotify_t));
					DEBUGPF("\n[CTIAC_AllocatePrivateData]type %d. [pPrivateData:0x%x]\n\n",type,  pEventMag->pPrivateData);	
#endif					
				}
			}
		}
		break;
#endif
		case PPID_NOTIFY:
		{
			CTI_PPidNotify_t PPidNotify;
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];				
				if( status == 0 )
				{
					iRet = CTICAS_GetCAcoreInfo(type, pparam, len, &PPidNotify);
					if(0 != iRet )
					{
						pbierror("\n[CTICAS_GetCAcoreInfo]type %d. [error %d]\n\n",type, iRet);	
						goto end;
					}
					DEBUGPF("[PPID_NOTIFY]  count = %d\n",PPidNotify.count);
					CTIAC_AllocatePrivateData(pEventMag, &PPidNotify, sizeof(CTI_PPidNotify_t));
				}
			}
		}
		break;
		case PPID_INFO_NOTIFY:
		{
			CTI_PPidInfo_t PPidInfo;
			U32 ppid_id = 0;
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				ppid_id = pparam[1] << 16 | pparam[2] << 8 | pparam[3];
				if( status == 0 )
				{
					iRet = CTICAS_GetCAcoreInfo(type, pparam, len, &PPidInfo);
					if(0 != iRet )
					{
						pbierror("\n[CTICAS_GetCAcoreInfo]type %d. [error %d]\n\n",type, iRet);	
						goto end;
					}
					DEBUGPF("[PPID_INFO_NOTIFY] ID = %d, %s\n",PPidInfo.id,PPidInfo.label);
					CTIAC_AllocatePrivateData(pEventMag, &PPidInfo, sizeof(CTI_PPidInfo_t));
				}
			}
		}
		break;
		case CA_MESSAGE_CODE:	
		{
			if(pparam != NULL && len > 0)
			{
				U16 program_num = 0;
				U8 index;
				status = pparam[0];
				program_num = (pparam[1] << 8) | pparam[2];				
				index = pparam[3];
				//len = 4;
				//CTICA_CLIENT_SEND_CA_MESSAGE_QUERY(pparam,len);
				DEBUGPF("[CA_MESSAGE_CODE] program_num[%d] status[%d] Desc_type[%d]\n", program_num, status, index);
			}
		}
		break;
		case USER_VIEW_INFO_NOTIFY:
		{
			CTI_UserViewInfoNotify_t UserView;
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				if(status  == 1)
				{
					iRet = CTICAS_GetCAcoreInfo(type, pparam, len, &UserView);
					if(0 != iRet)
					{
						pbierror("\n[CTICAS_GetCAcoreInfo]type %d. [error %d]\n\n",type, iRet);	
						goto end;
					}
					CTIAC_AllocatePrivateData(pEventMag, &UserView, sizeof(CTI_UserViewInfoNotify_t));
				}				
				DEBUGPF("[USER_VIEW_INFO_NOTIFY] status[%d]\n", status);
				
			}
		}
		break;
		case PROG_SKIP_NOTIFY:
		{
			CTI_ProgSkipNotify_t SkipNotify;
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				if( status == 0 )
				{
					iRet = CTICAS_GetCAcoreInfo(type, pparam, len, &SkipNotify);
					if(0 != iRet)
					{
						pbierror("\n[CTICAS_GetCAcoreInfo]type %d. [error %d]\n\n",type, iRet);	
						goto end;
					}
					DEBUGPF("[PROG_SKIP_NOTIFY] programber = %x,%x\n",SkipNotify.prognumber[0],SkipNotify.prognumber[1]);
					DEBUGPF("[PROG_SKIP_NOTIFY] message = %s\n",SkipNotify.skipdata.message);
					CTIAC_AllocatePrivateData(pEventMag, &SkipNotify, sizeof(CTI_ProgSkipNotify_t));
				}
			}
		}
		break;
		case PPV_LIST_INFO_NOTIFY:
		{
			CTI_PPVListNotify_t PPidInfo;
			U8 page = 0;
			U32 ppid_id = 0;
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				page = pparam[1];
				ppid_id = pparam[2] << 16 | pparam[3] << 8 | pparam[4];
				if( status == 0 )
				{
					int i = 0;
					iRet = CTICAS_GetCAcoreInfo(type, pparam, len, &PPidInfo);
					if(0 != iRet )
					{
						pbierror("\n[CTICAS_GetCAcoreInfo]type %d. [error %d]\n\n",type, iRet);	
						goto end;
					}
					DEBUGPF("[PPV_LIST_INFO_NOTIFY]  %d,%d\n",PPidInfo.page,PPidInfo.prog_count);
					for(i = 0; i < PPidInfo.prog_count; i++)
					{
						DEBUGPF("[PPV_LIST_INFO_NOTIFY][%d]Start  0x%x,0x%x,0x%x,0x%x,\n", i, PPidInfo.ppv[i].start_number[0]
							, PPidInfo.ppv[i].start_number[1]
							, PPidInfo.ppv[i].start_number[2]
							, PPidInfo.ppv[i].start_number[3]);
						DEBUGPF("[PPV_LIST_INFO_NOTIFY][%d]End  0x%x,0x%x,0x%x,0x%x,\n", i, PPidInfo.ppv[i].end_number[0]
							, PPidInfo.ppv[i].end_number[1]
							, PPidInfo.ppv[i].end_number[2]
							, PPidInfo.ppv[i].end_number[3]);
					}
					CTIAC_AllocatePrivateData(pEventMag, &PPidInfo, sizeof(CTI_PPVListNotify_t));
				}
			}
		}
		break;
		case IPPV_LIST_INFO_NOTIFY:
		{
			CTI_IPPVListNotify_t IPPidInfo;
			U8 page = 0;
			U32 ppid_id = 0;
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				page = pparam[1];
				ppid_id = pparam[2] << 16 | pparam[3] << 8 | pparam[4];
				if( status == 0 )
				{
					iRet = CTICAS_GetCAcoreInfo(type, pparam, len, &IPPidInfo);
					if(0 != iRet )
					{
						pbierror("\n[CTICAS_GetCAcoreInfo]type %d. [error %d]\n\n",type, iRet);	
						goto end;
					}
					DEBUGPF("[IPPV_LIST_INFO_NOTIFY]  %d,%d\n",IPPidInfo.page,IPPidInfo.prog_count);
					CTIAC_AllocatePrivateData(pEventMag, &IPPidInfo, sizeof(CTI_IPPVListNotify_t));
				}
			}
		}
		break;		
		case PROG_PARENTAL_RATE_NOTIFY:
		{
			unsigned char parental = 0;
			if(pparam != NULL && len > 0)
			{
				status = pparam[0];
				parental = pparam[1];
				
				
				DEBUGPF("[PROG_PARENTAL_RATE_NOTIFY]status[%d] parental = %d\n", status,parental);
				
			}
		}
		break;
		case CHILD_MOTHER_IP_NOTIFY:
		{
			BYTE bType = 0;
			int iLen = 0;
			BYTE IPInfo[120];
			BYTE IPLength = 0;
			if(pparam != NULL && len > 0)
			{
				bType = pparam[0];
				iLen = pparam[1]<<24 |pparam[2] <<16 | pparam[3] << 8 | pparam[4] ;
				
				DEBUGPF("[CHILD_MOTHER_IP_NOTIFY] type = %x,len = %d",bType,iLen);
				DEBUGPF(" IP address %x.%x.%x.%x,port = %x.%x",pparam[5],pparam[6],pparam[7],pparam[8],pparam[9],pparam[10]);
				iRet = CTICAS_GetCAcoreInfo(type, pparam, len,IPInfo);
				if(0 != iRet )
				{
					pbierror("\n[CTICAS_GetCAcoreInfo]type %d. [error %d]\n\n",type, iRet);	
					goto end;
				}
				IPLength = (IPInfo[0]<<8)|IPInfo[1];
				DEBUGPF("[CHILD_MOTHER_IP_NOTIFY] IPInfo len = %d",IPLength);
				DEBUGPF("IP SendReadData: ");
			//	for(i=0; i<IPLength+2;i++)
			//	{
			//		DEBUGPF("%02x ", IPInfo[i]);
			//	}
			//	DEBUGPF("\n");
				
				CTICA_CLIENT_SEND_CA_IP_QUERY(pparam,IPInfo,IPLength+2);
				

			}
		}
		break;
		case SECURITY_INIT_ERROR_NOTIFY:
		case COS_SYSTEM_NOTIFY:		
		default: 
		{
		 	DEBUGPF("\n\n CTICAclient_UpcallFunction not found type %d. \n\n",type);
			goto end;
		}
	 	break;		
	}
	CTIAC_AllocateEventData(pEventMag, type, pparam, len);
end:	
	Message.type = type;
	Message.pData = (BYTE *)pEventMag;
	Message.msg_length = sizeof(CTICA_Event_t);
	iRet = DRV_OS_SendMessage(g_uCTICATask, &Message);
	if(iRet != 0)
	{
		CTICA_CleanEvent(pEventMag);
	}
	return;
	
}
#endif
VOID CTICAclient_GetEpgTime(BYTE time[8])
{
	time_td_t tUTC_Time;
	tUTC_Time =Get_Local_Td_time();
	time[0] = (tUTC_Time.year >> 8) & 0xFF;
	time[1] = tUTC_Time.year & 0xFF;
	time[2] = tUTC_Time.month & 0xFF;
	time[3] = tUTC_Time.date & 0xFF;
	time[4] = tUTC_Time.hour & 0xFF;
	time[5] = tUTC_Time.minute & 0xFF;
	time[6] = tUTC_Time.second & 0xFF;
	time[7] = 0;
	
}

int CA_Init()
{
	CTI_CAcoreInitParam_t CTIparam;
	WORD size = 4*1024;
	int iError = 0;
	CTICA_OS_Init();
	
	CTIparam.Bmail_amount = 99;
	CTIparam.OSD_amount = 99;
	memset(CTIparam.STB_SerialNumber, 0, 8);
	CTIparam.cacore_trace_level = 7;
	CTIparam.STB_SC_Married_Type = 1;
	CTIparam.STB_Name[0] = 0x31;
	CTIparam.STB_Name[1] = 0;
	CTIparam.PARENTAL_TYPE = 0;	
	iError = CTICAS_CAcoreInit(CTIparam, &size);
	if(iError != 0)
	{
		pbierror("CAInit CTICAS_CAcoreInit---->error \n");
		return 1;
	}
/*ca task init */
	g_tCTICA_Lock = CTIOS_CreateSemaphore(1);
	if(g_tCTICA_Lock == 0)
	{
		pbierror("CAInit CTIOS_CreateSemaphore---->error \n");
		return 1;
	}
	if(OS_SemCreate(&g_tCTICA_Msg,"CTISemMsg",1,0) !=0)
	{
		pbierror("CAInit OS_SemCreate---->error \n");
		return 1;
	}
	g_uCTICATask = DRV_OS_TaskCreate(CTICA_MsgTask, CTICA_Priority, CTICA_Size);
	if(g_uCTICATask == 0)
	{
		pbierror("CAInit CTIOS_TaskCreate---->error \n");
		return 1;
	}

	g_uCTICAIPTask = DRV_OS_TaskCreate(CTICAIP_MsgTask, CTICAIP_Priority, CTICAIP_Size);
	if(g_uCTICAIPTask == 0)
	{
		pbierror("CAInit CTIOS_TaskCreate---->error \n");
		return 1;
	}
//	g_uCTICAIPSendTask = DRV_OS_TaskCreate(CTICAIP_MsgSendTask, CTICAIPSend_Priority, CTICAIPSend_Size);
//	if(g_uCTICAIPSendTask == 0)
//	{
//		pbierror("CAInit CTIOS_TaskCreate---->error \n");
//		return 1;
//	}

	return 0;
}


void CTI_TEST(unsigned char *data)
{
	unsigned char buffer[4] = {0,0,0,0};
	int len = 4;
	
	if(data[5] == '0')
	{
		CTICA_QueryControl(CA_CLIENT_CHECK_PIN_QUERY,buffer,len);
	}
	else if(data[5] == '1')
	{
		CTI_PinCode pin;
		pin.OLD_PinCode[0 ] = 0;
		pin.OLD_PinCode[1 ] = 0;
		pin.OLD_PinCode[2 ] = 0;
		pin.OLD_PinCode[3 ] = 0;
		pin.NEW_PinCode[0] = 1;
		pin.NEW_PinCode[1 ] = 1;
		pin.NEW_PinCode[2 ] = 1;
		pin.NEW_PinCode[3 ] = 1;

		CTICA_QueryControl(CA_CLIENT_CHANGE_PIN_QUERY,&pin,0);
	}
	else if(data[5] == '2')
	{
		CTICA_QueryControl(PARENTAL_CHANGE_QUERY,buffer,0);
	}
	else if(data[5] == '3')
	{
		CTI_Parental  Parental;
		Parental.Parental_rate = 4;
		Parental.PinCode[0] = 1;
		Parental.PinCode[1] = 1;
		Parental.PinCode[2] = 1;
		Parental.PinCode[3] = 1;
		CTICA_QueryControl(SET_PARENTAL_QUERY,&Parental,0);
	}
	else if(data[5]=='4')
	{
		CTI_UserView UserView;
		unsigned short start_channel,end_channel;
		static int set = 0;
		CTICAclient_GetEpgTime(UserView.control_start_time);
		UserView.PinCode[0] = 0;
		UserView.PinCode[1] = 0;
		UserView.PinCode[2] = 0;
		UserView.PinCode[3] = 0;

		UserView.type = 1;
		if(set==0)
		{
			UserView.flag = 1;
			set = 1;
		}
		else
		{
			set = 0;
			UserView.flag = 0;
		}
		UserView.status = 2;
		UserView.control_start_time[0] = 20;
		UserView.control_start_time[1] = 8;
		UserView.control_start_time[2] = 1;
		UserView.control_start_time[3] = 1;
		UserView.control_start_time[4] = 0;//hour
		UserView.control_start_time[5] =0;//minute
		UserView.control_start_time[6] = 1;//second
		UserView.control_start_time[7] = 1;
	//	CTICAclient_GetEpgTime(UserView.control_start_time);

		UserView.control_end_time[0] = 20;
		UserView.control_end_time[1] = 8;
		UserView.control_end_time[2] = 1;
		UserView.control_end_time[3] = 1;
		UserView.control_end_time[4] = 23;
		UserView.control_end_time[5] = 59;
		UserView.control_end_time[6] = 0;
		UserView.control_end_time[7] = 4;
		
	//	CTICAclient_GetEpgTime(UserView.control_end_time);
	//	UserView.control_end_time[4]+=2;
	
		start_channel = 1;
		end_channel = 4;
		printf("startchannel = %x,end = %x\n",start_channel,end_channel);
		UserView.control_start_channel[0] = (unsigned char)(start_channel>>8);
		UserView.control_start_channel[1] = (unsigned char)(start_channel);
		
		UserView.control_end_channel[0] = (unsigned char)(end_channel>>8);
		UserView.control_end_channel[1] = (unsigned char)(end_channel);
		
		CTICA_QueryControl(CA_CLIENT_USER_VIEW_QUERY,&UserView,0);
		
		
	}
	else if(data[5]=='5')
	{
		CTICA_QueryControl(CA_CLIENT_PPID_QUERY,0,0);
	}
	else if(data[5]=='6')
	{
		unsigned int  PID = 0x10;
		CTI_PPID_Info PPID_info;
		PPID_info.PinCode[0] = 0;
		PPID_info.PinCode[1] = 0;
		PPID_info.PinCode[2] = 0;
		PPID_info.PinCode[3] = 0;
		PPID_info.PPID = PID;
		CTICA_QueryControl(CA_CLIENT_PPID_INFO_QUERY,&PPID_info,0);
	}
	else if(data[5]=='7')
	{
		unsigned int  PID = 0x20;
		CTI_PPID_Info PPID_info;
		PPID_info.PinCode[0] = 0;
		PPID_info.PinCode[1] = 0;
		PPID_info.PinCode[2] = 0;
		PPID_info.PinCode[3] = 0;
		PPID_info.PPID = PID;
		CTICA_QueryControl(CA_CLIENT_PPID_INFO_QUERY,&PPID_info,0);
	}
	else if(data[5]=='8')
	{
		unsigned int  PID = 0x30;
		CTI_PPID_Info PPID_info;
		PPID_info.PinCode[0] = 0;
		PPID_info.PinCode[1] = 0;
		PPID_info.PinCode[2] = 0;
		PPID_info.PinCode[3] = 0;
		PPID_info.PPID = PID;
		CTICA_QueryControl(CA_CLIENT_PPID_INFO_QUERY,&PPID_info,0);
	}
	else if(data[5]=='9')
	{
		NIT_DATA[ 0 ] = 0x89;
		NIT_DATA[ 1 ] = 0x03;
		NIT_DATA[ 2 ] = 0x01;
		NIT_DATA[ 3 ] = 0x86;
		NIT_DATA[ 4 ] = 0xF2;
		CTICAS_SendMsgToCAcore(NIT_QUERY, NIT_DATA, 5);		
	}
	else if(data[5]=='a')
	{
		NIT_DATA[ 0 ] = 0x89;
		NIT_DATA[ 1 ] = 0x03;
		NIT_DATA[ 2 ] = 0;
		NIT_DATA[ 3 ] = 0x86;
		NIT_DATA[ 4 ] = 0xF2;
		CTICAS_SendMsgToCAcore(NIT_QUERY, NIT_DATA, 5);		
		
	}
	else if(data[5]=='b')
	{
		NIT_DATA[0] = 'a';
		NIT_DATA[1] = 'b';
		NIT_DATA[3] = 0;
		CTI_Client_SendDataBroadcast(9999,NIT_DATA,2);	
		
	}
	else if(data[5]=='c')
	{
		CTICAclient_GetSTBipAddr(NIT_DATA);
		g_uCTICAIPTask = DRV_OS_TaskCreate(CTICAIP_MsgTask, CTICAIP_Priority, CTICAIP_Size);
		if(g_uCTICAIPTask == 0)
		{
			pbierror("CAInit CTIOS_TaskCreate---->error \n");
			return 1;
		}

	}
	

}

