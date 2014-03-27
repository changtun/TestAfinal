#include <stdio.h>
#include <string.h>

#include "osapi.h"

#include "bcd.h"
#include "background.h"
#include "psi_si.h"
#include "systime.h"
#include <sys/time.h>
#include <time.h>/* for settime() added by pxiao on 2011-02-11 */ 
#include "pbitrace.h"
#include <errno.h> 



//U32						g_second = 0;
//static u32				g_sec;
//static time_td_t 			g_Local_Time;
static time_orig_t 			g_Utc_time;
static U32					g_Run_Sec = 0; 
static time_td_t 			g_Run_Time = {0, 0, 0, 0, 0, 0}; /*BYB(2008/10/31) add*/
//static task_t*	p_task_RTC = NULL;
//static void*	p_tDesc_RTC = NULL;
static void*	p_stack_RTC = NULL;
static U32 		guSystime_Task = 0xFFFFFFFF;
SysTimeNotifyFunc g_SystimeUpdate_CallBack = NULL;

static U32		g_sem_background_tdt;
static U32		g_sem_systemtime; 
unsigned long local_sec = 0;
Led_Time_Dot_Notify_t	g_systime_led_dot_update = NULL;
static   char   set_andsys_time =0;
//extern int errno;
STB_TimeMsg_Notify  g_time_update_cb;

time_td_t Time_Convert_Sec_to_Td(U32 sec);
static U32 _convert_BCD_time_to_sec (U32 hour_min_sec);
// U8 _convert_summer_offset(void);
// void _get_offset_value(U8 offset_flag,U8 *summer_sw,U8 *plus_value,U8 *hour_value,U8 *minute_value);
static U8 _return_offset_flag(U8 summer_sw,U8 plus_value,U8 hour_value,U8 minute_value);
static void  _time_plus_offset (time_orig_t*local_time, time_orig_t base_origtime,U32 offset,U8 negat_posit);
static void _tdt_table_notify(SYS_Table_Msg_t meg_send);
static int _RTCTime_Init(void);

void Time_RegMessage_Notify_CallBack(STB_TimeMsg_Notify Cb)
{
	g_time_update_cb = Cb;
}
void Time_UnRegMessage_Notify_CallBack(void)
{
	g_time_update_cb = NULL;
}



/******************************************************************************
* 函数 : Get_SysRunTime
* 参数 :
*		无
* 返回值 :	time_td_t 返回全局变量g_run_time
* 函数描述 :
*			系统运行时间，被改了可能有问题
******************************************************************************/
time_td_t* Get_SysRunTime( void )
{
	return (&g_Run_Time);
}
/******************************************************************************
* 函数 : Time_Set_Utc_to_Sys
* 参数 :
*		time_orig_t g_Utc_Sys_time 需要设置的时间 time_orig_t格式
* 返回值 :	long long int 由纪元1970年1月1日 00:00:00至需设置时间的秒数
* 函数描述 :
*			将传入的时间设置到系统时间
*修改历史      :
*1. 日期       : 2011-02-12
*作者       : Pxiao
*修改方式   : 新生成函数
******************************************************************************/
long long int Time_Set_Utc_to_Sys(time_orig_t g_Utc_Sys_time)
{
	/* 获取系统时间，秒级(1970) */
	time_td_t std_time;			
	struct timeval start;	
	int iyear = 0;
	int imon = 0;
	int iday = 0;
	int ihour = 0;
	int imin = 0;
	int isec = 0;
	int iera = 1970;/*纪元时间*/
	int i = 0;
	int leap_add = 0;
	int count_days = 0;
	/*add by HY 20110225*/
	g_Utc_time.bcd_time = g_Utc_Sys_time.bcd_time;
	g_Utc_time.mjd_date = g_Utc_Sys_time.mjd_date;
	/*add by HY 20110225*/
	std_time = Time_Convert_Orig_to_Td(g_Utc_Sys_time);
	std_time.hour += 8;
	//printf("#####___%d\%d\%d\__%d:%d:%d_%d_\n",std_time.year,std_time.month,std_time.date,std_time.hour,std_time.minute,std_time.second,std_time.week);
	
	iyear = std_time.year;
	imon = std_time.month;
	iday = std_time.date;
	ihour = std_time.hour;
	imin = std_time.minute;
	isec = std_time.second;
	i = iera;
	while( i < iyear )
	{
		if(((i % 4 == 0)&&(i % 100 != 0))||((i % 4 == 0)&&(i % 400 == 0)))/* 判断是否为闰年 */
		{
			++leap_add;/* 记录闰年的数量 */
		}
		++i;
	}
	i = 1;
	while(i < imon)
	{
		count_days += Get_Days_of_Month(iyear-2000,i);/* 计算该年1月到需设置时间的天数 */
		++i;
	}
	count_days = count_days + iday - 1;/* 抛去当天不算 */
	//printf("###_______iyear = %d,imon = %d,iday = %d,ihour = %d,imin = %d,isec = %d\n",iyear,imon,iday,ihour,imin,isec);
	start.tv_sec = (iyear-iera)*365*24*60*60 + leap_add*24*60*60 + count_days*24*60*60 + ihour*60*60 + imin*60 + isec;/* 计算秒数 */
	//printf("#####_____________start.tv_sec =%ld\n",start.tv_sec);			
	settimeofday(&start,NULL);/* 设置系统时间 */
	return start.tv_sec;
}
/**************************************************************************
函 数 名  : Time_Get_UTC_Time
功能描述  :得到从tdt解析出的时间，即实时时间
输入参数  : 无
	        
	        
输出参数  : time_orig_t *cpy        UTC时间指针
返 回 值  :   无
调用函数  : 
被调函数  : time_orig_t Get_Local_Time(void)	被此函数调用，此函数得到本地
											时间后再转为夏令时。	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
void Time_Get_UTC_Time(time_orig_t *cpy)
{	
	OS_SemWait(g_sem_systemtime);
	cpy->mjd_date = g_Utc_time.mjd_date;
	cpy->bcd_time = g_Utc_time.bcd_time;
	OS_SemSignal(g_sem_systemtime);
}

/******************************************************************************
* Function : Set_SystemUTCTime
* parameters :	
*		std_time : (IN) Time should be set .
* Return :	
*		Nothing
* Description :
*		This function get number of month by year(2000~2099) and month 
*
* Author : pxiao	2010/11/24
******************************************************************************/
 void Set_SystemUTCTime( time_orig_t std_time)
 {
	OS_SemWait(g_sem_systemtime);
	//printf("##########___Coming___Set_SystemUTCTime_____\n");
//	pbi_info(("##########___Coming___Set_SystemUTCTime_____\n"));
	Time_Set_Utc_to_Sys(std_time);/* 设置系统时间 by pxiao on 2011-02-12 */
	//printf("##########___Back___Set_SystemUTCTime_____\n");
//	pbi_info(("##########___Back___Set_SystemUTCTime_____\n"));
	OS_SemSignal(g_sem_systemtime);
 }

/**************************************************************************
函 数 名  : Get_Days_of_Month
功能描述  :得到某年某月的具体日子数。
输入参数  : 	U16 year 	年
				U16 month 	月
输出参数  : 无
返 回 值  :   	年月有效返回正确值。无效返回0
调用函数  : 
被调函数  : Time_Add_Td，Time_Minus_Td，td时间加减时被调用
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
U16 Get_Days_of_Month(U16 year,U16 month)
{
    U16      y,m;

	y=2000+year;
	m=month;
      	
    if((m<=0)||(m>12)||(y<=0))
	   	return 0;

    if (m<=7)
   	{
	   	  if (m==2)
	   	    { 
	   	    	if ((year % 400) == 0 || (((year % 100) != 0) && ((year % 4) == 0)))
			 		return 29;
		        else
			 		return 28;
		     }
		  if (m%2==1)
		 	  return 31;
		  else
		  	  return 30;
	  }
	else
	{
          if (m%2==0)
		       return 31;
		  else
		  	   return 30;
 	}

}


/**************************************************************************
函 数 名  : Time_Convert_YMD_to_MJD
功能描述  :将年月日转换为mjd时间
输入参数  : 	U16 year,年
				U16 month,月
				U16 date，日
输出参数  : 无
返 回 值  :   mjd,返回mjd时间。
调用函数  : 
被调函数  : Time_Convert_Td_to_Orig，此函数调用它，转换年月日
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
//注：以上公式适用于1900 年3月1日 至 2100 年2 月28 日。mjd 15079-19000301
U16 Time_Convert_YMD_to_MJD(U16 year,U16 month,U16 date)
 {
	/*int Julian_Date;
	int Modified_Julian_Date;
	
	Julian_Date = 367*year - (int)(7*(year + (int)((month+9)/12))/4) + (int)((275*month)/9) + date + 1721013;

	Modified_Julian_Date = Julian_Date - 2400000;
	
	return Modified_Julian_Date;*/
	int mjd;
	int l;
    
	year = year -1900;
	if(month == 1 || month == 2)
		l =1;
	else 
		l =0;
	mjd = 14956 + date + (year -l)*36525/100+(month + 1 + l*12)*306001/10000;
	return (U16)mjd;
		
}


/**************************************************************************
函 数 名  : Time_Convert_MJD_to_YMD
功能描述  :把mjd时间转换为年月日
输入参数  :U16 mjd_date, mjd时间
	        
	        
输出参数  : 	U16 *year,年
				U16 *month,月
				U16 *date，日
返 回 值  :   无
调用函数  : 
被调函数  :Time_Convert_Orig_to_Td
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
void Time_Convert_MJD_to_YMD(U16 mjd_date,U16 *year,U16 *month,U16 *date)
{
	int				y1, m1, k, mjd;

	mjd = mjd_date & 0x0000ffff;
	y1 = (mjd * 100 - 1507820) / 36525;
	m1 = (mjd * 10000 - 149561000 - (int)((y1 * 3652500) / 10000) * 10000) / 306001;
	//*date = ((mjd - 14956) * 10000 -
	//	((y1 * 3652500)/10000) * 10000 -
	//	((m1 * 306001)/10000) * 10000) / 10000;

	*date = ((mjd - 14956) * 10000 -
		((y1 * 3652500)/10000) * 10000 -
		((m1 * 306001)/10000) * 10000) / 10000;;
	
	if ((m1 == 14) || (m1 == 15))
		k = 1;
	else
		k = 0;

	*year = (y1 + k);
	//printf(":::::%d\n",*year);
	*year = *year+1900;
	*month = m1 - 1 - k * 12;
	//week = (((int)mjd +2)%7) + 1;
	if(*month > 12) *month = 1;
	//if(week > 6) week = 0;
	if(*date > 31) *date = 1;
}


/**************************************************************************
函 数 名  : Time_Convert_HMS_to_BCD
功能描述  :把时分秒转换为bcd
输入参数  : U16 hour,U16 min,U16 sec,时分秒
	        
	        
输出参数  : 无
返 回 值  :   U32 bcd型时间
调用函数  : bin_to_bcd
被调函数  :  Time_Convert_Td_to_Orig，此函数调用它，转换时分秒
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
U32 Time_Convert_HMS_to_BCD(U16 hour,U16 min,U16 sec)
{
	U32 	temp0=0,temp1=0,temp2=0;
    
	temp0 =	bin_to_bcd(hour);
	temp1 =	bin_to_bcd(min);
	temp2 =	bin_to_bcd(sec);
	return (((temp0<<16)|(temp1<<8))|temp2);
	//return (((hour/10)<<20 )|((hour%10<<16))|((min/10)<<12 )|((min%10<<8))|((sec/10)<<4 )|((sec%10)));
}


/**************************************************************************
函 数 名  : Time_Convert_BCD_to_HMS
功能描述  :把bcd转换为时分秒
输入参数  : U32 bcd_time,时间
	        
	        
输出参数  : U16 *hour,U16 *min,U16 *sec时分秒
返 回 值  :   无
调用函数  : bcd_to_bin
被调函数  : Time_Convert_Orig_to_Td
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
void Time_Convert_BCD_to_HMS(U32 bcd_time,U16 *hour,U16 *min,U16 *sec)
{
	U32 bin;
	bin = bcd_to_bin(bcd_time);
	*hour = bin/10000;
	*min = (bin%10000)/100;
	*sec = bin%100;
	//*hour = (U16)bcd_to_bin ((bcd_time & 0xff0000) >> 16);
	//*min = (U16)bcd_to_bin ((bcd_time & 0xff00) >> 8);
	//*sec = (U16)bcd_to_bin (bcd_time & 0xff);
	//printf("Time_Convert_BCD_to_HMSTime_Convert_BCD_to_HMS::%u:%u,%u\n",hour,min,bcd_time);
}


/**************************************************************************
函 数 名  : Time_Convert_Orig_to_Td
功能描述  :把orig时间转换为td时间
输入参数  : time_orig_t origtime，orig时间
	        
	        
输出参数  : 无
返 回 值  :   time_td_t 返回转换后的td时间
调用函数  : ime_Convert_MJD_to_YMD，Time_Convert_BCD_to_HMS
被调函数  : Get_Local_Td_time，Time_Add_Orig，Time_Minus_Orig
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
time_td_t Time_Convert_Orig_to_Td(time_orig_t origtime)
{
	time_td_t tdtime;
	//tdtime.year = 0;
	//tdtime.month = 0;
	//tdtime.date = 0;
	//tdtime.week = 0;
	//tdtime.hour = 0;
	//tdtime.minute= 0;
	//tdtime.second= 0;
	Time_Convert_MJD_to_YMD(origtime.mjd_date, &tdtime.year, &tdtime.month, &tdtime.date);
	Time_Convert_BCD_to_HMS(origtime.bcd_time, &tdtime.hour, &tdtime.minute, &tdtime.second);
    /*根据MJD计算WD*/
	tdtime.week = (((int)origtime.mjd_date+2)%7) + 1;
	if(tdtime.week > 6) 
    {
        tdtime.week = 0;
	}
	return tdtime;
}


/**************************************************************************
函 数 名  : Time_Convert_Td_to_Orig
功能描述  :把td时间转换为orig时间
输入参数  : time_td_t tdtime，要转换的td时间
	        
	        
输出参数  : 无
返 回 值  :  time_orig_t 返回orig时间
调用函数  : Time_Convert_YMD_to_MJD，Time_Convert_HMS_to_BCD
被调函数  : Time_Add_Orig，Time_Minus_Orig
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
time_orig_t Time_Convert_Td_to_Orig(time_td_t tdtime)
{
	time_orig_t origtime;
	//origtime.bcd_time = 0;
	//origtime.mjd_date = 0;
	origtime.mjd_date = Time_Convert_YMD_to_MJD(tdtime.year, tdtime.month, tdtime.date);
	origtime.bcd_time = Time_Convert_HMS_to_BCD(tdtime.hour, tdtime.minute, tdtime.second);
	return origtime;

}


//WEEK 0-6
/**************************************************************************
函 数 名  : Time_Add_Td
功能描述  :在某个td时间上加td时间
输入参数  : 	time_td_t tdtime,基础td时间
				time_td_t add_value 增加td时间
	        
	        
输出参数  : 无
返 回 值  :   time_td_t加后的td时间
调用函数  : Get_Days_of_Month，
被调函数  : Time_Add_Orig，orig时间不能直接加减要借助td时间加减和转换函数完成。
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
time_td_t Time_Add_Td(time_td_t tdtime,time_td_t add_value)
{
	time_td_t td_sum ={0,0,0,0,0,0,0};
	int days;
	days = Get_Days_of_Month(tdtime.year,tdtime.month);
	if( days == 0 )
	{
		return td_sum;

	}
	td_sum.second = tdtime.second + add_value.second;
	td_sum.minute = tdtime.minute + add_value.minute;
	td_sum.hour  	 = tdtime.hour   + add_value.hour;
	td_sum.week   = tdtime.week   + add_value.week;
	td_sum.date  	 = tdtime.date   + add_value.date;
	td_sum.month = tdtime.month  + add_value.month;
	td_sum.year   = tdtime.year   + add_value.year;
	while(td_sum.second >= 60)
	{
		td_sum.second -= 60;
		td_sum.minute+=1;
	}


	while(td_sum.minute>= 60)
	{
		td_sum.minute-= 60;
		td_sum.hour+=1;
	}
	
	while(td_sum.hour >= 24)
	{
		td_sum.hour -= 24;
		td_sum.date+=1;
		td_sum.week+=1;
		if(td_sum.week>=7)
			td_sum.week-=7;
	}
	while(td_sum.date >= days )
	{
		td_sum.date -=days;
		td_sum.month +=1;
	}
	
	while(td_sum.month >12)
	{
		td_sum.month -=12;
		td_sum.year +=1;
	}
	return td_sum;
}


/**************************************************************************
函 数 名  : Time_Add_Td
功能描述  :在某个td时间上减td时间
输入参数  : 	time_td_t tdtime,基础td时间
				time_td_t add_value 减去td时间
	        
	        
输出参数  : 无
返 回 值  :   time_td_t减后的td时间
调用函数  : Get_Days_of_Month，
被调函数  : Time_Minus_Orig，orig时间不能直接加减要借助td时间加减和转换函数完成。
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
time_td_t Time_Minus_Td(time_td_t tdtime,time_td_t minus_value)
{
	time_td_t td_sum;
	U16 days;
	days = Get_Days_of_Month(tdtime.year,tdtime.month);
	td_sum.second = tdtime.second - minus_value.second;
	td_sum.minute = tdtime.minute - minus_value.minute;
	td_sum.hour   = tdtime.hour   - minus_value.hour;
	td_sum.week   = tdtime.week   - minus_value.week;
	td_sum.date   = tdtime.date   - minus_value.date  ;
	td_sum.month  = tdtime.month  - minus_value.month;
	td_sum.year   = tdtime.year   - minus_value.year;
	 while((S16)td_sum.second < 0)
	{
		td_sum.second += 60;
		td_sum.minute-=1;
	}

	
	while((S16)td_sum.minute < 0)
	{
		td_sum.minute+= 60;
		td_sum.hour-=1;
	}

	while((S16)td_sum.hour < 0)
	{
		td_sum.hour += 24;
		td_sum.date -=1;
		td_sum.week -=1;
		if((S16)td_sum.week < 0)
			td_sum.week+=7;
	}

	while((S16)td_sum.date <= 0 )
	{
		if(((S16)td_sum.month )==1)
		{
			td_sum.month =13;
			td_sum.year -=1;
		}
		days = Get_Days_of_Month(td_sum.year,td_sum.month-1);
		td_sum.date +=days;
		td_sum.month -=1;
	}
	while((S16)td_sum.month <=0)
	{
		td_sum.month +=12;
		td_sum.year -=1;
	}
	return td_sum;

}


//只加bcd,年月日清0
/**************************************************************************
函 数 名  : Time_Add_Orig
功能描述  :orig时间相加，只加时分秒，即只加bcd,年月日清0
输入参数  : 	time_orig_t origtime,基础时间
				time_orig_t add_value增加时间
	        
	        
输出参数  : 无
返 回 值  :   time_orig_t 返回加后的orig时间
调用函数  : Time_Convert_Orig_to_Td，Time_Add_Td，Time_Convert_Td_to_Orig
被调函数  : _time_plus_offset，夏令时转换时，要加时间时用
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
time_orig_t Time_Add_Orig(time_orig_t origtime,time_orig_t add_value)
{
	time_td_t tdtime,add_td;
	tdtime = Time_Convert_Orig_to_Td( origtime);
	add_td = Time_Convert_Orig_to_Td( add_value);
	//printf("add orig %d/%d/%d\n",add_td.year,add_td.month,add_td.date);
	add_td.year=0;		
	add_td.month=0;
	add_td.date=0;
	tdtime = Time_Add_Td(tdtime,add_td);
	return Time_Convert_Td_to_Orig( tdtime);
	
}


/**************************************************************************
函 数 名  : Time_Add_Orig
功能描述  :orig时间相减，只减时分秒，即只减bcd,年月日清0
输入参数  : 	time_orig_t origtime,基础时间
				time_orig_t add_value减去时间
	        
	        
输出参数  : 无
返 回 值  :   time_orig_t 返回减后的orig时间
调用函数  : Time_Convert_Orig_to_Td，Time_Minus_Td，Time_Convert_Td_to_Orig
被调函数  : _time_plus_offset，夏令时转换时，要减时间时用
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
time_orig_t Time_Minus_Orig(time_orig_t origtime,time_orig_t minus_value)
{
	time_td_t tdtime,minus_td;
	tdtime = Time_Convert_Orig_to_Td( origtime);
	minus_td = Time_Convert_Orig_to_Td( minus_value);
	minus_td.year = 0;
	minus_td.month = 0;
	minus_td.date = 0;
	tdtime = Time_Minus_Td(tdtime,minus_td);
	return Time_Convert_Td_to_Orig( tdtime);
}


/**************************************************************************
函 数 名  : Get_Local_Time
功能描述  :得到本地时间，即得到tdt时间后再转夏令时后的orig时间
输入参数  : 无
	        
	        
输出参数  : 无
返 回 值  :   time_orig_t，返回orig时间
调用函数  : Time_Get_UTC_Time，Time_Convert_UTC_to_Local转夏令时
被调函数  : 
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
time_orig_t Get_Local_Time(void)
{
	time_orig_t localtm;
	
	Time_Get_UTC_Time(&localtm);
	//修正
//	realtime =Time_Add_Orig(localtm,  origtime);
	//转夏令时，返回
	return Time_Convert_UTC_to_Local( localtm );
	
}


/**************************************************************************
函 数 名  : Get_Local_Time
功能描述  :得到本地时间，即得到tdt时间后再转夏令时后的td时间
输入参数  : 无
	        
	        
输出参数  : 无
返 回 值  :   time_td_t，返回td时间
调用函数  : Get_Local_Time，Time_Convert_Orig_to_Td
被调函数  : 
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
time_td_t Get_Local_Td_time(void)
{
	time_orig_t origtime;
    
	origtime = Get_Local_Time();
	return Time_Convert_Orig_to_Td(origtime);
}


/**************************************************************************
函 数 名  : Time_Convert_UTC_to_Local
功能描述  :在当前时间之上加减夏令时，即夏令时转换
输入参数  : time_orig_t utctime，要转夏令时的时间
	        
	        
输出参数  : 无
返 回 值  :   time_orig_t，转换后的orig时间
调用函数  : _convert_summer_offset，_get_offset_value，Time_Convert_HMS_to_BCD，
				_time_plus_offset
被调函数  : 
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
time_orig_t Time_Convert_UTC_to_Local(time_orig_t utctime)
{
	time_orig_t local_time;
	unsigned char 	summer_on,plus_value,hour_value,minute_value,summer_offset;
	unsigned  int offset;
	summer_offset=_convert_summer_offset();
	_get_offset_value(summer_offset,&summer_on,&plus_value,&hour_value,&minute_value);
	offset = Time_Convert_HMS_to_BCD(hour_value, minute_value, 0);
	_time_plus_offset(&local_time, utctime, offset, plus_value);
	return local_time;

}


/************************************************************************************
										内部函数
************************************************************************************/
/**************************************************************************
函 数 名  : _get_offset_value
功能描述  :得到时区信息，是否使用夏令时和是否使用半时区等信息。北京时间为东八区设置为0x50
输入参数  : U8 offset_flag，夏令时标志
//offset 从低到高		第一位for minite_value,是否使用半时区
	//						第二位--第五位for hour_value，GMT要加减的时间，
	//						第六七为for plus_value，0表示西:-，1中间:=，2东:+，3无效						
	//						第八位for summer_sw是否使用夏令时
	        
输出参数  :	 U8 *summer_sw,保存是否使用夏令时
				U8 *plus_value,保存东西半区，即加减情况
				U8 *hour_value,保存加减的时间
				U8 *minute_value是保存否使用半时区
返 回 值  :   无
调用函数  : 
被调函数  : Time_Convert_UTC_to_Local
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
 void _get_offset_value(U8 offset_flag,U8 *summer_sw,U8 *plus_value,U8 *hour_value,U8 *minute_value)
{

	unsigned char time_offset_flag;
	
	time_offset_flag=offset_flag;

	
	//offset 从低到高		第一位for minite_value,是否使用半时区
	//						第二位--第五位for hour_value，GMT要加减的时间，
	//						第六七为for plus_value，0表示西:-，1中间:=，2东:+，3无效						
	//						第八位for summer_sw是否使用夏令时
	if(!(time_offset_flag & 0x01))			
		*minute_value=0;	
	else 		
		*minute_value = 30;
	*hour_value = (time_offset_flag & 0x1e) >> 1;
	*plus_value = (time_offset_flag & 0x60) >> 5;
	*summer_sw = time_offset_flag >> 7;

}


/**************************************************************************
函 数 名  : _return_offset_flag
功能描述  :把具体值转换为offset标志
输入参数  :  U8 summer_sw,是否使用夏令时
				U8 plus_value,东西半区，即加减情况
				U8 hour_value,加减的时间
				U8 minute_value是否使用半时区
输出参数  : 
返 回 值  :   U8，返回转换后的offset
调用函数  : 
被调函数  : Time_Convert_UTC_to_Local
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
static U8 _return_offset_flag(U8 summer_sw,U8 plus_value,U8 hour_value,U8 minute_value)
{
	unsigned  short time_offset_flag=0;
	unsigned char minute_offset;

	time_offset_flag  =  0x80&(summer_sw << 7 );
	time_offset_flag|= ((plus_value & 0x03) << 5);
	time_offset_flag|= ((hour_value & 0x0f) << 1);

	if(minute_value==0)			minute_offset=0;
	else if(minute_value==30)  	minute_offset=1;
	
	time_offset_flag|= minute_offset;

	return  time_offset_flag;

}


/**************************************************************************
函 数 名  : _time_plus_offset
功能描述  :根据negat_posit加减标志在基础时间上加减bcd时间
输入参数  : time_orig_t base_origtime,基础时间
				U32 bcd_offset,bcd时间
				U8 negat_posit，加减标志
输出参数  : time_orig_t*local_time，计算后的时间
返 回 值  :   
调用函数  : Time_Minus_Orig，Time_Add_Orig
被调函数  : time_orig_t*local_time
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
static void  _time_plus_offset (time_orig_t*local_time, time_orig_t base_origtime,U32 bcd_offset,U8 negat_posit)
{
	time_orig_t tmp;
    
	//这里，不能设置mjd_date为0 ，此时年实际上并不为0
	tmp.mjd_date = 0;
	tmp.bcd_time = bcd_offset;
	if(negat_posit == 0)
	{
        *local_time = Time_Minus_Orig( base_origtime,tmp );
	}
	else if(negat_posit == 1)//bujia
	{
        *local_time = base_origtime;
	}
	else//jia or jian
	{
		*local_time = Time_Add_Orig( base_origtime,tmp );
	}
}

/**************************************************************************
函 数 名  : _convert_summer_offset
功能描述  :换算夏令时标志
输入参数  : 
	        
	        
输出参数  : 
返 回 值  :   
调用函数  : _get_offset_value，_return_offset_flag，DB_SysCfg_Get
被调函数  : 
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/

#define eSYSCFG_TIME_OFFSET_VALUE 0x50

 U8  _convert_summer_offset(void)
{

	unsigned char hour_value,minute_value,plus_value,summer_on ;

	
	_get_offset_value(eSYSCFG_TIME_OFFSET_VALUE,&summer_on, &plus_value, &hour_value, &minute_value);

/*根据夏令时标示和时区标示进行时间换算*/
	if(summer_on)/*使用夏令时*/
	{
		if(plus_value==0)//-  西时区
		{
			if(hour_value==1)
			{
				plus_value =1;
				hour_value--;
			}
			else 
			{
				hour_value--;
			}
 
		}	
		else if(plus_value==1)//=
		{
			plus_value=2;
			hour_value++;
		}		
		else//+  东时区
		{
			hour_value++;
		}
		/*把换算后的时间再转换成offset标志*/
		return (_return_offset_flag( summer_on, plus_value, hour_value, minute_value));
	}
	else /*不使用夏令时*/
	{
		return eSYSCFG_TIME_OFFSET_VALUE;	
	}


}


/**************************************************************************
函 数 名  : _convert_BCD_time_to_sec
功能描述  :将传进来的时分秒转换成秒。如传进来的时间为12:20:24，转换成12*3600+20*60+24=44424
输入参数  : U32 hour_min_sec ，BCD时间时分秒
	        
	        
输出参数  : 
返 回 值  :   
调用函数  : 
被调函数  : 
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
static U32 _convert_BCD_time_to_sec (U32 hour_min_sec)
{
	U32 second;
	
  	second		= ((((hour_min_sec & 0xf00000) >> 20) * 10 + ((hour_min_sec & 0xf0000) >> 16))*3600) 
			+ ((((hour_min_sec & 0xf000) >> 12) * 10 + ((hour_min_sec & 0xf00) >> 8)) * 60) 
			+ (((hour_min_sec & 0xf0) >> 4) * 10 + (hour_min_sec & 0xf));  

	return second;
}


const U8 Days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/**************************************************************************
函 数 名  : Time_Convert_Sec_to_td
功能描述  : 把自 1970年1月1日 00:00:00 起的秒数转换为 td 时间。
输入参数  : U32 sec秒
	        
	        
输出参数  : 
返 回 值  :   返回time_td_t时间
调用函数  : 
被调函数  : 
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
//计算sec合计多长时间，不是个时间点，是时间长度用常用的年月的量
time_td_t Time_Convert_Sec_to_Td(U32 sec)
{
	time_td_t td_time;
	U32 time = sec;
    U32 uiPass4year;
    U32 uiHoursPerYear;

    //计算时差

    //取秒时间
    td_time.second = (int)(time % 60);
    time /= 60;
    //取分钟时间
    td_time.minute = (int)(time % 60);
    time /= 60;
    //取过去多少个四年，每四年有 1461*24 小时
    uiPass4year = time / (1461 * 24);
    //计算年份
    td_time.year = (uiPass4year << 2) + 70;
    //四年中剩下的小时数
    time %= 1461 * 24;
    //校正闰年影响的年份，计算一年中剩下的小时数
    for (;;)
    {
        //一年的小时数
        uiHoursPerYear = 365 * 24;
        //判断闰年
        if ((td_time.year & 3) == 0)
        {
            //是闰年，一年则多24小时，即一天
            uiHoursPerYear += 24;
        }

        if (time < uiHoursPerYear)
        {
            break;
        }
        td_time.year++;
        time -= uiHoursPerYear;
    }
    //小时数
    td_time.hour = (int)(time % 24);
    //一年中剩下的天数
    time /= 24;
    //假定为闰年
    time++;
    //校正润年的误差，计算月份，日期
    if ((td_time.year & 3) == 0)
    {
        if (time > 60)
        {
            time--;
        }
        else
        {
            if (time == 60)
            {
                td_time.month= 1;
                td_time.date = 29;

				td_time.month += 1;
				td_time.year += 1900;
				return td_time;
            }
        }
    }
	
    //计算月日
    for (td_time.month = 0; Days[td_time.month] < time; td_time.month++)
    {
          time -= Days[td_time.month];
}

    td_time.date = (int)(time);

	td_time.month += 1;
	td_time.year += 1900;
    return td_time;
}


/**************************************************************************
函 数 名  : Time_Convert_Td_to_Sec
功能描述  : 把 td 时间转换为自 1970年1月1日 00:00:00 起的秒数。
输入参数  : U32 sec秒
	        
	        
输出参数  : 
返 回 值  : 返回自 1970年1月1日 00:00:00 起的秒数。
调用函数  : 
被调函数  : 
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
long long int Time_Convert_Td_to_Sec( time_td_t td_time )
{
	long long int second = 0;
    
	td_time.month -= 1;
	td_time.year -= 1900;

    if(0 >= (S32)(td_time.month -= 2))
	{	/* 1..12 -> 11,12,1..10 */
		td_time.month += 12;              /* Puts Feb last since it has leap day*/
		td_time.year -= 1;
    }

    second = ((((U32)(td_time.year/4 - td_time.year/100 + td_time.year/400 + 367*td_time.month/12 +
             td_time.date) + td_time.year*365 - 719499
         )*24 + td_time.hour /* now have hours */
       )*60 + td_time.minute /* now have minutes */
     )*60 + td_time.second; /* finally seconds */

	return second;
}


/**************************************************************************
函 数 名  : _tdt_table_notify
功能描述  :从tdt中解析出orig时间，把解析的时间给全局变量
				g_Utc_time，每秒解析一次
输入参数  : SYS_Table_Msg_t meg_send 表信息
	        
	        
输出参数  : 
返 回 值  :   
调用函数  : 
被调函数  : 
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
int g_SystemTimeShowCount = 0;
static void _tdt_table_notify(SYS_Table_Msg_t meg_send)
{
	unsigned int 	UTC_time_m;
	unsigned int 	UTC_time_l;
	unsigned int 	ui_gUTC_time_m;
	unsigned int 	ui_gUTC_time_l;
	time_td_t			g_td_Time;

	if(NULL != meg_send.buffer)
	{
		switch(meg_send.table_id)
		{
			case TDT_TABLE_ID:
			{
				Parse_TDT(&UTC_time_m, &UTC_time_l, meg_send.buffer); 
			}
			break;
				
			case TOT_TABLE_ID:
			{
				Parse_TOT(&UTC_time_m, &UTC_time_l, meg_send.buffer);
			}
			break;
		}
		if(NULL != meg_send.pfFreeBuffer)
		{
			meg_send.pfFreeBuffer(meg_send.buffer);
		}
	}
	
	OS_SemWait(g_sem_systemtime);
	//将解析的utc时间转换为s_td_t时间，设置全局s_td_t时间
	ui_gUTC_time_m = g_Utc_time.mjd_date;
	ui_gUTC_time_l =g_Utc_time.bcd_time;

	g_Utc_time.mjd_date= UTC_time_m;
	g_Utc_time.bcd_time= UTC_time_l;
#if 1
	{
		
		g_td_Time = Time_Convert_Orig_to_Td( Time_Convert_UTC_to_Local( g_Utc_time ) );
	    g_SystemTimeShowCount++;
      //  if( g_SystemTimeShowCount >= 30 )
        {
		pbiinfo("\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
		pbiinfo("current time %04d-%02d-%02d %02d:%02d:%02d.\n", 
						g_td_Time.year, g_td_Time.month, g_td_Time.date,
						g_td_Time.hour, g_td_Time.minute, g_td_Time.second );
            g_SystemTimeShowCount = 0;
        }
	pbiinfo("%s ----%d   set_andsys_time---%d",__FUNCTION__,__LINE__,set_andsys_time);
	
	if(set_andsys_time ==0)
		{   
		#if 0
			char  buf[32];
			memset(buf,0,32);   
			pbiinfo("@@@@@@@@@@@@@@@2----set andoird systeime##############3\n");
			sprintf(buf,"date -s %d%02d%02d",g_td_Time.year,g_td_Time.month,g_td_Time.date);
			pbiinfo("--------%s\n",buf);
			system(buf);
			
	#else
			time_t  t;
			struct tm  *p,*p2;
			struct timeval tv;
			struct timezone tz;
			int ret;
			char  buf[32];
			char  flag =0;
			time(&t);
			
			p=gmtime(&t);

			pbiinfo("@@@@@@@@@cur time ==year%d mon%d day%d hour%d",(*p).tm_year,(*p).tm_mon,(*p).tm_mday,(*p).tm_hour);
			if((*p).tm_year <=(g_td_Time.year-1900))
			{
				if((*p).tm_year ==(g_td_Time.year-1900))
				{
					if((*p).tm_mon <=(g_td_Time.month-1))
					{
						if((*p).tm_mon ==(g_td_Time.month-1))
						{
							if((*p).tm_mday<=g_td_Time.date)
							{
								if((*p).tm_mday ==g_td_Time.date)
								{
									pbiinfo("@@@@@ %d curetime  ==new time  year&&month&&day ",__LINE__);
									flag =0;
								}
								else
								{
									pbiinfo("@@@@@ %d curetime  ==new timeday ",__LINE__);
									flag =1;
								}

							}
							else
							{								
								flag =0;
							}
						}
						else
						{
								flag =1;  
						}
					}
					else
					{
						flag =0;
					}
				}
				else
				{
					flag =1;  
				}
				
			}
			else
			{
				flag =0;    
			}
			
			if(flag ==1)
			{
			(*p).tm_year =g_td_Time.year-1900;
			(*p).tm_mon=g_td_Time.month-1;
			(*p).tm_mday=g_td_Time.date;
			(*p).tm_hour=g_td_Time.hour;
			(*p).tm_min=g_td_Time.minute;
			(*p).tm_sec =g_td_Time.second ;

			pbiinfo("@@@@@@@@@@@@@@cur time ==year%d mon%d day%d hour%d",(*p).tm_year,(*p).tm_mon,(*p).tm_mday,(*p).tm_hour);
			t=mktime(p);
			
			#if 0
			tz.tz_minuteswest =0;
			tz.tz_dsttime =0;
			
			tv.tv_sec =t;
			tv.tv_usec =0;
			ret =settimeofday(&tv,&tz);
			#endif
			memset(buf,0,32);   
			pbiinfo("@@@@@@@@@@@@@@@2----set andoird systeime##############3\n");
			sprintf(buf,"date %ld",t);
			pbiinfo("--------%s\n",buf);
			system(buf);
			
			if(g_time_update_cb)
				g_time_update_cb();
			}

		#endif	
			set_andsys_time =1;
			
		}
		
	}
#endif
	OS_SemSignal(g_sem_systemtime);

	//比较解析的时间和之前的utc全局时间
	//if((ui_gUTC_time_m != UTC_time_m) || (ui_gUTC_time_l != UTC_time_l)&&( ePOWER_STATE_ON == DB_Get_Last_Power_State() ))   //del by pxwang for epg 2012
	if((ui_gUTC_time_m != UTC_time_m) || (ui_gUTC_time_l != UTC_time_l))
	{
		if(g_SystimeUpdate_CallBack != NULL)
		{
		    //发送TIMEUPDATE消息
			g_SystimeUpdate_CallBack();
		}
	}
	OS_SemSignal(g_sem_background_tdt);
}

unsigned  int get_sec(unsigned hour_min_sec)
{
	unsigned  int second;
	
  	second		= ((((hour_min_sec & 0xf00000) >> 20) * 10 + ((hour_min_sec & 0xf0000) >> 16))*3600) 
			+ ((((hour_min_sec & 0xf000) >> 12) * 10 + ((hour_min_sec & 0xf00) >> 8)) * 60) 
			+ (((hour_min_sec & 0xf0) >> 4) * 10 + (hour_min_sec & 0xf));  

	return second;
}

void sec_to_utc_type(time_orig_t *time,signed int  time_sec, time_orig_t *utc_time_table)
{

	signed char 		second,minute,hour;
	unsigned  int	mod,temp0,temp1,temp2;
	signed int 	debug=0;

	if(time_sec<0) {
		time_sec=24*3600+time_sec;
		debug=-1;
	}
	hour=time_sec / 3600;
	mod=time_sec % 3600;
	if(time_sec>=(24*3600)){
		hour-=24;
		debug=1;
	}
	minute=mod / 60;
	mod= mod % 60;
	second = mod ;
	hour =(signed char)bin_to_bcd((unsigned  int)hour);
	minute =(signed char)bin_to_bcd((unsigned  int)minute);
	second =(signed char)bin_to_bcd((unsigned  int)second);
	temp0=(unsigned  int)hour;
	temp1=(unsigned  int)minute;
	temp2=(unsigned  int)second;
	time->mjd_date=utc_time_table->mjd_date+debug;
	time->bcd_time=(((temp0<<16)|(temp1<<8))|temp2);
}



/**************************************************************************
函 数 名  : _system_time_process
功能描述  :被改过了，不懂了，原意15秒解析tdt，并发timeupdate
				g_second记录系统运行时间，并在不解析tdt期间软修正本地时间
输入参数  : 
	        
	        
输出参数  : 
返 回 值  :   
调用函数  : 
被调函数  : 
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
static void _system_time_process(void * param)
{
	U32	g_second = 0;
	unsigned int	second = 0;
	while (1)
	{
		OS_SemWait(g_sem_systemtime);

		second = get_sec( g_Utc_time.bcd_time);
		second += 1;
		g_Run_Time.second += 1;
		sec_to_utc_type( &g_Utc_time, second, &g_Utc_time);
		#if 0
		if((local_sec%1000)==0 &&local_sec != 0)/*非0整秒计时*/
		{
			second = get_sec( g_Utc_time.bcd_time);
			second += 1;
			sec_to_utc_type( &g_Utc_time, second, &g_Utc_time);
		    g_second +=1;
			if( NULL != g_systime_led_dot_update )
			{
				g_systime_led_dot_update();
			}
		}
		#endif
		//if((g_second == 15)&&( ePOWER_STATE_ON == DB_Get_Last_Power_State() ))  //del by pxwang for epg 2012
		if(g_second == 15)
		{
			//15秒后，调用CALLBACK函数
			if( g_SystimeUpdate_CallBack != NULL )
			{
				g_SystimeUpdate_CallBack();
			}
			g_second = 0;
		}
		else
		{
			g_second++;
		}
		/* add by BYB(2008/12/11) for serial command show_run_time{{{*/
		if( g_Run_Time.second >= 60 )
		{
   			g_Run_Time.minute += 1;
			g_Run_Time.second = 0;
		
			if( g_Run_Time.minute >= 60 )
			{
				g_Run_Time.hour += 1;
				g_Run_Time.minute = 0;

				if( g_Run_Time.hour >= 0xffff )
				{
					g_Run_Time.hour = 0;
				}
			}
		}
		/* }}}add by BYB(2008/12/11) for serial command show_run_time*/
		OS_SemSignal(g_sem_systemtime);
		//local_sec++;
		/*临时这么用for AVIT*/
		//task_delay(((clock_t)(ST_GetClocksPerSecond()))/1000);
		OS_TaskDelay(1000); //xhren0706 delay1ms;
	}

}


/**************************************************************************
函 数 名  : _RTCTime_Init
功能描述  :初始化_system_time_process任务
输入参数  : 
	        
	        
输出参数  : 
返 回 值  :   
调用函数  : 
被调函数  : 
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
static int _RTCTime_Init(void)
{
	size_t 	stack_size;
	S32 		error = 0;

	stack_size = 1024;

	

	p_stack_RTC = OS_MemAllocate(system_p, stack_size);

	if (p_stack_RTC == NULL)
	{
		error = 1;
	}
	else
	{
		error = OS_TaskCreate(&guSystime_Task, "systime process", _system_time_process, 
		NULL, p_stack_RTC, stack_size, 3, 0);
	}
	
	if(error == 0)
	{
		return error;
	}
	else
	{			
		return error;
	}
}


/**************************************************************************
函 数 名  : System_Time_Init
功能描述  :模块初始化
输入参数  : 
	        
	        
输出参数  : 
返 回 值  :   
调用函数  : 
被调函数  : 
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
void System_Time_Init(SysTimeNotifyFunc SystimeUpdate_Callback)
{
	U8 		error = 0;
	//unsigned int 	UTC_time_m;
//	unsigned int 	UTC_time_l;
	SYS_Table_Param_t	tStartTDTFilter;
	SYS_Table_InitParam_t init_parameter;

	time_orig_t 			temp;

	g_SystimeUpdate_CallBack = SystimeUpdate_Callback;
	
	OS_SemCreate(&g_sem_systemtime, (const char *)"sem_systemtime",1,0);
	OS_SemCreate(&g_sem_background_tdt, (const char *)"g_sem_bg_tdt", 0,0);
	init_parameter.callback= _tdt_table_notify;
	init_parameter.p_semaphore= g_sem_background_tdt;
	error = SYS_BackGround_TableInit(SYS_BG_TDT, init_parameter);
	tStartTDTFilter.pid = 0x14;
	tStartTDTFilter.table_id = 0x70;
	
	tStartTDTFilter.timeout = ONESEC/2;

	tStartTDTFilter.pn = 0;	
	error =SYS_BackGround_TableStart(SYS_BG_TDT, tStartTDTFilter);
	
	if (error)
	//	printf("Background_TDT_Init error !!!!!!!\n");
		pbierror(("Background_TDT_Init error !!!!!!!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"));
	else
	{
		pbierror(("Background_TDT_Init okokokokok !!!!!!!++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"));

	}


	temp.mjd_date  =Time_Convert_YMD_to_MJD(2012,1,1);
	temp.bcd_time =Time_Convert_HMS_to_BCD(0,0,0);
	g_Utc_time.mjd_date =temp.mjd_date;
	g_Utc_time.bcd_time=temp.bcd_time;
	
	_RTCTime_Init();
	/*JWF  2006.06.26 add {{{*/
#if 0
	
	UTC_time_m = change_gregorian_calendar_to_mjd( 2006, 1, 1);
	UTC_time_l = 0;
	get_Utc_Time(&g_Utc_Time, UTC_time_m, UTC_time_l);
	g_Utc_Code.utc_m = UTC_time_m;
	g_Utc_Code.utc_l = UTC_time_l;
	get_Current_LocalTime(&g_Local_Time);
	#endif
	/*}}}*/
}

/**************************************************************************
函 数 名  : Register_Led_Dot_Update
功能描述  :不是我写的，不懂
输入参数  : 
	        
	        
输出参数  : 
返 回 值  :   
调用函数  : 
被调函数  : 
	 
修改历史      :
1. 日期       : 
作者       : 
修改方式   : 新生成函数
**************************************************************************/
void Register_Led_Dot_Update( Led_Time_Dot_Notify_t callback)
{
	OS_SemWait(g_sem_systemtime);
	g_systime_led_dot_update = callback;
	if( NULL == callback )
	{
	}
	OS_SemSignal(g_sem_systemtime);
}


