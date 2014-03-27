/******************************************************************************
 Copyright (C), 2001-2013, Pro Broadband Inc.
 File Name     : dvb_download_test.c
 Version       : Initial Draft
 Author        : dwzhang
 Created       : 2013/04/16
 Last Modified :
 Description   : dvb_download_test
 Function List :
 History       :
 1.Date        : 2013/04/16
 Author      :
 Modification: Created file
 ******************************************************************************/

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef ANDROID
#include <sys/socket.h>
#endif
#include <arpa/inet.h>

#include "osapi.h"
//#include "AVL_tuner.h"
#include <errno.h>
#include <fcntl.h>

#include "pbitrace.h"
#include "drv_tuner.h"
#include "drv_filter.h"
#include "dvbcore.h"
#include "dvb_download.h"
/**********************************************************************/
/*							Private Types									*/
/**********************************************************************/

#define	FILTER_DEPTH_SIZE            16
#define MAX_SECTION_LENGTH      (4096)
#define SECTION_OFFSET 			35
#define DL_FILENAME 			"/cache/update.zip"//"/data/data/com.pbi.dvb/update.zip"//"/mnt/sdcard/update.zip"//"/mnt/sda/sda1/update.zip"//"/data/update.zip"
#define DL_MASK_FILENAME 		"cache/dlmask"//"/data/data/com.pbi.dvb/dlmask"//"/mnt/sdcard/dlmask"//"/data/dlmask"
#define DL_SW_FILENAME 		"/cache/dlswver"//"/data/data/com.pbi.dvb/dlswver"//"/mnt/sdcard/swver"//"/data/swver"
static U32 guDL_Task = 0xFFFFFFFF;
static const U32 guDL_Stack_Size = 96 * 1024;
static const U32 guDL_Priority = 5;
static U32 guDL_Msg_Queue = 0xFFFFFFFF;

static U32 dl_running_flag = 0;
static U32 dl_init_flag = FALSE;
static U32 mask_flag = TRUE;
static U32 Last_sec_num = 0;
//static U32 LPercent_num = 100,SPercent_num = 100;
typedef struct {
	//U8  	cycle_count;
	U8 sections_mask;
} DOWNLOAD_MASK;
static DOWNLOAD_MASK *section_mask = NULL;
U8 size_errorcount = 0;
static U32 dl_cont = 0;
static S32 fp = 0, mask_fp = 0, sw_fp = 0;
static U32 dl_percent = 0;
static U32 sw_ver = 0;
static U32 sw_size = 0;

static DVB_DLMsg_Notify g_dvb_download_info_cb = NULL;
extern Loader_msg_t dl_msg;

static U8 *_dl_GetSectionDataBuffer(tSFILTER_SlotId slot);
static void _dl_FreeDataBuffer(void* buf);
static void _dl_FreeSectionDataBuffer(tSFILTER_SlotId slot, void* buf);
static void _dl_FilterInfoCallBack(tSFILTER_SlotId slot,
		tSFILTER_FilterId filter, tSFILTER_FreeBufferFct free_func, U8 *buffer,
		U32 length, U16 pid);

void Close_DL_File(void) {
	close(fp);
	fp = 0;
	close(mask_fp);
	mask_fp = 0;
	close(sw_fp);
	sw_fp = 0;
}

void Delete_DL_File(void) {
	char cmd[50];
	Close_DL_File();
	Delete_SectionMask();
	memset(cmd, 0, 50);
	sprintf(cmd, "rm %s", DL_MASK_FILENAME);
	system(cmd);
	sprintf(cmd, "rm %s", DL_SW_FILENAME);
	system(cmd);
	sprintf(cmd, "rm %s", DL_FILENAME);
	system(cmd);
	system("sync");
	usleep(1000);
}

S32 Open_DL_File(void) {
	S32 ret = 0;
	char cmd[50];

	ret = access(DL_FILENAME, F_OK);
	if (0 == ret) {
		memset(cmd, 0, 50);
		sprintf(cmd, "chmod %d %s", 777, DL_FILENAME);
		system(cmd);
		pbiinfo("\n--------dlfile access %d----------------\n", __LINE__);
	}
	fp = open(DL_FILENAME, O_RDWR | O_CREAT | O_SYNC, 777);
	if (-1 == fp) {
		pbiinfo("ZDW===>dlfile open failed!\n");
		return PV_FAILURE;
	}

	ret = access(DL_MASK_FILENAME, F_OK);
	if (0 == ret) {
		memset(cmd, 0, 50);
		sprintf(cmd, "chmod %d %s", 777, DL_MASK_FILENAME);
		system(cmd);
		pbiinfo("\n--------dlmaskfile access %d----------------\n", __LINE__);
	}
	mask_fp = open(DL_MASK_FILENAME, O_RDWR | O_CREAT | O_SYNC, 777);
	if (-1 == mask_fp) {
		pbiinfo("ZDW===>dlmaskfile open failed!\n");
		return PV_FAILURE;
	}

	ret = access(DL_SW_FILENAME, F_OK);
	if (0 == ret) {
		memset(cmd, 0, 50);
		sprintf(cmd, "chmod %d %s", 777, DL_SW_FILENAME);
		system(cmd);
		pbiinfo("\n--------dlswfile access %d----------------\n", __LINE__);
	}
	sw_fp = open(DL_SW_FILENAME, O_RDWR | O_CREAT | O_SYNC, 777);
	if (-1 == sw_fp) {
		pbiinfo("ZDW===>dlswfile open failed!\n");
		return PV_FAILURE;
	}
	return PV_SUCCESS;
}

/******************************************************************************
 Function Name : Set_All_Section_Mask
 Inputs : lastsection number
 Return : none
 Description :
 set the mask of all section need to download (the first seciton number is 0 )
 History : 

 ******************************************************************************/
S8 Set_All_Section_Mask(void) {
	//DOWNLOAD_MASK 
	U32 i;
	S32 status = 0;
	S8 ch = 0;

	section_mask =
			OS_MemAllocate(system_p, (Last_sec_num + 1) * sizeof(DOWNLOAD_MASK));
	if (NULL == section_mask) {
		return PV_NO_MEMORY;
	}
	memset(section_mask, 0, (Last_sec_num + 1) * sizeof(DOWNLOAD_MASK));
	for (i = 0; i <= Last_sec_num; i++) {
		//section_mask[i].cycle_count=0;
		section_mask[i].sections_mask = 0xff;
	}
	//ch=getc(mask_fp);  
	status = read(mask_fp, &ch, 1);
	if ((-1 == status) || (0 == status)) {
		pbiinfo("ZDW===>dlmask is null\n");
		status = lseek(mask_fp, 0, SEEK_SET);
		if (status < 0) {
			pbierror("%s	error %d \n", __FUNCTION__, __LINE__);
			return -1;
		}
		//for(i = 0; i<=Last_sec_num; i++)
		{
			//U8 buff = 0xff;
			status = write(mask_fp, section_mask,
					(Last_sec_num + 1) * sizeof(DOWNLOAD_MASK));
			if (status < 0) {
				pbierror("%s	error %d  status:%d\n", __FUNCTION__, __LINE__,
						status);
				pbierror("errno=%d,  %s\n", errno, strerror(errno));
				return -1;
			}
		}
		return 1; //������
	} else {
		pbiinfo("ZDW===>dlmask is not null,ch=%d\n", ch);
		status = lseek(mask_fp, 0, SEEK_SET);
		if (status < 0) {
			pbierror("%s	error %d \n", __FUNCTION__, __LINE__);
			return -1;
		}
		//for(i=0;i<=Last_sec_num;i++)
		{
			//U8 buff = 0;
			status = read(mask_fp, section_mask,
					(Last_sec_num + 1) * sizeof(DOWNLOAD_MASK));
			if (status < 0) {
				pbierror("%s	error %d \n", __FUNCTION__, __LINE__);
				return -1;
			}
			//section_mask[i].sections_mask = buff;
		}
		return 2; //�Ѵ���
	}
	//return 0;
}

/******************************************************************************
 Function Name : Set_section_mask
 Inputs : section number
 Return : none
 Description :
 set the mask of the section number
 History : 

 ******************************************************************************/
U8 Set_section_mask(U16 sec_num, U16 sec_len) {
	U32 offset = 0;
	S32 status = 0;
	//DOWNLOAD_MASK mask_temp;
#if 0
	U32 m,mset;

	m=sec_num%32;
	mset =~(0x00000000 | (1<<(31-m)));
	all_sections_mask[sec_num/32]&=mset;
#else

	if (sec_num < 0) {
		pbierror("%s param error %d \n", __FUNCTION__, __LINE__);
		return 1;
	}
	if (sec_num == Last_sec_num) {
		if (0 == sw_size) {
			return 1;
		}
	}
	//section_mask[sec_num].cycle_count = 1;
	section_mask[sec_num].sections_mask = 0;
	//mask_temp.cycle_count = 1;
	//mask_temp.sections_mask = 0;		
	offset = sec_num * (sizeof(DOWNLOAD_MASK));
	//pbiinfo("ZDW===>mask_fp :%d,offset:%d\n",mask_fp,offset);
	status = lseek(mask_fp, offset, SEEK_SET);
	if (status < 0) {
		pbierror("%s	error %d \n", __FUNCTION__, __LINE__);
		return 1;
	}
	status = write(mask_fp, (section_mask + sec_num), (sizeof(DOWNLOAD_MASK)));
	if (status < 0) {
		pbierror("%s	error %d  status:%d\n", __FUNCTION__, __LINE__, status);
		pbierror("errno=%d,  %s\n", errno, strerror(errno));
		return 1;
	}
	return 0;
#endif
}

/******************************************************************************
 Function Name : check_mask_of_all
 Inputs : none
 Return : ture for all the mask have been set to 1
 false for not all the mask was  set to 1
 Description :
 check all the mask value for get the download status
 History : 
 
 ******************************************************************************/
BOOL check_mask_of_all(void) {
	U32 i;
	BOOL ret = TRUE;
#if 0	
	for(i=0;i<MAX_SECTIONS/32;i++)
	{
		if(all_sections_mask[i]==0)
		{
			continue;
		}
		else
		{
			//not all data have been downloaded
			return FALSE;
		}
	}
	//we have all data of this partition
	return TRUE;
#else
//pbiinfo("ZDW===>check_mask_of_all!!!\n");
	for (i = 0; i <= Last_sec_num; i++) {
		//pbiinfo("ZDW===>mask=0x%x\n",section_mask[i].sections_mask);
		if (section_mask[i].sections_mask != 0) {
			ret = FALSE;
			if (mask_flag == TRUE) {
				continue;
			} else {
				break;
			}
			//return FALSE;
		} else {
			if (mask_flag == TRUE) {
				dl_cont++;
			}
		}
	}
	return ret;

#endif
}

void Delete_SectionMask(void) {
	if (section_mask != NULL) {
		OS_MemDeallocate(system_p, section_mask);
		section_mask = NULL;
	}
}

U8 Save_Section(U8*buff, U32 sec_num, U16 sec_len) {
	U32 offset = 0;
	S32 status = 0;
	//S32 ret = 0;
	//char cmd[50];	
	//U8 filename[150];
	//static S32 fp = 0;
	U32 last_sec_len = 0;

	if ((buff == NULL) || (sec_num < 0)) {
		pbierror("%s param error secno:%d  (%d) \n", __FUNCTION__, sec_num,
				__LINE__);
		return 1;
	}
	if ((sec_len > MAX_SECTION_LENGTH)||(sec_len < SECTION_OFFSET)){
	pbierror("%s param error %d \n",__FUNCTION__,__LINE__);
	return 1;
}
	//pbiinfo("\n dl section file write!!!\n\n");

	if (0 == sec_num) {
		//int i=0;
		//for(i=35;i<38;i++)
		//pbiinfo("ZDW===>section 0 buffer[%d] = %x \n", i, buffer[i]);
		sw_size = buff[35] | buff[36] << 8 | buff[37] << 16 | buff[38] << 24;
		pbiinfo("ZDW===>sw_size = %d \n", sw_size);
	} else {
		if (sec_num == Last_sec_num) {
			if (sw_size > 0)/*Ҫ���յ���0��secton, �ſ��������һ��*/
			{
				last_sec_len = sw_size % (MAX_SECTION_LENGTH - SECTION_OFFSET);
				if (0 == last_sec_len) {
					last_sec_len = MAX_SECTION_LENGTH - SECTION_OFFSET;
				}
				pbiinfo("ZDW===>last_sec_len = %d \n", last_sec_len);
			} else {
				return 2;
			}
		}

		offset = (sec_num - 1) * (sec_len - SECTION_OFFSET);
		//pbiinfo("ZDW===>fp :%d,offset:%d\n",fp,offset);
		status = lseek(fp, offset, SEEK_SET);
		if (status < 0) {
			pbierror("%s	error %d \n", __FUNCTION__, __LINE__);
			return 1;
		}
		if (sec_num == Last_sec_num) {
			status = write(fp, (buff + 31), last_sec_len);
		} else {
			status = write(fp, (buff + 31), (sec_len - SECTION_OFFSET));
		}
		if (status < 0) {
			pbierror("%s	error %d  status:%d\n", __FUNCTION__, __LINE__,
					status);
			pbierror("errno=%d,  %s\n", errno, strerror(errno));
			return 1;
		}
	}
	//usleep(20);
	//pbiinfo("ZDW===>Save_Section OK\n");
	//close(fp);
	//fp = 0;
	return 0;
}

void Save_SW_Ver(U32 sw_version) {
	S32 status = 0;
	status = write(sw_fp, &sw_version, 4);
	if (status < 0) {
		pbierror("%s	error %d  status:%d\n", __FUNCTION__, __LINE__, status);
		pbierror("errno=%d,  %s\n", errno, strerror(errno));
	}
}

S8 Check_SW_Ver(U32 sw_version) {
	S32 status = 0;
	if (TRUE == mask_flag) {
		status = read(sw_fp, &sw_ver, 4);
		if (status != 4) {
			return -1;
		}
	}

	if (sw_ver == sw_version) {
		return 1;
	} else {
		pbiinfo("ZDW===>oldver: %d, newver: %d\n", sw_ver, sw_version);
		return 0;
	}
}

U32 Get_percent(void) {
	return dl_percent;
}

static U8 *_dl_GetSectionDataBuffer(tSFILTER_SlotId slot) {
	U8 *pbuf = NULL;

	pbuf = (U8 *) OS_MemAllocate(system_p,MAX_SECTION_LENGTH);/*yangweiqing modify 10-04-26*/
	//	pbuf = (U8 *)small_mem_allocate(MAX_SECTION_LENGTH);
	if (!pbuf) {
		pbiinfo(
				"===_dl_FreeDataBuffer:   GetSectionDataBuffer() Failed. Not enough memory\n");
		//DB_BG((TRC_CRITICAL,"===BackGround:   GetSectionDataBuffer() Failed. Not enough memory\n"));
	}
	return pbuf;
}

static void _dl_FreeDataBuffer(void* buf) {
	if (!buf)
		pbiinfo(
				"===_dl_FreeDataBuffer:   FreeSectionDataBuffer() Failed. Null ptr\n");
	//DB_BG((TRC_CRITICAL,"===BackGround:   FreeSectionDataBuffer() Failed. Null ptr\n"));
	else {
		OS_MemDeallocate(system_p, buf);/*yangweiqing modify 10-04-26*/
		//	small_mem_deallocate(buf);
		buf = NULL;
	}
}

static void _dl_FreeSectionDataBuffer(tSFILTER_SlotId slot, void* buf) {
	_dl_FreeDataBuffer(buf);
}

static void _dl_FilterInfoCallBack(tSFILTER_SlotId slot,
		tSFILTER_FilterId filter, tSFILTER_FreeBufferFct free_func, U8 *buffer,
		U32 length, U16 pid) {
	S32 os_err = OS_SUCCESS;
	U8 *section_buffer = NULL;
	U16 section_length = 0;
	U16 extend_id = 0xFFFF;
	U8 table_id = 0xFF;
	U8 version = 0xFF;
	U8 sect_num = 0;
	U8 last_sect_num = 0;
	_DL_Msg_t msg;
	U16 section_num = 0, last_section_num = 0;
	U32 sf_ver = 0;
	//pbiinfo("_dl_FilterInfoCallBack.\n");
	if (NULL == buffer) {
		pbiinfo("_dl_FilterInfoCallBack: ( NULL == buffer ).\n");
		return;
	}

	if (pv_crc32(buffer, length, 1) != 0) {
		pbiinfo(
				"ZDW===>section CRC error. PID = 0x%04X, tabid = 0x%02X, extid = 0x%04X, sect_num = %d, last_sect_num = %d.\n",
				pid, table_id, extend_id, sect_num, last_sect_num);
		if (NULL != free_func) {
			free_func(slot, buffer);
		}
		return;
	}
	memset(&msg, 0, sizeof(_DL_Msg_t));
	section_length = (U16) length;
	table_id = *(buffer);
	//section_length = *(buffer + 1) << 8 | *(buffer + 4);

	msg.table_id = table_id;
	msg.sec_len = section_length;
	msg.buffer = buffer;

	msg.free_func = _dl_FreeSectionDataBuffer;
	//section_num = msg.buffer[3]*0x100 + msg.buffer[4];
	//sf_ver = msg.buffer[13]*0x1000000 + msg.buffer[14]*0x10000 + msg.buffer[15]*0x100 + msg.buffer[16];
	//last_section_num = msg.buffer[25]*0x100 + msg.buffer[26];
	//if(sf_ver!=0x296)
	//pbiinfo("ZDW===>secno=%d, last=%d, sf_ver=0x%x\n",section_num,last_section_num,sf_ver);
	//pbiinfo("send buffer: %x %x %x.addr = 0x%x.\n",msg.buffer[0],msg.buffer[1],msg.buffer[2], &buffer[0]);

	os_err = OS_QueueSend(guDL_Msg_Queue, &msg, sizeof(_DL_Msg_t), OS_PEND, 0);
	if (os_err != OS_SUCCESS) {
		pbiinfo("_dl_FilterInfoCallBack: send message to queue failed.\n");
		if ((NULL != free_func) && (NULL != buffer)) {
			free_func(slot, buffer);
		}
	}
	//usleep(200);
}

/***********************************************************
 *Dvb_DownLoad_Process:
 *	�������task
 * History :
 *   2013-05-09  by ZDW
 ************************************************************/
void Dvb_DownLoad_Process(Loader_msg_t* dl_info) {
	ts_src_info_t ptSrcInfo;
	TUNER_LOCKSTATUS_t TLK = eTun_unLock;
	int iI = 0;
	S32 os_err = OS_SUCCESS;
	tSFILTER_SlotId slot_id = -1;
	tSFILTER_FilterId filter_id = -1;
	U8 filter_data[FILTER_DEPTH_SIZE] = { 0 };
	U8 filter_mask[FILTER_DEPTH_SIZE] = { 0 };
	U8 filter_negate[FILTER_DEPTH_SIZE] = { 0 };
	U8 filter_err_flag = 0;
	U8 stop_recv_flag = 0;
	_DL_Param_t dlParam;
	_Search_Loader_Tbl_Err_e ret = eSEARCH_LOADER_TABLE_SUCCESS;

	if (NULL == dl_info) {
		Close_DL_File();
		ret = eSEARCH_LOADER_TABLE_PARAM_ERROR;
		if (g_dvb_download_info_cb != NULL)
			g_dvb_download_info_cb(eDOWNLOAD_FAILED, ret);
		return;
	}
	//pbiinfo("ZDW===>dl_info->Frequnce : %d\n",dl_info->Frequnce );
	//pbiinfo("ZDW===>dl_info->SymbolRate : %d\n",dl_info->SymbolRate );
	if (dl_info->Trans_Type == eDVB_C) {

		ptSrcInfo.MediumType = MEDIUM_TYPE_DVBC;
		ptSrcInfo.u.Cable.FrequencyKHertz = dl_info->Frequnce;	//355000;
		ptSrcInfo.u.Cable.SymbolRateSPS = dl_info->SymbolRate;	//6875;
		ptSrcInfo.u.Cable.Modulation = dl_info->QamSize;	//ePI_64QAM;
		ptSrcInfo.u.Cable.Bandwidth = dl_info->Bandwidth;	//ePI_BW_8MHZ;
		ptSrcInfo.u.Cable.Bandwidth = GetTunerBandWidth();
		//pbiinfo("ZDW===>ptSrcInfo.u.Cable.FrequencyKHertz : %d\n",ptSrcInfo.u.Cable.FrequencyKHertz );
		//pbiinfo("ZDW===>ptSrcInfo.u.Cable.SymbolRateSPS : %d\n",ptSrcInfo.u.Cable.SymbolRateSPS );
		//pbiinfo("ZDW===>Dvb_DownLoad_Process!!!\n");
	} else if (dl_info->Trans_Type == eDVB_T) {
		ptSrcInfo.MediumType = MEDIUM_TYPE_DVBT;
		ptSrcInfo.u.Terrestrial.FrequencyKHertz = dl_info->Frequnce;//355000;
		//ptSrcInfo.u.Terrestrial.SymbolRateSPS = dl_info->SymbolRate;//6875;
		ptSrcInfo.u.Terrestrial.Constellation = dl_info->QamSize;	//ePI_64QAM;
		ptSrcInfo.u.Terrestrial.Bandwidth = dl_info->Bandwidth;
	} else if (dl_info->Trans_Type == eDVB_S) {
		ptSrcInfo.MediumType = MEDIUM_TYPE_DVBS;
		ptSrcInfo.u.Satellite.FrequencyKHertz = dl_info->Frequnce;	//355000;
		ptSrcInfo.u.Satellite.SymbolRateSPS = dl_info->SymbolRate;		//6875;
		ptSrcInfo.u.Satellite.Modulation = dl_info->QamSize;		//ePI_64QAM;
	}

	DRV_Tuner_SetFrequency(0, &ptSrcInfo, 1000);

	for (iI = 0; iI < 50; iI++) {
		TLK = DRV_Tuner_GetLockStatus(0);
		if (TLK == 1)
			break;//pbierror("FUN:%s,LINE:%d,TLK:0x%x\n",__FUNCTION__,__LINE__,TLK);	return eTUNER_NOT_DONE;}
		usleep(10);
	}
	pbiinfo("ZDW===>TLK %d \n", TLK);
	if (TLK != eTun_Lock) {
		Close_DL_File();
		ret = eSEARCH_LOADER_TABLE_TUNER_ERROR;
		if (g_dvb_download_info_cb != NULL)
			g_dvb_download_info_cb(eDOWNLOAD_FAILED, ret);
		return;
	}

	/* create message queue */
	os_err =
			OS_QueueCreate(&guDL_Msg_Queue, "DVB_Download_Queue", 50, sizeof(_DL_Msg_t), 0);
	if (OS_SUCCESS != os_err) {
		pbiinfo(
				"DVB_Download_Queue, create message queue failed! Errcode = %d.\n",
				os_err);
		Close_DL_File();
		ret = eSEARCH_LOADER_TABLE_FAILED;
		if (g_dvb_download_info_cb != NULL)
			g_dvb_download_info_cb(eDOWNLOAD_FAILED, ret);
		return;		//-1;
	}
	//memset(filter_data, 0xff, FILTER_DEPTH_SIZE);
	//memset(filter_mask, 0xff, FILTER_DEPTH_SIZE);
	//memset(filter_negate, 0, FILTER_DEPTH_SIZE);
	/* set and start filter */
	/* allocate channel */
	dlParam.u16Pid = dl_info->Pid;	//0x1ec;
	dlParam.u32TimeoutMs = 10000;

	pbiinfo("ZDW===>SFILTER_Check_DataPID!!!\n");
	slot_id = SFILTER_Check_DataPID(dlParam.u16Pid,DEMUX_0);
	if (slot_id >= 0) {
		pbiinfo(
				"ZDW===>found exist channel, pid = 0x%04X. try to disable it.\n",
				dlParam.u16Pid);
		//xhren0713,����Ѿ��к͸�pid�������channel, �Ƚ���channel�ص�
		if (SFILTER_Free_Channel(slot_id) < 0) {
			pbiinfo("warning: diable channel failed.\n");
			ret = eSEARCH_LOADER_TABLE_FAILED;
		}
	}

	slot_id = SFILTER_Allocate_Channel(DEMUX_0);
	pbiinfo("ZDW===>SFILTER_Allocate_Channel!!! slot_id=%d\n", slot_id);
	if (slot_id < 0) {
		//xhren0713,����channelʧ�ܣ��˳�
		pbiinfo("Dvb_DownLoad_Process allocate channel failed.\n");
		filter_err_flag = 1;
		ret = eSEARCH_LOADER_TABLE_FAILED;
	} else {
		SFilterCallback_t callbacks;
		callbacks.pFCallBack =
				(tSFILTER_SlotCallbackFct) _dl_FilterInfoCallBack;
		callbacks.pFGetBuffer =
				(tSFILTER_GetBufferFct) _dl_GetSectionDataBuffer;
		callbacks.pFFreeBuffer =
				(tSFILTER_FreeBufferFct) _dl_FreeSectionDataBuffer;

		SFILTER_RegisterCallBack_Function(slot_id, &callbacks);
		pbiinfo("ZDW===>SFILTER_RegisterCallBack_Function!!!\n");
	}

	/* allocate filter */
	if (filter_err_flag == 0) {
		S8 scDemuxError;
		//filter_id = SFILTER_Allocate_Filter(slot_id, eSFILTER_MODE_HW);
		filter_id = SFILTER_Allocate_Filter(DEMUX_0);
		pbiinfo("ZDW===>SFILTER_Allocate_Filter!!! filter_id=%d\n", filter_id);
		if (filter_id < 0) {
			if (SFILTER_NumFilterAssociate_Slot(slot_id) == 0)
				SFILTER_Free_Channel(slot_id);
			filter_err_flag = 1;
			ret = eSEARCH_LOADER_TABLE_FAILED;
		} else {
			memset(filter_data, 0xff, FILTER_DEPTH_SIZE);
			memset(filter_mask, 0xff, FILTER_DEPTH_SIZE);
			filter_data[0] = 0x84;
			filter_mask[0] = 0x0;
			pbiinfo("ZDW===>SFILTER_SetFilter!!!slot_id=%d,filter_id=%d\n",
					slot_id, filter_id);
			scDemuxError = SFILTER_SetFilter(slot_id, filter_id,
					FILTER_DEPTH_SIZE, filter_mask, filter_data, filter_negate);
			if (scDemuxError < 0) {
				SFILTER_Free_Filter(filter_id);
				if (SFILTER_NumFilterAssociate_Slot(slot_id) == 0)
					SFILTER_Free_Channel(slot_id);
				filter_err_flag = 1;
				ret = eSEARCH_LOADER_TABLE_FAILED;
			} else {
				pbiinfo("ZDW===>SFILTER_SetPid!!!slot_id=%d,pid=%d\n", slot_id,
						dlParam.u16Pid);
				scDemuxError = SFILTER_SetPid(slot_id, dlParam.u16Pid);
				if (scDemuxError < 0) {
					SFILTER_Free_Filter(filter_id);
					if (SFILTER_NumFilterAssociate_Slot(slot_id) == 0)
						SFILTER_Free_Channel(slot_id);
					filter_err_flag = 1;
					ret = eSEARCH_LOADER_TABLE_FAILED;
				} else {
					pbiinfo("ZDW===>SFILTER_Open_Channel!!!slot_id=%d\n",
							slot_id);
					scDemuxError = SFILTER_Open_Channel(slot_id);
					if (scDemuxError < 0) {
						SFILTER_Free_Filter(filter_id);
						if (SFILTER_NumFilterAssociate_Slot(slot_id) == 0)
							SFILTER_Free_Channel(slot_id);
						filter_err_flag = 1;
						ret = eSEARCH_LOADER_TABLE_FAILED;
					}
				}
			}
		}
	}
	//sleep(1);
	if (filter_err_flag != 0) {
		if (g_dvb_download_info_cb != NULL)
			g_dvb_download_info_cb(eDOWNLOAD_FAILED, ret);
	}
	stop_recv_flag = filter_err_flag;
	while (stop_recv_flag == 0) {
		_DL_Msg_t msg;
		U32 actual_size = 0;
		S32 recv_ret = OS_SUCCESS;
		U16 section_num = 0, last_section_num = 0;
		U32 sw_temp = 0;
		static U8 last_percent_temp = 0xff;
		// buffer[4096];
		//pbiinfo("ZDW===>OS_QueueReceive.\n");
		memset(&msg, 0, sizeof(_DL_Msg_t));
		recv_ret = OS_QueueReceive(guDL_Msg_Queue, &msg, sizeof(_DL_Msg_t),
				&actual_size, (dlParam.u32TimeoutMs + 2000) );
		if (recv_ret < 0) {
			pbiinfo("ZDW===>eSEARCH_LOADER_TABLE_TIMEOUT.\n");
			ret = eSEARCH_LOADER_TABLE_TIMEOUT;
			stop_recv_flag = 2;
			break;
		}
#if 0
		{
			if( NULL != msg.buffer )
			{
				//msg.free_func(msg.buffer);
				_dl_FreeDataBuffer(msg.buffer);
				memset(&msg, 0, sizeof(_DL_Msg_t));
			}
			continue;
		}
#endif
		section_num = msg.buffer[3] * 0x100 + msg.buffer[4];
		sw_temp = msg.buffer[13] * 0x1000000 + msg.buffer[14] * 0x10000
				+ msg.buffer[15] * 0x100 + msg.buffer[16];
		last_section_num = msg.buffer[25] * 0x100 + msg.buffer[26];
		//pbiinfo("ZDW===>newver: %d\n", sw_temp);
		//buffer=msg.buffer+31;
		//memcpy(buffer,msg.buffer+31,4061);
		if (mask_flag) {
			S8 ret1 = 0;

			Last_sec_num = last_section_num;
			ret1 = Set_All_Section_Mask();
			if (1 == ret1)/*������*/
			{
				pbiinfo("ZDW===>ret1 1\n");
				sw_size = 0;
				sw_ver = sw_temp;
				pbiinfo("ZDW===>oldver: %d, newver: %d\n", sw_ver, sw_temp);
				Save_SW_Ver(sw_ver);
			} else if (2 == ret1)/*�ϵ�����*/
			{
				pbiinfo("ZDW===>ret1 2\n");
				ret1 = Check_SW_Ver(sw_temp);
				if (ret1 != 1)/*�汾�ѱ仯��ɾ��ɵ�*/
				{
					pbiinfo("ZDW===>SW VER ERROR AAA!!!\n");
					Delete_DL_File();
					Open_DL_File();
					mask_flag = TRUE;
					dl_cont = 0;
					sw_ver = 0;
					sw_size = 0;
					dl_percent = 0;
					if (NULL != msg.buffer) {
						//msg.free_func(msg.buffer);
						_dl_FreeDataBuffer(msg.buffer);
					}
					continue;
				} else {
					//section_mask[0].sections_mask = 0xff;/*�ϵ���Ҫ������section 0, �������һ��setion������*/
				}
			}
			//memset(Flash_In_Ram,0xff, g_All_Flash_EnAddr - FLASH_BASE_ADDRESS);
			pbiinfo("ZDW===>Set_All_Section_Mask.\n");
			if (check_mask_of_all() == TRUE)/*�������Ѵ���������ɵ�ͬ��İ汾*/
			{
				//DBG1(MSGprintf("over down\n"));
				stop_recv_flag = 1;
				dl_percent = 100;
				pbiinfo("ZDW===>DownLoad section was compelete!!!\n");
				if (g_dvb_download_info_cb != NULL)
					g_dvb_download_info_cb(eDOWNLOAD_COMPLETED, dl_percent);
				if (NULL != msg.buffer) {
					//msg.free_func(msg.buffer);
					_dl_FreeDataBuffer(msg.buffer);
					memset(&msg, 0, sizeof(_DL_Msg_t));
				}
				mask_flag = FALSE;
				break;
			} else {
				section_mask[0].sections_mask = 0xff;/*�ϵ���Ҫ������section 0, �������һ��setion������*/
			}
			mask_flag = FALSE;
		}
		if (Check_SW_Ver(sw_temp) != 1) {
			pbiinfo("ZDW===>SW VER ERROR BBB!!!\n");

			Delete_DL_File();
			Open_DL_File();
			mask_flag = TRUE;
			dl_cont = 0;
			sw_ver = 0;
			sw_size = 0;
			dl_percent = 0;
			if (NULL != msg.buffer) {
				//msg.free_func(msg.buffer);
				_dl_FreeDataBuffer(msg.buffer);
			}
			size_errorcount++;
			if (size_errorcount >= 30) {
				stop_recv_flag = 2;
				ret = eSEARCH_LOADER_TABLE_SECTION_ERROR;
				break;
			}
			continue;
		}
		//pbiinfo("ZDW===>section_mask[%d].sections_mask:%d.\n",section_num,section_mask[section_num].sections_mask);
		if (section_mask[section_num].sections_mask == 0xff) {
			U8 ret_temp = 0;
			/*copy buffer to flash in ram*/
			if (Last_sec_num != last_section_num) {
				//DBG1(MSGprintf("[TFLOADER]ERROR:error size1\n"));
				pbiinfo("[DLOADER]ERROR:error size1\n");
				size_errorcount++;
				if (size_errorcount >= 30) {
					stop_recv_flag = 2;
					ret = eSEARCH_LOADER_TABLE_SECTION_ERROR;
					if (NULL != msg.buffer) {
						//msg.free_func(msg.buffer);
						_dl_FreeDataBuffer(msg.buffer);
					}
					break;
				}
			}
			if ((section_num == Last_sec_num) && (0 == sw_size)) {
				if (NULL != msg.buffer) {
					//msg.free_func(msg.buffer);
					_dl_FreeDataBuffer(msg.buffer);
				}
				continue;
			}
			ret_temp = Save_Section(msg.buffer, section_num, msg.sec_len);
			if (ret_temp != 0) {
				if (NULL != msg.buffer) {
					//msg.free_func(msg.buffer);
					_dl_FreeDataBuffer(msg.buffer);
				}
				if (1 == ret_temp) {
					size_errorcount++;
					if (size_errorcount >= 30) {
						stop_recv_flag = 2;
						ret = eSEARCH_LOADER_TABLE_SAVE_ERROR;
						break;
					}
				}
				continue;
			}
			//section_mask[section_num].cycle_count = 0;
			ret_temp = Set_section_mask(section_num, msg.sec_len);
			if (ret_temp != 0) {
				if (NULL != msg.buffer) {
					//msg.free_func(msg.buffer);
					_dl_FreeDataBuffer(msg.buffer);
				}

				size_errorcount++;
				if (size_errorcount >= 30) {
					stop_recv_flag = 2;
					ret = eSEARCH_LOADER_TABLE_SAVE_ERROR;
					break;
				}
				continue;
			}
			dl_cont++;
			dl_percent = dl_cont * 100 / (Last_sec_num + 1);
			if ((dl_percent < 0) || (dl_percent > 100)) {
				Delete_DL_File();
				Open_DL_File();
				mask_flag = TRUE;
				dl_cont = 0;
				sw_ver = 0;
				sw_size = 0;
				dl_percent = 0;
				if (NULL != msg.buffer) {
					//msg.free_func(msg.buffer);
					_dl_FreeDataBuffer(msg.buffer);
				}
				continue;
			}
			if (last_percent_temp != dl_percent) {
				last_percent_temp = dl_percent;
				if (g_dvb_download_info_cb != NULL)
					g_dvb_download_info_cb(eDOWNLOAD_PROGRESS, dl_percent);
				pbiinfo(
						"ZDW===>cont:%d, new section_num:%d,Last_sec_num:%d, se_len:%d, per:%d\n",
						dl_cont, section_num, Last_sec_num, msg.sec_len,
						dl_percent);
			}
			//usleep(200);
			//pbiinfo("ZDW===>cont:%d, new section_num:%d,Last_sec_num:%d, se_len:%d, per:%d\n", dl_cont, section_num, Last_sec_num, msg.sec_len, dl_percent);
		} else {
			//section_mask[section_num].cycle_count++;
			//pbiinfo("!!!!!section_num repeat %d!!!!!!!\n",section_num);
			//usleep(200);
			//if((NULL != msg.buffer) && (NULL != msg.free_func))
			if (NULL != msg.buffer) {
				//msg.free_func(msg.buffer);
				_dl_FreeDataBuffer(msg.buffer);
			}
			continue;
		}
		if (check_mask_of_all() == TRUE)/*�������*/
		{
			//DBG1(MSGprintf("over down\n"));
			stop_recv_flag = 1;
			dl_percent = 100;
			if (g_dvb_download_info_cb != NULL)
				g_dvb_download_info_cb(eDOWNLOAD_COMPLETED, dl_percent);
			pbiinfo("ZDW===>DownLoad section compelete!!!\n");
		}
		//if((NULL != msg.buffer) && (NULL != msg.free_func))
		if (NULL != msg.buffer) {
			//msg.free_func(msg.buffer);
			_dl_FreeDataBuffer(msg.buffer);
		}
	}
	if (2 == stop_recv_flag) {
		if (g_dvb_download_info_cb != NULL)
			g_dvb_download_info_cb(eDOWNLOAD_FAILED, ret);
	}
	Close_DL_File();
	mask_flag = TRUE;
	/* stop and free filter */
	if (filter_err_flag == 0) {
		SFILTER_Free_Filter(filter_id);
		/*modify lsz 2010.05.9*/
		if (SFILTER_NumFilterAssociate_Slot(slot_id) == 0) {
			if (SFILTER_Close_Channel(slot_id) == 0) {
				//SFILTER_Free_Channel(slot_id);
				//ret = eSEARCH_TABLE_SUCCESS;
			} else {
				ret = eSEARCH_LOADER_TABLE_OTHER_ERROR;
			}
		}
	}
	/* clean message queue */
	{
		_DL_Msg_t msg;
		U32 actual_size = 0;

		memset(&msg, 0, sizeof(_DL_Msg_t));
		while (OS_QueueReceive(guDL_Msg_Queue, &msg, sizeof(_DL_Msg_t),
				&actual_size, OS_CHECK) > 0) {
			//static int gzxcount = 0;
			//pbiinfo("[%s %d]==count = %d=bufferaddr = 0x%x..\n",DEBUG_LOG,gzxcount++,msg.buffer);
			_dl_FreeDataBuffer(msg.buffer);
			memset(&msg, 0, sizeof(_DL_Msg_t));
		}
	}

	/* delete message queue */
	os_err = OS_QueueDelete( guDL_Msg_Queue );
	guDL_Msg_Queue = 0xFFFFFFFF;

	Delete_SectionMask();
	Dvb_DownLoad_SetStatus(0);
	pbiinfo("[%s %d]===========\n", DEBUG_LOG);
	//dl_init_flag = FALSE;
	return;
}

/***********************************************************
 *Dvb_DownLoad_Start:�ṩ�ϲ���������ݵĽӿ�
 *
 * History :
 *   2013-05-09  by ZDW
 ************************************************************/
S32 Dvb_DownLoad_Start(void/*Loader_msg_t* dlmsg*/) {
	//U8 filename[150];
	S32 ret = 0;
	S32 s32Err;

	if (TRUE == dl_init_flag) {
		pbiinfo("ZDW===>created Dvb_DownLoad_Process was running!\n");
		return PV_FAILURE;
	}
	dl_init_flag = TRUE;
	Dvb_DownLoad_SetStatus(1);
	ret = Open_DL_File();
	if (ret != PV_SUCCESS) {
		return ret;
	}
	//pbiinfo("ZDW===>msg.Frequnce : %d\n",dlmsg->Frequnce );
	//pbiinfo("ZDW===>msg.SymbolRate : %d\n",dlmsg->SymbolRate );

	pbiinfo("ZDW===>Dvb_DownLoad_Start!!!\n");
	s32Err = OS_TaskCreate(&guDL_Task, "Dvb_DownLoad_Process",
			(void *(*)(void *)) Dvb_DownLoad_Process, (void *) &dl_msg, NULL,
			guDL_Stack_Size, guDL_Priority, 0);
	if (OS_SUCCESS != s32Err) {
		pbiinfo("Dvb_DownLoad_Start, created Dvb_DownLoad_Process failed!\n");
		//OS_SemDelete(guInstall_Start_Sem);
		Dvb_DownLoad_SetStatus(0);
		if (g_dvb_download_info_cb != NULL)
			g_dvb_download_info_cb(eDOWNLOAD_FAILED, ret);
		return PV_FAILURE;
	}

	pbiinfo("ZDW===>Dvb_DownLoad_Start OK!!!\n");
	return PV_SUCCESS;
}

void DL_Info_RegMessage_Notify_CallBack(DVB_DLMsg_Notify Cb) {
	g_dvb_download_info_cb = Cb;
}

void DL_Info_UnRegMessage_Notify_CallBack(void) {
	g_dvb_download_info_cb = NULL;
}

void Dvb_DownLoad_SetStatus(U32 dl_stastus) {
	dl_running_flag = dl_stastus;
}

U32 Dvb_DownLoad_GetStatus(void) {
	return dl_running_flag;
}

