
/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name : stapi.c
* Description : Includes all other FILTER driver header files.
              	This allows a FILTER application to include only this one header
              	file directly. 
* History :
*	Date               Modification                                Name
*	----------         ------------                                ----------
*	2010/06/17         Created                                zheng dong sheng
******************************************************************************/

/*******************************************************/
/*              Includes				                                        */
/*******************************************************/
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "osapi.h" 
#include "drv_filter.h"
#include "am_dvr.h"
#include "pbitrace.h"

/*******************************************************/
/*               Private Defines and Macros			                    */
/*******************************************************/


#define INVAILD_HANDLE 				0xffffffff
#define INVAILD_ID 					-1
#define INVAILD_PID 					0x1fff
/*must be multiple of four */
#define SECTION_BUFFER_SIZE (128*1024) 
/*must be multiple of four*/

#define CHANNEL_TOTAL_NUMBER 48

#define MAX_FILTER_DEPTH 16
#define DEFAULT_MATCH 0x00
#define DEFAULT_MASK 0x00
#define DEFAULT_NEGATE 0x00

#define FILTER_INFO(...)   //printf (__VA_ARGS__)
#define FILTER_ERROR(x) printf (x)
#define FILTER_ENTER(x)
#define FILTER_RETURN(x)	return(x)

#define mLockAccess(lockKey)		(OS_SemWait(lockKey))
#define mUnLockAccess(lockKey)		(OS_SemSignal(lockKey))

/* ErrorCode */
#define kSFILTER_NO_ERROR								(0)
#define kSFILTER_ERROR_NORMAL							(-1)    //some low driver error 
#define kSFILTER_ERROR_TOO_MANY_SLOT					(kSFILTER_ERROR_NORMAL-1)   //no room for new slot
#define kSFILTER_ERROR_TOO_MANY_FILTER				(kSFILTER_ERROR_TOO_MANY_SLOT-1) //no room for new filter
#define kSFILTER_ERROR_INVALID_SLOT_ID					(kSFILTER_ERROR_TOO_MANY_FILTER-1)  // invalid slot id
#define kSFILTER_ERROR_INVALID_FILTER_ID				(kSFILTER_ERROR_INVALID_SLOT_ID-1)  // invalid filter id
#define kSFILTER_ERROR_FILTER_NOMATCH_SLOT			(kSFILTER_ERROR_INVALID_FILTER_ID-1) //filter mode not  match slot mode (sw hw)
#define kSFILTER_ERROR_PID_BEEN_TAKED					(kSFILTER_ERROR_FILTER_NOMATCH_SLOT-1) //pid have been taked by other slot
#define kSFILTER_ERROR_SLOT_NOHAVE_FILTER				(kSFILTER_ERROR_PID_BEEN_TAKED-1) // slot have no filter associated with it 
#define kSFILTER_ERROR_SLOT_HAVE_FILTER				(kSFILTER_ERROR_SLOT_NOHAVE_FILTER-1) //by now ,slot have filter associated with it ,couldn't be free
#define kSFILTER_ERROR_SLOT_NOT_REGISTED				(kSFILTER_ERROR_SLOT_HAVE_FILTER-1)//slot haven't regist with call back
#define kSFILTER_ERROR_SLOT_NO_PID						(kSFILTER_ERROR_SLOT_NOT_REGISTED-1) //slot haven't be set with pid ,so couldn't be control
#define kSFILTER_ERROR_FILTER_NODATA					(kSFILTER_ERROR_SLOT_NO_PID-1) //filter  haven't be set with data ,so couldn't be control
#define kSFILTER_ERROR_SLOT_DISABLE					(kSFILTER_ERROR_FILTER_NODATA-1)  //slot in disable state ,so reset command is illegal
#define kSFILTER_ERROR_SLOT_BEEMING_FREE				(kSFILTER_ERROR_SLOT_DISABLE-1)  //slot  beeming free right now ,but haven't finish  
#define kSFILTER_ERROR_INVALID_COMMAND				(kSFILTER_ERROR_SLOT_BEEMING_FREE-1) //your command is illegal
#define kSFILTER_ERROR_NO_MATCHED_SLOT				(kSFILTER_ERROR_INVALID_COMMAND-1) //no slot match the pid
#define kSFILTER_ERROR_INVALID_PRIORITY				(kSFILTER_ERROR_NO_MATCHED_SLOT-1) //your supply prio is illegal
#define kSFILTER_ERROR_FILTER_DISABLE					(kSFILTER_ERROR_INVALID_PRIORITY-1) //filter in disable state ,so reset command is illegal
#define kSFILTER_ERROR_INVALID_SLOT_HANDLE			(kSFILTER_ERROR_FILTER_DISABLE-1)//invaild slot handle
#define kSFILTER_ERROR_NOT_CLOSE_SET_PID				(kSFILTER_ERROR_INVALID_SLOT_HANDLE-1)//not close slot when set pid
#define kSFILTER_ERROR_SET_REPEATED_PID				(kSFILTER_ERROR_NOT_CLOSE_SET_PID-1)//pid has been setted in a channel,repeats setting the pid in another channel
#define kSFILTER_ERROR_CLOSE_SLOT						(kSFILTER_ERROR_SET_REPEATED_PID-1)//close channel error
#define kSFILTER_ERROR_DELETE_SLOT					(kSFILTER_ERROR_CLOSE_SLOT-1)//delete channel error

/*This definition may need be modified according to differnt portings.*/
#define NO_OF_SIMULTANEOUS_FILTERS			48

/*This definition may need be modified according to differnt portings.*/
#define NO_OF_SIMULTANEOUS_CHANNELS			48

#define INVERSEMASK(pucFilterMask, DEPTH)\
	{\
		int i = 0;\
		for(i = 0; i < DEPTH; i++)\
		{\
			pucFilterMask[i] = ~pucFilterMask[i];\
		}\
	}
#if 0
#define FILTERDEBUG(pucFilterMatch, pucFilterMask, DEPTH)\
	{\
		int i = 0;\
		pbiinfo("pucFilterMatch [%s|%d]", DEBUG_LOG);\
		for(i = 0; i < DEPTH; i++)\
		{\
			pbiinfo("[%x]", pucFilterMatch[i]);\
		}\
		pbiinfo("\n");\
		pbiinfo("pucFilterMask");\  
		for(i = 0; i < DEPTH; i++)\
		{\
			pbiinfo("[%x]", pucFilterMask[i]);\
		}\
	}
#else
#define FILTERDEBUG
#endif
#define DEMUX_DRIVER_NUM 0
#define DEMUX_DRIVER_NUM_1 1	
#define DEMUX_PVR_DRIVER_NUM 2
#define DEMUX_DAL_DRIVER_NUM 0

/*******************************************************/
/*               Private Types						                   */
/*******************************************************/
typedef	struct  tSFIL_Channel_e/* indexed by slot number */
{
	/* slot id */
	tSFILTER_SlotId				uiSlotId;
	tSFILTER_SlotStatus			eSlotStatus;
	U8							uDemuxId;	
	/* slot handle */
	U32							uSlotHandle ;
	tSFILTER_FilterId 			iFilterID[NO_OF_SIMULTANEOUS_FILTERS];
	/* number of the slot Associate filters  */
	U8							ucNumAssociateFilter;
	/* callback fonction to signal new section filter */
	tSFILTER_SlotCallbackFct		pFcallback;
	/* pointer to the received section data */
	U8							*pucSectionData;
	/*crc mode */
	U8							uCrcMode;
	/*channel type*/
	U8							uChannelType;
	/*output mode*/
	U8							uOutputMode;
	/*buffer size unit:kb*/
	U32							uBufferSize;
	/*slot pid*/
	U32							uSlotPid;
	/*callback functions to get or free a section buffer before a section signal callback*/
	tSFILTER_GetBufferFct			pFGetBuffer;
	tSFILTER_FreeBufferFct			pFFreeBuffer;
}tSFIL_Channel_t;

typedef	struct  tSFIL_Filter_e/* indexed by filter number */
{
	/* filter identificator */
	tSFILTER_FilterId			filterId;
	U32						filterHandle;
	/* slot id */
	tSFILTER_SlotId				SlotId;
	U8							uDemuxId;	
	/* to control the section filter*/
	U32							uiFilterDepth;
	tSFILTER_FilterStatus			eFilterStatus;
	U8 						ucFilterMask[MAX_FILTER_DEPTH];
	U8						ucFilterMatch[MAX_FILTER_DEPTH];
	U8						ucFilterNegate[MAX_FILTER_DEPTH];						
	U8						bFilterEnable;
}tSFIL_Filter_t;

typedef enum DRV_FILTER_TsSourcePort_e
{
	FILTER_TsSourcePort_QAM_IN=0,
	FILTER_TsSourcePort_QAM_OUT,
	FILTER_TsSourcePort_MEM
}DRV_FILTER_TsSourcePort_t;

/*******************************************************/
/*               Private Constants                                                    */
/*******************************************************/

/*******************************************************/
/*               Global Variables					                          */
/*******************************************************/

/*******************************************************/
/*               Private Variables (static)			                          */
/*******************************************************/
static U8					g_ucFilterInitFlag=FALSE; 
static U32					g_uiSectionTableAccess = 0;
static U32 					g_usFilterTask =0;
static tSFIL_Channel_t		**g_pptSFIL_SlotInfoTable;
static tSFIL_Filter_t		**g_pptSFIL_FilterInfoTable;
static int				PVR_Init_Source = -1;

/*******************************************************/
/*               Private Function prototypes		                          */
/*******************************************************/

/*******************************************************/
/*               Functions							                   */
/*******************************************************/
U32 g_GetDateHandleFlag = 0;
/*****************************************************************************
 Prototype    : SFILTER_Reset
 Description  : DVB fast run reset demux
 Input        : None
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2012/11/10
    Author       : zxguan
    Modification : Created function

*****************************************************************************/
S8 SFILTER_Reset()
{	
    return 0;
}



S8 SFILTER_PVR_Init()
{	
	AM_DMX_OpenPara_t 	para;
	AM_ErrorCode_t 		ErrCode = AM_SUCCESS;
	memset(&para, 0, sizeof(para));	
	ErrCode = AM_DMX_Open(DEMUX_PVR_DRIVER_NUM, &para);
	if(ErrCode != AM_SUCCESS)
	{
		pbiinfo("\n AM_DMX_Open error %d\n",ErrCode);
		FILTER_RETURN(kSFILTER_ERROR_NORMAL);
	}

	ErrCode = AM_DMX_SetSource(DEMUX_PVR_DRIVER_NUM, AM_DMX_SRC_TS0);
	if(ErrCode!=AM_SUCCESS)
	{
		pbiinfo("\n AM_DMX_SetSource error %d\n",ErrCode);
		FILTER_RETURN(kSFILTER_ERROR_NORMAL);
	}
	PVR_Init_Source = AM_DMX_SRC_TS0;	
    return 0;
}

S8 SFILTER_PVR_InitExt()
{	
	AM_DMX_OpenPara_t 	para;
	AM_ErrorCode_t 		ErrCode = AM_SUCCESS;
	memset(&para, 0, sizeof(para));	
	ErrCode = AM_DMX_Open(DEMUX_PVR_DRIVER_NUM, &para);
	if(ErrCode != AM_SUCCESS)
	{
		pbiinfo("\n AM_DMX_Open error %d\n",ErrCode);
		FILTER_RETURN(kSFILTER_ERROR_NORMAL);
	}

	ErrCode = AM_DMX_SetSource(DEMUX_PVR_DRIVER_NUM, AM_DMX_SRC_TS2);
	if(ErrCode!=AM_SUCCESS)
	{
		pbiinfo("\n AM_DMX_SetSource error %d\n",ErrCode);
		FILTER_RETURN(kSFILTER_ERROR_NORMAL);
	}
	PVR_Init_Source = AM_DMX_SRC_TS2;
    return 0;
}

S8 SFILTER_PVR_UnInit()
{	
	AM_ErrorCode_t 		ErrCode = AM_SUCCESS;	
	ErrCode = AM_DMX_Close(DEMUX_PVR_DRIVER_NUM);
	if(ErrCode != AM_SUCCESS)
	{
		pbiinfo("\n AM_DMX_Close error %d\n",ErrCode);
		FILTER_RETURN(kSFILTER_ERROR_NORMAL);
	}
	PVR_Init_Source = -1;	
    return 0;
}



S8 SFILTER_PVR_Player_Init( U32 *pBuffer )
{
    return 0;
}

S8 SFILTER_PVR_Player_UnInit( U32 *pBuffer)
{
    return 0;
}

PvrFilter_Callback_pfn gPVRpCalback = NULL;
void SFILTER_PVR_AMCallBack_Function(int dev_no, int fhandle, const uint8_t *data, int len, void *user_data)
{
	if(gPVRpCalback != NULL)
	{
		if(gPVRpCalback(data, len, user_data) == 0)
		{
			g_GetDateHandleFlag = 0;
			gPVRpCalback = NULL;
		}
	}
	
}

S8 SFILTER_PVR_AddFilter( U32 u32DmxId, U32 u32ChannelHandle, U32 u32TimeOutMs, U8 u8TableId, U16 u16ServId, U16 u16PmtPid, PvrFilter_Callback_pfn pCalback, void *pParam )
{
	AM_ErrorCode_t 	ErrCode = AM_SUCCESS;
	struct dmx_sct_filter_params param;
	int fid;
	if( NULL == pCalback || NULL == pParam )
    {
        PBIDEBUG(" param is NULL \n");
        return -1;
    }
    ErrCode = AM_DMX_AllocateFilter(DEMUX_PVR_DRIVER_NUM, &fid);
    if(AM_SUCCESS != ErrCode)
    {
    	pbierror("AM_DMX_AllocateFilter Error[%x]\n", ErrCode);
    }
    gPVRpCalback = pCalback;
	ErrCode = AM_DMX_SetCallback(DEMUX_PVR_DRIVER_NUM, fid, SFILTER_PVR_AMCallBack_Function, pParam);
	if(AM_SUCCESS != ErrCode)
    {
    	pbierror("AM_DMX_SetCallback Error[%x]\n", ErrCode);
    }
	memset(&param, 0, sizeof(param));
	param.pid = u16PmtPid;
    param.filter.filter[0] = u8TableId;
    param.filter.mask[0] = 0xff;
    if(u8TableId == 0x02)/*pmt = 0x02*/
    {
    	pbiinfo("PMT u16PmtPid[%d]\n", u16PmtPid);
    	pbiinfo("PMT u16ServId[%d]\n", u32ChannelHandle);
	    param.filter.filter[1] = (u16ServId >> 8) & 0xFF;
	    param.filter.mask[1] = 0xFF;
	    param.filter.filter[2] = (u16ServId & 0xFF);    
	    param.filter.mask[2] = 0xFF;
    }
    param.flags = DMX_CHECK_CRC;
	
	ErrCode = AM_DMX_SetSecFilter(DEMUX_PVR_DRIVER_NUM, fid, &param);
	if(AM_SUCCESS != ErrCode)
    {
    	pbierror("AM_DMX_SetSecFilter Error[%x]\n", ErrCode);
    }
	ErrCode = AM_DMX_SetBufferSize(DEMUX_PVR_DRIVER_NUM, fid, SECTION_BUFFER_SIZE);
	if(AM_SUCCESS != ErrCode)
    {
    	pbierror("AM_DMX_SetBufferSize Error[%x]\n", ErrCode);
    }
    g_GetDateHandleFlag = 1;
	ErrCode = AM_DMX_StartFilter(DEMUX_PVR_DRIVER_NUM, fid);
	if(AM_SUCCESS != ErrCode)
    {
    	pbierror("AM_DMX_StartFilter Error[%x]\n", ErrCode);
    }
    while(g_GetDateHandleFlag && u32TimeOutMs)
    {
    	usleep(1000*100);
    	u32TimeOutMs -= 100;
    }
    gPVRpCalback = NULL;
	ErrCode = AM_DMX_StopFilter(DEMUX_PVR_DRIVER_NUM, fid);
	if(AM_SUCCESS != ErrCode)
    {
    	pbierror("AM_DMX_StopFilter Error[%x]\n", ErrCode);
    }
	ErrCode = AM_DMX_FreeFilter(DEMUX_PVR_DRIVER_NUM, fid);
	if(AM_SUCCESS != ErrCode)
    {
    	pbierror("AM_DMX_FreeFilter Error[%x]\n", ErrCode);
    }
    return 0;
}



S8 SFILTER_PVR_AddPid(U32 u32DmxId, S32 pid, S32 s32ChannelType, U32 *pChannelHandle )
{
	AM_DVR_StartRecPara_t *spara = Get_Pvr_Data_Para();
	U32 i = 0, j = -1, pid_count = 0;	
	pid_count = spara->pid_count;
	for(i = 0; i < AM_DVR_MAX_PID_COUNT; i++)
	{
		if(spara->pids[i] == 0x1fff && j == -1)
		{
			spara->pids[i] = pid;
			*pChannelHandle = i; 
			spara->pid_count++;
			j = i;
		}
		else if(spara->pids[i] == pid)
		{
			if(j != -1) spara->pids[j] = 0x1fff;
			return 0;
		}
	}
	if(j == -1)
	{
		pbierror("[%s %d]not free pid; pid[0x%x],pid_count[%d].\n", DEBUG_LOG, pid , spara->pid_count);
		return -1;
	}
    return 0;
}

S8 SFILTER_PVR_DelPid(U32 u32DmxId, U32 *pChannelHandle )
{

	AM_DVR_StartRecPara_t *spara = Get_Pvr_Data_Para();
	if(*pChannelHandle >= AM_DVR_MAX_PID_COUNT )
		return 1;
	spara->pids[(*pChannelHandle)] = 0x1fff;
	spara->pid_count--;
    return 0;
}
/******************************************************************************
* Function :SFILTER_Init
* Parameters : 
*                    NULL
* Return :
*             kSFILTER_NO_ERROR
*             kSFILTER_ERROR_NORMAL
* Description :
*                   The function is used to initialize demux module.
* Author : dszheng     2010/06/17
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
S8 SFILTER_Init ( void )
{
	S32 							iIndex = 0;
	S32							sOS_Errcode = OS_SUCCESS;
	AM_ErrorCode_t 		ErrCode = AM_SUCCESS;
	AM_DMX_OpenPara_t 	para;
	if( TRUE == g_ucFilterInitFlag )
    {
        FILTER_RETURN( kSFILTER_NO_ERROR );
    }
	memset(&para, 0, sizeof(para));	
	ErrCode = AM_DMX_Open(DEMUX_DRIVER_NUM, &para);
	if(ErrCode != AM_SUCCESS)
	{
	//	OS_SemDelete(g_uiSectionTableAccess);
	//	OS_MemDeallocate( system_p, g_pptSFIL_FilterInfoTable );
		pbiinfo("\n AM_DMX_Open error %d\n",ErrCode);
		FILTER_RETURN(kSFILTER_ERROR_NORMAL);
	}
	
	//2013-09-30 gtsong cann't search 
	//ErrCode = AM_DMX_SetSource(DEMUX_DRIVER_NUM, AM_DMX_SRC_TS2);
	//AM_DMX_Close(DEMUX_DRIVER_NUM);
	//usleep(1000);
	
//	memset(&para, 0, sizeof(para));	
//	ErrCode = AM_DMX_Open(DEMUX_DRIVER_NUM, &para);
	ErrCode = AM_DMX_SetSource(DEMUX_DRIVER_NUM, AM_DMX_SRC_TS2);
	
	if(ErrCode!=AM_SUCCESS)
	{
	//	OS_SemDelete(g_uiSectionTableAccess);
	//	OS_MemDeallocate( system_p, g_pptSFIL_FilterInfoTable );
		pbiinfo("\n AM_DMX_SetSource error %d\n",ErrCode);
		FILTER_RETURN(kSFILTER_ERROR_NORMAL);
	}

	memset(&para, 0, sizeof(para));	
	ErrCode = AM_DMX_Open(DEMUX_DRIVER_NUM_1, &para);
	if(ErrCode != AM_SUCCESS)
	{
		pbiinfo("\n AM_DMX_Open error %d\n",ErrCode);
		FILTER_RETURN(kSFILTER_ERROR_NORMAL);
	}
	ErrCode = AM_DMX_SetSource(DEMUX_DRIVER_NUM_1, AM_DMX_SRC_TS0);
	
	if(ErrCode!=AM_SUCCESS)
	{
		pbiinfo("\n AM_DMX_SetSource error %d\n",ErrCode);
		FILTER_RETURN(kSFILTER_ERROR_NORMAL);
	}
	
	if(OS_SemCreate(&g_uiSectionTableAccess,"g_uiSectionTableAccess",1,0)!=0)
	{
		pbiinfo("\nSFILTER_Init :OSAL_SemCreate failure \n");
		FILTER_RETURN(kSFILTER_ERROR_NORMAL);
	}

	if(( g_pptSFIL_SlotInfoTable = ( tSFIL_Channel_t ** ) OS_MemAllocate ( system_p, NO_OF_SIMULTANEOUS_CHANNELS * sizeof( tSFIL_Channel_t * ))) == NULL )
	{
		pbiinfo("\ng_pptSFIL_SlotInfoTable general table allocation failure \n");
        OS_SemDelete(g_uiSectionTableAccess);
		FILTER_RETURN(kSFILTER_ERROR_NORMAL);
	}
 
	if(( g_pptSFIL_FilterInfoTable = ( tSFIL_Filter_t ** ) OS_MemAllocate ( system_p, NO_OF_SIMULTANEOUS_FILTERS * sizeof( tSFIL_Filter_t * ) )) == NULL )
	{
        OS_SemDelete(g_uiSectionTableAccess);
		OS_MemDeallocate( system_p, g_pptSFIL_SlotInfoTable );
		pbiinfo("\ng_pptSFIL_FilterInfoTable general table allocation failure \n");
		FILTER_RETURN(kSFILTER_ERROR_NORMAL);
	}
    
    for( iIndex=0; iIndex < NO_OF_SIMULTANEOUS_CHANNELS;iIndex++)
	{
		g_pptSFIL_SlotInfoTable[iIndex] = NULL;
	}
				
	for( iIndex=0; iIndex < NO_OF_SIMULTANEOUS_FILTERS;iIndex++ )
	{
		g_pptSFIL_FilterInfoTable[iIndex] = NULL;
	}
	g_ucFilterInitFlag = TRUE;

	FILTER_RETURN( kSFILTER_NO_ERROR );
}

/******************************************************************************
* Function :SFILTER_Terminate
* Parameters : 
*                    NULL
* Return :
*             kSFILTER_NO_ERROR
*             kSFILTER_ERROR_NORMAL
* Description :
*                   The function is used to term demux module.
* Author : dszheng     2010/06/17
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
S8 SFILTER_Terminate (void)
{
 	S32 iIndex = INVAILD_ID;
 
	FILTER_INFO("SFILTER_Terminate");

	if(TRUE == g_ucFilterInitFlag)
	{
		for(iIndex=0; iIndex < NO_OF_SIMULTANEOUS_CHANNELS;iIndex++)
		{
			if( g_pptSFIL_SlotInfoTable[iIndex] != NULL )
			{
				OS_MemDeallocate(system_p,g_pptSFIL_SlotInfoTable[iIndex]);
				g_pptSFIL_SlotInfoTable[iIndex] = NULL;
			}
		}

		for(iIndex=0; iIndex < NO_OF_SIMULTANEOUS_FILTERS;iIndex++)
		{   
			if( g_pptSFIL_FilterInfoTable[iIndex] != NULL )
			{
				OS_MemDeallocate(system_p,g_pptSFIL_FilterInfoTable[iIndex]);
				g_pptSFIL_FilterInfoTable[iIndex] = NULL;
			} 
		}

		OS_MemDeallocate(system_p,g_pptSFIL_SlotInfoTable);
		OS_MemDeallocate(system_p,g_pptSFIL_FilterInfoTable);

		OS_SemDelete(g_uiSectionTableAccess);

		g_ucFilterInitFlag=FALSE;
	}

	FILTER_INFO("Succesfully Terminated SFILTER module\n");
	FILTER_RETURN(kSFILTER_NO_ERROR);

}

/******************************************************************************
* Function :SFILTER_Allocate_Channel
* Parameters : 
*                    NULL
* Return :
*             tSFILTER_SlotId slotid
* Description :
*                   The function is used to allocate channel.
* Author : dszheng     2010/06/17
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
tSFILTER_SlotId SFILTER_Allocate_Channel ( U8	uDemuxId )
{
	S32							iSlotIndex = 0;
	mLockAccess(g_uiSectionTableAccess);

    for (iSlotIndex=0; iSlotIndex<NO_OF_SIMULTANEOUS_CHANNELS; iSlotIndex++)
	{
		if (NULL == g_pptSFIL_SlotInfoTable[iSlotIndex])
		{
			break;
		}
	}

	if( iSlotIndex >= NO_OF_SIMULTANEOUS_CHANNELS )
	{
		mUnLockAccess(g_uiSectionTableAccess);
		printf("SFILTER_Allocate_Channel => No Free Channel \n");
		FILTER_RETURN((tSFILTER_SlotId)kSFILTER_ERROR_TOO_MANY_SLOT);
	}

	if((g_pptSFIL_SlotInfoTable[iSlotIndex] = (tSFIL_Channel_t *)OS_MemAllocate(system_p, sizeof(tSFIL_Channel_t))) == NULL)
	{	
		mUnLockAccess(g_uiSectionTableAccess);
		printf("Section filter element table allocation failure \n");
		FILTER_RETURN((tSFILTER_SlotId)kSFILTER_ERROR_NORMAL);
	}
	else
	{
		memset( g_pptSFIL_SlotInfoTable[iSlotIndex], 0, sizeof( tSFIL_Channel_t ) );
		
		g_pptSFIL_SlotInfoTable[iSlotIndex]->uiSlotId = iSlotIndex;
		g_pptSFIL_SlotInfoTable[iSlotIndex]->ucNumAssociateFilter = 0;
		g_pptSFIL_SlotInfoTable[iSlotIndex]->pFcallback =NULL;
		g_pptSFIL_SlotInfoTable[iSlotIndex]->pucSectionData=NULL;
		g_pptSFIL_SlotInfoTable[iSlotIndex]->pFGetBuffer =NULL;
		g_pptSFIL_SlotInfoTable[iSlotIndex]->pFFreeBuffer =NULL;
		g_pptSFIL_SlotInfoTable[iSlotIndex]->eSlotStatus = eSFILTER_SLOT_NOT_USE;
		g_pptSFIL_SlotInfoTable[iSlotIndex]->uSlotHandle = INVAILD_HANDLE;
		g_pptSFIL_SlotInfoTable[iSlotIndex]->uSlotPid = INVAILD_PID;
		g_pptSFIL_SlotInfoTable[iSlotIndex]->uBufferSize = SECTION_BUFFER_SIZE;
		g_pptSFIL_SlotInfoTable[iSlotIndex]->uCrcMode = DMX_CHECK_CRC;
		g_pptSFIL_SlotInfoTable[iSlotIndex]->eSlotStatus = eSFILTER_SLOT_ALLOCATE;
		g_pptSFIL_SlotInfoTable[iSlotIndex]->uSlotHandle = 0;
		g_pptSFIL_SlotInfoTable[iSlotIndex]->uDemuxId =uDemuxId;
		memset(g_pptSFIL_SlotInfoTable[iSlotIndex]->iFilterID, -1, NO_OF_SIMULTANEOUS_FILTERS);
		//g_pptSFIL_SlotInfoTable[iSlotIndex]->iFilterID = -1;
	}
	
	mUnLockAccess(g_uiSectionTableAccess);

	FILTER_RETURN(iSlotIndex);
}

/******************************************************************************
* Function :SFILTER_Free_Channel
* Parameters : 
*                    tSFILTER_SlotId iSlotID
* Return :
*             kSFILTER_NO_ERROR
*             kSFILTER_ERROR_INVALID_SLOT_ID
*             kSFILTER_ERROR_INVALID_SLOT_HANDLE
*             kSFILTER_ERROR_CLOSE_SLOT
*             kSFILTER_ERROR_DELETE_SLOT
* Description :
*                   The function is used to free channel.
* Author : dszheng     2010/06/17
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
S8 SFILTER_Free_Channel( tSFILTER_SlotId iSlotID )
{
	AM_ErrorCode_t	ErrCode = AM_SUCCESS;
	mLockAccess(g_uiSectionTableAccess);

	if(iSlotID >= NO_OF_SIMULTANEOUS_CHANNELS || iSlotID < 0)
	{
		mUnLockAccess(g_uiSectionTableAccess);
		FILTER_INFO( "SFILTER_RegisterCallBack_Function => invalid Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}
	else if(g_pptSFIL_SlotInfoTable[iSlotID] == NULL)
	{
		mUnLockAccess(g_uiSectionTableAccess);
		FILTER_INFO( "SFILTER_RegisterCallBack_Function => FREE Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}
	else if( iSlotID != g_pptSFIL_SlotInfoTable[iSlotID]->uiSlotId )
	{
		mUnLockAccess(g_uiSectionTableAccess);
		FILTER_INFO( "SFILTER_RegisterCallBack_Function => INVAILD Slot handle \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_HANDLE);
	}
	else if( INVAILD_HANDLE == g_pptSFIL_SlotInfoTable[iSlotID]->uSlotHandle )
	{
		mUnLockAccess(g_uiSectionTableAccess);
		FILTER_INFO( "SFILTER_RegisterCallBack_Function => INVAILD Slot handle \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}
	else 
	{
		int i = 0, ucNumAssociateFilter = 0;
		tSFILTER_FilterId iFilterID = 0;
		ucNumAssociateFilter = g_pptSFIL_SlotInfoTable[iSlotID]->ucNumAssociateFilter;
		for(i = 0; i < ucNumAssociateFilter; i++)
		{
			//SFILTER_Free_Filter( g_pptSFIL_SlotInfoTable[iSlotID]->iFilterID[i] );//µ÷ÓÃ»áËÀËø
			iFilterID = g_pptSFIL_SlotInfoTable[iSlotID]->iFilterID[i];
			if(iFilterID >= NO_OF_SIMULTANEOUS_FILTERS || iFilterID < 0) 
			{
				FILTER_INFO( "SFILTER_Free => invalid Filter id \n");
				continue;
			}

			if(( NULL == g_pptSFIL_FilterInfoTable[iFilterID] ) || ( iFilterID != g_pptSFIL_FilterInfoTable[iFilterID]->filterId ))
			{
				FILTER_INFO("SFILTER_Free => invalid Filter id \n");
				continue;
			}

			if( INVAILD_HANDLE == g_pptSFIL_FilterInfoTable[iFilterID]->filterHandle )
			{
				FILTER_INFO("SFILTER_Free =>  invaild filter handle \n");
				continue;
			}
			if( g_pptSFIL_FilterInfoTable[iFilterID]->eFilterStatus  == eSFILTER_FILTER_ATTACH_SLOT )
			{
				ErrCode = AM_DMX_StopFilter(g_pptSFIL_FilterInfoTable[iFilterID]->uDemuxId, g_pptSFIL_FilterInfoTable[iFilterID]->filterHandle); 
				if( ErrCode != AM_SUCCESS )
				{
					mUnLockAccess(g_uiSectionTableAccess);		
					FILTER_INFO("SFILTER_Control => detach filter Error %x\n",ErrCode);
					continue;
				}
			}
			ErrCode = AM_DMX_FreeFilter(g_pptSFIL_FilterInfoTable[iFilterID]->uDemuxId, g_pptSFIL_FilterInfoTable[iFilterID]->filterHandle);
			if( ErrCode != AM_SUCCESS )
			{
				mUnLockAccess(g_uiSectionTableAccess);		
				FILTER_INFO("\nFree filter Error %d\n",ErrCode);
				continue;
			}
			OS_MemDeallocate( system_p, g_pptSFIL_FilterInfoTable[iFilterID] );
			g_pptSFIL_FilterInfoTable[iFilterID] = NULL;
			g_pptSFIL_SlotInfoTable[iSlotID]->ucNumAssociateFilter--;
		}
		OS_MemDeallocate( system_p, g_pptSFIL_SlotInfoTable[iSlotID] );
		g_pptSFIL_SlotInfoTable[iSlotID] = NULL;
	}

	mUnLockAccess(g_uiSectionTableAccess);

	FILTER_RETURN(kSFILTER_NO_ERROR);
}

void SFILTER_AMCallBack_Function(int dev_no, int fhandle, const uint8_t *data, int len, void *user_data)
{
	tSFIL_Channel_t		*SFIL_SlotInfoTable = (tSFIL_Channel_t *)user_data;
	//pbiinfo("SFILTER_AMCallBack_Function  data0[%x] \n",data[0]);
	U8 ii =0,find=0, iFilterID = 0;
    U8 *pBuffer = NULL;
    
	if(SFIL_SlotInfoTable  == NULL)
	{
		pbiinfo("%s PARAM error ********** %d\n",__FUNCTION__);	
		return;
	}
	for(ii =0;ii<SFIL_SlotInfoTable->ucNumAssociateFilter;ii++)
	{
		iFilterID = SFIL_SlotInfoTable->iFilterID[ii];
		if(NULL != g_pptSFIL_FilterInfoTable[iFilterID] && fhandle == g_pptSFIL_FilterInfoTable[iFilterID]->filterHandle)
		{
			find =1;
			break;
		}		
	}
	
	if((find ==0)||(len <0))
	{
		pbiinfo("%s data error ********** %d  fhandle%d,===%d\n",__FUNCTION__,len,fhandle,
			SFIL_SlotInfoTable->iFilterID[0]);  
		return;
	}

    if( NULL != SFIL_SlotInfoTable->pFGetBuffer )
    {
        pBuffer = SFIL_SlotInfoTable->pFGetBuffer(SFIL_SlotInfoTable->uiSlotId);
	    if( NULL == pBuffer )
        {
            return;
        }   
        memcpy( pBuffer, data, len );
    }
    
	if(SFIL_SlotInfoTable->pFcallback != NULL)
    {
		SFIL_SlotInfoTable->pFcallback(SFIL_SlotInfoTable->uiSlotId, 
										iFilterID, 
										SFIL_SlotInfoTable->pFFreeBuffer, 
										pBuffer, 
										len, 
										SFIL_SlotInfoTable->uSlotPid);
        AM_DMX_Sync(dev_no); 
    }
    else
    {
        SFIL_SlotInfoTable->pFFreeBuffer(SFIL_SlotInfoTable->uiSlotId, pBuffer);
    }
}

S8 SFILTER_CheakFilterID(tSFILTER_FilterId		iFilter)
{
	if(iFilter >= NO_OF_SIMULTANEOUS_FILTERS || iFilter < 0) 
	{
		pbiinfo( "SFILTER_CheakFilterID => invalid Filter id %d \n", iFilter);
		FILTER_RETURN(kSFILTER_ERROR_INVALID_FILTER_ID);
	}
	if(( NULL == g_pptSFIL_FilterInfoTable[iFilter] ) || ( iFilter != g_pptSFIL_FilterInfoTable[iFilter]->filterId ))
	{
		//mUnLockAccess(g_uiSectionTableAccess);	
		pbiinfo( "SFILTER_CheakFilterID => invalid Filter id %d \n", iFilter);
		FILTER_RETURN(kSFILTER_ERROR_INVALID_FILTER_ID);
	}

	if( INVAILD_HANDLE == g_pptSFIL_FilterInfoTable[iFilter]->filterHandle )
	{
		//mUnLockAccess(g_uiSectionTableAccess);		
		pbiinfo( "SFILTER_CheakFilterID => invalid Filter id %d \n", iFilter);
		FILTER_RETURN(kSFILTER_ERROR_FILTER_DISABLE);
	}
	FILTER_RETURN(kSFILTER_NO_ERROR);
}

/******************************************************************************
* Function :SFILTER_RegisterCallBack_Function
* Parameters : 
*                    tSFILTER_SlotId iSlotID , SFilterCallback_t *ptCallbacks
* Return :
*             tSFILTER_SlotId slotid
* Description :
*                   The function is used to regist a call back function for DVBSectionFilterTask.
* Author : dszheng     2010/06/17
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/

S8 SFILTER_RegisterCallBack_Function( tSFILTER_SlotId iSlotID, SFilterCallback_t *ptCallbacks )
{
	S8	ErrCode=kSFILTER_NO_ERROR;
	S8 FilterErr = kSFILTER_NO_ERROR;
	assert(NULL != ptCallbacks->pFGetBuffer);
	assert(NULL != ptCallbacks->pFFreeBuffer);
	assert(NULL != ptCallbacks->pFCallBack);

	mLockAccess(g_uiSectionTableAccess);
	if(iSlotID >= NO_OF_SIMULTANEOUS_CHANNELS || iSlotID < 0)
	{
		FILTER_INFO( "SFILTER_RegisterCallBack_Function => invalid Slot id \n");
		ErrCode =  kSFILTER_ERROR_INVALID_SLOT_ID;
	}
	else if(g_pptSFIL_SlotInfoTable[iSlotID] == NULL)
	{
		FILTER_INFO( "SFILTER_RegisterCallBack_Function => FREE Slot id \n");
		ErrCode =  kSFILTER_ERROR_INVALID_SLOT_ID;
	}
	else if( INVAILD_HANDLE == g_pptSFIL_SlotInfoTable[iSlotID]->uSlotHandle )
	{
		FILTER_INFO( "SFILTER_RegisterCallBack_Function => INVAILD Slot handle \n");
		ErrCode =  kSFILTER_ERROR_INVALID_SLOT_ID;
	}
	else if( iSlotID != g_pptSFIL_SlotInfoTable[iSlotID]->uiSlotId )
	{
		FILTER_INFO( "SFILTER_RegisterCallBack_Function => INVAILD Slot handle \n");
		ErrCode =  kSFILTER_ERROR_INVALID_SLOT_HANDLE;
	}
	else
	{
		g_pptSFIL_SlotInfoTable[iSlotID]->pFGetBuffer = ptCallbacks->pFGetBuffer;
		g_pptSFIL_SlotInfoTable[iSlotID]->pFFreeBuffer = ptCallbacks->pFFreeBuffer;
		g_pptSFIL_SlotInfoTable[iSlotID]->pFcallback = ptCallbacks->pFCallBack;		
	}

	mUnLockAccess(g_uiSectionTableAccess);
	FILTER_RETURN(ErrCode);
}

/******************************************************************************
* Function :SFILTER_Allocate_Filter
* Parameters : 
*                    NULL
* Return :
*             tSFILTER_FilterId filterid
* Description :
*                   The function is used to allocate filter.
* Author : dszheng     2010/06/17
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/

tSFILTER_FilterId SFILTER_Allocate_Filter ( U8		uDemuxId )
{
	AM_ErrorCode_t				ErrCode = AM_SUCCESS;
	S32 							iFilterIndex = 0;
	
	mLockAccess(g_uiSectionTableAccess);

	while( iFilterIndex < NO_OF_SIMULTANEOUS_FILTERS )
	{
		if( g_pptSFIL_FilterInfoTable[iFilterIndex] != NULL )
		{
			iFilterIndex++;
		}
		else
		{
			break;
		}
	}	
	
	if( iFilterIndex >= NO_OF_SIMULTANEOUS_FILTERS )
	{
		mUnLockAccess(g_uiSectionTableAccess);
		printf("SFILTER Allocate Filter => No Free Filter \n");
		FILTER_RETURN((tSFILTER_FilterId)kSFILTER_ERROR_TOO_MANY_FILTER);
	}

	if((g_pptSFIL_FilterInfoTable[iFilterIndex] = (tSFIL_Filter_t *)OS_MemAllocate(system_p,sizeof(tSFIL_Filter_t))) == NULL)
	{
		mUnLockAccess(g_uiSectionTableAccess);
		printf("Section filter element table allocation failure \n");
		FILTER_RETURN((tSFILTER_FilterId)kSFILTER_ERROR_NORMAL);
	}
	else
	{
		memset(g_pptSFIL_FilterInfoTable[iFilterIndex],0,sizeof( tSFIL_Filter_t ));
		
		g_pptSFIL_FilterInfoTable[iFilterIndex]->filterId = iFilterIndex;
		g_pptSFIL_FilterInfoTable[iFilterIndex]->filterHandle = INVAILD_HANDLE;
		g_pptSFIL_FilterInfoTable[iFilterIndex]->SlotId = INVAILD_ID;
		g_pptSFIL_FilterInfoTable[iFilterIndex]->eFilterStatus = ( tSFILTER_FilterStatus )eSFILTER_FILTER_NOT_USE;
		g_pptSFIL_FilterInfoTable[iFilterIndex]->uDemuxId = uDemuxId;
		ErrCode == AM_DMX_AllocateFilter(g_pptSFIL_FilterInfoTable[iFilterIndex]->uDemuxId, &(g_pptSFIL_FilterInfoTable[iFilterIndex]->filterHandle));
		if(ErrCode != AM_SUCCESS || g_pptSFIL_FilterInfoTable[iFilterIndex]->filterHandle == INVAILD_HANDLE)
		{
			OS_MemDeallocate(system_p,g_pptSFIL_FilterInfoTable[iFilterIndex]);
			mUnLockAccess(g_uiSectionTableAccess);
			printf("Allocate new filter error %d\n", ErrCode);
			FILTER_RETURN((tSFILTER_FilterId)kSFILTER_ERROR_NORMAL);
		}
		ErrCode == AM_DMX_SetBufferSize(g_pptSFIL_FilterInfoTable[iFilterIndex]->uDemuxId, g_pptSFIL_FilterInfoTable[iFilterIndex]->filterHandle, SECTION_BUFFER_SIZE);
		if(ErrCode != AM_SUCCESS)
		{
			OS_MemDeallocate(system_p,g_pptSFIL_FilterInfoTable[iFilterIndex]);
			mUnLockAccess(g_uiSectionTableAccess);
			printf("AM_DMX_SetBufferSize error %d\n", ErrCode);
			FILTER_RETURN((tSFILTER_FilterId)kSFILTER_ERROR_NORMAL);
		}
		g_pptSFIL_FilterInfoTable[iFilterIndex]->filterId = iFilterIndex;
		g_pptSFIL_FilterInfoTable[iFilterIndex]->eFilterStatus = eSFILTER_FILTER_ALLOCATE;
	}
	
	mUnLockAccess( g_uiSectionTableAccess );
	FILTER_RETURN( (tSFILTER_FilterId)iFilterIndex );
}



/******************************************************************************
* Function :SFILTER_SetPid
* Parameters : 
*                    tSFILTER_SlotId iSlot, U32 uiPid
* Return :
*		kSFILTER_NO_ERROR
*		kSFILTER_ERROR_NORMAL
*		kSFILTER_ERROR_INVALID_SLOT_ID
*		kSFILTER_ERROR_SET_REPEATED_PID
*		kSFILTER_ERROR_NOT_CLOSE_SET_PID
* Description :
*                   The function is used to set channel pid.
* Author : dszheng     2010/06/17
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
S8 SFILTER_SetPid( tSFILTER_SlotId iSlot, U32 uiPid )
{
	S32					iSlotIndex =0;
	AM_ErrorCode_t		ErrCode = AM_SUCCESS;
	struct dmx_sct_filter_params param;
	tSFILTER_FilterId		iFilter;
	S8 FilterErr = kSFILTER_NO_ERROR;

	mLockAccess(g_uiSectionTableAccess);


	if(iSlot < 0||iSlot >= NO_OF_SIMULTANEOUS_CHANNELS || NULL == g_pptSFIL_SlotInfoTable[iSlot])
	{
		mUnLockAccess(g_uiSectionTableAccess);
		pbiinfo("kSFILTER_ERROR_INVALID_SLOT_ID in SFILTER_SetPid func\n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}

	if(( NULL == g_pptSFIL_SlotInfoTable[iSlot] ) || ( iSlot != g_pptSFIL_SlotInfoTable[iSlot]->uiSlotId ))
	{
		mUnLockAccess(g_uiSectionTableAccess);
		pbiinfo("SFILTER_Set => invalid Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}

	for( iSlotIndex = 0; iSlotIndex < NO_OF_SIMULTANEOUS_CHANNELS; iSlotIndex++ )
	{
		if( g_pptSFIL_SlotInfoTable[iSlotIndex] == NULL)
		{
			continue;
		}
		else
		{
			if(( g_pptSFIL_SlotInfoTable[iSlotIndex]->uSlotPid == uiPid )
				&&(g_pptSFIL_SlotInfoTable[iSlotIndex]->uDemuxId ==g_pptSFIL_SlotInfoTable[iSlot]->uDemuxId))
			{
				pbiinfo("\nThe pid has been setted in one channel\n");
				break;
				/*qbxu m 20100720*/
			}
		}
	}
		
	if( g_pptSFIL_SlotInfoTable[iSlot]->eSlotStatus == eSFILTER_SLOT_NOT_USE )
	{
		mUnLockAccess(g_uiSectionTableAccess);
		pbiinfo("SFILTER_Set => Slot isn't allcoated \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}
	else
	{
		int i = 0, ucNumAssociateFilter = 0;
		ucNumAssociateFilter = g_pptSFIL_SlotInfoTable[iSlot]->ucNumAssociateFilter;
		for(i = 0; i < ucNumAssociateFilter; i++)
		{
			iFilter = g_pptSFIL_SlotInfoTable[iSlot]->iFilterID[i];
			FilterErr = SFILTER_CheakFilterID(iFilter);
			if(FilterErr != kSFILTER_NO_ERROR || g_pptSFIL_FilterInfoTable[iFilter]->eFilterStatus == eSFILTER_FILTER_ATTACH_SLOT )
			{
				mUnLockAccess(g_uiSectionTableAccess);
				pbiinfo("SFILTER_CheakFilterID => Slot isn't allcoated \n");
				FILTER_RETURN(FilterErr);
			}
			memset(&param, 0, sizeof(param));
			param.pid = uiPid;
			memcpy( param.filter.filter, g_pptSFIL_FilterInfoTable[iFilter]->ucFilterMatch, g_pptSFIL_FilterInfoTable[iFilter]->uiFilterDepth); 
			memcpy( param.filter.mask, g_pptSFIL_FilterInfoTable[iFilter]->ucFilterMask, g_pptSFIL_FilterInfoTable[iFilter]->uiFilterDepth);
			memcpy( param.filter.mode, g_pptSFIL_FilterInfoTable[iFilter]->ucFilterNegate, g_pptSFIL_FilterInfoTable[iFilter]->uiFilterDepth );
			param.flags = DMX_CHECK_CRC;
			pbiinfo( "%s  %d  \n", __FUNCTION__,__LINE__);
			FILTERDEBUG(g_pptSFIL_FilterInfoTable[iFilter]->ucFilterMatch, g_pptSFIL_FilterInfoTable[iFilter]->ucFilterMask, g_pptSFIL_FilterInfoTable[iFilter]->uiFilterDepth);
			ErrCode = AM_DMX_SetSecFilter(g_pptSFIL_FilterInfoTable[iFilter]->uDemuxId, g_pptSFIL_FilterInfoTable[iFilter]->filterHandle, &param);
			if( ErrCode != AM_SUCCESS )
			{
				mUnLockAccess(g_uiSectionTableAccess);		
				pbiinfo( "Filter attach slot error %x \n", ErrCode);
				FILTER_RETURN(kSFILTER_ERROR_FILTER_NOMATCH_SLOT);
			}
			g_pptSFIL_FilterInfoTable[iFilter]->eFilterStatus = eSFILTER_FILTER_SET_DATA;
		}
	}
	
	g_pptSFIL_SlotInfoTable[iSlot]->uSlotPid = uiPid;

	mUnLockAccess(g_uiSectionTableAccess);
	
	FILTER_RETURN(kSFILTER_NO_ERROR);
}

/******************************************************************************
* Function :SFILTER_SetFilter
* Parameters : 
*                   tSFILTER_SlotId	iSlot,
*			tSFILTER_FilterId	iFilter,	
*			U32				uiFilterDepth,
*			U8				*pucFilterMask,
*			U8				*pucFilterMatch,
*			U8				*pucFilterNegate
* Return :
*		kSFILTER_NO_ERROR
*		kSFILTER_ERROR_NORMAL
*		kSFILTER_ERROR_INVALID_SLOT_ID
*		kSFILTER_ERROR_INVALID_FILTER_ID
*		kSFILTER_ERROR_SLOT_DISABLE
*		kSFILTER_ERROR_FILTER_NOMATCH_SLOT
* Description :
*                   The function is used to set channel pid.
* Author : dszheng     2010/06/17
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
S8 SFILTER_SetFilter( tSFILTER_SlotId		iSlot,
						tSFILTER_FilterId		iFilter,	
						U32					uiFilterDepth,
						U8					*pucFilterMask,
						U8					*pucFilterMatch,
						U8					*pucFilterNegate)
{
	AM_ErrorCode_t				ErrCode = AM_SUCCESS;
	int ii =0,find=0;

	struct dmx_sct_filter_params param;
	if( iSlot >= NO_OF_SIMULTANEOUS_CHANNELS || iSlot < 0)
	{
		pbiinfo( "SFILTER_Set => invalid Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}
	if(iFilter >= NO_OF_SIMULTANEOUS_FILTERS || iFilter < 0) 
	{
		pbiinfo( "SFILTER_Set => invalid Filter id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_FILTER_ID);
	}
	
	mLockAccess(g_uiSectionTableAccess);

	if(( NULL == g_pptSFIL_SlotInfoTable[iSlot] ) || ( iSlot != g_pptSFIL_SlotInfoTable[iSlot]->uiSlotId ))
	{
		mUnLockAccess(g_uiSectionTableAccess);	
		pbiinfo("SFILTER_Set => invalid Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}

	if(( NULL == g_pptSFIL_FilterInfoTable[iFilter] ) || ( iFilter != g_pptSFIL_FilterInfoTable[iFilter]->filterId ))
	{
		mUnLockAccess(g_uiSectionTableAccess);	
		pbiinfo("SFILTER_Set => invalid Filter id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_FILTER_ID);
	}

	if( INVAILD_HANDLE == g_pptSFIL_SlotInfoTable[iSlot]->uSlotHandle )
	{
		mUnLockAccess(g_uiSectionTableAccess);		
		pbiinfo("SFILTER_Set =>  invaild slot handle \n");
		FILTER_RETURN(kSFILTER_ERROR_SLOT_DISABLE);
	}

	if( INVAILD_HANDLE == g_pptSFIL_FilterInfoTable[iFilter]->filterHandle )
	{
		mUnLockAccess(g_uiSectionTableAccess);		
		pbiinfo("SFILTER_Set =>  invaild filter handle \n");
		FILTER_RETURN(kSFILTER_ERROR_FILTER_DISABLE);
	}

	if( uiFilterDepth > MAX_FILTER_DEPTH )
	{
		mUnLockAccess(g_uiSectionTableAccess);		
		pbiinfo("Filter depth is bigger than MAX_FILTER_DEPTH \n");
		FILTER_RETURN(kSFILTER_ERROR_NORMAL);
	}
	for(ii =0;ii<g_pptSFIL_SlotInfoTable[iSlot]->ucNumAssociateFilter;ii++)
	{
		if(g_pptSFIL_SlotInfoTable[iSlot]->iFilterID[ii] ==iFilter)
		{
			find =1;
		}
		else
		{
			continue;
		}
	}
	if(find ==0)
	{
	g_pptSFIL_SlotInfoTable[iSlot]->iFilterID[g_pptSFIL_SlotInfoTable[iSlot]->ucNumAssociateFilter] = iFilter;
	g_pptSFIL_SlotInfoTable[iSlot]->ucNumAssociateFilter++;
	}
	ErrCode = AM_DMX_SetCallback(g_pptSFIL_FilterInfoTable[iFilter]->uDemuxId, g_pptSFIL_FilterInfoTable[iFilter]->filterHandle, SFILTER_AMCallBack_Function, (void *)g_pptSFIL_SlotInfoTable[iSlot]);
	if(ErrCode != AM_SUCCESS)
	{
		mUnLockAccess(g_uiSectionTableAccess);		
		pbiinfo("AM_DMX_SetCallback=------ret%x \n",ErrCode);
		FILTER_RETURN(ErrCode);
	}
	
	INVERSEMASK(pucFilterMask, uiFilterDepth);
	if(g_pptSFIL_SlotInfoTable[iSlot]->uSlotPid != INVAILD_PID)
	{
		memset(&param, 0, sizeof(param));
		param.pid = g_pptSFIL_SlotInfoTable[iSlot]->uSlotPid;
		memcpy( param.filter.filter, pucFilterMatch, uiFilterDepth); 
		memcpy( param.filter.mask, pucFilterMask, uiFilterDepth);
		memcpy( param.filter.mode, pucFilterNegate, uiFilterDepth );
		param.flags = DMX_CHECK_CRC;
		FILTERDEBUG(pucFilterMatch, pucFilterMask, uiFilterDepth);
		ErrCode = AM_DMX_SetSecFilter(g_pptSFIL_FilterInfoTable[iFilter]->uDemuxId, g_pptSFIL_FilterInfoTable[iFilter]->filterHandle, &param);
		if( ErrCode != AM_SUCCESS )
		{
			mUnLockAccess(g_uiSectionTableAccess);		
			pbiinfo( "Filter attach slot error %x \n", ErrCode);
			FILTER_RETURN(kSFILTER_ERROR_FILTER_NOMATCH_SLOT);
		}
	}
	g_pptSFIL_FilterInfoTable[iFilter]->uiFilterDepth = uiFilterDepth;
	
	g_pptSFIL_FilterInfoTable[iFilter]->eFilterStatus = eSFILTER_FILTER_SET_DATA;

	memset( g_pptSFIL_FilterInfoTable[iFilter]->ucFilterMatch, DEFAULT_MATCH, MAX_FILTER_DEPTH );
	memset( g_pptSFIL_FilterInfoTable[iFilter]->ucFilterMask, DEFAULT_MASK, MAX_FILTER_DEPTH );
	memset( g_pptSFIL_FilterInfoTable[iFilter]->ucFilterNegate, DEFAULT_NEGATE, MAX_FILTER_DEPTH );

	memcpy( g_pptSFIL_FilterInfoTable[iFilter]->ucFilterMatch, pucFilterMatch, uiFilterDepth); 
	memcpy( g_pptSFIL_FilterInfoTable[iFilter]->ucFilterMask, pucFilterMask, uiFilterDepth);
	memcpy( g_pptSFIL_FilterInfoTable[iFilter]->ucFilterNegate, pucFilterNegate, uiFilterDepth );
	g_pptSFIL_FilterInfoTable[iFilter]->SlotId = iSlot;	
	g_pptSFIL_FilterInfoTable[iFilter]->eFilterStatus = eSFILTER_FILTER_SET_DATA;
	
	

	mUnLockAccess(g_uiSectionTableAccess);
	FILTER_RETURN(kSFILTER_NO_ERROR);
}
S8 SFILTER_SetFilter_NOATTACH( tSFILTER_SlotId		iSlot,
						tSFILTER_FilterId		iFilter,	
						U32					uiFilterDepth,
						U8					*pucFilterMask,
						U8					*pucFilterMatch,
						U8					*pucFilterNegate)
{
	AM_ErrorCode_t				ErrCode = AM_SUCCESS;
	int ii =0,find=0;   
	struct dmx_sct_filter_params param;
	if( iSlot >= NO_OF_SIMULTANEOUS_CHANNELS || iSlot < 0)
	{
		printf( "SFILTER_Set => invalid Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}
	if(iFilter >= NO_OF_SIMULTANEOUS_FILTERS || iFilter < 0) 
	{
		printf( "SFILTER_Set => invalid Filter id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_FILTER_ID);
	}
	mLockAccess(g_uiSectionTableAccess);
	if(( NULL == g_pptSFIL_SlotInfoTable[iSlot] ) || ( iSlot != g_pptSFIL_SlotInfoTable[iSlot]->uiSlotId ))
	{
		mUnLockAccess(g_uiSectionTableAccess);	
		pbiinfo("SFILTER_Set => invalid Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}
	if(( NULL == g_pptSFIL_FilterInfoTable[iFilter] ) || ( iFilter != g_pptSFIL_FilterInfoTable[iFilter]->filterId ))
	{
		mUnLockAccess(g_uiSectionTableAccess);	
		pbiinfo("SFILTER_Set => invalid Filter id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_FILTER_ID);
	}
	if( INVAILD_HANDLE == g_pptSFIL_SlotInfoTable[iSlot]->uSlotHandle )
	{
		mUnLockAccess(g_uiSectionTableAccess);		
		pbiinfo("SFILTER_Set =>  invaild slot handle \n");
		FILTER_RETURN(kSFILTER_ERROR_SLOT_DISABLE);
	}
	if( INVAILD_HANDLE == g_pptSFIL_FilterInfoTable[iFilter]->filterHandle )
	{
		mUnLockAccess(g_uiSectionTableAccess);		
		pbiinfo("SFILTER_Set =>  invaild filter handle \n");
		FILTER_RETURN(kSFILTER_ERROR_FILTER_DISABLE);
	}
	if( uiFilterDepth > MAX_FILTER_DEPTH )
	{
		mUnLockAccess(g_uiSectionTableAccess);		
		pbiinfo("Filter depth is bigger than MAX_FILTER_DEPTH \n");
		FILTER_RETURN(kSFILTER_ERROR_NORMAL);
	}
	for(ii =0;ii<g_pptSFIL_SlotInfoTable[iSlot]->ucNumAssociateFilter;ii++)
	{
		if(g_pptSFIL_SlotInfoTable[iSlot]->iFilterID[ii] ==iFilter)
		{
			find =1;
		}
		else
		{
			continue;
		}
	}
	if(find ==0)
	{
	g_pptSFIL_SlotInfoTable[iSlot]->iFilterID[g_pptSFIL_SlotInfoTable[iSlot]->ucNumAssociateFilter] = iFilter;
	g_pptSFIL_SlotInfoTable[iSlot]->ucNumAssociateFilter++;
	}
	ErrCode = AM_DMX_SetCallback(g_pptSFIL_FilterInfoTable[iFilter]->uDemuxId, g_pptSFIL_FilterInfoTable[iFilter]->filterHandle, SFILTER_AMCallBack_Function, (void *)g_pptSFIL_SlotInfoTable[iSlot]);
	if(ErrCode != AM_SUCCESS)
	{
		mUnLockAccess(g_uiSectionTableAccess);		
		pbiinfo("AM_DMX_SetCallback error %x \n",ErrCode);
		FILTER_RETURN(ErrCode);
	}
	INVERSEMASK(pucFilterMask, uiFilterDepth);
	if(g_pptSFIL_SlotInfoTable[iSlot]->uSlotPid != INVAILD_PID)
	{
		memset(&param, 0, sizeof(param));
		param.pid = g_pptSFIL_SlotInfoTable[iSlot]->uSlotPid;	
		memcpy( param.filter.filter, pucFilterMatch, uiFilterDepth); 
		memcpy( param.filter.mask, pucFilterMask, uiFilterDepth);
		memcpy( param.filter.mode, pucFilterNegate, uiFilterDepth );
		param.flags = DMX_CHECK_CRC;
		FILTERDEBUG(pucFilterMatch, pucFilterMask, uiFilterDepth);
		ErrCode = AM_DMX_SetSecFilter(g_pptSFIL_FilterInfoTable[iFilter]->uDemuxId, g_pptSFIL_FilterInfoTable[iFilter]->filterHandle, &param);
		if( ErrCode != AM_SUCCESS )
		{
			mUnLockAccess(g_uiSectionTableAccess);		
			pbiinfo( "Filter attach slot error %x \n", ErrCode);
			FILTER_RETURN(kSFILTER_ERROR_FILTER_NOMATCH_SLOT);
		}
	}
	g_pptSFIL_FilterInfoTable[iFilter]->uiFilterDepth = uiFilterDepth;
	g_pptSFIL_FilterInfoTable[iFilter]->eFilterStatus = eSFILTER_FILTER_SET_DATA;
	memset( g_pptSFIL_FilterInfoTable[iFilter]->ucFilterMatch, DEFAULT_MATCH, MAX_FILTER_DEPTH );
	memset( g_pptSFIL_FilterInfoTable[iFilter]->ucFilterMask, DEFAULT_MASK, MAX_FILTER_DEPTH );
	memset( g_pptSFIL_FilterInfoTable[iFilter]->ucFilterNegate, DEFAULT_NEGATE, MAX_FILTER_DEPTH );
	memcpy( g_pptSFIL_FilterInfoTable[iFilter]->ucFilterMatch, pucFilterMatch, uiFilterDepth); 
	memcpy( g_pptSFIL_FilterInfoTable[iFilter]->ucFilterMask, pucFilterMask, uiFilterDepth);
	memcpy( g_pptSFIL_FilterInfoTable[iFilter]->ucFilterNegate, pucFilterNegate, uiFilterDepth );
	g_pptSFIL_FilterInfoTable[iFilter]->SlotId = iSlot;	
	g_pptSFIL_FilterInfoTable[iFilter]->eFilterStatus = eSFILTER_FILTER_SET_DATA;
	
	mUnLockAccess(g_uiSectionTableAccess);
	FILTER_RETURN(kSFILTER_NO_ERROR);
}
	
/******************************************************************************
* Function :SFILTER_Control_Filter
* Parameters : 
*                    tSFILTER_FilterId iFilterID, tSFILTER_Ctrl eFilterAttr
* Return :
*             tSFILTER_SlotId slotid
* Description :
*                   The function is used to free filter.
* Author : dszheng     2010/06/17
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
S8 SFILTER_Control_Filter( tSFILTER_FilterId iFilterID, tSFILTER_Ctrl eFilterAttr )
{
	S32				iSlotID = INVAILD_ID;
	AM_ErrorCode_t				ErrCode = AM_SUCCESS;

	if(iFilterID >= NO_OF_SIMULTANEOUS_FILTERS || iFilterID < 0) 
	{
		printf( "SFILTER_Control => invalid Filter id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_FILTER_ID);
	}

	if(( NULL == g_pptSFIL_FilterInfoTable[iFilterID] ) || ( iFilterID != g_pptSFIL_FilterInfoTable[iFilterID]->filterId ))
	{
		printf("SFILTER_Control => invalid Filter id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_FILTER_ID);
	}
	
	if( INVAILD_HANDLE == g_pptSFIL_FilterInfoTable[iFilterID]->filterHandle )
	{
		printf("SFILTER_Control =>  invaild filter handle \n");
		FILTER_RETURN(kSFILTER_ERROR_FILTER_DISABLE);
	}

	iSlotID = g_pptSFIL_FilterInfoTable[iFilterID]->SlotId;

	if( iSlotID >= NO_OF_SIMULTANEOUS_CHANNELS || iSlotID < 0)
	{
		printf( "SFILTER_Control => invalid Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}
	
	if(( NULL == g_pptSFIL_SlotInfoTable[iSlotID] ) || ( iSlotID != g_pptSFIL_SlotInfoTable[iSlotID]->uiSlotId ))
	{
		printf("SFILTER_Control => invalid Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}
	
	if( INVAILD_HANDLE == g_pptSFIL_SlotInfoTable[iSlotID]->uSlotHandle )
	{
		printf("SFILTER_Control =>  invaild slot handle \n");
		FILTER_RETURN(kSFILTER_ERROR_SLOT_DISABLE);
	}
		
	/*  lock the table */       
	mLockAccess(g_uiSectionTableAccess);

	if( eFilterAttr == eSFILTER_CTRL_DISABLE )
	{
		if( g_pptSFIL_FilterInfoTable[iFilterID]->eFilterStatus  == eSFILTER_FILTER_ATTACH_SLOT )
		{
			if(g_pptSFIL_SlotInfoTable[iSlotID]->eSlotStatus == eSFILTER_SLOT_OPEN)
			{
				ErrCode = AM_DMX_StopFilter(g_pptSFIL_FilterInfoTable[iFilterID]->uDemuxId, g_pptSFIL_FilterInfoTable[iFilterID]->filterHandle); 
				if( ErrCode != AM_SUCCESS )
				{
					mUnLockAccess(g_uiSectionTableAccess);		
					printf("SFILTER_Control => detach filter Error %x\n",ErrCode);
					FILTER_RETURN(kSFILTER_ERROR_SLOT_DISABLE);
				}
			}
			g_pptSFIL_FilterInfoTable[iFilterID]->eFilterStatus = eSFILTER_FILTER_NOTATTACH_SLOT;
			g_pptSFIL_SlotInfoTable[iSlotID]->ucNumAssociateFilter--;
		}
	}
	else if( eFilterAttr == eSFILTER_CTRL_ENABLE )
	{
		if( g_pptSFIL_FilterInfoTable[iFilterID]->eFilterStatus == eSFILTER_FILTER_NOTATTACH_SLOT )
		{
			if(g_pptSFIL_SlotInfoTable[iSlotID]->eSlotStatus != eSFILTER_SLOT_OPEN)
			{
				ErrCode = AM_DMX_StartFilter(g_pptSFIL_FilterInfoTable[iFilterID]->uDemuxId, g_pptSFIL_FilterInfoTable[iFilterID]->filterHandle); 
				if( ErrCode != AM_SUCCESS )
				{
					mUnLockAccess(g_uiSectionTableAccess);		
					printf("SFILTER_Control => attach filter Error %x\n",ErrCode);
					FILTER_RETURN(kSFILTER_ERROR_SLOT_DISABLE);
				}
			}
			g_pptSFIL_FilterInfoTable[iFilterID]->eFilterStatus = eSFILTER_FILTER_ATTACH_SLOT;
			g_pptSFIL_SlotInfoTable[iSlotID]->ucNumAssociateFilter++;
		}
	}
	else
	{
		mUnLockAccess(g_uiSectionTableAccess);		
		printf("SFILTER_Control => wrong filter attribute type\n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_COMMAND);
	}
	
	mUnLockAccess(g_uiSectionTableAccess);	

	FILTER_RETURN(kSFILTER_NO_ERROR);
}
/******************************************************************************
* Function :SFILTER_Free_Filter
* Parameters : 
*                    tSFILTER_FilterId iFilterID
* Return :
*             tSFILTER_SlotId slotid
* Description :
*                   The function is used to free filter.
* Author : dszheng     2010/06/17
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
S8 SFILTER_Free_Filter( tSFILTER_FilterId iFilterID )
{
	S32				iSlotID = INVAILD_ID;
	AM_ErrorCode_t	ErrCode = AM_SUCCESS;
	if(iFilterID >= NO_OF_SIMULTANEOUS_FILTERS || iFilterID < 0) 
	{
		printf( "SFILTER_Free => invalid Filter id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_FILTER_ID);
	}

	if(( NULL == g_pptSFIL_FilterInfoTable[iFilterID] ) || ( iFilterID != g_pptSFIL_FilterInfoTable[iFilterID]->filterId ))
	{
		printf("SFILTER_Free => invalid Filter id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_FILTER_ID);
	}

	if( INVAILD_HANDLE == g_pptSFIL_FilterInfoTable[iFilterID]->filterHandle )
	{
		printf("SFILTER_Free =>  invaild filter handle \n");
		FILTER_RETURN(kSFILTER_ERROR_FILTER_DISABLE);
	}

	iSlotID = g_pptSFIL_FilterInfoTable[iFilterID]->SlotId;


	/*  lock the table */       
	mLockAccess(g_uiSectionTableAccess);
	if(( g_pptSFIL_FilterInfoTable[iFilterID]->eFilterStatus  == eSFILTER_FILTER_ATTACH_SLOT )&&( iSlotID < NO_OF_SIMULTANEOUS_CHANNELS && iSlotID >= 0))
	{
		ErrCode = AM_DMX_StopFilter(g_pptSFIL_FilterInfoTable[iFilterID]->uDemuxId, g_pptSFIL_FilterInfoTable[iFilterID]->filterHandle); 
		if( ErrCode != AM_SUCCESS )
		{
			mUnLockAccess(g_uiSectionTableAccess);		
			printf("SFILTER_Control => detach filter Error %x\n",ErrCode);
			FILTER_RETURN(kSFILTER_ERROR_SLOT_DISABLE);
		}
	}
	ErrCode = AM_DMX_FreeFilter(g_pptSFIL_FilterInfoTable[iFilterID]->uDemuxId, g_pptSFIL_FilterInfoTable[iFilterID]->filterHandle);
	if( ErrCode != AM_SUCCESS )
	{
		mUnLockAccess(g_uiSectionTableAccess);		
		printf("\nFree filter Error %d\n",ErrCode);
		FILTER_RETURN(kSFILTER_ERROR_NORMAL);
	}
	OS_MemDeallocate( system_p, g_pptSFIL_FilterInfoTable[iFilterID] );
	g_pptSFIL_FilterInfoTable[iFilterID] = NULL;
	if(( iSlotID <= NO_OF_SIMULTANEOUS_CHANNELS) &&(iSlotID > 0)&&( NULL != g_pptSFIL_SlotInfoTable[iSlotID] ))
	{
		g_pptSFIL_SlotInfoTable[iSlotID]->ucNumAssociateFilter--;
	}

	mUnLockAccess(g_uiSectionTableAccess);	

	FILTER_RETURN(kSFILTER_NO_ERROR);
}
/******************************************************************************
* Function :SFILTER_Get_FilterAttach_SlotID
* Parameters : 
*                    tSFILTER_FilterId iFilterID
* Return :
*             uiSlotID
*             kSFILTER_ERROR_INVALID_FILTER_ID
*             kSFILTER_ERROR_INVALID_SLOT_ID
*             kSFILTER_ERROR_FILTER_DISABLE
* Description :
*                   The function is used to get the filter number attaching to a slot.
* Author : dszheng     2010/06/17
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/

tSFILTER_SlotId SFILTER_Get_FilterAttach_SlotID( tSFILTER_FilterId iFilterID )
{
	S32 iSlotID = INVAILD_ID;

	if(iFilterID >= NO_OF_SIMULTANEOUS_FILTERS || iFilterID < 0) 
	{
		printf( "SFILTER_Set => invalid Filter id '''\n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_FILTER_ID);
	}

	mLockAccess(g_uiSectionTableAccess);

	if(( NULL == g_pptSFIL_FilterInfoTable[iFilterID] ) || ( iFilterID != g_pptSFIL_FilterInfoTable[iFilterID]->filterId ))
	{
		mUnLockAccess(g_uiSectionTableAccess);	
		printf("SFILTER_Set => invalid Filter id ]]\n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_FILTER_ID);
	}

	if( INVAILD_HANDLE == g_pptSFIL_FilterInfoTable[iFilterID]->filterHandle )
	{
		mUnLockAccess(g_uiSectionTableAccess);		
		printf("SFILTER_Set =>  invaild filter handle \n");
		FILTER_RETURN(kSFILTER_ERROR_FILTER_DISABLE);
	}

	iSlotID = g_pptSFIL_FilterInfoTable[iFilterID]->SlotId;

	if( iSlotID >= NO_OF_SIMULTANEOUS_CHANNELS || iSlotID < 0)
	{
		mUnLockAccess(g_uiSectionTableAccess);	
		printf( "SFILTER_Set => invalid Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}

	if(( NULL == g_pptSFIL_SlotInfoTable[iSlotID] ) || ( iSlotID != g_pptSFIL_SlotInfoTable[iSlotID]->uiSlotId ))
	{
		mUnLockAccess(g_uiSectionTableAccess);	
		printf("SFILTER_Set => invalid Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}

	if( INVAILD_HANDLE == g_pptSFIL_SlotInfoTable[iSlotID]->uSlotHandle )
	{
		mUnLockAccess(g_uiSectionTableAccess);		
		printf("SFILTER_Set =>  invaild slot handle \n");
		FILTER_RETURN(kSFILTER_ERROR_SLOT_DISABLE);
	}

	mUnLockAccess(g_uiSectionTableAccess);	
	
	FILTER_RETURN(iSlotID);

}

/******************************************************************************
* Function :SFILTER_Check_DataPID
* Parameters : 
*                    U32 uiPid
* Return :
*             NumFilter
*             kSFILTER_ERROR_INVALID_SLOT_ID
* Description :
*                   The function is used to get the filter number attaching to a slot.
* Author : dszheng     2010/06/17
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/

tSFILTER_SlotId SFILTER_Check_DataPID( U32 uiPid,U8 demuxid )
{
	S32 iSlotIndex = 0;

	mLockAccess(g_uiSectionTableAccess);

	while( iSlotIndex < NO_OF_SIMULTANEOUS_CHANNELS )
	{
		
		if(( NULL != g_pptSFIL_SlotInfoTable[iSlotIndex] )&&( g_pptSFIL_SlotInfoTable[iSlotIndex]->eSlotStatus != eSFILTER_SLOT_NOT_USE)
			&&(g_pptSFIL_SlotInfoTable[iSlotIndex]->uSlotPid == uiPid)
			&&(g_pptSFIL_SlotInfoTable[iSlotIndex]->uDemuxId== demuxid))//liqian
		{
			break;
		}
		iSlotIndex++;
	}

	if(iSlotIndex >= NO_OF_SIMULTANEOUS_CHANNELS)
	{
		mUnLockAccess(g_uiSectionTableAccess);
	
		FILTER_RETURN(kSFILTER_ERROR_NO_MATCHED_SLOT);
	}

	mUnLockAccess(g_uiSectionTableAccess);
	
	FILTER_RETURN( iSlotIndex );
}


/******************************************************************************
* Function :SFILTER_NumFilterAssociate_Slot
* Parameters : 
*                    tSFILTER_SlotId iSlotID
* Return :
*             NumFilter
*             kSFILTER_ERROR_INVALID_SLOT_ID
* Description :
*                   The function is used to get the filter number attaching to a slot.
* Author : dszheng     2010/06/17
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/

S8 SFILTER_NumFilterAssociate_Slot( tSFILTER_SlotId iSlotID )
{
	U8 ucNumFilter = 0;
	
	if( iSlotID >= NO_OF_SIMULTANEOUS_CHANNELS || iSlotID < 0 )
	{
		printf( "SFILTER_Control_Channel => invalid Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}
	
	mLockAccess(g_uiSectionTableAccess); 
	
	if(NULL != g_pptSFIL_SlotInfoTable[iSlotID])
	{
		ucNumFilter = g_pptSFIL_SlotInfoTable[iSlotID]->ucNumAssociateFilter;
		mUnLockAccess(g_uiSectionTableAccess);
		FILTER_RETURN( ucNumFilter );
	}
	else
	{
		mUnLockAccess(g_uiSectionTableAccess);
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}
}

/******************************************************************************
* Function :SFILTER_Open_Channel
* Parameters : 
*                    tSFILTER_SlotId iSlot
* Return :
*             kSFILTER_NO_ERROR
*             kSFILTER_ERROR_NORMAL
* Description :
*                   The function is used to open channel.
* Author : dszheng     2010/06/17
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/

S8 SFILTER_Open_Channel( tSFILTER_SlotId iSlot )
{
	AM_ErrorCode_t		ErrCode = AM_SUCCESS;
	tSFILTER_FilterId iFilterID;
	S8 FilterErr = kSFILTER_NO_ERROR;
	if(iSlot >= NO_OF_SIMULTANEOUS_CHANNELS || iSlot < 0)
	{
		pbierror( "SFILTER_Control_Channel => invalid Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}
	
	mLockAccess(g_uiSectionTableAccess);

	if(( NULL == g_pptSFIL_SlotInfoTable[iSlot] ) || ( iSlot != g_pptSFIL_SlotInfoTable[iSlot]->uiSlotId ))
	{
		mUnLockAccess(g_uiSectionTableAccess);	
		pbierror("SFILTER_Set => invalid Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}

	if( INVAILD_HANDLE == g_pptSFIL_SlotInfoTable[iSlot]->uSlotHandle )
	{
		mUnLockAccess(g_uiSectionTableAccess);		
		pbierror("SFILTER_Set =>  invaild slot handle \n");
		FILTER_RETURN(kSFILTER_ERROR_SLOT_DISABLE);
	}
	
	if( g_pptSFIL_SlotInfoTable[iSlot]->eSlotStatus != eSFILTER_SLOT_OPEN )
	{
		int i = 0, ucNumAssociateFilter = 0;
		ucNumAssociateFilter = g_pptSFIL_SlotInfoTable[iSlot]->ucNumAssociateFilter;
		for(i = 0; i < ucNumAssociateFilter; i++)
		{
			iFilterID = g_pptSFIL_SlotInfoTable[iSlot]->iFilterID[i];
			FilterErr = SFILTER_CheakFilterID(iFilterID);
			if(FilterErr != kSFILTER_NO_ERROR || g_pptSFIL_FilterInfoTable[iFilterID]->eFilterStatus < eSFILTER_FILTER_SET_DATA)
			{
				mUnLockAccess(g_uiSectionTableAccess);		
				pbierror("SFILTER_CheakFilterID  error %x line %d\n",FilterErr,__LINE__);
				FILTER_RETURN(FilterErr);
			}
			ErrCode = AM_DMX_StartFilter(g_pptSFIL_FilterInfoTable[iFilterID]->uDemuxId, g_pptSFIL_FilterInfoTable[iFilterID]->filterHandle); 
			if( ErrCode != AM_SUCCESS )
			{
				mUnLockAccess(g_uiSectionTableAccess);		
				pbierror("SFILTER_Control => AM_DMX_StartFilter Error %x\n",ErrCode);
				FILTER_RETURN(kSFILTER_ERROR_FILTER_NOMATCH_SLOT);
			}
			g_pptSFIL_FilterInfoTable[iFilterID]->eFilterStatus = eSFILTER_FILTER_ATTACH_SLOT;
		}
		g_pptSFIL_SlotInfoTable[iSlot]->eSlotStatus = eSFILTER_SLOT_OPEN;
		mUnLockAccess(g_uiSectionTableAccess);
		FILTER_RETURN(kSFILTER_NO_ERROR);
	}
	else
	{
		mUnLockAccess(g_uiSectionTableAccess);
		FILTER_RETURN(kSFILTER_NO_ERROR);
	}
}

/******************************************************************************
* Function :SFILTER_Close_Channel
* Parameters : 
*                    tSFILTER_SlotId iSlot
* Return :
*             kSFILTER_NO_ERROR
*             kSFILTER_ERROR_NORMAL
*             kSFILTER_ERROR_SLOT_DISABLE
*             kSFILTER_ERROR_INVALID_SLOT_ID
* Description :
*                   The function is used to close channel.
* Author : dszheng     2010/06/17
* --------------------
* Modification History:
*
* --------------------
******************************************************************************/
	
S8 SFILTER_Close_Channel( tSFILTER_SlotId iSlot )
{
	AM_ErrorCode_t		ErrCode = AM_SUCCESS;
	tSFILTER_FilterId iFilterID;
	S8 FilterErr = kSFILTER_NO_ERROR;
	if(iSlot >= NO_OF_SIMULTANEOUS_CHANNELS || iSlot < 0)
	{
		pbierror("SFILTER_Control_Channel => invalid Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}
	/*  lock the table */
	mLockAccess(g_uiSectionTableAccess);
	if(( NULL == g_pptSFIL_SlotInfoTable[iSlot] ) || ( iSlot != g_pptSFIL_SlotInfoTable[iSlot]->uiSlotId ))
	{
		mUnLockAccess(g_uiSectionTableAccess);	
		pbierror("SFILTER_Set => invalid Slot id \n");
		FILTER_RETURN(kSFILTER_ERROR_INVALID_SLOT_ID);
	}

	if( INVAILD_HANDLE == g_pptSFIL_SlotInfoTable[iSlot]->uSlotHandle )
	{
		mUnLockAccess(g_uiSectionTableAccess);		
		pbierror("SFILTER_Set =>  invaild slot handle \n");
		FILTER_RETURN(kSFILTER_ERROR_SLOT_DISABLE);
	}
	if( g_pptSFIL_SlotInfoTable[iSlot]->eSlotStatus != eSFILTER_SLOT_CLOSE )
	{
		int i = 0, ucNumAssociateFilter = 0;
		ucNumAssociateFilter = g_pptSFIL_SlotInfoTable[iSlot]->ucNumAssociateFilter;
		for(i = 0; i < ucNumAssociateFilter; i++)
		{
			iFilterID = g_pptSFIL_SlotInfoTable[iSlot]->iFilterID[i];
			FilterErr = SFILTER_CheakFilterID(iFilterID);
			if(FilterErr != kSFILTER_NO_ERROR || g_pptSFIL_FilterInfoTable[iFilterID]->eFilterStatus < eSFILTER_FILTER_ATTACH_SLOT)
			{
				
				mUnLockAccess(g_uiSectionTableAccess);		
				pbierror("SFILTER_CheakFilterID  error %x line %d\n",FilterErr,__LINE__);
				
				FILTER_RETURN(FilterErr);
			}		
			ErrCode = AM_DMX_StopFilter(g_pptSFIL_FilterInfoTable[iFilterID]->uDemuxId, g_pptSFIL_FilterInfoTable[iFilterID]->filterHandle); 
			if( ErrCode != AM_SUCCESS )
			{
				mUnLockAccess(g_uiSectionTableAccess);		
				pbierror("SFILTER_Control => AM_DMX_StopFilter Error %x\n",ErrCode);
				FILTER_RETURN(kSFILTER_ERROR_SLOT_DISABLE);
			}
			g_pptSFIL_FilterInfoTable[iFilterID]->eFilterStatus = eSFILTER_FILTER_NOTATTACH_SLOT;
		}
	//	g_pptSFIL_SlotInfoTable[iSlot]->uSlotPid = INVAILD_PID;
		g_pptSFIL_SlotInfoTable[iSlot]->eSlotStatus = eSFILTER_SLOT_CLOSE;
		mUnLockAccess(g_uiSectionTableAccess);
		FILTER_RETURN(kSFILTER_NO_ERROR);
	}
	else
	{
		mUnLockAccess(g_uiSectionTableAccess);
		FILTER_RETURN(kSFILTER_NO_ERROR);
	}
}

S8 SFILTER_Control_Channel (tSFILTER_SlotId Slot, tSFILTER_Ctrl Ctrl)
{
    /* enable channel */
    if (eSFILTER_CTRL_ENABLE == Ctrl)
    {
		return SFILTER_Open_Channel( Slot);
    }
    /* disable channel */
    else if((eSFILTER_CTRL_DISABLE == Ctrl)||(eSFILTER_CTRL_RESET == Ctrl))
    {
		return SFILTER_Close_Channel( Slot);
    }
    return kSFILTER_NO_ERROR;
}

/*---------------------------------------------------------------*/
S32 SetDemuxDVB(int src)
{
	AM_ErrorCode_t 		ErrCode = AM_SUCCESS;
	ErrCode = AM_DMX_SetSource(DEMUX_DRIVER_NUM, src);
	if(ErrCode!=AM_SUCCESS)
	{
		pbiinfo("[SetDemuxDVB] AM_DMX_SetSource error %d, src %d\n",ErrCode, src);
		return -1;
	}	
	return 0;
}

S32 SetDemuxSourceByPvr(int src)
{
	AM_ErrorCode_t 		ErrCode = AM_SUCCESS;
	ErrCode = AM_DMX_SetSource(DEMUX_PVR_DRIVER_NUM, src);
	if(ErrCode!=AM_SUCCESS)
	{
		pbiinfo("[SetDemuxSourceByPvr] AM_DMX_SetSource error 0x%x\n",ErrCode);
		return ErrCode;
	}	
	return 0;
}

S32 SetDemuxP2S(void)
{
	AM_DMX_OpenPara_t 	para;
	AM_ErrorCode_t 		ErrCode = AM_SUCCESS;
	
	memset(&para, 0, sizeof(para));	

 	ErrCode=(AM_ErrorCode_t)Amlogic_av_player_init();
	if(ErrCode != 0)
	 {
		 pbierror("[SetDemuxP2S] call Amlogic_av_player_init failed.\n");
		 return ErrCode;
	 }

	ErrCode = AM_DMX_SetSource(DEMUX_DAL_DRIVER_NUM, AM_DMX_SRC_HIU);
	if(ErrCode!=AM_SUCCESS)
	{
		pbiinfo("[SetDemuxP2S] AM_DMX_SetSource error %d\n",ErrCode);
		return -1;
	}
	
   // AM_DMX_Sync(0);
    return 0;

}

