/***************************************************************************
 *  Copyright C 2009 by Amlogic, Inc. All Rights Reserved.
 */
/**\file
 * \brief 智能卡测试程�? *
 * \author Gong Ke <ke.gong@amlogic.com>
 * \date 2010-07-05: create the document
 ***************************************************************************/

#define AM_DEBUG_LEVEL 0

#include <am_debug.h>
#include <am_smc.h>
#include <string.h>
#include <unistd.h>

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

#if 1
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

#endif
	
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
	#if 0
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
#endif
		printf("\n");
	AM_TRY(AM_SMC_Close(SMC_DEV_NO));


	
	return 0;
}

int main(int argc, char **argv)
{
	int freq = 0;
	if(argc > 1)
	freq = atoi(argv[1]);
	//printf("sync mode test[%d]\n", freq);
	//smc_test(AM_TRUE, freq);
	
	printf("async mode test\n");
	smc_test(AM_FALSE,freq);
	return 0;
}

