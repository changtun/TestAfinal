#ifndef __SYSTIME_H_
#define __SYSTIME_H_
#include  "pvddefs.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef struct tag_time_date {
	U16				year;
	U16				month;
	U16				date;
	U16				hour;
	U16				minute;
	U16				second;
	U16				week;
}time_td_t;

typedef struct tag_utc {
	U16	mjd_date;		/* 16bits MSB of 40bits */
	U32 bcd_time;		/* 24bits LSB of 40bits */
}time_orig_t;

typedef struct tag_time {
	unsigned short int				hour;			
	unsigned short int				minute;		
	unsigned short int				second;		
}time_hms_t;

typedef time_hms_t s_dur;
typedef void (* SysTimeNotifyFunc)(void);
typedef void (* Led_Time_Dot_Notify_t)(void);

void 		System_Time_Init(SysTimeNotifyFunc SystimeUpdate_Callback);
time_td_t 	Time_Add_Td(time_td_t tdtime,time_td_t add_value);
time_td_t 	Time_Minus_Td(time_td_t tdtime,time_td_t minus_value);
time_orig_t Time_Add_Orig(time_orig_t origtime,time_orig_t add_value);
time_orig_t Time_Minus_Orig(time_orig_t origtime,time_orig_t minus_value);
//time_td_t Time_Convert_Second_to_td(u32 sec);
time_td_t 	Time_Convert_Orig_to_Td(time_orig_t origtime);
time_orig_t Time_Convert_Td_to_Orig(time_td_t tdtime);
U16 		Time_Convert_YMD_to_MJD(U16 year,U16 month,U16 date);
void 		Time_Convert_MJD_to_YMD(U16 mjd_date,U16 *year,U16 *month,U16 *date);
U32 		Time_Convert_HMS_to_BCD(U16 hour,U16 min,U16 sec);
void 		Time_Convert_BCD_to_HMS(U32 bcd_time,U16 *hour,U16 *min,U16 *sec);
U16 		Get_Days_of_Month(U16 year,U16 month);
time_orig_t Get_Local_Time(void);
time_td_t Get_Local_Td_time(void);
void _get_offset_value(U8 offset_flag,U8 *summer_sw,U8 *plus_value,U8 *hour_value,U8 *minute_value);
U8	_convert_summer_offset(void);
/* added by pxiao on 2010-11-24 {{{ */
void Set_SystemUTCTime( time_orig_t );
/* added by pxiao on 2010-11-24 {{{ */
/*****************************************************************
ÏÄÁîÊ±×ª»»

*****************************************************************/
time_orig_t Time_Convert_UTC_to_Local(time_orig_t utctime);
void		Time_Get_UTC_Time(time_orig_t *cpy);
void Register_Led_Dot_Update( Led_Time_Dot_Notify_t callback);

time_td_t Time_Convert_Sec_to_Td(U32 sec);
long long int Time_Convert_Td_to_Sec( time_td_t td_time );
long long int Time_Set_Utc_to_Sys(time_orig_t g_Utc_Sys_time);
typedef void (*STB_TimeMsg_Notify )();
void Time_RegMessage_Notify_CallBack(STB_TimeMsg_Notify Cb);
void Time_UnRegMessage_Notify_CallBack(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif	/* __SYSTIME_H_ */

