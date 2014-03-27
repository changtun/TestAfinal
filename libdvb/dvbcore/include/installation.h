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
    /* ���� installation ��˵�Զ�����, ��������, ��������û��ʲô����. ������
        ����Ƶ�� NIT, �ٸ�����Ƶ�� NIT ��������Ƶ���б��������. ������ NIT ʧ
        �����˳�����. Ψһ���������ָ������Ƶ����ܲ�ͬ, ��Ƶ����ĿҲ����ͬ.
        ������������ installation ��˵����ʽ����ͬ��. ֮�������ﻹ�������
        ������������ȫ��Ϊ���ϲ�ʹ�÷��� */
    eAUTO_SEARCH = 0,   /* �Զ����� */
    eQUICK_SEARCH,      /* �������� */
    eNIT_SEARCH,        /* �������� */
    eALL_SEARCH,        /* ȫƵ���� */
    eTP_SEARCH,         /* ��Ƶ������ */
    eOTHER_SEARCH       /* δ���� */
} Install_Search_Type_e;


/*******************************************************************************
* Type:
*   Install_Timeout_t
* Description:
*   �����������趨������ʱʱ��, ��λΪ ms(����). �����ʱ��������ǰƵ�������.
*******************************************************************************/
typedef struct
{
    U32 u32TunerTimeout;            /* tuner ��Ƶ��ʱʱ�� */
    U32 u32NITActualTimeout;        /* NIT actual ��ʱʱ�� */
    U32 u32NITOtherTimeout;         /* NIT other ��ʱʱ�� */
    U32 u32BATTimeout;              /* BAT ��ʱʱ�� */
    U32 u32PATTimeout;              /* PAT ��ʱʱ�� */
    U32 u32PMTTimeout;              /* PMT ��ʱʱ�� */
    U32 u32SDTActualTimeout;        /* SDT actual ��ʱʱ�� */
    U32 u32SDTOtherTimeout;         /* SDT other ��ʱʱ�� */
} Install_Timeout_t;



typedef enum
{
    eINSTALL_COMPLETED = 0,     /* ������ɣ������������ */
    eINSTALL_INTERRUPT,         /* �����жϣ������������ */
    eINSTALL_FAILED,            /* ����ʧ�� */
    eINSTALL_PROGRESS,          /* ��ʾ�������ȣ�����ǰ����Ƶ�� */
    eINSTALL_TP_COMPLETED,      /* ���һ��Ƶ�㣬�������Ƶ����������� */
    eINSTALL_TP_FAILED,         /* ����һ��Ƶ��ʧ�� */
    eINSTALL_OTHER
} Install_Notify_Type_e;


typedef struct
{
    U16                     u16NetId;       /* network_id */
    U16                     u16TS_Id;       /* transport_stream_id */
    U16                     u16ON_Id;       /* original_network_id */
    DVBCore_Tuner_Desc_u    uTunerDesc;    /* ��Ƶ��� tuner ���� */
    PSISI_Table_t *         ptPAT;      /* ��Ƶ���������� PAT ��, NULL ��ʾû�� PAT */
    PSISI_Table_t *         ptSDT;      /* ��Ƶ���������� SDT ��, NULL ��ʾû�� SDT */
} Install_Tp_t;


typedef struct
{
    U16                 u16ServId;      /* service_id */
    U16                 u16PMTPid;      /* PMT Pid, 0xFFFF ��ʾ�� service û�� PMT */
    U16                 u16LogicalNum; /* �߼�Ƶ���� */
    U8                  u8ServType;     /* service_type */
    U8                  u8FreeCA_Mode;  /* ���ű�־, 0 - û�м���; 1 - ���� */
    U8                  uServNameType;  /* ��Ŀ���� �������� */
    C8 *                sServName;      /* ��Ŀ����, NULL ��ʾû������ */
    U32                 u32ServNameLen; /* ��Ŀ���Ƴ��� */
    Install_Tp_t *      ptTpDesc;       /* �� service ���ڵ�Ƶ����Ϣ */
    SDT_Service_t *     ptServInfo;     /* �� service �� SDT ���е�����, NULL ��ʾ SDT ��û�и� service */
    PSISI_Table_t *     ptPMT;          /* �� service ��Ӧ�� PMT ��, NULL ��ʾû�� PMT */
} Install_Serv_t;


typedef struct
{
    U32                 u32TpNum;       /* TP ��Ŀ */
    Install_Tp_t *      ptTpList;       /* TP list ���� */
    U32                 u32ServNum;     /* Service ��Ŀ */
    Install_Serv_t *    ptServList;     /* Service list ���� */
    PSISI_Table_t *     ptNIT;          /* ��Ƶ�� NIT ��, NULL ��ʾû����Ƶ�� NIT */
    PSISI_Table_t *     ptBAT;          /* ��Ƶ�� BAT ��, NULL ��ʾû����Ƶ�� BAT */
} Install_Search_Result_t;



typedef struct
{
	U32                     u32Progress;    /* �������� */
	DVBCore_Tuner_Desc_u *  puCurrTuner;    /* ��ǰ����Ƶ�� */
} Install_Progress_t;



/*******************************************************************************
* Type:
*   Install_Notify_pfn
* Description:
*   ���������ڶ��� installation �� notify ����, ���� notify �������ϲ�ʵ��.
*   �ڿ�ʼ����ʱ���ϲ�ע��� installation ģ��. ���� installation ģ�����ϲ�
*   ͨ������״̬. �ϲ�ɸ��ݲ�ͬ�� notify type ��ȡ��Ӧ�Ĵ���.
* Parameter:
*   eNotifyType: ָ�� notify type, �൱�� notify �����ĵ���ԭ��.
*   pData: ָ�� notify ���ݵ�ָ��. notify type ��ͬ, ��Ӧ����������Ҳ��ͬ. ʹ��
*           ʱ��Ҫ����ָ��ת��Ϊ��Ӧ���������͵�ָ��.
*           notify type ���������Ͷ�Ӧ����:
*       ---------------------------------------------------------------------
*        eINSTALL_COMPLETED     |                           |���ص�Ƶ������ʱ
*        eINSTALL_INTERRUPT     | Install_Search_Result_t   |û�� tp ��
*        eINSTALL_TP_COMPELETED |                           |bouquet ����
*       ------------------------+---------------------------+----------------
*        eINSTALL_FAILED        | NULL                      |
*       ------------------------+---------------------------+----------------
*        eINSTALL_PROGRESS      | Install_Progress_t        |
*       ---------------------------------------------------------------------
* Return:
*   NULL
*
* NOTE: pData ָ����� notify ��������Ч. �� notify �����ⲿʹ�ø�ָ��ָ�����
*       ����δ֪��.
*
*******************************************************************************/
typedef void (* Install_Notify_pfn)(Install_Notify_Type_e eNotifyType, void * pData);



/*******************************************************************************
* Type:
*   Install_Cfg_t
* Description:
*   ��������������һ����������, ��Ϊ���� Installation_Start �Ĳ���.
*   eSearchType: ָ����������. ������ NIT ����, ȫƵ����. ��Ƶ�������е�һ��.
*   ptTimeout: ��������ʱ���� table �� retry ����.
*   puMainTpDescList: ָ��Ҫ��������Ƶ��� tuner �����б�.
*   puNormalTpDescList: ָ��Ҫ��������ͨƵ��� tuner �����б�
*       NIT ����ʱֻ��Ҫָ����Ƶ���б�, ����ָ����ͨƵ��.
*       ȫƵ������Ҫָ��Ҫ��������ͨƵ���б�, ����ָ����Ƶ��, Ҳ���Բ�ָ����
*           Ƶ��(��ָ����Ƶ��ʱֱ������ͨƵ���б�, ָ����Ƶ��ʱ������Ƶ�� NIT,
*           Ȼ���������� NIT ���г���Ƶ��, ��������ͨƵ���б���ʣ���Ƶ��).
*       ��Ƶ������ʱ����ָ����Ƶ��.
*   pfnNotifyFunc: �ص�����ָ��
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
*   �ú������ڳ�ʼ������ģ��.
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
*   �ú�����������һ����������.
* Parameter:
*   tCfg: (I) installation ���ò���. �������ж��ƵĲ�����������.
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
*   �ú��������ж�����.
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
*   �ú������ڻ�ȡ�������.
* Parameter:
*   ptRet: (O) �����������.
* Return:
*   PV_SUCCESS: success
*   PV_FAILURE: û�п��õ��������
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
*   �ú��������ͷ��������.
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
*   �ú�������ȥ��ʼ������ģ��, �ͷ�����ģ��ռ�õ���Դ.
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

