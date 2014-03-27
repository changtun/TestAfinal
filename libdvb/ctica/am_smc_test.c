/***************************************************************************
 *  Copyright C 2009 by Amlogic, Inc. All Rights Reserved.
 */
/**\file
 * \brief 鏅鸿兘鍗℃祴璇曠▼搴? *
 * \author Gong Ke <ke.gong@amlogic.com>
 * \date 2010-07-05: create the document
 ***************************************************************************/

#define AM_DEBUG_LEVEL 0

#include <am_debug.h>
#include <am_smc.h>
#include <string.h>
#include <unistd.h>
#include "drv_tuner.h"
#include "dvbplayer.h"
#define AM_TRY(_func) \
	do {\
	AM_ErrorCode_t _ret;\
	printf("[zshang][%s][%d]\n", __FUNCTION__, __LINE__);\
	if ((_ret=(_func))!=AM_SUCCESS)\
		printf("[zshang][%x]\n", _ret);\
	} while(0)

#define SMC_DEV_NO (0)


static void smc_cb(int dev_no, AM_SMC_CardStatus_t status, void *data)
{
	if(status==AM_SMC_CARD_IN)
	{
		printf("cb: card in!\n");
	}
	else
	{
		printf("cb: card out!\n");
	}
}
#if 0
#define S32 		int
#define U8 		unsigned char
#define U32 		unsigned int
#define BOOL 	bool
#define TRUE		true
#define FALSE	false

/****************************************************************
 * 函数名称 : void SMART_T0_ProcessProcedureBytes()
 * 函数功能 : 
 * 输入参数 : 无
 * 返回参数 : 
 * 全局变量 : 
 * 调用函数 : 
 * 创建人   : 
 * 创建日期 : 
 * 修改人   : 
 * 修改日期 : 
 * 版本     : 0.1
 ****************************************************************/
static S32 SMART_T0_ProcessProcedureBytes(U8 INS,U32 WriteSize,U32 ReadSize,U8 *Buf_p,U8 *Size_p,U32 *NextWriteSize_p,U32 *NextReadSize_p)
{
	S32 Error = FALSE;
	S32 CallError = FALSE;
	S32 MoreProcedureBytes;
	S32 actReadLen =0, Timeout= 4000;

	U8 ack, i;
	U32 NextWriteSize;

	/* Assume no more data to be sent by IFD */
	NextWriteSize = 0;

	/* Assume that the next read is all available bytes */
	*NextReadSize_p = ReadSize;
	i = 0;                              /* Procedure byte count */
	do                                  /* Process each procedure byte */
	{
		/* Assume no more procedure bytes to come */
		MoreProcedureBytes = FALSE;
		//pbierror("HI_UNF_SCI_Receive  line %d   \n    ",__LINE__);
		/* Try to read an available procedure byte */
		actReadLen = AM_SMC_ReadEx(g_u8SCIPort,&ack,1, Timeout);	
		if (actReadLen >= AM_SUCCESS)/* Do we have a procedure byte? */
		{
			/* Set the procedure byte in the status structure */
		//	pbierror("HI_UNF_SCI_Receive  line %d   ok \n    ",__LINE__);
			Buf_p[i] = ack;

			if (i == 0)                 /* First procedure byte? */
			{
				/* Check for ACK byte */
				if ((INS ^ ack) == 0x00) /* ACK == INS */
				{
					/* Vpp should be set idle - all bytes can go */

					if (WriteSize > 0)
					{
						/* Send all remaining bytes */
						NextWriteSize = WriteSize;
					}
					else if (ReadSize == 0)
					{
						/* Await further procedure bytes */
						MoreProcedureBytes = TRUE;
						i = 0;
						continue;
					}
				}
				else if ((INS ^ ack) == 0xFF) /* ACK == ~INS */
				{
					/* Vpp should be set idle - one byte can go */
					if (WriteSize > 0)
					{
						/* Send the next byte only */
						NextWriteSize = 1;
					}
					else if (ReadSize == 0)
					{
						/* No more bytes available - await further bytes */
						MoreProcedureBytes = TRUE;
						i = 0;
						continue;
					}
					else
					{
						*NextReadSize_p = 1;
					}
				}
				else if ((INS ^ ack) == 0x01) /* ACK == INS+1 */
				{
					/* Vpp should be set active - all bytes can go */
					if (WriteSize > 0)
					{
						/* Send all remaining bytes */
						NextWriteSize = WriteSize;
					}
					else if (ReadSize == 0)
					{
						/* No more bytes available - await further bytes */
						MoreProcedureBytes = TRUE;
						i = 0;
						continue;
					}
				}
				else if ((INS ^ ack) == 0xFE) /* ACK == ~INS+1 */
				{
					/* Vpp should be set active - next byte can go   */
					if (WriteSize > 0)
					{
						/* Send the next byte only */
						NextWriteSize = 1;
					}
					else if (ReadSize == 0)
					{
						/* No more bytes available - await further bytes */
						MoreProcedureBytes = TRUE;
						i = 0;
						continue;
					}
					else
					{
						*NextReadSize_p = 1;
					}
				}
				else if ((ack & 0xF0) == 0x60) /* SW1 or NULL */
				{
					/* Get next procedure byte too */
	   				MoreProcedureBytes = TRUE;

					/* 0x60 is a 'null' byte, which indicates "please
					 * wait". Therefore we shouldn't modify the bytes
					 * expected when we get that. The others are errors.
					 */
					if (ack != 0x60)
					{
						*NextReadSize_p = 0;
					}

					switch (ack)
					{
					case 0x60:
						i = 0;      /* New sequence of bytes to come */
						continue;
					case 0x6E:      /* SW1 byte */
					case 0x6D:      /* SW1 byte */
					case 0x68:      /* SW1 byte */
					case 0x67:      /* SW1 byte */
						Error = (-1);
						break;
					default:
					case 0x6F:      /* SW1 byte */
						Error = (-1);
						break;
					}
				}
				else if ((ack & 0xF0) == 0x90) /* SW1 byte */
				{
					/* Next procedure byte is the final one */
					MoreProcedureBytes = TRUE;
					*NextReadSize_p = 0;
				}
				else
				{
					/* Unrecognised status byte */
					Error = (-1);
				}
			}
			else
			{
				/* This was SW2, so we should set the error status now */
			}
		}
		else
		{
			pbierror("No answer from the card  ERROR 0x%x\n\n",CallError);
			/* No answer from the card */
			Error = (-1);
			break;
		}
		i++;                        /* Next procedure byte */
	} while (MoreProcedureBytes);

	/* Set number of procedure bytes received */
	*Size_p = i;

	/* Set number of bytes to write in next IO transfer */
	*NextWriteSize_p = NextWriteSize;

	/* Make sure that any errors from any functions we called
	 * get passed back up.
	 */
	if (Error == AM_SUCCESS)
	{
		Error = CallError;
	}
	//printf("SMART_T0_ProcessProcedureBytes OUT <<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");

	/* Return error code */
	return Error;
} /* SMART_T0_ProcessProcedureBytes() */


/****************************************************************
 * 函数名称 : BOOLEAN HWC_CASCDRV_SCSendData ( BYTE  *pucCmdBuffer,
 BYTE  *pucDataBuffer,
 BYTE  *pucSW1,
 BYTE  *pucSW2 )

 * 函数功能 : 
 * 输入参数 : 无
 * 返回参数 : 
 * 全局变量 : 
 * 调用函数 : 
 * 创建人   : XuHong
 * 创建日期 : 2002.10.18
 * 修改人   : 
 * 修改日期 : 
 * 版本     : 0.1
 ****************************************************************/
static BOOL HWC_CASCDRV_SCSendData ( U8        *pucCmdBuffer,
								 		   U8        *pucDataBuffer,
								           U8        *pucSW1,
								           U8        *pucSW2 )
{
	U32 ActwriteLen = 0,
	    Actread_len = 0, 
	    Senddata_len = 0;
	U32 Timeout = 5000;
	U32  Writelen = T0_CMD_LENGTH;
	AM_ErrorCode_t ErrCode = AM_SUCCESS;
	U16 dataTotalWrite = 0;
	U8 Ins =0,SW_buf[2]={0},sw_len = 0;
	AM_SMC_CardStatus_t  tSCIStatus;
	
	ErrCode = AM_SMC_GetCardStatus(g_u8SCIPort,&tSCIStatus);
	if ( ErrCode == AM_SUCCESS )
	{
		if(tSCIStatus == AM_SMC_CARD_OUT)
		{
			return TRUE;
		}
	}

	while (Writelen > 0 && ErrCode == AM_SUCCESS)
	{
	//	pbierror("HI_UNF_SCI_Send  line %d   \n    ",__LINE__);
		ActwriteLen = AM_SMC_WriteEx( g_u8SCIPort, pucCmdBuffer, Writelen, Timeout);
		if (ActwriteLen < AM_SUCCESS)
		{
			pbierror("SC_Write error %x, %s, %d\n",ErrCode,__FILE__,__LINE__);
			return TRUE;
		}
	//	pbierror("HI_UNF_SCI_Send  line %d    OK\n	 ",__LINE__);

		Ins = pucCmdBuffer[T0_INS_OFFSET];
		//Writelen -= T0_CMD_LENGTH;
		Writelen -= ActwriteLen;
		ErrCode = SMART_T0_ProcessProcedureBytes(Ins,Writelen,1,SW_buf,&sw_len, &ActwriteLen, &Actread_len);
//		printf("3 ErrCode=%d, SW_buf[0]=%x, SW_buf[1]=%x, sw_len=%d,Actread_len=%d, ActwriteLen=%d\n",ErrCode,SW_buf[0],SW_buf[1],sw_len, Actread_len,ActwriteLen);
	}

	dataTotalWrite = 0;
	Senddata_len = pucCmdBuffer[T0_P3_OFFSET];
	ActwriteLen = Senddata_len;
	while (ErrCode == AM_SUCCESS && ActwriteLen > 0 && Senddata_len > 0)
	{
		/* Send data response */
	//	pbierror("HI_UNF_SCI_Send  line %d   \n    ",__LINE__);
		ActwriteLen = AM_SMC_WriteEx( g_u8SCIPort, &pucDataBuffer[dataTotalWrite], Senddata_len, Timeout);
		/* Handle procedure bytes */
		if (ActwriteLen < AM_SUCCESS)
		{
			pbierror("SC_Read error %x, %s, %d\n",ErrCode,__FILE__,__LINE__);
			return TRUE;
		}
	//	pbierror("HI_UNF_SCI_Send  line %d	OK\n	 ",__LINE__);

//		printf("5 ErrCode=%d, pucDataBuff[0]=%x, pucDataBuff[1]=%x, Actread_len=%d\n",ErrCode,pucDataBuffer[0],pucDataBuffer[1], Actread_len);
		Senddata_len -= ActwriteLen;
		dataTotalWrite += ActwriteLen;
		ErrCode = SMART_T0_ProcessProcedureBytes(Ins,Senddata_len,0,SW_buf,&sw_len, &ActwriteLen, &Actread_len);
//		printf("6   TotalRead=%d,Readdata_len=%d,ActwriteLen=%d\n",dataTotalWrite, Senddata_len, ActwriteLen);
	}
	*pucSW1 = SW_buf[0];
	*pucSW2 = SW_buf[1];

	if (ErrCode==AM_SUCCESS) 
	{
		return FALSE; 
	}	 
	else
	{
		if((SW_buf[0] == 0x61) ||(SW_buf[0] == 0x6B) ||(SW_buf[0] == 0x95) || (SW_buf[0] == 0x94))
			return FALSE;
		return TRUE;  
	}
}


unsigned char CTISC_Command(unsigned char bLength,unsigned char* pabMessage,unsigned char* pabReplay)
{
	U8 	char_value[5];
	U8 	data[256];
	U8  SW1,SW2;
	U8  cmd_resend = 0;
	U8	iError =1;
#if 0	
	int i;
	unsigned char command[256];
	unsigned char chLRC;
	memcpy(command+3,pabMessage,bLength);
	command[ 0 ] = 0;
	command[ 1 ] = 0;
	command[ 2 ] = (unsigned char)(bLength);
	for(i = 0; i < 3+(bLength); i++)
	{
		chLRC^= command[ i ];
	}
	command[ bLength+3 ] = chLRC;
#endif

	while (iError)
	{
		//if(ulTxLength>T0_CMD_LENGTH)/*write coomand*/
		{
			iError = HWC_CASCDRV_SCSendData(char_value,data,&SW1,&SW2);
//			printf("send error %d ", iError);
		//	pbRxData [	0 ] = SW1;
		//	pbRxData [	1 ] = SW2;
		//	*pulRxLength = 2;
		}
		//else/*read command*/
		{
		//	iError = HWC_CASCDRV_SCReceiveData ( char_value, pbRxData, (U32*)pulRxLength, &SW1, &SW2 );
		//	pbierror("receive iError %d \n", iError);
		#if 0
			for(i = 0; i < *pwReplyLen; i++)
			{
				printf("%02x	", pbyReply [i]);
			}
			printf("\n");
		#endif
		//		pbRxData [	*pulRxLength ] = SW1;
		//		pbRxData [	*pulRxLength+1 ] = SW2;
		//		*pulRxLength+=2;
	
			}
			cmd_resend++;//-a hrq 10/08/16
	//		printf("######################### SW1:%x, SW2:%x\n\n",SW1,SW2);
	
			if (((SW1&0xf0)!=0x60)&&((SW1&0xf0)!=0x90))
				pbierror("######################### SW1:%x, SW2:%x\n",SW1,SW2);
			if (cmd_resend>3)
				break;
		}
#if 0		
		if (((SW1&0xf0)!=0x60)&&((SW1&0xf0)!=0x90)&&(cmd_resend>3))
		{
			pbierror("TRDRV_SCARD_AdpuFunction 	 *reset card\n\n");
			AM_ErrorCode_t ErrCode = AM_SUCCESS;
			AM_SMC_CardStatus_t  tSCIStatus;
			ErrCode = AM_SMC_GetCardStatus(g_u8SCIPort,&tSCIStatus);
			if ( ErrCode == AM_SUCCESS )
			{
				if(tSCIStatus == AM_SMC_CARD_IN)
				{
					U8  atr_tmp[40]={0};
				 	U8 	atr_recv_count = 40;
					memset(atr_tmp,0,sizeof(atr_tmp));
					ErrCode = AM_SMC_Reset(g_u8SCIPort, atr_tmp, &atr_recv_count);//no need get ATR to calib 
					if ( ErrCode != AM_SUCCESS )
					{
						pbierror("error error %x+++++++++++++++++++++++++++++++++++++++++++\n\n\n",ErrCode);
					}
					usleep(1000*1000);
				}
			}			
		}
#endif	
}
#endif
static int smc_test(AM_Bool_t sync, int freq)
{
	AM_SMC_OpenPara_t para;
	uint8_t atr[AM_SMC_MAX_ATR_LEN];
	int i, len;
	AM_SMC_CardStatus_t status;
	uint8_t sbuf[5]={0x80, 0x44, 0x00, 0x00, 0x08};
	uint8_t rbuf[256];
	uint8_t sbuf1[9]={0x0, 0x16, 0, 0x00, 0x04,0,0,0,0};
	uint8_t rbuf1[256];
	uint8_t sbuf2[5]={0x0, 0x18, 0, 0x00, 0x10};
	uint8_t rbuf2[256];
	AM_SMC_Param_t SMCpara;
	int rlen = sizeof(rbuf);
	int rlen1 =sizeof(rbuf1);
	int rlen2 =sizeof(rbuf2);
	
	memset(&para, 0, sizeof(para));
	para.enable_thread = !sync;
	AM_TRY(AM_SMC_Open(SMC_DEV_NO, &para));
	//AM_TRY(AM_SMC_Active(SMC_DEV_NO));
	
	if(!sync)
	{
		AM_SMC_SetCallback(SMC_DEV_NO, smc_cb, NULL);
	}
	printf("please insert a card\n");
	
	do {
		AM_TRY(AM_SMC_GetCardStatus(SMC_DEV_NO, &status));
		usleep(100000);
	} while(status==AM_SMC_CARD_OUT);
	
	printf("card in\n");

	AM_TRY(AM_SMC_GetParam(SMC_DEV_NO, &SMCpara));
	 printf("[zshang_old]\n SMCpara.f[%d]\n SMCpara.d[%d]\n SMCpara.n[%d]\n SMCpara.bwi[%d]\n SMCpara.cwi[%d]\n SMCpara.bgt[%d]\n SMCpara.freq[%d]\n SMCpara.recv_invert[%d]\n SMCpara.recv_lsb_msb[%d]\n SMCpara.recv_no_parity[%d]\n SMCpara.xmit_invert[%d]\n SMCpara.xmit_lsb_msb[%d]\n SMCpara.xmit_retries[%d]\n SMCpara.xmit_repeat_dis[%d]\n", 
	 SMCpara.f,
	 SMCpara.d,
	 SMCpara.n,
	 SMCpara.bwi,
	 SMCpara.cwi,
	 SMCpara.bgt,
	 SMCpara.freq,
	 SMCpara.recv_invert,
	 SMCpara.recv_lsb_msb,
	 SMCpara.recv_no_parity,
	 SMCpara.xmit_invert,
	 SMCpara.xmit_lsb_msb,
	 SMCpara.xmit_retries,
	 SMCpara.xmit_repeat_dis);
	 if(freq)
	 	SMCpara.freq = freq;
	AM_TRY(AM_SMC_SetParam(SMC_DEV_NO, &SMCpara));
	
	AM_TRY(AM_SMC_GetParam(SMC_DEV_NO, &SMCpara));
	printf("[zshang_new]\n SMCpara.f[%d]\n SMCpara.d[%d]\n SMCpara.n[%d]\n SMCpara.bwi[%d]\n SMCpara.cwi[%d]\n SMCpara.bgt[%d]\n SMCpara.freq[%d]\n SMCpara.recv_invert[%d]\n SMCpara.recv_lsb_msb[%d]\n SMCpara.recv_no_parity[%d]\n SMCpara.xmit_invert[%d]\n SMCpara.xmit_lsb_msb[%d]\n SMCpara.xmit_retries[%d]\n SMCpara.xmit_repeat_dis[%d]\n", 
	 SMCpara.f,
	 SMCpara.d,
	 SMCpara.n,
	 SMCpara.bwi,
	 SMCpara.cwi,
	 SMCpara.bgt,
	 SMCpara.freq,
	 SMCpara.recv_invert,
	 SMCpara.recv_lsb_msb,
	 SMCpara.recv_no_parity,
	 SMCpara.xmit_invert,
	 SMCpara.xmit_lsb_msb,
	 SMCpara.xmit_retries,
	 SMCpara.xmit_repeat_dis);
	 
	len = sizeof(atr);
	AM_TRY(AM_SMC_Reset(SMC_DEV_NO, atr, &len));
	printf("ATR: ");
	for(i=0; i<len; i++)
	{
		printf("%02x ", atr[i]);
	}
	printf("\n");

#if 0	
	AM_TRY(AM_SMC_TransferT0(SMC_DEV_NO, sbuf1, sizeof(sbuf1), rbuf1, &rlen1));
	printf("send: ");
	for(i=0; i<sizeof(sbuf1); i++)
	{
		printf("%02x ", sbuf1[i]);
	}
	printf("\n");
	
	printf("recv: ");   
	for(i=0; i<rlen1; i++)
	{
		printf("%02x ", rbuf1[i]);
	}
	printf("\n");
	AM_TRY(AM_SMC_TransferT0(SMC_DEV_NO, sbuf2, sizeof(sbuf2), rbuf2, &rlen2));
	printf("send2: ");
	for(i=0; i<sizeof(sbuf1); i++)
	{
		printf("%02x ", sbuf1[i]);
	}
	printf("\n");
	
	printf("recv2: ");   
	for(i=0; i<rlen2; i++)
	{
		printf("%02x ", rbuf1[i]);
	}
	printf("\n");


	
	do {
		AM_TRY(AM_SMC_GetCardStatus(SMC_DEV_NO, &status));
		usleep(100000);
	} while(status==AM_SMC_CARD_IN);
	
	printf("card out---------\n");
	do {
			AM_TRY(AM_SMC_GetCardStatus(SMC_DEV_NO, &status));
			usleep(100000);
		} while(status==AM_SMC_CARD_OUT);

	len = sizeof(atr);
		AM_TRY(AM_SMC_Reset(SMC_DEV_NO, atr, &len));
		printf("ATR:---------------- ");
		for(i=0; i<len; i++)
		{
			printf("%02x ", atr[i]);
		}
		printf("\n");
		rlen1 = sizeof(rbuf1);
		
		AM_TRY(AM_SMC_TransferT0(SMC_DEV_NO, sbuf1, sizeof(sbuf1), rbuf1, &rlen1));
		printf("send3: ");
		for(i=0; i<sizeof(sbuf1); i++)
		{
			printf("%02x ", sbuf1[i]);
		}
		printf("\n");
		
		printf("recv3: ");	
		for(i=0; i<rlen1; i++)
		{
			printf("%02x ", rbuf1[i]);
		}
#endif
		printf("\n");
	AM_TRY(AM_SMC_Close(SMC_DEV_NO));


	
	return 0;
}

void open_screen()
{
	system("echo 1 > /sys/class/graphics/fb0/blank");
}

int set_ca_frequency()
{
	int i = 0;
	ts_src_info_t ptSrcInfo;
	ptSrcInfo.MediumType = MEDIUM_TYPE_DVBC;
	ptSrcInfo.u.Cable.FrequencyKHertz = 714000;
	ptSrcInfo.u.Cable.Modulation = 2;
	ptSrcInfo.u.Cable.SymbolRateSPS = 6875;
	ptSrcInfo.u.Cable.Bandwidth = ePI_BW_8MHZ;
	DRV_Tuner_SetFrequency(0, &ptSrcInfo, 4000);
	while(1)
	{
		if(DRV_Tuner_GetLockStatus( 0 )==1)
		{
			printf("lock .....\n");
			return 1;
		}
		printf(" un lock\n");
		usleep(50000);
		i ++;
		if(i>20)
		{
			break;
		}
	}

	return 0;

}
void cti_play(int type)
{
	DVBPlayer_Play_Param_t pParam;
    DVBCore_Signal_Source_e     eSignalSource;
    U8                          u8TunerId;
    U32					        u32Freq;
    U32					        u32SymbRate;
    DVBCore_Modulation_e        eMod;
    DVBCore_Conv_Code_Rate_e    eFEC_Inner;
    DVBCore_FEC_Outer_e         eFEC_Outer;
open_screen();	
	pParam.uTunerParam.tCable.eSignalSource = eDVBCORE_SIGNAL_SOURCE_DVBC;
	pParam.uTunerParam.tCable.u8TunerId = 0;
	pParam.uTunerParam.tCable.u32Freq = 714000;
	pParam.uTunerParam.tCable.u32SymbRate = 6875;
	pParam.uTunerParam.tCable.eMod = 2;
	pParam.uTunerParam.tCable.eFEC_Inner = 0;
	pParam.uTunerParam.tCable.eFEC_Outer = 0;
if(type==0)
{
	pParam.uProgParam.tServParam.eProgType = eDVBPLAYER_PROG_TYPE_SERVICE;
	pParam.uProgParam.tServParam.eServLockFlag = 0;
	pParam.uProgParam.tServParam.u16ServId = 102;
	pParam.uProgParam.tServParam.u16PMTPid = 0x401;
	pParam.uProgParam.tServParam.u16TsId = 1;
	pParam.uProgParam.tServParam.u16NetId = 0x4080;
	pParam.uProgParam.tServParam.u16LogicNum = 0;
	pParam.uProgParam.tServParam.u16historyAudioPid = 0;
}
else if(type==1)
{
	pParam.uProgParam.tServParam.eProgType = eDVBPLAYER_PROG_TYPE_SERVICE;
	pParam.uProgParam.tServParam.eServLockFlag = 0;
	pParam.uProgParam.tServParam.u16ServId = 1;
	pParam.uProgParam.tServParam.u16PMTPid = 0x10d;
	pParam.uProgParam.tServParam.u16TsId = 1;
	pParam.uProgParam.tServParam.u16NetId = 0x457;
	pParam.uProgParam.tServParam.u16LogicNum = 0;
	pParam.uProgParam.tServParam.u16historyAudioPid = 0;
}
else if(type==2)
{
	pParam.uProgParam.tServParam.eProgType = eDVBPLAYER_PROG_TYPE_SERVICE;
	pParam.uProgParam.tServParam.eServLockFlag = 0;
	pParam.uProgParam.tServParam.u16ServId = 2;
	pParam.uProgParam.tServParam.u16PMTPid = 0x10c;
	pParam.uProgParam.tServParam.u16TsId = 1;
	pParam.uProgParam.tServParam.u16NetId = 0x457;
	pParam.uProgParam.tServParam.u16LogicNum = 0;
	pParam.uProgParam.tServParam.u16historyAudioPid = 0;

}
else if(type==3)
{
	pParam.uProgParam.tServParam.eProgType = eDVBPLAYER_PROG_TYPE_SERVICE;
	pParam.uProgParam.tServParam.eServLockFlag = 0;
	pParam.uProgParam.tServParam.u16ServId = 3;
	pParam.uProgParam.tServParam.u16PMTPid = 0x10b;
	pParam.uProgParam.tServParam.u16TsId = 1;
	pParam.uProgParam.tServParam.u16NetId = 0x457;
	pParam.uProgParam.tServParam.u16LogicNum = 0;
	pParam.uProgParam.tServParam.u16historyAudioPid = 0;
}
else if(type==4)
{
	pParam.uProgParam.tServParam.eProgType = eDVBPLAYER_PROG_TYPE_SERVICE;
	pParam.uProgParam.tServParam.eServLockFlag = 0;
	pParam.uProgParam.tServParam.u16ServId = 4;
	pParam.uProgParam.tServParam.u16PMTPid = 0x109;
	pParam.uProgParam.tServParam.u16TsId = 1;
	pParam.uProgParam.tServParam.u16NetId = 0x457;
	pParam.uProgParam.tServParam.u16LogicNum = 0;
	pParam.uProgParam.tServParam.u16historyAudioPid = 0;
}
else
{
	pParam.uProgParam.tServParam.eProgType = eDVBPLAYER_PROG_TYPE_SERVICE;
	pParam.uProgParam.tServParam.eServLockFlag = 0;
	pParam.uProgParam.tServParam.u16ServId = 3;
	pParam.uProgParam.tServParam.u16PMTPid = 0x10b;
	pParam.uProgParam.tServParam.u16TsId = 1;
	pParam.uProgParam.tServParam.u16NetId = 0x457;
	pParam.uProgParam.tServParam.u16LogicNum = 0;
	pParam.uProgParam.tServParam.u16historyAudioPid = 0;
	
}
	DVBPlayer_Play_Program( &pParam );

}
void CTI_SendMessageTOCA()
{
	//CTICAS_SendMsgToCAcore(SC_INFO_QUERY,NULL,0);
}

void free_command()
{
	system("free");
}
  
int main(int argc, char **argv)
{
	//int freq = 0;
	//if(argc > 1)
	//freq = atoi(argv[1]);
	//printf("sync mode test[%d]\n", freq);
	//smc_test(AM_TRUE, freq);
	
	//printf("async mode test\n");
	//smc_test(AM_FALSE);
	char buf[200];
	int running = 1;
	
	Pvware_DRV_Init();
	DrvPlayerInit(100);
	
	DVBTable_Init();
	DVBPlayer_Init(NULL);
	cti_set_call_back();	
	CA_Init();

	if(set_ca_frequency()==1)
	{
		printf(" lock \n");
	}
	else
	{
		printf(" no lock \n");
	}

	while(running)
	{
		if(gets(buf))
		{
			if(!strncmp(buf, "quit", 4))
			{
				running = 0;
				close_sc();
			}
			else if(!strncmp(buf, "run", 3))
			{
				CTI_SendMessageTOCA();
			}
			else if(!strncmp(buf, "test", 4))
			{
				cit_test();
			}
			else if(!strncmp(buf, "reset", 5))
			{
				CTISC_Reset();
			}
			else if(!strncmp(buf, "play0", 5))
			{
				cti_play(0);
			}			
			else if(!strncmp(buf, "play1", 5))
			{
				cti_play(1);
			}
			else if(!strncmp(buf, "play2", 5))
			{
				cti_play(2);
				
			}
			else if(!strncmp(buf, "play3", 5))
			{
				cti_play(3);
				
			}
			else if(!strncmp(buf, "play4", 5))
			{
				cti_play(4);				
			}
			else if(!strncmp(buf, "check",5))
			{
				CTI_TEST(buf);
			}
			else 
			{
				free_command();
			}
		}
		OS_TaskDelay(200);
	}
	return 0;
}

