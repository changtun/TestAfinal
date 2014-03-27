/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : PSI/SI
* File name   : nit.c
* Description : Include implementation of NIT parse function.
*
* History :
*   2012-05-02 ZYJ
*       Initial Version.
*******************************************************************************/
#include <unistd.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include "pvddefs.h"
#include <fcntl.h>
#include <errno.h>


#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/param.h> 
#include <sys/ioctl.h> 
#include <linux/sockios.h>
#include <net/route.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h> 
#include <net/if_arp.h> 

#include "pbitrace.h"
#include "dvbcore.h"
#include "psi_si.h"
#include "table.h"
#include "descriptor.h"
#include "dvb_download.h"
#include "system_info.h"
#ifdef __cplusplus
    extern "C" {
#endif

#define NIT_MIN_SECTION_LEN (16)

#define NIT_HEAD_LEN		(10)

typedef struct
{
	U8 table_id						:8;
#if TBL_BYTE_ORDER == TBL_BIG_ENDIAN
	U8 section_syntax_indicator		:1;
	U8 								:3;
	U8 sec_len_hi					:4;
#else
	U8 sec_len_hi					:4;
	U8								:3;
	U8 section_syntax_indicator 	:1;
#endif
	U8 sec_len_lo					:8;
	U8 network_id_hi				:8;
	U8 network_id_lo				:8;
#if TBL_BYTE_ORDER == TBL_BIG_ENDIAN
	U8 								:2;
	U8 version_number				:5;
	U8 current_next_indicator		:1;
#else
	U8 current_next_indicator		:1;
	U8 version_number				:5;
	U8								:2;
#endif
	U8 sec_num						:8;
	U8 last_sec_num					:8;	
#if TBL_BYTE_ORDER == TBL_BIG_ENDIAN
	U8 								:4;
	U8 network_desc_len_hi			:4;
#else
	U8 network_desc_len_hi			:4;
	U8								:4;
#endif
	U8 network_desc_len_lo			:8;
} _NIT_Sect_Template_t;


#define NIT_ITEM_HEAD_LEN			(6)

typedef struct
{
	U8 ts_id_hi			:8;
	U8 ts_id_lo			:8;
	U8 on_id_hi			:8;
	U8 on_id_lo			:8;
#if TBL_BYTE_ORDER == TBL_BIG_ENDIAN
	U8 					:4;
	U8 ts_desc_len_hi	:4;
#else
	U8 ts_desc_len_hi	:4;
	U8					:4;
#endif
	U8 ts_desc_len_lo	:8;
} _NIT_Item_Template_t;

typedef struct
{
    U8 u8LinkDesNum;
    U8 u8NitTpStreamNum;
    //U8 u8DeliveryDesNum;
    U8 u8DvbCDesNum;
    U8 u8DvbSDesNum;
    U8 u8DvbTDesNum;
    U8 u8OtherNum;
    U32 u32OtherFlashNum;
    U8 *pLinkDes;
    U8 *pNitTpStream;
    NIT_Section_t *pNIT_Head;
    PSISI_Descriptor_t *    pOtherHead;
    //U8 *pDeliveryDes;
    Delivery_System_Desc_Cab_t *pDvbC;
    Delivery_System_Desc_Sat_t *pDvbS;
    Delivery_System_Desc_Ter_t *pDvbT;
    U8 *pOther;
}_NIT_Des_Temp_t;

Loader_msg_t		dl_msg;

/*****************************************************************************
 �� �� ��  : _BuildNitFlashMem
 ��������  : ���� NIT Flash �ṹ
 �������? : 
              NIT_Section_t *pNit_Section  
             _NIT_Des_Temp_t *NitDesTemp  
 �������? : ��
 �� �� ֵ  : static
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2012��5��4��
    ��    ��   : zxguan
    �޸�����   : ����ɺ���?

*****************************************************************************/
static S32 _BuildNitFlashMem( _NIT_Des_Temp_t *NitDesTemp )
{
    U32 FlashMemSize = 0;
    U8 *pFlashMem = NULL;
    
    if( NULL == NitDesTemp )
    {
        return PV_NULL_PTR;
    }

    FlashMemSize = sizeof(NIT_Section_t) \
        + NitDesTemp->u8LinkDesNum * sizeof(Linkage_Desc_t) \
        + NitDesTemp->u8NitTpStreamNum * sizeof(NIT_TpStream_t) \
        + NitDesTemp->u8OtherNum * sizeof(PSISI_Descriptor_t) \
        + NitDesTemp->u8DvbCDesNum * sizeof(Delivery_System_Desc_Cab_t) \
        + NitDesTemp->u8DvbSDesNum * sizeof(Delivery_System_Desc_Sat_t) \
        + NitDesTemp->u8DvbTDesNum * sizeof(Delivery_System_Desc_Ter_t) \
        + NitDesTemp->u32OtherFlashNum;

    FlashMemSize = (FlashMemSize + 3) & 0xFFFFFFFC;

    pFlashMem = (U8 *)psisi_section_malloc(FlashMemSize);
    if( NULL == pFlashMem )
    {
        return PV_NO_MEMORY;
    }
#if 0
    printf("Link num %d...\n",NitDesTemp->u8LinkDesNum);
    printf("u8NitTpStreamNum %d...\n",NitDesTemp->u8NitTpStreamNum);
    printf("u8OtherNum %d...\n",NitDesTemp->u8OtherNum);
    printf("u8DvbCDesNum %d...\n",NitDesTemp->u8DvbCDesNum);
    printf("u8DvbSDesNum %d...\n",NitDesTemp->u8DvbSDesNum);
    printf("u8DvbTDesNum %d...\n",NitDesTemp->u8DvbTDesNum);
    printf("u32OtherFlashNum %d...\n",NitDesTemp->u32OtherFlashNum);
    printf("FlashMemSize %d...\n",FlashMemSize);
#endif    
    memset( pFlashMem, 0, FlashMemSize );

    NitDesTemp->pNIT_Head = (NIT_Section_t *)pFlashMem;
    pFlashMem += sizeof(NIT_Section_t);
    /* Link */
    if( 0 != NitDesTemp->u8LinkDesNum )
    {
        NitDesTemp->pLinkDes = pFlashMem;
        pFlashMem += ( NitDesTemp->u8LinkDesNum * sizeof(Linkage_Desc_t) );
    }
    else
    {
        NitDesTemp->pLinkDes = NULL;
    }
    /* NIT TP Stream */
    if( 0 != NitDesTemp->u8NitTpStreamNum )
    {
        NitDesTemp->pNitTpStream = pFlashMem;
        pFlashMem += ( NitDesTemp->u8NitTpStreamNum * sizeof(NIT_TpStream_t) );
    }
    else
    {
        NitDesTemp->pNitTpStream = NULL;
    }
    /* NIT Other Head */
    if( 0 != NitDesTemp->u8OtherNum )
    {
        NitDesTemp->pOtherHead= (PSISI_Descriptor_t *)pFlashMem;
        pFlashMem += ( NitDesTemp->u8OtherNum * sizeof(PSISI_Descriptor_t) );
    }
    else
    {
        NitDesTemp->pOtherHead = NULL;
    }
    /* DVB C */
    if( 0 != NitDesTemp->u8DvbCDesNum )
    {
        NitDesTemp->pDvbC = (Delivery_System_Desc_Cab_t *)pFlashMem;
        pFlashMem += ( NitDesTemp->u8DvbCDesNum * sizeof(Delivery_System_Desc_Cab_t) );
    }
    else
    {
        NitDesTemp->pDvbC = NULL;
    }
    /* DVB S */
    if( 0 != NitDesTemp->u8DvbSDesNum )
    {
        NitDesTemp->pDvbS = (Delivery_System_Desc_Sat_t *)pFlashMem;
        pFlashMem += ( NitDesTemp->u8DvbSDesNum * sizeof(Delivery_System_Desc_Sat_t) );
    }
    else
    {
        NitDesTemp->pDvbS = NULL;
    }
    /* DVB T */
    if( 0 != NitDesTemp->u8DvbTDesNum )
    {
        NitDesTemp->pDvbT = (Delivery_System_Desc_Ter_t *)pFlashMem;
        pFlashMem += ( NitDesTemp->u8DvbTDesNum * sizeof(Delivery_System_Desc_Ter_t) );
    }
    else
    {
        NitDesTemp->pDvbT = NULL;
    }
    /* Other Mem */
    if( 0 != NitDesTemp->u32OtherFlashNum )
    {
        NitDesTemp->pOther = pFlashMem;
    }
    else
    {
        NitDesTemp->pOther = NULL;
    }

    return PV_SUCCESS;
}

/*****************************************************************************
 �� �� ��  : PSISI_Parse_NIT_Section
 ��������  : ���� NIT Section
 �������? : PSISI_Table_t * ptTable  
             U8 * ptData              
             U16 u16Len               
 �������? : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2012��5��4��
    ��    ��   : zxguan
    �޸�����   : ����ɺ���?

*****************************************************************************/
S32 PSISI_Parse_NIT_Section(PSISI_Table_t * ptTable, U8 * ptData, U16 u16Len)
{
    U8 section_num = 0;
    U8 section_last_num = 0;
    U8 des_tag = 0;
    U8 des_len = 0;
    U8 *pStr = NULL;
    U8  DesFlag = 1;
    S16 NetDesLen = 0;
    S16 TsStreamLoopLen = 0;
    S16 TrDesLen = 0;
    S16 section_len = 0;
    U16 DesCount = 0;
    U16 DesCount2 = 0;
    S32 RetErr = 0;
    U32 section_crc = 0;
    NIT_Section_t *pNit_Section = NULL;
    _NIT_Sect_Template_t	*nit_tmplt = NULL;
    _NIT_Des_Temp_t    NitDesTemp;

    if ((NULL == ptTable) || (NULL == ptData))
    {
        return PV_NULL_PTR;
    }
 
    if ( ((ptData[0] != NIT_TABLE_ID) && (ptData[0] != NIT_OTHER_TABLE_ID))
        || (u16Len < NIT_MIN_SECTION_LEN))
    {
        return PV_INVALID_PARAM;
    }
    #if 0
    int kk = 0;
    int jj = 0;
    for(kk = 0; kk< u16Len;kk++)
    {
        printf("%02x ",ptData[kk]);
        if(0 == kk%16 && kk !=0)
            printf("\n");
    }
    printf("\n");
    #endif
    nit_tmplt = (_NIT_Sect_Template_t *)ptData;

    /* �� table id �� section length �ֶα������ڵĳ��� */
    section_len = (S16)((nit_tmplt->sec_len_hi << 8) + nit_tmplt->sec_len_lo + 3);
  //  nit_sect->network_id = (nit_tmplt->network_id_hi << 8) + nit_tmplt->network_id_lo;
    section_num = nit_tmplt->sec_num;
    section_last_num = nit_tmplt->last_sec_num;

    section_crc = ( (ptData[section_len - 4] << 24) \
        | (ptData[section_len - 3] << 16) \
        | (ptData[section_len - 2] << 8) \
        | (ptData[section_len - 1]) ); 

    
    if( NULL != ptTable->ppSection[section_num] )
    {   
        pNit_Section = (NIT_Section_t *)ptTable->ppSection[section_num];
        if ((pNit_Section->u8Version != nit_tmplt->version_number)
            || (pNit_Section->u32CRC != section_crc))
        {
            return PSISI_ERR_TABLE_UPDATE;
        }
        else
        {
            return PSISI_ERR_TABLE_REPEAT;
        }
    }

    memset( &NitDesTemp, 0, sizeof(_NIT_Des_Temp_t) );
    
AGAIN_PARSE:

    pStr = ptData + NIT_HEAD_LEN;
	NetDesLen = (S16)( (nit_tmplt->network_desc_len_hi << 8)
							+ nit_tmplt->network_desc_len_lo );
    section_len = (S16)((nit_tmplt->sec_len_hi << 8) + nit_tmplt->sec_len_lo + 3);
    section_len = section_len - NIT_HEAD_LEN - 4;	/* ��ȥ��ͷ�� CRC ʣ��Ĳ���?*/
    section_len -= NetDesLen;
    while( NetDesLen > 0 )
    {
        des_tag = pStr[0];
        des_len = pStr[1];
        //pStr += 2;
        switch( des_tag )
        {
            case LINKAGE_DESCRIPTOR_TAG:
            {
                if( 1 == DesFlag )
                {
                    RetErr = parse_linkage_descriptor( pStr, NULL, &NitDesTemp.u32OtherFlashNum, DesFlag);
                    if( PV_SUCCESS != RetErr )
                    {
                        return RetErr;
                    }
                    NitDesTemp.u8LinkDesNum++;
                }
                else
                {   
                    if( NULL == pNit_Section->ptLinkageDesc )
                    {
                        pNit_Section->ptLinkageDesc = (Linkage_Desc_t *)NitDesTemp.pLinkDes;
                    }
                    DesCount = pNit_Section->u16LinkageDescNum;
                    RetErr = parse_linkage_descriptor( pStr, &pNit_Section->ptLinkageDesc[DesCount], \
                        NitDesTemp.pLinkDes, DesFlag);
                    if( PV_SUCCESS != RetErr )
                    {
                        psisi_section_free( pNit_Section );
                        return RetErr;
                    }
                    NitDesTemp.pLinkDes += pNit_Section->ptLinkageDesc[DesCount].u8PrivateDataLen;
                    pNit_Section->u16LinkageDescNum++;
                }
            }break;
            default:
            {
                if( 1 == DesFlag )
                {                    
                    RetErr = parse_unknown_descriptor( pStr, NULL, &NitDesTemp.u32OtherFlashNum, DesFlag);
                    if( PV_SUCCESS != RetErr )
                    {
                        return RetErr;
                    }
                    NitDesTemp.u8OtherNum++;
                }
                else
                {
                    if( NULL == pNit_Section->ptDesc)
                    {
                        pNit_Section->ptDesc = NitDesTemp.pOtherHead;
                    }
                    DesCount = pNit_Section->u16OtherDescNum;
                    RetErr = parse_unknown_descriptor( pStr, \
                        &pNit_Section->ptDesc[DesCount], NitDesTemp.pOther, DesFlag);
                    if( PV_SUCCESS != RetErr )
                    {
                        psisi_section_free( pNit_Section );
                        return RetErr;
                    }
                    pNit_Section->u16OtherDescNum++;
                    NitDesTemp.pOther += pNit_Section->ptDesc[DesCount].u8Len;
                }
            }break;
        }
        NetDesLen -= (des_len + 2);
        pStr += (des_len + 2);
    }


    if( 0 == DesFlag )
    {
        NitDesTemp.pOtherHead += pNit_Section->u16OtherDescNum;
    }
    if( 0xF != ((pStr[0]&0xF0)>>4))
    {
        return PV_FAILURE;
    }
    TsStreamLoopLen = (S16)( ((pStr[0] & 0xF) << 8) + pStr[1] );
    pStr += 2;
    while( TsStreamLoopLen > 0 )
    {
        if( 1 == DesFlag )
        {
            NitDesTemp.u8NitTpStreamNum++;
        }
        else
        {
            if( NULL == pNit_Section->ptTpStream )
            {
                pNit_Section->ptTpStream = (NIT_TpStream_t *)NitDesTemp.pNitTpStream;
            }
            DesCount = pNit_Section->u16TpStreamNum;
            pNit_Section->ptTpStream[DesCount].u16TS_Id = ( (pStr[0] << 8) | pStr[1] );
            pNit_Section->ptTpStream[DesCount].u16ON_Id = ( (pStr[2] << 8) | pStr[3] );
            pNit_Section->u16TpStreamNum++;            
        }
        if( 0xF != ((pStr[4]&0xF0)>>4))
        {
            return PV_FAILURE;
        }
        TrDesLen = ( ((pStr[4] & 0xF) << 8) | pStr[5] );
        TsStreamLoopLen -= (TrDesLen + 6);
        pStr += 6;
        while( TrDesLen > 0 )
        {
            des_tag = pStr[0];
            des_len = pStr[1];
            //pStr += 2;
            switch( des_tag )
            {
                case CABLE_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
                {
                    if( 1 == DesFlag )
                    {
                        NitDesTemp.u8DvbCDesNum++;
                    }
                    else
                    {   
                        pNit_Section->ptTpStream[DesCount].u8DeliveryDescTag = des_tag;
                        pNit_Section->ptTpStream[DesCount].pDeliveryDesc = (U8 *)NitDesTemp.pDvbC;
                        RetErr = parse_delivery_system_cable_descriptor( pStr, NitDesTemp.pDvbC );
                        if( PV_SUCCESS != RetErr )
                        {
                            psisi_section_free( pNit_Section );
                            return RetErr;
                        }
                        NitDesTemp.pDvbC++;
                    }
                }break;
                case SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
                {
                    if( 1 == DesFlag )
                    {
                        NitDesTemp.u8DvbSDesNum++;
                    }
                    else
                    {
                        pNit_Section->ptTpStream[DesCount].u8DeliveryDescTag = des_tag;
                        pNit_Section->ptTpStream[DesCount].pDeliveryDesc = (U8 *)NitDesTemp.pDvbS;
                        RetErr = parse_delivery_system_satellite_descriptor( pStr, NitDesTemp.pDvbS );
                        if( PV_SUCCESS != RetErr )
                        {
                            psisi_section_free( pNit_Section );
                            return RetErr;
                        }
                        NitDesTemp.pDvbS++;
                    }
                }break;
                case TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR_TAG:
                {
                    if( 1 == DesFlag )
                    {
                        NitDesTemp.u8DvbTDesNum++;
                    }
                    else
                    {
                        pNit_Section->ptTpStream[DesCount].u8DeliveryDescTag = des_tag;
                        pNit_Section->ptTpStream[DesCount].pDeliveryDesc = (U8 *)NitDesTemp.pDvbT;
                        RetErr = parse_delivery_system_terrestrial_descriptor( pStr, NitDesTemp.pDvbT );
                        if( PV_SUCCESS != RetErr )
                        {
                            psisi_section_free( pNit_Section );
                            return RetErr;
                        }
                        NitDesTemp.pDvbT++;
                    }
                }break;
                case 0x79:
                
                break;
                case 0x41:
                
                break;
                default:
                {
                    if( 1 == DesFlag )
                    {
                        RetErr = parse_unknown_descriptor( pStr, NULL, &NitDesTemp.u32OtherFlashNum, DesFlag);
                        if( PV_SUCCESS != RetErr )
                        {
                            return RetErr;
                        }
                        NitDesTemp.u8OtherNum++;
                    }
                    else
                    {

                        if( NULL == pNit_Section->ptTpStream[DesCount].ptDesc )
                        {
                            pNit_Section->ptTpStream[DesCount].ptDesc = NitDesTemp.pOtherHead;
                        }
                        DesCount2 = pNit_Section->ptTpStream[DesCount].u16OtherDescNum;
                        RetErr = parse_unknown_descriptor( pStr, \
                            &pNit_Section->ptTpStream[DesCount].ptDesc[DesCount2], \
                            NitDesTemp.pOther, DesFlag);
                        if( PV_SUCCESS != RetErr )
                        {
                            psisi_section_free( pNit_Section );
                            return RetErr;
                        }
                        pNit_Section->ptTpStream[DesCount].u16OtherDescNum++;
                        NitDesTemp.pOther += pNit_Section->ptTpStream[DesCount].ptDesc[DesCount2].u8Len;
                    }
                }break;
            }
            TrDesLen -= (des_len + 2);
            pStr += (des_len + 2);
        }
        
    }
    if( 1 == DesFlag )
    {
        RetErr = _BuildNitFlashMem( &NitDesTemp );
        if( PV_SUCCESS != RetErr )
        {
            return RetErr;
        }
        
        ptTable->ppSection[section_num] = (void *)NitDesTemp.pNIT_Head;
        pNit_Section = ptTable->ppSection[section_num];
        
        DesFlag = 0;
        goto AGAIN_PARSE;
    }
    else
    {
        pNit_Section->u8TableId = NIT_TABLE_ID;
        pNit_Section->u8Version = nit_tmplt->version_number;
		pNit_Section->u16NetworkId = (nit_tmplt->network_id_hi << 8) + nit_tmplt->network_id_lo;
        pNit_Section->u32CRC = section_crc;
        ptTable->u16RecievedNum++;
    }

    printf("info --SUCCESS---%d--------%s--------.\n",__LINE__,__FUNCTION__);
    return PV_SUCCESS;
}



#if 1
/*****************************************************************************
 �� �� ��  : Check_MAC
 ��������  : �ж�MAC��ַ
 �������? : U64 mac_start, U64 mac_end               
 �������? : ��
 �� �� ֵ  :  	'0': OK
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��5��21��
    ��    ��   : dwzhang
    �޸�����   : ����ɺ���?

*****************************************************************************/
U8 Check_MAC( U64 mac_start, U64 mac_end)
{
	U8 i = 0;
	int value = 0;
	U64 result = 0ULL;
	char ar[17] = "0123456789abcdef";
	//U64 mac_addr = 0ULL;
	U8 mac_addr[30]={0};
	
	GetLocalMac( mac_addr );
	//sscanf("",x,&lngHex);
	//strlwr(mac_addr);  //�������?
	while( i < 12 )
	{
		if((mac_addr[i] <= 90) && (mac_addr[i] >= 65))
		{
			mac_addr[i] = mac_addr[i] + 32;//��дתСд
		}
		i++;
	}
	for( i = 0; i < 12; i++)
	{
	    value = strchr(ar, mac_addr[i]) - ar;
	   //pbiinfo("%x ",value);
	    result = result*16 + value;
	}
	pbiinfo("ZDW===>result: %llx\n",result);
	//pbiinfo("ZDW===>mac_addr:\n");
	//for(i=0; i<12; i++)
	//pbiinfo("%c ",mac_addr[i]);
	//pbiinfo("\n");
	if((result >= mac_start) && (result <= mac_end))
		return 0;
	else
		return 1;
}

/*****************************************************************************
 �� �� ��  : Check_SFVersion
 ��������  : �ж�����汾��?
 �������? : U32 sfver               
 �������? : ��
 �� �� ֵ  :  	'0': OK
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��5��22��
    ��    ��   : dwzhang
    �޸�����   : ����ɺ���?

*****************************************************************************/
U8 Check_SFVersion(U32 sfver)
{
	U8 sfver_local[100] = {0}, i = 0, flag = 0;
	U32 result = 0;
	
	getSoftWareID(sfver_local);
	pbiinfo("ZDW===>sfver_local:%s\n",sfver_local);
	for(i=0; (i<100)&&(sfver_local[i]!='\0'); i++)
	{
		if(sfver_local[i] == 78)//'N'
		{
			U8 j = 0;
			pbiinfo("ZDW===>find N: %d\n",i);
			i++;
			while(i<100)
			{
				//pbiinfo("ZDW===>sfver_local[%d]: %d\n",i,sfver_local[i]);
				if(sfver_local[i]>=0x30 && sfver_local[i]<=0x39)
				{
					j++;
					//pbiinfo("ZDW===>sfver_local[%d]: %d\n",i,sfver_local[i]);
					result = result*10 + (sfver_local[i]-0x30);
					if( 6 == j )
					{
						pbiinfo("ZDW===>result: %d\n",result);
						flag = 1;
						break;
					}
				}
				i++;
			}
			break;
		}
	}
	if( 1 == flag )
	{
		if( sfver > result )
		{
			pbiinfo("ZDW===>Check_SFVersion ok!!!\n");
			return 0;
		}
		else
		{
			pbiinfo("ZDW===>Check_SFVersion error!!!\n");
			return 1;
		}
	}
	else
	{
		pbiinfo("ZDW===>not find swver!!!\n");
		return 2;
	}
	
}

/*****************************************************************************
 �� �� ��  : Loader_Parse_NIT
 ��������  : ����NIT������
 �������? : U8 *buffer               
 �������? : ��
 �� �� ֵ  :  	'1': ��⵽�����?
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��5��11��
    ��    ��   : dwzhang
    �޸�����   : ����ɺ���?

*****************************************************************************/
S32 Loader_Parse_NIT(U8 *buffer)
{
	int				i, j;
	unsigned short 	section_length_tmp;
	//unsigned short 	version_number;
	//unsigned short 	section_number;
	//unsigned short 	last_section_number;
	unsigned short 	network_descriptors_length;
	unsigned short 	transport_stream_loop_length;
	
	section_length_tmp = ((buffer[1]&0x0f)<<8) + buffer[2];

	if(NULL == buffer)
	{
		return PV_NULL_PTR;
	}

	if(((buffer[0] != NIT_TABLE_ID) && (buffer[0] != NIT_OTHER_TABLE_ID))
		|| (section_length_tmp < NIT_MIN_SECTION_LEN))
	{
		return PV_INVALID_PARAM;
	}
	//version_number=(buffer[5]>>1)&0x1f;
	//section_number=buffer[6];
	//last_section_number=buffer[7];
	network_descriptors_length = ((buffer[8]&0x0f)<<8) + buffer[9];
	/*network_descriptors*/
	//pbiinfo("ZDW===>Loader_Parse_NIT\n");
	for(i=0;i<network_descriptors_length;)
	{
		unsigned short 	descriptor_length = buffer[i+11];
		switch (buffer[i+10])
		{			
			case DVB_DOWNLOAD_DESCRIPTOR_TAG:
			{
				U8	product_id = 0, 
					user_id = 0, 
					dl_type = 0, 
					temp_mod = 0,
					band_width = 0,
					trans_type = 0;
				U32 SoftwareVer = 0, x = 0, y = 0;
				U64 start_mac = 0ULL, end_mac = 0ULL;
 				U16 temp_freq = 0, temp_SR = 0, dl_pid = 0;

				product_id = buffer[i+12];
				pbiinfo("ZDW===>product_id: %d\n", product_id);
				/*****************��ֲʱ���޸�:**********************/
				if( product_id != ePRODUCT_F865 )
				{
					break;
				}
				
				user_id = buffer[i+13];
				pbiinfo("ZDW===>user_id: %d\n", user_id);
				if( user_id != eUSER_SX )
				{
					break;
				}
				/*****************��ֲʱ���޸�end**********************/
				SoftwareVer = (buffer[i+14]>>4)*100000 + (buffer[i+14]&0xf)*10000 + (buffer[i+15]>>4)*1000
								+ (buffer[i+15]&0xf)*100 + (buffer[i+16]>>4)*10 + (buffer[i+16]&0xf);
				pbiinfo("ZDW===>SoftwareVer: %d\n", SoftwareVer);
				
				if( Check_SFVersion(SoftwareVer) != 0 )
				{
					break;
				}
				x = (buffer[i+17]<<8)|(buffer[i+18]);
				y = (buffer[i+19]<<24)|(buffer[i+20]<<16)|(buffer[i+21]<<8)|(buffer[i+22]);
				start_mac = x*0x100000000 + y;//(buffer[i+17]*0x10000000000)+(buffer[i+18]*0x100000000)+(buffer[i+19]*0x1000000)+(buffer[i+20]*0x10000)+(buffer[i+21]*0x100)+(buffer[i+22]);//(buffer[i+17]<<40)|(buffer[i+18]<<32)|(buffer[i+19]<<24)|(buffer[i+20]<<16)|(buffer[i+21]<<8)|(buffer[i+22]);
				x = (buffer[i+23]<<8)|(buffer[i+24]);
				y = (buffer[i+25]<<24)|(buffer[i+26]<<16)|(buffer[i+27]<<8)|(buffer[i+28]);
				end_mac = x*0x100000000 + y;//(buffer[i+23]*0x10000000000)+(buffer[i+24]*0x100000000)+(buffer[i+25]*0x1000000)+(buffer[i+26]*0x10000)+(buffer[i+27]*0x100)+(buffer[i+28]);//(buffer[i+23]<<40)|(buffer[i+24]<<32)|(buffer[i+25]<<24)|(buffer[i+26]<<16)|(buffer[i+27]<<8)|(buffer[i+28]);
				//pbiinfo("ZDW===>start_macAAA: %x%x%x%x%x%x\n",buffer[i+17],buffer[i+18],buffer[i+19],buffer[i+20],buffer[i+21],buffer[i+22]);
				//pbiinfo("ZDW===>end_macBBB: %x%x%x%x%x%x\n",buffer[i+23],buffer[i+24],buffer[i+25],buffer[i+26],buffer[i+27],buffer[i+28]);
				pbiinfo("ZDW===>start_mac: %llx\n",start_mac);
				pbiinfo("ZDW===>end_mac: %llx\n",end_mac);
				if(Check_MAC( start_mac, end_mac) != 0)
				{
					pbiinfo("ZDW===>Check_MAC ERROR!\n");
					break;
				}
				dl_type = buffer[i+29];
				temp_freq = (buffer[i+30]&0x0f)*100 + (buffer[i+31]>>4)*10 + (buffer[i+31]&0x0f);
				temp_SR= (buffer[i+32]>>4)*1000 + (buffer[i+32]&0x0f)*100 + (buffer[i+33]>>4)*10 + (buffer[i+33]&0x0f);
				temp_mod = buffer[i+34];
				band_width = buffer[i+35];
				trans_type = buffer[i+36];
				dl_pid = ((buffer[i+37])<<8)|(buffer[i+38]);
				pbiinfo("ZDW===>dl_type: %d\n",dl_type);
				pbiinfo("ZDW===>temp_freq: %d\n",temp_freq);
				pbiinfo("ZDW===>temp_SR: %d\n",temp_SR);
				pbiinfo("ZDW===>temp_mod: %d\n",temp_mod);
				pbiinfo("ZDW===>band_width: %d\n",band_width);
				pbiinfo("ZDW===>trans_type: %d\n",trans_type);
				pbiinfo("ZDW===>dl_pid: 0x%x\n",dl_pid);
				if( temp_freq == 0 ||temp_SR == 0 )
				{
					pbiinfo("error frequence\n");
					break;
				}
				else
				{
					dl_msg.Frequnce = temp_freq * 1000;
					dl_msg.SymbolRate = temp_SR;
					dl_msg.QamSize = temp_mod + 4;
					dl_msg.Pid = dl_pid;
					dl_msg.Download_Type = dl_type;
					dl_msg.Bandwidth = band_width;
					dl_msg.Trans_Type = trans_type;
					// Dvb_DownLoad_Start();
					return 1;
				}
			}
			break;
			
			default:
			break;
			
		}
		i+= descriptor_length+2;
	}

	return 0;
}
#endif

#if 0 /* uesd test show data */
S32 TestNitShow(PSISI_Table_t * ptTable)
{
    NIT_Section_t *pNit_Section = NULL;
    int ii = 0;
    
    if( NULL == ptTable->pNextTable )
    {
        printf("Test-err------%d----%s-----.\n",__LINE__,__FUNCTION__);
        return PV_NULL_PTR;
    }

    printf("table id : 0x%x.\n", ptTable->u8TableId);
    printf("table pid : 0x%x.\n", ptTable->u16TablePid);
    printf("section num : 0x%x.\n", ptTable->u16SectionNum);
    printf("Recieced num : 0x%x.\n", ptTable->u16RecievedNum);

    for( ii = 0; ii < ptTable->u16SectionNum; ii++ )
    {
        pNit_Section = (NIT_Section_t *)ptTable->ppSection[ii];
        if( NULL == pNit_Section )
        {
            printf("Test-err------%d----%s-----.\n",__LINE__,__FUNCTION__);
            return PV_NULL_PTR;
        }

        printf("TableId[%d] 0x%x.\n", ii, pNit_Section->u8TableId);
        printf("Version[%d] 0x%x.\n", ii, pNit_Section->u8Version);
        printf("CRC[%d] 0x%x.\n", ii, pNit_Section->u32CRC);
        printf("NetworkId[%d] 0x%x.\n", ii, pNit_Section->u16NetworkId);
        printf("LinkageDescNum[%d] 0x%x.\n", ii, pNit_Section->u16LinkageDescNum);
        printf("TpStreamNum[%d] 0x%x.\n", ii, pNit_Section->u16TpStreamNum);
        printf("OtherDescNum[%d] 0x%x.\n\n", ii, pNit_Section->u16OtherDescNum);

    }
    printf("\n");
    
    return PV_SUCCESS;
}
#endif

#ifdef __cplusplus
}
#endif
