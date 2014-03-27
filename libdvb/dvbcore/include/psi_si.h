/*******************************************************************************
* Copyright (c) 2012 Pro. Broadband Inc. PVware
*
* Module name : PSI/SI
* File name   : psi_si.h
* Description : This file include Definitions of PSI/SI tables, sections,
*               descriptors, etc. And API prototypes.
*
* History :
*   2012-05-28 ZYJ
*       Initial Version.
*******************************************************************************/
#ifndef __PSI_SI_H_
#define __PSI_SI_H_
    
#include "pvddefs.h"
    
#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
*                              constants of PID
*******************************************************************************/
#define PAT_PID             (0x0000)
#define CAT_PID             (0x0001)

#define NIT_PID             (0x0010)
#define BAT_PID             (0x0011)
#define SDT_PID             (0x0011)
#define EIT_PID             (0x0012)
#define TDT_PID             (0x0014)
#define TOT_PID             (0x0014)

#define MAX_PID             (0x1FFF)
#define INVALID_PID         (0xFFFF)


/*******************************************************************************
*                            constants of table_id
*******************************************************************************/
#define PAT_TABLE_ID        (0x00)
#define CAT_TABLE_ID        (0x01)
#define PMT_TABLE_ID        (0x02)

#define NIT_TABLE_ID        (0x40)
#define NIT_OTHER_TABLE_ID  (0x41)
#define SDT_TABLE_ID        (0x42)
#define SDT_OTHER_TABLE_ID  (0x46)
#define BAT_TABLE_ID        (0x4A)
#define TDT_TABLE_ID        (0x70)
#define TOT_TABLE_ID        (0x73)


/*******************************************************************************
*                  constants of ISO/IEC 13818-1 descriptor tag
*******************************************************************************/
#define VIDEO_STREAM_DESCRIPTOR_TAG                 (0x02)  /* 视频流描述符 */
#define AUDIO_STREAM_DESCRIPTOR_TAG                 (0x03)  /* 音频流描述符 */
#define HIERARCHY_DESCRIPTOR_TAG                    (0x04)  /* 体系描述符 */
#define REGISTRATION_DESCRIPTOR_TAG                 (0x05)  /* 注册描述符 */
#define DATA_STREAM_ALIGNMENT_DESCRIPTOR_TAG        (0x06)  /* 数据流对齐描述符 */
#define TARGET_BACKGROUND_GRID_DESCRIPTOR_TAG       (0x07)  /* 目标背景栅格描述符 */
#define VIDEO_WINDOW_DESCRIPTOR_TAG                 (0x08)
#define CA_DESCRIPTOR_TAG                           (0x09)
#define ISO_639_LANGUAGE_DESCRIPTOR_TAG             (0x0A)
#define SYSTEM_CLOCK_DESCRIPTOR_TAG                 (0x0B)
#define MULTIPLEX_BUFFER_UTILISATION_DESCRIPTOR_TAG (0x0C)
#define COPYRIGHT_DESCRIPTOR_TAG                    (0x0D)
#define MAXIMAL_BITRATE_DESCRIPTOR_TAG              (0x0E)
#define PRIVATE_DATA_INDICATOR_DESCRIPTOR_TAG       (0x0F)
/*
#define SMOOTHING_BUFFER_DESCRIPTOR_TAG             (0x10)
#define STD_DESCRIPTOR_TAG                          (0x11)
#define IBP_DESCRIPTOR_TAG                          (0x12)

#define MPEG4_VIDEO_DESCRIPTOR_TAG                  (0x1B)
#define MPEG4_AUDIO_DESCRIPTOR_TAG                  (0x1C)
#define IOD_DESCRIPTOR_TAG                          (0x1D)
#define SL_DESCRIPTOR_TAG                           (0x1E)
#define FMC_DESCRIPTOR_TAG                          (0x1F)
#define EXTERNAL_ES_ID_DESCRIPTOR_TAG               (0x20)
#define MUXCODE_DESCRIPTOR_TAG                      (0x21)
#define FMXBUFFERSIZE_DESCRIPTOR_TAG                (0x22)
#define MULTIPLEXBUFFER_DESCRIPTOR_TAG              (0x23)
*/


/*******************************************************************************
*                    constants of DVB-SI descriptor tag
*******************************************************************************/
#define NETWORK_NAME_DESCRIPTOR_TAG                 (0x40)
#define SERVICE_LIST_DESCRIPTOR_TAG                 (0x41)
#define STUFFING_DESCRIPTOR_TAG                     (0x42)
#define SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_TAG    (0x43)
#define CABLE_DELIVERY_SYSTEM_DESCRIPTOR_TAG        (0x44)
#define VBI_DATA_DESCRIPTOR_TAG                     (0x45)
#define VBI_TELETEXT_DESCRIPTOR_TAG                 (0x46)
#define BOUQUET_NAME_DESCRIPTOR_TAG                 (0x47)
#define SERVICE_DESCRIPTOR_TAG                      (0x48)
#define COUNTRY_AVAILABILITY_DESCRIPTOR_TAG         (0x49)
#define LINKAGE_DESCRIPTOR_TAG                      (0x4A)
#define NVOD_REFERENCE_DESCRIPTOR_TAG               (0x4B)
#define TIME_SHIFTED_SERVICE_DESCRIPTOR_TAG         (0x4C)
#define SHORT_EVENT_DESCRIPTOR_TAG                  (0x4D)
#define EXTENDED_EVENT_DESCRIPTOR_TAG               (0x4E)
#define TIME_SHIFTED_EVENT_DESCRIPTOR_TAG           (0x4F)
#define COMPONENT_DESCRIPTOR_TAG                    (0x50)
#define MOSAIC_DESCRIPTOR_TAG                       (0x51)
#define STREAM_IDENTIFIER_DESCRIPTOR_TAG            (0x52)
#define CA_IDENTIFIER_DESCRIPTOR_TAG                (0x53)
#define CONTENT_DESCRIPTOR_TAG                      (0x54)
#define PARENTAL_RATING_DESCRIPTOR_TAG              (0x55)
#define TELETEXT_DESCRIPTOR_TAG                     (0x56)
#define TELEPHONE_DESCRIPTOR_TAG                    (0x57)
#define LOCAL_TIME_OFFSET_DESCRIPTOR_TAG            (0x58)
#define SUBTITLING_DESCRIPTOR_TAG                   (0x59)
#define TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR_TAG  (0x5A)
#define MULTILINGUAL_NETWORK_NAME_DESCRIPTOR_TAG    (0x5B)
#define MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR_TAG    (0x5C)
#define MULTILINGUAL_SERVICE_NAME_DESCRIPTOR_TAG    (0x5D)
#define MULTILINGUAL_COMPONENT_DESCRIPTOR_TAG       (0x5E)
#define PRIVATE_DATA_SPECIFIER_DESCRIPTOR_TAG       (0x5F)
#define SERVICE_MOVE_DESCRIPTOR_TAG                 (0x60)
#define SHORT_SMOOTHING_BUFFER_DESCRIPTOR_TAG       (0x61)
#define FREQUENCY_LIST_DESCRIPTOR_TAG               (0x62)
#define PARTIAL_TRANSPORT_STREAM_DESCRIPTOR_TAG     (0x63)
#define DATA_BROADCAST_DESCRIPTOR_TAG               (0x64)
#define CA_SYSTEM_DESCRIPTOR_TAG                    (0x65)
#define DATA_BROADCAST_ID_DESCRIPTOR_TAG            (0x66)
#define TRANSPORT_STREAM_DESCRIPTOR_TAG             (0x67)
#define DSNG_DESCRIPTOR_TAG                         (0x68)
#define PDC_DESCRIPTOR_TAG                          (0x69)
#define AC3_DESCRIPTOR_TAG                          (0x6A)
#define ANCILLARY_DATA_DESCRIPTOR_TAG               (0x6B)
#define CELL_LIST_DESCRIPTOR_TAG                    (0x6C)
#define CELL_FREQUENCY_LINK_DESCRIPTOR_TAG          (0x6D)
#define ANNOUNCEMENT_SUPPORT_DESCRIPTOR_TAG         (0x6E)
#define ENHANCE_AC3_DESCRIPTOR_TAG					(0x7A)
#define LOGICAL_CHANNEL_DESCRIPTOR_TAG      (0x81)
#define DVB_DOWNLOAD_DESCRIPTOR_TAG                      (0x84)
/*******************************************************************************
*                             other constants
*******************************************************************************/
/* stream types of component */
#define STREAM_TYPE_MPEG1_VIDEO         (0x01)
#define STREAM_TYPE_MPEG2_VIDEO         (0x02)
#define STREAM_TYPE_MPEG1_AUDIO         (0x03)
#define STREAM_TYPE_MPEG2_AUDIO         (0x04)
#define STREAM_TYPE_PRIVATE_SECTION     (0x05)
#define STREAM_TYPE_PRIVATE_PES_DATA    (0x06)
#define STREAM_TYPE_MPEG_AAC            (0x0F)
#define STREAM_TYPE_MPEG4P2             (0x10)
#define STREAM_TYPE_MPEG4_AAC           (0x11)
#define STREAM_TYPE_H264                (0x1B)
#define STREAM_TYPE_AC3                 (0x81)
#define STREASM_TYPE_EAC3				(0x7a)

/* service types */
#define SERVICE_TYPE_NONE			(0xFF)
#define SERVICE_TYPE_UNKNOWN            (0x00)
#define SERVICE_TYPE_TV                 (0x01)
#define SERVICE_TYPE_RADIO              (0x02)
#define SERVICE_TYPE_TELETEXT           (0x03)
#define SERVICE_TYPE_NVOD_REFER         (0x04)
#define SERVICE_TYPE_NVOD_TIMESHIFT     (0x05)
#define SERVICE_TYPE_MOSAIC             (0x06)
#define SERVICE_TYPE_DATA_BRODCAST      (0x0C)


/* character set of text */
#define CHARACTER_SET_ISOIEC_6937       (0x00)
#define CHARACTER_SET_GB2312            (0x13)


/* modulation scheme of cable_delivery_system_descriptor */
#define CABLE_MOD_NOT_DEFINED           (0x00)
#define CABLE_MOD_16QAM                 (0x01)
#define CABLE_MOD_32QAM                 (0x02)
#define CABLE_MOD_64QAM                 (0x03)
#define CABLE_MOD_128QAM                (0x04)
#define CABLE_MOD_256QAM                (0x05)
/* 0x06 ~ 0xFF: reserved for future use */


/* outer FEC scheme */
#define FEC_OUTER_NOT_DEFINED           (0x0)   /* not defined */
#define FEC_OUTER_NO_CODING             (0x1)   /* no outer FEC coding */
#define FEC_OUTER_RS_204_188            (0x2)   /* RS(204/188) Reed-Solomon codes */
/* 0x3 ~ 0xF: reserved for future use */


/* inner FEC scheme */
#define FEC_INNER_NOT_DEFINED           (0x0)   /* not defined */
#define FEC_INNER_CODE_RATE_1_2         (0x1)   /* 1/2 convolutional code rate */
#define FEC_INNER_CODE_RATE_2_3         (0x2)   /* 2/3 convolutional code rate */
#define FEC_INNER_CODE_RATE_3_4         (0x3)   /* 3/4 convolutional code rate */
#define FEC_INNER_CODE_RATE_5_6         (0x4)   /* 5/6 convolutional code rate */
#define FEC_INNER_CODE_RATE_7_8         (0x5)   /* 7/8 convolutional code rate */
#define FEC_INNER_NO_CONV_CODING        (0xF)   /* no convolutional coding */
/* 0x6 ~ 0xE: reserved for future use */


/* modulation scheme of satellite_delivery_system_descriptor */
#define SATELLITE_MOD_NOT_DEFINED       (0x00)
#define SATELLITE_MOD_QPSK              (0x01)
/* 0x02 ~ 0x1F: reserved for future use */


/* polarization scheme of satellite_delivery_system_descriptor */
#define POLARIZATION_LINEAR_HORIZONTAL  (0x0)
#define POLARIZATION_LINEAR_VERTICAL    (0x1)
#define POLARIZATION_CIRCULAR_LEFT      (0x2)
#define POLARIZATION_CIRCULAR_RIGHT     (0x3)


/* bandwidth scheme of terrestrial_delivery_system_descriptor */
#define BANDWIDTH_8MHz                  (0x0)
#define BANDWIDTH_7MHz                  (0x1)
#define BANDWIDTH_6MHz                  (0x2)
#define BANDWIDTH_5MHz                  (0x3)
/* 0x4 ~ 0x7: reserved for future use */


/* constellation scheme of terrestrial_delivery_system_descriptor */
#define CONSTELLATION_QPSK              (0x0)
#define CONSTELLATION_16QAM             (0x1)
#define CONSTELLATION_64QAM             (0x2)
/* 0x3: reserved for future use */


/* hierarchy_information scheme of terrestrial_delivery_system_descriptor */
#define NON_HIERARCHICAL                (0x0)
#define HIERARCHICAL_ALPHA1             (0x1)
#define HIERARCHICAL_ALPHA2             (0x2)
#define HIERARCHICAL_ALPHA3             (0x3)
/* 0x4 ~ 0x7: reserved for future use */


/* HP_stream, LP_stream code rate scheme of terrestrial_delivery_system_descriptor */
#define STREAM_CODE_RATE_1_2            (0x0)
#define STREAM_CODE_RATE_2_3            (0x1)
#define STREAM_CODE_RATE_3_4            (0x2)
#define STREAM_CODE_RATE_5_6            (0x3)
#define STREAM_CODE_RATE_7_8            (0x4)
/* 0x5 ~ 0x7: reserved for future use */


/* guard_interval scheme of terrestrial_delivery_system_descriptor */
#define GUARD_INTERVAL_1_32             (0x0)
#define GUARD_INTERVAL_1_16             (0x1)
#define GUARD_INTERVAL_1_8              (0x2)
#define GUARD_INTERVAL_1_4              (0x3)


/* transmission_mode scheme of terrestrial_delivery_system_descriptor */
#define TRANSMISSION_MODE_2K            (0x0)
#define TRANSMISSION_MODE_8K            (0x1)
#define TRANSMISSION_MODE_4K            (0x2)
/* 0x3: reserved for future use */



/*******************************************************************************
*                           structures of descriptors
*******************************************************************************/

typedef struct
{
    U32     u32CharSet;     /* 字符集 */
    U8      u8Len;
    C8 *    sText;
} Text_Info_t;


typedef struct
{
    U16     u16CASystemId;
    U16     u16CAPid;
    U8      u8PrivateDataLen;
    U8 *    pu8PrivateData;
} CA_Desc_t;


typedef struct
{
    U32     u32Freq;
    U32     u32SymRate;
    U8      u8Mod;
    U8      u8FEC_Outer;
    U8      u8FEC_Inner;
} Delivery_System_Desc_Cab_t;


typedef struct
{
    U32     u32Freq;
    U32     u32SymRate;
    U16     u16OrbitalPosition;
    U8      u8WestEastFlag;
    U8      u8Polarization;
    U8      u8Mod;
    U8      u8FEC_Inner;
} Delivery_System_Desc_Sat_t;


typedef struct
{
    U32     u32CentreFreq;
    U8      u8BandWidth;
    U8      u8Constellation;
    U8      u8HierarchyInfo;
    U8      u8CodeRateHP_Stream;
    U8      u8CodeRateLP_Stream;
    U8      u8GuardInterval;
    U8      u8TransmissionMode;
    U8      u8OtherFreqFlag;
} Delivery_System_Desc_Ter_t;


typedef struct
{
    U32     u32LangCode;
    U8      u8AudioType;
} Language_Info_t;

typedef struct
{
    U16                 u16LangNum;
    Language_Info_t *   ptLangInfo;
} ISO_639_Language_Desc_t;


typedef struct
{
    U16     u16TS_Id;
    U16     u16ON_Id;
    U16     u16ServId;
    U8      u8LinkageType;
    U8      u8PrivateDataLen;
    U8 *    pu8PrivateData;
} Linkage_Desc_t;


typedef struct
{
    U16     u16TS_Id;
    U16     u16ON_Id;
    U16     u16ServId;
} NVOD_Ref_Info_t;


typedef struct
{
    U16                 u16RefInfoNum;
    NVOD_Ref_Info_t *   ptRefInfo;
} NVOD_Reference_Desc_t;


typedef struct
{
    U16     u16RefServId;
    U16     u16RefEventId;
} NVOD_TimeShift_Event_Desc_t;


typedef struct
{
    U16     u16RefServId;
} NVOD_TimeShift_Serv_Desc_t;


typedef struct
{
    U8              u8ServType;
    Text_Info_t *   ptServProviderName;
    Text_Info_t *   ptServName;
} Service_Desc_t;


typedef struct
{
    U8 u8BuquetNameLen;
    C8 *pBuquetName;
}Bouquet_Name_t;

typedef struct
{
    U16 u16ServiceId;
    U16 u16LogicalNum;
}Bouquet_Logical_Channel_t;


typedef struct
{
    U16 u16ServiceId;
    U8 u8ServiceType;
}Service_Info_t;

typedef struct
{
    U16 u16ListNum;
    Service_Info_t *pServInfo;
}Service_List_Des_t;

/* LW requirement {{{ */
typedef struct
{
    U16 u16ServiceId;
    U8  u8VisibleServicesFlag;/* 1 表示可见0 不可见*/
    U16 u16LogicalNum;
}LW_Logical_Num_t;

typedef struct
{
    U16 u16ListNum;
    LW_Logical_Num_t *pLogicalInfo;
}LW_Logical_List_t;

/* }}} LW zxguan */

typedef struct
{
    U16 u16TS_Id;
    U16 u16ON_Id;
    
    U16 u16ServListDesNum;
    Service_List_Des_t *pServList;
    
#if 1 /*LW requirement*/
   U16 u16LwListNum;
   LW_Logical_List_t *pLwList;
#endif
    U16 u16PsiSiDesNum;
  
}BAT_Component_t;

typedef struct
{
    U16 u16ServiceId;
    U8 u8ServiceType;
}Service_List_t;


/*
描述子通用结构
    u8Tag: 描述子标识
    u8len: 描述子长度
    pu8Data: 描述子数据
*/
typedef struct
{
    U8      u8Tag;
    U8      u8Len;
    U8 *    pu8Data;
} PSISI_Descriptor_t;



/*******************************************************************************
*                           structures of section
*******************************************************************************/
typedef struct
{
    U16                     u16ProgramNumber;
    U16                     u16PMTPid;
} PAT_Program_t;


typedef struct
{
    U8                      u8TableId;
    U8                      u8Version;
    U16                     u16TS_Id;
    U32                     u32CRC;
    /* programs */
    U16                     u16ProgNum;
    PAT_Program_t *         ptProgram;
} PAT_Section_t;




typedef struct
{
    U16                     u16ServId;
    U8                      u8EitSchFlag;
    U8                      u8EitPF_Flag;
    U8                      u8RunningStatus;
    U8                      u8FreeCA_Mode;
    /* service descriptor */
    Service_Desc_t *                ptServDesc;
    /* NVOD reference descriptor */
    NVOD_Reference_Desc_t *         ptNvodRefDesc;
    /* NVOD timeshift service descriptor */
    NVOD_TimeShift_Serv_Desc_t *    ptNvodTimeShiftDesc;
    /* other descriptors */
    U16                     u16OtherDescNum;
    PSISI_Descriptor_t *    ptDesc;
} SDT_Service_t;


typedef struct
{
    U8                      u8TableId;
    U8                      u8Version;
    U16                     u16TS_Id;
    U16                     u16ON_Id;
    U32                     u32CRC;
    /* services */
    U16                     u16ServiceNum;
    SDT_Service_t *         ptService;
} SDT_Section_t;


typedef struct
{
    U16 u16BouquetId;
    U8 u8Version;
    U32 u32CRC;
    U16 u16BouquetNameCount;
    Bouquet_Name_t *pBouquetName;
    //U16 u16BouquetLogicalNum;
    //Bouquet_Logical_Channel_t *pLogicalList;
    U16 u16BouquetComNum;
    BAT_Component_t *pBouquetCom;
#if 1    
    U16 u16Logical_Channel_Num;
    LW_Logical_Num_t *pLwLogicalInfo;
#endif
    U16 u16PsiSiDesNum;
    PSISI_Descriptor_t *pDescriptor;
}BAT_Section_t;

#if 0
typedef struct 
{
    U8 u8TableId;
    U16 u16ServiceNum;
    BAT_Service_t *ptService;   
}BAT_Section_t;
#endif

typedef struct
{
    U16                     u16TS_Id;
    U16                     u16ON_Id;
    /* delivery system descriptor */
    U8                      u8DeliveryDescTag;
    void *                  pDeliveryDesc;
    /* other descriptors */
    U16                     u16OtherDescNum;
    PSISI_Descriptor_t *    ptDesc;
} NIT_TpStream_t;


typedef struct
{
    U8                      u8TableId;
    U8                      u8Version;
    U16                     u16NetworkId;
    U32                     u32CRC;
    /* linkage descriptors */
    U16                     u16LinkageDescNum;
    Linkage_Desc_t *        ptLinkageDesc;
    /* other descriptors */
    U16                     u16OtherDescNum;
    PSISI_Descriptor_t *    ptDesc;
    /* transport streams */
    U16                     u16TpStreamNum;
    NIT_TpStream_t *        ptTpStream;
} NIT_Section_t;




typedef struct
{
    U8                          u8StreamType;
    U16                         u16ElementaryPid;
    /* CA descriptors */
    U16                         u16CA_DescNum;
    CA_Desc_t *                 ptCA_Desc;
    /* iso 639 language descriptor */
    ISO_639_Language_Desc_t *   ptLangDesc;
    /* other descriptors */
    U16                         u16OtherDescNum;
    PSISI_Descriptor_t *        ptDesc;
} PMT_Component_t;


typedef struct
{
    U8                      u8TableId;
    U8                      u8Version;
    U16                     u16ProgramNum;
    U32                     u32CRC;
    U16                     u16PCRPid;
    /* CA descriptors */
    U16                     u16CA_DescNum;
    CA_Desc_t *             ptCA_Desc;
    /* other descriptors */
    U16                     u16OtherDescNum;
    PSISI_Descriptor_t *    ptDesc;
    /* video */
    PMT_Component_t *       ptVideo;
    /* audio */
    U16                     u16AudioNum;
    PMT_Component_t *       ptAudio;
    /* components */
    U16                     u16ComponentNum;
    PMT_Component_t *       ptComponent;
} PMT_Section_t;


/*******************************************************************************
*                    table common structure and APIs
*
* NOTE: the structures and prototypes listed below is ONLY for PAT, CAT, PMT,
*       NIT, BAT, SDT and EIT table. DO NOT use it for TDT/TOT table
*******************************************************************************/

#define PSISI_ERR_TABLE_REPEAT      (0x80100001)
#define PSISI_ERR_TABLE_UPDATE      (0x80100002)

typedef struct
{
    void *  pNextTable;         /* 指向下一个 PSISI_Table_t 结构, 用于有多个 extend_id 的情况, 如 BAT */
    U16     u16TablePid;
    U8      u8TableId;
    U16     u16ExtendId;
    U16     u16SectionNum;      /* table 中的 section 总数 */
    U16     u16RecievedNum;     /* 收到的 section 数目 */
    void *  ppSection[1];
} PSISI_Table_t;


PSISI_Table_t * PSISI_Alloc_Table(U16 u16Pid, U8 * ptData, U16 u16Len);

void PSISI_Free_Table(PSISI_Table_t * ptTable);

S32 PSISI_Is_Table_Complete(PSISI_Table_t * ptTable);



S32 PSISI_Parse_PAT_Section(PSISI_Table_t * ptTable, U8 * ptData, U16 u16Len);

S32 PSISI_Parse_SDT_Section(PSISI_Table_t * ptTable, U8 * ptData, U16 u16Len);

S32 PSISI_Parse_NIT_Section(PSISI_Table_t * ptTable, U8 * ptData, U16 u16Len);

S32 PSISI_Parse_PMT_Section(PSISI_Table_t * ptTable, U8 * ptData, U16 u16Len);

S32 PSISI_Parse_BAT_Section(PSISI_Table_t * ptTable, U8 * ptData, U16 u16Len);



/*******************************************************************************
*                   TDT and TOT table structure and APIs
*******************************************************************************/
U8 Parse_TDT(U32 *UTC_time_m, U32 *UTC_time_l, U8 *data_buffer);
U8 Parse_TOT(U32 *UTC_time_m, U32 *UTC_time_l, U8 *data_buffer);


#ifdef __cplusplus
}
#endif

#endif /* __PSI_SI_H_ */

