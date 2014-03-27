/******************************************************************************

  Copyright (C), 2001-2011, Pro Broadband Inc.

 ******************************************************************************
  File Name     : drv_pvr_player.c
  Version       : Initial Draft
  Author        : zhang
  Created       : 2013/11/26
  Last Modified :
  Description   : PBI PVR Function
  Function List :
  History       :
  1.Date        : 2013/11/26
    Author      : zshang
    Modification: Created file

******************************************************************************/
#define AM_DEBUG_LEVEL 7
/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/
 
#define LOG_TAG "DVBPvrPlayer"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "osapi.h"
#include "pbitrace.h"
#include "drv_pvr.h"
#include <errno.h>
#include <fcntl.h>
#include <am_dmx.h>
#include <am_dsc.h>
#include "drv_pvr_player.h"

#define PLAYER_DMXID 1
#define PLAYER_DSCID 0
#define PLAYER_AVID 0

#define BUFFER_SIZE (188 * 188 * 5)
#define MAXSENDDATA (32 * 1024)
#define DEF_BITREAT 4000 //4Mb/s
#define BITREAT_STEP 300 

#define MAX_CORRECT 90000 //90ms
#define CORRECT_STEP 3000 //3ms 


#define mPlayerLock(lockKey)		(OS_SemWait(lockKey))
#define mPlayerUnLock(lockKey)		(OS_SemSignal(lockKey))


#define DEBUG_E pbierror //printf
#define DEBUG_I pbiinfo  //printf
#define isNegative(x) ((x) ? (-1) : 1)
/*
compute Player_thread Big-O for Sleep really time,
if you modify Player_thread, please modify here.
*/
#define BIG_O(n) (45 * n)

static U8 isPlayerInit = 0;
static pvr_player_info_t gplay_info;
static long lCurTime = 0;
static U8 gPacketLength = 0;
static U64 readCurIndex = 0;
static EventCallBack gPlayerEventCb = NULL;
static U32 gCurrectTime = 0;
static U32 gFullTime = 0;
static U8 isCanSeek = 0;
static S32 gInjectSize = 0;
static U8 errorCount = 0;
char gTimeshiftFile[PVR_FILELEN] = {0};

	
static S32 Player_GetDscId(U32 PID);
static U8 Player_GetCurDataLevel(S32 *sData, S32 *sSize);
static U8 Player_CheakLevel();
static Player_Error  Player_SetKey(S32 ulDescId, AM_DSC_KeyType_t type,U8 *pbEvenKey, U8 bEvenLen);
static S32 Player_FileSeek(U32 fHandel, S32 offest, U8 type);


static void ASCIIToCharByHex(char *buff, int len)
{
	int i = 0;
	while((len%2 == 0) &&( len/2) > i)
	{	
		char temp1 = (buff[i * 2] >= '0' && buff[i * 2] <= '9') ? '0' : ('a'-10);
		char temp2 = (buff[i * 2 + 1] >= '0' && buff[i * 2 + 1] <= '9') ? '0' : ('a'-10);
		buff[i] = (buff[i * 2] - temp1) * 16;
		buff[i] += (buff[i * 2 + 1] - temp2);
		i++;
	}
}

static void Player_Sleep(U32 us)
{
	struct timespec sleepTime;
	if(us > 0 && us < 1000 * 1000 * 10) {
		sleepTime.tv_sec = 0;
    	sleepTime.tv_nsec = us * 1000; 
		nanosleep(&sleepTime, NULL);
		//usleep(us);
	}
}

static U32 Player_StrRchr(const U8 *str, U8 ch)
{
	U32 index = -1;
	U8 *pStr = str;
	while(pStr && (pStr = strchr(pStr, ch)) != NULL){
		index = pStr - str;
		pStr++;
	}
	return index;
}


static void Player_CallEvent( U32 eEventType, S32 s32EventValue, void *args)
{
	if(gPlayerEventCb) gPlayerEventCb(0, eEventType, s32EventValue, args);
}

static void Player_BitRateCtrl(U32 sendBuf)
{
	U32 time = 0;
#ifdef COMPUTE_REAT
	time = 0;
	if(gplay_info.uBitRate != 0)
		time = ((sendBuf * 8000 /gplay_info.uBitRate)) - ((gPacketLength >= 188) ? BIG_O(sendBuf/gPacketLength) : 0);
	//DEBUG_I("Player_GetDiffTime[%d] uBitRate[%d] sendBuf[%d]\n", time, gplay_info.uBitRate, sendBuf);
#if (PVR_PLAYER_TYPE == PLAYER_TIMESHIFT)
		if( gFullTime && ((gFullTime - gCurrectTime) < 3))
			time = time / 2;
		Player_Sleep(time);
#elif (PVR_PLAYER_TYPE == PLAYER_INJECT)		
		if(gplay_info.isDefRate){
			U8 uLevel = Player_CheakLevel(); 
			if(uLevel < gplay_info.MinLevel && errorCount++ > 5) {
				gplay_info.uBitRate += BITREAT_STEP;
				DEBUG_I("isDefRate Change:gplay_info.uBitRate[%d]\n", gplay_info.uBitRate);
				errorCount = 0;
			}
			else if(uLevel > gplay_info.MaxLevel && errorCount++ > 20) {
				gplay_info.uBitRate -= BITREAT_STEP;
				DEBUG_I("isDefRate Change:gplay_info.uBitRate[%d]\n", gplay_info.uBitRate);
				errorCount = 0;
			}			
		}
		else
		{
			U8 uLevel = Player_CheakLevel(); 
			if(uLevel < gplay_info.MinLevel && errorCount++ > 5) {
				U32 uLevel = (gplay_info.MinLevel - uLevel);
				gplay_info.sCorrectTime += CORRECT_STEP * (1 << uLevel);
				DEBUG_I("Change:gplay_info.sCorrectTime[%d]\n", gplay_info.sCorrectTime);
				errorCount = 0;
			}			
		}
		if(abs(gplay_info.sCorrectTime) > MAX_CORRECT)
			gplay_info.sCorrectTime = MAX_CORRECT;
		Player_Sleep(time - gplay_info.sCorrectTime);
#endif		
#endif
}

static void Player_CheakKey()
{
	KeyInfo_t *pKeyTemp;
	KeyInfoList_t *pKeyList = &(gplay_info.KeyList);
	
	if(gplay_info.isEnctypt && pKeyList->pHead != NULL) {
		pKeyTemp = pKeyList->pHead;
		while(pKeyTemp->next){	
			if(!pKeyTemp->isUser && pKeyTemp->uSetIndex <= (readCurIndex/1024) 
				&& pKeyTemp->KeyData){			
				/*DEBUG_I( "CheakTime:[%llu][%u][%u][%u] readCurIndex[%lld]\n", (pKeyTemp->uSetIndex), 
													(pKeyTemp->uPID),
													(pKeyTemp->uKeyType),
													(pKeyTemp->uKetLen),
													readCurIndex/1024);	*/			
				Player_SetKey(Player_GetDscId(pKeyTemp->uPID), pKeyTemp->uKeyType, pKeyTemp->KeyData, pKeyTemp->uKetLen);
				pKeyTemp->isUser = 1;
			}
			pKeyTemp = pKeyTemp->next;
		}
	}
}
static void Player_SeekKey(U32 offset)
{
	KeyInfo_t *pKeyTemp;
	KeyInfo_t *pKeyTemp2;
	KeyInfoList_t *pKeyList = &(gplay_info.KeyList);
	if(gplay_info.isEnctypt && pKeyList->pHead != NULL) {
		pKeyTemp = pKeyList->pHead;
		while(pKeyTemp->next){	
			pKeyTemp->isUser = 0;
			pKeyTemp = pKeyTemp->next;
		}
	}
	Player_CheakKey();
}



static U32 Player_FindPacketHead(U8 *uData, U32 uLen)
{
	U32 iI = 0;
	if(uLen < 408)
		return 0;
	for(iI = 0; iI < 204; iI++)
	{
		if(uData[iI] == 0x47 )
		{
			if(uData[iI+188] == 0x47)
			{
				gPacketLength = 188;
				break;
			}
			else if(uData[iI+204] == 0x47)
			{
				gPacketLength = 204;
				break;
			}
			else
			{
				//DEBUG_E("the file is not ts format\n");
			}			
		}
	}
	if(iI == 204)
		iI = 0;
	//DEBUG_I("Player_FindPacketHead: iI[%d] uData[iI][0x%02x]gPacketLength[%d]\n", iI, uData[iI], gPacketLength);
	return iI;
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
static U64 PCR_Base(U8* pPkt, S32 n)   
{   
    U8 PCRbase_1, PCRbase_2, PCRbase_3, PCRbase_4, PCRbase_5;   
    U64 PCR_basefun;   
   
    /*33 bit PCR_base 以90kHz为单位*/   
    memcpy(&PCRbase_1, &pPkt[n+2],1);   
    memcpy(&PCRbase_2, &pPkt[n+3], 1);   
    memcpy(&PCRbase_3, &pPkt[n+4], 1);   
    memcpy(&PCRbase_4, &pPkt[n+5], 1);   
    memcpy(&PCRbase_5, &pPkt[n+6], 1);   
    PCR_basefun = (PCRbase_1 <<25) | (PCRbase_2 <<17) | (PCRbase_3 <<9) | (PCRbase_4 <<1) |(PCRbase_5 >>7);  
	
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
static U64 PCR_Extension(U8* pPkt, S32 n)   
{   
    U8 PCRext_h, PCRext_l;   
    U64 PCR_extenfun;   
   
    /*9 bit PCR_extension以27MHz为单位*/   
    memcpy(&PCRext_h, &pPkt[n+6],1);   
    memcpy(&PCRext_l, &pPkt[n+7],1);   
  
    PCR_extenfun = ((PCRext_h & 0x01) <<8) |PCRext_l;   
   
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
static U64 CalcuteRate(U8 *Buffer,U32 n,U32 *uiPacketNum,U64 *PCR)
{
	U64 PCRBase,PCRExt, uiDiff=0;
	U64 Rate = 0;
	U64 BytesNum = 0;
	
	if(PCR[0] == 0)
	{
		PCRBase = PCR_Base(Buffer, n); 
		PCRExt = PCR_Extension(Buffer,n);
		PCR[0] = PCRBase * 300 + PCRExt;
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
			BytesNum = (*uiPacketNum) * gPacketLength * 8 ;  /*bits between two PCR*/
			Rate = ((BytesNum* 27000) / (long)uiDiff); // Mb/ms
			Rate = Rate;
		}

		*uiPacketNum = 0;
		PCR[0] = PCR[1];
		return Rate;// Mb/ms
	}

}
static void Player_ComputeBitRate(U8 *uData, U32 uLen)
{
	U8 *pPacket = uData;
	U32 uDataLen = uLen;
	U16	PID = 0;
	U8  TS_Error_Indicator = 0;
	U8	ucAdaptField = 0;
	U8	ucTransportScrambling = 0;
	U32	PCRIntervalPacket = 0;
	U8	ucPCRflag = 0;
	U32 uIndex = 0;
	U64 uPCR[2]={0,0};
	U64 TSRateNew = 0;
	U16 uiPcrPid = 0;
	
	uIndex = Player_FindPacketHead(pPacket, uDataLen);
	if(gPacketLength == 0)
		return -1;
	pPacket += uIndex;
	uDataLen -= uIndex;
	while(uDataLen)
	{
		if(pPacket[0] != 0x47)
		{
			uIndex = Player_FindPacketHead(pPacket, uDataLen);
			if(uIndex > 0){
				pPacket += uIndex;
				uDataLen -= uIndex;
				continue;
			}
			else
			return;
		}
		if(uiPcrPid != 0)
		{
			PCRIntervalPacket++;
		}
		TS_Error_Indicator = (pPacket[1] >> 7) & 0x01;
		PID = ((pPacket[1] & 0x1f) << 8) | pPacket[2];
		ucTransportScrambling = (pPacket[3] >> 6);
		ucAdaptField = (pPacket[3] >>4) & 0x03;
		//if( PID == 0x1fff || TS_Error_Indicator == 1 ||ucTransportScrambling!=0)/*NULL packet or error packet ignore it*/
		if( PID == 0x1fff || TS_Error_Indicator == 1 )/*Modify CA  zshang by 20131225*/
		{
			pPacket += gPacketLength;
			uDataLen -= gPacketLength;
			continue;
		}
		if(ucAdaptField == 0x02 || ucAdaptField == 0x03)
		{
			ucPCRflag = (pPacket[5] >> 4) & 0x01;
			if(pPacket[4] != 0 && ucPCRflag == 0x01)
			{
				if(uiPcrPid == 0 || uiPcrPid == PID)
				{		
					uiPcrPid = PID;
					TSRateNew= CalcuteRate(pPacket, 4,&PCRIntervalPacket,uPCR);
					if(TSRateNew != 0  && abs(TSRateNew - gplay_info.uBitRate) > 100)
					{
						PCRIntervalPacket = 0;
						gplay_info.uBitRate = (int)TSRateNew;
						gplay_info.isDefRate = 0;
						//DEBUG_I("TS new rate:%d\n",(int)TSRateNew);
					} 
				}
			}
		}
		pPacket += gPacketLength;
		uDataLen -= gPacketLength;
	}
}

static void Player_ReadLock()
{
	S32 dataSize = 0;
	if(gplay_info.player_state < PLAYER_STATE_PLAY)
		return;
	while(Player_CheakLevel() > gplay_info.MaxLevel)
		Player_Sleep(200 * 1000);
}

static S32 Player_FileRead(U32 fHandel, U8* uBuff, S32 Len)
{
	S64 ret = 0;
	if(fHandel == -1){
		return -1;
	}
	Player_ReadLock();
	ret = read(fHandel, uBuff, Len);
	if(ret < 0 && (errno != EAGAIN || errno != EINTR))
	{
		DEBUG_E("Player_FileRead failed\n");
		gplay_info.player_state = PLAYER_STATE_ERROR;
		Player_CallEvent(3, 0, 0);
		DEBUG_E( "[error] Player_FileRead errno[0x%x][%s]\n", errno, strerror(errno));
	}
	else if(ret == 0)
	{
		if(gplay_info.pCurFile->next) {
			gplay_info.pCurFile = gplay_info.pCurFile->next;
			if(gplay_info.pCurFile->ufHandle == -1)
			{
				gplay_info.pCurFile->ufHandle = open(gplay_info.pCurFile->pName, O_RDONLY | O_LARGEFILE, 0666);
			}
			Player_FileSeek(gplay_info.pCurFile->ufHandle, 0, SEEK_SET);
			DEBUG_I( "Player_FileList:pName[%s] ufHandle[%d]\n", gplay_info.pCurFile->pName, gplay_info.pCurFile->ufHandle);
			return 0;
		}
		DEBUG_E("Player_FileRead Close\n");
		gplay_info.player_state = PLAYER_STATE_INJECT_END;
		Player_CallEvent(12, 0, 0);
		DEBUG_E( "[error] Player_FileRead errno[0x%x][%s]\n", errno, strerror(errno));
		return -1;
	}
	return ret;
}

static S32 Player_FileSeek(U32 fHandel, S32 offest, U8 type)
{
	S32 ret = 0;
	DEBUG_I("Player_FileSeek offest[%d] type[%d]\n", offest, type);	
	ret = lseek64(fHandel, offest, type);
	if(ret == -1 && errno != EINTR)
	{
		DEBUG_E("Player_FileSeek failed\n");
		gplay_info.player_state = PLAYER_STATE_ERROR;
		Player_CallEvent(3, 0, 0);
		DEBUG_E( "[error] Player_FileSeek errno[0x%x][%s]\n", errno, strerror(errno));
	}
	return ret;
}

static S32 Player_FileClose(U32 fHandel)
{
	return close(fHandel);
}

static U32 Player_PlayClean()
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	ErrCode = AM_AV_ClearVideoBuffer(PLAYER_AVID);
	if( AM_SUCCESS != ErrCode )
	{
		pbierror("Player_PlayClean error %d AM_FALSE\n",ErrCode);
		return 0;
	}
	return ErrCode;
}


static U64 Player_PlaySeek(U64 offest)
{
	FileInfo_t *pFileTemp = NULL;
	FileInfoList_t *pFileList = &(gplay_info.FileList);
	U64 uFileSize = 0;
	
	offest = (offest >= 0) ? offest : 0;
	offest = (offest >= gplay_info.uFileSize) ? gplay_info.uFileSize : offest;

	pFileTemp = pFileList->pHead;
	while(pFileTemp && pFileTemp->uFileSize < offest)
	{		
		offest -= pFileTemp->uFileSize;
		uFileSize += pFileTemp->uFileSize;
		pFileTemp = pFileTemp->next;
	}
	if(pFileTemp == NULL || offest < 0) {
		gplay_info.pCurFile = pFileList->pHead;
		return 0;
	}
	gplay_info.pCurFile = pFileTemp;
	if(gplay_info.pCurFile->ufHandle == -1)
	{
		gplay_info.pCurFile->ufHandle = open(gplay_info.pCurFile->pName, O_RDONLY | O_LARGEFILE, 0666);
	}
	DEBUG_I( "Player_PlaySeek:pName[%s] ufHandle[%d]\n", gplay_info.pCurFile->pName, gplay_info.pCurFile->ufHandle);
	DEBUG_I( "Player_PlaySeek:offest[%llu] uFileSize[%llu]\n", offest, uFileSize);
	offest = (U64)Player_FileSeek(gplay_info.pCurFile->ufHandle, offest, SEEK_SET);
	if(offest == -1 && errno != EINTR)
	{
		DEBUG_E("Player_FileSize failed SEEK_END\n");
		DEBUG_E( "[error] Player_FileSeek errno[0x%x][%s]\n", errno, strerror(errno));
		gplay_info.pCurFile = pFileList->pHead;
		return 0;
	}
	Player_PlayClean();
	return offest + uFileSize;
}
static U8 Player_isCanSeek()
{
	return gInjectSize > ONE_FRAME_SIZE(gplay_info.uBitRate);
}

static U8 Player_CheakLevel()
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	U8 uVLevel = 0, uALevel = 0;
	AM_AV_VideoStatus_t VStatus;
	AM_AV_AudioStatus_t AStatus;
	ErrCode = AM_AV_GetAudioStatus (PLAYER_AVID, &AStatus);
	if( AM_SUCCESS != ErrCode )
	{
		DEBUG_E("AM_AV_GetAudioStatus error %d AM_FALSE\n",ErrCode);
		return 0;
	}		
	uALevel = AStatus.ab_data * 100/AStatus.ab_size;	
	/*DEBUG_I("Player_GetCurDataLevel Audio ab_data[%d] ab_size[%d] uCurLevel[%d%]\n",
		AStatus.ab_data,
		AStatus.ab_size,
		uALevel);*/
		
	ErrCode = AM_AV_GetVideoStatus (PLAYER_AVID, &VStatus);
	if( AM_SUCCESS != ErrCode )
	{
		DEBUG_E("AM_AV_GetVideoStatus error %d AM_FALSE\n",ErrCode);
		return 0;
	}
	uVLevel = VStatus.vb_data * 100/VStatus.vb_size;	
	/*DEBUG_I("Player_GetCurDataLevel Video vb_data[%d] vb_size[%d] uCurLevel[%d%]\n",
		VStatus.vb_data,
		VStatus.vb_size,
		uVLevel);*/
	return (uVLevel >= uALevel) ? uALevel : uVLevel;
}


static U8 Player_GetCurDataLevel(S32 *sData, S32 *sSize)
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	U8 uCurLevel = 0;	
	if(gplay_info.eInjectType == AM_AV_INJECT_AUDIO) {
		AM_AV_AudioStatus_t AStatus;
		ErrCode = AM_AV_GetAudioStatus (PLAYER_AVID, &AStatus);
		if( AM_SUCCESS != ErrCode )
		{
			DEBUG_E("AM_AV_GetAudioStatus error %d AM_FALSE\n",ErrCode);
			return 0;
		}		
		uCurLevel = AStatus.ab_data * 100/AStatus.ab_size;
		if(sData) *sData = AStatus.ab_data;
		if(sSize) *sData = AStatus.ab_size;
		/*DEBUG_I("Player_GetCurDataLevel Audio ab_data[%d] ab_size[%d] uCurLevel[%d%]\n",
			AStatus.ab_data,
			AStatus.ab_size,
			uCurLevel);*/
	}
	else {
		AM_AV_VideoStatus_t VStatus;
		ErrCode = AM_AV_GetVideoStatus (PLAYER_AVID, &VStatus);
		if( AM_SUCCESS != ErrCode )
		{
			DEBUG_E("AM_AV_GetVideoStatus error %d AM_FALSE\n",ErrCode);
			return 0;
		}
		uCurLevel = VStatus.vb_data * 100/VStatus.vb_size;
		if(sData) *sData = VStatus.vb_data;
		if(sSize) *sData = VStatus.vb_size;
		/*DEBUG_I("Player_GetCurDataLevel Video vb_data[%d] vb_size[%d] uCurLevel[%d%]\n",
			VStatus.vb_data,
			VStatus.vb_size,
			uCurLevel);*/
	}
	return uCurLevel;
}

static U8 Player_isCanClose()
{
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	S32 dataSize = 0;	
	Player_GetCurDataLevel(&dataSize, NULL);
	DEBUG_E("dataSize[%d]\n", dataSize);
	if(dataSize <= (ONE_FRAME_SIZE(gplay_info.uBitRate)/4)) {
		return 0;
	}
	return 1;
}

static S32 Player_FileSize(U32 fHandel)
{
	S32 sCurOffset = 0, sOffset = 0;
	sCurOffset = lseek64(fHandel, 0, SEEK_CUR);
	if(sCurOffset == -1 && errno != EINTR)
	{
		DEBUG_E("Player_FileSize failed SEEK_CUR\n");
		DEBUG_E( "[error] Player_FileSeek errno[0x%x][%s]\n", errno, strerror(errno));
		return 0;
	}
	sOffset = lseek64(fHandel, 0, SEEK_END);
	if(sOffset == -1 && errno != EINTR)
	{
		DEBUG_E("Player_FileSize failed SEEK_END\n");
		DEBUG_E( "[error] Player_FileSeek errno[0x%x][%s]\n", errno, strerror(errno));
		return 0;
	}
	sCurOffset = lseek64(fHandel, sCurOffset, SEEK_SET);
	if(sCurOffset == -1 && errno != EINTR)
	{
		DEBUG_E("Player_FileSize failed SEEK_SET\n");
		DEBUG_E( "[error] Player_FileSeek errno[0x%x][%s]\n", errno, strerror(errno));
		//return 0;
	}
	return sOffset;
}

static S32 Player_FileHead(U32 fHandel)
{
	S32 sCurOffset = 0, sIndex = 0;
	U8 uTemp[408];
	sCurOffset = lseek64(fHandel, 0, SEEK_CUR);
	if(sCurOffset == -1 && errno != EINTR)
	{
		DEBUG_E("Player_FileHead failed SEEK_CUR\n");
		DEBUG_E( "[error] Player_FileHead errno[0x%x][%s]\n", errno, strerror(errno));
		return 0;
	}
	sIndex = lseek64(fHandel, 0, SEEK_SET);
	if(sIndex == -1 && errno != EINTR)
	{
		DEBUG_E("Player_FileHead failed SEEK_END\n");
		DEBUG_E( "[error] Player_FileHead errno[0x%x][%s]\n", errno, strerror(errno));
		return 0;
	}
	Player_FileRead(fHandel, uTemp, 408);
	sIndex = Player_FindPacketHead(uTemp, 408);
	sCurOffset = lseek64(fHandel, sCurOffset, SEEK_SET);
	if(sCurOffset == -1 && errno != EINTR)
	{
		DEBUG_E("Player_FileHead failed SEEK_SET\n");
		DEBUG_E( "[error] Player_FileHead errno[0x%x][%s]\n", errno, strerror(errno));
		//return 0;
	}
	return sIndex;
}


static void* Player_thread(void *arg)
{
	U32 fHandel = (int)arg;
	static U8 buf[BUFFER_SIZE];
	S32 len, left=0, send, ret;
	S32 offset = 0;
	U8 ErrCount = 0;
	
	DEBUG_I("Player_thread start fHandel[%d] isRunning[%d]\n", fHandel, gplay_info.isRunning);
	readCurIndex = 0;
	memset(buf, 0xFF, BUFFER_SIZE);
	while(gplay_info.isRunning)
	{
		mPlayerLock(gplay_info.uSemId);
		if(gplay_info.player_state == PLAYER_STATE_SF)
		{
			Player_CallEvent(0x013, 0, 0);
			gplay_info.player_state = PLAYER_STATE_PLAY;
			memset(buf, 0xFF, BUFFER_SIZE);
			left = 0;
		}
		len = sizeof(buf)-left;
		if(len) {
			ret = Player_FileRead(gplay_info.pCurFile->ufHandle, buf+left, len);
			if(ret < 0){
				mPlayerUnLock(gplay_info.uSemId);
				goto end; 
			}
			left += ret;
		}
		if(left)
		{
#if (PVR_PLAYER_TYPE == PLAYER_TIMESHIFT)
			send = left > MAXSENDDATA ? MAXSENDDATA : left;
			ret = AM_AV_TimeshiftFillData(PLAYER_AVID, buf, send);
			if(ret != 0) //EIO
			{
				DEBUG_I( "[error] AM_AV_TimeshiftFillData errno[0x%x][%s]\n", errno, strerror(errno));
				DEBUG_I( "[error] AM_AV_TimeshiftFillData [0x%x] send[%d]\n", ret, send);
				gplay_info.isRunning = 0;
				Player_CallEvent(3, 0, 0);
				mPlayerUnLock(gplay_info.uSemId);
				goto end; 
			}
#elif (PVR_PLAYER_TYPE == PLAYER_INJECT)
			send = 0;
			if(gplay_info.player_state != PLAYER_STATE_PAUSE){
				send = left;
				ret = AM_AV_InjectData(PLAYER_AVID, gplay_info.eInjectType, buf, &send, -1);
				if(ret != 0) //EIO
				{
					DEBUG_I( "[error] AM_AV_InjectData errno[0x%x][%s]\n", errno, strerror(errno));
					DEBUG_I( "[error] AM_AV_InjectData [0x%x] send[%d]\n", ret, send);
					gplay_info.isRunning = 0;
					Player_CallEvent(3, 0, 0);
					mPlayerUnLock(gplay_info.uSemId);
					goto end; 
				}
			}
#endif						
			if(send)
			{
				readCurIndex += send;		
				left -= send;
				if(left)
					memmove(buf, buf+send, left);
				Player_CheakKey();
				#ifdef COMPUTE_REAT
				Player_ComputeBitRate(buf, send);			
				Player_BitRateCtrl(send);
				#endif
			}			
		}		
		mPlayerUnLock(gplay_info.uSemId);
	}
end:
#if (PVR_PLAYER_TYPE == PLAYER_INJECT)
	while(gplay_info.isRunning && Player_isCanClose()){ 	
		Player_Sleep(300 * 1000);
	}
	Player_CallEvent(1, 0, 0);
#endif
	gplay_info.isRunning = 0;
	close(fHandel);
	DEBUG_I("inject thread end\n");
	return NULL;
}


static int Player_Creat_File(char *FileName, U8 isClose)
{
	int fd = -1;
	if(FileName == NULL)
	{
		return fd;
	}
	fd = open(FileName, O_TRUNC | O_WRONLY | O_CREAT ,0777);
	if (fd == -1)
	{
		DEBUG_E("Cannot open record file '%s' for DVR%d", FileName);
	}	
	if (fd != -1 && isClose)
	{
		close(fd);
		fd = -1;
	}
	return fd;
}

static void Player_SetTimeshiftName(const char *pName)
{
	U32 index = 0;
	if(pName == NULL)
		return;
	memcpy(gTimeshiftFile, pName, PVR_FILELEN);
	gTimeshiftFile[PVR_FILELEN - 1] = '\0';
	DEBUG_I( "TimeshiftName [%s]\n", gTimeshiftFile);
	index = Player_StrRchr(gTimeshiftFile, '/');
	if(index != -1)
		gTimeshiftFile[index] = '\0';
	else
		gTimeshiftFile[0] = '\0';
	sprintf( gTimeshiftFile, "%s/%s", gTimeshiftFile, TIMESHIFT_FIX );
	DEBUG_I( "TimeshiftName [%s]\n", gTimeshiftFile);
	return;
}


static void Player_keyListInit(KeyInfoList_t *pKeyList)
{
	KeyInfo_t *pKeyTemp = NULL;
	pKeyList->uListLen = 0;
	while(pKeyList->pHead) {
		pKeyTemp = pKeyList->pHead;
		pKeyList->pHead = pKeyTemp->next;
		free(pKeyTemp);
	}
	pKeyList->pHead = NULL;
	pKeyList->pTail = NULL;
}
#if 0
static int Player_compute_BitRate(char *file_path)
{
	FILE *fp = NULL;
	long file_size;
	long time = 0;
	KeyInfo_t *pKeyTemp = NULL;
	KeyInfoList_t *pKeyList = &(gplay_info.KeyList);
	if(pKeyList->pHead == NULL)
		return -1;
	fp = fopen(file_path, "rt");
    if (fp == NULL)
    {
    	DEBUG_E( "Player_compute_BitRate [%s]\n", file_path);
        return -1;
    }
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fclose(fp);
	gplay_info.uFileSize = file_size;
	DEBUG_I( "Player_compute_BitRate size[%ld]\n", file_size);
	pKeyTemp = pKeyList->pHead;
	while(pKeyTemp && pKeyTemp->next)
		pKeyTemp = pKeyTemp->next;
	if(pKeyTemp != NULL)
	time = pKeyTemp->uSetTime - pKeyList->pHead->uSetTime;
	gplay_info.uFileTime = time;
	DEBUG_I( "Player_compute_BitRate time[%ld]\n", time);
	return time ? ((file_size * 8) / (time * 1000)) : 0;
}
#endif

static Player_Error Player_CheakIsEnctypt(KeyInfo_t *pHead, U8 *IsEnctypt)
{
	KeyInfo_t *pKeyTemp = NULL;
	U64 count = 0, oldIndex = -2;
	*IsEnctypt = 0;
	pKeyTemp = pHead;
	while(pKeyTemp) {
		if(oldIndex != pKeyTemp->uSetIndex) {
			count++;
			if(count >= 2){
				*IsEnctypt = 1;
				break;
			}
			oldIndex = pKeyTemp->uSetIndex;			
		}
		pKeyTemp = pKeyTemp->next;
	}
	return PLAYER_NO_ERROR;
}

static void Player_FileListInit(FileInfoList_t *pFileList)
{
	FileInfo_t *pFileTemp = NULL;
	pFileList->uListLen = 0;
	while(pFileList->pHead) {
		pFileTemp = pFileList->pHead;
		pFileList->pHead = pFileTemp->next;
		if(pFileTemp->ufHandle != -1) close(pFileTemp->ufHandle);
		free(pFileTemp);
	}
	pFileList->pHead = NULL;
	pFileList->pTail = NULL;
}


static Player_Error Player_FileList(const *pName)
{
	char pTemp[PVR_FILELEN] = {0};
	U32 ufHandle = -1;
	U32 index = 1;
	FileInfo_t *pFileTemp = NULL;
	FileInfo_t *pFileTemp1 = NULL;
	FileInfoList_t *pFileList = &(gplay_info.FileList);
	
	sprintf( pTemp, "%s", pName);
	gplay_info.ufHandle = -1;
	gplay_info.uFileSize = 0;
	Player_FileListInit(pFileList);
	while((ufHandle = open(pTemp, O_RDONLY | O_LARGEFILE, 0666)) != -1
		&& ((pFileTemp = malloc(sizeof(FileInfo_t))) != NULL))
	{	
		DEBUG_I( "Player_FileList[%s]\n", pTemp);
		memcpy((pFileTemp->pName), pTemp, PVR_FILELEN);
		pFileTemp->pName[PVR_FILELEN - 1] = 0;
		pFileTemp->uFileSize = Player_FileSize(ufHandle);		
		if(gplay_info.ufHandle == -1)
		{
			gplay_info.ufHandle = ufHandle;
			gplay_info.uHeadIndex = Player_FileHead(gplay_info.ufHandle);
			gplay_info.pCurFile = pFileTemp;
			pFileTemp->ufHandle = ufHandle;
		}
		else
		{
			close(ufHandle);
			pFileTemp->ufHandle = -1;
		}
		gplay_info.uFileSize += pFileTemp->uFileSize;
		if(!(pFileList->pHead)) {	
			pFileList->pHead = pFileTemp;	
			pFileList->pHead->next = NULL;
			pFileList->pHead->prev = NULL;
		}
		if( pFileTemp1 ) {	
			pFileTemp1->next = pFileTemp;
			pFileTemp->prev = pFileTemp1;
		}
		sprintf( pTemp, "%s.[%d]", pName, index++ );
		pFileTemp1 = pFileTemp;
		pFileTemp = NULL;
		pFileList->uListLen++;		
	}	
	if(pFileTemp) 	free(pFileTemp);
	if(pFileTemp1) {	
		pFileTemp1->next = NULL;	
		pFileList->pTail = pFileTemp1;
	}
	return PLAYER_NO_ERROR;
}
static Player_Error Player_Config(const *pName)
{
	FILE *fp = NULL;
	char pTemp[PVR_FILELEN + 10] = {0};
	KeyInfo_t *pKeyTemp = NULL;
	KeyInfo_t *pKeyTemp1 = NULL;
	KeyInfoList_t *pKeyList = &(gplay_info.KeyList);
#if (PVR_PLAYER_TYPE == PLAYER_TIMESHIFT)
	AM_AV_TimeshiftPara_t *pPlayerPara = &(gplay_info.PlayerPara);
#elif (PVR_PLAYER_TYPE == PLAYER_INJECT)
	AM_AV_InjectPara_t *pPlayerPara = &(gplay_info.PlayerPara);
#endif	
	if(pName == NULL)
		return PLAYER_ERROR;
	sprintf( pTemp, "%s.%s", pName, CONFIG_FIX );
	fp = fopen(pTemp, "rt");
    if (fp == NULL)
    {
    	DEBUG_E( "[error ]open [%s] failed\n", pTemp);
        return PLAYER_ERROR;
    }
	Player_keyListInit(pKeyList);
	if(fscanf(fp, "[%d][%d][%d][%d]\n", 
		&(pPlayerPara->vid_id), 
		&(pPlayerPara->aud_id),
		&(pPlayerPara->vid_fmt),
		&(pPlayerPara->aud_fmt)) <= 0)
	{
		return PLAYER_ERROR;
	}
	gplay_info.hasvid = 0;
	gplay_info.hasaud = 0;
	gplay_info.isHD = 0;
	if(pPlayerPara->vid_id < 0x1FFF){
		if(pPlayerPara->vid_fmt == 2 || pPlayerPara->vid_fmt == 9)//H264
			gplay_info.isHD = 1;	
		gplay_info.hasvid = 1;		
	}
	if(pPlayerPara->aud_id < 0x1FFF){
		gplay_info.hasaud = 1;
	}
	
#if (PVR_PLAYER_TYPE == PLAYER_INJECT)
	if(gplay_info.hasaud && gplay_info.hasaud)
	{
		gplay_info.eInjectType = AM_AV_INJECT_MULTIPLEX;
	}
	else if(gplay_info.hasvid)
	{
		gplay_info.eInjectType = AM_AV_INJECT_VIDEO;
	}
	else if(gplay_info.hasaud)
	{
		gplay_info.eInjectType = AM_AV_INJECT_AUDIO;
	}
	else
	{
		gplay_info.eInjectType = AM_AV_INJECT_MULTIPLEX;
	}
	
	if(gplay_info.isHD)	{
		gplay_info.MaxLevel = 15;
		gplay_info.MinLevel = 5;
	}
	else {
		gplay_info.MaxLevel = 7;
		gplay_info.MinLevel = 2;
	}
	DEBUG_I( "hasvid:[%d] hasaud:[%d] eInjectType:[%d]\n", 
		gplay_info.hasvid,
		gplay_info.hasaud,
		gplay_info.eInjectType);	
#endif
	DEBUG_I( "pTimeshiftPara:[%d][%d][%d][%d]\n", 
		(pPlayerPara->vid_id), 
		(pPlayerPara->aud_id),
		(pPlayerPara->vid_fmt),
		(pPlayerPara->aud_fmt));
	while(((pKeyTemp = malloc(sizeof(KeyInfo_t))) != NULL) 
		&& (fscanf(fp, "[%llu][%u][%u][%u]:%s\n", 
		&(pKeyTemp->uSetIndex), 
		&(pKeyTemp->uPID),
		&(pKeyTemp->uKeyType),
		&(pKeyTemp->uKetLen),
		(pKeyTemp->KeyData)) > 0))
	{
		pKeyTemp->isUser = 0;
		DEBUG_I( "pKeyTemp:[%llu][%u][%u][%u][%s]\n", 
		(pKeyTemp->uSetIndex), 
		(pKeyTemp->uPID),
		(pKeyTemp->uKeyType),
		(pKeyTemp->uKetLen),
		(pKeyTemp->KeyData));
		ASCIIToCharByHex(pKeyTemp->KeyData, strlen(pKeyTemp->KeyData));
		if(!(pKeyList->pHead)) {	
			pKeyList->pHead = pKeyTemp;	
			pKeyList->pHead->next = NULL;
			pKeyList->pHead->prev = NULL;
		}
		if( pKeyTemp1 ) {	
			pKeyTemp1->next = pKeyTemp;
			pKeyTemp->prev = pKeyTemp1;
		}		
		pKeyTemp1 = pKeyTemp;
		pKeyTemp = NULL;
		pKeyList->uListLen++;
	}
	if(pKeyTemp) 	free(pKeyTemp);
	if(pKeyTemp1) {	
		pKeyTemp1->next = NULL;	
		pKeyList->pTail = pKeyTemp1;
	}
	fclose(fp);	
	//gplay_info.uBitRate = Player_compute_BitRate(pName);
	gplay_info.uBitRate = DEF_BITREAT;
	gplay_info.isDefRate = 1;
	DEBUG_I( "Player_keyConfig uBitRate[%d]\n", gplay_info.uBitRate);
	
	Player_CheakIsEnctypt(pKeyList->pHead, &(gplay_info.isEnctypt));	
	if(!gplay_info.isEnctypt){
		Player_keyListInit(pKeyList);		
	}
	
	return PLAYER_NO_ERROR;
}

static U8 * Player_StatusToStr(pvr_player_status status)
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
static void Player_ShowPlayInfo(player_info_t pInfo)
{
#if 1
	DEBUG_I("\n***********************************************\n");
	DEBUG_I("name:[%s]\n", 				pInfo.name);
	DEBUG_I("last_sta:[0x%x][%s]\n", 	pInfo.last_sta, Player_StatusToStr(pInfo.last_sta));
	DEBUG_I("status:[0x%x][%s]\n", 		pInfo.status, Player_StatusToStr(pInfo.status));
	DEBUG_I("full_time:[%d]\n", 		pInfo.full_time);
	DEBUG_I("current_time:[%d]\n",	 	pInfo.current_time);
	DEBUG_I("current_ms:[%d]\n", 		pInfo.current_ms);
	DEBUG_I("last_time:[%d]\n", 		pInfo.last_time);
	DEBUG_I("error_no:[%d]\n", 			pInfo.error_no);
	DEBUG_I("start_time:[%d]\n", 		pInfo.start_time);
	DEBUG_I("pts_video:[%d]\n", 		pInfo.pts_video);
	DEBUG_I("pts_pcrscr:[%d]\n", 		pInfo.pts_pcrscr);
	DEBUG_I("current_pts:[%d]\n", 		pInfo.current_pts);
	DEBUG_I("curtime_old_time:[%ld]\n",	pInfo.curtime_old_time);
	DEBUG_I("video_error_cnt:[%u]\n", 	pInfo.video_error_cnt);
	DEBUG_I("audio_error_cnt:[%u]\n", 	pInfo.audio_error_cnt);
	DEBUG_I("audio_bufferlevel:[%f]\n", pInfo.audio_bufferlevel);
	DEBUG_I("video_bufferlevel:[%f]\n", pInfo.video_bufferlevel);
	DEBUG_I("\n***********************************************\n");
#endif
}
static void Player_EventCallBack(int dev_no, int event_type, void *param, void *data)
{
	//pvr_player_status
	if(event_type == AM_AV_EVT_PLAYER_UPDATE_INFO) {
		player_info_t pInfo = *(player_info_t *)param;
		//Player_ShowPlayInfo(pInfo);
#if (PVR_PLAYER_TYPE == PLAYER_TIMESHIFT)
		if((!gplay_info.isRunning) 
			&& (pInfo.full_time == gFullTime) 
			&& (pInfo.current_time/1000 >= pInfo.full_time))
			Player_CallEvent(1, 0, 0);
		if(gplay_info.player_state != PLAYER_STATE_PLAY && pInfo.status == PLAYER_RUNNING) {
			Player_CallEvent(0x013, 0, 0);
			gplay_info.player_state = PLAYER_STATE_PLAY;
		}
		gCurrectTime = pInfo.current_time/1000;
		gFullTime = pInfo.full_time;
#endif
		DEBUG_I("Status:[0x%x][%s]\n", 		pInfo.status, Player_StatusToStr((pvr_player_status)pInfo.status));
		DEBUG_I("full_time:[%d]\n", 		pInfo.full_time);
		DEBUG_I("current_time:[%d]\n",	 	pInfo.current_time);
	}
}

static Player_Error Player_DemuxInit()
{
	Player_Error ErrCode = PLAYER_NO_ERROR;
	AM_DMX_OpenPara_t 	para;
	memset(&para, 0, sizeof(para));	
	ErrCode = AM_DMX_Open(PLAYER_DMXID, &para);
	if(ErrCode != AM_SUCCESS && ErrCode != AM_DMX_ERR_BUSY)
	{
		pbiinfo("\n AM_DMX_Open error [0x%x]\n",ErrCode);
		return PLAYER_ERROR;
	}
	ErrCode = AM_DMX_SetSource(PLAYER_DMXID, AM_DMX_SRC_HIU);
	if(ErrCode != AM_SUCCESS)
	{
		DEBUG_E("\n AM_DMX_SetSource error [0x%x]\n",ErrCode);
		return PLAYER_ERROR;
	}
	return PLAYER_NO_ERROR;
}

static Player_Error Player_DemuxUnInit()
{
	Player_Error ErrCode = PLAYER_NO_ERROR;
	ErrCode = AM_DMX_Close(PLAYER_DMXID);
	if(ErrCode != AM_SUCCESS)
	{
		pbiinfo("\n AM_DMX_Open error %d\n",ErrCode);
		return PLAYER_ERROR;
	}
	
	return PLAYER_NO_ERROR;
}



static Player_Error Player_DscInit()
{
//	AM_DSC_OpenPara_t  dsc_para;
	Player_Error ErrRet = PLAYER_NO_ERROR;
//	memset(&dsc_para,0,sizeof(AM_DSC_OpenPara_t));
/*	
	ErrRet = AM_DSC_Close(PLAYER_DSCID);
    if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
        //return ErrRet;
    }
	
	ErrRet = AM_DSC_Open(PLAYER_DSCID, &dsc_para);
    if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
        return ErrRet;
    }
*/
	ErrRet = AM_DSC_SetSource(PLAYER_DSCID, PLAYER_DMXID);
    if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
        return ErrRet;
    }	

	gplay_info.uVDscId = -1;
	if(gplay_info.PlayerPara.vid_id < 0x1FFF){
		ErrRet = AM_DSC_AllocateChannel( PLAYER_DSCID, &(gplay_info.uVDscId));
	    if( PLAYER_NO_ERROR != ErrRet )
	    {
	        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
	        return ErrRet;
	    }
		DEBUG_E("[%s %d]--uVPid--0x%x--\n", DEBUG_LOG, gplay_info.PlayerPara.vid_id );
		ErrRet = AM_DSC_SetChannelPID( PLAYER_DSCID, gplay_info.uVDscId, gplay_info.PlayerPara.vid_id);
	    if( PLAYER_NO_ERROR != ErrRet )
	    {
	        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
	        return ErrRet;
	    }
	}

	gplay_info.uADscId = -1;
	if(gplay_info.PlayerPara.aud_id < 0x1FFF){
		ErrRet = AM_DSC_AllocateChannel( PLAYER_DSCID, &(gplay_info.uADscId));
	    if( PLAYER_NO_ERROR != ErrRet )
	    {
	        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
	        return ErrRet;
	    }
		DEBUG_E("[%s %d]--uAPid--0x%x--\n", DEBUG_LOG, gplay_info.PlayerPara.aud_id );
		ErrRet = AM_DSC_SetChannelPID( PLAYER_DSCID, gplay_info.uADscId, gplay_info.PlayerPara.aud_id);
	    if( PLAYER_NO_ERROR != ErrRet )
	    {
	        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
	        return ErrRet;
	    }
	}
	return ErrRet;
}

static S32 Player_GetDscId(U32 PID)
{
	DEBUG_I("[%s %d]--PID--0x%x--\n", DEBUG_LOG, PID );
	if(gplay_info.PlayerPara.vid_id == PID)
		return gplay_info.uVDscId;
	if(gplay_info.PlayerPara.aud_id == PID)
		return gplay_info.uADscId;
	return -1;
}

static Player_Error  Player_SetKey(S32 ulDescId, AM_DSC_KeyType_t type,U8 *pbEvenKey, U8 bEvenLen)
{
	Player_Error ErrRet = PLAYER_NO_ERROR;
	if(( pbEvenKey == NULL )||( bEvenLen <=0 ) || (ulDescId < 0))
	{
		DEBUG_E("[%s %d]--Parm Error--\n", DEBUG_LOG);
		return PLAYER_NO_ERROR;
	}
	DEBUG_I("[DSR] dsc_setkey DEVID[%d] ulDescId[%d] type[%d] pbEvenKey[0x%02x][0x%02x][0x%02x][0x%02x][0x%02x][0x%02x][0x%02x][0x%02x]\n", PLAYER_DSCID, ulDescId, type, 
		pbEvenKey[0], 
		pbEvenKey[1], 
		pbEvenKey[2], 
		pbEvenKey[3], 
		pbEvenKey[4], 
		pbEvenKey[5], 
		pbEvenKey[6], 
		pbEvenKey[7]);
	ErrRet = AM_DSC_SetKey(PLAYER_DSCID, ulDescId, type, pbEvenKey);
    if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
        return ErrRet;
    }
	return ErrRet;
}
static Player_Error Player_DscUnInit()
{
	Player_Error ErrRet = PLAYER_NO_ERROR;
	
	ErrRet = AM_DSC_SetSource(PLAYER_DSCID, 0);
    if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
        return ErrRet;
    }
	if(gplay_info.uVDscId >= 0){
		ErrRet = AM_DSC_FreeChannel( PLAYER_DSCID, gplay_info.uVDscId);
	    if( PLAYER_NO_ERROR != ErrRet )
	    {
	        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
	        return ErrRet;
	    }
		gplay_info.uVDscId = -1;
	}
	if(gplay_info.uADscId >= 0){
		ErrRet = AM_DSC_FreeChannel( PLAYER_DSCID, gplay_info.uADscId);
	    if( PLAYER_NO_ERROR != ErrRet )
	    {
	        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
	        return ErrRet;
	    }
		gplay_info.uADscId = -1;
	}
/*	
	ErrRet = AM_DSC_Close(PLAYER_DSCID);
    if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
        return ErrRet;
    }
*/
	return ErrRet;
}


static Player_Error Player_AVInit()
{
	Player_Error ErrRet = PLAYER_NO_ERROR;
	ErrRet = DRV_AVCtrl_UnInit();
    if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
        return ErrRet;
    }

    ErrRet = DRV_AVCtrl_Init(0);
    if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
        return ErrRet;
    }
	ErrRet = AM_AV_SetTSSource(PLAYER_AVID, AM_AV_TS_SRC_HIU);//AM_AV_TS_SRC_TS2);
    if( PLAYER_NO_ERROR != ErrRet )
    {
        PBIDEBUG("AM_AV_SetTSSource error!");
        return AVCTRL_ERROR;
    }
	return ErrRet;
}

static Player_Error Player_UnAVInit()
{
	Player_Error ErrRet = PLAYER_NO_ERROR;
	ErrRet = DRV_AVCtrl_UnInit();
    if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
        return ErrRet;
    }    
	return ErrRet;
}

static Player_Error Player_RemoveTsFile()
{
	U8 Temp[PVR_FILELEN + 10] = {0};
	if(gTimeshiftFile[0] != 0){
		remove(gTimeshiftFile);
	}
	return PLAYER_NO_ERROR;
}
static Player_Error Player_ParmInit( const *pName )
{
	Player_Error ErrRet = PLAYER_NO_ERROR;
#if (PVR_PLAYER_TYPE == PLAYER_TIMESHIFT)
	AM_AV_TimeshiftPara_t PlayerPara;
#elif (PVR_PLAYER_TYPE == PLAYER_INJECT)
	AM_AV_InjectPara_t PlayerPara;
#endif	
	if(pName == NULL)
		return PLAYER_ERROR;	
	Player_keyListInit(&(gplay_info.KeyList));
#if (PVR_PLAYER_TYPE == PLAYER_TIMESHIFT)	
	Player_SetTimeshiftName(pName);	
	memset(&gplay_info, 0, sizeof(gplay_info));
	PlayerPara.dmx_id = PLAYER_DMXID;
	PlayerPara.aud_fmt = -1; 
	PlayerPara.vid_fmt = -1;
	PlayerPara.aud_id = -1;
	PlayerPara.vid_id = -1;
	PlayerPara.duration = 3600;
	PlayerPara.file_path = gTimeshiftFile;
	PlayerPara.playback_only = 0;
	Player_Creat_File(gTimeshiftFile, 1);
	DEBUG_I("[%s %d]--pName--[%s]--\n", DEBUG_LOG, gTimeshiftFile );
#elif (PVR_PLAYER_TYPE == PLAYER_INJECT)
	PlayerPara.aud_fmt = -1; 
	PlayerPara.vid_fmt = -1;
	PlayerPara.aud_id = -1;
	PlayerPara.vid_id = -1;
	PlayerPara.pkg_fmt = PFORMAT_TS;	
	gplay_info.eInjectType = AM_AV_INJECT_MULTIPLEX;
#endif

	gplay_info.PlayerPara = PlayerPara;
	ErrRet = Player_FileList(pName);
	if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
        return ErrRet;
    }

	ErrRet = Player_Config(pName);
	if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
        return ErrRet;
    }
	memcpy(gplay_info.pName, pName, PVR_FILELEN);
	gplay_info.pName[PVR_FILELEN - 1] = 0;
    return ErrRet;
}

static Player_Error Player_SemInit()
{
	Player_Error ErrRet = 0;
	gplay_info.uSemId = -1;
	if((ErrRet = OS_SemCreate(&(gplay_info.uSemId),"PvrPlayerSem",1,0)) != 0)
	{
		DEBUG_E("\n Player_SemUnInit :OS_SemCreate failure [%d]\n", ErrRet);
		return PLAYER_ERROR;
	}
	return PLAYER_NO_ERROR;
}

static Player_Error Player_SemUnInit()
{
	Player_Error ErrRet = 0;
	if((ErrRet = OS_SemDelete(gplay_info.uSemId)) != 0)
	{
		DEBUG_E("\n Player_SemUnInit :OS_SemDelete failure [%d]\n", ErrRet);
		return PLAYER_ERROR;
	}
	return PLAYER_NO_ERROR;
}


Player_Error DRV_PVR_PlayerInit(const *pName)
{
	Player_Error ErrRet = PLAYER_NO_ERROR;
	DEBUG_E("[%s %d]--Ver-[%s][%s]--\n", DEBUG_LOG, DEBUG_VER );
	if(isPlayerInit == 1)
	{
		DEBUG_E("[%s %d]--DRV_PVR_PlayInit-already--\n", DEBUG_LOG );
		return PLAYER_NO_ERROR;
	}
	gTimeshiftFile[0] = '\0';
	ErrRet = Player_AVInit();
    if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[error] Player_AVInit\n");
        return ErrRet;
    }
	ErrRet = Player_ParmInit(pName);
    if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[error] Player_ParmInit\n");
        return ErrRet;
    }
	if(gplay_info.isEnctypt)
	{
		ErrRet = Player_DscInit();
		if( PLAYER_NO_ERROR != ErrRet )
	    {
	        DEBUG_E("[error] Player_DscInit\n");
	        return ErrRet;
	    }	
	}
	ErrRet = Player_DemuxInit();
	if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[error] Player_DemuxInit\n");
        return ErrRet;
    }
	ErrRet = Player_SemInit();
	if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[error] Player_SemInit\n");
        return ErrRet;
    }
	ErrRet = AM_EVT_Subscribe(PLAYER_AVID, AM_AV_EVT_PLAYER_UPDATE_INFO, Player_EventCallBack, NULL);
	if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[error] AM_EVT_Subscribe\n");
        return ErrRet;
    }
	isPlayerInit = 1;
	gplay_info.player_state = PLAYER_STATE_INIT_OK;
	return ErrRet;
}

Player_Error DRV_PVR_PlayerUnInit()
{
	Player_Error ErrRet = PLAYER_NO_ERROR;
	if(isPlayerInit == 0)
	{
		DEBUG_E("[%s %d]--Player isn't Init--\n", DEBUG_LOG );
		return PLAYER_NO_ERROR;
	}
	Player_keyListInit(&(gplay_info.KeyList));
	Player_FileListInit(&(gplay_info.FileList));
	ErrRet = Player_UnAVInit();
    if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[error] Player_AVInit\n");
        return ErrRet;
    }

	if(gplay_info.isEnctypt)
	{
		ErrRet = Player_DscUnInit();
		if( PLAYER_NO_ERROR != ErrRet )
	    {
	        DEBUG_E("[error] Player_DscInit\n");
	        return ErrRet;
	    }
	}
	ErrRet = Player_SemUnInit();
    if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[error] Player_SemUnInit\n");
        return ErrRet;
    }
	ErrRet = AM_EVT_Unsubscribe(PLAYER_AVID, AM_AV_EVT_PLAYER_UPDATE_INFO, Player_EventCallBack, NULL);
	if( PLAYER_NO_ERROR != ErrRet )
    {
        DEBUG_E("[error] AM_EVT_Unsubscribe\n");
        return ErrRet;
    }
	DRV_PVR_PlayerRegisterCallBack(NULL);	
#if (PVR_PLAYER_TYPE == PLAYER_TIMESHIFT)	
	Player_RemoveTsFile();
#endif
	isPlayerInit = 0;
	gplay_info.player_state = PLAYER_STATE_UNINIT;
	return ErrRet;
}

Player_Error DRV_PVR_PlayerStart()
{
	Player_Error ErrRet = PLAYER_NO_ERROR;
	if(isPlayerInit == 0)
	{
		DEBUG_E("[%s %d]--Player isn't Init--\n", DEBUG_LOG );
		return PLAYER_ERROR;
	}
#if (PVR_PLAYER_TYPE == PLAYER_TIMESHIFT)
	ErrRet = AM_AV_StartTimeshift (PLAYER_AVID, &(gplay_info.PlayerPara));	
	if (ErrRet != PLAYER_NO_ERROR)
	{
		DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
		return ErrRet; 
	}
#elif (PVR_PLAYER_TYPE == PLAYER_INJECT)
	ErrRet = AM_AV_StartInject (PLAYER_AVID, &(gplay_info.PlayerPara));	
	if (ErrRet != PLAYER_NO_ERROR)
	{
		DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
		return ErrRet; 
	}
#endif

	DEBUG_I( "DRV_PVR_PlayerStart path[%s]\n", gplay_info.pName);	
	gplay_info.isRunning = 1;
	pthread_create(&(gplay_info.pThreadId), NULL, Player_thread, (void*)(gplay_info.ufHandle));
	gplay_info.player_state = PLAYER_STATE_START;
	return ErrRet;
}

Player_Error DRV_PVR_PlayerPlay()
{
	Player_Error ErrRet = PLAYER_NO_ERROR;
	if(isPlayerInit == 0)
	{
		DEBUG_E("[%s %d]--Player isn't Init--\n", DEBUG_LOG );
		return PLAYER_ERROR;
	}
#if (PVR_PLAYER_TYPE == PLAYER_TIMESHIFT)
	ErrRet = AM_AV_PlayTimeshift (PLAYER_AVID);	
	if (ErrRet != PLAYER_NO_ERROR)
	{
		DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
		return ErrRet; 
	}
#elif (PVR_PLAYER_TYPE == PLAYER_INJECT)
	/*ErrRet = AM_AV_StartInject (PLAYER_AVID, &(gplay_info.PlayerPara)); 
	if (ErrRet != PLAYER_NO_ERROR)
	{
		DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
		return ErrRet; 
	}*/
#endif	
	gplay_info.player_state = PLAYER_STATE_PLAY;
	return ErrRet;
}

Player_Error DRV_PVR_PlayerPause()
{
	Player_Error ErrRet = PLAYER_NO_ERROR;
	if(isPlayerInit == 0)
	{
		DEBUG_E("[%s %d]--Player isn't Init--\n", DEBUG_LOG );
		return PLAYER_ERROR;
	}
#if (PVR_PLAYER_TYPE == PLAYER_TIMESHIFT)
	ErrRet = AM_AV_PauseTimeshift (PLAYER_AVID);	
	if (ErrRet != PLAYER_NO_ERROR)
	{
		DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
		return ErrRet; 
	}
#elif (PVR_PLAYER_TYPE == PLAYER_INJECT)	
	ErrRet = AM_AV_PauseInject (PLAYER_AVID); 
	if (ErrRet != PLAYER_NO_ERROR)
	{
		DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
		return ErrRet; 
	}	
#endif	
	gplay_info.player_state = PLAYER_STATE_PAUSE;
	return ErrRet;
}

Player_Error DRV_PVR_PlayerResume()
{
	Player_Error ErrRet = PLAYER_NO_ERROR;
	if(isPlayerInit == 0)
	{
		DEBUG_E("[%s %d]--Player isn't Init--\n", DEBUG_LOG );
		return PLAYER_ERROR;
	}
#if (PVR_PLAYER_TYPE == PLAYER_TIMESHIFT)
	ErrRet = AM_AV_ResumeTimeshift (PLAYER_AVID);	
	if (ErrRet != PLAYER_NO_ERROR)
	{
		DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
		return ErrRet; 
	}	
#elif (PVR_PLAYER_TYPE == PLAYER_INJECT)
	ErrRet = AM_AV_ResumeInject (PLAYER_AVID);	
	if (ErrRet != PLAYER_NO_ERROR)
	{
		DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
		return ErrRet; 
	}	
#endif
	gplay_info.player_state = PLAYER_STATE_PLAY;
	return ErrRet;
}

Player_Error DRV_PVR_PlayerStop()
{
	Player_Error ErrRet = PLAYER_NO_ERROR;
	if(isPlayerInit == 0)
	{
		DEBUG_E("[%s %d]--Player isn't Init--\n", DEBUG_LOG );
		return PLAYER_NO_ERROR;
	}
	if(gplay_info.isRunning == 1){
		gplay_info.isRunning = 0;
		pthread_join(gplay_info.pThreadId, NULL);
	}
#if (PVR_PLAYER_TYPE == PLAYER_TIMESHIFT)
	ErrRet = AM_AV_StopTimeshift (PLAYER_AVID);
	if (ErrRet != PLAYER_NO_ERROR)
	{
		DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
		return ErrRet; 
	}
#elif (PVR_PLAYER_TYPE == PLAYER_INJECT)
	ErrRet = AM_AV_StopInject (PLAYER_AVID);	
	if (ErrRet != PLAYER_NO_ERROR)
	{
		DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
		return ErrRet; 
	}
	Player_PlayClean();
#endif
	gplay_info.player_state = PLAYER_STATE_STOP;	
	return ErrRet;
}

Player_Error DRV_PVR_PlayerSpeed(PVR_SPEED_E ePlaySpeed)
{
	AM_ErrorCode_t	ErrCode = AM_SUCCESS;
	S32 speed = 0, x = 0;
	U64 offset = 0;
	Player_Error ErrRet;
	pbiinfo("[%s %d]--ePlaySpeed[%d]--\n", DEBUG_LOG, ePlaySpeed );
#if (PVR_PLAYER_TYPE == PLAYER_TIMESHIFT)
	speed = ePlaySpeed - PVR_PLAY_SPEED_NORMAL;
	if( speed >= 0 )
	{
		speed = speed * 4;
		pbiinfo("[%s %d]--FF[%d]--\n", DEBUG_LOG, speed );
		ErrCode = AM_AV_FastForwardTimeshift(PLAYER_AVID, speed);
		if (ErrCode != AM_SUCCESS)
		{
			pbierror("[%s %d]--ERROR--0x%x--speed[%d]\n", DEBUG_LOG, ErrCode, speed);
			return PLAYER_ERROR;
		}
		gplay_info.player_state = PLAYER_STATE_FB;
	}
	else
	{
		speed = (-speed) * 4;
		pbiinfo("[%s %d]--FB[%d]--\n", DEBUG_LOG, speed );
		ErrCode = AM_AV_FastBackwardTimeshift(PLAYER_AVID, speed);
		if (ErrCode != AM_SUCCESS)
		{
			pbierror("[%s %d]--ERROR--0x%x--speed[%d]", DEBUG_LOG, ErrCode, speed);
			return PLAYER_ERROR;
		}			
		gplay_info.player_state = PLAYER_STATE_FF;
	}
#elif (PVR_PLAYER_TYPE == PLAYER_INJECT)
	mPlayerLock(gplay_info.uSemId);
	ErrRet = AM_AV_StopInject (PLAYER_AVID);	
	if (ErrRet != PLAYER_NO_ERROR)
	{
		DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
		return PLAYER_ERROR;
	}
	offset = (U64)(ePlaySpeed * (gplay_info.uFileSize / 100));
	DEBUG_I("Player_thread seek offset[%llu]\n", offset);
	offset -= (offset - gplay_info.uHeadIndex)%gPacketLength;
	DEBUG_I("Player_thread seek offset[%llu]\n", offset);
	offset = Player_PlaySeek(offset);
	DEBUG_I("Player_thread seek offset[%llu]\n", offset);
	readCurIndex = offset;	
	Player_SeekKey(offset);	
	ErrRet = AM_AV_StartInject (PLAYER_AVID, &(gplay_info.PlayerPara)); 
	if (ErrRet != PLAYER_NO_ERROR)
	{
		DEBUG_E("[%s %d]--ERROR--0x%x--\n", DEBUG_LOG, ErrRet );
		mPlayerUnLock(gplay_info.uSemId);
		return PLAYER_ERROR;
	}
	speed = ePlaySpeed;	
	gplay_info.player_state = PLAYER_STATE_SF;
	mPlayerUnLock(gplay_info.uSemId);
	
#endif
	gplay_info.uSpeed = speed;
	return PLAYER_NO_ERROR;
}

Player_Error DRV_PVR_PlayerGetStatus(PVR_PLAY_STATUS_t *PVRState)
{
	memset(PVRState, 0, sizeof(PVR_PLAY_STATUS_t));
	PVRState->enState = gplay_info.player_state;
	PVRState->enSpeed = gplay_info.uSpeed;
	PVRState->u32CurPlayTimeInMs = (readCurIndex/1024);
    return 0;
}

Player_Error DRV_PVR_PlayerGetFileAttr(PVR_FILE_ATTR_t *pPvrFileAttr)
{
	memset(pPvrFileAttr, 0, sizeof(PVR_FILE_ATTR_t));
	pPvrFileAttr->u32StartTimeInMs = 0;
    pPvrFileAttr->u32EndTimeInMs = (gplay_info.uFileSize/1024);
    return 0;
}


Player_Error DRV_PVR_PlayerRegisterCallBack(EventCallBack pEventCallBack)
{
	gPlayerEventCb = pEventCallBack;
	return PLAYER_NO_ERROR;
}

#if 0
int main(int argc, char *args[])
{
	if(argc < 2)
	return 1;
	DRV_PVR_PlayerInit(args[1]);
	DRV_PVR_PlayerStart();
	DRV_PVR_PlayerPlay();
	while(gplay_info.isRunning)
	{
		usleep(200 * 1000);
	}
	DRV_PVR_PlayerStop();
	DRV_PVR_PlayerUnInit();
	return 0;
}
#endif

#undef LOG_TAG

