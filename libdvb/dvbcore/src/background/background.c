/*****************************************************************************
*		(c)	Copyright Pro. Broadband Inc. PVware
*
* File name : 		BackGround.c
* Description : 	Support the Data Update
* 	History:
* Date			Modification				      Name
*----------         ------------        			----------
*2008/04/03         Created                                     pxwang
 ******************************************************************************/

/********************************************************/
/*							Includes									*/
/********************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "pvddefs.h"
//#include "drv_ge.h"

#include "osapi.h"
#include "drv_filter.h"

#include "background.h"
#include "pbitrace.h"
#include "dvbplayer.h"
/*******************************************************/
/*		Private Defines and Macros						*/
/*******************************************************/

#define MAX_SECTION_NUM         (256)
#define MAX_SECTION_LENGTH      (4096)

#define	BG_DEBUG					0

#if BG_DEBUG
	#define DB_BG(__statement__)  TRC_DBMSG(__statement__)
	TRC_DBMSG_SET_MODULE(SYS);
#else
	#define DB_BG(__statement__)
#endif

#define   BgOpLock(Back_Type)			{\
                                           OS_SemWait( bg_SemInUse[Back_Type]);	\
                                         /*  printf("BgOpLock [%d] LINE %d\n", Back_Type, __LINE__);*/\
                                        }//����BackGround���ź������� 
#define   BgOpUnLock(Back_Type)			{\
                                           OS_SemSignal( bg_SemInUse[Back_Type]);\
                                        /*   printf("BgOpUnLock [%d] LINE %d\n", Back_Type, __LINE__);*/\
                                        }//����BackGround���ź�������
/*LSZ 2010/04/16�޸��ź���û�г�ʼ������*/
/**********************************************************************/
/*							Private Constants						*/
/**********************************************************************/
#define	BG_MAX_FILTER					8		//֧�ֵ�BackGround����
#define	BG_SLOT_PAUSE					2		//BackGround��״̬BackGround�Ѿ�Start�����Ѿ�Pause
#define	BG_SLOT_USE					1		//BackGround��״̬BackGround�Ѿ�Start
#define	BG_SLOT_IDLE					0		//BackGround��״̬BackGround�Ѿ�Stop
#define	INVALID_DEMUX_SOCKETID		-1		//��Slot�ĳ�ʼֵ
#define	FILTER_DEPTH_SIZE                       16
#define NO_OF_SIMULTANEOUS_CHANNELS			30
#define NO_OF_SIMULTANEOUS_FILTERS			48

 /**********************************************************************/
 /*							Private Types									*/
 /**********************************************************************/
typedef struct
{
	U8 						status; 				//��ʶBackGround��״̬ 
	tSFILTER_FilterId 			filterID;				//��ʶBackGround��FilterID
	tSFILTER_SlotId 			slotID;				//��ʶBackGround��SlotID
	U32						filterPID;			//����BackGroundҪ������ݵ�PID
	U32 						tableID;				//����BackGroundҪ������ݵ�TableID
	SYS_BG_Type_e 			bg_type;			//��ʶBackGround������
	U32 						semaphore;			//ע���BackGround���ź����������������?
	U32 						first_section;		//��ʶ��ǰ���µ��Ƿ���FirstSection
	U32						section_version;		//��ʶ��ǰBackGround��Section��
	table_notify_fct_t			callback_fun;		//ע����BackGround���ϲ�CallBack
	U32						sections_arrived[MAX_SECTION_NUM/32];
	U32						all_sections_mask[MAX_SECTION_NUM/32];
}BG_SetFilter_t;

typedef enum
{
	BAT_UPDATING = 0,
	BAT_UPDATE_COMPLETE = 1,
	BAT_UPDATE_NONE = 2
} BAT_UPDATE_TYPE;
 /**********************************************************************/
 /*							Global Variables								*/
 /**********************************************************************/
 extern void CA_PostChanChangeState(U16 wChannelId, U8 bState);
extern S16 CA_PostPMTTable(void *pvTableData);
extern S16 CA_PostCATTable(void *pvTableData);
 /**********************************************************************/
 /*							Private Variables (static)						*/
 /**********************************************************************/
static BG_SetFilter_t			bg_SetFilterData[BG_MAX_FILTER];	//BackGround���еĲ���
static U32						bg_SemInUse[BG_MAX_FILTER]; 	//BackGround���ź���
static U8						Boot_FirstNIT;					//��Nit���ñ�ʶ
static SYS_BG_TPParam_t			g_tp_parameters={0,0,0,0,0,0};	//TP��Ϣ
//static SYS_Table_InitParam_t	pmt_zapping_param;				//����ԭ��PMT��Zapping&Message֮�� ��ʱ�ṩ��
static U32						bat_check[65536];				//��BackGround���м���Ӧ��BrouqetID��Version

#if BG_DEBUG
static U32 						TestCount = 0, DataFlag[BG_MAX_FILTER] = {0};
static struct timeval                 	time_start, time_end;
#endif

tSFILTER_SlotId g_pmt_slot_id = 0;
static U16   UTI_pid =0x1fff;//liqian
 static U16 nit_version=0xffff;
 U32 g_semNIT_CallBack = 0xFFFFFFFF;
static STB_NITMsg_Notify g_nitver_update_cb =NULL;
static STB_NITMsg_Notify g_nit_download_cb =NULL;
 static U8	 g_cat_buffer[4096];
 static U8 g_nit_buffer[4096];
static U16  g_cat_len=0;
static U16 g_nit_len = 0;
    static U8 dl_flag = 0;

 /**********************************************************************/
 /*							Private Function prototypes						*/
 /**********************************************************************/
void _bg_SetFilterDataToDef(U32  Back_Type);
static U32 _bg_closeFilter(BG_SetFilter_t bgfilterdata);
static U8 *_bgGetSectionDataBuffer(tSFILTER_SlotId  slot);
static void _bgFreeDataBuffer(void* buf);
static void _bgFreeSectionDataBuffer(tSFILTER_SlotId  slot,void* buf);
static void _bg_FilterInfoCallBack(tSFILTER_SlotId 	slot,
											tSFILTER_FilterId	filter,
											tSFILTER_FreeBufferFct free_func,
											U8 			*buffer,
											U32 		length,
											U16			pid
		);

static U32 _bg_PAT_Table(U8 *data_buffer, U32 sectionlength,U16 pid,tSFILTER_SlotId 	slot);
static U32 _bg_NIT_Table(U8 *data_buffer, U32 sectionlength,U16 pid,tSFILTER_SlotId 	slot);
static U32 _bg_SDT_Table(U8 *data_buffer, U32 sectionlength,U16 pid,tSFILTER_SlotId  slot);
//static U32 _bg_BAT_Table(U8 *data_buffer, U32 sectionlength,U16 pid,tSFILTER_SlotId 	slot);
static U32 _bg_CAT_Table(U8 *data_buffer, U32 sectionlength,U16 pid,tSFILTER_SlotId  slot);
static U32 _bg_TDT_Table(U8 *data_buffer, U32 sectionlength,U16 pid,tSFILTER_SlotId 	slot);
static U32 _bg_EIT_Table(U8 *data_buffer, U32 sectionlength,U16 pid,tSFILTER_SlotId 	slot);
static U32 _bg_PMT_Table(U8 *data_buffer, U32 sectionlength,U16 pid,tSFILTER_SlotId  slot);
static U8 _bg_check_pat_info(SYS_BG_TPParam_t param_infor, U8 *pbuffer); 
static U8  _bg_check_nit_info(SYS_BG_TPParam_t param_infor, U8 *pbuffer,U16 section_length);
static U8 _bg_check_sdt_info(SYS_BG_TPParam_t param_infor, U8 *pbuffer);
//static U8 _bg_check_bat_info(U16 cur_version, U8 *pbuffer);
static SYS_BG_Type_e _bg_check_Type_Byslotid(U16 slotid);
void	_get_program_info(SYS_BG_TPParam_t *tp_parameters);

 
 /**********************************************************************/
 /*							Functions								*/
 /**********************************************************************/

/* UTI CA {{{*/
 static DEMUX_CALLBACK g_pmt_callbackfun = NULL;
 static U8 *pmt_section_buf = NULL;

 tSFILTER_SlotId Get_Pmt_SlotId( void )
  {
	 return bg_SetFilterData[SYS_BG_PMT].slotID;
  }
 
 void Register_Pmt_Callback_Fun( DEMUX_CALLBACK callback_fun ,U16 pid)
 {
	 g_pmt_callbackfun = callback_fun;
	 UTI_pid = pid;
 }

 void UnRegister_PMT_Callback_Fun()
 {
	 g_pmt_callbackfun = NULL;
	  UTI_pid =0x1fff;
 }
/* }}}  UTI CA*/


 /******************************************************************************
* Function : 		_bgGetSectionDataBuffer
* parameters :	
*				slot : The slot ID
* Return :		
*				pbuf : Malloc Address
* Description :
*				Malloc the Buffer to a slot for save the FilterData
*				�ṩ��Filter�ķ���ռ�ĺ���
*
* Author : 		pxwang	2008/03/26
******************************************************************************/
  static U8 *_bgGetSectionDataBuffer(tSFILTER_SlotId  slot)
 {
 	U8 *pbuf = NULL;
	
	pbuf = (U8 *)OS_MemAllocate(system_p,MAX_SECTION_LENGTH);/*yangweiqing modify 10-04-26*/
//	pbuf = (U8 *)small_mem_allocate(MAX_SECTION_LENGTH);
	if(!pbuf)
	{
	       pbiinfo("===BackGround:   GetSectionDataBuffer() Failed. Not enough memory\n");
		//DB_BG((TRC_CRITICAL,"===BackGround:   GetSectionDataBuffer() Failed. Not enough memory\n"));
	}
	return pbuf;
 }


 /******************************************************************************
* Function : 		_bgFreeSectionDataBuffer
* parameters :	
*				SlotId : The slot ID
*				BufferAddress : The Address to Free
* Return :	
*				(nothing)		
* Description :
*				Free the Buffer to a slot
*				�ṩ��CallBack�ĺ���Ҳ��ʵ�ʵ��ͷſռ�ĺ���?
*
* Author : 		pxwang	2008/03/26
******************************************************************************/
static void _bgFreeDataBuffer(void* buf)
{ 	
	if(!buf)
            pbiinfo("===BackGround:   FreeSectionDataBuffer() Failed. Null ptr\n");
		//DB_BG((TRC_CRITICAL,"===BackGround:   FreeSectionDataBuffer() Failed. Null ptr\n"));
	else
	{
		OS_MemDeallocate(system_p,buf);
	//	small_mem_deallocate(buf);
		buf = NULL;
	}
}
 /******************************************************************************
* Function : 		_bgFreeSectionDataBuffer
* parameters :	
*				SlotId : The slot ID
*				BufferAddress : The Address to Free
* Return :	
*				(nothing)		
* Description :
*				Free the Buffer to a slot
*				�ṩ��Filter���ͷſռ�ĺ���?
*
* Author : 		pxwang	2008/03/26
******************************************************************************/
static void _bgFreeSectionDataBuffer(tSFILTER_SlotId  slot,void* buf)
{ 	
	_bgFreeDataBuffer(buf);
}

 /******************************************************************************
* Function : 		_bg_FilterInfoCallBack
* parameters :	
*				slot : The Slot ID
*				filter : The Filter ID
*				free_func : The function of Free The buffer
*				buffer : the data buffer address
*				length : the data section length
*				pid : the Slot PID
* Return :	
*				(nothing)		
* Description :
*				Provide the Filter A CallBack for send the data to BackGround and dealwith the data
*				�ṩ��Filter��CallBack����
*				Filter���յ���ݣ�CheckCRC��ȷ�Ժ󣬷��������ݸ�BackGround
*				Ȼ��ȥ���������Table���ͣ����з�����
*
* Author : 		pxwang	2008/03/26
******************************************************************************/
static void _bg_FilterInfoCallBack(tSFILTER_SlotId 	slot,
											tSFILTER_FilterId	filter,
											tSFILTER_FreeBufferFct free_func,
											U8 			*buffer,
											U32 		length,
											U16			pid
		)
{
	SYS_BG_Type_e	iI = SYS_BG_NOTABLE;
	U16	error = SYS_TABLE_NOERROR;
	
#if BG_DEBUG
	U32 TestTableID = 0xffff, ticktest,iData= 0;
	TestTableID = buffer[0];
	TestCount++;

	memset(&time_start, 0, sizeof(time_start));
	memset(&time_end, 0, sizeof(time_end));
	if(TestCount==1)
		gettimeofday(&time_start, NULL);

	if(pid==0x00)
		DataFlag[0]++;
	else if (pid==0x10)
		DataFlag[1]++;
	else if (pid ==0x01)
		DataFlag[2]++;
	else if (pid==0x14)
		DataFlag[3]++;
	else if(pid==0x12)
		DataFlag[4]++;
	else if(pid ==0x11)
	{
		if (TestTableID==0x4a)
			DataFlag[5]++;
		else
			DataFlag[6]++;
	}
	else
		DataFlag[7]++;	
	
	gettimeofday(&time_end, NULL);
	ticktest = time_end.tv_sec-time_start.tv_sec;  //�õ�����
	if(ticktest>1)
	{
		DB_BG((TRC_INFO, "=== The pat:%d,nit:%d,sdt:%d,bat:%d,cat:%d,tdt%d,eit:%d,pmt:%d\n",
						DataFlag[0],DataFlag[1],DataFlag[6],DataFlag[5],DataFlag[2],DataFlag[3],DataFlag[4],DataFlag[7]));
		TestCount=0;
		for (iData =0 ;iData<8;iData++)
		{
			DataFlag[iData]=0;
		}
		time_start = time_end;
	}
#endif

	
	if ((length > MAX_SECTION_LENGTH) || (buffer == NULL))
	{
	        pbiinfo("===BackGround:    section length error \n");
		//DB_BG((TRC_CRITICAL,"===BackGround:    section length error \n"));
		_bgFreeDataBuffer(buffer);
		return;
	}	
	if(buffer[0]!=0x70)
	{
		if(pv_crc32(buffer,length,1)!=0)
	       {
			pbiinfo("_bg_FilterInfoCallBack  crcr eror----tablid[%x]\n",buffer[0]);
                     _bgFreeDataBuffer(buffer);
			return;
	       }
	}
//	printf("*********_bg_FilterInfoCallBack=>the slot = %d,buffer[0] = %d,the pid = %d\n",slot,buffer[0],pid);
	iI=_bg_check_Type_Byslotid(slot);
	
	if( bg_SetFilterData[iI].callback_fun != NULL )
	{
		switch(iI)
		{
			case SYS_BG_PAT:
				error=_bg_PAT_Table(buffer, length,pid,slot);
			break;
			case SYS_BG_NIT:
		{
			S32 ret = 0;
			//U32 _version = 0;
			#if 0
			PSISI_Table_t **    pptTable = NULL;
			NIT_Section_t *pNit_Section = NULL;
			int sec_num = 0, desc_num = 0;
			*pptTable = PSISI_Alloc_Table(pid, buffer, length);
			if (NULL == *pptTable)
			{
				_bgFreeDataBuffer(buffer);
				return;
			}
			PSISI_Parse_NIT_Section(*pptTable, buffer, length);
			if (PSISI_Is_Table_Complete(*pptTable) != 0)
			{
				for( sec_num = 0; sec_num < *pptTable->u16SectionNum; sec_num++ )
				{
					pNit_Section = (NIT_Section_t *)pptTable->ppSection[sec_num];
					desc_num = pNit_Section->u16OtherDescNum
					Loader_Parse_Descriptor(pNit_Section->ptDesc, desc_num);
				}
				PSISI_Free_Table(pptTable);
			}
			#endif
			// _version = ( buffer[ 5 ] & 0x3e ) >> 1;
			//pbiinfo("ZDW===>_version=%d\n", _version);
			if( 0 == dl_flag )
			{
				ret = Loader_Parse_NIT(buffer);
				if( 1 == ret )
				{
					dl_flag = 1;
					if(g_nit_download_cb !=NULL)
					{
						g_nit_download_cb(1);
						error = SYS_TABLE_NOERROR;
						//pbiinfo("ZDW===>nit download!\n");
						break;
					}
					else
					{
						dl_flag = 0;
						pbiinfo("ZDW===>nit  download g_nitver_download_cb is NULL!\n");
					}
				}				
			}
			
			//pbiinfo("ZDW===>_bg_NIT_Table!!!");
				error=_bg_NIT_Table(buffer, length,pid,slot);
		}
			break;

			case SYS_BG_BAT:
			case SYS_BG_SDT:
				error=_bg_SDT_Table(buffer, length,pid,slot);
			break;
			case SYS_BG_CAT:
				error=_bg_CAT_Table(buffer, length,pid,slot);
			break;
			case SYS_BG_TDT:
				error=_bg_TDT_Table(buffer, length,pid,slot);
			break;
			case SYS_BG_EIT:
				error=_bg_EIT_Table(buffer, length,pid,slot);
			break;
			case SYS_BG_PMT:
		
#if 0
/* UTI CA start */
				if((g_pmt_callbackfun!=NULL)&&(UTI_pid !=0x1fff))
				{
					S32   cmp_flag =0;
					
					if(1/*check_crc(buffer)==1*/)
					{
						
						if(pmt_section_buf == NULL)
						{
							printf("send pmt to cas --_bg_FilterInfoCallBack %x,%d,%d,%d--------------\n",buffer, length,pid,slot);
							g_pmt_callbackfun(0,pid,buffer );
						}
						else
						{
							cmp_flag = memcmp( pmt_section_buf, buffer, length );
							if(cmp_flag !=0)
							{
								g_pmt_callbackfun(0,pid,buffer );					

							}
						}
						if(pmt_section_buf !=NULL)
						{
							OS_MemDeallocate(system_p, pmt_section_buf);
						}
						pmt_section_buf =OS_MemAllocate(system_p, length);
						if(pmt_section_buf !=NULL)
						{
							memcpy(pmt_section_buf,buffer,length);
						}
						
					}										
					
				}
/*  UTI CA  end */				
#endif				
				error=_bg_PMT_Table(buffer, length,pid,slot);
			break;
			default:
			break;
		}
	}
	if (error == SYS_TABLE_CHECKDATA_ERROR && buffer != NULL)
    {
        //pbiinfo("[%s %d] Bg Tebale = 0x%x.\n",DEBUG_LOG, iI);
		_bgFreeDataBuffer(buffer);
}
}

 /******************************************************************************
* Function : 		_bg_SetFilterDataToDef
* parameters :	
*				Back_Type : the type of table
* Return :	
*				(nothing)		
* Description :
*				Init the data of SetFilterData 
*
* Author : 		pxwang	2008/03/26
******************************************************************************/
 void _bg_SetFilterDataToDef(U32  Back_Type)
{	
	bg_SetFilterData[Back_Type].status = BG_SLOT_IDLE;
	bg_SetFilterData[Back_Type].slotID = 			NO_OF_SIMULTANEOUS_CHANNELS;
	bg_SetFilterData[Back_Type].filterID = 			NO_OF_SIMULTANEOUS_FILTERS;
	bg_SetFilterData[Back_Type].filterPID = 		0xFFFF;
	bg_SetFilterData[Back_Type].tableID = 		0xFFFF;
	bg_SetFilterData[Back_Type].bg_type= SYS_BG_NOTABLE;
	bg_SetFilterData[Back_Type].first_section=1;
	bg_SetFilterData[Back_Type].section_version = 0xFFFF;
	bg_SetFilterData[Back_Type].callback_fun = NULL;
}

void SYS_BackGround_Bat_Default(void)
 {
 	U32 iI;	
	for(iI=0;iI<65536;iI++)	//��ʼ��BAT Check�����?
		bat_check[iI]=0xffff;
 }

 /******************************************************************************
* Function : 		_bg_closeFilter
* parameters :	
*				bgfilterdata :  the Data to set Filter & Slot
* Return :	
*				(nothing)		
* Description :
*				This function is close a type of BackGround
*				ͨ������Ҫ�رյ�Filter����ݣ��ر�ָ�����͵�BackGround
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
static U32 _bg_closeFilter(BG_SetFilter_t bgfilterdata)
{
 	U32 ret = SYS_TABLE_NOERROR;
	S32 filterid, slotid;

	filterid= bgfilterdata.filterID;
	slotid = bgfilterdata.slotID;
	if(SFILTER_Free_Filter(filterid) == 0)
	{
		if(SFILTER_NumFilterAssociate_Slot(slotid)==0)/*Modify by LSZ 2010.05.20*/
		{
			if(SFILTER_Close_Channel(slotid)==0)
			{
				/*scj modify 100919{{{*/
				if( SFILTER_Free_Channel(slotid) == 0 )
					ret =SYS_TABLE_NOERROR;
				else
					ret =SYS_TABLE_CHANFR_ERROR;
				/*}}}scj modify 100919*/
			}
			else
			{
				 ret =SYS_TABLE_CHANFR_ERROR;
			}
		}
	}
	else
		ret =SYS_TABLE_FILTFR_ERROR;
	
	return ret;
 }

 /******************************************************************************
* Function : 		SYS_BackGround_Init
* parameters :	
*				(nothing)	
* Return :	
*				(nothing)		
* Description :
*				Init the data of All BackGround & other data 
*				��ʼ��BackGround�����?
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
void SYS_BackGround_Init(void)
{
	U32	iI = 0;
	C8 buffer[20];

	for(iI = 0 ; iI < BG_MAX_FILTER; iI++) //��ʼ������BackGround�����?
	{
	    memset( buffer, 0, sizeof(buffer));
		_bg_SetFilterDataToDef(iI);
	    sprintf(buffer, "%s%d", "SYS_BG_Sem",iI);
	    OS_SemCreate(&bg_SemInUse[iI], buffer, 1, 0);
        /*LSZ 2010/04/16�޸��ź���û�г�ʼ������*/

	}
	SYS_BackGround_Bat_Default();
}
 /******************************************************************************
* Function : 		Set_NIT_First_flag
* parameters :	
*				enable :  the NIT FirstFlag value
* Return :	
*				(nothing)		
* Description :
*				set the Nit First Flag
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
void Set_NIT_First_flag(U8 enable)
{
	Boot_FirstNIT = enable;
}

 /******************************************************************************
* Function : 		SYS_BackGround_TableInit
* parameters :	
*				Back_Type :  Start the background type
*				init :  the Semaphore & CallBack			
* Return :		
*				
* Description :	Init One background
*				����һ�����͵�BackGround����ز���?
*				����
*				1.CallBack������ݱ������ź���?
*				2.��������ز���TableID��PID  (PMT������ΪPMT��PID����)
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
U32 SYS_BackGround_TableInit(SYS_BG_Type_e Back_Type, SYS_Table_InitParam_t init)
{
	U32 ret = SYS_TABLE_NOERROR;	
	SYS_Table_Param_t bgSetFilter ;
	
	memset( &bgSetFilter, 0, sizeof(SYS_Table_Param_t));
/*LSZ 2010/04/16�޸��ź���û�г�ʼ������*/
BgOpLock(Back_Type);	
	bg_SetFilterData[Back_Type].callback_fun = init.callback;
	bg_SetFilterData[Back_Type].semaphore=init.p_semaphore;
	bg_SetFilterData[Back_Type].bg_type= Back_Type;	
	switch(Back_Type)
	{
		case SYS_BG_PAT:
			bg_SetFilterData[Back_Type].filterPID = 0x00;
			bg_SetFilterData[Back_Type].tableID = 0x00;		
		break;
		case SYS_BG_NIT:
			bg_SetFilterData[Back_Type].filterPID = 0x10;
			bg_SetFilterData[Back_Type].tableID = 0x40;	
			Boot_FirstNIT = TRUE;
		break;
		case SYS_BG_SDT:
			bg_SetFilterData[Back_Type].filterPID = 0x11;
			bg_SetFilterData[Back_Type].tableID = 0x42;	
		break;
		case SYS_BG_BAT:
			bg_SetFilterData[Back_Type].filterPID = 0x11;
			bg_SetFilterData[Back_Type].tableID = 0x4a;	
		break;
		case SYS_BG_CAT:
			bgSetFilter.pid=bg_SetFilterData[Back_Type].filterPID = 0x01;
			bgSetFilter.table_id=bg_SetFilterData[Back_Type].tableID = 0x01;	
		break;
		case SYS_BG_TDT:
			bgSetFilter.pid=bg_SetFilterData[Back_Type].filterPID = 0x14;
			bgSetFilter.table_id=bg_SetFilterData[Back_Type].tableID = 0x70;	
		break;
		case SYS_BG_EIT:
			bgSetFilter.pid=bg_SetFilterData[Back_Type].filterPID = 0x12;
			bgSetFilter.table_id=bg_SetFilterData[Back_Type].tableID = 0x50;				
		break;

		default:
		break;
	}
BgOpUnLock(Back_Type);	
	return ret;
}

 /******************************************************************************
* Function : 		SYS_BackGround_TableStart
* parameters :	
*				Back_Type :  Start the background type
*				bgfilterdata :  he Data set to Filter&Slot
* Return :		
*				SYS_TABLE_NOERROR - The Work is OK
*				
* Description :	Start One background
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
#define EPG_TABLE_NUM    (6)
U32 SYS_BackGround_TableStart(SYS_BG_Type_e Back_Type , SYS_Table_Param_t bgfilterdata)
{
	U8 Data[FILTER_DEPTH_SIZE] = {0};
	U8 Mask[FILTER_DEPTH_SIZE] = {0};
	U8 scFilterNegate[FILTER_DEPTH_SIZE] = {0};
	U32	iI = 0, ret = SYS_TABLE_UNKNOWN, bNoError = 1, bChannelFlag = 1;
 	tSFILTER_SlotId slotid = -1;
	tSFILTER_FilterId filterid = -1;
	
	if(bg_SetFilterData[Back_Type].status == BG_SLOT_USE)
	{
		return SYS_TABLE_NOERROR;
	}

#ifdef AMLOGIC_DEMUX_EPG      
	if(Back_Type ==SYS_BG_EIT)
	{
		tSFILTER_FilterId filterid1[EPG_TABLE_NUM];
		
		U8      tableid[EPG_TABLE_NUM] ={0x51,0x50,0x4e,0x4f,0x60,0x61}; 
		U8     ii;
		
		BgOpLock(Back_Type);	
		slotid = SFILTER_Check_DataPID(bgfilterdata.pid,DEMUX_0); 
		if(slotid >= 0)
		{
			pbiinfo("warning: diable channel failed.  line%d\n",__LINE__);
		}
		else
		{	
			bChannelFlag =0;
			slotid = SFILTER_Allocate_Channel(DEMUX_0);
			if(slotid <0)
			{
				pbiinfo("warning: diable channel failed.  line%d\n",__LINE__);
				BgOpUnLock(Back_Type);	
				return SYS_TABLE_FILTFR_ERROR;
			}

			SFilterCallback_t callbacks;
			callbacks.pFCallBack = (tSFILTER_SlotCallbackFct)_bg_FilterInfoCallBack;
			callbacks.pFGetBuffer = (tSFILTER_GetBufferFct)_bgGetSectionDataBuffer;
			callbacks.pFFreeBuffer =(tSFILTER_FreeBufferFct)_bgFreeSectionDataBuffer;
			SFILTER_RegisterCallBack_Function(slotid,	&callbacks);
                     pbiinfo("===epg:   slot allocate Successful\n");
		}

		for(ii=0;ii<EPG_TABLE_NUM;ii++)
		{
			filterid = SFILTER_Allocate_Filter(DEMUX_0);
			if(filterid<0)
			{
				if(bChannelFlag==0)
				{
					if(SFILTER_NumFilterAssociate_Slot(slotid) == 0)
						SFILTER_Free_Channel(slotid);
				}
					
				slotid = INVALID_DEMUX_SOCKETID;	
				ret = SYS_TABLE_FILTALLO_ERROR;
				BgOpUnLock(Back_Type);	
				return SYS_TABLE_FILTFR_ERROR;
			}

			memset(Mask, 0xff, FILTER_DEPTH_SIZE);
			memset(Data, 0x0, FILTER_DEPTH_SIZE);
			
			Data[0] =tableid[ii];
			Mask[0] = 0x0;

			bNoError = SFILTER_SetFilter(slotid, filterid, FILTER_DEPTH_SIZE, Mask, Data, scFilterNegate);
			if(bNoError<0)
			{
				SFILTER_Free_Filter(filterid);
				if(bChannelFlag == 0)
				{
					if(SFILTER_NumFilterAssociate_Slot(slotid) == 0)
						SFILTER_Free_Channel(slotid);
				}
				
				pbiinfo("warning: diable channel failed.  line%d\n",__LINE__);
				BgOpUnLock(Back_Type);	
				return SYS_TABLE_FILTFR_ERROR;
			}
		}

			
		if(bChannelFlag==0)
		{
			bNoError = SFILTER_SetPid(slotid, bgfilterdata.pid);
			if(bNoError<0)
			{
				SFILTER_Free_Filter(filterid);
				if(SFILTER_NumFilterAssociate_Slot(slotid) == 0)
					SFILTER_Free_Channel(slotid);
				
				pbiinfo("warning: diable channel failed.  line%d\n",__LINE__);
				BgOpUnLock(Back_Type);	
				return SYS_TABLE_FILTFR_ERROR;
			}
			else
			{
				bNoError = SFILTER_Open_Channel(slotid);
				if(bNoError<0)
				{
					SFILTER_Free_Filter(filterid);
					if(SFILTER_NumFilterAssociate_Slot(slotid) == 0)
						SFILTER_Free_Channel(slotid);
					
					pbiinfo("warning: diable channel failed.  line%d\n",__LINE__);
					BgOpUnLock(Back_Type);	
					return SYS_TABLE_FILTFR_ERROR;
				}
				else
				{
					//printf("SYS_BackGround_TableStart %d\n",__LINE__);
					bNoError=1;	
				}
					
			}
		}

		if (bg_SetFilterData[Back_Type].status == BG_SLOT_IDLE)
		{			
			bg_SetFilterData[Back_Type].status = BG_SLOT_USE;
			bg_SetFilterData[Back_Type].slotID = slotid;
			bg_SetFilterData[Back_Type].filterID = filterid;
			bg_SetFilterData[Back_Type].filterPID = bgfilterdata.pid;
			bg_SetFilterData[Back_Type].tableID = bgfilterdata.table_id;	
		}

		BgOpUnLock(Back_Type);	
		 pbiinfo("===SYS_BackGround_TableStart:   slot allocate Successful   --------------------------\n");
		return 0;

		
	}

#endif
	

	BgOpLock(Back_Type);	
	if (Back_Type != SYS_BG_PMT)
	{
		bgfilterdata.pid = bg_SetFilterData[Back_Type].filterPID;
		bgfilterdata.table_id = bg_SetFilterData[Back_Type].tableID;		
	}
	else
	{
		bg_SetFilterData[Back_Type].first_section = 1;
	}

	slotid = SFILTER_Check_DataPID(bgfilterdata.pid,DEMUX_0); //xhren0709
	//printf("bgfilterdata.pid = %d slotid = %d\n",bgfilterdata.pid,slotid);

	/*add by yao lihui 20110312 ���С��background nit����{{{*/
	if(slotid >= 0)
	{
		if (SFILTER_Free_Channel(slotid) < 0)
		{
			printf("warning: diable channel failed.\n");
			ret = SYS_TABLE_FILTFR_ERROR;
		}
	}
	
	/*add by yao lihui 20110312 ���С��background nit����{{{*/
 //	if(slotid < 0)//del by yao lihui 20110312 ���С��background nit����
	{
		memset(Mask, 0xff, FILTER_DEPTH_SIZE);
		bChannelFlag=0;
		//slotid = SFILTER_Allocate_Channel(eSBUFFER_MODE_4K, 4, bgfilterdata.timeout);
		slotid = SFILTER_Allocate_Channel(DEMUX_0);
		if(slotid < 0)
		{
			slotid = INVALID_DEMUX_SOCKETID;
			ret = SYS_TABLE_CHANALLO_ERROR;
                     pbiinfo("===BackGround:   slot allocate failure\n");
			//DB_BG((TRC_INFO,"===BackGround:   slot allocate failure\n"));
			bNoError=0;
		}
		else
		{
			SFilterCallback_t callbacks;
			callbacks.pFCallBack = (tSFILTER_SlotCallbackFct)_bg_FilterInfoCallBack;
			callbacks.pFGetBuffer = (tSFILTER_GetBufferFct)_bgGetSectionDataBuffer;
			callbacks.pFFreeBuffer =(tSFILTER_FreeBufferFct)_bgFreeSectionDataBuffer;
			SFILTER_RegisterCallBack_Function(slotid,	&callbacks);
                     pbiinfo("===BackGround:   slot allocate Successful\n");
			//DB_BG((TRC_INFO,"===BackGround:   slot allocate Successful\n"));
		}
	}
	//printf("SYS_BackGround_TableStart (), Back_Type = %d, slotIndex = %d\n", Back_Type, slotid);
	if(bNoError==1)
	{
		//printf("SYS_BackGround_TableStart %d\n",__LINE__);
		bNoError = 0;	
		S8 scDemuxError;
	//	filterid = SFILTER_Allocate_Filter(slotid, eSFILTER_MODE_HW);
		filterid = SFILTER_Allocate_Filter(DEMUX_0);
		if(filterid<0)
		{
			// xhren0713, �ͷŸ�background�����channel,�𴦷����channel���ͷ�
			//printf("SYS_BackGround_TableStart %d\n",__LINE__);
			if(bChannelFlag==0)
			{
				if(SFILTER_NumFilterAssociate_Slot(slotid) == 0)
					SFILTER_Free_Channel(slotid);
			}
				
			slotid = INVALID_DEMUX_SOCKETID;	
			ret = SYS_TABLE_FILTALLO_ERROR;
			DB_BG((TRC_INFO,"===BackGround:   Filter allocate Successful\n"));
		}
		else
		{
			if(bgfilterdata.table_id >= 0)
			{	
				//printf("SYS_BackGround_TableStart %d\n",__LINE__);
				if ((Back_Type == SYS_BG_EIT)|| (Back_Type == SYS_BG_SDT)) //����SDT&EIT��BackGround��ֹһ��TableID�Ĵ���
					Mask[0]=0xff;
				else
				{
					Data[0] = bgfilterdata.table_id;
					Mask[0] = 0;
				}
				if(Back_Type == SYS_BG_PMT)
				{
			    		Data[1] = (bgfilterdata.pn>> 8) & 0xff;
				   	Data[2] = bgfilterdata.pn & 0xff;
					Mask[1] = 0;
					Mask[2] = 0;
				}
			}
			//SFILTER_SetFilter(slotid, filterid, Data, Mask, 0);
			scDemuxError = SFILTER_SetFilter(slotid, filterid, FILTER_DEPTH_SIZE, Mask, Data, scFilterNegate);
			if(scDemuxError<0)
			{
				SFILTER_Free_Filter(filterid);
				if(bChannelFlag == 0)
				{
					if(SFILTER_NumFilterAssociate_Slot(slotid) == 0)
						SFILTER_Free_Channel(slotid);
				}
				
				ret = SYS_TABLE_FILTSET_ERROR;
				//printf("SYS_BackGround_TableStart %d\n",__LINE__);
			}
			else
			{
				//printf("bChannelFlag = %d\n",bChannelFlag);
				if(bChannelFlag==0)
				{
					scDemuxError = SFILTER_SetPid(slotid, bgfilterdata.pid);
					if(scDemuxError<0)
					{
						SFILTER_Free_Filter(filterid);
						if(SFILTER_NumFilterAssociate_Slot(slotid) == 0)
							SFILTER_Free_Channel(slotid);
						
						ret = SYS_TABLE_FILTSET_ERROR;
						//printf("SYS_BackGround_TableStart %d\n",__LINE__);
					}
					else
					{
						scDemuxError = SFILTER_Open_Channel(slotid);
						if(scDemuxError<0)
						{
							SFILTER_Free_Filter(filterid);
							if(SFILTER_NumFilterAssociate_Slot(slotid) == 0)
								SFILTER_Free_Channel(slotid);
							
							ret = SYS_TABLE_CHANCON_ERROR;
							//printf("SYS_BackGround_TableStart %d\n",__LINE__);
						}
						else
						{
							//printf("SYS_BackGround_TableStart %d\n",__LINE__);
							bNoError=1;	
						}
							
					}
				}
				
			}
					
		}
	}
	if (bNoError)
	{	
		//printf("SYS_BackGround_TableStart %d\n",__LINE__);
		if (bg_SetFilterData[Back_Type].status == BG_SLOT_IDLE)
		{			
			bg_SetFilterData[Back_Type].status = BG_SLOT_USE;
			bg_SetFilterData[Back_Type].slotID = slotid;
			bg_SetFilterData[Back_Type].filterID = filterid;
			bg_SetFilterData[Back_Type].filterPID = bgfilterdata.pid;
			bg_SetFilterData[Back_Type].tableID = bgfilterdata.table_id;	
		}
		if (iI >= BG_MAX_FILTER)
		{
			//DB_BG((TRC_INFO,"===BackGround:    BG OVERFLOW\n\n"));
                     pbierror("===BackGround:    BG OVERFLOW\n\n");
			ret = SYS_TABLE_LARTHANMAX_ERROR;
		}
		else
		{
			ret = SYS_TABLE_NOERROR;
		}
		//printf("iI = %d\n",iI);
		//printf("SYS_BackGround_TableStart %d\n",__LINE__);
	}
BgOpUnLock(Back_Type);	
	if (ret)
	{
		//DB_BG((TRC_INFO,"===BackGround:    The SYS_BG Type:%d   Error is %d\n",Back_Type,ret));
              pbierror("===BackGround:    The SYS_BG Type:%d   Error is %d\n",Back_Type,ret);
	}
	return ret;
}

 /******************************************************************************
* Function : 		SYS_BackGround_TableStop
* parameters :	
*				Back_Type :  Stop the background type
* Return :		
*				SYS_TABLE_NOERROR :  The Work is OK
*				SYS_TABLE_NORUNNING  :  The BackGroud not running couldn't control
*
* Description :	Stop One background
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
U32 SYS_BackGround_TableStop(SYS_BG_Type_e Back_Type)
 {
 	U32 ret=SYS_TABLE_UNKNOWN;
	
	BgOpLock(Back_Type);	
	if (bg_SetFilterData[Back_Type].status == BG_SLOT_USE)
	{
		ret = _bg_closeFilter(bg_SetFilterData[Back_Type]);
		if (ret == SYS_TABLE_NOERROR)
		{
			bg_SetFilterData[Back_Type].status = BG_SLOT_IDLE;
		}
	}
	else
		ret =SYS_TABLE_NOERROR;
	BgOpUnLock(Back_Type);	
	if (ret)
	{
	       pbierror("[%s %d]...SYS_BackGround_TableStop error  Error is %d.....\n",DEBUG_LOG,ret);
		//DB_BG((TRC_INFO,"===BackGround:    The SYS_BG Type:%d   Error is %d\n",Back_Type,ret));
	}
	return ret;
 }

 /******************************************************************************
* Function : 		SYS_BackGround_TablePause
* parameters :	
*				Back_Type :  Pause the background type
* Return :		
*				SYS_TABLE_NOERROR :  The Work is OK
*				SYS_TABLE_FILTCON_ERROR :  The Filter Control is Error
*				SYS_TABLE_CHANCON_ERROR :  he Channel contril is error
*				SYS_TABLE_NORUNNING :  The BackGroud not running couldn't control
*
* Description :	Pause One background
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
U32 SYS_BackGround_TablePause(SYS_BG_Type_e Back_Type)
{

	S32 filterid, slotid,ret=SYS_TABLE_UNKNOWN;
	
	BgOpLock(Back_Type);	
	if(bg_SetFilterData[Back_Type].status == BG_SLOT_USE)
	{
		filterid= bg_SetFilterData[Back_Type].filterID;
		slotid = bg_SetFilterData[Back_Type].slotID;
	
			if (SFILTER_Close_Channel(slotid)>=0)
			{
			S8 scDemuxError = SFILTER_Control_Filter(filterid,eSFILTER_CTRL_DISABLE);
			if(scDemuxError<0)
			{
				printf("SYS_BackGround_TablePause=>SFILTER CONTROL FILTER ERROR!\n");
				ret = SYS_TABLE_FILTCON_ERROR;
			}
			else
			{
					bg_SetFilterData[Back_Type].status=BG_SLOT_PAUSE;
					ret =SYS_TABLE_NOERROR;		
			}
		}
			else
			{
				printf("SYS_BackGround_TablePause=>SYS_TABLE_CHANCON_ERROR\n");
				ret =SYS_TABLE_CHANCON_ERROR;
			}

	}
	else
	{
		ret = SYS_TABLE_NOERROR;
	}
	BgOpUnLock(Back_Type);	
	if (ret)
	{
		//DB_BG((TRC_INFO,"===BackGround:    The SYS_BG Type:%d   Error is %d\n",Back_Type,ret));
		printf("===BackGround:    The SYS_BG Type:%d   Error is %d\n",Back_Type,ret);

	}
	return ret;
}

/******************************************************************************
* Function : 		SYS_BackGround_TableResume
* parameters :	
*				Back_Type :  Resume the background type
* Return :		
*				SYS_TABLE_NOERROR :  The Work is OK
*				SYS_TABLE_FILTCON_ERROR :  The Filter Control is Error
*				SYS_TABLE_CHANCON_ERROR  :  The Channel contril is error
*				SYS_TABLE_NORUNNING :  The BackGroud not running couldn't control
*
* Description :	Resume One background
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
U32 SYS_BackGround_TableResume(SYS_BG_Type_e Back_Type)
{
	S32 filterid, slotid,ret=SYS_TABLE_UNKNOWN;
	
	BgOpLock(Back_Type);	
	if(bg_SetFilterData[Back_Type].status == BG_SLOT_PAUSE)
	{
		filterid= bg_SetFilterData[Back_Type].filterID;
		slotid = bg_SetFilterData[Back_Type].slotID;
		if(SFILTER_Open_Channel(slotid)>=0) 
		{
			S8 scDemuxError = SFILTER_Control_Filter(filterid,eSFILTER_CTRL_ENABLE);
			if(scDemuxError<0)
		{
				printf("SYS_BackGround_TablePause=>SFILTER CONTROL FILTER ERROR!\n");
				ret = SYS_TABLE_FILTCON_ERROR;
			}
			else
			{
				bg_SetFilterData[Back_Type].status=BG_SLOT_USE;
				ret =SYS_TABLE_NOERROR;		
			}
		}
		else 
		{
			printf("SYS_BackGround_TablePause=>SYS_TABLE_CHANCON_ERROR\n");
			ret =SYS_TABLE_CHANCON_ERROR;
	}
	}
	else
	{
		ret = SYS_TABLE_NOERROR;	
	}
	BgOpUnLock(Back_Type);	

	if (ret)
	{
		DB_BG((TRC_INFO,"===BackGround:    The SYS_BG Type:%d   Error is %d\n",Back_Type,ret));
	}
	return ret;
}
/******************************************************************************
* Function : 		SYS_BackGround_TableStart
* parameters :	
*				Back_Type :  Start the background type
*				bgfilterdata :  he Data set to Filter&Slot
* Return :		
*				SYS_TABLE_NOERROR - The Work is OK
*				
* Description :	Start One background
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
U32 SYS_BackGround_PMT_Start(void)
 {
	U32	iI = 0;
	U32 ret = SYS_TABLE_UNKNOWN;
	U32 bNoError = 1;
	U32 Start_TahleID =0xffff;
 	tSFILTER_SlotId slotid = 0;
	tSFILTER_FilterId filterid = 0;

	if(bg_SetFilterData[SYS_BG_PMT].status == BG_SLOT_USE)
	{
		return SYS_TABLE_NOERROR;
	}
	BgOpLock(SYS_BG_PMT);	
	Start_TahleID = 0x02;
	bg_SetFilterData[SYS_BG_PMT].first_section = 1;

	slotid = SFILTER_Allocate_Channel(DEMUX_0);
	if(slotid < 0)
	{
		slotid = INVALID_DEMUX_SOCKETID;
		ret = SYS_TABLE_CHANALLO_ERROR;
		DB_BG((TRC_INFO,"===BackGround:   slot allocate failure\n"));
		bNoError=0;
	}
	else
	{
		SFilterCallback_t callbacks;
		callbacks.pFCallBack = (tSFILTER_SlotCallbackFct)_bg_FilterInfoCallBack;
		callbacks.pFGetBuffer = (tSFILTER_GetBufferFct)_bgGetSectionDataBuffer;
		callbacks.pFFreeBuffer =(tSFILTER_FreeBufferFct)_bgFreeSectionDataBuffer;
		SFILTER_RegisterCallBack_Function(slotid,	&callbacks);
		DB_BG((TRC_INFO,"===BackGround:   slot allocate Successful\n"));
	}
	g_pmt_slot_id = slotid;
	if(bNoError==1)
	{
		bNoError = 0;	
		filterid = SFILTER_Allocate_Filter(DEMUX_0);
		if(filterid<0)
		{
				if(SFILTER_NumFilterAssociate_Slot(slotid) == 0)
					SFILTER_Free_Channel(slotid);
			
			slotid = INVALID_DEMUX_SOCKETID;	
			ret = SYS_TABLE_FILTALLO_ERROR;
			DB_BG((TRC_INFO,"===BackGround:   Filter allocate Successful\n"));
		}
		else
		{
				bNoError=1;			
		}
	}

	if (bNoError)
	{			
		if (bg_SetFilterData[SYS_BG_PMT].status == BG_SLOT_IDLE)
		{			
			bg_SetFilterData[SYS_BG_PMT].slotID = slotid;
			bg_SetFilterData[SYS_BG_PMT].filterID = filterid;
			bg_SetFilterData[SYS_BG_PMT].tableID = Start_TahleID;	
		}
		if (iI >= BG_MAX_FILTER)
		{
			DB_BG((TRC_INFO,"===BackGround:    BG OVERFLOW\n\n"));		
			ret = SYS_TABLE_LARTHANMAX_ERROR;
		}
		else
		{
			ret = SYS_TABLE_NOERROR;
		}
	}
	BgOpUnLock(SYS_BG_PMT);	
	if (ret)
	{
		DB_BG((TRC_INFO,"===BackGround:    The SYS_BG Type:%d   Error is %d\n",Back_Type,ret));
	}
	return ret;
 }


/******************************************************************************
* Function : 		SYS_BackGround_TableReset
* parameters :	
*				Back_Type :  Resume the background type
* Return :		
*				SYS_TABLE_NOERROR :  The Work is OK
*				SYS_TABLE_FILTCON_ERROR :  The Filter Control is Error
*				SYS_TABLE_CHANCON_ERROR  :  The Channel contril is error
*				SYS_TABLE_NORUNNING :  The BackGroud not running couldn't control
*
* Description :	Resume One background
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
U32 SYS_BackGround_PMT_Reset(U32 reqpid, U32 reqpn)
{
	S32 filterid, slotid,ret=SYS_TABLE_UNKNOWN;
	U8 Data[FILTER_DEPTH_SIZE] = {0};
	U8 Mask[FILTER_DEPTH_SIZE] = {0};
	U8 scFilterNegate[FILTER_DEPTH_SIZE] = {0};

	BgOpLock(SYS_BG_PMT);
	memset(Mask, 0xff, FILTER_DEPTH_SIZE);
	if(bg_SetFilterData[SYS_BG_PMT].status != BG_SLOT_USE)
	{
		filterid= bg_SetFilterData[SYS_BG_PMT].filterID;
		slotid = bg_SetFilterData[SYS_BG_PMT].slotID;

		if(bg_SetFilterData[SYS_BG_PMT].tableID> 0)
		{	
			Data[0] = bg_SetFilterData[SYS_BG_PMT].tableID;
			Mask[0] = 0;
	    		Data[1] = (reqpn>> 8) & 0xff;
		   	Data[2] = reqpn & 0xff;
			Mask[1] = 0;
			Mask[2] = 0;
		}

		if (SFILTER_SetFilter(slotid, filterid, FILTER_DEPTH_SIZE, Mask, Data, scFilterNegate) >= 0)
		{
			S8 scDemuxError;
			if (SFILTER_SetPid(slotid, reqpid) >= 0)
			{
				bg_SetFilterData[SYS_BG_PMT].status=BG_SLOT_USE;				
				bg_SetFilterData[SYS_BG_PMT].filterPID = reqpid;	
				bg_SetFilterData[SYS_BG_PMT].first_section = 1;
				ret =SYS_TABLE_NOERROR;		
				
				scDemuxError = SFILTER_Open_Channel(slotid);
				if(scDemuxError<0)
				{
					SFILTER_Free_Filter(filterid);
					if(SFILTER_NumFilterAssociate_Slot(slotid) == 0)
						SFILTER_Free_Channel(slotid);

					ret = SYS_TABLE_CHANCON_ERROR;
					
				}
				else
				{
					ret = SYS_TABLE_CHANSET_ERROR;
			}
			}
			else
			{
				ret = SYS_TABLE_CHANSET_ERROR;
			}
		}
		else
		{
			ret = SYS_TABLE_FILTSET_ERROR;
		}
	}
	else
	{
		ret = SYS_TABLE_NOERROR;	
	}
	BgOpUnLock(SYS_BG_PMT);	
	if (ret)
	{
		DB_BG((TRC_INFO,"===BackGround:    The SYS_BG Type:%d   Error is %d\n",Back_Type,ret));
	}
	return ret;
}

 /******************************************************************************
* Function : 		_bg_PAT_Table
* parameters :	
*				data_buffer :  the buffer save the new data to dealwith
*				sectionlength :   the section length of the data
*				pid :  the Slot PID
*				slot :  the slot id
* Return :		
*				1. the data update
*				0. the data not update
*
* Description :	Deal with the receive data
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
static U32 _bg_PAT_Table(U8 *data_buffer, U32 sectionlength,U16 pid,tSFILTER_SlotId 	slot)
{
	U32			section_number, last_section_number;
	S32 			total_sections, j;
	U8			new_table;
	SYS_BG_TPParam_t param_infor;
	SYS_Table_Msg_t 	meg_send;

	if(0 != data_buffer[0])
	{
		return SYS_TABLE_CHECKDATA_ERROR;
	}

    if( NULL != bg_SetFilterData[SYS_BG_PAT].callback_fun )
    {
        meg_send.buffer = data_buffer;
        meg_send.pid = pid;
        meg_send.section_length = sectionlength;
        meg_send.table_id = bg_SetFilterData[SYS_BG_PAT].tableID;
        meg_send.version = bg_SetFilterData[SYS_BG_PAT].section_version;
        meg_send.manage_type = pTable_update;
        meg_send.msgType = pTable_complete;
        meg_send.pfFreeBuffer = (tBm_FreeBufferFct)_bgFreeDataBuffer;
        (*bg_SetFilterData[SYS_BG_PAT].callback_fun)(meg_send);
        OS_SemWait(bg_SetFilterData[SYS_BG_PAT].semaphore);
        return SYS_TABLE_NOERROR;
    }

#if 0
	_get_program_info(&param_infor);
	new_table = _bg_check_pat_info(param_infor, data_buffer);
	if (new_table)
	{		
		if( bg_SetFilterData[SYS_BG_PAT].first_section ==  1 )
		{
			bg_SetFilterData[SYS_BG_PAT].first_section = 0;
			bg_SetFilterData[SYS_BG_PAT].section_version = ( data_buffer[ 5 ] & 0x3e ) >> 1;
			bg_SetFilterData[SYS_BG_PAT].tableID = data_buffer[0];
			last_section_number = data_buffer[ 7 ];
			for( j = 0; j < MAX_SECTION_NUM / 32; j ++ )
			{
				 bg_SetFilterData[SYS_BG_PAT].sections_arrived[ j ] = 0;
				 bg_SetFilterData[SYS_BG_PAT].all_sections_mask[ j ] = 0;
			}
			total_sections = last_section_number + 1;
			j = 0;
			while( total_sections >= 32 )
			{
				 bg_SetFilterData[SYS_BG_PAT].all_sections_mask[ j ] = 0xffffffff;
				j ++;
				total_sections -=32;
			}
			if ( total_sections )
				 bg_SetFilterData[SYS_BG_PAT].all_sections_mask[ j ] = ( ( 1 << total_sections ) - 1 ) ;
		}
		meg_send.buffer = data_buffer;
		meg_send.pid = pid;
		meg_send.section_length = sectionlength;
		meg_send.table_id = bg_SetFilterData[SYS_BG_PAT].tableID;
		meg_send.version = bg_SetFilterData[SYS_BG_PAT].section_version;
		meg_send.pfFreeBuffer=(tBm_FreeBufferFct)_bgFreeDataBuffer;
		section_number = data_buffer[ 6 ];

		if(((data_buffer[5]&0x3e)>>1) != bg_SetFilterData[SYS_BG_PAT].section_version )
		{		
			bg_SetFilterData[SYS_BG_PAT].first_section = 1;		
			return SYS_TABLE_CHECKDATA_ERROR;
		}
		j = 0;
		while( section_number >= 32 )
		{
			j ++;
			section_number -=32;
		}
		if(  bg_SetFilterData[SYS_BG_PAT].sections_arrived[ j ] &  ( 1 << ( section_number ) ) )
		{
			return SYS_TABLE_CHECKDATA_ERROR;
		}
		 bg_SetFilterData[SYS_BG_PAT].sections_arrived[j] |= (1<<(section_number));

		for( j = 0; j <	MAX_SECTION_NUM / 32; j ++ )
		{
			if (  bg_SetFilterData[SYS_BG_PAT].sections_arrived[ j ] !=  bg_SetFilterData[SYS_BG_PAT].all_sections_mask[ j ] ) 
				break;
		}
		if (j == MAX_SECTION_NUM / 32)
		{
			meg_send.manage_type = pTable_update;
			meg_send.msgType = pTable_complete;
			(*bg_SetFilterData[SYS_BG_PAT].callback_fun)(meg_send);	
//			OS_SemWait(bg_SetFilterData[SYS_BG_PAT].semaphore);
			return SYS_TABLE_NOERROR;
		}
		else
		{
			meg_send.manage_type = pTable_update;
			meg_send.msgType = pTable_multi_section;
			(*bg_SetFilterData[SYS_BG_PAT].callback_fun)(meg_send);	
//			OS_SemWait(bg_SetFilterData[SYS_BG_PAT].semaphore);
			return SYS_TABLE_NOERROR;
		}		
	}
#endif
	return SYS_TABLE_CHECKDATA_ERROR;
}

 /******************************************************************************
* Function : 		_bg_NIT_Table
* parameters :	
*				data_buffer :  the buffer save the new data to dealwith
*				sectionlength :  the section length of the data
*				pid :  the Slot PID
*				slot :  the slot id
* Return :		
*				1. the data update
*				0. the data not update
*
* Description :	Deal with the receive data
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
static U32 _bg_NIT_Table(U8 *data_buffer, U32 sectionlength,U16 pid,tSFILTER_SlotId 	slot)
{
	U32 	version = 0xffff, table_id = 0xffff;
	SYS_Table_Msg_t 	meg_send;

	if((0x40 != data_buffer[0]) && (0x41 != data_buffer[0]))
	{
		return SYS_TABLE_CHECKDATA_ERROR;
	}
	if(DVBPlayer_Get_RunningStatus()!=eDVBPLAYER_RUNNING_STATUS_PLAY)
	{
		//pbiinfo("_bg_NIT_Table----------dvb not player --return");
		return SYS_TABLE_CHECKDATA_ERROR;
	}
#if 1

{	
	table_id = data_buffer[0];
	version = (data_buffer[5] & 0x3e) >> 1; 	
	// {{{ ��PMT��First_section��ʱ�� ʹ�õ���Zapping
//printf(" %s %d bg_SetFilterData[SYS_BG_NIT].first_section  =%d \n",__FILE__,__LINE__,bg_SetFilterData[SYS_BG_NIT].first_section);
	if (bg_SetFilterData[SYS_BG_NIT].first_section)
	{
		if (bg_SetFilterData[SYS_BG_NIT].callback_fun != NULL)
		{			
			bg_SetFilterData[SYS_BG_NIT].first_section = 0;
			bg_SetFilterData[SYS_BG_NIT].section_version = version;
			meg_send.buffer = data_buffer;
			meg_send.pid = pid;
			meg_send.section_length = sectionlength;
			meg_send.table_id = table_id;
			meg_send.version = version;
			meg_send.pfFreeBuffer = (tBm_FreeBufferFct)_bgFreeDataBuffer;
			meg_send.manage_type = pTable_scan;
			meg_send.msgType = pTable_complete;
			bg_SetFilterData[SYS_BG_NIT].callback_fun(meg_send);
			OS_SemWait(bg_SetFilterData[SYS_BG_NIT].semaphore);
			//printf(" %s %d bg_SetFilterData[SYS_BG_NIT].first_section  =%d \n",__FILE__,__LINE__,bg_SetFilterData[SYS_BG_NIT].first_section);
		}
		else
		{
			return SYS_TABLE_CHECKDATA_ERROR;
		}
	}
// }}} ��PMT��First_section��ʱ�� ʹ�õ���Zapping
	else
	{
		if(version != bg_SetFilterData[SYS_BG_NIT].section_version)
		{
			//printf("pmt new version = %d, old version = %d.\n", version, bg_SetFilterData[SYS_BG_NIT].section_version);
			bg_SetFilterData[SYS_BG_NIT].section_version = version;
			if (bg_SetFilterData[SYS_BG_NIT].callback_fun != NULL)
			{
				meg_send.buffer = data_buffer;
				meg_send.pid = pid;
				meg_send.section_length = sectionlength;
				meg_send.table_id = table_id;
				meg_send.version = version;		
				meg_send.manage_type = pTable_update;
				meg_send.msgType = pTable_complete;
				meg_send.pfFreeBuffer = (tBm_FreeBufferFct)_bgFreeDataBuffer;
				bg_SetFilterData[SYS_BG_NIT].callback_fun(meg_send);	
				OS_SemWait(bg_SetFilterData[SYS_BG_NIT].semaphore);
			}
			else
			{
				return SYS_TABLE_CHECKDATA_ERROR;
			}
		}
		else
		{
			return SYS_TABLE_CHECKDATA_ERROR;
		}
	}

}
#else
	return SYS_TABLE_CHECKDATA_ERROR;
#endif

	
	
	if(version !=nit_version)
			{
		if(g_nitver_update_cb !=NULL)
		{
			nit_version =version;
			g_nitver_update_cb(nit_version);
			//pbiinfo("_bg_NIT_Table-------send nit ver %d ",version);
		}
		else
		{
			pbiinfo("_bg_NIT_Table-------nit cb is null---\n");
		}
	}	
	return SYS_TABLE_NOERROR;
}

 /******************************************************************************
* Function : 		_bg_SDT_Table
* parameters :	
*				data_buffer :  the buffer save the new data to dealwith
*				sectionlength :  the section length of the data
*				pid  :  the Slot PID
*				slot  :  the slot id
* Return :		
*				1. the data update
*				0. the data not update
*
* Description :	Deal with the receive data
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
static U32 _bg_SDT_Table(U8 *data_buffer, U32 sectionlength,U16 pid,tSFILTER_SlotId 	slot)
{
#if BAT_SORT
	U16		bouquetid;
#endif

	U32 		table_id= 0xffff;
	U32		section_number;
	U32		last_section_number;
	U32		version = 0xffff;
	S32 		total_sections, j;
	U8		new_table;
	SYS_BG_TPParam_t param_infor;
	SYS_Table_Msg_t 	meg_send;

	if( ( data_buffer[0] == 0x42 ) || ( data_buffer[0] == 0x46 ) )		/*SDT*/
	{
		_get_program_info(&param_infor);
		new_table = _bg_check_sdt_info(param_infor, data_buffer);
		if (new_table)
		{
			if( bg_SetFilterData[SYS_BG_SDT].first_section ==  1 )
			{
				bg_SetFilterData[SYS_BG_SDT].first_section = 0;
				bg_SetFilterData[SYS_BG_SDT].section_version = ( data_buffer[ 5 ] & 0x3e ) >> 1;
				bg_SetFilterData[SYS_BG_SDT].tableID = data_buffer[0];
				last_section_number = data_buffer[ 7 ];
				for( j = 0; j < MAX_SECTION_NUM / 32; j ++ )
				{
					bg_SetFilterData[SYS_BG_SDT].sections_arrived[ j ] = 0;
					bg_SetFilterData[SYS_BG_SDT].all_sections_mask[ j ] = 0;
				}
				total_sections = last_section_number + 1;
				j = 0;
				while( total_sections >= 32 )
				{
					bg_SetFilterData[SYS_BG_SDT].all_sections_mask[ j ] = 0xffffffff;
					j ++;
					total_sections -=32;
				}
				if ( total_sections )
					bg_SetFilterData[SYS_BG_SDT].all_sections_mask[ j ] = ( ( 1 << total_sections ) - 1 ) ;
			}
			meg_send.buffer = data_buffer;
			meg_send.pid = pid;
			meg_send.section_length = sectionlength;
			meg_send.table_id = bg_SetFilterData[SYS_BG_SDT].tableID;
			meg_send.version = bg_SetFilterData[SYS_BG_SDT].section_version;
			meg_send.pfFreeBuffer=(tBm_FreeBufferFct)_bgFreeDataBuffer;
			section_number = data_buffer[ 6 ];
			if((((data_buffer[5]&0x3e)>>1) != bg_SetFilterData[SYS_BG_SDT].section_version )||
				((bg_SetFilterData[SYS_BG_SDT].tableID!=data_buffer[0])))
			{
				bg_SetFilterData[SYS_BG_SDT].first_section = 1;					 	
				return SYS_TABLE_CHECKDATA_ERROR;
			}
			j = 0;
			while( section_number >= 32 )
			{
				j ++;
				section_number -=32;
			}
			if( bg_SetFilterData[SYS_BG_SDT].sections_arrived[ j ] &  ( 1 << ( section_number ) ) )
			{
				//return SYS_TABLE_CHECKDATA_ERROR;
			}
			bg_SetFilterData[SYS_BG_SDT].sections_arrived[j] |= (1<<(section_number));

			for( j = 0; j <	MAX_SECTION_NUM / 32; j ++ )
			{
				if ( bg_SetFilterData[SYS_BG_SDT].sections_arrived[ j ] != bg_SetFilterData[SYS_BG_SDT].all_sections_mask[ j ] ) 
					break;
			}
			if (j == MAX_SECTION_NUM / 32)
			{
				meg_send.manage_type = pTable_update;
				meg_send.msgType = pTable_complete;
				(*bg_SetFilterData[SYS_BG_SDT].callback_fun)(meg_send);
//				OS_SemWait(bg_SetFilterData[SYS_BG_SDT].semaphore);
				return SYS_TABLE_NOERROR;
			}
			else
			{
				meg_send.manage_type = pTable_update;
				meg_send.msgType = pTable_multi_section;
				(*bg_SetFilterData[SYS_BG_SDT].callback_fun)(meg_send);	
//				OS_SemWait(bg_SetFilterData[SYS_BG_SDT].semaphore);
				return SYS_TABLE_NOERROR;
			}
		}
		return SYS_TABLE_CHECKDATA_ERROR;
	}
#if BAT_SORT
	else if( data_buffer[0] == 0x4A )	
	{
		static U16 first_bouqet= 0xffff;
		static U32 first_section_number= 0xffffffff;
		static BAT_UPDATE_TYPE complete_flag = BAT_UPDATE_NONE;
		static U8 first_section = 1;
		
		table_id = data_buffer[0];
		bouquetid = (data_buffer[3]<<8) + data_buffer[4];
		version = (data_buffer[5]&0x3e)>>1;
		section_number = data_buffer[6];
		
		if((bat_check[bouquetid] != version) && (first_section == 1))
		{ 
			first_section_number = section_number;
			first_bouqet = bouquetid;
			complete_flag = BAT_UPDATING;
			first_section = 0;
			printf("_bg_BAT_Table(),first_section_number=%d, first_bouqet=%d, first_section=%d\n", first_section_number, first_bouqet, first_section);
		}
		else
		{
			if((first_bouqet == bouquetid) && (first_section_number == section_number))
				complete_flag = BAT_UPDATE_COMPLETE;
		}
		
		if(complete_flag == BAT_UPDATING)
		{
			bat_check[bouquetid]=version;
			meg_send.buffer = data_buffer;
			meg_send.pid = pid;
			meg_send.section_length = sectionlength;
			meg_send.table_id = table_id;
			meg_send.version = version;
			meg_send.pfFreeBuffer=(tBm_FreeBufferFct)_bgFreeDataBuffer;
			meg_send.manage_type = pTable_update;
			meg_send.msgType = pTable_multi_section;
			(*bg_SetFilterData[SYS_BG_SDT].callback_fun)(meg_send);	
			return SYS_TABLE_NOERROR;
		}
		else if(complete_flag == BAT_UPDATE_COMPLETE)
		{
			complete_flag = BAT_UPDATE_NONE;
			first_bouqet = 0xffff;
			first_section_number = 0xffffffff;
			first_section = 1;
			memset(&meg_send, 0, sizeof(meg_send));
			meg_send.pid = pid;
			meg_send.section_length = sectionlength;
			meg_send.table_id = table_id;
			printf("_bg_BAT_Table(), pTable_complete, b_id=%d\n", bouquetid);
			meg_send.pfFreeBuffer=(tBm_FreeBufferFct)_bgFreeDataBuffer;
			meg_send.manage_type = pTable_update;
		   	 meg_send.msgType = pTable_complete;
			(*bg_SetFilterData[SYS_BG_SDT].callback_fun)(meg_send);
			return SYS_TABLE_NOERROR;
		}
	}
#endif
	return SYS_TABLE_CHECKDATA_ERROR;
}

#if 0
/******************************************************************************
* Function : 		_bg_BAT_Table
* parameters :	
*				data_buffer :  the buffer save the new data to dealwith
*				sectionlength :  the section length of the data
*				pid :  the Slot PID
*				slot :  the slot id
* Return :		
*				1. the data update
*				0. the data not update
*
* Description :	Deal with the receive data
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
static U32 _bg_BAT_Table(U8 *data_buffer, U32 sectionlength,U16 pid,tSFILTER_SlotId slot)
{	
	U8			new_table;
	U16 bouquet_id = 0xffff, complete_flag = 0;
	U32 	table_id= 0xffff, version= 0xffff;
	SYS_Table_Msg_t 	meg_send;

	new_table = _bg_check_bat_info(bat_version, data_buffer);
	if (new_table)
	{
		if( bg_SetFilterData[SYS_BG_BAT].first_section ==  1 )
		{
			bg_SetFilterData[SYS_BG_BAT].first_section = 0;
			complete_flag = 0;
			table_id = data_buffer[0];
			version = ( data_buffer[ 5 ] & 0x3e ) >> 1;
			bg_SetFilterData[SYS_BG_PAT].section_version
			bouquet_id = (data_buffer[3]<<8) + data_buffer[4];
	 	}
		else
		{
			if(bouquet_id == ((data_buffer[3]<<8) + data_buffer[4]))
			{
				complete_flag = 1;
			}
		}
		meg_send.buffer = data_buffer;
		meg_send.pid = pid;
		meg_send.section_length = sectionlength;
		meg_send.table_id = table_id;
		meg_send.version = version;
		meg_send.pfFreeBuffer=(tBm_FreeBufferFct)_bgFreeDataBuffer;
	 	if(((data_buffer[5]&0x3e)>>1) != version )
	 	{
	 		bg_SetFilterData[SYS_BG_BAT].first_section = 1;	
			return SYS_TABLE_CHECKDATA_ERROR;
	 	}
		if( complete_flag == 1 )
		{
			meg_send.manage_type = pTable_update;
    	      		meg_send.msgType = pTable_complete;
			bat_version = version;
			(*bg_SetFilterData[SYS_BG_BAT].callback_fun)(meg_send);	
//			OS_SemWait(bg_SetFilterData[SYS_BG_BAT].semaphore);
			return SYS_TABLE_NOERROR;
		}
		else
		{
			meg_send.manage_type = pTable_update;
	    	       meg_send.msgType = pTable_multi_section;
			(*bg_SetFilterData[SYS_BG_BAT].callback_fun)(meg_send);
//			OS_SemWait(bg_SetFilterData[SYS_BG_BAT].semaphore);
			return SYS_TABLE_NOERROR;
		}
	}	
	return SYS_TABLE_CHECKDATA_ERROR;
}
#endif
 /******************************************************************************
* Function : 		_bg_CAT_Table
* parameters :	
*				data_buffer :  the buffer save the new data to dealwith
*				sectionlength :  the section length of the data
*				pid :  the Slot PID
*				slot :  the slot id
* Return :		
*				1. the data update
*				0. the data not update
*
* Description :	Deal with the receive data
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
static U32 _bg_CAT_Table(U8 *data_buffer, U32 sectionlength, U16 pid, tSFILTER_SlotId slot)
{
	U32 		section_number, last_section_number;
	S32 	total_sections, j;
	U16 transport_stream_id=0xffff, original_network_id=0xffff;
	SYS_Table_Msg_t 	meg_send;
	U16   oldlen=0,sendflag =0;
	
	if(0x01 != data_buffer[0])
	{
		pbiinfo("_bg_CAT_Table---------------line %d",__LINE__);
		return SYS_TABLE_CHECKDATA_ERROR;
	}
	if((sectionlength <0)||(sectionlength>4096))
	{
		
		pbiinfo("_bg_CAT_Table---------------line %d",__LINE__);
		return SYS_TABLE_CHECKDATA_ERROR;

	}
	if(bg_SetFilterData[SYS_BG_CAT].callback_fun==NULL)
	{
		pbiinfo("_bg_CAT_Table---------------line %d",__LINE__);
		return SYS_TABLE_CHECKDATA_ERROR;
	}

	if(g_cat_len!=sectionlength)
	{
		sendflag =1;
	}
	else
	{
		int ii=0;
		for(ii =0;ii<g_cat_len;ii++ )
		{
			if(g_cat_buffer[ii]!=data_buffer[ii])
			{
				sendflag =1;
				break;
			}
		}

	}
	if(sendflag ==0)
	{
		return SYS_TABLE_CHECKDATA_ERROR;

	}

	
	pbiinfo("_bg_CAT_Table---------------send cat %d",__LINE__);
	meg_send.buffer = data_buffer;
	meg_send.pid = pid;
	meg_send.section_length = sectionlength;
	meg_send.table_id = bg_SetFilterData[SYS_BG_CAT].tableID;
	meg_send.version = bg_SetFilterData[SYS_BG_CAT].section_version ;
	meg_send.pfFreeBuffer=(tBm_FreeBufferFct)_bgFreeDataBuffer;
	section_number = data_buffer[ 6 ];
	meg_send.msgType = pTable_complete;//type no use
	(*bg_SetFilterData[SYS_BG_CAT].callback_fun)(meg_send); 
//		OS_SemWait(bg_SetFilterData[SYS_BG_CAT].semaphore);
	return SYS_TABLE_NOERROR;

	
	#if 0
	if((((data_buffer[5]&0x3e)>>1) != bg_SetFilterData[SYS_BG_CAT].section_version) || 
	((((data_buffer[3] << 8) + data_buffer[4]) != transport_stream_id) || 
	(((data_buffer[8] << 8) + data_buffer[9]) != original_network_id)))
	{
		bg_SetFilterData[SYS_BG_CAT].first_section = 1;
	}
	if(bg_SetFilterData[SYS_BG_CAT].first_section ==  1)
	{
		transport_stream_id = ((data_buffer[3] << 8) + data_buffer[4]);
		original_network_id = ((data_buffer[8] << 8) + data_buffer[9]);
		bg_SetFilterData[SYS_BG_CAT].first_section = 0;
		bg_SetFilterData[SYS_BG_CAT].section_version = ( data_buffer[ 5 ] & 0x3e ) >> 1;
		bg_SetFilterData[SYS_BG_CAT].tableID= data_buffer[0];
		last_section_number = data_buffer[ 7 ];

		for( j = 0; j < MAX_SECTION_NUM / 32; j ++ )
		{
			bg_SetFilterData[SYS_BG_CAT].sections_arrived[j] = 0;
			bg_SetFilterData[SYS_BG_CAT].all_sections_mask[j] = 0;
		}
		total_sections = last_section_number + 1;
		j = 0;
		while( total_sections >= 32 )
		{
			bg_SetFilterData[SYS_BG_CAT].all_sections_mask[j] = 0xffffffff;
			j ++;
			total_sections -=32;
		}
		if ( total_sections )
			bg_SetFilterData[SYS_BG_CAT].all_sections_mask[j] = ( ( 1 << total_sections ) - 1 ) ;
	}
	if(((data_buffer[5]&0x3e)>>1) != bg_SetFilterData[SYS_BG_CAT].section_version )
	{
		return SYS_TABLE_CHECKDATA_ERROR;
	}
	j = 0;
	while( section_number >= 32 )
	{
		j ++;
		section_number -=32;
	}

	if( bg_SetFilterData[SYS_BG_CAT].sections_arrived[ j ] &  ( 1 << ( section_number ) ) )
	{
		return SYS_TABLE_CHECKDATA_ERROR;
	}
	bg_SetFilterData[SYS_BG_CAT].sections_arrived[j] |= (1<<(section_number));

	for( j = 0; j <	MAX_SECTION_NUM / 32; j ++ )
	{
		if ( bg_SetFilterData[SYS_BG_CAT].sections_arrived[ j ] != bg_SetFilterData[SYS_BG_CAT].all_sections_mask[ j ] ) 
			break;
	}
	if (j == MAX_SECTION_NUM / 32)
	{
		meg_send.msgType = pTable_complete;
		(*bg_SetFilterData[SYS_BG_CAT].callback_fun)(meg_send);	
//		OS_SemWait(bg_SetFilterData[SYS_BG_CAT].semaphore);
		return SYS_TABLE_NOERROR;
	}
	else
	{
		meg_send.msgType = pTable_multi_section;
		(*bg_SetFilterData[SYS_BG_CAT].callback_fun)(meg_send);	
//		OS_SemWait(bg_SetFilterData[SYS_BG_CAT].semaphore);
		return SYS_TABLE_NOERROR;
	}
	return SYS_TABLE_CHECKDATA_ERROR;
	#endif
}

 /******************************************************************************
* Function : 		_bg_TDT_Table
* parameters :	
*				data_buffer :  the buffer save the new data to dealwith
*				sectionlength :  the section length of the data
*				pid :  the Slot PID
*				slot :   the slot id
* Return :		
*				1. the data update
*				0. the data not update
*
* Description :	Deal with the receive data
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
static U32 _bg_TDT_Table(U8 *data_buffer, U32 sectionlength,U16 pid,tSFILTER_SlotId 	slot)
{
	U32 	table_id= 0xffff;
	SYS_Table_Msg_t 	meg_send;
	
	if((0x70 != data_buffer[0]) && (0x73 != data_buffer[0]))
	{
		return SYS_TABLE_CHECKDATA_ERROR;
	}
	table_id = data_buffer[0];
	meg_send.buffer = data_buffer;
	meg_send.pid = pid; 
	meg_send.section_length = sectionlength;
	meg_send.table_id = table_id;
	meg_send.pfFreeBuffer=(tBm_FreeBufferFct)_bgFreeDataBuffer;
	meg_send.msgType = pTable_complete;
	(*bg_SetFilterData[SYS_BG_TDT].callback_fun)(meg_send);	
//	OS_SemWait(bg_SetFilterData[SYS_BG_TDT].semaphore);
	return SYS_TABLE_NOERROR;
}

/******************************************************************************
* Function : 		_bg_EIT_Table
* parameters :	
*				data_buffer :  the buffer save the new data to dealwith
*				sectionlength :  the section length of the data
*				pid :  the Slot PID
*				slot :  the slot id
* Return :		
*				1. the data update
*				0. the data not update
*
* Description :	Deal with the receive data
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
static U32 _bg_EIT_Table(U8 *data_buffer, U32 sectionlength,U16 pid, tSFILTER_SlotId 	slot)
{
	U32 	version,table_id;
	SYS_Table_Msg_t 	meg_send;
	
	if(((0x4f != data_buffer[0]) && (0x4e != data_buffer[0]) && (0x50 != data_buffer[0])
		 && (0x51 != data_buffer[0]) && (0x60 != data_buffer[0]) && (0x61 != data_buffer[0]))||(sectionlength < 30))
	{
		return SYS_TABLE_CHECKDATA_ERROR;
	}
	table_id = data_buffer[0];
	version = ( data_buffer[ 5 ] & 0x3e ) >> 1;
	meg_send.buffer = data_buffer;
	meg_send.pid = pid;
	meg_send.section_length = sectionlength;
	meg_send.table_id = table_id;
	meg_send.version = version;	
	meg_send.pfFreeBuffer=(tBm_FreeBufferFct)_bgFreeDataBuffer;
	meg_send.msgType = pTable_multi_section;
	meg_send.manage_type = pTable_update;
	(*bg_SetFilterData[SYS_BG_EIT].callback_fun)(meg_send);
//	OS_SemWait(bg_SetFilterData[SYS_BG_EIT].semaphore);	
	return SYS_TABLE_NOERROR;
}

/******************************************************************************
* Function : 		_bg_PMT_Table
* parameters :	
*				data_buffer :  the buffer save the new data to dealwith
*				sectionlength :  the section length of the data
*				pid :  the Slot PID
*				slot :  the slot id
* Return :		
*				1. the data update
*				0. the data not update
*
* Description :	Deal with the receive data
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
static U32 _bg_PMT_Table(U8 *data_buffer, U32 sectionlength,U16 pid, tSFILTER_SlotId 	slot)
{	
	U32 	version = 0xffff, table_id = 0xffff;
	SYS_Table_Msg_t 	meg_send;

	table_id = data_buffer[0];
	version = (data_buffer[5] & 0x3e) >> 1; 	
	// {{{ ��PMT��First_section��ʱ�� ʹ�õ���Zapping
	if(0x02 != table_id)
	{
		return SYS_TABLE_CHECKDATA_ERROR;
	}
//printf(" %s %d bg_SetFilterData[SYS_BG_PMT].first_section  =%d \n",__FILE__,__LINE__,bg_SetFilterData[SYS_BG_PMT].first_section);
	if (bg_SetFilterData[SYS_BG_PMT].first_section)
	{
		if (bg_SetFilterData[SYS_BG_PMT].callback_fun != NULL)
		{			
			bg_SetFilterData[SYS_BG_PMT].first_section = 0;
			bg_SetFilterData[SYS_BG_PMT].section_version = version;
			meg_send.buffer = data_buffer;
			meg_send.pid = pid;
			meg_send.section_length = sectionlength;
			meg_send.table_id = table_id;
			meg_send.version = version;
			meg_send.pfFreeBuffer = (tBm_FreeBufferFct)_bgFreeDataBuffer;
			meg_send.manage_type = pTable_scan;
			meg_send.msgType = pTable_complete;
			bg_SetFilterData[SYS_BG_PMT].callback_fun(meg_send);
			OS_SemWait(bg_SetFilterData[SYS_BG_PMT].semaphore);
			//printf(" %s %d bg_SetFilterData[SYS_BG_PMT].first_section  =%d \n",__FILE__,__LINE__,bg_SetFilterData[SYS_BG_PMT].first_section);
		}
		else
		{
			return SYS_TABLE_CHECKDATA_ERROR;
		}
	}
// }}} ��PMT��First_section��ʱ�� ʹ�õ���Zapping
	else
	{
		if(version != bg_SetFilterData[SYS_BG_PMT].section_version)
		{
			//printf("pmt new version = %d, old version = %d.\n", version, bg_SetFilterData[SYS_BG_PMT].section_version);
			bg_SetFilterData[SYS_BG_PMT].section_version = version;
			if (bg_SetFilterData[SYS_BG_PMT].callback_fun != NULL)
			{
				meg_send.buffer = data_buffer;
				meg_send.pid = pid;
				meg_send.section_length = sectionlength;
				meg_send.table_id = table_id;
				meg_send.version = version;		
				meg_send.manage_type = pTable_update;
				meg_send.msgType = pTable_complete;
				meg_send.pfFreeBuffer = (tBm_FreeBufferFct)_bgFreeDataBuffer;
				bg_SetFilterData[SYS_BG_PMT].callback_fun(meg_send);	
				OS_SemWait(bg_SetFilterData[SYS_BG_PMT].semaphore);
			}
		}
		else
		{
			return SYS_TABLE_CHECKDATA_ERROR;
		}
	}

	return SYS_TABLE_NOERROR;
}

 /******************************************************************************
* Function : 		_bg_check_pat_info
* parameters :	
*				param_infor :  the Tp Param Info
*				pbuffer :  the new data buffer
* Return :		
*				1. need to Update pat
*				0. need not to updata pat
*
* Description :	Check Update pat data YES/NO
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
static U8 _bg_check_pat_info(SYS_BG_TPParam_t param_infor, U8 *pbuffer)
{
	U8 version;
	U16 transport_stream_id;

	transport_stream_id = ((pbuffer[3] << 8) + pbuffer[4]);
	version = (pbuffer[ 5 ] & 0x3e) >> 1;
	if ((param_infor.usPATversion != version) && (param_infor.usTSid == transport_stream_id))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

 /******************************************************************************
* Function : 		_bg_check_nit_info
* parameters :	
*				param_infor :  the Tp Param Info
*				pbuffer :  the new data buffer
* Return :		
*				1. need to Update nit
*				0. need not to updata nit
*
* Description :	Check Update nit data YES/NO
*
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
static U8  _bg_check_nit_info(SYS_BG_TPParam_t param_infor, U8 *pbuffer,U16 section_length)
{
	U16 version;

	version = ( pbuffer[ 5 ] & 0x3e ) >> 1;
	return 1;
	if(Boot_FirstNIT)
	{
		bg_SetFilterData[SYS_BG_NIT].first_section = 1;
		Boot_FirstNIT = FALSE;
		return 1;
	}
	//printf("ZDW---->_bg_check_nit_info---NEWVER = %d, OLDVER = %d\n",version,param_infor.usNITversion);
	if ((param_infor.usNITversion== version) && (bg_SetFilterData[SYS_BG_NIT].first_section != 0))
	{
		//prinf("ZDW--->>NIT NOT UPDATE!!!");
		return 0;
	}
	else
	{
		//prinf("ZDW--->>NIT UPDATE!!!");
		return 1;
	}
}

/******************************************************************************
* Function : 		_bg_check_sdt_info
* parameters :	
*				param_infor - the Tp Param Info
*				pbuffer - the new data buffer
* Return :		
*				1. need to Update sdt
*				0. need not to updata sdt
* Description :	Check Update sdt data YES/NO
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
static U8 _bg_check_sdt_info(SYS_BG_TPParam_t param_infor, U8 *pbuffer)
{
	U16 transport_stream_id, original_network_id,version;

	transport_stream_id = ((pbuffer[3] << 8) + pbuffer[4]);
	original_network_id = ((pbuffer[8] << 8) + pbuffer[9]);
	version = ( pbuffer[ 5 ] & 0x3e ) >> 1;	
	if (param_infor.usONid == original_network_id && param_infor.usTSid == transport_stream_id)
	{
		if (param_infor.usSDTversion == version)
		{
			return 0;
		}
		else
		{
			return 1;
		}		
	}
	return 0;
}
#if 0
 /******************************************************************************
* Function : 		_bg_check_bat_info
* parameters :	
*				cur_version - the Tp Param 
*				pbuffer - the new data buffer
* Return :		
*				1. need to Update bat
*				0. need not to updata bat
* Description :	Check Update bat data YES/NO
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
static U8 _bg_check_bat_info(U16 cur_version, U8 *pbuffer)
{
	U8 new_version = 0;
	
	new_version = ( pbuffer[ 5 ] & 0x3e ) >> 1;
	if(cur_version == new_version)
	{
		return 0;
	}
	else		
	{
		return 1;
	}
}
#endif
 /******************************************************************************
* Function : 		_bg_check_Type_Byslotid
* parameters :	slotid - the Slot ID
* Return :		SYS_BG_Type_e - the type of the data return
* Description :	Get the CallBack Return Data Type
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
static SYS_BG_Type_e _bg_check_Type_Byslotid(U16 slotid)
{
	U16 Ii;
	for (Ii=0;Ii<BG_MAX_FILTER;Ii++)
	{
		if (slotid==bg_SetFilterData[Ii].slotID)
			break;
	}
	return bg_SetFilterData[Ii].bg_type;
}

 /******************************************************************************
* Function : 		Start_Background
* parameters :	void
* Return :		
*				1 - Start PAT\NIT\SDT Background Failed
*				0 - Start PAT\NIT\SDT Background succeed
* Description :
*				Start the PAT\NIT\SDT Background
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
U32 Start_Background(void)
{
	SYS_Table_Param_t  tempSetFilter;
	U32 ret = SYS_TABLE_NOERROR;
	
	memset( &tempSetFilter, 0, sizeof(SYS_Table_Param_t));
	tempSetFilter.timeout = ONESEC;
    //SYS_BackGround_Bat_Default(); /* close by zxguan 2013-02-28 */

	if(ret == SYS_TABLE_NOERROR)
	{
		ret = SYS_BackGround_TableStart(SYS_BG_NIT, tempSetFilter);	
		//printf("Start_Background %d ret = %d\n",__LINE__,ret);
		if (ret != SYS_TABLE_NOERROR)
		{
			DB_BG((TRC_INFO,"===BackGround:    NIT BG Start Failed\n\n"));	
		}
	}

	/*{{{ deleted by ZYJ 2011-04-03 for �����������?*/
#if 0
	ret = SYS_BackGround_TableStart(SYS_BG_PAT, tempSetFilter);
	//printf("Start_Background %d ret = %d\n",__LINE__,ret);
	if (ret != SYS_TABLE_NOERROR)
	{
		DB_BG((TRC_INFO,"===BackGround:    PAT BG Start Failed\n\n"));	
	}		
#endif
	/*}}} deleted by ZYJ 2011-04-03 for �����������?*/

	if(ret == SYS_TABLE_NOERROR)
	{
		ret = SYS_BackGround_TableStart(SYS_BG_SDT, tempSetFilter);
		//printf("Start_Background %d ret = %d\n",__LINE__,ret);
		if (ret != SYS_TABLE_NOERROR)
		{
			printf("Start_Background %d\n",__LINE__);
			DB_BG((TRC_INFO,"===BackGround:    SDT BG Start Failed\n\n"));	
		}
	}
	return ret;
}

 /******************************************************************************
* Function : 		Stop_Background
* parameters :	void
* Return :		
*				1 - Stop PAT\NIT\SDT Background Failed
*				0 - Stop PAT\NIT\SDT Background succeed
* Description :
*				Stop the  PAT\NIT\SDT Background
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
U32 Stop_Background(void)
{
	U32 ret = SYS_TABLE_NOERROR;
	
	/*{{{ deleted by ZYJ 2011-04-03 for �����������?*/
#if 0
	ret = SYS_BackGround_TableStop(SYS_BG_PAT);
	//printf("Stop_Background %d %d\n",__LINE__,ret);
	if (ret != SYS_TABLE_NOERROR)
	{
		DB_BG((TRC_INFO,"===BackGround:    PAT BG Stop Failed\n\n"));	
	}
#endif
	/*}}} deleted by ZYJ 2011-04-03 for �����������?*/

	ret = SYS_BackGround_TableStop(SYS_BG_NIT);
	//printf("Stop_Background %d %d\n",__LINE__,ret);
	if (ret != SYS_TABLE_NOERROR)
	{
		DB_BG((TRC_INFO,"===BackGround:    NIT BG Stop Failed\n\n"));	
	}
	
	ret = SYS_BackGround_TableStop(SYS_BG_SDT);
	//printf("Stop_Background %d %d\n",__LINE__,ret);
	if (ret != SYS_TABLE_NOERROR)
	{
		DB_BG((TRC_INFO,"===BackGround:    SDT BG Stop Failed\n\n"));
	}

    ret = SYS_BackGround_TableStop(SYS_BG_EIT);
	if (ret != SYS_TABLE_NOERROR)
	{
		DB_BG((TRC_INFO,"===BackGround:    EIT BG Stop Failed\n\n"));	
	}
#if 1
    ret = SYS_BackGround_TableStop(SYS_BG_TDT);
	if (ret != SYS_TABLE_NOERROR)
	{
		DB_BG((TRC_INFO,"===BackGround:    TDT BG Stop Failed\n\n"));	
	}
#endif
        ret = SYS_BackGround_TableStop(SYS_BG_PMT);
	if (ret != SYS_TABLE_NOERROR)
	{
		DB_BG((TRC_INFO,"===BackGround:    TDT BG Stop Failed\n\n"));	
	}
    ret = SYS_BackGround_TableStop(SYS_BG_CAT);
	if (ret != SYS_TABLE_NOERROR)
	{
		DB_BG((TRC_INFO,"===BackGround:    CAT BG Stop Failed\n\n"));	
	}
    
	return ret;
}

 /******************************************************************************
* Function : 		Pause_background
* parameters :	void
* Return :		
*				1 - Pause PAT\NIT\SDT Background Failed
*				0 - Pause PAT\NIT\SDT Background succeed
* Description :
*				Pause the  PAT\NIT\SDT Background
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
U32	Pause_background(void)
{
	U32 ret = SYS_TABLE_NOERROR;

	/*{{{ deleted by ZYJ 2011-04-03 for �����������?*/
	#if 0
	ret = SYS_BackGround_TablePause(SYS_BG_PAT);
	if (ret != SYS_TABLE_NOERROR)
	{
		DB_BG((TRC_INFO,"===BackGround:    PAT BG Pause Failed\n\n"));	
	}
	#endif
	/*}}} deleted by ZYJ 2011-04-03 for �����������?*/

	if(ret == SYS_TABLE_NOERROR)
	{
		ret = SYS_BackGround_TablePause(SYS_BG_NIT);
		if (ret != SYS_TABLE_NOERROR)
		{
			DB_BG((TRC_INFO,"===BackGround:    NIT BG Pause Failed\n\n"));	
		}
	}
	if(ret == SYS_TABLE_NOERROR)
	{
		ret = SYS_BackGround_TablePause(SYS_BG_SDT);	
		if (ret != SYS_TABLE_NOERROR)
		{
			DB_BG((TRC_INFO,"===BackGround:    NIT BG Pause Failed\n\n"));	
		}
	}
	return ret;
}

 /******************************************************************************
* Function : 		Resume_Background
* parameters :	void
* Return :				
*				1 - Resume PAT\NIT\SDT Background Failed
*				0 - Resume PAT\NIT\SDT Background succeed
* Description :
*				Resume the  PAT\NIT\SDT Background
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
U32 Resume_Background(void)
{
	U32 ret = SYS_TABLE_NOERROR;
	
	/*{{{ deleted by ZYJ 2011-04-03 for �����������?*/
#if 0
	ret = SYS_BackGround_TableResume(SYS_BG_PAT);
	if (ret != SYS_TABLE_NOERROR)
	{
		DB_BG((TRC_INFO,"===BackGround:    PAT BG Resume Failed\n\n"));	
	}		
#endif
	/*}}} deleted by ZYJ 2011-04-03 for �����������?*/

	if(ret == SYS_TABLE_NOERROR)
	{
		ret = SYS_BackGround_TableResume(SYS_BG_NIT);
		if (ret != SYS_TABLE_NOERROR)
		{
			DB_BG((TRC_INFO,"===BackGround:    NIT BG Resume Failed\n\n"));	
		}
	}
	if(ret == SYS_TABLE_NOERROR)
	{
		ret = SYS_BackGround_TableResume(SYS_BG_SDT);
		if (ret != SYS_TABLE_NOERROR)
		{
			DB_BG((TRC_INFO,"===BackGround:    NIT BG Resume Failed\n\n"));	
		}
	}
	return ret;
}

 /******************************************************************************
* Function : 		_get_program_info
* parameters :	tp_parameters - the Tp parameter
* Return :		void
* Description :	get the Tp parameter
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
void _get_program_info(SYS_BG_TPParam_t *tp_parameters)
{
	tp_parameters->usNid =	g_tp_parameters.usNid;
	tp_parameters->usTSid=g_tp_parameters.usTSid;
	tp_parameters->usONid=g_tp_parameters.usONid;
	tp_parameters->usPATversion=g_tp_parameters.usPATversion;
	tp_parameters->usSDTversion=g_tp_parameters.usSDTversion;
	tp_parameters->usNITversion=g_tp_parameters.usNITversion;
}

 /******************************************************************************
* Function : 		Set_program_info
* parameters :	tp_parameters - the Tp parameter
* Return :		void
* Description :	set the Tp parameter
* Author : 		pxwang	2008/03/26
******************************************************************************/ 
void Set_program_info(SYS_BG_TPParam_t *tp_parameters)
{
	g_tp_parameters.usNid =	tp_parameters->usNid;
	g_tp_parameters.usTSid=tp_parameters->usTSid;
	g_tp_parameters.usONid=tp_parameters->usONid;
	g_tp_parameters.usPATversion=tp_parameters->usPATversion;
	g_tp_parameters.usSDTversion=tp_parameters->usSDTversion;
	g_tp_parameters.usNITversion=tp_parameters->usNITversion;
}

SYS_BG_TPParam_t Get_program_info( void )
{
	return g_tp_parameters;
}

U32 g_semCAT_CallBack = 0xFFFFFFFF;
U16 g_CAT_Table_Version = 0xFFFF;
U16 g_NIT_Table_Version = 0xFFFF;
void CatTableVersionClaer()
{
    g_CAT_Table_Version = 0xFFFF;
}
void CatTableReset()
{
	//memset(g_cat_buffer,0xff,4096);
	g_cat_len =0;

}
void Cat_notify_CallBack(SYS_Table_Msg_t message)
{
    S16 ret = 0;
    U8 version = 0xFFFF;
	U16 len=0;
    DVBPlayer_Event_t tEvent;
	if(DVBPlayer_Get_RunningStatus()!=eDVBPLAYER_RUNNING_STATUS_PLAY)
	{
		//pbiinfo("Cat_notify_CallBack----------dvb not player --return");
		message.pfFreeBuffer(message.buffer);
   	    //OS_SemSignal(g_semCAT_CallBack);
		return;
	}

	
	memset(g_cat_buffer,0xff,4096);
	g_cat_len =message.section_length;
	memcpy(g_cat_buffer,message.buffer,g_cat_len);
	
    memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
        g_CAT_Table_Version= version;
        tEvent.eEventType = 1;
       tEvent.pEventData = message.buffer;
	pbiinfo("\n_________________ send cat to trcas ________________\n");
        CA_DVBPlay_CallBack( tEvent );
    message.pfFreeBuffer(message.buffer);
   // OS_SemSignal(g_semCAT_CallBack);
}
void background_cat_init()
{
	int error;
	SYS_Table_InitParam_t init_parameter;
	g_CAT_Table_Version = 0xFFFF;
	memset(g_cat_buffer,0xff,4096);
	g_cat_len =0;
    OS_SemCreate(&(g_semCAT_CallBack), (const char *)"g_semPMT_CallBack", 0, 0);
    init_parameter.p_semaphore = g_semCAT_CallBack;
    init_parameter.callback= Cat_notify_CallBack;
	error = SYS_BackGround_TableInit(SYS_BG_CAT,init_parameter);
	if (error)
		pbiinfo("Background_CAT_Init error !!!!!!!\n");
	else
		pbiinfo("Background_CAT_Init ok !!!!!!!\n");
}
void Background_Cat_Stop()
{
    SYS_BackGround_TableStop(SYS_BG_CAT);
}
void Nit_notify_CallBack(SYS_Table_Msg_t message)
{
    S16 ret = 0;
    U8 version = 0xFFFF;
	U16 len=0;
    DVBPlayer_Event_t tEvent;
	if(DVBPlayer_Get_RunningStatus()!=eDVBPLAYER_RUNNING_STATUS_PLAY)
	{
		//pbiinfo("Cat_notify_CallBack----------dvb not player --return");
		message.pfFreeBuffer(message.buffer);
		OS_SemSignal(bg_SetFilterData[SYS_BG_NIT].semaphore);
		return;
	}

	
	memset(g_nit_buffer,0xff,4096);
	g_nit_len =message.section_length;
	memcpy(g_nit_buffer,message.buffer,g_nit_len);
	
    memset( &tEvent, 0, sizeof(DVBPlayer_Event_t) );
        g_NIT_Table_Version= version;
        tEvent.eEventType = 7;
       tEvent.pEventData = message.buffer;
	pbiinfo("\n_________________ send NIT to cticas ________________\n");
        CA_DVBPlay_CallBack( tEvent );
    message.pfFreeBuffer(message.buffer);
    OS_SemSignal(bg_SetFilterData[SYS_BG_NIT].semaphore);
	
}
void background_nit_init()
{
	int error;
	SYS_Table_InitParam_t init_parameter;
	nit_version= 0xFFFF;
    OS_SemCreate(&(g_semNIT_CallBack), (const char *)"g_semNIT_CallBack", 0, 0);
    init_parameter.p_semaphore = g_semNIT_CallBack;
    init_parameter.callback= Nit_notify_CallBack;
	error = SYS_BackGround_TableInit(SYS_BG_NIT,init_parameter);
	if (error)
		pbiinfo("Background_nit_Init error !!!!!!!\n");
	else
		pbiinfo("Background_nit_Init ok !!!!!!!\n");
}
void Nit_RegMessage_Notify_CallBack(STB_NITMsg_Notify Cb)
{
	g_nitver_update_cb = Cb;
}
void Nit_UnRegMessage_Notify_CallBack(void)
{
	g_nitver_update_cb = NULL;
}

void DL_Nit_RegMessage_Notify_CallBack(STB_NITMsg_Notify Cb)
{
	g_nit_download_cb = Cb;
}
void DL_Nit_UnRegMessage_Notify_CallBack(void)
{
	g_nit_download_cb = NULL;
}


