
/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : drv_filter.h
* Description : Includes all other FILTER driver header files.
              	This allows a FILTER application to include only this one header
              	file directly. 
* Author : zheng dong sheng
* History :
*	2010/06/17 : Initial Version
******************************************************************************/
#ifndef _SFILTER_H_
#define _SFILTER_H_
#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************/
/*              Includes                                                                   */
/*******************************************************/
#include "pvddefs.h"
#include "am_dmx.h"
/*******************************************************/
/*              Exported Defines and Macros                                     */
/*******************************************************/

#define DEMUX_0 0
#define DEMUX_1 1
#define DEMUX_2 2

#define TS_PORT     2


#define AMLOGIC_DEMUX_EPG      

/*******************************************************/
/*              Exported Types			                                        */
/*******************************************************/
/*Definitions of type of IDs for Slots */
typedef   int tSFILTER_SlotId;

/*Definitions of type of IDs for Filters*/
typedef   int tSFILTER_FilterId;

typedef enum
{
	eSFILTER_CTRL_RESET,/**< Reset*/
	eSFILTER_CTRL_ENABLE,/**< Enable*/
	eSFILTER_CTRL_DISABLE/**< Disable*/
}tSFILTER_Ctrl;

typedef enum
{
	eSFILTER_SLOT_NOT_USE = 0x00,
	eSFILTER_SLOT_ALLOCATE = 0x01,
	eSFILTER_SLOT_SET_PID = 0x02,
	eSFILTER_SLOT_ATTACH_FILTER = 0x04,
	eSFILTER_SLOT_OPEN = 0x08,
	eSFILTER_SLOT_CLOSE = 0x10
}tSFILTER_SlotStatus;

typedef enum
{
	eSFILTER_FILTER_NOT_USE = 0x00,
	eSFILTER_FILTER_ALLOCATE = 0x01,
	eSFILTER_FILTER_SET_DATA = 0x02,
	eSFILTER_FILTER_ATTACH_SLOT = 0x04,
	eSFILTER_FILTER_NOTATTACH_SLOT = 0x08
}tSFILTER_FilterStatus;

typedef  unsigned char *(*tSFILTER_GetBufferFct) (U32 uiSlotId);
	
typedef  void (*tSFILTER_FreeBufferFct) (U32 uiSlotId, void *ptr);

/*callback function called when a new section filter is requested */	
typedef  void(*tSFILTER_SlotCallbackFct) ( tSFILTER_SlotId					uiSlot,
											tSFILTER_FilterId					uiFilter,
											tSFILTER_FreeBufferFct	pFFree_Func,
											U8					*pucBuffer,
											U32					uiLength,
											U16					usPid );

/*callback struct assigned when be registered with a slot.*/
typedef struct
{
	tSFILTER_GetBufferFct		pFGetBuffer;
	tSFILTER_FreeBufferFct		pFFreeBuffer;
	tSFILTER_SlotCallbackFct	pFCallBack;
}SFilterCallback_t;
/*******************************************************/
/*              Exported Variables		                                        */
/*******************************************************/

/*******************************************************/
/*              External				                                        */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                                        */
/*******************************************************/
S8 SFILTER_Init ( void );
S8 SFILTER_Reset();
S8 SFILTER_Terminate (void);
tSFILTER_SlotId SFILTER_Allocate_Channel ( U8		uDemuxId );
S8 SFILTER_Free_Channel( tSFILTER_SlotId uiSlotID );
tSFILTER_FilterId SFILTER_Allocate_Filter  ( U8		uDemuxId ) ;
S8 SFILTER_SetPid (tSFILTER_SlotId uiSlot, U32 uiPid);
S8 SFILTER_RegisterCallBack_Function (tSFILTER_SlotId uiSlotID, SFilterCallback_t *ptCallbacks);
S8 SFILTER_SetFilter (tSFILTER_SlotId		uiSlot,
						tSFILTER_FilterId	uiFilter,	
						U32					uiFilterDepth,
						U8					*pucFilterMask,
						U8					*pucFilterMatch,
						U8					*pucFilterNegate);
S8 SFILTER_SetFilter_NOATTACH( tSFILTER_SlotId		iSlot,
						tSFILTER_FilterId		iFilter,	
						U32					uiFilterDepth,
						U8					*pucFilterMask,
						U8					*pucFilterMatch,
						U8					*pucFilterNegate);
S8 SFILTER_Free_Filter( tSFILTER_FilterId uiFilterID );
tSFILTER_SlotId SFILTER_Get_FilterAttach_SlotID( tSFILTER_FilterId uiFilterID );
tSFILTER_SlotId SFILTER_Check_DataPID( U32 uiPid,U8 demuxid );
S8 SFILTER_NumFilterAssociate_Slot( tSFILTER_SlotId uiSlotID );
S8 SFILTER_Open_Channel( tSFILTER_SlotId uiSlot );
S8 SFILTER_Close_Channel( tSFILTER_SlotId uiSlot );

S8 SFILTER_Control_Filter( tSFILTER_FilterId iFilterID, tSFILTER_Ctrl eFilterAttr );
S8 SFILTER_Control_Channel (tSFILTER_SlotId Slot, tSFILTER_Ctrl Ctrl);

void Filter_test(void);


typedef S32 (* PvrFilter_Callback_pfn)( U8 *pData, U16 Len, void *param);

extern S8 SFILTER_PVR_AddFilter( U32 u32DmxId, U32 u32ChannelHandle, U32 u32TimeOutMs, U8 u8TableId, U16 u16ServId, U16 u16PmtPid, PvrFilter_Callback_pfn pCalback, void *pParam );
extern S8 SFILTER_PVR_AddPid(U32 u32DmxId, S32 pid, S32 s32ChannelType, U32 *ChannelHandle );
extern S8 SFILTER_PVR_DelPid(U32 u32DmxId, U32 *pChannelHandle );
extern S8 SFILTER_PVR_Player_UnInit( U32 *pBuffer);
extern S8 SFILTER_PVR_Player_Init( U32 *pBuffer );
extern S8 SFILTER_PVR_UnInit();
extern S8 SFILTER_PVR_Init();

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __DRV_FILTER_H */
/* End of drv_filter.h  --------------------------------------------------------- */

