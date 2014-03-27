#ifndef __BACKGROUND_H_
#define __BACKGROUND_H_

#include "pvddefs.h"
#include "osapi.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
	SYS_BG_PAT,                  						//BackGround����PAT        		
	SYS_BG_NIT,                         					//BackGround����NIT 
	SYS_BG_SDT,                      					//BackGround����SDT  
	SYS_BG_BAT,								//BackGround����BAT  
	SYS_BG_CAT,								//BackGround����CAT  
	SYS_BG_TDT,								//BackGround����TDT  
	SYS_BG_EIT,									//BackGround����EIT
	SYS_BG_PMT,   			  					//BackGround����PMT  
	SYS_BG_NOTABLE							//BackGround����NULL  
}SYS_BG_Type_e;		//BackGround������


enum
{
	SYS_TABLE_NOERROR=0,                       		//û�д���
	SYS_TABLE_FILTFR_ERROR,     			//Filter Free�Ĵ���
	SYS_TABLE_FILTALLO_ERROR,                      //Filter Allocate�Ĵ���
	SYS_TABLE_FILTCON_ERROR,				//Filter Control�Ĵ���
	SYS_TABLE_FILTSET_ERROR,				//Filter Set�Ĵ���
	SYS_TABLE_FILTASSO_ERROR,			//Filter Associate�Ĵ���
	SYS_TABLE_CHANFR_ERROR,				//Channel Free�Ĵ���
	SYS_TABLE_CHANALLO_ERROR,			//Channel Allocate�Ĵ���
	SYS_TABLE_CHANCON_ERROR,				//Channel Control�Ĵ���
	SYS_TABLE_CHANSET_ERROR,				//Channel Set�Ĵ���
	SYS_TABLE_LARTHANMAX_ERROR,			//BackGround ��������Ĵ���
	SYS_TABLE_NORUNNING,					//BackGroundû�����еĴ���
	SYS_TABLE_CHECKDATA_ERROR,			//BackGround CheckData�Ĵ���
	SYS_TABLE_UNKNOWN					//δ֪�Ĵ���
};						//���������



typedef struct 
{
	U16				usTSid;
	U16		       	usNid;
  	U16           	usONid;
  	U16				usSDTversion;
  	U16				usNITversion;
  	U16				usPATversion;
}SYS_BG_TPParam_t;



typedef enum
{
 	pTable_scan=0,
	pTable_update
}SYS_Table_Manage_Type_e;

typedef enum
{
	pTable_timeout = 0,
	pTable_stop,
	pTable_multi_section,
 	pTable_complete,
 	pTable_section_repeat
}SYS_Table_Msg_Type_e;

typedef  void (*tBm_FreeBufferFct) (void *Ptr);

typedef struct
{
	U8 							*buffer;
	U32							section_length;
	U32							pid;
	U32							table_id;
	U32							version;
	SYS_Table_Msg_Type_e  		msgType;
	SYS_Table_Manage_Type_e		manage_type;
	tBm_FreeBufferFct				pfFreeBuffer;
}SYS_Table_Msg_t;



typedef struct 
{
	U32 		pid;
	U32 		table_id;
	U32 		pn;
	OS_TIME	timeout;
} SYS_Table_Param_t;




typedef void (* table_notify_fct_t)(SYS_Table_Msg_t message);


typedef struct 
{
	table_notify_fct_t 	callback;
	U32 			p_semaphore;	
} SYS_Table_InitParam_t;





/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/
void  SYS_BackGround_Init(void);
U32  SYS_BackGround_TableInit(SYS_BG_Type_e Back_Type, SYS_Table_InitParam_t init);
U32  SYS_BackGround_TablePause(SYS_BG_Type_e Back_Type);
U32  SYS_BackGround_TableResume(SYS_BG_Type_e Back_Type);
U32  SYS_BackGround_TableStart(SYS_BG_Type_e Back_Type, SYS_Table_Param_t bgfilterdata);
U32  SYS_BackGround_TableStop(SYS_BG_Type_e Back_Type);
void  Set_NIT_First_flag(U8 enable);
U32  Start_Background(void);
U32  Stop_Background(void);
U32	 Pause_background(void);
U32  Resume_Background(void);
void  Set_program_info(SYS_BG_TPParam_t *tp_parameters);
U32 SYS_BackGround_PMT_Start(void);
U32 SYS_BackGround_PMT_Reset(U32 reqpid, U32 reqpn);
void SYS_BackGround_Bat_Default(void);
SYS_BG_TPParam_t Get_program_info( void );

typedef S32 (*DEMUX_CALLBACK)(U16 tsInputId, U16 pid, U8 *section);
void Register_Pmt_Callback_Fun( DEMUX_CALLBACK callback_fun ,U16 pid);

void UnRegister_PMT_Callback_Fun(void);
void CatTableVersionClaer();
void background_nit_init();
typedef void (*STB_NITMsg_Notify )(int  param1);
void Nit_RegMessage_Notify_CallBack(STB_NITMsg_Notify Cb);
void Nit_UnRegMessage_Notify_CallBack(void);
void CatTableReset();

void DL_Nit_RegMessage_Notify_CallBack(STB_NITMsg_Notify Cb);
void DL_Nit_UnRegMessage_Notify_CallBack(void);
#ifdef __cplusplus
}
#endif


#endif	/* __BACKGROUND_H_ */

