/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : PSI/SI
* File name   : table.h
* Description : Include table's common definitions, API prototypes.
*
* History :
*   2012-04-28 ZYJ
*       Initial Version.
*******************************************************************************/
#ifndef __TABLE_H_
#define __TABLE_H_

#include "pvddefs.h"

#ifdef __cplusplus
    extern "C" {
#endif


#define TBL_BIG_ENDIAN		(12345678)
#define TBL_LITTER_ENDIAN	(78563412)

#define TBL_BYTE_ORDER		TBL_LITTER_ENDIAN


void * psisi_table_malloc(U32 u32Size);
void psisi_table_free(void * p);

void * psisi_section_malloc(U32 u32Size);
void psisi_section_free(void *p);



#ifdef __cplusplus
}
#endif


#endif	/* __TABLE_H_ */

