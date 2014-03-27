/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc.
*
* Module name : PSI/SI
* File name   : descriptor.c
* Description : This file include implementation of descriptor parse functions.
*
* History :
*   2012-05-02 ZYJ
*       Initial Version.
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "dvbcore.h"
#include "psi_si.h"
#include "descriptor.h"

#include "pbitrace.h"
#ifdef __cplusplus
extern "C" {
#endif

static S32 parse_text_info
(
    U8 *                pu8Data,
    U8                  u8DataLen,
    void *              pTextInfo,
    void *              pString,
    U8                  u8Flag
);

/******************************************************************************
* Function:
*   parse_ca_descriptor
* Description:
*   该函数用于解析 ca_descriptor.
* Parameter:
*   pu8Data:      (I) 待解析数据.
*   ptDesc:       (O) 存放解析结果.
*   u8Flag:       (I) 0 - 解析; 1 - 不解析, 统计 private data 长度
*   pPrivateData: (O) 统计长度时, 返回统计结果; 解析时, 存放 private data 数据.
* Return:
*   PBI_SUCCESS:       success
*   PBI_NULL_PTR:      传入参数有空指针
*   PBI_INVALID_PARAM: 待解析数据不是对应描述符的数据
* History :
*   2012-05-02 ZYJ
*       Initial.
*
*******************************************************************************/
S32 parse_ca_descriptor
(
    U8 *        pu8Data,
    CA_Desc_t * ptDesc,
    void *      pPrivateData,
    U8          u8Flag
)
{
    U8  u8DescLen;

    if (NULL == pu8Data)
    {
        return PBI_NULL_PTR;
    }

    if (pu8Data[0] != CA_DESCRIPTOR_TAG)
    {
        return PBI_INVALID_PARAM;
    }

    u8DescLen = pu8Data[1];

    if (u8Flag != 0)    /* 统计长度信息 */
    {
        U16 *   pu16PrivateDataLen = (U16 *)pPrivateData;

        if (NULL == pPrivateData)
        {
            return PBI_NULL_PTR;
        }

        *pu16PrivateDataLen += (u8DescLen - 4);
    }
    else    /* 解析 */
    {
        if (NULL == ptDesc)
        {
            return PBI_NULL_PTR;
        }

        ptDesc->u16CASystemId = (pu8Data[2] << 8) + pu8Data[3];
        ptDesc->u16CAPid = (pu8Data[4] << 8) + pu8Data[5];
        ptDesc->u8PrivateDataLen = u8DescLen - 4;
        if (ptDesc->u8PrivateDataLen != 0)
        {
            if (NULL == pPrivateData)
            {
                return PBI_NULL_PTR;
            }
            memcpy(pPrivateData, pu8Data+6, ptDesc->u8PrivateDataLen);
            ptDesc->pu8PrivateData = (U8 *)pPrivateData;
        }
        else
        {
            ptDesc->pu8PrivateData = NULL;
        }
    }

    return PBI_SUCCESS;
}

/******************************************************************************
* Function:
*   parse_delivery_system_cable_descriptor
* Description:
*   该函数用于解析 cable_delivery_system_descriptor.
* Parameter:
*   pu8Data:      (I) 待解析数据.
*   ptDesc:       (O) 存放解析结果.
* Return:
*   PBI_SUCCESS:       success
*   PBI_NULL_PTR:      传入参数有空指针
*   PBI_INVALID_PARAM: 待解析数据不是对应描述符的数据
* History :
*   2012-05-02 ZYJ
*       Initial.
*
*******************************************************************************/
S32 parse_delivery_system_cable_descriptor
(
    U8 *                            pu8Data,
    Delivery_System_Desc_Cab_t *    ptDesc
)
{

    if ((NULL == pu8Data) || (NULL == ptDesc))
    {
        return PBI_NULL_PTR;
    }

    if ( (pu8Data[0] != CABLE_DELIVERY_SYSTEM_DESCRIPTOR_TAG)
        || (pu8Data[1] != 11) )
    {
        return PBI_INVALID_PARAM;
    }

    ptDesc->u32Freq = ((pu8Data[2] >> 4)  * 10000000)
                    + ((pu8Data[2] & 0xF) * 1000000)
                    + ((pu8Data[3] >> 4)  * 100000)
                    + ((pu8Data[3] & 0xF) * 10000)
                    + ((pu8Data[4] >> 4)  * 1000)
                    + ((pu8Data[4] & 0xF) * 100)
                    + ((pu8Data[5] >> 4)  * 10)
                    + ( pu8Data[5] & 0xF);
    ptDesc->u32Freq /= 10;

    ptDesc->u8FEC_Outer = (pu8Data[7] & 0xF);
    ptDesc->u8Mod = pu8Data[8];

    ptDesc->u32SymRate = ((pu8Data[9] >> 4)   * 1000000)
                       + ((pu8Data[9] & 0xF)  * 100000)
                       + ((pu8Data[10] >> 4)  * 10000)
                       + ((pu8Data[10] & 0xF) * 1000)
                       + ((pu8Data[11] >> 4)  * 100)
                       + ((pu8Data[11] & 0xF) * 10)
                       + ( pu8Data[12] >> 4);
    ptDesc->u32SymRate /= 10;

    ptDesc->u8FEC_Inner = (pu8Data[12] & 0xF);

    return PBI_SUCCESS;
}

/******************************************************************************
* Function:
*   parse_delivery_system_satellite_descriptor
* Description:
*   该函数用于解析 satellite_delivery_system_descriptor.
* Parameter:
*   pu8Data:      (I) 待解析数据.
*   ptDesc:       (O) 存放解析结果.
* Return:
*   PBI_SUCCESS:       success
*   PBI_NULL_PTR:      传入参数有空指针
*   PBI_INVALID_PARAM: 待解析数据不是对应描述符的数据
* History :
*   2012-05-02 ZYJ
*       Initial.
*
*******************************************************************************/
S32 parse_delivery_system_satellite_descriptor
(
    U8 *                            pu8Data,
    Delivery_System_Desc_Sat_t *    ptDesc
)
{
    if ((NULL == pu8Data) || (NULL == ptDesc))
    {
        return PBI_NULL_PTR;
    }

    if ( (pu8Data[0] != SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_TAG)
        || ( pu8Data[1] != 11) )
    {
        return PBI_INVALID_PARAM;
    }

    ptDesc->u32Freq = ((pu8Data[2] >> 4)  * 10000000)
                    + ((pu8Data[2] & 0xF) * 1000000)
                    + ((pu8Data[3] >> 4)  * 100000)
                    + ((pu8Data[3] & 0xF) * 10000)
                    + ((pu8Data[4] >> 4)  * 1000)
                    + ((pu8Data[4] & 0xF) * 100)
                    + ((pu8Data[5] >> 4)  * 10)
                    +  (pu8Data[5] & 0xF);

    ptDesc->u16OrbitalPosition = ((pu8Data[6] >> 4)  * 1000)
                               + ((pu8Data[6] & 0xF) * 100)
                               + ((pu8Data[7] >> 4)  * 10)
                               +  (pu8Data[7] & 0xF);

    ptDesc->u8WestEastFlag = ((pu8Data[8] >> 7) & 0x1);

    ptDesc->u8Polarization = ((pu8Data[8] >> 5) & 0x3);

    ptDesc->u8Mod = (pu8Data[8] & 0x1F);

    ptDesc->u32SymRate = ((pu8Data[9] >> 4)   * 1000000)
                       + ((pu8Data[9] & 0xF)  * 100000)
                       + ((pu8Data[10] >> 4)  * 10000)
                       + ((pu8Data[10] & 0xF) * 1000)
                       + ((pu8Data[11] >> 4)  * 100)
                       + ((pu8Data[11] & 0xF) * 10)
                       + ( pu8Data[12] >> 4);

    ptDesc->u8FEC_Inner = (pu8Data[12] & 0xF);

    return PBI_SUCCESS;
}

/******************************************************************************
* Function:
*   parse_delivery_system_terrestrial_descriptor
* Description:
*   该函数用于解析 terrestrial_delivery_system_descriptor.
* Parameter:
*   pu8Data:      (I) 待解析数据.
*   ptDesc:       (O) 存放解析结果.
* Return:
*   PBI_SUCCESS:       success
*   PBI_NULL_PTR:      传入参数有空指针
*   PBI_INVALID_PARAM: 待解析数据不是对应描述符的数据
* History :
*   2012-05-02 ZYJ
*       Initial.
*
*******************************************************************************/
S32 parse_delivery_system_terrestrial_descriptor
(
    U8 *                            pu8Data,
    Delivery_System_Desc_Ter_t *    ptDesc
)
{
    if ((NULL == pu8Data) || (NULL == ptDesc))
    {
        return PBI_NULL_PTR;
    }

    if ( (pu8Data[0] != TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR_TAG)
        || (pu8Data[1] != 11) )
    {
        return PBI_INVALID_PARAM;
    }

    ptDesc->u32CentreFreq = ( (pu8Data[2] << 24)
                            + (pu8Data[3] << 16)
                            + (pu8Data[4] << 8)
                            +  pu8Data[5] );

    ptDesc->u8BandWidth         = ((pu8Data[6] >> 5) & 0x7);
    ptDesc->u8Constellation     = ((pu8Data[7] >> 6) & 0x3);
    ptDesc->u8HierarchyInfo     = ((pu8Data[7] >> 3) & 0x7);
    ptDesc->u8CodeRateHP_Stream =  (pu8Data[7] & 0x7);
    ptDesc->u8CodeRateLP_Stream = ((pu8Data[8] >> 5) & 0x7);
    ptDesc->u8GuardInterval     = ((pu8Data[8] >> 3) & 0x3);
    ptDesc->u8TransmissionMode  = ((pu8Data[8] >> 1) & 0x3);
    ptDesc->u8OtherFreqFlag     =  (pu8Data[8] & 0x1);

    return PBI_SUCCESS;
}

/******************************************************************************
* Function:
*   parse_iso_639_language_descriptor
* Description:
*   该函数用于解析 iso_639_language_descriptor.
* Parameter:
*   pu8Data:   (I) 待解析数据.
*   ptDesc:    (O) 存放解析结果.
*   u8Flag:    (I) 0 - 解析; 1 - 不解析, 统计 Language_Info_t 个数
*   pLangInfo: (O) 统计个数时, 返回统计结果; 解析时, 存放 Language_Info_t 数据.
* Return:
*   PBI_SUCCESS:       success
*   PBI_NULL_PTR:      传入参数有空指针
*   PBI_INVALID_PARAM: 待解析数据不是对应描述符的数据
* History :
*   2012-05-02 ZYJ
*       Initial.
*
*******************************************************************************/
S32 parse_iso_639_language_descriptor
(
    U8 *                        pu8Data,
    ISO_639_Language_Desc_t *   ptDesc,
    void *                      pLangInfo,
    U8                          u8Flag
)
{
    U8  u8DescLen;

    if (NULL == pu8Data)
    {
        return PBI_NULL_PTR;
    }

    if (pu8Data[0] != ISO_639_LANGUAGE_DESCRIPTOR_TAG)
    {
        return PBI_INVALID_PARAM;
    }

    u8DescLen = pu8Data[1];

    if (u8Flag != 0)    /* 统计长度信息 */
    {
        U16 *   pu16LangNum = (U16 *)pLangInfo;

        if (NULL == pLangInfo)
        {
            return PBI_NULL_PTR;
        }

        *pu16LangNum += (u8DescLen >> 2);
    }
    else    /* 解析 */
    {
        Language_Info_t * ptLang = (Language_Info_t *)pLangInfo;

        if (NULL == ptDesc)
        {
            return PBI_NULL_PTR;
        }

        ptDesc->u16LangNum = 0;
        ptDesc->ptLangInfo = NULL;

        if ((u8DescLen != 0) && (NULL == pLangInfo))
        {
            return PBI_NULL_PTR;
        }

        pu8Data += 2;
        while (u8DescLen > 0)
        {
            ptLang->u32LangCode = ((pu8Data[0] << 16)
                                + (pu8Data[1] << 8)
                                + (pu8Data[2]));
            ptLang->u8AudioType = pu8Data[3];
            ptLang++;
            ptDesc->u16LangNum++;
            pu8Data += 4;
            u8DescLen -= 4;
        }

        ptDesc->ptLangInfo = (Language_Info_t *)pLangInfo;
    }

    return PBI_SUCCESS;
}

/******************************************************************************
* Function:
*   parse_linkage_descriptor
* Description:
*   该函数用于解析 linkage_descriptor.
* Parameter:
*   pu8Data:      (I) 待解析数据.
*   ptDesc:       (O) 存放解析结果.
*   u8Flag:       (I) 0 - 解析; 1 - 不解析, 统计 private data 长度
*   pPrivateData: (O) 统计长度时, 返回统计结果; 解析时, 存放 private data 数据.
* Return:
*   PBI_SUCCESS:       success
*   PBI_NULL_PTR:      传入参数有空指针
*   PBI_INVALID_PARAM: 待解析数据不是对应描述符的数据
* History :
*   2012-05-02 ZYJ
*       Initial.
*
*******************************************************************************/
S32 parse_linkage_descriptor
(
    U8 *                pu8Data,
    Linkage_Desc_t *    ptDesc,
    void *              pPrivateData,
    U8                  u8Flag
)
{
    U8  u8DescLen;

    if (NULL == pu8Data)
    {
        return PBI_NULL_PTR;
    }

    if (pu8Data[0] != LINKAGE_DESCRIPTOR_TAG) 
    {
        return PBI_INVALID_PARAM;
    }

    u8DescLen = pu8Data[1];

    if (u8Flag != 0)    /* 统计长度信息 */
    {
        U16 *   pu16PrivateDataLen = (U16 *)pPrivateData;

        if (NULL == pPrivateData)
        {
            return PBI_NULL_PTR;
        }

        *pu16PrivateDataLen += (u8DescLen - 7);
    }
    else
    {
        if (NULL == ptDesc)
        {
            return PBI_NULL_PTR;
        }

        ptDesc->u16TS_Id = ((pu8Data[2] << 8) | pu8Data[3]);
        ptDesc->u16ON_Id = ((pu8Data[4] << 8) | pu8Data[5]);
        ptDesc->u16ServId = ((pu8Data[6] << 8) | pu8Data[7]);
        ptDesc->u8LinkageType = pu8Data[8];
        ptDesc->u8PrivateDataLen = u8DescLen - 7;

        if (ptDesc->u8PrivateDataLen != 0)
        {
            if (NULL == pPrivateData)
            {
                return PBI_NULL_PTR;
            }
            memcpy(pPrivateData, pu8Data+9, ptDesc->u8PrivateDataLen);
            ptDesc->pu8PrivateData = (U8 *)pPrivateData;
        }
        else
        {
            ptDesc->pu8PrivateData = NULL;
        }
    }

    return PBI_SUCCESS;
}

/******************************************************************************
* Function:
*   parse_nvod_reference_descriptor
* Description:
*   该函数用于解析 nvod_reference_descriptor.
* Parameter:
*   pu8Data:      (I) 待解析数据.
*   ptDesc:       (O) 存放解析结果.
*   u8Flag:       (I) 0 - 解析; 1 - 不解析, 统计 NVOD_Ref_Info_t 个数
*   pRefInfo:     (O) 统计个数时, 返回统计结果; 解析时, 存放 NVOD_Ref_Info_t
*                     数据.
* Return:
*   PBI_SUCCESS:       success
*   PBI_NULL_PTR:      传入参数有空指针
*   PBI_INVALID_PARAM: 待解析数据不是对应描述符的数据
* History :
*   2012-05-02 ZYJ
*       Initial.
*
*******************************************************************************/
S32 parse_nvod_reference_descriptor
(
    U8 *                    pu8Data,
    NVOD_Reference_Desc_t * ptDesc,
    void *                  pRefInfo,
    U8                      u8Flag
)
{
    U8  u8DescLen;

    if (NULL == pu8Data)
    {
        return PBI_NULL_PTR;
    }

    if ( (pu8Data[0] != NVOD_REFERENCE_DESCRIPTOR_TAG)
        || (pu8Data[1] % 6 != 0) )
    {
        return PBI_INVALID_PARAM;
    }

    u8DescLen = pu8Data[1];

    if (u8Flag != 0)    /* 统计长度信息 */
    {
        U16 * pu16RefServNum = (U16 *)pRefInfo;

        if (NULL == pRefInfo)
        {
            return PBI_NULL_PTR;
        }

        *pu16RefServNum += (u8DescLen / 6);
    }
    else
    {
        NVOD_Ref_Info_t *   ptRefServInfo = (NVOD_Ref_Info_t *)pRefInfo;

        if( NULL == ptDesc )
        {
            return PBI_NULL_PTR;
        }

        ptDesc->u16RefInfoNum = 0;
        ptDesc->ptRefInfo = NULL;

        if ((u8DescLen > 0) && (pRefInfo == NULL))
        {
            return PBI_NULL_PTR;
        }

        pu8Data += 2;
        while (u8DescLen > 0)
        {
            ptRefServInfo->u16TS_Id = ((pu8Data[0] << 8) + pu8Data[1]);
            ptRefServInfo->u16ON_Id = ((pu8Data[1] << 8) + pu8Data[2]);
            ptRefServInfo->u16ServId = ((pu8Data[3] << 8) + pu8Data[4]);
            ptRefServInfo++;
            ptDesc->u16RefInfoNum++;
            pu8Data += 6;
            u8DescLen -= 6;
        }

        ptDesc->ptRefInfo = (NVOD_Ref_Info_t *)pRefInfo;
    }

    return PBI_SUCCESS;
}

/******************************************************************************
* Function:
*   parse_service_descriptor
* Description:
*   该函数用于解析 service_descriptor.
* Parameter:
*   pu8Data:      (I) 待解析数据.
*   ptDesc:       (O) 存放解析结果.
*   u8Flag:       (I) 0 - 解析; 1 - 不解析, 统计所有文本信息
*   pTextInfo:    (O) 统计信息时, 返回统计结果; 解析时, 存放 Text_Info_t 数据.
*   pString:      (O) 统计信息时, 返回统计结果; 解析时, 存放字符串信息.
* Return:
*   PBI_SUCCESS:       success
*   PBI_NULL_PTR:      传入参数有空指针
*   PBI_INVALID_PARAM: 待解析数据不是对应描述符的数据
* History :
*   2012-05-02 ZYJ
*       Initial.
*
*******************************************************************************/
S32 parse_service_descriptor
(
    U8 *                pu8Data,
    Service_Desc_t *    ptDesc,
    void *              pTextInfo,
    void *              pString,
    U8                  u8Flag
)
{
    U8  u8ProviderNameLen;
    U8  u8ServNameLen;
    S32 s32Error = PBI_SUCCESS;

    if (NULL == pu8Data)
    {
        return PBI_NULL_PTR;
    }

    if (pu8Data[0] != SERVICE_DESCRIPTOR_TAG)
    {
        return PBI_INVALID_PARAM;
    }

    u8ProviderNameLen = pu8Data[3];
    u8ServNameLen = pu8Data[u8ProviderNameLen+4];

    if (u8Flag != 0)    /* 统计长度信息 */
    {
        pu8Data += 4;
        s32Error = parse_text_info(pu8Data, u8ProviderNameLen, pTextInfo, pString, u8Flag);
        if (s32Error < 0)
        {
            return s32Error;
        }

        pu8Data += u8ProviderNameLen;
        pu8Data += 1;   /* skip service name length field. */
        s32Error = parse_text_info(pu8Data, u8ServNameLen, pTextInfo, pString, u8Flag);
        if (s32Error < 0)
        {
            return s32Error;
        }
    }
    else    /* 解析 */
    {
        Text_Info_t *   ptText = (Text_Info_t *)pTextInfo;
        U8 *            pu8String = (U8 *)pString;

        if (NULL == ptDesc)
        {
            return PBI_NULL_PTR;
        }

        ptDesc->u8ServType = pu8Data[2];
        ptDesc->ptServProviderName = NULL;
        ptDesc->ptServName = NULL;

        pu8Data += 4;

        s32Error = parse_text_info(pu8Data, u8ProviderNameLen, ptText, pu8String, u8Flag);
        if (s32Error < 0)
        {
            return s32Error;
        }

        if (s32Error > 0)
        {
            ptDesc->ptServProviderName = ptText;
            pu8String += ptText->u8Len;
            ptText++;
        }
        
        pu8Data += u8ProviderNameLen;
        pu8Data += 1;   /* skip service name length field. */
        s32Error = parse_text_info(pu8Data, u8ServNameLen, ptText, pu8String, u8Flag);
        if (s32Error < 0)
        {
            return s32Error;
        }

        if (s32Error > 0)
        {
            ptDesc->ptServName = ptText;
        }
    }

    return PBI_SUCCESS;
}

/******************************************************************************
* Function:
*   parse_time_shifted_event_descriptor
* Description:
*   该函数用于解析 time_shifted_event_descriptor.
* Parameter:
*   pu8Data:      (I) 待解析数据.
*   ptDesc:       (O) 存放解析结果.
* Return:
*   PBI_SUCCESS:       success
*   PBI_NULL_PTR:      传入参数有空指针
*   PBI_INVALID_PARAM: 待解析数据不是对应描述符的数据
* History :
*   2012-05-02 ZYJ
*       Initial.
*
*******************************************************************************/
S32 parse_time_shifted_event_descriptor
(
    U8 *                            pu8Data,
    NVOD_TimeShift_Event_Desc_t *   ptDesc
)
{
    if ((NULL == pu8Data) || (NULL == ptDesc))
    {
        return PBI_NULL_PTR;
    }

    if ( (pu8Data[0] != TIME_SHIFTED_EVENT_DESCRIPTOR_TAG)
        || (pu8Data[1] != 4) )
    {
        return PBI_INVALID_PARAM;
    }

    ptDesc->u16RefServId = ((pu8Data[2] << 8) + pu8Data[3]);
    ptDesc->u16RefEventId = ((pu8Data[4] << 8) + pu8Data[5]);

    return PBI_SUCCESS;
}

/******************************************************************************
* Function:
*   parse_time_shifted_service_descriptor
* Description:
*   该函数用于解析 time_shifted_service_descriptor.
* Parameter:
*   pu8Data:      (I) 待解析数据.
*   ptDesc:       (O) 存放解析结果.
* Return:
*   PBI_SUCCESS:       success
*   PBI_NULL_PTR:      传入参数有空指针
*   PBI_INVALID_PARAM: 待解析数据不是对应描述符的数据
* History :
*   2012-05-02 ZYJ
*       Initial.
*
*******************************************************************************/
S32 parse_time_shifted_service_descriptor
(
    U8 *                            pu8Data,
    NVOD_TimeShift_Serv_Desc_t *    ptDesc
)
{
    if ((NULL == pu8Data) || (NULL == ptDesc))
    {
        return PBI_NULL_PTR;
    }

    if ( (pu8Data[0] != TIME_SHIFTED_SERVICE_DESCRIPTOR_TAG)
        || (pu8Data[1] != 2) )
    {
        return PBI_INVALID_PARAM;
    }

    ptDesc->u16RefServId = ((pu8Data[2] << 8) + pu8Data[3]);

    return PBI_SUCCESS;
}

#if 0
S32 parse_video_stream_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_audio_stream_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}
    
S32 parse_hierarchy_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}


S32 parse_registration_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_data_stream_alignment_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_target_background_grid_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_video_window_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_system_clock_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_multiplex_buffer_utilisation_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_copyright_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_maximal_bitrate_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_private_data_indicator_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_network_name_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}
#endif
S32 parse_service_list_descriptor( U8 * pu8Data, void *  ptDesc, void *pData )
{
    U16 *pSize = NULL;
    Service_List_Des_t *pServListDes = NULL;
    
    if( NULL == pu8Data || NULL == pData )
    {
        pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
        return PBI_NULL_PTR;
    }

    if( NULL == ptDesc )
    {
        if( NULL == pData )
        {
            return PBI_NULL_PTR;
        }

        pSize = (U16 *) pData;

        *pSize = pu8Data[1] / 3; /* sizeof(Service_Info_t)  ==> 4 but  Service_Info_t size is 3 */
        
    }
    else
    {
        if( NULL == ptDesc )
        {
            pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
            return PBI_NULL_PTR;
        }
        
        pServListDes = (Service_List_Des_t *)ptDesc;

        memcpy( pData, &pu8Data[2], pu8Data[1] );

        pServListDes->pServInfo = (Service_Info_t *)pData;
        pServListDes->u16ListNum = pu8Data[1] / 3; /* sizeof(Service_Info_t)  ==> 4 but  Service_Info_t size is 3 */
        
    }

    
    return PBI_SUCCESS;
}
#if 0
S32 parse_stuffing_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_vbi_data_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_vbi_teletext_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

#endif
S32 parse_bouquet_name_descriptor( U8 *    pu8Data, void *  ptDesc, void *pData )
{
    Bouquet_Name_t *ptBouquetName = NULL;
    U16 *pMemLen = NULL;
    C8 *pPrivateData = NULL;
    
    if( NULL == pu8Data )
    {
        pbierror("[%s %d]-------------\n",__FUNCTION__,__LINE__);
        return PBI_NULL_PTR;
    }

    if( BOUQUET_NAME_DESCRIPTOR_TAG != pu8Data[0] )
    {
        pbierror("[%s %d]-------------\n",__FUNCTION__,__LINE__);
        return PBI_INVALID_PARAM;
    }

    if( NULL == ptDesc )
    {
        pMemLen =  (U16 *)pData;
        *pMemLen += pu8Data[1] +1;
    }
    else
    {
        if( NULL == ptDesc || NULL == pData )
        {
            pbierror("[%s %d]-------------\n",__FUNCTION__,__LINE__);
            return PBI_INVALID_PARAM;
        }
        
        pPrivateData = (C8 *)pData;
    
        ptBouquetName = (Bouquet_Name_t*)ptDesc;
        ptBouquetName->u8BuquetNameLen = pu8Data[1] + 1;
        if( ptBouquetName->u8BuquetNameLen > 0 )
        {
            memcpy( pPrivateData, &pu8Data[2], pu8Data[1] );
            ptBouquetName->pBuquetName = pPrivateData;
            ptBouquetName->pBuquetName[pu8Data[1]] = '\0';
        }
        else
        {
            ptBouquetName->pBuquetName = NULL;
        }
    }

    return PBI_SUCCESS;
}
#if 0
S32 parse_logical_channel_descriptor(U8 *pu8Data, void *ptDesc, void *pData )
{
    U16 *pLogicalCount;
    BAT_Component_t *pBatCom;
    Bouquet_Logical_Channel_t *pLogicalChannel;
    

    if( NULL == pu8Data )
    {
        pbierror("[%s %d]-------------\n",__FUNCTION__,__LINE__);
        return PBI_NULL_PTR;
    }

    if( LOGICAL_CHANNEL_DESCRIPTOR_TAG != pu8Data[0] )
    {
        pbierror("[%s %d]-------------\n",__FUNCTION__,__LINE__);
        return PBI_INVALID_PARAM;
    }

    if( NULL == ptDesc )
    {
        pLogicalCount =  (U16*)pData;
        *pLogicalCount = pu8Data[1]/4;
    }
    else
    {
        if( NULL == ptDesc || NULL == pData )
        {
            pbierror("[%s %d]-------------\n",__FUNCTION__,__LINE__);
            return PBI_INVALID_PARAM;
        }

        pBatCom = (BAT_Component_t *)ptDesc;
        pLogicalChannel = (Bouquet_Logical_Channel_t *)pData;

        memcpy( pLogicalChannel, &pu8Data[2], pu8Data[1] );

        pBatCom->pLogicalChannel = pLogicalChannel;
        
    }
    


    return PBI_SUCCESS;
}


S32 parse_country_availability_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_short_event_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_extended_event_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_component_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_mosaic_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_stream_identifier_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_ca_identifier_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_content_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_parental_rating_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}
    
S32 parse_teletext_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_telephone_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_local_time_offset_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_subtitling_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_multilingual_network_name_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_multilingual_bouquet_name_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_multilingual_service_name_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_multilingual_component_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_private_data_specifier_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_service_move_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_short_smoothing_buffer_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_frequency_list_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_partial_transport_stream_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_data_broadcast_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_ca_system_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_data_broadcast_id_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_transport_stream_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_dsng_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_pdc_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_ac3_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_ancillary_data_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_cell_list_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_cell_frequency_link_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}

S32 parse_announcement_support_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
)
{
    return PBI_SUCCESS;
}
#endif


/******************************************************************************
* Function:
*   parse_unknown_descriptor
* Description:
*   该函数用于解析未知描述符.
* Parameter:
*   pu8Data: (I) 待解析数据.
*   ptDesc:  (O) 存放解析结果.
*   u8Flag:  (I) 0 - 解析; 1 - 不解析, 统计 data 长度
*   pData:   (O) 统计长度时, 返回统计结果; 解析时, 存放数据.
* Return:
*   PBI_SUCCESS:       success
*   PBI_NULL_PTR:      传入参数有空指针
* History :
*   2012-05-02 ZYJ
*       Initial.
*
*******************************************************************************/
S32 parse_unknown_descriptor
(
    U8 *                    pu8Data,
    PSISI_Descriptor_t *    ptDesc,
    void *                  pData,
    U8                      u8Flag
)
{
    if (NULL == pu8Data)
    {
        pbierror("[%s %d]-------------\n",__FUNCTION__,__LINE__);
        return PBI_NULL_PTR;
    }

    if (u8Flag != 0)    /* 统计长度信息 */
    {
        U16 *   pu16DataLen = (U16 *)pData;

        if (NULL == pData)
        {
            pbierror("[%s %d]-------------\n",__FUNCTION__,__LINE__);
            return PBI_NULL_PTR;
        }

        *pu16DataLen += pu8Data[1];
    }
    else    /* 解析 */
    {
        if (NULL == ptDesc)
        {
            pbierror("[%s %d]-------------\n",__FUNCTION__,__LINE__);
            return PBI_NULL_PTR;
        }

        ptDesc->u8Tag = pu8Data[0];
        ptDesc->u8Len = pu8Data[1];

        if (ptDesc->u8Len != 0)
        {
            if (NULL == pData)
            {
                pbierror("[%s %d]-------------\n",__FUNCTION__,__LINE__);
                return PBI_NULL_PTR;
            }
            memcpy(pData, pu8Data+2, ptDesc->u8Len);
            ptDesc->pu8Data = (U8 *)pData;
        }
        else
        {
            ptDesc->pu8Data = NULL;
        }
    }

    return PBI_SUCCESS;
}


/******************************************************************************
* Function:
*   parse_text_info
* Description:
*   该函数用于解析文本信息.
* Parameter:
*   pu8Data:      (I) 待解析数据
*   u8DataLen:    (I) 待解析文本数据长度
*   u8Flag:       (I) 0 - 解析; 1 - 不解析, 统计所有文本信息
*   pTextInfo:    (O) 统计信息时, 返回统计结果; 解析时, 存放 Text_Info_t 数据.
*   pString:      (O) 统计信息时, 返回统计结果; 解析时, 存放字符串信息.
* Return:
*   1   :      成功
*   0   :      无文本, 或文本长度为 0
*   < 0 :      failed
* History :
*   2012-06-27 ZYJ
*       Initial.
*
*******************************************************************************/
static S32 parse_text_info
(
    U8 *                pu8Data,
    U8                  u8DataLen,
    void *              pTextInfo,
    void *              pString,
    U8                  u8Flag
)
{
    U8  u8StringLen;
    S32 s32Ret = 0;

    if (NULL == pu8Data)
    {
        return PBI_NULL_PTR;
    }

    if (u8Flag != 0)    /* 统计长度信息 */
    {
        U16 *   pu16TextNum = (U16 *)pTextInfo;
        U16 *   pu16StrLen = (U16 *)pString;

        if ((NULL == pTextInfo) || (NULL == pString))
        {
            return PBI_NULL_PTR;
        }

        if ((u8DataLen > 0) && (pu8Data[0] > 0))
        {
            if (pu8Data[0] < 0x20)
            {
                u8StringLen = u8DataLen - 1;
                if ((u8StringLen > 0) /*&& (pu8Data[1] > 0)*/)
                {
                    *pu16TextNum += 1;
                    *pu16StrLen += (u8StringLen + 1);
                    s32Ret = 1;
                }
            }
            else
            {
                *pu16TextNum += 1;
                *pu16StrLen += (u8DataLen + 1);
                s32Ret = 1;
            }
        }
    }
    else    /* 解析 */
    {
        Text_Info_t *   ptText = (Text_Info_t *)pTextInfo;

        if ((u8DataLen > 0) && (pu8Data[0] > 0))
        {
            if (pu8Data[0] < 0x20)
            {
                u8StringLen = u8DataLen - 1;
                if ((u8StringLen > 0) /*&& (pu8Data[1] > 0)*/)
                {
                    if ((NULL == pTextInfo) || (NULL == pString))
                    {
                        return PBI_NULL_PTR;
                    }
        
                    ptText->u32CharSet = pu8Data[0];
                    ptText->u8Len = u8StringLen + 1;
                    ptText->sText = (C8 *)pString;
                    memcpy(pString, pu8Data+1, u8StringLen);
                    ptText->sText[u8StringLen] = 0;
                    s32Ret = 1;
                }

            }
            else
            {
                if ((NULL == pTextInfo) || (NULL == pString))
                {
                    pbierror("[%s %d]---null-------\n",__FUNCTION__,__LINE__);
                    return PBI_NULL_PTR;
                }
                
                ptText->u32CharSet = CHARACTER_SET_ISOIEC_6937;
                ptText->u8Len = u8DataLen + 1;
                ptText->sText = (C8 *)pString;
                memcpy(pString, pu8Data, u8DataLen);
                ptText->sText[u8DataLen] = 0;
           //     pbiinfo("[%s %d]==Name %s======\n",__FUNCTION__,__LINE__,ptText->sText);
                s32Ret = 1;
				
            }
        }
        else
        {

        }
    }

    return s32Ret;
}


/*
    联维自定义TAG 0x83 
*/
S32 _parse_LW_Logical_Channel( U8 *pu8Data, void *ptDesc, void *pData )
{
    U16 *pSize = NULL;
    LW_Logical_List_t *pLwLogicaList = NULL;
    LW_Logical_Num_t *pLwLogical = NULL;
    U8 *pTemp = NULL;
    int ii = 0;
    int iCount = 0;
    
    if(  NULL == pu8Data || NULL == pData )
    {
        return PV_NULL_PTR;
    }

    if( 0x83 != pu8Data[0] )
    {
        return PV_INVALID_PARAM;
    }

    if( NULL == ptDesc )
    {
      //  pbiinfo("zxguan[%s %d]----len %d---------\n",__FUNCTION__,__LINE__,pu8Data[1]);
        pSize  = (U16 *)pData;
        if( 0 != pu8Data[1] )
        {
            *pSize += (pu8Data[1] /4);
        }
        else
        {
            *pSize = 0;
        }
      //  pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
    }
    else
    {   
       // pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
        iCount = 0;
        pLwLogicaList = (LW_Logical_List_t *)ptDesc;
        pLwLogical = (LW_Logical_Num_t *)pData;
        pTemp = &pu8Data[2];
        
        //pbiinfo("zxguan[%s %d]------for len = %d-------\n",__FUNCTION__,__LINE__,pu8Data[1]);
        for( ii = 0; ii < pu8Data[1]; ii += 4)
        {
           // pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
            pLwLogical->u16ServiceId = (pTemp[0 + ii] << 8 ) | (pTemp[1 + ii] );
          //  pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
            pLwLogical->u8VisibleServicesFlag = (pTemp[2 + ii] >> 7);
            pLwLogical->u16LogicalNum = ( (pTemp[2 + ii] & 0x3F) << 8) | (pTemp[3 + ii]);
#if 0
            pbiinfo("[%s %d]   addr 0x%x....\n",DEBUG_LOG, &pLwLogical->u16ServiceId );
            pbiinfo("servicId 0x%x .....logivalNum  0x%x.......visibleflg %d.....\n", \
                pLwLogical->u16ServiceId, pLwLogical->u16LogicalNum, pLwLogical->u8VisibleServicesFlag);
#endif
            pLwLogical++;
            iCount++;
        }
      //  pbiinfo("zxguan[%s %d]-------------\n",__FUNCTION__,__LINE__);
        pLwLogicaList->pLogicalInfo = (LW_Logical_Num_t *)pData;
        pLwLogicaList->u16ListNum = iCount;    
    }
    
    return PV_SUCCESS;
}




#ifdef __cplusplus
}
#endif

