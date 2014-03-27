/*******************************************************************

  Source file name : ctisc.h

  Description: 
	SC Driver API Interfaces for the Compunicate Smart Card driver   

   COPYRIGHT (C) Compunicate Technologies, Inc. 2003-2005

*********************************************************************/

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "osapi.h" 
#include "pbitrace.h"
#include "am_smc.h"
#include "ctisc.h"
#include "ca.h"

//#define	CTI_DEBUG
#define		CTI_TEST

#define AM_TRY(_func) \
	do {\
	AM_ErrorCode_t _ret;\
	printf("[CA][%s][%d]\n", __FUNCTION__, __LINE__);\
	if ((_ret=(_func))!=AM_SUCCESS)\
		printf("[CA][%x]\n", _ret);\
	} while(0)

typedef S16 (*SC_ATR_Notify)(U8 bCardNumber, U8 bStatus, U8 *pbATR, U8 bProtocol);

typedef enum{
	CARD_OUT_EVENT,
	CARD_IN_EVENT,
}Card_Event_t;

static SC_ATR_Notify sc_callback_func = NULL;
static U8 g_u8SCIPort = 0;
static U32	 Detect_signal = 0xffffffff;
static Card_Event_t Global_SC_STATS=CARD_OUT_EVENT;
static S32 card_detect_taskid =0xffffffff;
int Cas_Get_Last_Err()
{
	return 0;
}

int CTISC_GetScStatus()
{
	S32  	 ErrCode;/*times*/
	AM_SMC_CardStatus_t tSCIStatus;
	
	ErrCode = AM_SMC_GetCardStatus(g_u8SCIPort,&tSCIStatus);
	if ( ErrCode == AM_SUCCESS )
	{
		if(tSCIStatus == AM_SMC_CARD_IN)
		{
			return CARD_IN_EVENT;
		}	
		else
		{
			return CARD_OUT_EVENT;  
		}
	}

	return 2;  
}

void SC_SetParam()
{
	AM_SMC_Param_t SMCpara;
	AM_TRY(AM_SMC_GetParam(g_u8SCIPort, &SMCpara));
#if 0	
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
#endif	 
	SMCpara.freq = 6000;
	SMCpara.bwi = 4;
	SMCpara.cwi =5;
	
	AM_TRY(AM_SMC_SetParam(g_u8SCIPort, &SMCpara));
#if 0	
	AM_TRY(AM_SMC_GetParam(g_u8SCIPort, &SMCpara));
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
#endif	
}

/****************************************************************
 * 函数名称 : void SMARTCARD_Detect_notify(void *pvParam)
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
static void SMARTCARD_Detect_notify(void)
{

	S32  	 ErrCode;/*times*/
	Card_Event_t eventStatus=CARD_OUT_EVENT;
	AM_SMC_CardStatus_t tSCIStatus;
	S32 s32ResetTime;	
	U8 atr_tmp[255];
	int atr_recv_count;
	static int cardouttimes =0;
	
	while(TRUE)
	{
		OS_SemWait( Detect_signal );
		ErrCode = AM_SMC_GetCardStatus(g_u8SCIPort,&tSCIStatus);
		if ( ErrCode == AM_SUCCESS )
		{
			if(tSCIStatus == AM_SMC_CARD_IN)
			{
				eventStatus = CARD_IN_EVENT;
			}	
			else
			{
				eventStatus=CARD_OUT_EVENT;  
			}
		}
		
	//	printf("SMARTCARD_Detect_notify----line %d---%d\n",__LINE__,eventStatus);
		if (eventStatus == Global_SC_STATS)
		{
			if(eventStatus == CARD_OUT_EVENT)
			{
				CTICAS_SendStatusToCAcore(1);
				Global_SC_STATS = CARD_OUT_EVENT;
				if(Cas_Get_Last_Err()!=0)
				{
					cardouttimes++;
					if(cardouttimes>=10)
					{
					//	STB_CAS_Notify(2,4,0);
						cardouttimes=0;
					}
				}
			}
			OS_TaskDelay(500);
			OS_SemSignal( Detect_signal );
			continue;
		}
	//	pbiinfo("SMARTCARD_Detect_notify----line %d  evnet%d\n",__LINE__,eventStatus);

		if(eventStatus == CARD_IN_EVENT)
		{	
			
			Global_SC_STATS = CARD_IN_EVENT;
			atr_recv_count = 40;
			memset(atr_tmp,0,sizeof(atr_tmp));
			printf("SMARTCARD_Detect_notify----line %d\n",__LINE__);
			//OS_TaskDelay(200);
			SC_SetParam();
			ErrCode = AM_SMC_Reset(g_u8SCIPort, atr_tmp, &atr_recv_count);
			if(ErrCode !=AM_SUCCESS)
			{
				int i =0;
				OS_TaskDelay(200);
				OS_SemSignal( Detect_signal );
				CTICAS_SendStatusToCAcore(2);
				printf("errrorocode %x -------line %d\n",ErrCode,__LINE__);
				continue;
			}
			else
			{
				CTICAS_SendStatusToCAcore( 0 );
			}
			printf("CARD_IN_EVENT!!!!                Status=%d   ******\n",tSCIStatus);
		} 
		else if(eventStatus == CARD_OUT_EVENT)
		{
			AM_SMC_Deactive(g_u8SCIPort);
			CTICAS_SendStatusToCAcore(1);	
			Clear_All_Current_Keys();
		}
		Global_SC_STATS=eventStatus;

		OS_TaskDelay(500);
		OS_SemSignal( Detect_signal );

	}
	

}


BYTE CTISC_Init(void)
{
	AM_ErrorCode_t ret = AM_SUCCESS;
	U32 u32Freq=4000;
	AM_SMC_CardStatus_t tSCIStatus;
	AM_SMC_OpenPara_t para;
	U8  atr_tmp[33]={0};
	int 	atr_recv_count = 33;
	
	S32  taskidSCDetectProcess;
	U32  task_id;
	AM_SMC_Param_t SMCpara;
	
	Global_SC_STATS =CARD_OUT_EVENT;
	atr_recv_count =sizeof(atr_tmp);

	memset(atr_tmp,0,atr_recv_count);
	memset(&para, 0, sizeof(para));
	para.enable_thread = 0;
	
	ret = AM_SMC_Open(g_u8SCIPort, &para);
	if (AM_SUCCESS != ret)
	{
		printf(" HI_UNF_SCI_Init Init error:%08x\n",ret);
		return 1;
	}
#if 0	
	ret =AM_SMC_Active(g_u8SCIPort);
	if (AM_SUCCESS != ret)
	{
		printf(" AM_SMC_Active Init error:%08x\n",ret);
		return 1;
	}
#endif	
	if(sc_callback_func ==NULL)
	{
	//	sc_callback_func = SC_ATR_Notify;
	} 
	
	OS_SemCreate(&Detect_signal, "detectcasingal", 0, 0);
	
	taskidSCDetectProcess = OS_TaskCreate(&task_id, "ScDetectProcess", SMARTCARD_Detect_notify, NULL, NULL, 2048, 8, 0);
	if (  taskidSCDetectProcess != 0 )
	{
		printf( "%s %d> smart card driver: unable to spawn ScDetect process\n",
				__FILE__, 
				__LINE__ );
		//return 1;
	}


#if 0
	ret = AM_SMC_GetCardStatus(g_u8SCIPort,&tSCIStatus);
	if ( ret != AM_SUCCESS )
	{
		printf(" AM_SMC_GetCardStatus  ret %x\n", ret);
		OS_SemSignal( Detect_signal );
		return 1;			
	}
	if(tSCIStatus == AM_SMC_CARD_OUT)
	{
		sc_callback_func(0,1,0,0);
		Global_SC_STATS =CARD_OUT_EVENT;
		printf(" AM_SMC_GetCardStatus CARD_OUT_EVENT  error\n");
		//return 1;
	}
	else
	{
		AM_SMC_GetParam(g_u8SCIPort, &SMCpara);
		pbiinfo("[param---old]\n SMCpara.f[%d]\n SMCpara.d[%d]\n SMCpara.n[%d]\n SMCpara.bwi[%d]\n SMCpara.cwi[%d]\n SMCpara.bgt[%d]\n SMCpara.freq[%d]\n SMCpara.recv_invert[%d]\n SMCpara.recv_lsb_msb[%d]\n SMCpara.recv_no_parity[%d]\n SMCpara.xmit_invert[%d]\n SMCpara.xmit_lsb_msb[%d]\n SMCpara.xmit_retries[%d]\n SMCpara.xmit_repeat_dis[%d]\n", 
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
		if(u32Freq)
		{
			SMCpara.freq = u32Freq;
		}
	  AM_SMC_SetParam(g_u8SCIPort, &SMCpara);
	  AM_SMC_GetParam(g_u8SCIPort, &SMCpara);
		  pbiinfo("[param---new]\n SMCpara.f[%d]\n SMCpara.d[%d]\n SMCpara.n[%d]\n SMCpara.bwi[%d]\n SMCpara.cwi[%d]\n SMCpara.bgt[%d]\n SMCpara.freq[%d]\n SMCpara.recv_invert[%d]\n SMCpara.recv_lsb_msb[%d]\n SMCpara.recv_no_parity[%d]\n SMCpara.xmit_invert[%d]\n SMCpara.xmit_lsb_msb[%d]\n SMCpara.xmit_retries[%d]\n SMCpara.xmit_repeat_dis[%d]\n", 
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
		  
		  ret = AM_SMC_Reset(g_u8SCIPort, atr_tmp, &atr_recv_count);
		  if(ret != AM_SUCCESS)
		  {
			  printf(" AM_SMC_Reset  error\n");
			//  return 1;
		  }
		  else
		  {
			  int ii;
			  pbiinfo("atr====");
			  for(ii=0;ii<atr_recv_count;ii++)
			  {
				  pbiinfo(" 0x%x ",atr_tmp[ii]);
			  }
		  	  sc_callback_func(0,0,atr_tmp,0);			  
			  Global_SC_STATS =CARD_IN_EVENT; 
		  
		  }

	}
#endif
	printf(" TRDRV_SCARD_Initialise	ok");
	OS_SemSignal( Detect_signal );
	return 0;

}

BYTE CTISC_Command(BYTE bLength, PBYTE pabMessage, PBYTE pabReplay)
{
	int i =0;
	int replay_length = 0;
	unsigned char command[256];
	unsigned char chLRC = 0;
	int ActwriteLen = 0;
	int len = 0;
	int read_len = 0;
	int size = 0;
	memcpy(command+3,pabMessage,bLength);
	command[ 0 ] = 0;
	command[ 1 ] = 0;
	command[ 2 ] = (unsigned char)(bLength);
	for(i = 0; i < 3+(bLength); i++)
	{
		chLRC^= command[ i ];
	}
	command[ bLength+3 ] = chLRC;
	len = bLength+4;
#if 0
	printf("CTISC_Command: ");
	for(i=0; i<len; i++)
	{
		printf("%02x ", command[i]);
	}
	printf("\n");
#endif

	ActwriteLen = AM_SMC_WriteEx( g_u8SCIPort, command, len, 4000);
	if(ActwriteLen>=AM_SUCCESS)
	{
		OS_TaskDelay(100);
		read_len  = AM_SMC_ReadEx(g_u8SCIPort,pabReplay,3,300);
		if(read_len>=AM_SUCCESS)
		{
			size = pabReplay[2];
			//printf("data = %x,%x,%x,read_len = %d\n",pabReplay[0],pabReplay[1],pabReplay[2],read_len);
			size+=1;
			read_len = AM_SMC_ReadEx(g_u8SCIPort,pabReplay+3,size,1000);
			if(read_len >= AM_SUCCESS)
			{
#ifdef CTI_DEBUG	
			printf("READ: = %d\n ",read_len);
			for(i=0; i<pabReplay[2]+4; i++)
			{
				printf("%02x ", pabReplay[i]);
			}
			printf("\n");
#endif			
				return 0;
			}
		}
		else
		{
			printf("read fail = %x\n",read_len);
		}
	}
	else
	{
		printf("write fail = %x\n",read_len);
		
	}
	
	//TRDRV_SCARD_AdpuFunction(0,pabMessage,bLength,pabReplay,&replay_length);
	return 3;
}

void CTISC_Reset(void)
{
	AM_ErrorCode_t ret = AM_SUCCESS;
	U32 s32ResetTime = 0;
	AM_SMC_CardStatus_t tSCIStatus;
	U8  atr_tmp[40]={0};
	U8 	atr_recv_count = 40;
	int i =0;
	printf("TRDRV_SCARD_ResetCard-----start  line%d\n",__LINE__);
	ret = AM_SMC_GetCardStatus(g_u8SCIPort,&tSCIStatus);
	if ( ret != AM_SUCCESS )
	{
		printf(" AM_SMC_GetCardStatus  ret %x\n", ret);
	}
	if(tSCIStatus == AM_SMC_CARD_OUT)
	{
		//sc_callback_func(0,1,0,0);
		CTICAS_SendStatusToCAcore(1);
		Global_SC_STATS =CARD_OUT_EVENT;
		printf(" AM_SMC_GetCardStatus CARD_OUT_EVENT  error\n");
		return;
	}
	Global_SC_STATS =CARD_IN_EVENT;
	memset(atr_tmp,0,sizeof(atr_tmp));
	SC_SetParam();
	ret = AM_SMC_Reset(g_u8SCIPort, atr_tmp, &atr_recv_count);
	if(ret != AM_SUCCESS)
	{
		printf(" HI_UNF_SCI_ResetCard  error\n");
		CTICAS_SendStatusToCAcore(2);
		return;
	}
	printf("ATR: ");
	for(i=0; i<atr_recv_count; i++)
	{
		printf("%02x ", atr_tmp[i]);
	}
	printf("\n");

	//sc_callback_func(0,0,atr_tmp,0);			
	Global_SC_STATS =CARD_IN_EVENT;
	OS_TaskDelay(500);
	CTICAS_SendStatusToCAcore(0);	
//	CTICAS_SetScProtocolToT1();
	printf("TRDRV_SCARD_ResetCard------ok\n");
}

#ifdef	CTI_TEST
void close_sc()
{
	AM_SMC_Close(g_u8SCIPort);
}
void cit_test()
{
#if 0
	int i =0;
	unsigned char buffer[] = { 0x00, 0x00, 0x05,0x00 ,0x84 ,0x00, 0x00, 0x10, 0x91};
	unsigned char replay[256];
	int len = 0;

	len = 9;
	if(CTISC_Command(len,buffer,replay)==0)
	{
	}
	unsigned char buffer1[] = {0x00,0x00,0x0e,0x00, 0x0c, 0x00, 0x00, 0x10, 0xe4, 0x74, 0x27, 0xee,0xd2, 0x9d,0xb0, 0x25, 0x56, 0xc7};
	len = buffer1[2]+4;
	if(CTISC_Command(len,buffer1,replay)==0)
	{
	}
	unsigned char buffer2[] = {0x00, 0x00, 0x0e,0x00, 0x0b, 0x00, 0x00, 0x10, 0xf1, 0x3d, 0x02, 0x84,0xcf, 0x51, 0x0e,0xe1, 0x56,0x78};
	len = buffer2[2]+4;
	if(CTISC_Command(len,buffer2,replay)==0)
	{
	}
	unsigned char buffer3[] = {	0x00, 0x00, 0x05,0x81, 0xd4, 0x00, 0x01, 0x05, 0x54};
	len = buffer3[2]+4;
	if(CTISC_Command(len,buffer3,replay)==0)
	{
	
	}
	unsigned char buffer4[] = {	0x00, 0x00, 0x05,0x81, 0xd0, 0x00, 0x01, 0x08, 0x5d};
	len = buffer4[2]+4;
	if(CTISC_Command(len,buffer4,replay)==0)
	{
	
	}
	
	
	int i =0;
	unsigned char buffer[] = { 0x00 ,0x84 ,0x00, 0x00, 0x10};
	unsigned char replay[256];
	int len = 0;

	len = 5;
	if(CTISC_Command(len,buffer,replay)==0)
	{
	}
	unsigned char buffer1[] = {0x00, 0x0c, 0x00, 0x00, 0x10, 0xe4, 0x74, 0x27, 0xee,0xd2, 0x9d,0xb0, 0x25, 0x56};
	len = 0x0e;
	if(CTISC_Command(len,buffer1,replay)==0)
	{
	}
	unsigned char buffer2[] = {0x00, 0x0b, 0x00, 0x00, 0x10, 0xf1, 0x3d, 0x02, 0x84,0xcf, 0x51, 0x0e,0xe1, 0x56};
	len = 0x0e;
	if(CTISC_Command(len,buffer2,replay)==0)
	{
	}
	unsigned char buffer3[] = {	0x81, 0xd4, 0x00, 0x01, 0x05};
	len = 5;
	if(CTISC_Command(len,buffer3,replay)==0)
	{
	
	}
	unsigned char buffer4[] = {	0x81, 0xd0, 0x00, 0x01, 0x08};
	len = 5;
	if(CTISC_Command(len,buffer4,replay)==0)
	{
	
	}

    unsigned char buffer[] = { 0x00, 0x26, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x10, 0x42, 0x3a, 0x35, 0xc7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4e, 0x20 };
    unsigned char replay[256];
    int len = 0;
    len = 0x14;
    if(CTISC_Command(len,buffer,replay)==0)
    {
    
    }
    
#else
	unsigned char data[20];
    CTICAclient_GetSTBipAddr(data);
#endif
	
}
void tfca_test()
{
	int buffer[] = {0x00 ,0xa4, 0x04, 0x00, 0x05,0xf9, 0x5a ,0x54 ,0x00 ,0x06};
	unsigned char replay[256];
	int len = 5;
	int i = 0;
	int relen = 0;
	int error = 0;
	error = AM_SMC_TransferT0(g_u8SCIPort,buffer,10,replay,&relen);
	if(error==AM_SUCCESS)
	{
		printf("ATR: ");
		for(i=0; i<relen; i++)
		{
			printf("%02x ", replay[i]);
		}
		printf("\n");
		
	}
	else
	{
		printf("write fail = %x\n",error);

	}
	
#if 0	
	if(CTISC_Command(len,buffer,replay)==0)
	{
		printf("ATR: ");
		for(i=0; i<replay[2]; i++)
		{
			printf("%02x ", replay[i]);
		}
		printf("\n");
	}
#endif	
	
}
#endif

