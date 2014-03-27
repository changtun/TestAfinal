#ifndef	_CTI_SYS_H_
#define	_CTI_SYS_H_

#define	CTICA_PIN_CODE_LENGTH	4
#include "ctidef.h"
enum{

CA_CLIENT_CAT_QUERY = 0x02,
CA_CLIENT_PMT_QUERY = 0x03,
CA_CLIENT_CHANGE_PROG = 0x07,
CA_CLIENT_STOP_PROG_QUERY = 0x04,
CA_CLIENT_CHECK_PIN_QUERY = 0x05,
CA_CLIENT_CHANGE_PIN_QUERY = 0x06,
CA_CLIENT_IPPV_QUERY = 0x08,
CA_CLIENT_SC_INFO_QUERY = 0x0c,
CA_CLIENT_PPID_QUERY = 0x0f,
CA_CLIENT_PPID_INFO_QUERY = 0x0e,
CA_CLIENT_PPV_LIST_INFO_QUERY = 0x14,
CA_CLIENT_ANTI_RECORD_QUERY = 0x15,
CA_CLIENT_SET_PARENTAL_QUERY = 0x16,
CA_CLIENT_USER_VIEW_QUERY = 0x19,
CA_CLIENT_PARENTAL_CHANGE_QUERY = 0x1a,
CA_CLIENT_NIT_QUERY = 0x0a,
CA_CLIENT_CHANGE_PID_QUERY = 0x0d,
CA_CLIENT_PMT_QUERY_EXT = 0x20,
};

union OS_message_info 
{
   BYTE     c[8];
   WORD     s[4];
   ULONG    l[2];
   PVOID    vfp[2];
};

/*消息中Codebyte的值应为数字0~9对应的ASCII码，*/
/*例如：如果用户输入的PIN码中某位是’1’，*/
/*则其对应的Codebyte是’0x31’*/

typedef struct CTI_PinCode_Tag
{
	unsigned char OLD_PinCode[CTICA_PIN_CODE_LENGTH];
	unsigned char NEW_PinCode[CTICA_PIN_CODE_LENGTH];
}CTI_PinCode;

typedef struct CTI_IPPV_Tag
{
	unsigned char IPPV_FLag;		/*   0 停止购买IPPV-t节目，1点播IPPV-节目; 8点播IPPV-p节目*/
	unsigned int	IPPV_ProgramNo;	/*IPPV节目的标识ID*/
	unsigned char PinCode[4];
}CTI_IPPV;

typedef struct CTI_PPID_Info_Tag
{
	unsigned char PinCode[4];
	unsigned int 	PPID;		//占用低24位
	
}CTI_PPID_Info;

typedef struct CTI_PPV_List_Tag
{
	unsigned int PPID_ID;		//*占用低24位*/
	unsigned char List_Type;	// 1 获取IPPV节目列表;8获取PPV节目列表
	unsigned char List_Page;	// 标识所获取节目列表的页数，PPV节目共分8页，IPPV为1页
}CTI_PPV_List;

typedef struct CTI_Parental_Tag
{
	unsigned char PinCode[4];
	unsigned char Parental_rate;
}CTI_Parental;

typedef struct CTI_UserView_Tag
{
	unsigned char	type;
	unsigned char PinCode[4];
	unsigned char flag;
	unsigned char control_start_time[8];
	unsigned char control_end_time[8];
	unsigned char control_start_channel[2];
	unsigned char control_end_channel[2];
	unsigned char status;	
}CTI_UserView;

typedef struct
{
    INT                   type;
    union OS_message_info info;
} OS_MESSAGE;

typedef struct CAPMT_QueryData_tag
{
	unsigned short	frequency;
	unsigned short	network_id;
	unsigned short	ts_id;
	unsigned short	service_id;
}CAPMT_QueryData;


#endif

