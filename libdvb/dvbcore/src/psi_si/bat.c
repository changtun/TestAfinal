/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : PSI/SI
* File name   : bat.c
* Description : Include implementation of BAT parse function.
*
* History :
*   2012-05-02 ZYJ
*       Initial Version.
*
*   2012-11-05 zxguan
*       Add
*******************************************************************************/
#include <stdio.h>
#include "pvddefs.h"

#include "dvbcore.h"
#include "psi_si.h"
#include "table.h"
#include "descriptor.h"

#include "pbitrace.h"

#ifdef __cplusplus
    extern "C" {
#endif

#define BAT_MIN_SECTION_LEN (16)

#define BAT_HEAD_LEN		(10)

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
	U8 bouquet_id_hi				:8;
	U8 bouquet_id_lo				:8;
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
	U8 bouquet_desc_len_hi			:4;
#else
	U8 bouquet_desc_len_hi			:4;
	U8								:4;
#endif
	U8 bouquet_desc_len_lo			:8;
} _BAT_Sect_Template_t;


#define BAT_ITEM_HEAD_LEN			(6)

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
} _BAT_Item_Template_t;

#if 1
typedef struct
{  
    U32 u32BouquetNameDescCount;
    Bouquet_Name_t *pBouquetNameDesc;

    U32 u32BouquetNameLen;
    C8 *pName;

    U32 u32BatComCount;
    BAT_Component_t *pBatCom;

     U16 u16ServListDesNum;
    Service_List_Des_t *pServList;
    

    U16 u16LwListNum;
    LW_Logical_List_t *pLwList;


    U16 u16ServInfoCount;
    Service_Info_t *pServInfo;

    U16 u16ListNum;
    LW_Logical_Num_t *pLogicalInfo;


    U32 u32PsiSiDescCount;
    PSISI_Descriptor_t *pPsiSiDesc;
    
    U32 u32PrivateDataLen;
    U8 *pPrivateData;

    BAT_Section_t *pBatHead;
}_BAT_Des_Temp_t;
#endif



static S32 _BuildBatFlashMem( _BAT_Des_Temp_t *pBatDesTemp )
{
    U32 u32FlashMemSize = 0;
    U8 *pFlashMem = NULL;
    
    if( NULL == pBatDesTemp )
    {
        return PV_NULL_PTR;
    }

    u32FlashMemSize = sizeof(BAT_Section_t) \
        + pBatDesTemp->u32BouquetNameDescCount * sizeof(Bouquet_Name_t)  \
        + pBatDesTemp->u32BouquetNameLen \
        + pBatDesTemp->u32BatComCount * sizeof(BAT_Component_t)  \
        + pBatDesTemp->u16ServListDesNum * sizeof( Service_List_Des_t)  \
        + pBatDesTemp->u16LwListNum * sizeof( LW_Logical_List_t )  \
        + pBatDesTemp->u16ServInfoCount *sizeof( Service_Info_t ) \
        + pBatDesTemp->u16ListNum *sizeof( LW_Logical_Num_t ) \
        + pBatDesTemp->u32PsiSiDescCount *sizeof( PSISI_Descriptor_t ) \
        + pBatDesTemp->u32PrivateDataLen; 
    

    u32FlashMemSize = (u32FlashMemSize + 3) & 0xFFFFFFFC;

    pbiinfo("pBatDesTemp->u32BouquetNameDescCount %d\n",pBatDesTemp->u32BouquetNameDescCount);
    pbiinfo("pBatDesTemp->u32BouquetNameLen %d",pBatDesTemp->u32BouquetNameLen);
    pbiinfo("pBatDesTemp->u32BatComCount %d",pBatDesTemp->u32BatComCount);
    pbiinfo("pBatDesTemp->u16ServListDesNum =%d",pBatDesTemp->u16ServListDesNum);
    pbiinfo("pBatDesTemp->u16LwListNum =%d",pBatDesTemp->u16LwListNum);
    pbiinfo("pBatDesTemp->u16ServInfoCount =%d",pBatDesTemp->u16ServInfoCount);
    pbiinfo("pBatDesTemp->u16ListNum =%d",pBatDesTemp->u16ListNum);
    pbiinfo("pBatDesTemp->u32PsiSiDescCount =%d",pBatDesTemp->u32PsiSiDescCount);
    pbiinfo("pBatDesTemp->u32PrivateDataLen =%d",pBatDesTemp->u32PrivateDataLen);
    
    pFlashMem = (U8 *)psisi_section_malloc(u32FlashMemSize);
    if( NULL == pFlashMem )
    {
        return PV_NO_MEMORY;
    }

    memset( pFlashMem, 0, u32FlashMemSize );

    pBatDesTemp->pBatHead = pFlashMem;
    pFlashMem += sizeof(BAT_Section_t);

    if( 0 != pBatDesTemp->u32BouquetNameDescCount )
    {
        pBatDesTemp->pBouquetNameDesc = pFlashMem;
        pFlashMem += pBatDesTemp->u32BouquetNameDescCount * sizeof(Bouquet_Name_t);
    }
    else
    {
        pBatDesTemp->pBouquetNameDesc = NULL;
    }
    if( 0 != pBatDesTemp->u32BouquetNameLen )
    {
        pBatDesTemp->pName = pFlashMem;
        pFlashMem += pBatDesTemp->u32BouquetNameLen;
    }
    else
    {
        pBatDesTemp->pName = NULL;
    }
    if( 0 != pBatDesTemp->u32BatComCount )
    {
        pBatDesTemp->pBatCom = pFlashMem;
        pFlashMem += pBatDesTemp->u32BatComCount * sizeof(BAT_Component_t);
    }
    else
    {
        pBatDesTemp->pBatCom = NULL;
    }
    if( 0 != pBatDesTemp->u16ServListDesNum )
    {
        pBatDesTemp->pServList = pFlashMem;
        pFlashMem += pBatDesTemp->u16ServListDesNum * sizeof( Service_List_Des_t);
    }
    else
    {
        pBatDesTemp->pServList = NULL;
    }
    if( 0 != pBatDesTemp->u16LwListNum )
    {
        pBatDesTemp->pLwList = pFlashMem;
        pFlashMem += pBatDesTemp->u16LwListNum * sizeof( LW_Logical_List_t );
    }
    else
    {
        pBatDesTemp->pLwList = NULL;
    }
    if( 0 != pBatDesTemp->u16ServInfoCount )
    {
        pBatDesTemp->pServInfo = pFlashMem;
        pFlashMem += pBatDesTemp->u16ServInfoCount *sizeof( Service_Info_t );
    }
    else
    {
        pBatDesTemp->pServInfo = NULL;
    }
    if( 0 != pBatDesTemp->u16ListNum )
    {
        pBatDesTemp->pLogicalInfo = pFlashMem;
        pFlashMem += pBatDesTemp->u16ListNum *sizeof( LW_Logical_Num_t );
    }
    else
    {
        pBatDesTemp->pLogicalInfo = NULL;
    }
    if( 0 != pBatDesTemp->u32PsiSiDescCount )
    {
        pBatDesTemp->pPsiSiDesc = pFlashMem;
        pFlashMem += pBatDesTemp->u32PsiSiDescCount *sizeof( PSISI_Descriptor_t );
    }
    else
    {
        pBatDesTemp->pPsiSiDesc = NULL;
    }
    if( 0 != pBatDesTemp->u32PrivateDataLen )
    {
        pBatDesTemp->pPrivateData = pFlashMem;
        pFlashMem += pBatDesTemp->u32PrivateDataLen;
    }
    else
    {
        pBatDesTemp->pPrivateData = NULL;
    }


    return PV_SUCCESS;
}



S32 PSISI_Parse_BAT_Section(PSISI_Table_t * ptTable, U8 * ptData, U16 u16Len)
{
    U8      u8MemCntFlag = 1;
    S32     s32RetErr = PV_SUCCESS;
    U8 *    pu8DataPtr = NULL;
    
    _BAT_Sect_Template_t *ptBAT_Head = NULL;
    _BAT_Item_Template_t *ptItem_Head = NULL;

    BAT_Section_t  *ptBATSection = NULL;
    _BAT_Des_Temp_t tBatDesTemp; 

    S16 s16SectionLen = 0;
    S16 s16SectOtherLen = 0;
    S16 s16ServLoop1Len = 0;
    S16 s16ServLoop2Len = 0;
    
    U16 u16BouquetId = 0;
    U32 u32CRC = 0;
    U8 u8Version = 0;


    U8  u8DescTag = 0;
    U8  u8DescLen = 0;
    
    if ((NULL == ptTable) || (NULL == ptData))
    {
        return PV_NULL_PTR;
    }

    if ((ptData[0] != BAT_TABLE_ID) || (u16Len < BAT_MIN_SECTION_LEN))
    {
        return PV_INVALID_PARAM;
    }

    memset( &tBatDesTemp, 0, sizeof(_BAT_Des_Temp_t) );

    pu8DataPtr = ptData;
    
    ptBAT_Head = (_BAT_Sect_Template_t *)pu8DataPtr;

    u8Version = (U8)( ptBAT_Head->version_number);
    s16SectionLen = (S16)((ptBAT_Head->sec_len_hi << 8) + ptBAT_Head->sec_len_lo + 3);
    u16BouquetId = (U16)( (ptBAT_Head->bouquet_id_hi << 8) | (ptBAT_Head->bouquet_id_lo) );
    u32CRC = (pu8DataPtr[s16SectionLen - 4] << 24)
            + (pu8DataPtr[s16SectionLen - 3] << 16)
            + (pu8DataPtr[s16SectionLen - 2] << 8)
            + (pu8DataPtr[s16SectionLen - 1]);



parse_begin:  

    pu8DataPtr = ptData;
     pbiinfo("zxguan[%s %d]-------parse------\n",__FUNCTION__,__LINE__);
    if( 0 != u8MemCntFlag )
    {
         if ((ptTable->u8TableId != ptBAT_Head->table_id))
             //   || (ptTable->u16ExtendId != ptBAT_Head->))
         {
            return PV_INVALID_PARAM;
         }

         if (ptTable->u16SectionNum != (U16)(ptBAT_Head->last_sec_num + 1))
         {
            /* maybe table update */
            return PSISI_ERR_TABLE_UPDATE;
         }

        if (ptTable->ppSection[ptBAT_Head->sec_num] != NULL)
        {
            ptBATSection = (BAT_Section_t *)(ptTable->ppSection[ptBAT_Head->sec_num]);
            if ((ptBATSection->u8Version != ptBAT_Head->version_number)
                || (ptBATSection->u32CRC != u32CRC))
            {
                return PSISI_ERR_TABLE_UPDATE;
            }
            else
            {
                return PSISI_ERR_TABLE_REPEAT;
            }
        }
    }
    else
    {
        s32RetErr = _BuildBatFlashMem( &tBatDesTemp );
        if( PV_SUCCESS != s32RetErr )
        {
            return s32RetErr;
        }

        tBatDesTemp.pBatHead->u16BouquetId = u16BouquetId;
        tBatDesTemp.pBatHead->u8Version = u8Version;
        tBatDesTemp.pBatHead->u32CRC = u32CRC;
        tBatDesTemp.pBatHead->pBouquetCom = tBatDesTemp.pBatCom;
        tBatDesTemp.pBatHead->pBouquetName = tBatDesTemp.pBouquetNameDesc;
        tBatDesTemp.pBatHead->pDescriptor = tBatDesTemp.pPsiSiDesc;
        tBatDesTemp.pBatHead->pLwLogicalInfo = tBatDesTemp.pLogicalInfo;
    }
    
    s16SectOtherLen = s16SectionLen - BAT_HEAD_LEN - 4;  /* 除去表头及 CRC 剩余的部分 */
    pu8DataPtr += BAT_HEAD_LEN;
    s16ServLoop1Len = (S16)( (ptBAT_Head->bouquet_desc_len_hi << 8) |(ptBAT_Head->bouquet_desc_len_lo) );

   // s16ServLoop2Len = s16SectOtherLen - s16ServLoop1Len;

    //U32 BouquenNameCount = 0;  // no use
    U32 PsiSiDescCount = 0;
    U32 NameDesCount = 0;
    U32 BatComDesCount = 0;
    U32 LogicalChannelCount = 0;
    
    U16 LwLogicalListDesCount = 0;
    U16 u16BouquetNameLen = 0;
    U16 u16PsisiTempLen = 0;
    U16 u16LogicalTempCount = 0;

    U32 ServListDesCount = 0;
    U16 u16ServListTempCount = 0;
    U16 LwLogChanCount  = 0;
    U16 LwLogicalChannelNUM = 0;
    // pbiinfo("zxguan[%s %d]-------loop1 %d---DescTab 0x%x  DescLen 0x%x---\n",__FUNCTION__,__LINE__,s16ServLoop1Len, pu8DataPtr[0], pu8DataPtr[1]);
    while( s16ServLoop1Len > 0 )
    {
            u8DescTag = pu8DataPtr[0];
            u8DescLen = pu8DataPtr[1];

            
            s16ServLoop1Len -= (u8DescLen + 2);
            
            switch( u8DescTag )
            {
                case BOUQUET_NAME_DESCRIPTOR_TAG:
                {
                    if( 0 != u8MemCntFlag )
                    {
                        u16BouquetNameLen = 0;
                        s32RetErr = parse_bouquet_name_descriptor( pu8DataPtr, NULL, &u16BouquetNameLen );
                        if( PV_SUCCESS != s32RetErr )
                        {
                            return PV_FAILURE;
                        }
                        tBatDesTemp.u32BouquetNameDescCount++;
                        tBatDesTemp.u32BouquetNameLen += u16BouquetNameLen;
                    }
                    else
                    {
                       // pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                        s32RetErr = parse_bouquet_name_descriptor( pu8DataPtr, &tBatDesTemp.pBouquetNameDesc[NameDesCount], tBatDesTemp.pName );
                        if( PV_SUCCESS != s32RetErr )
                        {
                            psisi_section_free( tBatDesTemp.pBatHead );
                            return PV_FAILURE;
                        }
                        tBatDesTemp.pName += tBatDesTemp.pBouquetNameDesc[NameDesCount].u8BuquetNameLen;
                        NameDesCount++;
                        //pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                    }
            }
            break;
                default:
                {
                    if( 0 != u8MemCntFlag )
                    {
                        u16PsisiTempLen = 0;
                        s32RetErr = parse_unknown_descriptor( pu8DataPtr, NULL, &u16PsisiTempLen, 1 );
                        if( PV_SUCCESS != s32RetErr )
                        {
                            return PV_FAILURE;
                        }
                        tBatDesTemp.u32PsiSiDescCount++;
                        tBatDesTemp.u32PrivateDataLen += u16PsisiTempLen;
                    }
                    else
                    {
                       // pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                        s32RetErr = parse_unknown_descriptor( pu8DataPtr, &tBatDesTemp.pPsiSiDesc[PsiSiDescCount], tBatDesTemp.pPrivateData, 0 );
                        if( PV_SUCCESS != s32RetErr )
                        {
                            psisi_section_free( tBatDesTemp.pBatHead );
                            return PV_FAILURE;
                        }
                        tBatDesTemp.pPrivateData +=  tBatDesTemp.pPsiSiDesc[PsiSiDescCount].u8Len;
                        PsiSiDescCount++;
                      //  pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                    }
            }
            break;
            }
          //  pbiinfo("pu8dataptr 0x%x. len = 0x%x \n",*pu8DataPtr, pu8DataPtr[1]);
            pu8DataPtr += (u8DescLen + 2);
           // pbiinfo("zxguan[%s %d]====data 0x%x===len %d=====\n",__FUNCTION__,__LINE__,pu8DataPtr[0],pu8DataPtr[1]);
    }

    U16 TS_Id = 0;
    U16 ON_Id = 0;
    S16 Loop3Len =0;
    
    s16ServLoop2Len = ( (pu8DataPtr[0] & 0xF ) << 8  ) | (pu8DataPtr[1]);
 //pbiinfo("zxguan[%s %d]-------parse----loop2 0x%x---data0-1 0x%x-  0x%x---\n",__FUNCTION__,__LINE__,s16ServLoop2Len,pu8DataPtr[0],pu8DataPtr[1]);
    pu8DataPtr += 2;
    while( s16ServLoop2Len > 0 )
    {

            TS_Id = (pu8DataPtr[0] << 8) |( pu8DataPtr[1]);
            ON_Id = (pu8DataPtr[2] << 8) |( pu8DataPtr[3]);

            //pbiinfo(" TSId 0x%x   On ID 0x%x....\n",TS_Id, ON_Id);
        
            Loop3Len = ( (pu8DataPtr[4] & 0xF) << 8   ) | (pu8DataPtr[5]);
            //pbiinfo("zxguan[%s %d]-loop3len = %d-------\n",__FUNCTION__,__LINE__,Loop3Len);
            s16ServLoop2Len -= (Loop3Len + 6 );
            pu8DataPtr += 6;
            
            while( Loop3Len > 0 )
            {
                    u8DescTag = pu8DataPtr[0];
                    u8DescLen = pu8DataPtr[1];
                    Loop3Len -= (u8DescLen + 2);
                    // pbiinfo("zxguan[%s %d]-------tag-0x%x--loop3Len %d---\n",__FUNCTION__,__LINE__,u8DescTag,Loop3Len);
                    switch( u8DescTag )
                    {
                        case SERVICE_LIST_DESCRIPTOR_TAG:
                        {
                            if( 0 != u8MemCntFlag )
                            {
                                u16ServListTempCount = 0;
                                // pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                                s32RetErr = parse_service_list_descriptor( pu8DataPtr, NULL, (void *)&u16ServListTempCount );
                                if( PV_SUCCESS != s32RetErr )
                                {
                                    pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                                    return PV_FAILURE;
                                }
                                tBatDesTemp.u16ServInfoCount += u16ServListTempCount;
                                tBatDesTemp.u16ServListDesNum++;
                            }
                            else
                            {
                              //  pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                                tBatDesTemp.pBatCom[BatComDesCount].u16ON_Id = ON_Id;
                                tBatDesTemp.pBatCom[BatComDesCount].u16TS_Id = TS_Id;

                                if( NULL == tBatDesTemp.pBatCom[BatComDesCount].pServList )
                                {
                                    tBatDesTemp.pBatCom[BatComDesCount].pServList = tBatDesTemp.pServList;
                                    tBatDesTemp.pServList++;
                                    ServListDesCount = 0;
                              //      pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                                }
                            
                                ServListDesCount = tBatDesTemp.pBatCom[BatComDesCount].u16ServListDesNum;
                            //    pbiinfo("zxguan[%s %d]-----ServListDesCount %d--------\n",__FUNCTION__,__LINE__,ServListDesCount);
                                s32RetErr = parse_service_list_descriptor( pu8DataPtr, \ 
                                    &tBatDesTemp.pBatCom[BatComDesCount].pServList[ServListDesCount], tBatDesTemp.pServInfo );
                                if( PV_SUCCESS != s32RetErr )
                                {
                                    pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                                    return PV_FAILURE;
                                }
                                tBatDesTemp.pServInfo += tBatDesTemp.pBatCom[BatComDesCount].pServList[ServListDesCount].u16ListNum;
                                tBatDesTemp.pBatCom[BatComDesCount].u16ServListDesNum++;
                            }   
                            
                }
                break;
                        case 0x83:
                        {
                             if( 0 != u8MemCntFlag )
                            {
                                LwLogChanCount = 0;
                              //  pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                                s32RetErr = _parse_LW_Logical_Channel( pu8DataPtr, NULL, &LwLogChanCount );
                                if( PV_SUCCESS != s32RetErr )
                                {
                                    pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                                    return PV_FAILURE;
                                }
                                tBatDesTemp.u16LwListNum++;
                                tBatDesTemp.u16ListNum += LwLogChanCount;
                            }
                            else
                            {
                              //  pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                                tBatDesTemp.pBatCom[BatComDesCount].u16ON_Id = ON_Id;
                                tBatDesTemp.pBatCom[BatComDesCount].u16TS_Id = TS_Id;
                                if( NULL == tBatDesTemp.pBatCom[BatComDesCount].pLwList )
                                {
                                    tBatDesTemp.pBatCom[BatComDesCount].pLwList = tBatDesTemp.pLwList;
                                    tBatDesTemp.pLwList++;
                                    LwLogicalListDesCount = 0;
                                }
                               // pbiinfo("zxguan[%s %d]------BatComDesCount = %d-------\n",__FUNCTION__,__LINE__,BatComDesCount);
                                LwLogicalListDesCount = tBatDesTemp.pBatCom[BatComDesCount].u16LwListNum;
                                s32RetErr = _parse_LW_Logical_Channel( pu8DataPtr, \
                                    &tBatDesTemp.pBatCom[BatComDesCount].pLwList[LwLogicalListDesCount], tBatDesTemp.pLogicalInfo );
                                if( PV_SUCCESS != s32RetErr )
                                {
                                    pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                                    return PV_FAILURE;
                                }
                              //  pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                                tBatDesTemp.pLogicalInfo += tBatDesTemp.pBatCom[BatComDesCount].pLwList[LwLogicalListDesCount].u16ListNum;
                                LwLogicalChannelNUM += tBatDesTemp.pBatCom[BatComDesCount].pLwList[LwLogicalListDesCount].u16ListNum;
                                tBatDesTemp.pBatCom[BatComDesCount].u16LwListNum++;
                            }

                }
                break;
                        default:
                        {
                            if( 0 != u8MemCntFlag )
                            {
                                u16PsisiTempLen = 0;
                              //  pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                                s32RetErr = parse_unknown_descriptor( pu8DataPtr, NULL, &u16PsisiTempLen, 1 );
                                if( PV_SUCCESS != s32RetErr )
                                {
                                    return PV_FAILURE;
                                }
                                tBatDesTemp.u32PsiSiDescCount++;
                                tBatDesTemp.u32PrivateDataLen += u16PsisiTempLen;
                            }
                            else
                            {
                             //   pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                                s32RetErr = parse_unknown_descriptor( pu8DataPtr, &tBatDesTemp.pPsiSiDesc[PsiSiDescCount], tBatDesTemp.pPrivateData, 0 );
                                if( PV_SUCCESS != s32RetErr )
                                {
                                    psisi_section_free( tBatDesTemp.pBatHead );
                                    return PV_FAILURE;
                                }
                                tBatDesTemp.pPrivateData +=  tBatDesTemp.pPsiSiDesc[PsiSiDescCount].u8Len;
                                PsiSiDescCount++;
                           //     pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
                            }        
                }
                break;
                    }    
//                    pbiinfo("zxguan[%s %d]-------parse--desclen %d--Loop3Len %d--\n",__FUNCTION__,__LINE__,u8DescLen,Loop3Len);
                    // pbiinfo("zxguan[%s %d]--loop2len  %d----\n",__FUNCTION__,__LINE__,s16ServLoop2Len);
                     pu8DataPtr += (u8DescLen + 2);
                     
                }
                tBatDesTemp.u32BatComCount++;
                BatComDesCount++;
              //  pbiinfo("zxguan[%s %d]--loop2len  %d----\n",__FUNCTION__,__LINE__,s16ServLoop2Len);
    }
    
   //  pbiinfo("zxguan[%s %d]-------parse------\n",__FUNCTION__,__LINE__);
    if (u8MemCntFlag != 0)
    {
        /* calculate memory size finish, goto really parse phase */
        u8MemCntFlag = 0;
        goto parse_begin;
    }
    else
    {
        tBatDesTemp.pBatHead->u16BouquetId = u16BouquetId;
        tBatDesTemp.pBatHead->u16BouquetComNum = BatComDesCount;
        tBatDesTemp.pBatHead->u16BouquetNameCount = NameDesCount;
        tBatDesTemp.pBatHead->u16PsiSiDesNum = PsiSiDescCount;
        tBatDesTemp.pBatHead->u16Logical_Channel_Num = LwLogicalChannelNUM;
        ptTable->ppSection[ptBAT_Head->sec_num] = (void*)tBatDesTemp.pBatHead;
        ptTable->u16ExtendId = u16BouquetId;
        ptTable->u16RecievedNum++;
        pbiinfo("table extendid 0x%x.\n",ptTable->u16ExtendId);
        pbiinfo("zxguan[%s %d]-------parse ok------\n",__FUNCTION__,__LINE__);
    }
    
    return PV_SUCCESS;
}


#ifdef __cplusplus
}
#endif


