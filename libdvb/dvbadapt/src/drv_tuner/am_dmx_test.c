/***************************************************************************
 *  Copyright C 2009 by Amlogic, Inc. All Rights Reserved.
 */
/**\file
 * \brief è§£å¤ç”¨è®¾å¤‡æµ‹è¯•ç¨‹åº
 *
 * \author Gong Ke <ke.gong@amlogic.com>
 * \date 2010-06-07: create the document
 ***************************************************************************/

#define AM_DEBUG_LEVEL 5

#include <am_debug.h>
#include <am_dmx.h>
//#include <am_fend.h>
#include <string.h>
#include <unistd.h>
#include "drv_filter.h"

#define DMX_DEV_NO    (0)

#define AM_TRY(_func) \
	do {\
	AM_ErrorCode_t _ret;\
	printf("[zshang][%s][%d]\n", __FUNCTION__, __LINE__);\
	if ((_ret=(_func))!=AM_SUCCESS)\
		printf("[zshang][%d]\n", _ret);\
	} while(0)

#define MAX_SECTION_NUM         (256)
#define MAX_SECTION_LENGTH      (4096)

#define	BG_DEBUG					0


/*LSZ 2010/04/16ĞŞ¸ÄĞÅºÅÁ¿Ã»ÓĞ³õÊ¼»¯ÎÊÌâ*/
/**********************************************************************/
/*							Private Constants						*/
/**********************************************************************/
#define	BG_MAX_FILTER					8		//Ö§³ÖµÄBackGroundÊıÁ¿
#define	BG_SLOT_PAUSE					2		//BackGroundµÄ×´Ì¬BackGroundÒÑ¾­Startµ«ÊÇÒÑ¾­Pause
#define	BG_SLOT_USE					1		//BackGroundµÄ×´Ì¬BackGroundÒÑ¾­Start
#define	BG_SLOT_IDLE					0		//BackGroundµÄ×´Ì¬BackGroundÒÑ¾­Stop
#define	FILTER_DEPTH_SIZE                       16
#define NO_OF_SIMULTANEOUS_FILTERS			48

#define PV_SUCCESS          (0)
#define PV_FAILURE          (-1)
#define PV_INVALID_PARAM    (-2)
#define PV_NULL_PTR         (-3)
#define PV_NO_MEMORY        (-4)

 /**********************************************************************/
 /*							Private Types									*/
 /**********************************************************************/

typedef enum
{
 	pTable_scan=0,
	pTable_update
}SYS_Table_Manage_Type_e;

typedef enum
{
	pTable_timeout = 0,
	pTable_stop,
	pTable_multi_section,
 	pTable_complete,
 	pTable_section_repeat
}SYS_Table_Msg_Type_e;
typedef  void (*tBm_FreeBufferFct) (void *Ptr);

typedef struct 
{
	U32 		pid;
	U32 		table_id;
	U32 		pn;
	clock_t	timeout;
} SYS_Table_Param_t;

typedef struct
{
	U8 							*buffer;
	U32							section_length;
	U32							pid;
	U32							table_id;
	U32							version;
	SYS_Table_Msg_Type_e  		msgType;
	SYS_Table_Manage_Type_e		manage_type;
	tBm_FreeBufferFct				pfFreeBuffer;
}SYS_Table_Msg_t;




typedef void (* table_notify_fct_t)(SYS_Table_Msg_t message);

typedef struct 
{
	table_notify_fct_t 	callback;
	U32 			p_semaphore;	
} SYS_Table_InitParam_t;

typedef enum
{
	SYS_BG_PAT,                  						//BackGroundÀàĞÍPAT        		
	SYS_BG_NIT,                         					//BackGroundÀàĞÍNIT 
	SYS_BG_SDT,                      					//BackGroundÀàĞÍSDT  
	SYS_BG_BAT,								//BackGroundÀàĞÍBAT  
	SYS_BG_CAT,								//BackGroundÀàĞÍCAT  
	SYS_BG_TDT,								//BackGroundÀàĞÍTDT  
	SYS_BG_EIT,									//BackGroundÀàĞÍEIT
	SYS_BG_PMT,   			  					//BackGroundÀàĞÍPMT  
	SYS_BG_NOTABLE							//BackGroundÀàĞÍNULL  
}SYS_BG_Type_e;		//BackGroundµÄÀàĞÍ

typedef struct
{
	U8 						status; 				//±êÊ¶BackGroundµÄ×´Ì¬ 
	tSFILTER_FilterId 			filterID;				//±êÊ¶BackGroundµÄFilterID
	U32						filterPID;			//ÉèÖÃBackGroundÒª¸üĞÂÊı¾İµÄPID
	U32 						tableID;				//ÉèÖÃBackGroundÒª¹ıÂËÊı¾İµÄTableID
	SYS_BG_Type_e 			bg_type;			//±êÊ¶BackGroundµÄÀàĞÍ
	U32 						semaphore;			//×¢²á¸øBackGroundµÄĞÅºÅÁ¿£¬±£»¤·µ»ØÊı¾İ
	U32 						first_section;		//±êÊ¶µ±Ç°¸üĞÂµÄÊÇ·ñÊÇFirstSection
	U32						section_version;		//±êÊ¶µ±Ç°BackGroundµÄSectionºÅ
	table_notify_fct_t			callback_fun;		//×¢²á¸ø¸ÃBackGroundµÄÉÏ²ãCallBack
	U32						sections_arrived[MAX_SECTION_NUM/32];
	U32						all_sections_mask[MAX_SECTION_NUM/32];
}BG_SetFilter_t;

enum
{
	SYS_TABLE_NOERROR=0,                       		//Ã»ÓĞ´íÎó
	SYS_TABLE_FILTFR_ERROR,     			//Filter FreeµÄ´íÎó
	SYS_TABLE_FILTALLO_ERROR,                      //Filter AllocateµÄ´íÎó
	SYS_TABLE_FILTCON_ERROR,				//Filter ControlµÄ´íÎó
	SYS_TABLE_FILTSET_ERROR,				//Filter SetµÄ´íÎó
	SYS_TABLE_FILTASSO_ERROR,			//Filter AssociateµÄ´íÎó
	SYS_TABLE_CHANFR_ERROR,				//Channel FreeµÄ´íÎó
	SYS_TABLE_CHANALLO_ERROR,			//Channel AllocateµÄ´íÎó
	SYS_TABLE_CHANCON_ERROR,				//Channel ControlµÄ´íÎó
	SYS_TABLE_CHANSET_ERROR,				//Channel SetµÄ´íÎó
	SYS_TABLE_LARTHANMAX_ERROR,			//BackGround ÊıÁ¿¹ı´óµÄ´íÎó
	SYS_TABLE_NORUNNING,					//BackGroundÃ»ÓĞÔËĞĞµÄ´íÎó
	SYS_TABLE_CHECKDATA_ERROR,			//BackGround CheckDataµÄ´íÎó
	SYS_TABLE_UNKNOWN					//Î´ÖªµÄ´íÎó
};						//´íÎóµÄÀàĞÍ





 /**********************************************************************/
 /*							Global Variables								*/
 /**********************************************************************/
 /**********************************************************************/
 /*							Private Variables (static)						*/
 /**********************************************************************/
static BG_SetFilter_t			bg_SetFilterData[BG_MAX_FILTER];	//BackGroundµ±ÖĞµÄ²ÎÊı

void _bg_SetFilterDataToDef(U32  Back_Type)
{	
	bg_SetFilterData[Back_Type].status = BG_SLOT_IDLE;
	bg_SetFilterData[Back_Type].filterID = 			NO_OF_SIMULTANEOUS_FILTERS;
	bg_SetFilterData[Back_Type].filterPID = 		0xFFFF;
	bg_SetFilterData[Back_Type].tableID = 		0xFFFF;
	bg_SetFilterData[Back_Type].bg_type= SYS_BG_NOTABLE;
	bg_SetFilterData[Back_Type].first_section=1;
	bg_SetFilterData[Back_Type].section_version = 0xFFFF;
	bg_SetFilterData[Back_Type].callback_fun = NULL;
}

void SYS_BackGround_Init(void)
{
	U32	iI = 0;
	C8 buffer[20];

	for(iI = 0 ; iI < BG_MAX_FILTER; iI++) //³õÊ¼»¯ÓĞËùBackGroundµÄÊı¾İ
	{
	    memset( buffer, 0, sizeof(buffer));
		_bg_SetFilterDataToDef(iI);
	}
}


static void _bg_FilterInfoCallBack(int dev_no, int fhandle, const uint8_t *buffer, int length, void *user_data)
{
	SYS_BG_Type_e	iI = SYS_BG_NOTABLE;
	U16 pid = 0x1FFF;	
	printf("*********_bg_FilterInfoCallBack=>the length = %d,buffer[0] = %d,the pid = %d, type = %d\n",length,buffer[0],pid, iI);
	
}

U32 SYS_BackGround_TableInit(SYS_BG_Type_e Back_Type, SYS_Table_InitParam_t init)
{
	U32 ret = SYS_TABLE_NOERROR;	
	SYS_Table_Param_t bgSetFilter ;
	
	memset( &bgSetFilter, 0, sizeof(SYS_Table_Param_t));
/*LSZ 2010/04/16ĞŞ¸ÄĞÅºÅÁ¿Ã»ÓĞ³õÊ¼»¯ÎÊÌâ*/
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
			//Boot_FirstNIT = TRUE;
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
	return ret;
}

U32 SYS_BackGround_TableStart(SYS_BG_Type_e Back_Type , SYS_Table_Param_t bgfilterdata)
{
	U8 Data[FILTER_DEPTH_SIZE] = {0};
	U8 Mask[FILTER_DEPTH_SIZE] = {0};
	U8 scFilterNegate[FILTER_DEPTH_SIZE] = {0};
	U32	iI = 0, ret = SYS_TABLE_UNKNOWN, bNoError = 1, bChannelFlag = 1;
	tSFILTER_FilterId filterid = -1;	
	memset(Data, 0, FILTER_DEPTH_SIZE);
	memset(Mask, 0, FILTER_DEPTH_SIZE);
	memset(scFilterNegate, 0, FILTER_DEPTH_SIZE);
	if (Back_Type != SYS_BG_PMT)
	{
		bgfilterdata.pid = bg_SetFilterData[Back_Type].filterPID;
		bgfilterdata.table_id = bg_SetFilterData[Back_Type].tableID;
		printf("[zshang]pid:%d, table_id:%d\n", bgfilterdata.pid, bgfilterdata.table_id);
	}
	else
	{
		bg_SetFilterData[Back_Type].first_section = 1;
	}

	filterid = SFILTER_Check_DataPID(bgfilterdata.pid); 
	printf("bgfilterdata.pid = %d filterid = %d\n",bgfilterdata.pid, filterid);
	if(filterid >= 0)
	{
		if (SFILTER_Free_Filter(filterid) < 0)
		{
			printf("warning: diable channel failed.\n");
			ret = SYS_TABLE_FILTFR_ERROR;
		}
	}
	printf("SYS_BackGround_TableStart (), Back_Type = %d, slotIndex = %d\n", Back_Type, filterid);
	if(bNoError==1)
	{
		//printf("SYS_BackGround_TableStart %d\n",__LINE__);
		bNoError = 0;	
		S8 scDemuxError;
		filterid = SFILTER_Allocate_Filter();
		printf("[zshang]filterid:%d\n", filterid);
		if(filterid<0)
		{			
			ret = SYS_TABLE_FILTALLO_ERROR;
			printf(("===BackGround:   Filter allocate error\n"));
		}
		else
		{
			SFilterCallback_t callbacks;
			callbacks.pFCallBack = (tSFILTER_SlotCallbackFct)_bg_FilterInfoCallBack;
			callbacks.pFGetBuffer = NULL;
			callbacks.pFFreeBuffer = NULL;
			SFILTER_RegisterCallBack_Function(filterid,	&callbacks);
            printf("===BackGround:   slot allocate Successful\n");
			if(bgfilterdata.table_id> 0)
			{	
				printf("[zshang 1]SYS_BackGround_TableStart %d\n",__LINE__);
				if ((Back_Type == SYS_BG_EIT)|| (Back_Type == SYS_BG_SDT)) //¶ÔÓÚSDT&EITµÄBackGround²»Ö¹Ò»¸öTableIDµÄ´¦Àí
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
			scDemuxError = SFILTER_SetFilter(filterid, FILTER_DEPTH_SIZE, Mask, Data, scFilterNegate, bgfilterdata.pid);
			if(scDemuxError<0)
			{
				SFILTER_Free_Filter(filterid);
				ret = SYS_TABLE_FILTSET_ERROR;
				printf("SYS_BackGround_TableStart SFILTER_SetFilter error %d\n",scDemuxError);
			}
			bNoError= 1;
		}
	}
	if (bNoError)
	{	
		printf("SYS_BackGround_TableStart %d\n",__LINE__);
		if (bg_SetFilterData[Back_Type].status == BG_SLOT_IDLE)
		{			
			bg_SetFilterData[Back_Type].status = BG_SLOT_USE;
			//bg_SetFilterData[Back_Type].slotID = slotid;
			bg_SetFilterData[Back_Type].filterID = filterid;
			bg_SetFilterData[Back_Type].filterPID = bgfilterdata.pid;
			bg_SetFilterData[Back_Type].tableID = bgfilterdata.table_id;	
		}
		if (iI >= BG_MAX_FILTER)
		{
            printf("===BackGround:    BG OVERFLOW\n\n");
			ret = SYS_TABLE_LARTHANMAX_ERROR;
		}
		else
		{
			ret = SYS_TABLE_NOERROR;
		}
		printf("iI = %d\n",iI);
		printf("SYS_BackGround_TableStart %d\n",__LINE__);
	}
	if (ret)
	{
        printf("===BackGround:    The SYS_BG Type:%d   Error is %d\n",Back_Type,ret);
	}
	return ret;
}
int dmx_test(int argc, char **argv)
{
	SYS_Table_Param_t  tempSetFilter;
	U32 ret = SYS_TABLE_NOERROR;
	ret = SFILTER_Init();
	if( 0 != ret )
	{
		printf("%s  SFILTER_Init  error!\n", __FUNCTION__ );
	}
	tempSetFilter.timeout = 1000;
	{
		SYS_Table_InitParam_t pmt_init_param;
		pmt_init_param.p_semaphore = 0;
	    pmt_init_param.callback = NULL;
	    ret = SYS_BackGround_TableInit(SYS_BG_NIT, pmt_init_param);
	    if( 0 != ret )
	    {
	        printf("SYS_BackGround_TableInit error");
	    }
		ret = SYS_BackGround_TableStart(SYS_BG_NIT, tempSetFilter);	
		printf("Start_Background %d ret = %d\n",__LINE__,ret);
		if (ret != SYS_TABLE_NOERROR)
		{
			printf("===BackGround:    NIT BG Start Failed\n\n");	
		}
	}
	return 0;
}

