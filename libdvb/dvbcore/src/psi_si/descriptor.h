/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc.
*
* Module name : PSI/SI
* File name   : descriptor.h
* Description : This file include prototype of descriptor parse functions.
*
* History :
*   2012-05-02 ZYJ
*       Initial Version.
*******************************************************************************/
#ifndef __DESCRIPTORS_H_
#define __DESCRIPTORS_H_

#include "psi_si.h"

#ifdef __cplusplus
extern "C" {
#endif

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);


S32 _parse_LW_Logical_Channel( U8 *pu8Data, void *ptDesc, void *pData );
#if 0
S32 parse_video_stream_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_audio_stream_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);
    
S32 parse_hierarchy_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_registration_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_data_stream_alignment_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_target_background_grid_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_video_window_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_system_clock_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_multiplex_buffer_utilisation_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_copyright_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_maximal_bitrate_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_private_data_indicator_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_network_name_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_service_list_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_stuffing_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_vbi_data_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_vbi_teletext_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_bouquet_name_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);
    
S32 parse_country_availability_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_short_event_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_extended_event_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_component_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_mosaic_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_stream_identifier_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_ca_identifier_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_content_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_parental_rating_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);
    
S32 parse_teletext_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_telephone_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_local_time_offset_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_subtitling_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_multilingual_network_name_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_multilingual_bouquet_name_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_multilingual_service_name_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_multilingual_component_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_private_data_specifier_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_service_move_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_short_smoothing_buffer_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_frequency_list_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_partial_transport_stream_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_data_broadcast_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_ca_system_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_data_broadcast_id_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_transport_stream_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_dsng_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_pdc_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_ac3_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_ancillary_data_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_cell_list_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_cell_frequency_link_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);

S32 parse_announcement_support_descriptor
(
    U8 *    pu8Data,
    void *  ptDesc
);
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
);


#ifdef __cplusplus
}
#endif

#endif /* __DESCRIPTORS_H_ */

