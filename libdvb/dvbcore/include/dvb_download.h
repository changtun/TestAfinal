/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : dvb_download
* File name   : dvb_download.h
* Description : dvb_download.
*
* History :
*   2013-05-09 ZDW
*       Initial Version.
*******************************************************************************/

#ifndef __DVBDL_H_
#define __DVBDL_H_

#include "pvddefs.h"
#include "dvbcore.h"

#ifdef __cplusplus
extern "C" {
#endif

/*{{移植时需按项目添加修改*/
typedef enum
{
	ePRODUCT_F835 = 0x01,
	ePRODUCT_F865 = 0x02,
	ePRODUCT_OTHER
}_Product_ID_e;

typedef enum
{
	eUSER_LW = 0x01,
	eUSER_SX = 0x02,	
	//eUSER_LW,
	eUSER_OTHER
}_User_ID_e;
/*}}移植时需按项目添加修改*/

typedef enum
{
	eDVB_C = 0x01,
	eDVB_T = 0x02,
	eDVB_S = 0x03
}_TRANS_TYPE_e;

typedef struct
{
	U8  		Download_Type;
	U16 		Pid;
	U32		Frequnce;
	U32		SymbolRate;
	U8		QamSize;
	U8		Bandwidth;
	_TRANS_TYPE_e		Trans_Type;
}Loader_msg_t;

typedef struct
{
	U16     u16Pid;
	//U8      u8TableId;
	//U8      reserved1;
	//U16     u16ExtendId;
	U32     u32TimeoutMs;
} _DL_Param_t;

typedef enum
{
	eSEARCH_LOADER_TABLE_SUCCESS = 0,
	eSEARCH_LOADER_TABLE_TIMEOUT,
	eSEARCH_LOADER_TABLE_FAILED,
	eSEARCH_LOADER_TABLE_STOP,
	eSEARCH_LOADER_TABLE_PARAM_ERROR,
	eSEARCH_LOADER_TABLE_TUNER_ERROR,
	eSEARCH_LOADER_TABLE_SECTION_ERROR,
	eSEARCH_LOADER_TABLE_SAVE_ERROR,
	eSEARCH_LOADER_TABLE_OTHER_ERROR
}_Search_Loader_Tbl_Err_e;

typedef enum
{
    eDOWNLOAD_COMPLETED = 0,     /* 下载完成*/
    eDOWNLOAD_FAILED,            /* 下载失败 */
    eDOWNLOAD_PROGRESS,          /* 显示下载进度*/
    eDOWNLOAD_OTHER
}DL_Notify_Type_e;

typedef void(* SectionFreeFunc)(void *ptr );

typedef struct _table_msg_t
{
	U16                 pid;
	U8                  table_id;
	U16                 sec_len;
	U8 *                buffer;
	SectionFreeFunc     free_func;
} _DL_Msg_t;

typedef void (*DVB_DLMsg_Notify )(int  param1, int  param2);

/***********************************************************
*Dvb_DownLoad_Start:提供上层下载升级数据的接口
*
* History :
*   2013-05-09  by ZDW
************************************************************/
S32 Dvb_DownLoad_Start(void/*Loader_msg_t *dlmsg*/);

void DL_Info_RegMessage_Notify_CallBack(DVB_DLMsg_Notify Cb);
void DL_Info_UnRegMessage_Notify_CallBack(void);

void  Dvb_DownLoad_SetStatus(U32 dl_stastus);
U32 Dvb_DownLoad_GetStatus(void);
#ifdef __cplusplus
}
#endif

#endif /* __DVBDL_H_ */

