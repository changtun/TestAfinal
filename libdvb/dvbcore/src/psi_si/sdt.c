/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : PSI/SI
* File name   : sdt.c
* Description : Include implementation of SDT parse function.
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

#define SDT_MIN_SECTION_LEN (15)

#define SDT_HEAD_LEN	(11)	/* SDT 表头长度 */

typedef struct
{
	U8 table_id						:8;
#if TBL_BYTE_ORDER == TBL_BIG_ENDIAN
	U8 section_syntax_indicator		:1;
	U8 								:3;
	U8 sec_len_hi					:4;
#else
	U8 sec_len_hi					:4;
	U8 								:3;
	U8 section_syntax_indicator		:1;
#endif
	U8 sec_len_lo					:8;
	U8 ts_id_hi						:8;
	U8 ts_id_lo						:8;
#if TBL_BYTE_ORDER == TBL_BIG_ENDIAN
	U8 								:2;
	U8 version_number				:5;
	U8 current_next_indicator		:1;
#else
	U8 current_next_indicator		:1;
	U8 version_number				:5;
	U8 								:2;
#endif
	U8 sec_num						:8;
	U8 last_sec_num					:8;
	U8 on_id_hi						:8;
	U8 on_id_lo						:8;
	U8 								:8;
} _SDT_Sect_Template_t;



#define SDT_ITEM_HEAD_LEN 		(5)

typedef struct
{
	U8 service_id_hi				:8;
	U8 service_id_lo				:8;
#if TBL_BYTE_ORDER == TBL_BIG_ENDIAN
	U8 								:6;
	U8 eit_schedule_flag			:1;
	U8 eit_present_following_flag	:1;

	U8 running_status				:3;
	U8 free_ca_mode					:1;
	U8 desc_loop_len_hi				:4;
#else
	U8 eit_present_following_flag	:1;
	U8 eit_schedule_flag			:1;
	U8 								:6;

	U8 desc_loop_len_hi				:4;
	U8 free_ca_mode					:1;
	U8 running_status				:3;
#endif
	U8 desc_loop_len_lo				:8;
} _SDT_Item_Template_t;


S32 PSISI_Parse_SDT_Section(PSISI_Table_t * ptTable, U8 * ptData, U16 u16Len)
{
    _SDT_Sect_Template_t *  ptSDT_Head = NULL;
    _SDT_Item_Template_t *  ptServ_Head = NULL;

    U8      u8MemCntFlag = 1;
    S32     s32RetErr = PV_SUCCESS;

    U8 *    pu8DataPtr = NULL;
    S16     s16SectionLen = 0;
    U16     u16TS_Id = 0;
    S16     s16ServLoopLen = 0;
    U32     u32CRC = 0;

    U16     u16ServiceCnt = 0;
    U16     u16ServDescCnt = 0;
    U16     u16TextInfoCnt = 0;
    U16     u16NvodRefDescCnt = 0;
    U16     u16NvodRefInfoCnt = 0;
    U16     u16NvodTshDescCnt = 0;
    U16     u16OtherDescCnt = 0;
    U16     u16BytesCnt = 0;

    U32     u32MemSize = 0;
    U8 *    pu8MemPtr = NULL;

    SDT_Section_t *                 ptSDTSection = NULL;
    SDT_Service_t *                 ptService = NULL;
    Service_Desc_t *                ptServDesc = NULL;
    Text_Info_t *                   ptTextInfo = NULL;
    NVOD_Reference_Desc_t *         ptNvodRefDesc = NULL;
    NVOD_Ref_Info_t *               ptNvodRefInfo = NULL;
    NVOD_TimeShift_Serv_Desc_t *    ptNvodTshDesc = NULL;
    PSISI_Descriptor_t *            ptOtherDesc = NULL;
    U8 *                            pu8Bytes = NULL;

    if ((NULL == ptTable) || (NULL == ptData))
    {
        return PV_NULL_PTR;
    }

    if ( ((ptData[0] != SDT_TABLE_ID) && (ptData[0] != SDT_OTHER_TABLE_ID))
        || (u16Len < SDT_MIN_SECTION_LEN))
    {
        return PV_INVALID_PARAM;
    }

parse_begin:
    pu8DataPtr = ptData;
    ptSDT_Head = (_SDT_Sect_Template_t *)pu8DataPtr;
    s16SectionLen = (S16)((ptSDT_Head->sec_len_hi << 8) + ptSDT_Head->sec_len_lo + 3);
    u16TS_Id = (ptSDT_Head->ts_id_hi << 8) + ptSDT_Head->ts_id_lo;
    u32CRC = (pu8DataPtr[s16SectionLen - 4] << 24)
            + (pu8DataPtr[s16SectionLen - 3] << 16)
            + (pu8DataPtr[s16SectionLen - 2] << 8)
            + (pu8DataPtr[s16SectionLen - 1]);
    if (u8MemCntFlag != 0)
    {
        if ((ptTable->u8TableId != ptSDT_Head->table_id)
            || (ptTable->u16ExtendId != u16TS_Id))
        {
            return PV_INVALID_PARAM;
        }

        if (ptTable->u16SectionNum != (U16)(ptSDT_Head->last_sec_num + 1))
        {
            /* maybe table update */
            return PSISI_ERR_TABLE_UPDATE;
        }

        if (ptTable->ppSection[ptSDT_Head->sec_num] != NULL)
        {
            ptSDTSection = (SDT_Section_t *)(ptTable->ppSection[ptSDT_Head->sec_num]);
            if ((ptSDTSection->u8Version != ptSDT_Head->version_number)
                || (ptSDTSection->u32CRC != u32CRC))
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
        /* allocate memory */
        u32MemSize = sizeof(SDT_Section_t)
                    + (u16ServiceCnt * sizeof(SDT_Service_t))
                    + (u16ServDescCnt * sizeof(Service_Desc_t))
                    + (u16TextInfoCnt * sizeof(Text_Info_t))
                    + (u16NvodRefDescCnt * sizeof(NVOD_Reference_Desc_t))
                    + (u16NvodRefInfoCnt * sizeof(NVOD_Ref_Info_t))
                    + (u16NvodTshDescCnt * sizeof(NVOD_TimeShift_Serv_Desc_t))
                    + (u16OtherDescCnt * sizeof(PSISI_Descriptor_t))
                    + u16BytesCnt;
    
        u32MemSize = (u32MemSize + 3) & 0xFFFFFFFC;
        pu8MemPtr = (U8 *)psisi_section_malloc(u32MemSize);
        if (NULL == pu8MemPtr)
        {
            return PV_NO_MEMORY;
        }
        memset(pu8MemPtr, 0, u32MemSize);

        ptSDTSection = (SDT_Section_t *)pu8MemPtr;
        pu8MemPtr += sizeof(SDT_Section_t);
        if (u16ServiceCnt != 0)
        {
            ptService = (SDT_Service_t *)pu8MemPtr;
            pu8MemPtr += (u16ServiceCnt * sizeof(SDT_Service_t));
        }
        if (u16ServDescCnt != 0)
        {
            ptServDesc = (Service_Desc_t *)pu8MemPtr;
            pu8MemPtr += (u16ServDescCnt * sizeof(Service_Desc_t));
        }
        if (u16TextInfoCnt != 0)
        {
            ptTextInfo = (Text_Info_t *)pu8MemPtr;
            pu8MemPtr += (u16TextInfoCnt * sizeof(Text_Info_t));
        }
        if (u16NvodRefDescCnt != 0)
        {
            ptNvodRefDesc = (NVOD_Reference_Desc_t *)pu8MemPtr;
            pu8MemPtr += (u16NvodRefDescCnt * sizeof(NVOD_Reference_Desc_t));
        }
        if (u16NvodRefInfoCnt != 0)
        {
            ptNvodRefInfo = (NVOD_Ref_Info_t *)pu8MemPtr;
            pu8MemPtr += (u16NvodRefInfoCnt * sizeof(NVOD_Ref_Info_t));
        }
        if (u16NvodTshDescCnt != 0)
        {
            ptNvodTshDesc = (NVOD_TimeShift_Serv_Desc_t *)pu8MemPtr;
            pu8MemPtr += (u16NvodTshDescCnt * sizeof(NVOD_TimeShift_Serv_Desc_t));
        }
        if (u16OtherDescCnt != 0)
        {
            ptOtherDesc = (PSISI_Descriptor_t *)pu8MemPtr;
            pu8MemPtr += (u16OtherDescCnt * sizeof(PSISI_Descriptor_t));
        }
        if (u16BytesCnt != 0)
        {
            pu8Bytes = pu8MemPtr;
        }
        /* memory allocate finished. */

        ptSDTSection->u8TableId = ptSDT_Head->table_id;
        ptSDTSection->u8Version = ptSDT_Head->version_number;
        ptSDTSection->u16TS_Id = (ptSDT_Head->ts_id_hi << 8) + ptSDT_Head->ts_id_lo;
        ptSDTSection->u16ON_Id = (ptSDT_Head->on_id_hi << 8) + ptSDT_Head->on_id_lo;
        ptSDTSection->u32CRC = u32CRC;
        ptSDTSection->u16ServiceNum = u16ServiceCnt;
        ptSDTSection->ptService = ptService;

    }
    s16ServLoopLen = s16SectionLen - SDT_HEAD_LEN - 4;  /* 除去表头及 CRC 剩余的部分 */
    pu8DataPtr += SDT_HEAD_LEN;

    while (s16ServLoopLen > 0)
    {
        S16     s16DescLoopLen = 0;

        ptServ_Head = (_SDT_Item_Template_t *)pu8DataPtr;
        if (u8MemCntFlag != 0)
        {
            u16ServiceCnt++;
        }
        else
        {
            ptService->u16ServId = (ptServ_Head->service_id_hi << 8)
                                + ptServ_Head->service_id_lo;
            ptService->u8EitSchFlag = ptServ_Head->eit_schedule_flag;
            ptService->u8EitPF_Flag = ptServ_Head->eit_present_following_flag;
            ptService->u8RunningStatus = ptServ_Head->running_status;
            ptService->u8FreeCA_Mode = ptServ_Head->free_ca_mode;
            ptService->ptServDesc = NULL;
            ptService->ptNvodRefDesc = NULL;
            ptService->ptNvodTimeShiftDesc = NULL;
            ptService->u16OtherDescNum = 0;
            ptService->ptDesc = NULL;
        }
        s16DescLoopLen = (S16)((ptServ_Head->desc_loop_len_hi << 8)
                                + ptServ_Head->desc_loop_len_lo);
        pu8DataPtr += SDT_ITEM_HEAD_LEN;
        s16ServLoopLen -= (SDT_ITEM_HEAD_LEN + s16DescLoopLen);
        
        while (s16DescLoopLen > 0)
        {
            
            U8  u8DescTag = pu8DataPtr[0];
            U8  u8DescLen = pu8DataPtr[1];

            if (SERVICE_DESCRIPTOR_TAG == u8DescTag)
            {
                if (u8MemCntFlag != 0)
                {
                    u16ServDescCnt++;
                    s32RetErr = parse_service_descriptor(pu8DataPtr, NULL,
                            &u16TextInfoCnt, &u16BytesCnt, u8MemCntFlag);
                    if( PV_SUCCESS != s32RetErr )
                    {
                        return s32RetErr;
                    }
                }
                else
                {
                    s32RetErr = parse_service_descriptor(pu8DataPtr, ptServDesc,
                            ptTextInfo, pu8Bytes, u8MemCntFlag);
                    if( PV_SUCCESS != s32RetErr )
                    {
                        psisi_section_free( ptSDTSection );
                        return s32RetErr;
                    }
                    if ( NULL != ptServDesc->ptServProviderName )
                    {
                        ptTextInfo++;
                        pu8Bytes += ptServDesc->ptServProviderName->u8Len;
                    }
                    if ( NULL != ptServDesc->ptServName )
                    {
                        ptTextInfo++;
                        pu8Bytes += ptServDesc->ptServName->u8Len;
                    }

                    if (NULL == ptService->ptServDesc)
                    {
                        ptService->ptServDesc = ptServDesc;
                    }
                    ptServDesc++;
                }
            }
            else if (NVOD_REFERENCE_DESCRIPTOR_TAG == u8DescTag)
            {
                if (u8MemCntFlag != 0)
                {
                    u16NvodRefDescCnt++;
                    s32RetErr= parse_nvod_reference_descriptor(pu8DataPtr, NULL,
                            &u16NvodRefInfoCnt, u8MemCntFlag);
                    if( PV_SUCCESS != s32RetErr )
                    {
                        return s32RetErr;
                    }
                }
                else
                {
                    s32RetErr = parse_nvod_reference_descriptor(pu8DataPtr, ptNvodRefDesc,
                            ptNvodRefInfo, u8MemCntFlag);
                    if( PV_SUCCESS != s32RetErr )
                    {
                        psisi_section_free( ptSDTSection );
                        return s32RetErr;
                    }
                    if (ptNvodRefDesc->u16RefInfoNum != 0)
                    {
                        ptNvodRefInfo += ptNvodRefDesc->u16RefInfoNum;
                    }

                    if (NULL == ptService->ptNvodRefDesc)
                    {
                        ptService->ptNvodRefDesc = ptNvodRefDesc;
                    }
                    ptNvodRefDesc++;
                }
            }
            else if (TIME_SHIFTED_SERVICE_DESCRIPTOR_TAG == u8DescTag)
            {
                if (u8MemCntFlag != 0)
                {
                    u16NvodTshDescCnt++;
                }
                else
                {
                    s32RetErr = parse_time_shifted_service_descriptor(pu8DataPtr, ptNvodTshDesc);
                    if( PV_SUCCESS != s32RetErr )
                    {
                        psisi_section_free( ptSDTSection );
                        return s32RetErr;
                    }
                    if (NULL == ptService->ptNvodTimeShiftDesc)
                    {
                        ptService->ptNvodTimeShiftDesc = ptNvodTshDesc;
                    }
                    ptNvodTshDesc++;
                }
            }
            else
            {
                if (u8MemCntFlag != 0)
                {
                    u16OtherDescCnt++;
                    u16BytesCnt += u8DescLen;
                }
                else
                {
                    ptOtherDesc->u8Tag = u8DescTag;
                    ptOtherDesc->u8Len = u8DescLen;
                    if (u8DescLen != 0)
                    {
                        memcpy(pu8Bytes, pu8DataPtr+2, u8DescLen);
                        ptOtherDesc->pu8Data = pu8Bytes;
                        pu8Bytes += u8DescLen;
                    }
                    else
                    {
                        ptOtherDesc->pu8Data = NULL;
                    }

                    if (NULL == ptService->ptDesc)
                    {
                        ptService->ptDesc = ptOtherDesc;
                    }
                    ptService->u16OtherDescNum++;
                    ptOtherDesc++;
                }
            }

            pu8DataPtr += (u8DescLen + 2);
            s16DescLoopLen -= (u8DescLen + 2);
        }

        ptService++;
    }

    if (u8MemCntFlag != 0)
    {
        /* calculate memory size finish, goto really parse phase */
        u8MemCntFlag = 0;
        goto parse_begin;
    }
    else
    {
        ptTable->ppSection[ptSDT_Head->sec_num] = ptSDTSection;
        ptTable->u16RecievedNum++;
    }

    return PV_SUCCESS;
}


#ifdef __cplusplus
}
#endif

