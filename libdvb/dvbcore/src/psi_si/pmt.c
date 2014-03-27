/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : PSI/SI
* File name   : pmt.c
* Description : Include implementation of PMT parse function.
*
* History :
*   2012-05-02 ZYJ
*       Initial Version.
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "pvddefs.h"

#include "dvbcore.h"
#include "psi_si.h"
#include "table.h"
#include "descriptor.h"
#include "pbitrace.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PMT_MIN_SECTION_LEN (16)

#define PMT_HEAD_LEN		(12)


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
    U8 prog_num_hi					:8;
    U8 prog_num_lo					:8;
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
    U8 								:3;
    U8 pcr_pid_hi					:5;
#else
    U8 pcr_pid_hi					:5;
    U8								:3;
#endif
    U8 pcr_pid_lo					:8;
#if TBL_BYTE_ORDER == TBL_BIG_ENDIAN
    U8 								:4;
    U8 prog_info_len_hi				:4;
#else
    U8 prog_info_len_hi				:4;
    U8								:4;
#endif
    U8 prog_info_len_lo				:8;
} _PMT_Sect_Template_t;



#define PMT_ITEM_HEAD_LEN			(5)

typedef struct
{
    U8 stream_type			:8;
#if TBL_BYTE_ORDER == TBL_BIG_ENDIAN
    U8 						:3;
    U8 elementary_pid_hi	:5;
#else
    U8 elementary_pid_hi	:5;
    U8						:3;
#endif
    U8 elementary_pid_lo	:8;

#if TBL_BYTE_ORDER == TBL_BIG_ENDIAN
    U8 						:4;
    U8 es_info_len_hi		:4;
#else
    U8 es_info_len_hi		:4;
    U8						:4;
#endif
    U8 es_info_len_lo		:8;
} _PMT_Item_Template_t;


/* 在创建PMT Flash 结构时用， 记录第一次搜索descriptor， flash 分配信息 */

typedef struct
{
    U8 u8CaNum;
    U8 u8PsiSiNum;
    U8 u8AudioNum;
    U8 u8VideoNum;
    U8 u8ComponentNum;
    U8 u8ISO639Num;
    U8 u8LangInfoNum;
    U8 u8OtherDescNum;
    U32 u32PrivateSize;
    PMT_Section_t *pSectionHead;
    CA_Desc_t *pCaDesc;
    PSISI_Descriptor_t *pPsiSiDesc;
    PSISI_Descriptor_t *pOtherDesc;
    Language_Info_t *pLangInfo;
    PMT_Component_t *pAudio;
    PMT_Component_t *pVideo;
    PMT_Component_t *pComponent;
    ISO_639_Language_Desc_t *pISO639;
    U8 *pPrivate;
} _PMT_Des_Temp_t;

/*****************************************************************************
 函 数 名  : _BuildPmtFlashMem
 功能描述  : 建立PMT 内存 结构
 输入参数  :
              PMT_Section_t *pPmt_Section
             _PMT_Des_Temp_t *pPmtDesTemp
 输出参数  : 无
 返 回 值  : static
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月4日
    作    者   : zxguan
    修改内容   : 新生成函数

*****************************************************************************/
static S32 _BuildPmtFlashMem( _PMT_Des_Temp_t *pPmtDesTemp )
{
    U32 u32FlashMemSize = 0;
    U8 *pFlashMem = NULL;

    if( NULL == pPmtDesTemp )
    {
        return PV_NULL_PTR;
    }

    u32FlashMemSize = sizeof(PMT_Section_t) \
                      + pPmtDesTemp->u8CaNum * sizeof(CA_Desc_t) \
                      + pPmtDesTemp->u8PsiSiNum * sizeof(PSISI_Descriptor_t) \
                      + pPmtDesTemp->u8OtherDescNum * sizeof(PSISI_Descriptor_t) \
                      + pPmtDesTemp->u8AudioNum * sizeof(PMT_Component_t) \
                      + pPmtDesTemp->u8VideoNum * sizeof(PMT_Component_t) \
                      + pPmtDesTemp->u8ComponentNum * sizeof(PMT_Component_t) \
                      + pPmtDesTemp->u8ISO639Num * sizeof(ISO_639_Language_Desc_t) \
                      + pPmtDesTemp->u8LangInfoNum * sizeof(Language_Info_t) \
                      + pPmtDesTemp->u32PrivateSize;

    u32FlashMemSize = (u32FlashMemSize + 3) & 0xFFFFFFFC;

    pFlashMem = (U8 *)psisi_section_malloc(u32FlashMemSize);
    if( NULL == pFlashMem )
    {
        return PV_NO_MEMORY;
    }

    memset( pFlashMem, 0, u32FlashMemSize );

    /* PMT section head */
    pPmtDesTemp->pSectionHead = (PMT_Section_t *)pFlashMem;
    pFlashMem += sizeof(PMT_Section_t);
    /* CA */
    if( 0 != pPmtDesTemp->u8CaNum )
    {
        pPmtDesTemp->pCaDesc = (CA_Desc_t *)pFlashMem;
        pFlashMem += ( pPmtDesTemp->u8CaNum * sizeof(CA_Desc_t) );
    }
    else
    {
        pPmtDesTemp->pCaDesc = NULL;
    }
    /* PSISI */
    if( 0 != pPmtDesTemp->u8PsiSiNum )
    {
        pPmtDesTemp->pPsiSiDesc = (PSISI_Descriptor_t *)pFlashMem;
        pFlashMem += ( pPmtDesTemp->u8PsiSiNum * sizeof(PSISI_Descriptor_t) );
    }
    else
    {
        pPmtDesTemp->pPsiSiDesc = NULL;
    }
    /* Other Desc*/
    if( 0 != pPmtDesTemp->u8OtherDescNum )
    {
        pPmtDesTemp->pOtherDesc = (PSISI_Descriptor_t *)pFlashMem;
        pFlashMem += ( pPmtDesTemp->u8OtherDescNum * sizeof(PSISI_Descriptor_t) );
    }
    else
    {
        pPmtDesTemp->pOtherDesc = NULL;
    }
    /* Audio */
    if( 0 != pPmtDesTemp->u8AudioNum )
    {
        pPmtDesTemp->pAudio = (PMT_Component_t *)pFlashMem;
        pFlashMem += ( pPmtDesTemp->u8AudioNum * sizeof(PMT_Component_t) );
    }
    else
    {
        pPmtDesTemp->pAudio = NULL;
    }
    /* Video */
    if( 0 != pPmtDesTemp->u8VideoNum )
    {
        pPmtDesTemp->pVideo = (PMT_Component_t *)pFlashMem;
        pFlashMem += ( pPmtDesTemp->u8VideoNum * sizeof(PMT_Component_t) );
    }
    else
    {
        pPmtDesTemp->pVideo = NULL;
    }
    /* Component */
    if( 0 != pPmtDesTemp->u8ComponentNum )
    {
        pPmtDesTemp->pComponent = (PMT_Component_t *)pFlashMem;
        pFlashMem += ( pPmtDesTemp->u8ComponentNum * sizeof(PMT_Component_t) );
    }
    else
    {
        pPmtDesTemp->pComponent = NULL;
    }
    /* ISO 639 */
    if( 0 != pPmtDesTemp->u8ISO639Num )
    {
        pPmtDesTemp->pISO639 = (ISO_639_Language_Desc_t *)pFlashMem;
        pFlashMem += ( pPmtDesTemp->u8ISO639Num * sizeof(ISO_639_Language_Desc_t) );
    }
    else
    {
        pPmtDesTemp->pISO639 = NULL;
    }
    /* LangInfo */
    if( 0 != pPmtDesTemp->u8LangInfoNum )
    {
        pPmtDesTemp->pLangInfo = (Language_Info_t *)pFlashMem;
        pFlashMem += ( pPmtDesTemp->u8LangInfoNum * sizeof(Language_Info_t) );
    }
    else
    {
        pPmtDesTemp->pLangInfo = NULL;
    }
    /* Private data */
    if( 0 != pPmtDesTemp->u32PrivateSize )
    {
        pPmtDesTemp->pPrivate = pFlashMem;
        pFlashMem += pPmtDesTemp->u32PrivateSize;
    }
    else
    {
        pPmtDesTemp->pPrivate = NULL;
    }

    return PV_SUCCESS;
}
/*****************************************************************************
 函 数 名  : PSISI_Parse_PMT_Section
 功能描述  : 解析并建立PMT数据结构
 输入参数  : PSISI_Table_t * ptTable
             U8 * ptData
             U16 u16Len
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月2日
    作    者   : zxguan
    修改内容   : 新生成函数

*****************************************************************************/
S32 PSISI_Parse_PMT_Section(PSISI_Table_t * ptTable, U8 * ptData, U16 u16Len)
{
    U8 *pStr = NULL;
    U8 section_num = 0;
    U8 DesTag = 0;
    U8 DesLen = 0;
    U8 DesFlag = 1;
    S16 section_length = 0;
    S16 prog_info_len = 0;
    U16 DesCount = 0;
    U16 es_info_len = 0;
    S32 RetErr = 0;
    U32 SectionCrc =0;
    PMT_Section_t   *pPmt_Section = NULL;
    _PMT_Sect_Template_t    *pmt_tmplt = NULL;
    _PMT_Des_Temp_t PmtDesTemp;

    if ((NULL == ptTable) || (NULL == ptData))
    {
        return PV_NULL_PTR;
    }

    if ((ptData[0] != PMT_TABLE_ID) || (u16Len < PMT_MIN_SECTION_LEN))
    {
        return PV_INVALID_PARAM;
    }


    pmt_tmplt = (_PMT_Sect_Template_t *)ptData;
    section_length = (S16)((pmt_tmplt->sec_len_hi << 8) + pmt_tmplt->sec_len_lo + 3);
    section_num = pmt_tmplt->sec_num;
    SectionCrc = ( (ptData[section_length - 4] << 24)
                   | (ptData[section_length - 3] << 16)
                   | (ptData[section_length - 2] << 8)
                   | (ptData[section_length - 1]) );

    pPmt_Section = (PMT_Section_t *)ptTable->ppSection[section_num];
    if( NULL != ptTable->ppSection[section_num] )
    {
        if ((pPmt_Section->u8Version != pmt_tmplt->version_number)
                || (pPmt_Section->u32CRC != SectionCrc))
        {
            return PSISI_ERR_TABLE_UPDATE;
        }
        else
        {
            return PSISI_ERR_TABLE_REPEAT;
        }
    }
    memset( &PmtDesTemp, 0, sizeof(_PMT_Des_Temp_t) );
AGAIN_PARSE:
#if 0
    printf("canum %d.\n",PmtDesTemp.u8CaNum);
    printf("u8PsiSiNum %d.\n",PmtDesTemp.u8PsiSiNum);
    printf("u8AudioNum %d.\n",PmtDesTemp.u8AudioNum);
    printf("u8VideoNum %d.\n",PmtDesTemp.u8VideoNum);
    printf("u8ComponentNum %d.\n",PmtDesTemp.u8ComponentNum);
    printf("u8ISO639Num %d.\n",PmtDesTemp.u8ISO639Num);
    printf("u8LangInfoNum %d.\n",PmtDesTemp.u8LangInfoNum);
    printf("u32PrivateSize %d.\n",PmtDesTemp.u32PrivateSize);
#endif

    pStr = ptData + PMT_HEAD_LEN;
    section_length = (S16)((pmt_tmplt->sec_len_hi << 8) + pmt_tmplt->sec_len_lo + 3);
    section_length = section_length - PMT_HEAD_LEN - 4;
    prog_info_len = (S16)( (pmt_tmplt->prog_info_len_hi << 8) \
                           + pmt_tmplt->prog_info_len_lo );
    section_length -= prog_info_len;
    while( prog_info_len > 0 )
    {
        DesTag = pStr[0];
        DesLen = pStr[1];

        switch( DesTag )
        {
        case CA_DESCRIPTOR_TAG:
        {
            if( 1 == DesFlag )
            {
                RetErr = parse_ca_descriptor( pStr, NULL, &PmtDesTemp.u32PrivateSize, DesFlag );
                if( PV_SUCCESS != RetErr )
                {
                    return RetErr;
                }
                PmtDesTemp.u8CaNum++;
            }
            else
            {
                if( NULL == pPmt_Section->ptCA_Desc )
                {
                    pPmt_Section->ptCA_Desc = PmtDesTemp.pCaDesc;
                }
                DesCount = pPmt_Section->u16CA_DescNum;
                RetErr = parse_ca_descriptor( pStr, &pPmt_Section->ptCA_Desc[DesCount], \
                                              PmtDesTemp.pPrivate, DesFlag );
                if( PV_SUCCESS != RetErr )
                {
                    psisi_section_free( pPmt_Section );
                    return RetErr;
                }
                pPmt_Section->u16CA_DescNum++;
                PmtDesTemp.pPrivate += pPmt_Section->ptCA_Desc[DesCount].u8PrivateDataLen;
                PmtDesTemp.pCaDesc++;
            }
        }
        break;
        default:
        {
            if( 1 == DesFlag )
            {
                RetErr = parse_unknown_descriptor( pStr, NULL, &PmtDesTemp.u32PrivateSize, DesFlag );
                if( PV_SUCCESS != RetErr )
                {
                    return RetErr;
                }
                PmtDesTemp.u8PsiSiNum++;
            }
            else
            {
                if( NULL == pPmt_Section->ptDesc )
                {
                    pPmt_Section->ptDesc = PmtDesTemp.pPsiSiDesc;
                }
                DesCount = pPmt_Section->u16OtherDescNum;
                RetErr = parse_unknown_descriptor( pStr, &pPmt_Section->ptDesc[DesCount], \
                                                   PmtDesTemp.pPrivate, DesFlag );
                if( PV_SUCCESS != RetErr )
                {
                    psisi_section_free( pPmt_Section );
                    return RetErr;
                }
                pPmt_Section->u16OtherDescNum++;
                PmtDesTemp.pPrivate += pPmt_Section->ptDesc[DesCount].u8Len;
            }
        }
        break;
        }
        pStr += (DesLen + 2);
        prog_info_len -= (DesLen + 2);
    }

    while( section_length > 0 )
    {
        PMT_Component_t *pThisComp = NULL;
        switch( pStr[0] )
        {
        case STREAM_TYPE_MPEG1_VIDEO:
        case STREAM_TYPE_MPEG2_VIDEO: /* video */
        case STREAM_TYPE_MPEG4P2:
        case STREAM_TYPE_H264:
        {
            if( 1 == DesFlag )
            {
                PmtDesTemp.u8VideoNum++;
            }
            else
            {
                //DesCount = pPmt_Section->u8
                if( NULL == pPmt_Section->ptVideo )
                {
                    pPmt_Section->ptVideo = PmtDesTemp.pVideo;
                }
                pThisComp = PmtDesTemp.pVideo;
                pThisComp->u8StreamType = pStr[0];
                pThisComp->u16ElementaryPid = ( (pStr[1] & 0x1F ) << 8 ) | pStr[2];

                PmtDesTemp.pVideo++;
            }
        }
        break;
        case STREAM_TYPE_MPEG1_AUDIO:
        case STREAM_TYPE_MPEG2_AUDIO: /* audio */
        case STREAM_TYPE_MPEG_AAC:
        case STREAM_TYPE_MPEG4_AAC:
        case STREAM_TYPE_AC3:
        {
            if( 1 == DesFlag )
            {
                PmtDesTemp.u8AudioNum++;
            }
            else
            {
                if( NULL == pPmt_Section->ptAudio )
                {
                    pPmt_Section->ptAudio = PmtDesTemp.pAudio;
                }
                DesCount = pPmt_Section->u16AudioNum;
                pThisComp = &pPmt_Section->ptAudio[DesCount];
                pThisComp->u8StreamType = pStr[0];
                pThisComp->u16ElementaryPid = ( (pStr[1] & 0x1F ) << 8 ) | pStr[2];
                pPmt_Section->u16AudioNum++;
                PmtDesTemp.pAudio++;
            }
        }
        break;

        default:
        {
            if( 1 == DesFlag )
            {
                PmtDesTemp.u8ComponentNum++;
            }
            else
            {
                if( NULL == pPmt_Section->ptComponent )
                {
                    pPmt_Section->ptComponent = PmtDesTemp.pComponent;
                }
                DesCount = pPmt_Section->u16ComponentNum;
                pThisComp = &pPmt_Section->ptComponent[DesCount];
                pThisComp->u8StreamType = pStr[0];
                pThisComp->u16ElementaryPid = ( (pStr[1] & 0x1F ) << 8 ) | pStr[2];
                pPmt_Section->u16ComponentNum++;
            }

        }
        break;
        }

        es_info_len = ( ((pStr[3] & 0xF) << 8) | pStr[4] );
        pStr += 5;
        section_length -= ( es_info_len + 5 );
        while( es_info_len > 0 )
        {
            DesTag = pStr[0];
            DesLen = pStr[1];
            switch( DesTag )
            {
            case CA_DESCRIPTOR_TAG:
            {
                if( 1 == DesFlag )
                {
                    RetErr = parse_ca_descriptor( pStr, NULL, &PmtDesTemp.u32PrivateSize, DesFlag );
                    if( PV_SUCCESS != RetErr )
                    {
                        return RetErr;
                    }
                    PmtDesTemp.u8CaNum++;
                }
                else
                {
                    if( NULL == pThisComp->ptCA_Desc )
                    {
                        pThisComp->ptCA_Desc = PmtDesTemp.pCaDesc;
                    }
                    DesCount = pThisComp->u16CA_DescNum;
                    RetErr = parse_ca_descriptor( pStr, &pThisComp->ptCA_Desc[DesCount], \
                                                  PmtDesTemp.pPrivate, DesFlag );
                    if( PV_SUCCESS != RetErr )
                    {
                        psisi_section_free( pPmt_Section );
                        return RetErr;
                    }
                    PmtDesTemp.pPrivate += pThisComp->ptCA_Desc[DesCount].u8PrivateDataLen;
                    PmtDesTemp.pCaDesc++;
                    pThisComp->u16CA_DescNum++;
                }

            }
            break;
            case ISO_639_LANGUAGE_DESCRIPTOR_TAG:
            {
                if( 1 == DesFlag )
                {
                    RetErr = parse_iso_639_language_descriptor( pStr, \
                             NULL, &PmtDesTemp.u8LangInfoNum, DesFlag );
                    if( PV_SUCCESS != RetErr )
                    {
                        return RetErr;
                    }
                    PmtDesTemp.u8ISO639Num++;
                }
                else
                {
                    if( NULL == pThisComp->ptLangDesc)
                    {
                        pThisComp->ptLangDesc = PmtDesTemp.pISO639;
                    }
                    RetErr = parse_iso_639_language_descriptor( pStr, \
                             PmtDesTemp.pISO639, PmtDesTemp.pLangInfo, DesFlag );
                    if( PV_SUCCESS != RetErr )
                    {
                        psisi_section_free( pPmt_Section );
                        return RetErr;
                    }
                    PmtDesTemp.pLangInfo += ( PmtDesTemp.pISO639->u16LangNum  );
                    PmtDesTemp.pISO639++;
                }

            }
            break;
            default:
            {
                if( 1 == DesFlag )
                {
                    RetErr = parse_unknown_descriptor( pStr, \
                                                       NULL, &PmtDesTemp.u32PrivateSize, DesFlag );
                    if( PV_SUCCESS != RetErr )
                    {
                        return RetErr;
                    }
                    PmtDesTemp.u8OtherDescNum++;
                }
                else
                {
                    if( NULL == pThisComp->ptDesc )
                    {
                        pThisComp->ptDesc = (PSISI_Descriptor_t *)PmtDesTemp.pOtherDesc;
                    }
                    DesCount = pThisComp->u16OtherDescNum;
                    RetErr = parse_unknown_descriptor( pStr, \
                                                       &pThisComp->ptDesc[DesCount], PmtDesTemp.pPrivate, DesFlag );
                    if( PV_SUCCESS != RetErr )
                    {
                        psisi_section_free( pPmt_Section );
                        return RetErr;
                    }
                    PmtDesTemp.pPrivate += pThisComp->ptDesc[DesCount].u8Len;
                    pThisComp->u16OtherDescNum++;
                    PmtDesTemp.pOtherDesc++;
                }

            }
            break;
            }
            pStr += (DesLen + 2);
            es_info_len -= (DesLen + 2);
        }
    }


    if( 1 == DesFlag )
    {
        RetErr = _BuildPmtFlashMem( &PmtDesTemp );
        if( PV_SUCCESS != RetErr )
        {
            return RetErr;
        }
        ptTable->ppSection[section_num] = (void *)PmtDesTemp.pSectionHead;
        pPmt_Section = (PMT_Section_t *)ptTable->ppSection[section_num];

        DesFlag = 0;
        goto AGAIN_PARSE;
    }
    else
    {
        //pPmt_Section = ptData->ppSection[0];
        pPmt_Section->u16ProgramNum = (pmt_tmplt->prog_num_hi << 8) + pmt_tmplt->prog_num_lo;
        pPmt_Section->u16PCRPid = (pmt_tmplt->pcr_pid_hi << 8) + pmt_tmplt->pcr_pid_lo;
        pPmt_Section->u8Version = pmt_tmplt->version_number;
        pPmt_Section->u8TableId = pmt_tmplt->table_id;
        pPmt_Section->u32CRC = SectionCrc;
        ptTable->u16RecievedNum++;
    }
    //  printf("info ---pmt success-----------%d---%s----.\n",__LINE__,__FUNCTION__);
    return PV_SUCCESS;
}



#if 1 /* uesd test show pmt data */
S32 TestPmtShow(PSISI_Table_t * ptTable)
{
    PMT_Section_t *pPmt_Section = NULL;
    int ii = 0;

    if( NULL == ptTable )
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
        pPmt_Section = (PMT_Section_t *)ptTable->ppSection[ii];
        if( NULL == pPmt_Section )
        {
            printf("Test-err------%d----%s-----.\n",__LINE__,__FUNCTION__);
            return PV_NULL_PTR;
        }

        printf("TableId[%d] 0x%x.\n", ii, pPmt_Section->u8TableId);
        printf("Version[%d] 0x%x.\n", ii, pPmt_Section->u8Version);
        printf("CRC[%d] 0x%x.\n", ii, pPmt_Section->u32CRC);
        printf("ProgramNum[%d] 0x%x.\n", ii, pPmt_Section->u16ProgramNum);
        printf("PCRPid[%d] 0x%x.\n", ii, pPmt_Section->u16PCRPid);
        printf("AudioNum[%d] 0x%x.\n", ii, pPmt_Section->u16AudioNum);
        printf("ComponentNum[%d] 0x%x.\n", ii, pPmt_Section->u16ComponentNum);
        printf("OtherDescNum[%d] 0x%x.\n", ii, pPmt_Section->u16OtherDescNum);
        printf("CA_DescNum[%d] 0x%x.\n", ii, pPmt_Section->u16CA_DescNum);

    }
    printf("\n");

    return PV_SUCCESS;
}
#endif



#ifdef __cplusplus
}
#endif
