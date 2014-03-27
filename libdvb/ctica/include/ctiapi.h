/****************************************************************************
 * COPYRIGHT (C) Compunicate Technologies, Inc. 2003-2009
 *
 * Source file name : ctiapi.h
 *
 *  Created on: 2009-7-22    Author: znjin
 *
 *  Description: the application specific defined for api.
 ***************************************************************************/

#ifndef API_H_
#define API_H_

#include "ctidef.h"
/*---------------------------- Type Definitions --------------------------------------------------*/
typedef enum
{
	CA_TRACE_NONE = 0,
	CA_TRACE_BASIC = 1,
	CA_TRACE_NORMAL = 3,
	CA_TRACE_DEBUG = 7
}CTI_TraceLevel_t;

typedef enum
{
	CACORE_INIT_OK = 0,
	CACORE_INIT_FAILED = 0xfe,
}CTI_CACoreInitReply_t;

typedef enum
{
	GET_CA_NOTIFY_INFO_OK = 0,
	GET_CA_CAT_INFO_ERROR,
	GET_CA_PMT_INFO_EEROR,
	GET_CA_IPPV_INFO_EEROR,
	GET_CA_SC_INFO_EEROR,
	GET_CA_PPID_EEROR,
	GET_CA_PPID_INFO_EEROR,
	GET_CA_PPV_LIST_INFO_EEROR,
	GET_CA_IPPV_LIST_INFO_EEROR,
	GET_CA_BMAIL_INFO_EEROR,
	GET_CA_OSD_INFO_EEROR,
	GET_CA_PROGSKIP_INFO_EEROR,
	GET_CA_USERVIEW_INFO_EEROR,
	GET_CA_NO_MATCHING_INFO
}CTI_GetCANotifyReply_t;

typedef enum
{
	CAT_QUERY = 0x02,
	PMT_QUERY = 0x03,
	CHANGE_PROG = 0x07,
	STOP_PROG_QUERY = 0x04,
	CHECK_PIN_QUERY = 0x05,
	CHANGE_PIN_QUERY = 0x06,
	IPPV_QUERY = 0x08,
	SC_INFO_QUERY = 0x0c,
	PPID_QUERY = 0x0f,
	PPID_INFO_QUERY = 0x0e,
	PPV_LIST_INFO_QUERY = 0x14,
	ANTI_RECORD_QUERY = 0x15,
	SET_PARENTAL_QUERY = 0x16,
	USER_VIEW_QUERY = 0x19,
	PARENTAL_CHANGE_QUERY = 0x1a,
	NIT_QUERY = 0x0a,
	CHANGE_PID_QUERY = 0x0d,
	CA_MESSAGE_QUERY = 0x21,
	CA_IP_DATA_QUERY = 0x33,
}CTI_CAMessage_t;

typedef enum
{
	CAT_QUERY_REPLY = 0x02,
	PMT_QUERY_REPLY = 0x03,
	STOP_PROG_QUERY_REPLY = 0x04,
	CHECK_PIN_QUERY_REPLY = 0x05,
	CHANGE_PIN_QUERY_REPLY = 0x06,
	IPPV_QUERY_REPLY = 0x08,
	NIT_QUERY_REPLY = 0x0a,
	NVRAM_OP_REPLY	= 0x0b,
	ANTI_RECORD_QUERY_REPLY = 0x15,
	SET_PARENTAL_QUERY_REPLY = 0x16,

	IPPV_NOTIFY = 0x07,
	BMAIL_NOTIFY = 0x09,
	CAT_INFO_NOTIFY = 0x10,
	PMT_INFO_NOTIFY = 0x11,
	SUBTITLE_NOTIFY = 0x12,
	OSD_DISPLAY_NOTIFY = 0x14,
	EMM_STATUS_NOTIFY = 0x13,
	SC_INFO_NOTIFY = 0x0c,
	PPID_NOTIFY = 0x0d,
	PPID_INFO_NOTIFY = 0x0e,
	CA_MESSAGE_CODE = 0x21,
	FINGERPRINT_NOTIFY = 0x20,
	USER_VIEW_INFO_NOTIFY = 0x22,
	PROG_SKIP_NOTIFY = 0x2a,
	PPV_LIST_INFO_NOTIFY = 0x2d,
	IPPV_LIST_INFO_NOTIFY = 0x2e,
	PROG_PARENTAL_RATE_NOTIFY = 0x30,
	SECURITY_INIT_ERROR_NOTIFY = 0x31,
	CHILD_MOTHER_IP_NOTIFY = 0x33,
	COS_SYSTEM_NOTIFY = 0x26,
}CTI_CAInfoNotify_t;

typedef struct
{
	BYTE pid_count;
	BYTE emm_pid[2];
}CTI_CatInfoNotify_t;

typedef struct
{
	BYTE program_number[2];
	BYTE desc_type;
	BYTE pid_count;
	BYTE ecm_pid[12][2];
}CTI_PmtInfoNotify_t;

typedef struct
{
	BYTE prog_number[2];
	BYTE status;
	BYTE ppid_id[3];
	CHAR ppid_label[17];
	BYTE p_type_support;
	BYTE t_type_support;
	BYTE ippv_prog_num[4];
	BYTE ippv_p_price[4];
	BYTE ippv_t_price[4];
}CTI_IppvInfoNotify_t;

typedef struct
{
	BYTE id_len;
	BYTE stb_id[16];
	BYTE sc_id[16];
}CTI_SecurityInfo_t;

typedef struct
{
	CHAR verify_start_time[8];
	BYTE work_period[2];
	BYTE mother_ua[5];
}CTI_ChildCardInfo_t;

typedef struct
{
	BYTE cardnumber[8];
	BYTE scstatus;
	BYTE usedstatus;
	BYTE systemid[2];
	BYTE ecmsubsysid[2];
	BYTE emmsubsysid[2];
	CHAR cosversion[9];
	CHAR cardlabel[17];
	CHAR issuetime[17];
	CHAR expiretime[17];
	BYTE parentalrating;
	BYTE groupcontrol;
	BYTE antimovectrl[3];
	BYTE smartcard_type;
	BYTE smartcard_matching;
	BYTE stb_ird_matching;
	CTI_ChildCardInfo_t childCard;
	CTI_SecurityInfo_t chip;
}CTI_SCInfoNotify_t;

typedef struct
{
	BYTE flag;
	BYTE control_start_time[8];
	BYTE control_end_time[8];
	BYTE control_start_channel[2];
	BYTE control_end_channel[2];
	BYTE status;
}CTI_UserViewInfoNotify_t;

typedef struct
{
	BYTE id[3];
	CHAR label[17];
	BYTE usedstatus;
	BYTE credit_deposit[4];
	CHAR credit_date[11];
	BYTE prog_right[32];
	CHAR prog_right_start_date[11];
	CHAR prog_right_end_date[11];
}CTI_PPidInfo_t;

typedef struct
{
	BYTE count;
	CTI_PPidInfo_t info[8];
}CTI_PPidNotify_t;

typedef struct
{
	BYTE start_number[4];
	BYTE end_number[4];
}CTI_PPVNumber_t;

typedef struct
{
	BYTE page;
	BYTE prog_count;
	CTI_PPVNumber_t ppv[32];
}CTI_PPVListNotify_t;

typedef struct
{
	CHAR prog_number[4];
}CTI_IPPVNumber_t;

typedef struct
{
	BYTE page;
	BYTE prog_count;
	CTI_IPPVNumber_t ippv[32];
}CTI_IPPVListNotify_t;

typedef struct
{
	BYTE status;
	BYTE tag;
	BYTE mark;
	BYTE build_time[8];
	BYTE send_time[8];
	BYTE from_len;
	CHAR from_context[256];
	BYTE title_len;
	CHAR title_context[256];
	WORD body_len;
	CHAR body_context[2048];
}CTI_CABmailInfo_t;

typedef struct
{
	BYTE avail;
	WORD coordinate_x_lt;
	WORD coordinate_y_lt;
	WORD coordinate_x_rt;
	WORD coordinate_y_rt;
	BYTE loop_num;
	WORD loop_interval;
	BYTE loop_speed;
	BYTE font_size;
	BYTE font_R;
	BYTE font_G;
	BYTE font_B;
	BYTE font_Z;
	BYTE back_R;
	BYTE back_G;
	BYTE back_B;
	BYTE back_Z;
}CTI_CAOsdDisplayStatus_t;

typedef struct
{
	BYTE status;
	BYTE type;
	BYTE netid_or_frq[2];
	BYTE tsid_or_mod[2];
	BYTE srvid_or_symbol[2];
	BYTE es_id[2];
	WORD message_len;
	CHAR message[256];
}CTI_ProgSkipInfo_t;

typedef struct
{
	BYTE prognumber[2];
	CTI_ProgSkipInfo_t skipdata;
}CTI_ProgSkipNotify_t;

typedef struct
{
	WORD Bmail_amount;
	WORD OSD_amount;
	BYTE STB_SerialNumber[8];
	CTI_TraceLevel_t cacore_trace_level;
	BYTE Author_Serial[11];
	BYTE STB_Name[11];
	BYTE STB_SC_Married_Type;
	BYTE STB_SC_Married_Control;
	BYTE PARENTAL_TYPE;
}CTI_CAcoreInitParam_t;


typedef struct
{
	WORD	Len;
	BYTE	Data[64*1024];
}CTI_CA_CM_IP_Param_t;


/*---------------------------- Variables Definitions -----------------------------------------------*/

/*------------------------------- API functions ----------------------------------------------------*/
/* core API */
BYTE CTICAS_CAcoreInit(CTI_CAcoreInitParam_t STB_Parameter, WORD *NVRAM_size);
BYTE CTICAS_SendStatusToCAcore(BYTE status);
BYTE CTICAS_SendMsgToCAcore(CTI_CAMessage_t type, BYTE* pdata, WORD len);
BYTE CTICAS_GetCAcoreInfo(CTI_CAInfoNotify_t type, BYTE* pparam, WORD len, void* pinfo);
VOID CTICAS_GetCAcoreVersion(CHAR *pVersion);
BYTE CTICAS_SetScProtocolToT1(void);
BYTE CTICAS_DeleteNvramCAinfo(void);
VOID CTICAS_HexadecimalToDecimal(BYTE *pHex, CHAR *pDec);

/* STB API */
void CTICAclient_UpcallFunction(CTI_CAInfoNotify_t type, BYTE* pparam, WORD len);
VOID CTICAclient_GetEpgTime(BYTE time[8]);
/*--------------------------------------------------------------------------------------------------*/
#endif /* API_H_ */
