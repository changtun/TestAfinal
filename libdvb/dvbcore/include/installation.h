/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : Installation
* File name   : installation.h
* Description : APIs of Installation.
*
* History :
*   2012-05-23 ZYJ
*       Initial Version.
*******************************************************************************/
#ifndef __INSTALLATION_H_
#define __INSTALLATION_H_

#include "pvddefs.h"
#include "dvbcore.h"
#include "psi_si.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define DEFAULT_SERVICE_NAME            "PROGRAM"


typedef enum
{
    /* 对于 installation 来说自动搜索, 快速搜索, 网络搜索没有什么区别. 都是先
        搜主频点 NIT, 再根据主频点 NIT 中描述的频点列表进行搜索. 若搜索 NIT 失
        败则退出搜索. 唯一的区别就是指定的主频点可能不同, 主频点数目也不相同.
        但这个区别对于 installation 来说处理方式是相同的. 之所以这里还是添加了
        这三种类型完全是为了上层使用方便 */
    eAUTO_SEARCH = 0,   /* 自动搜索 */
    eQUICK_SEARCH,      /* 快速搜索 */
    eNIT_SEARCH,        /* 网络搜索 */
    eALL_SEARCH,        /* 全频搜索 */
    eTP_SEARCH,         /* 单频点搜索 */
    eOTHER_SEARCH       /* 未定义 */
} Install_Search_Type_e;


/*******************************************************************************
* Type:
*   Install_Timeout_t
* Description:
*   该类型用于设定搜索超时时间, 单位为 ms(毫秒). 如果超时将跳过当前频点的搜索.
*******************************************************************************/
typedef struct
{
    U32 u32TunerTimeout;            /* tuner 锁频超时时间 */
    U32 u32NITActualTimeout;        /* NIT actual 超时时间 */
    U32 u32NITOtherTimeout;         /* NIT other 超时时间 */
    U32 u32BATTimeout;              /* BAT 超时时间 */
    U32 u32PATTimeout;              /* PAT 超时时间 */
    U32 u32PMTTimeout;              /* PMT 超时时间 */
    U32 u32SDTActualTimeout;        /* SDT actual 超时时间 */
    U32 u32SDTOtherTimeout;         /* SDT other 超时时间 */
} Install_Timeout_t;



typedef enum
{
    eINSTALL_COMPLETED = 0,     /* 搜索完成，返回搜索结果 */
    eINSTALL_INTERRUPT,         /* 搜索中断，返回搜索结果 */
    eINSTALL_FAILED,            /* 搜索失败 */
    eINSTALL_PROGRESS,          /* 显示搜索进度，及当前搜索频点 */
    eINSTALL_TP_COMPLETED,      /* 完成一个频点，返回完成频点的搜索数据 */
    eINSTALL_TP_FAILED,         /* 搜索一个频点失败 */
    eINSTALL_OTHER
} Install_Notify_Type_e;


typedef struct
{
    U16                     u16NetId;       /* network_id */
    U16                     u16TS_Id;       /* transport_stream_id */
    U16                     u16ON_Id;       /* original_network_id */
    DVBCore_Tuner_Desc_u    uTunerDesc;    /* 该频点的 tuner 参数 */
    PSISI_Table_t *         ptPAT;      /* 该频点搜索到的 PAT 表, NULL 表示没有 PAT */
    PSISI_Table_t *         ptSDT;      /* 该频点搜索到的 SDT 表, NULL 表示没有 SDT */
} Install_Tp_t;


typedef struct
{
    U16                 u16ServId;      /* service_id */
    U16                 u16PMTPid;      /* PMT Pid, 0xFFFF 表示该 service 没有 PMT */
    U16                 u16LogicalNum; /* 逻辑频道号 */
    U8                  u8ServType;     /* service_type */
    U8                  u8FreeCA_Mode;  /* 加扰标志, 0 - 没有加扰; 1 - 加扰 */
    U8                  uServNameType;  /* 节目名称 编码类型 */
    C8 *                sServName;      /* 节目名称, NULL 表示没有名称 */
    U32                 u32ServNameLen; /* 节目名称长度 */
    Install_Tp_t *      ptTpDesc;       /* 该 service 所在的频点信息 */
    SDT_Service_t *     ptServInfo;     /* 该 service 在 SDT 表中的描述, NULL 表示 SDT 中没有该 service */
    PSISI_Table_t *     ptPMT;          /* 该 service 对应的 PMT 表, NULL 表示没有 PMT */
} Install_Serv_t;


typedef struct
{
    U32                 u32TpNum;       /* TP 数目 */
    Install_Tp_t *      ptTpList;       /* TP list 数组 */
    U32                 u32ServNum;     /* Service 数目 */
    Install_Serv_t *    ptServList;     /* Service list 数组 */
    PSISI_Table_t *     ptNIT;          /* 主频点 NIT 表, NULL 表示没有主频点 NIT */
    PSISI_Table_t *     ptBAT;          /* 主频点 BAT 表, NULL 表示没有主频点 BAT */
} Install_Search_Result_t;



typedef struct
{
	U32                     u32Progress;    /* 搜索进度 */
	DVBCore_Tuner_Desc_u *  puCurrTuner;    /* 当前搜索频率 */
} Install_Progress_t;



/*******************************************************************************
* Type:
*   Install_Notify_pfn
* Description:
*   该类型用于定义 installation 的 notify 函数, 这种 notify 函数由上层实现.
*   在开始搜索时由上层注册给 installation 模块. 用于 installation 模块向上层
*   通报搜索状态. 上层可根据不同的 notify type 采取相应的处理.
* Parameter:
*   eNotifyType: 指定 notify type, 相当于 notify 函数的调用原因.
*   pData: 指向 notify 数据的指针. notify type 不同, 相应的数据类型也不同. 使用
*           时需要将该指针转换为相应的数据类型的指针.
*           notify type 与数据类型对应如下:
*       ---------------------------------------------------------------------
*        eINSTALL_COMPLETED     |                           |返回单频点数据时
*        eINSTALL_INTERRUPT     | Install_Search_Result_t   |没有 tp 和
*        eINSTALL_TP_COMPELETED |                           |bouquet 数据
*       ------------------------+---------------------------+----------------
*        eINSTALL_FAILED        | NULL                      |
*       ------------------------+---------------------------+----------------
*        eINSTALL_PROGRESS      | Install_Progress_t        |
*       ---------------------------------------------------------------------
* Return:
*   NULL
*
* NOTE: pData 指针仅在 notify 函数内有效. 在 notify 函数外部使用该指针指向的数
*       据是未知的.
*
*******************************************************************************/
typedef void (* Install_Notify_pfn)(Install_Notify_Type_e eNotifyType, void * pData);



/*******************************************************************************
* Type:
*   Install_Cfg_t
* Description:
*   该类型用于配置一个搜索流程, 做为函数 Installation_Start 的参数.
*   eSearchType: 指定搜索类型. 可以是 NIT 搜索, 全频搜索. 单频点搜索中的一种.
*   ptTimeout: 配置搜索时各个 table 的 retry 次数.
*   puMainTpDescList: 指定要搜索的主频点的 tuner 参数列表.
*   puNormalTpDescList: 指定要搜索的普通频点的 tuner 参数列表。
*       NIT 搜索时只需要指定主频点列表, 不必指定普通频点.
*       全频搜索需要指定要搜索的普通频点列表, 可以指定主频点, 也可以不指定主
*           频点(不指定主频点时直接搜普通频点列表, 指定主频点时先搜主频点 NIT,
*           然后优先搜索 NIT 中列出的频点, 再搜索普通频点列表中剩余的频点).
*       单频点搜索时不必指定主频点.
*   pfnNotifyFunc: 回调函数指针
*******************************************************************************/
typedef struct
{
    Install_Search_Type_e   eSearchType;
    Install_Timeout_t *     ptTimeout;
    U32                     u32MainTpNum;
    DVBCore_Tuner_Desc_u *  puMainTpDescList;
    U32                     u32NormalTpNum;
    DVBCore_Tuner_Desc_u *  puNormalTpDescList;
    Install_Notify_pfn      pfnNotifyFunc;
    U8                      u8SearchLimitFlag;
} Install_Cfg_t;


/******************************************************************************
* Function:
*   Installation_Init
* Description:
*   该函数用于初始化搜索模块.
* Parameter:
*   NULL
* Return:
*   PV_SUCCESS: success
*   PV_FAILURE: failure
* History :
*   2012-05-03 ZYJ
*       Initial.
*
*******************************************************************************/
S32 Installation_Init( void );


/******************************************************************************
* Function:
*   Installation_Start
* Description:
*   该函数用于启动一个搜索流程.
* Parameter:
*   tCfg: (I) installation 配置参数. 根据其中定制的参数进行搜索.
* Return:
*   PV_SUCCESS: success
*   PV_FAILURE: failure
* History :
*   2012-05-03 ZYJ
*       Initial.
*
*******************************************************************************/
S32 Installation_Start( Install_Cfg_t tCfg );


/******************************************************************************
* Function:
*   Installation_Stop
* Description:
*   该函数用于中断搜索.
* Parameter:
*   NULL
* Return:
*   PV_SUCCESS: success
*   PV_FAILURE: failure
* History :
*   2012-04-27 ZYJ
*       Initial.
*
*******************************************************************************/
S32 Installation_Stop( void );


/******************************************************************************
* Function:
*   Installation_Get_SearchRet
* Description:
*   该函数用于获取搜索结果.
* Parameter:
*   ptRet: (O) 返回搜索结果.
* Return:
*   PV_SUCCESS: success
*   PV_FAILURE: 没有可用的搜索结果
*   PV_NULL_PTR: null pointer err
* History :
*   2012-05-23 ZYJ
*       Initial.
*
*******************************************************************************/
S32 Installation_Get_SearchRet( Install_Search_Result_t *ptRet );


/******************************************************************************
* Function:
*   Installation_Free_SearchRet
* Description:
*   该函数用于释放搜索结果.
* Parameter:
*   NULL
* Return:
*   NULL
* History :
*   2012-05-03 ZYJ
*       Initial.
*
*******************************************************************************/
void Installation_Free_SearchRet( void );


/******************************************************************************
* Function:
*   Installation_Deinit
* Description:
*   该函数用于去初始化搜索模块, 释放搜索模块占用的资源.
* Parameter:
*   NULL
* Return:
*   PV_SUCCESS: success
*   PV_FAILURE: failure
* History :
*   2012-04-27 ZYJ
*       Initial.
*
*******************************************************************************/
S32 Installation_Deinit( void );
U16 _Get_install_Nit_Version(void);

#ifdef __cplusplus
}
#endif


#endif	/* __INSTALLATION_H_ */

