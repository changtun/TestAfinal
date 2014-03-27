/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : PSI/SI
* File name   : pat.c
* Description : Include implementation of PAT parse function.
*
* History :
*   2012-05-02 ZYJ
*       Initial Version.
*******************************************************************************/
#include <stdio.h>
#include "pvddefs.h"

#include "dvbcore.h"
#include "psi_si.h"
#include "table.h"
#include "descriptor.h"

#ifdef __cplusplus
    extern "C" {
#endif

#define PAT_MIN_SECTION_LEN (12)

#define PAT_HEAD_LEN		(8)

typedef struct
{
	U8 table_id					:8;
#if TBL_BYTE_ORDER == TBL_BIG_ENDIAN
	U8 syntax_indicator			:1;
	U8 dummy					:1;		/* has to be 0 */
	U8 							:2;
	U8 sec_len_hi				:4;
#else
	U8 sec_len_hi				:4;
	U8 							:2;
	U8 dummy					:1;		/* has to be 0 */
	U8 syntax_indicator			:1;
#endif
	U8 sec_len_lo				:8;
	U8 ts_id_hi					:8;
	U8 ts_id_lo					:8;
#if TBL_BYTE_ORDER == TBL_BIG_ENDIAN
	U8 							:2;
	U8 version_number			:5;
	U8 current_next_indicator	:1;
#else
	U8 current_next_indicator	:1;
	U8 version_number			:5;
	U8 							:2;
#endif
	U8 sec_num					:8;
	U8 last_sec_num				:8;
} _PAT_Sect_Template_t;


#define PAT_ITEM_LEN		(4)

typedef struct
{
	U8 prog_num_hi				:8;
	U8 prog_num_lo				:8;
#if TBL_BYTE_ORDER == TBL_BIG_ENDIAN
	U8 							:3;
	U8 pmt_pid_hi				:5;
#else
	U8 pmt_pid_hi				:5;
	U8 							:3;
#endif
	U8 pmt_pid_lo				:8;
} _PAT_Item_Template_t;


/*****************************************************************************
 函 数 名  : PSISI_Parse_PAT_Section
 功能描述  : 解析PAT表
 输入参数  : 
             PSISI_Table_t * ptTable  
             U8 * ptData              
             U16 u16Len               
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2012年5月3日
    作    者   : zxguan
    修改内容   : 新生成函数

*****************************************************************************/
S32 PSISI_Parse_PAT_Section(PSISI_Table_t * ptTable, U8 * ptData, U16 u16Len)
{
    U8 section_num = 0;
    U8 section_last_num = 0;
    S16 section_length = 0;
    U32 section_crc = 0;
    U32 u32FlashSize = 0;
    U8 * pPatFlash = NULL;
    U8 * pPatFlashProgram = NULL;
    U8 * pStr = NULL;
    U8 ii = 0;
    U8 u8ForLen = 0;
    
    _PAT_Sect_Template_t * pPat_Sect_Temp = NULL;
    _PAT_Item_Template_t *pPat_Item_Temp = NULL;

    PAT_Section_t *pPat_Section = NULL;
        
    if ((NULL == ptTable) || (NULL == ptData))
    {
        return PV_NULL_PTR;
    }

    if ((ptData[0] != PAT_TABLE_ID) || (u16Len < PAT_MIN_SECTION_LEN))
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
    pPat_Sect_Temp = (_PAT_Sect_Template_t *)ptData;

    section_length = (S16)((pPat_Sect_Temp->sec_len_hi << 8) + pPat_Sect_Temp->sec_len_lo + 3);


    //	pat_sect->cur_next = pPat_Sect_Temp->current_next_indicator;
    section_num = pPat_Sect_Temp->sec_num;
    section_last_num = pPat_Sect_Temp->last_sec_num;

    section_crc = ( (ptData[section_length - 4] << 24) \
        | (ptData[section_length - 3] << 16) \
        | (ptData[section_length - 2] << 8) \
        | (ptData[section_length - 1]) ); 

    if( NULL != ptTable->ppSection[section_num] )
    {    
        pPat_Section = (PAT_Section_t *)ptTable->ppSection[section_num];
        if ((pPat_Section->u8Version != pPat_Sect_Temp->version_number)
            || (pPat_Section->u32CRC != section_crc))
        {
            return PSISI_ERR_TABLE_UPDATE;
        }
        else
        {
            return PSISI_ERR_TABLE_REPEAT;
        }
    }

    u8ForLen =  (section_length - PAT_HEAD_LEN - 4);
    u32FlashSize = u8ForLen + sizeof(PAT_Section_t);
    u32FlashSize = (u32FlashSize + 3) & 0xFFFFFFFC;

    pPatFlash = (U8 *)psisi_section_malloc(u32FlashSize); 
    if( NULL == pPatFlash )
    {
        return PV_NO_MEMORY;
    }

    ptTable->ppSection[section_num] = (void *)pPatFlash;
    pPat_Section = (PAT_Section_t *)ptTable->ppSection[section_num];
    pPatFlashProgram = pPatFlash + sizeof(PAT_Section_t);

    pPat_Section->u8TableId = PAT_TABLE_ID;
    pPat_Section->u16TS_Id = (pPat_Sect_Temp->ts_id_hi << 8) + pPat_Sect_Temp->ts_id_lo;
    pPat_Section->u8Version = pPat_Sect_Temp->version_number;
    pPat_Section->u32CRC = section_crc;
    pPat_Section->ptProgram = (PAT_Program_t *)pPatFlashProgram;
    pPat_Section->u16ProgNum = 0;
    pStr = ( ptData + PAT_HEAD_LEN );
    for( ii = 0; u8ForLen > 0; u8ForLen -= sizeof(PAT_Program_t), ii++ )
    {
        pPat_Item_Temp = (_PAT_Item_Template_t *)&pStr[ii * sizeof(PAT_Program_t)];
        pPat_Section->ptProgram[ii].u16ProgramNumber = (pPat_Item_Temp->prog_num_hi << 8)
    										| pPat_Item_Temp->prog_num_lo;
        pPat_Section->ptProgram[ii].u16PMTPid = (pPat_Item_Temp->pmt_pid_hi << 8) 
                                                                    | pPat_Item_Temp->pmt_pid_lo;
        pPat_Section->u16ProgNum++;
    }

    //ptTable->u8TableId = pPat_Sect_Temp->table_id;
    ptTable->u16RecievedNum++;
    printf("pat success...\n");
    return PV_SUCCESS;
}




#if 1 /* uesd test show pat data */
S32 TestPatShow(PSISI_Table_t * ptTable)
{
    PAT_Section_t *pPat_Section = NULL;
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
        pPat_Section = (PAT_Section_t *)ptTable->ppSection[ii];
        if( NULL == pPat_Section )
        {
            printf("Test-err------%d----%s-----.\n",__LINE__,__FUNCTION__);
            return PV_NULL_PTR;
        }

        printf("TableId[%d] 0x%x.\n", ii, pPat_Section->u8TableId);
        printf("Version[%d] 0x%x.\n", ii, pPat_Section->u8Version);
        printf("CRC[%d] 0x%x.\n", ii, pPat_Section->u32CRC);
        printf("ProgNum[%d] 0x%x.\n", ii, pPat_Section->u16ProgNum);
        printf("TableId[%d] 0x%x.\n", ii, pPat_Section->u8TableId);

    }
    printf("\n");
    
    return PV_SUCCESS;
}
#endif

#ifdef __cplusplus
}
#endif
