
/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name :ctidmux.c
* Description : Includes all other FILTER driver header files.
              	This allows a FILTER application to include only this one header
              	file directly. 
* History :
* Date               Modification                                Name
* ----------         ------------                        ----------
* 20130412         Created                                zshang
******************************************************************************/

/*******************************************************/
/*              Includes				                                        */
/*******************************************************/
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "osapi.h" 
//#include "Tr_Cas.h"
#include "ctidmux.h"
#include "drv_filter.h"
#include "pbitrace.h"

#include "am_dsc.h"
#include "drv_avctrl.h"

/*******************************************************/
/*               Private Defines and Macros			                    */
/*******************************************************/

#define FILTER_DEPTH 				16
#define	CTI_MAX_MASK_LENGTH			16
#define	CTI_MAX_SECTION_LENGTH		4096
#define CTI_MAX_CHANNEL				16

#define 	CTI_MAX_DESC_NUM    	16
#define 	INVALIDPID				0x1fff
#define 	DSC_DEV_NO  			0

#define DEBUGPF pbiinfo

typedef void (*upfct)(DMUX_CHANNEL_M SecChannelID);

typedef struct 
{
	BOOL 	 Inuse;											/*是否创建 
															解扰器创建分配好就一直使用不重新创建
	   														Inuse分配后一直为TRUE
	   													*/
    U32   CTI_DescramblerHandle;				/*解扰器句柄*/		
	//U32      ActMapedPidNum;						/*实际对应解扰的PID个数*/
	U32   SlotOrPid;		/*对应音频或视频PID列表，即一个解扰器可解扰多个stream PID*/
	U16		demuxid;
	BOOL 	 attcha_flag;			
}CTI_Descrambler_s;

typedef struct CTI_dmx_slot_s
{
	S32 filter_id;
	S32 slot_id;
	BOOL status;
	U16	ReqID;
	upfct CTI_upcallfunc;
	U8  unreadflag;
	U8  depth;
	U8  section_data[CTI_MAX_SECTION_LENGTH];
	U8	*buffer;
	U16 demuxid;	
}CTI_dmx_slot_t;

typedef struct CTI_dmx_filter_s
{
	S32 filter_id;
	U32 slot_id;
	//U32 pid;
	BOOL status;
	//U8	ReqID;
	U16 demuxid;
	
}CTI_dmx_filter_t;




/*******************************************************/
/*               Private Constants                                                    */
/*******************************************************/

/*******************************************************/
/*               Global Variables					                          */
/*******************************************************/
static CTI_dmx_slot_t   g_cti_slot_info[CTI_MAX_CHANNEL];
static CTI_dmx_filter_t g_cti_filter_info[CTI_MAX_CHANNEL];
static U32			g_casSectionProcess = 0;
CTI_Descrambler_s CTI_Descrambler_t[CTI_MAX_DESC_NUM];	/*视频和音频解扰器HANDLE*/

/*******************************************************/
/*               Private Variables (static)			                          */
/*******************************************************/




/*******************************************************/
/*               Private Function prototypes		                          */
/*******************************************************/

static void _CasFreeDataBuffer(tSFILTER_SlotId  slot,void* buf);
static U8 *_CasGetSectionDataBuffer(tSFILTER_SlotId  slot);
static void _CasFilterInfoCallBack(		tSFILTER_SlotId 	slot,
											tSFILTER_FilterId	filter,
											tSFILTER_FreeBufferFct free_func,
											U8 					*buffer,
											U32 				length,
											U16					pid);


/*******************************************************/
/*               Functions							                   */
/*******************************************************/

static U16  section_length =0;
/******************************************************************************
* Function : 	   _CasGetSectionDataBuffer
* parameters :  
*			   
*			   slot : The slot identifier
* Return :  
*			   (nothing)	   
* Description :
*			  _CasGetSectionDataBuffer
*			 
* Author :	 zshang
******************************************************************************/
/*amlogic 平台不调用这个函数*/
#define MAX_SECTION_LENGTH      (4096)
static U8 *_CasGetSectionDataBuffer(tSFILTER_SlotId  slot)
{
	U32 i;
	DEBUGPF("%s     line %d \n",__FUNCTION__,__LINE__);
#if 0
	OS_SemWait(g_casSectionProcess);
	
	for( i =0; i< CTI_MAX_CHANNEL; i++)
	{
	   if((g_cti_slot_info[i].status ==TRUE)&&(g_cti_slot_info[i].slot_id ==slot))
		   break;
	}

	if(i ==CTI_MAX_CHANNEL)
	{
	   DEBUGPF("%s  error line %d \n",__FUNCTION__,__LINE__);
	   OS_SemSignal(g_casSectionProcess);
	   return NULL;
	}

	g_cti_slot_info[i].unreadflag =1;
	return (U8 *)(&(g_cti_slot_info[i].section_data));
#endif
	U8 *pbuf = NULL;
	pbuf = (U8 *)OS_MemAllocate(system_p,MAX_SECTION_LENGTH);/*yangweiqing modify 10-04-26*/
	if(!pbuf)
	{
	       pbiinfo("===BackGround:   GetSectionDataBuffer() Failed. Not enough memory\n");
	}
	return pbuf;
}

 /******************************************************************************
* Function : 		_CasFreeDataBuffer
* parameters :	
*				
*				buf : The Address to Free
* Return :	
*				(nothing)		
* Description :
*				Free the Buffer to a slot
*
* Author : 		zshang
******************************************************************************/
/*amlogic 平台不调用这个函数*/
static void _CasFreeDataBuffer(tSFILTER_SlotId  slot,void* buf)
{ 	

	U32 i;
    DEBUGPF("%s     line %d \n",__FUNCTION__,__LINE__);
   #if 0
	for( i =0; i< CTI_MAX_CHANNEL; i++)
   {
	   if((g_cti_slot_info[i].status ==TRUE)&&(g_cti_slot_info[i].slot_id ==slot))
		   break;
   }
   if(i ==CTI_MAX_CHANNEL)
   {
	   DEBUGPF("%s  error line %d \n",__FUNCTION__,__LINE__);
        return ;
   }

	if(g_cti_slot_info[i].unreadflag ==1)
	{
		OS_SemSignal(g_casSectionProcess);
		memset(g_cti_slot_info[i].section_data,0,CTI_MAX_SECTION_LENGTH);
		g_cti_slot_info[i].unreadflag =0;
		section_length =0; 
	}
	DEBUGPF("%s	   line %d   ok\n",__FUNCTION__,__LINE__);
	#endif
	if(!buf)
            pbiinfo("===BackGround:   FreeSectionDataBuffer() Failed. Null ptr\n");
	else
	{
		OS_MemDeallocate(system_p,buf);
		buf = NULL;
	}
}
 
 /******************************************************************************
 * Function :_CasFilterInfoCallBack
 * Parameters : 
 *			 
 * Return :
 *			 buffer pointer
 * Description :
 *					 The function is used to tell cell a new section arrived
 * Author : 
 *					 zshang 	20130412
 * ---------------------------------------------------------
 * Modification History:	   
 *					   Initalization
 * ---------------------------------------------------------
 *****************************************************************************/
static void _CasFilterInfoCallBack(		tSFILTER_SlotId 	slot,
											tSFILTER_FilterId	filter,
											tSFILTER_FreeBufferFct free_func,
											U8 			*buffer,
											U32 		length,
											U16			pid)
{
	
	
	 U32 i,k;
	 

	 for( i =0; i< CTI_MAX_CHANNEL; i++)   
	{
		if((g_cti_slot_info[i].status ==TRUE)&&(g_cti_slot_info[i].slot_id ==slot))
			break;
	}
	if(i ==CTI_MAX_CHANNEL)
	{
		DEBUGPF("%s	error line %d \n",__FUNCTION__,__LINE__);
	//	OS_SemSignal(g_casSectionProcess);
		return ;
	}
	if(length>CTI_MAX_SECTION_LENGTH)
	{
		DEBUGPF("%s	error line %d \n",__FUNCTION__,__LINE__);
		return ;
	}
	/*if( (buffer[0] ==0x80)||(buffer[0] ==0x81))
	{
		 if(g_cti_slot_info[i].data0!=buffer[0])
		 {
			 DEBUGPF("%s	data NOMATCH %d %x\n+++++++++++++++++++++++++++\+++++++++++++++++++\n",__FUNCTION__,__LINE__,buffer[0]);
		 	return;
		 }
	}*/
	
    OS_SemWait(g_casSectionProcess);
    g_cti_slot_info[i].unreadflag =1;

	section_length=length; 
	//DEBUGPF("%s     line %d     slot id %d  filter %d pid %d  \n ",__FUNCTION__,__LINE__,g_cti_slot_info[i].slot_id ,filter,pid);
	DEBUGPF("tableid %x LEN [%d]\n",buffer[0],section_length);
	
#if 1
	memset(g_cti_slot_info[i].section_data,0xff,CTI_MAX_SECTION_LENGTH);

	memcpy(g_cti_slot_info[i].section_data,buffer,length);
#else
	g_cti_slot_info[i].buffer = OS_MemAllocate(system_p, length+1);
	if(g_cti_slot_info[i].buffer!=NULL)
	{
		memcpy(g_cti_slot_info[i].buffer,buffer,length);
	}
#endif
	if(g_cti_slot_info[i].CTI_upcallfunc )
	{
		g_cti_slot_info[i].CTI_upcallfunc(g_cti_slot_info[i].slot_id); 
	}
//	DEBUGPF("%s   ok \n",__FUNCTION__);


}



/******************************************************************************
* Function :CTIDMUX_Init
* Parameters : 
*			none
* Return :
*			buffer pointer
* Description :
*                   The function is used to initalize the demux driver
* Author : 
*                    zshang 	20130412
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/
//S16   TRDRV_DEMUX_Initialise(void)
BYTE CTIDMUX_Init(void)
{
	
   U32  i;
   DEBUGPF("-----------ctica  %s    %d \n",__FUNCTION__,__LINE__);

   for( i =0; i< CTI_MAX_CHANNEL; i++)
   {
		g_cti_slot_info[i].filter_id =DMUX_INVALID_FILTER_ID;
		g_cti_slot_info[i].ReqID =0;
		g_cti_slot_info[i].slot_id =DMUX_INVALID_CHANNEL_ID;
		g_cti_slot_info[i].status =FALSE;
		g_cti_slot_info[i].CTI_upcallfunc = NULL;
		g_cti_slot_info[i].unreadflag =0;
		g_cti_slot_info[i].depth =0;
		memset(g_cti_slot_info[i].section_data,0xff,CTI_MAX_SECTION_LENGTH);
   }

   for( i =0; i< CTI_MAX_CHANNEL; i++)
   {
	   g_cti_filter_info[i].filter_id =DMUX_INVALID_FILTER_ID;
	   g_cti_filter_info[i].slot_id =DMUX_INVALID_CHANNEL_ID;
	   g_cti_filter_info[i].status = FALSE;
   }

	
   
   if(OS_SemCreate(&g_casSectionProcess,"g_casSectionProcess",1,0)!=0)
   {
	   pbierror("\n CTIDMUX_Init OS_SemCreate :OSAL_SemCreate failure \n");
	   return 1;
   }
   DEBUGPF("-----------ctica  %s    %d \n",__FUNCTION__,__LINE__);

   CTI_DescramblerInit();
   DEBUGPF("-----------ctica  %s    %d \n",__FUNCTION__,__LINE__);
   return 0;
   
}

void   TRDRV_DEMUX_Uninit(void)
{
	U32  i;
	U32  ret =0;
	S16  error =0;
	S32  semerror =0;
	
   	DEBUGPF("-----------ctica  %s    %d \n",__FUNCTION__,__LINE__);

	 for( i =0; i< CTI_MAX_CHANNEL; i++)
	 {		
		 if((g_cti_slot_info[i].status ==TRUE)&&(g_cti_slot_info[i].slot_id!=DMUX_INVALID_CHANNEL_ID))
		 {
			ret = SFILTER_Control_Channel(g_cti_slot_info[i].slot_id,eSFILTER_CTRL_DISABLE);
			if(ret !=0)
			{
				pbierror("-----------ctica  %s    %d   error %x \n",__FUNCTION__,__LINE__,ret);
			}
		 }
	 }
	 
	 for( i =0; i< CTI_MAX_CHANNEL; i++)
	 {
		 if((g_cti_filter_info[i].status ==TRUE)&&(g_cti_filter_info[i].filter_id!=DMUX_INVALID_FILTER_ID))
		 {
			 CTIDMUX_CloseFilter(g_cti_filter_info[i].slot_id,g_cti_filter_info[i].filter_id);
		 }
	 }
	 for( i =0; i< CTI_MAX_CHANNEL; i++)
	 {		
		 if((g_cti_slot_info[i].status ==TRUE)&&(g_cti_slot_info[i].slot_id!=DMUX_INVALID_CHANNEL_ID))
		 {
			CTIDMUX_CloseSectionChannel(g_cti_slot_info[i].slot_id);
			
		 }
	 }

	 
	 for( i =0; i< CTI_MAX_CHANNEL; i++)
	{
		 g_cti_slot_info[i].filter_id =DMUX_INVALID_FILTER_ID;
		 g_cti_slot_info[i].ReqID =0;
		 g_cti_slot_info[i].slot_id =DMUX_INVALID_CHANNEL_ID;
		 g_cti_slot_info[i].status =FALSE;
		 g_cti_slot_info[i].CTI_upcallfunc = NULL;
		 g_cti_slot_info[i].unreadflag =0;
		 g_cti_slot_info[i].depth =0;
		 memset(g_cti_slot_info[i].section_data,0xff,CTI_MAX_SECTION_LENGTH);
	}
 
	for( i =0; i< CTI_MAX_CHANNEL; i++)
	{
		g_cti_filter_info[i].filter_id =DMUX_INVALID_FILTER_ID;
		g_cti_filter_info[i].slot_id =DMUX_INVALID_CHANNEL_ID;
		g_cti_filter_info[i].status = FALSE;
	}
	if(g_casSectionProcess!=0)
	{
		semerror = OS_SemDelete(g_casSectionProcess);
		if(semerror !=0)
		{
			pbierror("-----------ctica  %s    %d   error %x \n",__FUNCTION__,__LINE__,ret);
		}
	}
	g_casSectionProcess =0;

}




    
/******************************************************************************
* Function :CTIDMUX_OpenSectionChannel
* Parameters : 
*			wMaxFilterNumber :maximum number of the filters on this channel
*                 wMaxFilterSize :maximum number of the match and mask byte in the filter,
*                                      it indicate the depth of the filter
*                 ulBufferSize :   the buffer size of the channel
* Return :
*			channel id
* Description :
*                   The function is used to allocate one section channel to get ECM or EMM date for TS.
* Author : 
*                   zshang 	20130412
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/
//U32   TRDRV_DEMUX_AllocateSectionChannel(U16 wMaxFilterNumber, U16 wMaxFilterSize, U32 ulBufferSize)
DMUX_CHANNEL_M 	CTIDMUX_OpenSectionChannel(INT max_filter_number, INT max_filter_size, WORD buffer_size, BYTE bConnection)
{
    U32  i;
 	tSFILTER_SlotId slotid = -1;
	SFilterCallback_t callbacks;
	
	DEBUGPF("%s	   line %d bConnection = %d\n",__FUNCTION__,__LINE__,bConnection);

	for( i =0; i< CTI_MAX_CHANNEL; i++)
	{
		if(g_cti_slot_info[i].status ==FALSE)
			break;
	 }
	
	if(i ==CTI_MAX_CHANNEL)
	{
		pbierror("%s  error line %d \n",__FUNCTION__,__LINE__);
		return DMUX_INVALID_CHANNEL_ID;
	}

    slotid = SFILTER_Allocate_Channel(bConnection);
	if(slotid < 0)
	{
		pbierror("CTIDMUX_OpenSectionChannel error line %d error 0x%x \n",__LINE__,slotid);
		return DMUX_INVALID_CHANNEL_ID;
	}

	
	callbacks.pFCallBack = (tSFILTER_SlotCallbackFct)_CasFilterInfoCallBack;
	callbacks.pFGetBuffer = (tSFILTER_GetBufferFct)_CasGetSectionDataBuffer;
	callbacks.pFFreeBuffer =(tSFILTER_FreeBufferFct)_CasFreeDataBuffer;
	SFILTER_RegisterCallBack_Function(slotid,	&callbacks);
	
	g_cti_slot_info[i].status = TRUE;
	g_cti_slot_info[i].slot_id =slotid;
	g_cti_slot_info[i].depth =max_filter_size;
	g_cti_slot_info[i].demuxid = bConnection;
	
	DEBUGPF("%s	   line %d   ok slotid %x\n",__FUNCTION__,__LINE__,slotid);
	
	return slotid;

}



/******************************************************************************
* Function :CTIDMUX_SetChannelPID
* Parameters : 
*			ulChannelId :the identifier of the channel to allocate pid
*                 wChannelPid :the pid value
* Return :
*			null
* Description :
*                   The function is set the filter pid of one allocate channel
* Author : 
*                   zshang 	20130412
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/
//void  TRDRV_DEMUX_SetChannelPid(U32 ulChannelId, U16 wChannelPid)
void CTIDMUX_SetChannelPID(DMUX_CHANNEL_M SecChannelID, WORD pid)
{
	
	U32  		i;
	S8 			scDemuxError;
//	DEBUGPF("%s	   line %d \n",__FUNCTION__,__LINE__);

	for( i =0; i< CTI_MAX_CHANNEL; i++)
	{
		if((g_cti_slot_info[i].slot_id ==SecChannelID)&&(g_cti_slot_info[i].status ==TRUE))
			break;
	}
	if(i ==CTI_MAX_CHANNEL)
	{
		pbierror("%s	error line %d \n",__FUNCTION__,__LINE__);
		return ;
	}
	
	scDemuxError = SFILTER_SetPid(g_cti_slot_info[i].slot_id, pid);
	if(scDemuxError < 0)
	{
		pbierror("%s	error line %d \n",__FUNCTION__,__LINE__);
		return ;
	}
	g_cti_slot_info[i].ReqID =pid;
		DEBUGPF("%s	   line %d   ok  SecChannelID %x    pid %x\n",__FUNCTION__,__LINE__,SecChannelID,pid);

}


/******************************************************************************
* Function :CTIDMUX_ControlChannelState
* Parameters : 
*			ulChannelId :the identifier of the channel to allocate pid
*                 eAction :the input control type
* Return :
*			null
* Description :
*                   The function is control one given channel.All allocated filters one this channel will have the same states
* Author : 
*                    zshang 	20130412
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/
//void  TRDRV_DEMUX_ControlChannel(U32 ulChannelId, CTI_DEMUX_CTRL eAction)
void CTIDMUX_ControlChannelState(DMUX_CHANNEL_M SecChannelID,BYTE WorkStatus)
{
	
	U32  		i;
	S8 			scDemuxError;
//	DEBUGPF("%s	   line %d---channel id  %d==== action%d\n",__FUNCTION__,__LINE__, SecChannelID,WorkStatus);

	for( i =0; i< CTI_MAX_CHANNEL; i++)
	{
		if((g_cti_slot_info[i].slot_id ==SecChannelID)&&(g_cti_slot_info[i].status ==TRUE))
			break;
	}
	if(i ==CTI_MAX_CHANNEL)
	{
		pbierror("%s	error line %d \n",__FUNCTION__,__LINE__);
		return ;
	}
	if(WorkStatus == DMUX_STOP_CHANNEL)
	{
		scDemuxError = SFILTER_Control_Channel(g_cti_slot_info[i].slot_id,eSFILTER_CTRL_DISABLE);
		
	}
	else if(WorkStatus == DMUX_START_CHANNEL)
	{
		scDemuxError =SFILTER_Control_Channel(g_cti_slot_info[i].slot_id,eSFILTER_CTRL_ENABLE);
	}
	else if(WorkStatus == DMUX_RESET_CHANNEL)
	{
		scDemuxError =SFILTER_Control_Channel(g_cti_slot_info[i].slot_id,eSFILTER_CTRL_RESET);
	}
	if(scDemuxError != 0)
	{
		pbierror("%s	error line %d \n",__FUNCTION__,__LINE__);
		return ;
	}
	
	DEBUGPF("%s	   line %d	 ok   SecChannelID %x   eAction %d \n",__FUNCTION__,__LINE__,SecChannelID,WorkStatus);
	

}

/******************************************************************************
* Function :CTIDMUX_RegisterChannelCallback
* Parameters : 
*			ulChannelId :the channel identifier of the to allocate channel that must register the up-call function
*                 chUpcallFct :the function to be called when  the channel get datas
* Return :
*			null
* Description :
*                   Associate a up-call function with one associated section channel . when user's drvier notifies MCELL
*                   that there is a section ,MCELL will use the function TRDRV_DEMUX_GetSectionData to read the section date
* Author : 
*                   zshang 	20130412
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/
//void  TRDRV_DEMUX_RegisterChannelUpcallFct(U32 ulChannelId, IN void (*chUpcallFct)(U32 ulChannelId))
void CTIDMUX_RegisterChannelCallback(DMUX_CHANNEL_M SecChannelID, void (*upfct)(DMUX_CHANNEL_M SecChannelID))
{
	U32 i;
	DEBUGPF("%s     line %d    SecChannelID %x \n",__FUNCTION__,__LINE__,SecChannelID);
	
	for( i =0; i< CTI_MAX_CHANNEL; i++)
	{
		if((g_cti_slot_info[i].status ==TRUE)&&(g_cti_slot_info[i].slot_id ==SecChannelID))
			break;
	}
	if(i ==CTI_MAX_CHANNEL)
	{
		pbierror("%s  error line %d \n",__FUNCTION__,__LINE__);
			return ;
	}
	g_cti_slot_info[i].CTI_upcallfunc =upfct;
	

}



/******************************************************************************
* Function :CTIDMUX_CloseSectionChannel
* Parameters : 
*			ulChannelId :the identifier of the channel to be released.
* Return :
*			null
* Description :
*                   The function is to release one section channel .
* Author : 
*                    zshang 	20130412
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/
//void  TRDRV_DEMUX_FreeSectionChannel(U32 ulChannelId)
void CTIDMUX_CloseSectionChannel(DMUX_CHANNEL_M SecChannelID)
{
	
	U32  		i;
	S8 			scDemuxError;
//	DEBUGPF("%s	   line %d \n",__FUNCTION__,__LINE__);

	for( i =0; i< CTI_MAX_CHANNEL; i++)
	{
		 if((g_cti_slot_info[i].status ==TRUE)&&(g_cti_slot_info[i].slot_id ==SecChannelID))
			break;
	}
	if(i ==CTI_MAX_CHANNEL)
	{
		pbierror("%s	error line %d \n",__FUNCTION__,__LINE__);
		return ;
	}

	scDemuxError =SFILTER_Free_Channel(SecChannelID);

	if(scDemuxError <0)
	{
		pbierror("%s	error line %d \n",__FUNCTION__,__LINE__);
		return ;
	}
	g_cti_slot_info[i].slot_id =DMUX_INVALID_CHANNEL_ID;
	g_cti_slot_info[i].status =FALSE;
	g_cti_slot_info[i].ReqID =0;
	g_cti_slot_info[i].filter_id =DMUX_INVALID_FILTER_ID;
	g_cti_slot_info[i].CTI_upcallfunc =NULL;
	g_cti_slot_info[i].unreadflag =0;
	g_cti_slot_info[i].depth =0;
	memset(g_cti_slot_info[i].section_data,0xff,CTI_MAX_SECTION_LENGTH);
	DEBUGPF("%s	   line %d  ok chanelid %x \n",__FUNCTION__,__LINE__,SecChannelID);
	

}


/******************************************************************************
* Function :CTIDMUX_OpenFilter
* Parameters : 
*			ulChannelId :the identifier of the channel to be allocate filter
* Return :
*			filter identifer
* Description :
*                   The function is to allocate one filter on the given channel.
* Author : 
*                    zshang 	20130412
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/
//U32   TRDRV_DEMUX_AllocateFilter(U32 ulChannelId)
DMUX_FILTER_M 	CTIDMUX_OpenFilter(DMUX_CHANNEL_M SecChannelID)
{
	
	U32  				i,j;
	tSFILTER_FilterId 	filterid =-1;	
	
//	DEBUGPF("%s	   line %d \n",__FUNCTION__,__LINE__);

	for( i =0; i< CTI_MAX_CHANNEL; i++)
	{
		 if((g_cti_slot_info[i].status ==TRUE)&&(g_cti_slot_info[i].slot_id ==SecChannelID))
			break;
	}
	if(i ==CTI_MAX_CHANNEL)
	{
		pbierror("%s	error line %d \n",__FUNCTION__,__LINE__);
		return DMUX_INVALID_FILTER_ID;
	}

	for( j =0; j< CTI_MAX_CHANNEL; j++)
	{
		if(g_cti_filter_info[j].status ==FALSE)
			break;
	}
	if(j ==CTI_MAX_CHANNEL)
	{
		pbierror("%s	error line %d \n",__FUNCTION__,__LINE__);
		return DMUX_INVALID_FILTER_ID;
	}

	filterid =SFILTER_Allocate_Filter(g_cti_slot_info[i].demuxid);
	if(filterid < 0)
	{
		pbierror("%s	error line %d \n",__FUNCTION__,__LINE__);
		return DMUX_INVALID_FILTER_ID;
	}
	//g_cti_filter_info[j].slot_id =SecChannelID;
	g_cti_filter_info[j].filter_id =filterid;
	g_cti_filter_info[j].status =TRUE;
	g_cti_filter_info[j].demuxid = g_cti_slot_info[i].demuxid;
	
	DEBUGPF("%s	   line %d  ok chanelid %x  filter %x\n",__FUNCTION__,__LINE__,SecChannelID,filterid);
	return filterid;

}

/******************************************************************************
* Function :CTIDMUX_SetFilterValue
* Parameters : 
*			ulChannelId :the identifier of the channel 
*			ulFilterId: the identifier of the filter to set the mask and date bytes
*			pbMask: the pointer of the mask bytes
*			pbData : the pointer of the data bytes
* Return :
*			0: successful
*			1: failure
* Description :
*                   The function is to set the mask and date bytes to filter.
* Author : 
*                   zshang 	20130412
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/
//S16   TRDRV_DEMUX_SetFilter(U32 ulChannelId, U32 ulFilterId, IN U8 *pbMask, IN U8 *pbData)
BYTE CTIDMUX_SetFilterValue(DMUX_CHANNEL_M SecChannelID,DMUX_FILTER_M FilterID, PBYTE pMask, PBYTE pMatch)
{
	
	U32  		slotno,filterno;
	S8 			scDemuxError;
	U8			negdata[FILTER_DEPTH] ={0};
	
	U8 		mask[FILTER_DEPTH] ;
	U8 		data[FILTER_DEPTH];
	U8 		i;
	U8		data_len =0;

	if( (pMask == NULL) ||(pMatch == NULL))
	{
		DEBUGPF("demux  %s	error line %d \n",__FUNCTION__,__LINE__);
		return 1;
	}
	
//	DEBUGPF("demux    %s	   line %d    channel ID %x  fileter %x  pMatch[0]  %x\n",__FUNCTION__,__LINE__,SecChannelID,FilterID,pMatch[0]);
	for( slotno =0; slotno< CTI_MAX_CHANNEL; slotno++)
	{
		 if((g_cti_slot_info[slotno].status ==TRUE)&&(g_cti_slot_info[slotno].slot_id ==SecChannelID))
			break;
	}
	if(slotno == CTI_MAX_CHANNEL)
	{
		pbierror("demux  %s	error line %d \n",__FUNCTION__,__LINE__);
		return 1;
	}

	for( filterno =0; filterno< CTI_MAX_CHANNEL; filterno++)
	{
		if(g_cti_filter_info[filterno].filter_id == FilterID)
			break;
	}
	if(filterno ==CTI_MAX_CHANNEL)
	{
		pbierror("demux  %s	error line %d \n",__FUNCTION__,__LINE__);
		return 1;
	}
	data_len = g_cti_slot_info[slotno].depth;

#if 0
{
		
		DEBUGPF("pMask: ");
		for(i=0; i<data_len; i++)
		{
			DEBUGPF("%02x ", pMask[i]);
		}
		DEBUGPF("\n");
		
		DEBUGPF("pMatch: ");
		for(i=0; i<data_len; i++)
		{
			DEBUGPF("%02x ", pMatch[i]);
		}
		DEBUGPF("\n");
}
#endif	

	
	{
		memset(data,0,16);
		memset(mask,0,16);
	    	data[0] =pMatch[0];
		mask[0] =pMask[0];
		for(i = 1; i < data_len; i++)
		{
			data[i] =pMatch[i];
			mask[i]= pMask[i];
		}
		for(i = 0; i < 16;i++)
		{
			mask[i] = ~mask[i];
		}
	
		//memset(mask+1,0xff,15);
		
	
		}
#if 0
{
		
		DEBUGPF("data: ");
		for(i=0; i<data_len; i++)
		{
			DEBUGPF("%02x ", data[i]);
		}
		DEBUGPF("\n");
		
		DEBUGPF("mask: ");
		for(i=0; i<data_len; i++)
		{
			DEBUGPF("%02x ", mask[i]);
		}
		DEBUGPF("\n");
}
#endif	
		
	if(g_cti_filter_info[filterno].slot_id ==DMUX_INVALID_CHANNEL_ID)
	{
		DEBUGPF("demux  %s	11111111111111    line %d   slotid %x fileterid %x   mak0 [%x]\n",__FUNCTION__,__LINE__,
			g_cti_slot_info[slotno].slot_id,
			g_cti_filter_info[filterno].filter_id,
			mask[0]);
		scDemuxError =SFILTER_SetFilter(g_cti_slot_info[slotno].slot_id,
										g_cti_filter_info[filterno].filter_id,
										FILTER_DEPTH,
										mask,
										data,
										negdata);
		
	}
	else //if(g_cti_filter_info[filterno].slot_id !=DMUX_INVALID_CHANNEL_ID)
	{
		DEBUGPF("demux  %s	22222222222222     line %d   slotid %x fileterid %x \n",__FUNCTION__,__LINE__,
			g_cti_slot_info[slotno].slot_id,
			g_cti_filter_info[filterno].filter_id);
		scDemuxError =SFILTER_SetFilter(g_cti_slot_info[slotno].slot_id,
												g_cti_filter_info[filterno].filter_id,
												FILTER_DEPTH,
												mask,
												data,
												negdata);

	}
	//DEBUGPF("%s	   line %d	   tableid %x  ok \n",__FUNCTION__,__LINE__,data[0]); 

	if(scDemuxError<0)
	{
		pbierror("demux  %s	error line %d \n",__FUNCTION__,__LINE__);
		return 1;
	}

//	g_cti_slot_info[slotno].data0 =data[0];
	
	g_cti_filter_info[filterno].slot_id  =SecChannelID;
	
	DEBUGPF("%s	   line %d   ok\n",__FUNCTION__,__LINE__);
	return 0;

}


/******************************************************************************
* Function :CTIDMUX_CloseFilter
* Parameters : 
*			ulChannelId :the identifier of the channel associate this filter
*			ulFilterId: the identifier of the filter to be released
* Return :
*			none
* Description :
*                   The function is to release one given filter on this channel
* Author : 
*                   zshang 	20130412
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/

//S16   TRDRV_DEMUX_FreeFilter(U32 ulChannelId, U32 ulFilterId)
void CTIDMUX_CloseFilter(DMUX_CHANNEL_M SecChannelID,DMUX_FILTER_M FilterID)
{
	U32 	filterno;
	S8		scDemuxError;
//	DEBUGPF("%s	   line %d \n",__FUNCTION__,__LINE__);

	for( filterno =0; filterno < CTI_MAX_CHANNEL; filterno++)
	{
		if((g_cti_filter_info[filterno].filter_id == FilterID)&&(g_cti_filter_info[filterno].status =TRUE))
			break;
	}
	if(filterno ==CTI_MAX_CHANNEL)
	{
		pbierror("%s	error line %d \n",__FUNCTION__,__LINE__);
		return;
	}


	scDemuxError =SFILTER_Free_Filter(g_cti_filter_info[filterno].filter_id);
	if(scDemuxError <0)
	{
		pbierror("%s	error line %d \n",__FUNCTION__,__LINE__);
		return;
	}
	g_cti_filter_info[filterno].filter_id=DMUX_INVALID_FILTER_ID;
	//g_cti_filter_info[filterno].ReqID=0x1fff;
	g_cti_filter_info[filterno].status =FALSE;
	g_cti_filter_info[filterno].slot_id =DMUX_INVALID_CHANNEL_ID;
	DEBUGPF("%s	   line %d    SecChannelID %x   FilterID %x  ok \n",__FUNCTION__,__LINE__,SecChannelID,FilterID);

	return;
	

}

/******************************************************************************
* Function :CTIDMUX_GetDataSection                        
* Parameters : 
*			ulChannelId :the identifier of the channel  to get section data
*			pulSectionSize: a pointer of the buffer length of the section data
* Return :
*			none
* Description :
*                   The function is to return the buffer pointer of the section data to be read
* Author : 
*                    zshang 	20130412
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/

//U8   *TRDRV_DEMUX_GetSectionData(U32 ulChannelId, OUT U32 *pulSectionSize)
void CTIDMUX_GetDataSection(DMUX_CHANNEL_M SecChannelID, BYTE **section_ptr, INT *pSecLen)
{
	
	U32  				i;
	
//	DEBUGPF("%s	   line %d \n",__FUNCTION__,__LINE__);
	if(pSecLen == NULL || section_ptr == NULL)
	{
		pbierror("%s	   line %d  param error\n",__FUNCTION__,__LINE__);
		OS_SemSignal(g_casSectionProcess);		
		return;
	}

	for( i =0; i< CTI_MAX_CHANNEL; i++)
	{
		if((g_cti_slot_info[i].slot_id ==SecChannelID) &&(g_cti_slot_info[i].status ==TRUE))
			break;
	}
	if(i ==CTI_MAX_CHANNEL)
	{
		pbierror("%s	error line %d \n",__FUNCTION__,__LINE__);
		g_cti_slot_info[i].unreadflag =0;
		OS_SemSignal(g_casSectionProcess);		
		return;
	}
	*pSecLen =section_length;
	//DEBUGPF("%s	   line %d   ok  \n",__FUNCTION__,__LINE__);
//	DEBUGPF("CTIDMUX_GetDataSection   ok-----tableid[%x]  section length [ %d ]\n",g_cti_slot_info[i].section_data[0],*pSecLen);
#if 1
	*section_ptr =  g_cti_slot_info[i].section_data;
#else
	*section_ptr =  g_cti_slot_info[i].buffer;
#endif

	OS_SemSignal(g_casSectionProcess);

}



/******************************************************************************
* Function :CTIDMUX_CloseSection                        
* Parameters : 
*			ulChannelId :the identifier of the channel  to release pointer
*			pulSectionSize: a pointer of the buffer length of the section data
* Return :
*			none
* Description :
*                   The function is to relase the section data. 
* Author : 
*                   zshang 	20130412
* ------------------------------
* Modification History:       
*                  Initalization
* --------------------
*****************************************************************************/

//void  TRDRV_DEMUX_FreeSectionData(U32 ulChannelId, U32 ulSectionSize)
void CTIDMUX_CloseSection(DMUX_CHANNEL_M SecChannelID, BYTE *pSection, INT SecLen)
{
	
	U32  				i;
	
//	DEBUGPF("%s	   line %d \n",__FUNCTION__,__LINE__);

	for( i =0; i< CTI_MAX_CHANNEL; i++)
	{
		if((g_cti_slot_info[i].slot_id ==SecChannelID) &&(g_cti_slot_info[i].status ==TRUE))
			break;
	}
	if(i ==CTI_MAX_CHANNEL)
	{
		pbierror("%s	error line %d \n",__FUNCTION__,__LINE__);		
//		OS_SemSignal(g_casSectionProcess);
		g_cti_slot_info[i].unreadflag =0;
		section_length =0;
		return ;
	}
   // memset(&g_cti_slot_info[i].section_data,0xff,CTI_MAX_SECTION_LENGTH);

	g_cti_slot_info[i].unreadflag =0;
	section_length =0;

//	OS_SemSignal(g_casSectionProcess);
//	DEBUGPF("%s	   line %d   ok\n",__FUNCTION__,__LINE__);

}


void CTI_DescramblerInit(void)
{
	S32  i=0,j=0;
	AM_ErrorCode_t  ret;

	AM_DSC_OpenPara_t  para;
	AM_DSC_Source_t src =AM_DSC_SRC_DMX0;	

	memset(&para,0,sizeof(AM_DSC_OpenPara_t));
	ret= AM_DSC_Open(DSC_DEV_NO,&para);
	if(ret != AM_SUCCESS)
	{
		pbierror("[DSR] %s   %d  error %x ",__FUNCTION__,__LINE__,ret);
		return;
	}
	ret= AM_DSC_SetSource(DSC_DEV_NO,src);
	if(ret != AM_SUCCESS)
	{
		pbierror("[DSR] %s   %d  error %x ",__FUNCTION__,__LINE__,ret);
		return;
	}

	for(i=0;i<CTI_MAX_DESC_NUM;i++)
	{
		CTI_Descrambler_t[i].Inuse=FALSE;
		//for(j=0;j<MAX_Ecm_MapEdPid_Num;j++)
		{
			CTI_Descrambler_t[i].SlotOrPid=INVALIDPID;
		}
		CTI_Descrambler_t[i].CTI_DescramblerHandle =0xffffffff;
		CTI_Descrambler_t[i].attcha_flag =FALSE;
	}
}

void CTI_DescramblerUnInit(void)
{
	S32  i=0,j=0;
	
	for(i=0;i<CTI_MAX_DESC_NUM;i++)
	{
		if((CTI_Descrambler_t[i].Inuse==TRUE)&&(CTI_Descrambler_t[i].CTI_DescramblerHandle!=0xffffffff))
		{
			pbierror("[DSR] CTI_DescramblerUnInit---close descramble %x\n",CTI_Descrambler_t[i].CTI_DescramblerHandle);
			CTIDMUX_CloseDescrambler(CTI_Descrambler_t[i].CTI_DescramblerHandle);		
		}
	}

}



U32 	Clear_All_Current_Keys(void)
{
	AM_ErrorCode_t	Error = AM_SUCCESS;
	S8 				pbyEvenKey[16];
	S8 				pbyOddKey[16];
	int 			i=0;

	DEBUGPF("[DSR] file %s %s	   line %d \n",__FILE__,__FUNCTION__,__LINE__);

	for(i=0;i<CTI_MAX_DESC_NUM;i++)
	{
		if( CTI_Descrambler_t[i].Inuse==FALSE)
		{
			continue;			
		}
		
		DEBUGPF("[DSR] file %s %s	   line %d   id%d\n",__FILE__,__FUNCTION__,__LINE__,CTI_Descrambler_t[i].CTI_DescramblerHandle);
		memset(pbyEvenKey,0,16);
		memset(pbyOddKey,0,16);

		AM_DSC_SetKey(DSC_DEV_NO, CTI_Descrambler_t[i].CTI_DescramblerHandle, AM_DSC_KEY_TYPE_EVEN, pbyEvenKey);
		AM_DSC_SetKey(DSC_DEV_NO, CTI_Descrambler_t[i].CTI_DescramblerHandle, AM_DSC_KEY_TYPE_ODD, pbyOddKey);

		if(Error != AM_SUCCESS)
		{
			pbierror("[DSR] file %s %s	   line %d	 ret%x\n",__FILE__,__FUNCTION__,__LINE__,Error);
			return -1;
		}
		CTIDMUX_CloseDescrambler(CTI_Descrambler_t[i].CTI_DescramblerHandle);

		//DEBUGPF("----------------------------------Clear_All_Current_Keys DVN_Descrambler_t[ %d] ok\n*************\n",i);	

	
	}
	return 0;
	
}

U32  Attach_Audio_Channels(U16 curpid,U16 lastpid)
{
	return 0;
}



U32  PVR_Attach_Descrabme(U16 pid,U32 pvr_handle)
{
	return 0;	
}



U32  PVR_Detach_Descrabme(U16 pid,U32 pvr_handle)
{
	return 0;	
}


U32  PVR_Detach_ALL_Descrabme(void)
{
	return 0;	
}


/*在时移播放结束的时候，重新绑定播放通道的音视频ch*/
U32 PVR_Retach_PlayerVideoHandle(U16  videopid)
{
	return 0;
}


/*在时移播放结束的时候，重新绑定播放通道的音视频ch*/
U32 PVR_Retach_PlayerAudioHandle(U16  audiopid)
{
	return 0;
}



/******************************************************************************
* Function :CTIDMUX_OpenDescrambler
* Parameters : 
*			none
* Return :
*			return the descrambler identifier or DMUX_INVALID_DESCRAMBLER_ID
* Description :
*                   The function is used to open a descrambler
* Author : 
*                   zshang 20130412
* ------------------------------
* Modification History:       
*                  Initalization
* --------------------
*****************************************************************************/
//U32   TRDRV_DESC_OpenDescrambler(void)
DMUX_DESCRAM_M  CTIDMUX_OpenDescrambler(BYTE bConnection)
{
	
	AM_ErrorCode_t 	 Error = AM_SUCCESS;
	U8     	 i = 0,
		 	 j = 0;
		 	 
	DEBUGPF("[DSR] file %s %s	   line %d, connection = %d \n",__FILE__,__FUNCTION__,__LINE__,bConnection);

	for(i =0;i < CTI_MAX_DESC_NUM; i++ )
	{
		if( CTI_Descrambler_t[i].Inuse == FALSE)
		{
			break;
		}
	}

	if(i >= CTI_MAX_DESC_NUM )
	{
		pbierror("[DSR] %s  error --->no descrambler !\n",__FUNCTION__);
		return DMUX_INVALID_DESCRAMBLER_ID;
	}
	
	if(bConnection==0)
	{
	    Error = AM_DSC_AllocateChannel( DSC_DEV_NO, &(CTI_Descrambler_t[i].CTI_DescramblerHandle));
		if ( Error != AM_SUCCESS)
		{
			pbierror("[DSR] DVN_Descrambler_Allocate failed!!\n*************\n");
			return DMUX_INVALID_DESCRAMBLER_ID;
		}
	}
	else
	{
		pbiinfo("[DSR] %s  DEMUX-======2 --ALLOC!\n",__FUNCTION__);
	}

	//CTI_Descrambler_t[i].ActMapedPidNum=0;
	CTI_Descrambler_t[i].Inuse=TRUE;

	//for(j=0;j<MAX_Ecm_MapEdPid_Num;j++)
	{
		CTI_Descrambler_t[i].SlotOrPid=INVALIDPID;
	}
	CTI_Descrambler_t[i].attcha_flag =FALSE;
	CTI_Descrambler_t[i].demuxid = bConnection;
	
	DEBUGPF("[DSR] %s	   line %d   id %x --------------i%d\n",__FUNCTION__,__LINE__,CTI_Descrambler_t[i].CTI_DescramblerHandle,i);

	return i;//CTI_Descrambler_t[i].CTI_DescramblerHandle;
	
}

/******************************************************************************
* Function :CTIDMUX_CloseDescrambler
* Parameters : 
*			ulDescId: the  identifier of the descramlber to be closed
* Return :
*			none
* Description :
*                   The function is used to close a descrambler
* Author : 
*                  zshang 20130412
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/
//void  TRDRV_DESC_CloseDescrambler(U32 ulDescId)
void  CTIDMUX_CloseDescrambler(DMUX_DESCRAM_M DescId)
{
	
	AM_ErrorCode_t Error = AM_SUCCESS;
	U8           i = 0,
		 	j = 0,kk;
	S32   video_handle =0;
	S32   auido_handle =0;
	
	DEBUGPF("[DSR] %s	   line %d  id=%x,demux id = %d \n",__FUNCTION__,__LINE__,DescId,CTI_Descrambler_t[DescId].demuxid);
/*
	for(i =0;i < CTI_MAX_DESC_NUM; i++ )
	{
		if( (CTI_Descrambler_t[i].Inuse==TRUE) &&(CTI_Descrambler_t[i].CTI_DescramblerHandle == DescId))
		{
			break;
		}
	}

	if(i >= CTI_MAX_DESC_NUM )
	{
		pbierror("[DSR] %s  error --->no descrambler ! uldesid %d\n",__FUNCTION__);
		return ;
	}*/
	
	if(CTI_Descrambler_t[DescId].Inuse==FALSE)
	{
		pbierror("[DSR] %s  error --->no descrambler !\n",__FUNCTION__);
		return ;
	}
	
	if(CTI_Descrambler_t[DescId].demuxid==0)
	{
		Error = AM_DSC_FreeChannel(DSC_DEV_NO, (int)(CTI_Descrambler_t[DescId].CTI_DescramblerHandle));
		if ( Error != AM_SUCCESS)
		{
			pbierror("[DSR] HI_UNF_DMX_DetachDescrambler failed!! ==0x%x\n*************\n",Error);
			return ;
		}	
	}	
	else
	{
		pbiinfo("[DSR] %s  DEMUX-======2 --free!\n",__FUNCTION__);		
	}
	CTI_Descrambler_t[DescId].Inuse = FALSE;
	CTI_Descrambler_t[DescId].CTI_DescramblerHandle = DMUX_INVALID_DESCRAMBLER_ID;
	//for(j=0;j<MAX_Ecm_MapEdPid_Num;j++)
	{
		CTI_Descrambler_t[DescId].SlotOrPid=INVALIDPID;
	}
	//CTI_Descrambler_t[i].ActMapedPidNum =0;
	CTI_Descrambler_t[DescId].attcha_flag =FALSE;

	DEBUGPF("[DSR] %s	   line %d   id%x   ok",__FUNCTION__,__LINE__,DescId);
	 
}


/******************************************************************************
* Function :TRDRV_DESC_SetDescramblerPid
* Parameters : 
*			ulDescId: the  identifier of the descramlber to be setted
*			wPid : the PID to descrambler
* Return :
*			none
* Description :
*                   The function is used to set pid on a descrambler
* Author : 
*                   zshang 20130412
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/
//void  TRDRV_DESC_SetDescramblerPid(U32 ulDescId, U16 wPid)
void  CTIDMUX_SetDescramblerPID(DMUX_DESCRAM_M DescId, INT Pid)
{
	S32  Error = AM_SUCCESS;
	U8      i = 0,kk=0;
	S32 	SlotTemp=0; 
	U32    pid_vid = 0;
	U32	   pid_aud = 0;

	DEBUGPF("[DSR]  %s %s	   line %d \n  id%x  PID %x\n",__FILE__,__FUNCTION__,__LINE__,DescId,Pid);
#if 0
	for(i =0;i < CTI_MAX_DESC_NUM; i++ )
	{
		if( (CTI_Descrambler_t[i].Inuse==TRUE) &&(CTI_Descrambler_t[i].CTI_DescramblerHandle == DescId))
		{
			break;
		}
	}

	if(i >= CTI_MAX_DESC_NUM )
	{
		pbierror("[DSR] %s  error --->no descrambler !\n",__FUNCTION__);
		return ;
	}
#else
	if(CTI_Descrambler_t[DescId].Inuse==FALSE)
	{
		pbierror("[DSR] %s  error --->no descrambler !\n",__FUNCTION__);
		return ;
	}
#endif	
	if(CTI_Descrambler_t[DescId].demuxid==0)
	{	
		Error = AM_DSC_SetChannelPID(DSC_DEV_NO, (int)(CTI_Descrambler_t[DescId].CTI_DescramblerHandle), Pid);
		if ( Error != AM_SUCCESS)
		{
			pbierror("[DSR] AM_DSC_SetChannelPID failed!! ==0x%x\n*************\n",Error);
			return ;
		}
		CTI_Descrambler_t[DescId].SlotOrPid=Pid;
		CTI_Descrambler_t[DescId].attcha_flag =TRUE;	
	}
	else
	{
		CTI_Descrambler_t[DescId].SlotOrPid=Pid;	
	}

	DEBUGPF("[DSR]  %s	   line %d \n",__FUNCTION__,__LINE__);

}



/******************************************************************************
* Function :CTIDMUX_SetDescramblerEven_Key
* Parameters : 
*			ulDescId: the  identifier of the descramlber to be setted
*			pbEvenKey : the pbEvenKey to descrambler
*			bEvenLen:   the pbEvenKey length
* Return :
*			none
* Description :
*                   The function is used to set pbEvenKey on a descrambler
* Author : 
*                   zshang 20130412
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/

//void  TRDRV_DESC_SetDescramblerEvenKey(U32 ulDescId, IN U8 *pbEvenKey, U8 bEvenLen)
void  CTIDMUX_SetDescramblerEven_Key(DMUX_DESCRAM_M DescId, PBYTE pKey)

{
	
	S32  Error = AM_SUCCESS;
	U8      i = 0;
	DEBUGPF("[DSR] [liqian ]  file %s %s	   line %d \n",__FILE__,__FUNCTION__,__LINE__);

	if( pKey == NULL )
	{
		pbierror("[DSR] %s parma error line %d \n",__FUNCTION__,__LINE__);
		return;
	}	
#if 0
	for(i =0;i < CTI_MAX_DESC_NUM; i++ )
	{
		if( (CTI_Descrambler_t[i].Inuse==TRUE) &&(CTI_Descrambler_t[i].CTI_DescramblerHandle == DescId))
		{
			break;
		}
	}

	if(i >= CTI_MAX_DESC_NUM )  
	{
		pbierror("[DSR] %s     error --->no descrambler !\n",__FUNCTION__);
		return ;
	}
#else
	DEBUGPF("[DSR] = %d\n",CTI_Descrambler_t[DescId].demuxid);

	if(CTI_Descrambler_t[DescId].Inuse==FALSE)
	{
		pbierror("[DSR] %s  error --->no descrambler !\n",__FUNCTION__);
		return ;
	}
	
#endif

	if(CTI_Descrambler_t[DescId].demuxid==0)
	{
#if 1
		{
			U8 j =0;
			DEBUGPF("\n");
			while(j<8)
			{
				DEBUGPF(" 0x%x  \n",pKey[j++]);
			}
			DEBUGPF("\n");

		}
#endif
	
		Error = AM_DSC_SetKey(DSC_DEV_NO, (int)(CTI_Descrambler_t[DescId].CTI_DescramblerHandle), AM_DSC_KEY_TYPE_EVEN, pKey);
		if(Error != AM_SUCCESS)
		{
			pbierror("[DSR] HI_UNF_DMX_SetDescramblerEvenKey( ) failed!!\n *************\n");
			
		}
	}
	else
	{
#if 1
		{
			U8 j =0;
			DEBUGPF("\n");
			while(j<8)
			{
				DEBUGPF(" 0x%x  \n",pKey[j++]);
			}
			DEBUGPF("\n");

		}
#endif
	DRV_PVR_Cas_SetKey(CTI_Descrambler_t[DescId].SlotOrPid, AM_DSC_KEY_TYPE_EVEN, pKey, 8);

	}	
	DEBUGPF("[DSR] TRDRV_DESC_SetDescramblerEvenKey   ok \n");
	return ;

}





/******************************************************************************
* Function :CTIDMUX_SetDescramblerOdd_Key
* Parameters : 
*			ulDescId: the  identifier of the descramlber to be setted
*			pbOddKey : the pbOddKey to descrambler
*			bOddLen:   the pbOddKey length
* Return :
*			none
* Description :
*                   The function is used to set pbOddKey on a descrambler
* Author : 
*                  zshang 20130412
* ------------------------------
* Modification History:       
*                     Initalization
* --------------------
*****************************************************************************/
//void  TRDRV_DESC_SetDescramblerOddKey(U32 ulDescId, IN U8 *pbOddKey, U8 bOddLen)
void  CTIDMUX_SetDescramblerOdd_Key(DMUX_DESCRAM_M DescId, PBYTE pKey)
{
	S32  Error = AM_SUCCESS;
	U8      i = 0;
	DEBUGPF("[DSR] [liqian ]  file %s %s	   line %d\n",__FILE__,__FUNCTION__,__LINE__);

	if( pKey == NULL )
	{
		pbierror("[DSR] %s parma error line %d \n",__FUNCTION__,__LINE__);
		return;
	}
	
#if 0
	{
		U8 j =0;
		DEBUGPF("\n");
		while(j<bOddLen)
		{
			DEBUGPF(" 0x%x  \n",pKey[j++]);
		}
		DEBUGPF("\n");

	}
	#endif
#if 0
	for(i =0;i < CTI_MAX_DESC_NUM; i++ )
	{
		if( (CTI_Descrambler_t[i].Inuse==TRUE) &&(CTI_Descrambler_t[i].CTI_DescramblerHandle == DescId))
		{
			break;
		}
	}

	if(i >= CTI_MAX_DESC_NUM )
	{
		pbierror("[DSR] %s  error --->no descrambler !\n",__FUNCTION__);
		return ;
	}
#else	
	DEBUGPF("[DSR] = %d\n",CTI_Descrambler_t[DescId].demuxid);
	
	if(CTI_Descrambler_t[DescId].Inuse==FALSE)
	{
		pbierror("[DSR] %s  error --->no descrambler !\n",__FUNCTION__);
		return ;
	}
#endif
	if(CTI_Descrambler_t[DescId].demuxid==0)
	{
#if 1
		{
			U8 j =0;
			DEBUGPF("\n");
			while(j<8)
			{
				DEBUGPF(" 0x%x  \n",pKey[j++]);
			}
			DEBUGPF("\n");

		}
#endif	
		Error = AM_DSC_SetKey(DSC_DEV_NO, (int)(CTI_Descrambler_t[DescId].CTI_DescramblerHandle), AM_DSC_KEY_TYPE_ODD, pKey);;
		if(Error != AM_SUCCESS)
		{
			pbierror("[DSR] AM_DSC_SetKey( ) failed!!\n *************  line%d  ret %x\n",__LINE__,Error);
		}
	}
	else
	{
#if 1
		{
			U8 j =0;
			DEBUGPF("\n");
			while(j<8)
			{
				DEBUGPF(" 0x%x  \n",pKey[j++]);
			}
			DEBUGPF("\n");

	}
#endif
		DRV_PVR_Cas_SetKey(CTI_Descrambler_t[DescId].SlotOrPid, AM_DSC_KEY_TYPE_ODD, pKey, 8);

	}
	DEBUGPF("[DSR] %s   ok \n",__FUNCTION__);
	return ;


}

