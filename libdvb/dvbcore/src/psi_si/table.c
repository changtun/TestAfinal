/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : PSI/SI
* File name   : table.c
* Description : Include implementations of table's common API.
*
* History :
*   2012-05-02 ZYJ
*       Initial Version.
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "pvddefs.h"
#include "osapi.h"

#include "psi_si.h"
#include "table.h"
#include "pbitrace.h"

#ifdef __cplusplus
    extern "C" {
#endif

void * psisi_table_malloc(U32 u32Size)
{
    return OS_MemAllocate(system_p, u32Size);
}

void psisi_table_free(void * p)
{
    OS_MemDeallocate(system_p, p);
}

void * psisi_section_malloc(U32 u32Size)
{
    return OS_MemAllocate(system_p, u32Size);
}

void psisi_section_free(void *p)
{
    OS_MemDeallocate(system_p, p);
}


PSISI_Table_t * PSISI_Alloc_Table(U16 u16Pid, U8 * ptData, U16 u16Len)
{
    PSISI_Table_t * ptNewTable = NULL;

    if ((NULL == ptData) || (u16Len < 8))
    {
        return NULL;
    }
    if ( ((PAT_PID == u16Pid) && (PAT_TABLE_ID == ptData[0]))
        || ((CAT_PID == u16Pid) && (CAT_TABLE_ID == ptData[0]))
        || ((NIT_PID == u16Pid) && (NIT_TABLE_ID == ptData[0]))
        || ((NIT_PID == u16Pid) && (NIT_OTHER_TABLE_ID == ptData[0]))
        || ((SDT_PID == u16Pid) && (SDT_TABLE_ID == ptData[0]))
        || ((SDT_PID == u16Pid) && (SDT_OTHER_TABLE_ID == ptData[0]))
        || ((BAT_PID == u16Pid) && (BAT_TABLE_ID == ptData[0]))
        || ((EIT_PID == u16Pid) && (ptData[0] >= 0x4E) && (ptData[0] <= 0x6F) )
        || (PMT_TABLE_ID == ptData[0])    )
    {
        U8  u8LastSectNum = ptData[7];
        U32 u32Size = sizeof(PSISI_Table_t) + u8LastSectNum * sizeof(void *);
        U32 i;

        ptNewTable = psisi_table_malloc(u32Size);
        if (NULL != ptNewTable)
        {
            memset(ptNewTable, 0, u32Size);
            ptNewTable->u16TablePid = u16Pid;
            ptNewTable->u8TableId = ptData[0];
            ptNewTable->u16ExtendId = (U16)(ptData[3] << 8) + ptData[4];
            ptNewTable->u16SectionNum = u8LastSectNum + 1;
            ptNewTable->u16RecievedNum = 0;
        }

        return ptNewTable;
    }
    else
    {
        return NULL;
    }
}

void PSISI_Free_Table(PSISI_Table_t * ptTable)
{
    U16 i;

    if (NULL == ptTable)
    {
        return;
    }

    for (i=0; i<ptTable->u16SectionNum; i++)
    {
        if (ptTable->ppSection[i] != NULL)
        {
            psisi_section_free(ptTable->ppSection[i]);
        }
    }

    psisi_table_free((void *)ptTable);
}

S32 PSISI_Is_Table_Complete(PSISI_Table_t * ptTable)
{
    if (NULL == ptTable)
    {
        return 0;
    }
    pbiinfo("zxguan[%s %d]-------RecNUm %d----SecionNum %d---------\n",__FUNCTION__,__LINE__,ptTable->u16RecievedNum,ptTable->u16SectionNum );
    if (ptTable->u16RecievedNum < ptTable->u16SectionNum)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


#ifdef __cplusplus
}
#endif

