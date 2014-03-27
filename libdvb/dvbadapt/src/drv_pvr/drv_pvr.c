/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : drv_pvr.c
  Version       : Initial Draft
  Author        : zxguan
  Created       : 2012/11/29
  Last Modified :
  Description   : PBI PVR Function
  Function List :
  History       :
  1.Date        : 2012/11/29
    Author      : zxguan
    Modification: Created file

******************************************************************************/
#define AM_DEBUG_LEVEL 7
/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "osapi.h"
#define LOG_TAG "DVBPvr"
#include "pbitrace.h"
#include "drv_pvr.h"
#include "drv_pvr_player.h"
#include "drv_filter.h"
#include <errno.h>
#include <fcntl.h>

#define	PHONE_RECORD

#ifdef	PHONE_RECORD

static char	phone_sending = 0;	/*是否要把数据发送到手机端	0 不用发送，1需要发送，其它不处理*/

#endif

#define AV_DEV_NO 0
#define EFUSE_USEID_PATH "/sys/class/efuse/userdata"
#define BUFFLEN 100

#define PACKET_LEN 		188
#define PVR_RECFILE_MAXSIZE	(2 * 1000 * 1000 * 1000) //文件不能大于2G



/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/
typedef struct
{
	int id;
	char file_name[PVR_FILELEN];
	pthread_t thread;
	int running;
	int fd;
	AM_DVR_StartRecPara_t spara;
	U32 ChannelHandle;
}DVRData;

typedef struct PVR_USER_DATA_S 
{
    U16 u16VidPid;
    U8  u8AudNum;
    U16 u16AudPid[16];
    U8  u8VidFmt;
    U8  u8AudFmt[16];
	U16 u16PcrPid;
    U8  pName[128];
}PVR_USER_DATA_T;

static U32 gDevNo = PVR_DRIVER_NUM;
static U64 wirteCurIndex = 0;
static DVRData data_threads[DVB_PVR_COUNT];
AM_DVR_StartRecPara_t gRecPara;
U8 gPVRPlayFlag = 0;
PVR_PLAY_STATE_E  gPVRState;
PVR_SPEED_E gPlaySpeed = PVR_PLAY_SPEED_NORMAL;
static C8 gUserDataName[256];
static C8 *gpUserData = NULL;
static U32 gDataLen = 0;
static U32 gCurrectTime = 0;
static U32 gFullTime = 0;
static EventCallBack gPlayerEventCb = NULL;
static U8 gFileIndex = 0;

C8 g_UseId[BUFFLEN];
U8 gUseIdFlag = 0;
U8 gUseIdLen = 0;


KeyInfoList_t CurKeyHead;

int fKeyConfig = -1;
/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
void DRV_PVR_ThreadsInit();
static int DRV_PVR_Creat_File(char *FileName, U8 isClose);
int DRV_PVR_GetUseId(char *buff, int len);
static int DRV_PVR_Write(int fd, uint8_t *buf, int size);
static U8 DRV_PVR_InitFile(int dev_no, U8 index);


/** launcher used flag file add by zxguan 2013-04-01 {{{ */
S32 _DRV_PVR_TouchFile()
{
    //system("touch /mnt/expand/RunDvbPvr");
    DRV_PVR_Creat_File("/mnt/expand/RunDvbPvr", 1);
    return 0;
}

S32 _DRV_PVR_DelFile()
{
    //system("rm /mnt/expand/RunDvbPvr");
    remove("/mnt/expand/RunDvbPvr");
    return 0;
}
/** }}} launcher used flag file add by zxguan 2013-04-01 */

/*****************************************************************************
 Prototype    : DRV_PVR_Init
 Description  : PVR Init
 Input        : None
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2012/11/30
    Author       : zxguan
    Modification : Created function

*****************************************************************************/
S32 DRV_PVR_Init()
{
	AM_ErrorCode_t 	ErrCode = AM_SUCCESS;
	AM_DVR_OpenPara_t dpara;
	
	DRV_PVR_ThreadsInit();
	
	ErrCode = AM_DVR_Open(PVR_DRIVER_NUM, &dpara);
	if( AM_SUCCESS != ErrCode )
	{
		pbierror("AM_DVR_Open Error = 0x%x!\n", ErrCode);
        return 1;
	}
	ErrCode = AM_DVR_Open(TIMESHIFT_DRIVER_NUM, &dpara);
	if( AM_SUCCESS != ErrCode )
	{
		pbierror("AM_DVR_Open Error = 0x%x!\n", ErrCode);
        return 1;
	}
	gPVRState = PVR_PLAY_STATE_INVALID;
    _DRV_PVR_DelFile();

	PBIDEBUG("PVR Init OK!!!!");
    return 0;
}
/*****************************************************************************
 Prototype    : DRV_PVR_UnInit
 Description  : PVR Uninit
 Input        : None
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2012/11/30
    Author       : zxguan
    Modification : Created function

*****************************************************************************/
S32 DRV_PVR_UnInit()
{
	AM_ErrorCode_t 	ErrCode = AM_SUCCESS;
	
	if(gpUserData != NULL)
	{
		free(gpUserData);
		gpUserData = NULL;
	}	
	ErrCode = AM_DVR_Close(PVR_DRIVER_NUM);
	if( AM_SUCCESS != ErrCode )
	{
		pbierror("AM_DVR_Close Error = 0x%x!\n", ErrCode);
        return 1;
	}
	ErrCode = AM_DVR_Close(TIMESHIFT_DRIVER_NUM);
	if( AM_SUCCESS != ErrCode )
	{
		pbierror("AM_DVR_Close Error = 0x%x!\n", ErrCode);
        return 1;
	}
	gPVRState = PVR_PLAY_STATE_INVALID;
    return 0;
}

S32 DRV_PVR_MessageInit( EventCallBack pEventCallBack )
{
	gPlayerEventCb = pEventCallBack;
    return 0;
}

S32 DRV_PVR_MessageUnInit()
{
	gPlayerEventCb = NULL;
    return 0;
}

/* ---------------------------------------------------------------- */
/* -------------------PVR REC CTRL--------------------------------- */
/* ---------------------------------------------------------------- */
S32 DRV_PVR_RecInit(  DRV_PVR_REC_ATTR_t *ptRecAttr, U16 LogicNum )
{
	AM_ErrorCode_t	ErrCode = AM_SUCCESS;
	gDevNo = ptRecAttr->u32DemuxID;
	
	memcpy(data_threads[gDevNo].file_name,ptRecAttr->szFileName, PVR_FILELEN);
 	data_threads[gDevNo].file_name[PVR_FILELEN-1] = 0;
 	ErrCode = AM_DVR_SetSource(gDevNo, AM_DVR_SRC_ASYNC_FIFO1);;
	if (ErrCode != AM_SUCCESS)
	{
		pbiinfo("[%s]DRV_PVR_RecInit Error!!!!!", data_threads[gDevNo].file_name);
		return 1;
	}
	ErrCode = SetDemuxSourceByPvr(0);
	if (ErrCode != AM_SUCCESS)
	{
		pbiinfo("DRV_PVR_RecInit Error!!!!!");
		return 1;
	}
    pbiinfo("[%s]DRV_PVR_RecInit OK!!!!!", data_threads[gDevNo].file_name);
    _DRV_PVR_TouchFile();
    return 0;
}

S32 DRV_PVR_RecUnInit()
{
	DRV_PVR_ThreadsInit();
    _DRV_PVR_DelFile();
    return 0;
}

void DRV_PVR_ThreadsInit()
{
	pbiinfo("DRV_PVR_ThreadsInit start\n");
	int i = 0, j = 0;
	for(i = 0; i < DVB_PVR_COUNT; i++)
	{
		data_threads[i].ChannelHandle = -1;
		data_threads[i].fd = -1;
		data_threads[i].file_name[0] = 0;
		data_threads[i].id = i;
		data_threads[i].running = 0;
		data_threads[i].spara.pid_count = 0;
		data_threads[i].thread = -1;
		gRecPara.pid_count = 0;
		for(j = 0; j < AM_DVR_MAX_PID_COUNT; j++) {
			data_threads[i].spara.pids[j] = 0x1FFF;
			gRecPara.pids[j] = 0x1FFF;
		}
	}
}

static unsigned char Encryption_Char(unsigned char buf, long offset)
{
	//pbiinfo("decryption_char:[0x%02x]->>[%c]>>", buf, useid[offset%useidlen]);
	buf =  (~buf)^g_UseId[offset%gUseIdLen];
	//pbiinfo(,"[0x%02x]\n", buf);
	return buf;
}


static void DRV_PVR_Encryption(int fd, unsigned char *buf, int size)
{
	long offset = 0;
	long i = 0;
	
	if(fd != -1){
		offset = lseek(fd, 0, SEEK_CUR);
		if(gUseIdFlag == 0){
			memset(g_UseId, 0, BUFFLEN);
			if(DRV_PVR_GetUseId(g_UseId, BUFFLEN) != 0){
				gUseIdLen = strlen(g_UseId);
				gUseIdLen = gUseIdLen>BUFFLEN ? BUFFLEN:gUseIdLen;
				gUseIdFlag = 1;
			}
		}	
		while(size > 0){
			(*buf) = Encryption_Char(*buf, offset++);
			buf++;
			size--;
		}
	}
}

static int DRV_PVR_FindPacketHead(uint8_t *buf, int size)
{
	int i = 0, hIndex = -1, count = 0;
	for(i = 0; i < size; i++)
	{
		if(buf[i] == 0x47){
			if(!count) hIndex = i;
			i += (PACKET_LEN - 1);
			count++;
			if(count >= 4)
				break;
				
		}
		else if(count)
		{
			i = hIndex;
			hIndex = -1;
			count = 0;
		}
	}	
	if(count < 4 && count < size/PACKET_LEN)
		hIndex = -1;	
	return hIndex;
}
static int DRV_PVR_CheakData(uint8_t *buf, int size)
{
	int i = 0, j = 0;
	int start = 0, index = 0;
	int nSize = size;
	index = DRV_PVR_FindPacketHead(buf, size);
	if(index == -1) return 0;
	nSize -= index;
	pbiinfo("DRV_PVR_CheakData:find index[%d]", index);
	for(i = index; i < size; i += PACKET_LEN, start += PACKET_LEN)
	{		
		if(buf[i] != 0x47) {
			index = DRV_PVR_FindPacketHead((buf + i), (size - i));
			if(index == -1) return 0;
			nSize -= index;
			i += index;
			pbiinfo("DRV_PVR_CheakData:find index[%d]", index);
		}
		if(index != 0)
			memcpy(buf + start, buf + i, PACKET_LEN);
	}
	pbiinfo("DRV_PVR_CheakData:return size[%d]", nSize);
	return nSize;
}


static int DRV_PVR_Write(int fd, uint8_t *buf, int size)
{
	int ret;
	int left = size;
	uint8_t *p = buf;	
	
	while (left > 0)
	{
		ret = write(fd, p, left);
		if (ret == -1)
		{
			if (errno != EINTR)
			{
				pbiinfo("Write DVR data failed: %s", strerror(errno));
				return -1;
			}
			ret = 0;
		}		
		left -= ret;
		p += ret;
	}
	
	return (size - left);
}

static int DRV_PVR_Read(DVRData *dd, uint8_t *buf, int size)
{
	int cnt = 0;
	int left = size;
	uint8_t *p = buf;	
	int isBad = 0;
	uint8_t *pTemp = p;
	int num = 0;
	while (left > 0 && dd->running)
	{
		cnt = AM_DVR_Read(dd->id, p, left, 1000);
		if (cnt <= 0)
		{
			pbiinfo("No data available from DVR%d\n", dd->id);
			usleep(200*1000);
			continue;
		}		
		left -= cnt;
		p += cnt;
	}	
	//return DRV_PVR_CheakData(buf, (size - left));
	return (size - left);
}

static U8 isCanFillData()
{
	AM_AV_VideoStatus_t VStatus;
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	ErrCode = AM_AV_GetVideoStatus (AV_DEV_NO, &VStatus);
	if( AM_SUCCESS != ErrCode )
	{
		pbierror("AM_AV_GetVideoStatus error %d AM_FALSE\n",ErrCode);
		return 0;
	}
	pbiinfo("VStatus.vb_data[%d]vb_size[%d]\n", VStatus.vb_data, VStatus.vb_size);
	if(VStatus.vb_data*100/VStatus.vb_size <= 66) {
		return 0;
	}

	return 1;
}

static void* DRV_PVR_Thread(void *arg)
{
	DVRData *dd = (DVRData*)arg;
	int cnt, ret = 0;
	uint8_t buf[PACKET_LEN * 1394]; //~= 256 * 1024
	AM_ErrorCode_t	ErrCode = AM_SUCCESS;
	pbiinfo("Data thread for DVR%d start ,record file will save to '%s'\n", dd->id, dd->file_name);
	wirteCurIndex = 0;
	while (dd->running)
	{
		cnt = DRV_PVR_Read(dd, buf, sizeof(buf));
		if (cnt <= 0)
		{
			pbiinfo("Read:No data available from DVR%d\n", dd->id);
			usleep(200*1000);
			continue;
		}
		pbiinfo("read from DVR%d return %d bytes\n", dd->id, cnt);
#ifdef	PHONE_RECORD
		if(phone_sending)
		{
			//PVR_Send_Multi_Data(buf,cnt);	
			//continue;
		}
#endif		
		if (dd->fd != -1)
		{	
			//DRV_PVR_Encryption(dd->fd, buf, cnt);
			ret = DRV_PVR_Write(dd->fd, buf, cnt);
			if(ret == -1) {
				break;
			}
			
			wirteCurIndex += ret;
			if( (wirteCurIndex / PVR_RECFILE_MAXSIZE) > gFileIndex) {	
				pbiinfo("The file too long.Error:wirteCurIndex[%llu]\n", wirteCurIndex);
				gFileIndex++;
				if(DRV_PVR_InitFile(gDevNo, gFileIndex) != 0)
				{
					break;
				}				
			}
		}
		if(gPVRPlayFlag == 1)
		{
			//while(isCanFillData())
			//	usleep(200 * 1000);
			pbiinfo("AM_AV_TimeshiftFillData:len[%d]\n", cnt);
			ErrCode = AM_AV_TimeshiftFillData(AV_DEV_NO, buf, cnt);
			if (ErrCode != AM_SUCCESS)
			{
				pbiinfo("AM_AV_TimeshiftFillData Error:len[%d][x%x]\n", cnt, ErrCode);
				break;
			}
			//usleep(200 * 1000);
		}
	}
	if (dd->fd != -1)
	{
		close(dd->fd);
		dd->fd = -1;
	}
	pbiinfo("Data thread for DVR%d now exit\n", dd->id);
	return NULL;
}
void DRV_PVR_WriteKey(int fd, U32 iPid, U8 cType,U8 *pbKey, U8 bLen)
{
	char pTemp1[256];
	int i = 0;
	if(fd == -1 || pbKey == NULL || bLen <= 0) {
		sprintf( pTemp1, "[%llu][%u][%d][%d]:0000000000000000\n", wirteCurIndex/1024, 0x1FFF, 0, 0);
		DRV_PVR_Write(fd, pTemp1, strlen(pTemp1));
		return;
	}
	sprintf( pTemp1, "[%llu][%u][%d][%d]:", wirteCurIndex/1024, iPid, cType,bLen);
	pbiinfo("DRV_PVR_WriteKey: %s", pTemp1);
	DRV_PVR_Write(fd, pTemp1, strlen(pTemp1));
	for(i = 0; i < bLen; i++){
		sprintf( pTemp1, "%02x", pbKey[i]);
		DRV_PVR_Write(fd, pTemp1, strlen(pTemp1));
	}
	sprintf( pTemp1, "\n");
	DRV_PVR_Write(fd, pTemp1, strlen(pTemp1));
}

void DRV_PVR_SetCurKey(U32 iPid, U8 cType,U8 *pbKey, U8 bLen)
{
	KeyInfo_t *pTemp = NULL;
	KeyInfo_t *pListEnd = NULL;
	if(pbKey == NULL || bLen == 0 || iPid > 0x1FFF || iPid < 0) {
		pbierror("DRV_PVR_SetCurKey Parm Error\n");
		return;
	}
	pbiinfo("DRV_PVR_SetCurKey iPid[%u]\n", iPid);
	if(CurKeyHead.pHead == NULL){
		CurKeyHead.pHead = malloc(sizeof(KeyInfo_t));
		if(CurKeyHead.pHead == NULL){
			pbierror("DRV_PVR_SetCurKey Malloc pHead NULL\n");
			return;
		}
		CurKeyHead.pHead->uPID = iPid;
		CurKeyHead.pHead->uKeyType = cType;
		CurKeyHead.pHead->uKetLen = bLen;
		memcpy(CurKeyHead.pHead->KeyData, pbKey, bLen);
		CurKeyHead.pHead->next = NULL;
	}
	else
	{
		pTemp = CurKeyHead.pHead;
		while(pTemp) {
			if(pTemp->uPID == iPid && pTemp->uKeyType == cType)
			{				
				memcpy(pTemp->KeyData, pbKey, bLen);
				pTemp->uKetLen = bLen;
				break;
			}
			pListEnd = pTemp;
			pTemp = pTemp->next;
		}
		if(pTemp == NULL){
			pTemp = malloc(sizeof(KeyInfo_t));
			if(pTemp == NULL){
				pbierror("DRV_PVR_SetCurKey Malloc pTemp NULL\n");
				return;
			}
			pTemp->uPID = iPid;
			pTemp->uKeyType = cType;
			pTemp->uKetLen = bLen;
			memcpy(pTemp->KeyData, pbKey, bLen);
			pTemp->next = NULL;
			pListEnd->next = pTemp;
		}
	}
}

void DRV_PVR_FreeCurKey()
{
	KeyInfo_t *pTemp = NULL;
	while(CurKeyHead.pHead) {
		pTemp = CurKeyHead.pHead;
		CurKeyHead.pHead = pTemp->next;
		free(pTemp);
	}
	CurKeyHead.pHead = NULL;
}


void DRV_PVR_WriteCurKey()
{
	KeyInfo_t *pTemp = NULL;
	pTemp = CurKeyHead.pHead;
	while(pTemp) {
		pbiinfo("DRV_PVR_WriteKey: uPID[%u]", pTemp->uPID);
		DRV_PVR_WriteKey(fKeyConfig, pTemp->uPID, pTemp->uKeyType, pTemp->KeyData, pTemp->uKetLen);
		pTemp = pTemp->next;
	}
}

void DRV_PVR_Cas_SetKey(U32 iPid, U8 cType,U8 *pbKey, U8 bLen)
{
	U8 uPacket[PACKET_LEN];
	U32 index = 0;
	U32 defPid = 0x1FE;
	U32 i = 0;
	U32 Crc = 0;
	U32	usSectionLength = 0;
	U8 section_number = 0;
	U8 last_section_number = 0;
	U8 version = 0;
	DVRData *dd = &data_threads[gDevNo];

	PVR_USER_DATA_T *pUserData = (PVR_USER_DATA_T *)gpUserData;	 
	if(pUserData->u16VidPid != iPid && pUserData->u16AudPid[0] != iPid)
		return;
	DRV_PVR_SetCurKey(iPid, cType, pbKey, bLen);
	if(dd->running != 1)
		return;
#if 1
	DRV_PVR_WriteKey(fKeyConfig, iPid, cType, pbKey, bLen);
#else
	memset(uPacket, 0xFF, PACKET_LEN);
	uPacket[0] = 0x47;
	defPid |= 0x4000;
	uPacket[1] = (defPid >> 8);
	uPacket[2] = (defPid & 0xFF);
	uPacket[3] = 0x10;
	uPacket[4] = 0;
	
	uPacket[5] = index;
	usSectionLength = 19 + bLen - 3;
	usSectionLength = usSectionLength&0x0FFF;
	usSectionLength |= 0x8000;				// section syntax indicator	=1
	usSectionLength |= 0x3000;				//reserverd 11
	uPacket[6] = (unsigned char)(usSectionLength>>8);
	uPacket[7] = (unsigned char)(usSectionLength);

	version = (version<<1);
	version &= 0x3E;
	version|= 0x01;			//current next indicator
	version|= 0xc0;			// 2 bit reserved

	uPacket[8] = version;
	uPacket[9] = section_number;
	uPacket[10] = last_section_number;
	
	uPacket[11] = (iPid >> 8) & 0x1F;
	uPacket[12] = iPid & 0xFF;
	uPacket[13] = cType;
	uPacket[14] = bLen;
	for(i = 0; i < bLen; i++)
		uPacket[15 + i] = pbKey[i];
	Crc = pv_crc32(uPacket + 5, bLen+5, 1);
	uPacket[15 + bLen] = (Crc >> 24) & 0xFF;
	uPacket[16 + bLen] = (Crc >> 16) & 0xFF;
	uPacket[17 + bLen] = (Crc >>  8) & 0xFF;
	uPacket[18 + bLen] = (Crc) & 0xFF;
	pbiinfo("DRV_PVR_Cas_SetKey Start Crc[0x%x]\n", Crc);
	while(index <= 10) {
		//uPacket[5] = index;
		//DRV_PVR_Write(dd->fd, uPacket, PACKET_LEN);
		index++;
	}
	pbiinfo("DRV_PVR_Cas_SetKey End \n");
#endif
}


void DRV_PVR_Cheak_File(char *FileName)
{
	char *pFileTemp = NULL;	
	char ruleChar[8] = {'\\',':','?','*','<','>','|','\"'};
	int i = 0;
	int len = strlen(FileName);
	if(FileName == NULL || len > 255 || len == 0)
	{
		return;
	}
	for(i = 0; i < 7; i++)
	{
		pFileTemp = FileName;
		while((*pFileTemp))
		{
			if((*pFileTemp) == ruleChar[i])
				(*pFileTemp) = ' ';
			pFileTemp++;
		}
	}
}

static int DRV_PVR_Creat_File(char *FileName, U8 isClose)
{
	int fd = -1;
	if(FileName == NULL)
	{
		return fd;
	}
	DRV_PVR_Cheak_File(FileName);
	fd = open(FileName, O_TRUNC | O_WRONLY | O_CREAT ,0777);
	if (fd == -1)
	{
		pbiinfo("Cannot open record file '%s' for DVR%d", FileName);
		return fd;
	}	
	if (isClose)
	{
		close(fd);
		fd = -1;
	}
	return fd;
}

int CHexToIDec(char cNum)
{
	if(cNum >= '0' && cNum <= '9')
		return cNum-'0';
	if(cNum >= 'a' && cNum <= 'f')
		return cNum-'a'+10;
	if(cNum >= 'A' && cNum <= 'F')
		return cNum-'A'+10;
	return 0;
}

int DRV_PVR_GetUseId(char *buff, int len)
{	
	FILE *pEfuse = NULL;
	int iRet = 0;
	char cBuffer[BUFFLEN];
    	char cName[BUFFLEN];
	int i = 0;
	if(buff == NULL)
		return 0;

	memset( cBuffer, 0, BUFFLEN );
	iRet = access(EFUSE_USEID_PATH, F_OK );
	if( 0 != iRet )
	{
	    pbierror("[%s %d] File Not Find Error.\n", __FUNCTION__,__LINE__);
	    goto error;
	}
	
	pEfuse = fopen( EFUSE_USEID_PATH, "r");
	if( NULL == pEfuse )
	{
	    pbierror("[%s %d] Open Error.\n", __FUNCTION__,__LINE__);
	    goto error;
	}
	
	memset( cBuffer, 0, BUFFLEN );
	if( NULL == fgets( cBuffer, BUFFLEN, pEfuse) )
	{
	    pbierror("[%s %d] File Get String Error[%x].\n", __FUNCTION__,__LINE__, iRet);
	    goto error;
	}
	
	memset( cName, 0, BUFFLEN );
	while(cBuffer[i*2]  && i < BUFFLEN/2)
	{
		cName[i] =  CHexToIDec(cBuffer[i*2])<<4;
		cName[i] +=  CHexToIDec(cBuffer[i*2+1]);
		i++;
	}
	pbiinfo("USEID:[%s]\n", cName);
	memset(buff, 0, len);
	memcpy(buff, cName, len>BUFFLEN ? BUFFLEN:len);	
	fclose( pEfuse );
	return i;
error:
	strncpy(buff, "nouseid", len);
	if( NULL != pEfuse )
		fclose( pEfuse );	
	return len > 7 ? 7:len;
}

static U8 DRV_PVR_ChangeVidFmt(U8 VidFmt)
{
	switch(VidFmt)
	{
		case 0x01:
		case 0x02:
		{
			VidFmt = 0;
		}
		break;

		case 0x10:
		{
			VidFmt = 1;
		}
		break;

		case 0x1B:
		{
			VidFmt = 2;
		}
		break;

		default:
		{
			VidFmt = 0;
		}
		break;
	}
	return VidFmt;
}

static U8 DRV_PVR_ChangeAudFmt(U8 AudFmt)
{
	switch(AudFmt)
   	{
        case 0x03:
        case 0x04:
        {
            AudFmt = 0;
        }
        break;
        case 0x11:
		{
            AudFmt = 19;
        }
        break;
        case 0x0F:
        {
            AudFmt = 2;
        }
        break;
        case 0x81:
        {
            AudFmt = 3;
        }
        break;
        case 0x7a:
        {
            AudFmt = 4;
        }
        break;
        default:
        {
            AudFmt = 0;
        }
        break;
    }
	return AudFmt;
}

static U8 DRV_PVR_InitFile(int dev_no, U8 index)
{
	DVRData *dd = &data_threads[dev_no];
	char pTemp[PVR_FILELEN + 10];
	if(dd->fd != -1)
		close(dd->fd);

	if(index != 0)
		sprintf( pTemp, "%s.[%d]", dd->file_name, index);
	else
		sprintf( pTemp, "%s", dd->file_name);
	pbiinfo("DRV_PVR_InitFile '%s' for DVR%d", pTemp, dd->id);
	dd->fd = DRV_PVR_Creat_File( pTemp, 0);
	if (dd->fd == -1)
	{
		pbiinfo("Cannot open record file '%s' for DVR%d", dd->file_name, dd->id);
		return 1;
	}
	//DRV_PVR_Write(dd->fd, strbuf, sizeof(strbuf));
	if(index == 0) {
		sprintf( pTemp, "%s.%s", dd->file_name, CONFIG_FIX );
		fKeyConfig = DRV_PVR_Creat_File(pTemp, 0);
		if (fKeyConfig == -1)
		{
			pbiinfo("Cannot open record file '%s' for DVR%d", dd->file_name, dd->id);
			return 1;
		}
		wirteCurIndex = 0;
		if(gpUserData != NULL)
		{
			PVR_USER_DATA_T *pUserData = (PVR_USER_DATA_T *)gpUserData;
			sprintf(pTemp, "[%d][%d][%d][%d]\n", pUserData->u16VidPid, pUserData->u16AudPid[0], DRV_PVR_ChangeVidFmt(pUserData->u8VidFmt), DRV_PVR_ChangeAudFmt(pUserData->u8AudFmt[0]));
			pbiinfo("pUserData:[%d][%d][%d][%d]\n", pUserData->u16VidPid, pUserData->u16AudPid[0], (pUserData->u8VidFmt), (pUserData->u8AudFmt[0]));
			pbiinfo("pUserData pTemp:%s\n", pTemp);
			DRV_PVR_Write(fKeyConfig, pTemp, strlen(pTemp));
			DRV_PVR_WriteCurKey();
		}
	}
	return 0;
}

static U8 DRV_PVR_StartThread(int dev_no)
{
	DVRData *dd = &data_threads[dev_no];	
	char strbuf[] = "pbipvr";
	if (dd->running)
	{
		pbiinfo("DRV_PVR_StartThread :already start\n");
		return 1;
	}
#if 1 
	gFileIndex = 0;
	if(DRV_PVR_InitFile(dev_no, gFileIndex) != 0)
	{
		pbiinfo("DRV_PVR_InitFile :Init Error\n");
		return 1;
	}
#endif
	dd->running = 1;
	pthread_create(&dd->thread, NULL, DRV_PVR_Thread, dd);
	return 0;
}

static U8 DRV_PVR_StopThread(int dev_no)
{
	DVRData *dd = &data_threads[dev_no];

	if (!dd->running)
	{
		pbiinfo("DRV_PVR_StopThread :Not start\n");
		return 1;
	}
	dd->running = 0;
	DRV_PVR_WriteKey(fKeyConfig, 0x1FFF, -1, NULL, 0);	
	close(fKeyConfig);	
	sync();
	if(dd->thread != -1)
	pthread_join(dd->thread, NULL);
	pbiinfo("Data thread for DVR%d has exit", dd->id);
	return 0;
}

AM_DVR_StartRecPara_t *Get_Pvr_Data_Para( )
{
	return &(gRecPara);
}

S32 DRV_PVR_RecStartByDev(U8 dev_no)
{
	AM_ErrorCode_t	ErrCode = AM_SUCCESS;
	memcpy(&(data_threads[dev_no].spara), &gRecPara, sizeof(AM_DVR_StartRecPara_t));
	pbiinfo("AM_DVR_StartRecord pid_count[%d] pids[0x%x]", gRecPara.pid_count, gRecPara.pids[0]);
	ErrCode = AM_DVR_StartRecord(dev_no, &(data_threads[dev_no].spara));	
	if (ErrCode != AM_SUCCESS)
	{
		pbierror("AM_DVR_StartRecord dev_no[%d] Error[0x%x]", dev_no, ErrCode);
		return 1;
	}
	ErrCode = DRV_PVR_StartThread(dev_no);
	if(ErrCode != AM_SUCCESS)
	{
		pbierror("DRV_PVR_Thread dev_no[%d] Error[0x%x]", dev_no, ErrCode);
		return 1;
	}
    return 0;
}

S32 DRV_PVR_RecStopByDev(U8 dev_no)
{
	AM_ErrorCode_t	ErrCode = AM_SUCCESS;
	ErrCode |= AM_DVR_StopRecord(dev_no);
	ErrCode |= DRV_PVR_PlayUnInit();	
	ErrCode |= DRV_PVR_StopThread(dev_no);
	if (ErrCode != AM_SUCCESS)
	{
		pbierror("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrCode );
	}	
    return 0;
}


S32 DRV_PVR_RecStart()
{
	DRV_PVR_RecStartByDev(gDevNo);
    return 0;
}

S32 DRV_PVR_RecStop()
{
	DRV_PVR_RecStopByDev(gDevNo);
    return 0;
}

S32 DRV_PVR_RecGetStatus(PVR_REC_STATUS_t *ptRecStatus )
{
	memset(ptRecStatus, 0, sizeof(PVR_REC_STATUS_t));
	if(data_threads[gDevNo].running)
	{
		ptRecStatus->enState = PVR_REC_STATE_RUNNING;
	}
	else
	{
		ptRecStatus->enState = PVR_REC_STATE_STOP;
	}
	ptRecStatus->u32CurTimeInMs = gCurrectTime;
    ptRecStatus->u32StartTimeInMs = 0;
    ptRecStatus->u32EndTimeInMs = gFullTime * 1000;
    return 0;
}

S32 DRV_PVR_RemoveFile(C8 *pName )
{
	if(pName != NULL)
    	remove(pName);
	return 0;
}

/* ---------------------------------------------------------------- */
/* -------------------PVR PLAY CTRL-------------------------------- */
/* ---------------------------------------------------------------- */

static void DRV_PVR_CallEvent( U32 eEventType, S32 s32EventValue, void *args)
{
	if(gPlayerEventCb) gPlayerEventCb(0, eEventType, s32EventValue, args);
}

static U8 * DRV_PVR_StatusToStr(pvr_player_status status)
{
    switch (status) {
    case PLAYER_INITING:
        return "BEGIN_INIT";

    case PLAYER_TYPE_REDY:
        return "TYPE_READY";

    case PLAYER_INITOK:
        return "INIT_OK";

    case PLAYER_RUNNING:
        return "PLAYING";

    case PLAYER_BUFFERING:
        return "BUFFERING";

    case PLAYER_BUFFER_OK:
        return "BUFFEROK";

    case PLAYER_PAUSE:
        return "PAUSE";

    case PLAYER_SEARCHING:
        return "SEARCH_FFFB";

    case PLAYER_SEARCHOK:
        return "SEARCH_OK";

    case PLAYER_START:
        return "START_PLAY";

    case PLAYER_FF_END:
        return "FF_END";

    case PLAYER_FB_END:
        return "FB_END";
		
    case PLAYER_PLAY_NEXT:
        return "PLAY_NEXT";
		
    case PLAYER_FOUND_SUB:
        return "NEW_SUB";
    default:
        return "UNKNOW_STATE";
    }
}

static void DRV_PVR_EventCallBack(int dev_no, int event_type, void *param, void *data)
{
	//pvr_player_status
	if(event_type == AM_AV_EVT_PLAYER_UPDATE_INFO) {
		player_info_t pInfo = *(player_info_t *)param;
		if(gPVRState != PLAYER_STATE_PLAY && pInfo.status == PLAYER_RUNNING) {
			DRV_PVR_CallEvent(0x013, 0, 0);
			gPVRState = PLAYER_STATE_PLAY;
		}
		gCurrectTime = pInfo.current_time;
		gFullTime = pInfo.full_time;
		pbiinfo("Status:[0x%x][%s]\n", 		pInfo.status, DRV_PVR_StatusToStr((pvr_player_status)pInfo.status));
		pbiinfo("full_time:[%d]\n", 		pInfo.full_time);
		pbiinfo("current_time:[%d]\n",	 	pInfo.current_time);
	}
}

S32 DRV_PVR_PlayInit( U32 *pTsBuffer, U32 *pHandle, C8 *pName, U8 u8UseEnctypt )
{
	AM_AV_TimeshiftPara_t TimeshiftPara;
	AM_ErrorCode_t	ErrCode = AM_SUCCESS;
	if(gPVRPlayFlag == 1)
	{
		pbierror("[%s %d]--DRV_PVR_PlayInit-already--\n", DEBUG_LOG );
	}	
	ErrCode = SetDemuxSourceByPvr(2);
	if(ErrCode!=AM_SUCCESS)
	{
		pbiinfo("[DRV_PVR_PlayInit] SetDemuxSourceByPvr error 0x%x\n",ErrCode);
		return 1;
	}
	if (data_threads[gDevNo].fd != -1)
	{
		close(data_threads[gDevNo].fd);
		data_threads[gDevNo].fd = -1;
	}
	TimeshiftPara.dmx_id = 1;
	TimeshiftPara.aud_fmt = _DRV_AVCTRL_GetAudFmt(); 
	TimeshiftPara.vid_fmt = _DRV_AVCTRL_GetVidFmt();
	TimeshiftPara.aud_id = DRV_AVCtrl_Aud_GetPID();
	TimeshiftPara.vid_id = DRV_AVCtrl_Vid_GetPID();
	TimeshiftPara.duration = 0;
	TimeshiftPara.file_path = pName;
	TimeshiftPara.playback_only = 0;
	DRV_PVR_Creat_File(pName, 1);
	ErrCode = AM_AV_StartTimeshift (AV_DEV_NO, &TimeshiftPara);
	pbiinfo("[%s %d]--pName--[%s]--\n", DEBUG_LOG, pName );
	if (ErrCode != AM_SUCCESS)
	{
		pbierror("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrCode );
		return 1; 
	}
	gCurrectTime = 0;
	gFullTime = 0;
	ErrCode  = AM_EVT_Subscribe(AV_DEV_NO, AM_AV_EVT_PLAYER_UPDATE_INFO, DRV_PVR_EventCallBack, NULL);
	if( PLAYER_NO_ERROR != ErrCode  )
    {
        pbierror("[error] AM_EVT_Subscribe\n");
        return ErrCode ;
    }
	gPVRState = PVR_PLAY_STATE_INIT;
	gPVRPlayFlag = 1;
    return 0;
}

S32 DRV_PVR_PlayUnInit()
{
	AM_ErrorCode_t	ErrCode = AM_SUCCESS;
	if(gPVRPlayFlag == 0)
	{
		pbierror("[%s %d]--DRV_PVR_PlayUnInit-already--\n", DEBUG_LOG );
		return 0;
	}
	SetDemuxDVB(2);
	ErrCode = AM_AV_StopTimeshift (AV_DEV_NO);
	if (ErrCode != AM_SUCCESS)
	{
		pbierror("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrCode );
		return 1;    
	}
	ErrCode = AM_AV_ClearVideoBuffer(AV_DEV_NO);
	if( AM_SUCCESS != ErrCode )
	{
		pbierror("Player_PlayClean error %d AM_FALSE\n",ErrCode);
		return 1;
	}
	ErrCode  = AM_EVT_Unsubscribe(AV_DEV_NO, AM_AV_EVT_PLAYER_UPDATE_INFO, DRV_PVR_EventCallBack, NULL);
	if( PLAYER_NO_ERROR != ErrCode  )
    {
        pbierror("[error] AM_EVT_Unsubscribe [0x%x]\n", ErrCode);
        return 1 ;
    }
	gPVRPlayFlag = 0;
    return 0;
}

S32 DRV_PVR_PlayStart()
{
	AM_ErrorCode_t	ErrCode = AM_SUCCESS;
	ErrCode = AM_AV_PlayTimeshift (AV_DEV_NO);
	if (ErrCode != AM_SUCCESS)
	{
		pbierror("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrCode );
		return 1;
	}
	gPVRState = PVR_PLAY_STATE_PLAY;
	DRV_PVR_PlayPause();
    return 0;
}

S32 DRV_PVR_PlayPause()
{
	AM_ErrorCode_t	ErrCode = AM_SUCCESS;
	if(gPlaySpeed != PVR_PLAY_SPEED_NORMAL)
	{
		pbiinfo("[%s %d]--DRV_PVR_PlayTrickMode--[%d]--\n", DEBUG_LOG, gPlaySpeed );
		ErrCode = AM_AV_FastForwardTimeshift(AV_DEV_NO, 0);
		if (ErrCode != AM_SUCCESS)
		{
			pbierror("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrCode );
			return 1;
		}
		gPlaySpeed = PVR_PLAY_SPEED_NORMAL;
		usleep(500 * 1000);
	}
	ErrCode = AM_AV_PauseTimeshift (AV_DEV_NO);
	if (ErrCode != AM_SUCCESS)
	{
		pbierror("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrCode );
		return 1;
	}
	gPVRState = PVR_PLAY_STATE_PAUSE;
    return 0;
}

S32 DRV_PVR_PlayResume()
{
	AM_ErrorCode_t	ErrCode = AM_SUCCESS;
	if(gPlaySpeed != PVR_PLAY_SPEED_NORMAL)
	{
		pbiinfo("[%s %d]--DRV_PVR_PlayTrickMode--[%d]--\n", DEBUG_LOG, gPlaySpeed );
		ErrCode = AM_AV_FastForwardTimeshift(AV_DEV_NO, 0);
		if (ErrCode != AM_SUCCESS)
		{
			pbierror("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrCode );
			return 1;
		}
		gPlaySpeed = PVR_PLAY_SPEED_NORMAL;
		usleep(500 * 1000);
	}
	ErrCode = AM_AV_ResumeTimeshift (AV_DEV_NO);
	if (ErrCode != AM_SUCCESS)
	{
		pbierror("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrCode );
		return 1;
	}
	gPVRState = PVR_PLAY_STATE_PLAY;
    return 0;
}

S32 DRV_PVR_PlayStop()
{
	AM_ErrorCode_t	ErrCode = AM_SUCCESS;	
	gPVRState = PVR_PLAY_STATE_STOP;	
    return 0;
}

S32 DRV_PVR_PlayTrickMode(PVR_SPEED_E  ePlaySpeed)
{
	int speed = 0;
	AM_ErrorCode_t	ErrCode = AM_SUCCESS;	
	pbiinfo("[%s %d]--DRV_PVR_PlayTrickMode--[%d]--\n", DEBUG_LOG, ePlaySpeed );
	if((ePlaySpeed ) > PVR_PLAY_SPEED_NORMAL){
		gPVRState = PLAYER_STATE_FF;
	}
	else if((ePlaySpeed) < PVR_PLAY_SPEED_NORMAL) {
		gPVRState = PLAYER_STATE_FB;
	}
	else if((ePlaySpeed) == PVR_PLAY_SPEED_NORMAL){
		gPVRState = PLAYER_STATE_PLAY;
	}
	speed = pow(2, abs(ePlaySpeed - PVR_PLAY_SPEED_NORMAL)) * 2;
	pbiinfo("[%s %d]--speed[%d]-gPVRState[%d]-\n", DEBUG_LOG, speed, gPVRState );
	if( gPVRState == PLAYER_STATE_FF )
	{
		ErrCode = AM_AV_FastForwardTimeshift(AV_DEV_NO, speed);
		if (ErrCode != AM_SUCCESS)
		{
			pbierror("[%s %d]--ERROR--0x%x--speed[%d]\n", DEBUG_LOG, ErrCode, speed);
			return 1;
		}
		gPlaySpeed = ePlaySpeed;
	}
	else if( gPVRState == PLAYER_STATE_FB )
	{
	    ErrCode = AM_AV_FastBackwardTimeshift(AV_DEV_NO, speed);
		if (ErrCode != AM_SUCCESS)
		{
			pbierror("[%s %d]--ERROR--0x%x--speed[%d]", DEBUG_LOG, ErrCode, speed);
			return 1;
		}			
		gPlaySpeed = ePlaySpeed;
	}
    return 0;
}


S32 DRV_PVR_PlayGetStatus(PVR_PLAY_STATUS_t *PVRState)
{
	memset(PVRState, 0, sizeof(PVR_PLAY_STATUS_t));
	if(data_threads[gDevNo].running){
		PVRState->enState = gPVRState;
	}
	else {
		PVRState->enState = PVR_PLAY_STATE_STOP;
	}
	PVRState->enSpeed = gPlaySpeed;
	PVRState ->u32CurPlayTimeInMs = gCurrectTime;
    return 0;
}

S32 DRV_PVR_PlayGetFileAttr(PVR_FILE_ATTR_t *pPvrFileAttr)
{
	memset(pPvrFileAttr, 0, sizeof(PVR_FILE_ATTR_t));
	pPvrFileAttr->u32StartTimeInMs = 0;
	pPvrFileAttr->u32EndTimeInMs = gFullTime * 1000;
    return 0;
}


S32 DRV_PVR_SetUserData( C8 *pName, C8 *pData, U32 DataLen )
{
	if(pName != NULL)
	{
		memcpy(gUserDataName, pName, 256);
		gUserDataName[255] = 0;
	}
	if(pData != NULL)
	{

		if(gpUserData != NULL){
			free(gpUserData);
		}
		gDataLen = DataLen;
		gpUserData = malloc(DataLen);
		memcpy(gpUserData, pData, DataLen);
	}
    return 0;
}

S32 DRV_PVR_GetUserData( C8 *pName, C8 *pData, U32 *pDataLen )
{
	//no pName len, catn't memcpy!
	if(pName != 0)
		pName[0] = '\0';
	if(pData != NULL) {
		memcpy(pData, gpUserData, gDataLen);
		*pDataLen = gDataLen;
	}
    return 0;
}
#ifdef	PHONE_RECORD

void	 DRV_PVR_Start_Phone_Sending()
{
	phone_sending = 1;
}

void	 DRV_PVR_Stop_Phone_Sending()
{
	phone_sending = 0;
}

int DRV_PVR_Is_Phone_Sending()
{
	return phone_sending;
}

#endif

#undef LOG_TAG
