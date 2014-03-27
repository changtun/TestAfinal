
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/*Pv core*/
#include "osapi.h"
#include "bcd.h"
#include "epg.h"
#include "epg_common.h"
#include "pbitrace.h"




/*******************************************************/
/*               Private Defines and Macros			   */
#define CMP_EVENT_UTC_L(EVENT,EVENT_CMP)				( ( ( NULL== EVENT )||( NULL== EVENT_CMP )||( ( EVENT)->start_time.bcd_time != (EVENT_CMP)->start_time.bcd_time ) )?1:0 )
#define CMP_EVENT_UTC_M(EVENT, EVENT_CMP)			( ( ( NULL== EVENT )||( NULL== EVENT_CMP )||( ( EVENT)->start_time.mjd_date != (EVENT_CMP)->start_time.mjd_date ) )?1:0 )



static U8 _CmpEventEndTime( const EVENT* event , time_td_t local_time);


/******************************************************************************
* Function : _CmpEventEndTime
* parameters :	
*		event(in) : 	事件信息
*		time(in):	相比较的时间
*		
* Return :	
*		1	: 当前时间大于时间的结束时间
*		0	: 当前时间等于或小于结束时间
*		
*		
* Description :
*		输入一个事件信息指针，取出事件的结束时间与输入的时间做比较。
*
* Author : SCJ	2009/11/23
******************************************************************************/
static U8 _CmpEventEndTime( const EVENT* event , time_td_t local_time)
{
	time_td_t tEnd_Time;

	if( NULL == event )
		return 0;
	
	App_Get_Event_End_Time( &tEnd_Time, event);/*得到输入事件的结束时间*/
	return( App_time_compare_all_param( tEnd_Time ,local_time ) );
}


void App_Register_cb(EpgNotifyFunc epg_notify_func)
{
	EPG_Register_Callback(epg_notify_func);

}




/******************************************************************************
* Function : App_Get_Event_Start_Time
* parameters :	
*		pEvent(in) : 当前播放的事件信息
*		time_date(out): 输出的当前事件的开始时间
*		
* Return :	
*		成功:  0
*		失败:  1
*		
*		
* Description :
*		输入事件信息，获取当前事件开始时间
*
* Author : SCJ	2009/11/23
******************************************************************************/
U32 App_Get_Event_Start_Time(time_td_t *time_date, const EVENT *pEvent)
{
	time_orig_t  	local_start_time;
	time_td_t		start_time_td_t;

	if( ( NULL == pEvent )||( NULL == time_date ) )
		return 1;

	memset( &local_start_time, 0, sizeof( time_orig_t ));
	memset( &start_time_td_t, 0, sizeof( time_td_t ));
	
	//App_Get_Event_Start_Time_Utc( &local_start_time, pEvent );
	*time_date = Time_Convert_Orig_to_Td( pEvent->start_time );
	
	return 0;
}


/******************************************************************************
* Function : App_Get_Event_End_Time
* parameters :	
*		pEvent(in) : 当前播放的事件信息
*		time_date(out): 输出的当前事件的结束时间
*		
* Return :	
*		成功:  0
*		失败:  1
*		
*		
* Description :
*		输入事件信息，获取当前事件结束时间
*
* Author : SCJ	2009/11/23
******************************************************************************/
U32 App_Get_Event_End_Time(time_td_t *time_date, const EVENT *pEvent)
{
	char 		cDuration[3];
	time_orig_t	tTime;
	time_td_t		tPtime;

	if( ( NULL == pEvent )||( NULL == time_date ) )
	{
		pbiinfo("\n parammeter   NULL====line %d\n",__LINE__);
		return 1;
	}
	
	memset( cDuration, 0, 3*sizeof( char ) );
	memset( &tTime, 0, sizeof( time_orig_t ) );
	memset( &tPtime, 0, sizeof( time_td_t ) );
	
	if(App_Get_Event_Start_Time(time_date, pEvent))
		return 1;
	
	cDuration[0]=pEvent->duration[0];
	cDuration[1]=pEvent->duration[1];
	cDuration[2]=pEvent->duration[2];
	
	
	time_date->hour+=bcd_to_bin (cDuration[0]);
	time_date->minute+=bcd_to_bin (cDuration[1]);
	time_date->second+=bcd_to_bin (cDuration[2]);
	
	/*if Event last from today to tomorrow*/
	time_date->minute+=(time_date->second/60);
	time_date->second%=60;
	time_date->hour+=(time_date->minute/60);
	time_date->minute%=60;
	tTime = pEvent->start_time;
	tTime.mjd_date+=(time_date->hour/24);
	time_date->hour%=24;

	//tlocal_end_time = Time_Convert_UTC_to_Local( tTime );
	tPtime = Time_Convert_Orig_to_Td( tTime );
		
	/*re-calculate utc_m to s_td_t*/	
	
	time_date->date=tPtime.date;
	time_date->week=tPtime.week;
	time_date->month=tPtime.month;
	time_date->year=tPtime.year;

	//pbiinfo("\n %s  line %d eventid %d  ENDTIME [%d%d%d][%d%d]=======++++++++++++++++++++++++++++++++++++++++\n",
		//__FUNCTION__,__LINE__,pEvent->event_id,time_date->year,time_date->month,time_date->date,
		//time_date->hour,time_date->minute,time_date->second);

	return 0;
}


/******************************************************************************
* Function : App_time_compare_all_param
* parameters :	
*			time1(in):a specific time
*			time2(in):a specific time
* Return :	
*		1:time1 is later than time2.
*		0:time1 is early than time2.
*		2:time1 is equal to time2.
* Description :
*			compare time all time param
* Author : Dugangfeng	2006/05/18
******************************************************************************/
U16 App_time_compare_all_param( time_td_t time1, time_td_t time2)
{
	if( time1.year > time2.year )
		return 1;
	else
	{
		if( time1.year < time2.year )
			return 0;
		else
		{
			if( time1.month> time2.month )
				return 1;
			else
			{
				if( time1.month < time2.month )
					return 0;
				else
				{
					if( time1.date> time2.date )
						return 1;
					else
					{
						if( time1.date < time2.date )
							return 0;
						else
						{
							if( time1.hour > time2.hour )
								return 1;
							else
							{
								if( time1.hour < time2.hour )
									return 0;
								else
								{
									if( time1.minute > time2.minute )
										return 1;
									else
									{
								           if( time1.minute < time2.minute )
									   		return 0;
									    else
									    {
									           if( time1.second > time2.second )
											   	return 1;
										    else
										    {
										         if( time1.second < time2.second )
											   		return 0;
										         else
												 return 2;	//equel
										    }
									    }
								     }	
								}
							}
						}
					}
				}
			}
		}
	}
}


U8 App_Get_Service_Rating(const U16 serv_id,const U16 ts_id,const U16 on_id)
{
	EVENT *event = NULL;
	U8	   parent_rate = 0;
	
	event = App_Get_Pf_Event( serv_id,ts_id,on_id, 0 );
	if( ( NULL != event )&&( NULL != event->parental_rating ) )
	{
		parent_rate = event->parental_rating->rate;
		pbiinfo("\App_Get_Service_Rating ==========%d++++++++++++++++++++++++++++++++++++++++\n",parent_rate);
	}
	else
	{
		parent_rate = 0;
		
		pbiinfo("\App_Get_Service_Rating ===NOTGET====++++++++++++++++++++++++++++++++++++++++\n");
	}

	return parent_rate;
}

/******************************************************************************
* Function : App_Get_event_Info_For_Dialog
* parameters :	
*		pService(in) : 		频道信息
*		cur_date(in):		 	界面中选择的天数
		number(in):			界面中选中的某一个事件的序号
		show_buffer(out):		输出事件的详细信息
*		
* Return :	
*		成功: 0
*		失败: 1
*		
*		
* Description :
*		输入频道信息、当前天数、事件在页面中的序号，在now、next或是schedule里面找到第number个事件，
		将这个事件的详细信息，写入到输入的buffer里，输出事件详细信息的BUFFER

*		注意，显示完BUFFER内容后，释放内存
* Author : SCJ	2009/11/24
******************************************************************************/
U32 App_Get_event_Info_For_Dialog(const U16 serv_id,const U16 ts_id,const U16 on_id, U8 flag,
											U8 cur_date, U16 number, char *show_buffer,U16 *buflen)
{
	EVENT 				*ptEvent = NULL;
	ext_eit				*ptExtend_Event = NULL;
	char					*ptcTemp = NULL;
	U32					uiI = 0,
						uiJ = 0,
						uiStrlength = 0;
	Extended_Items		*ptExtItems = NULL;
		
	if(show_buffer==NULL)
	{
		pbiinfo(("\n show_buffer  NULL\n"));
		return 1;
	}
	
	if( cur_date > 6 )
	{
		pbiinfo(("748cur_date > 6\n"));
		return 1;
	}
	if( (short)number <= 0 )
	{
		pbiinfo(("753number <= 0\n"));
		return 1;
	}	
	if(EPG_Try_Lock(200) == 0)
	{
		ptEvent = App_Get_Event_Info( serv_id,ts_id,on_id, cur_date, number );

		if( NULL == ptEvent )
		{
			EPG_Unlock();
			//printf("763NULL == ptEvent \n");
			pbiinfo(("763NULL == ptEvent \n"));
			return 1;
		}
			
		if( NULL != ptEvent->short_epg->short_event )
		{
			if( 0 != ptEvent->short_epg->short_length )
			{
				uiStrlength += ptEvent->short_epg->short_length + 1;
				ptcTemp = ( char * )OS_MemAllocate(system_p, uiStrlength);
				
				if( NULL == ptcTemp )
				{
					//printf("776NULL == ptcTemp\n");
					pbiinfo(("776NULL == ptcTemp\n"));
					return 1;
				}
				else
				{
					memset( ptcTemp, 0, uiStrlength );
					for( uiI = 0; uiI < ( ptEvent->short_epg->short_length + 1 ); uiI++ )
					{
						ptcTemp[uiJ] = ptEvent->short_epg->short_event[uiI];
						if( ptcTemp[uiJ]  == '\0' )
						{
							break;
						}
						if( ptcTemp[uiJ] == '\x8a')
						{
							ptcTemp[uiJ] = '\n';
							while( ptEvent->short_epg->short_event[ uiI+1 ] == '\x8a' )
							{
								uiI++;
							}
						}
						uiJ++;
					}
				}
				
				pbiinfo("802 short_epg = %s\n", ptEvent->short_epg->short_event);
			}
		}
		
		if( NULL != ptEvent->extended_epg )
		{
			ptExtend_Event = ptEvent->extended_epg;

			while( NULL != ptExtend_Event )
			{
				ptExtItems = ptExtend_Event->ExtItems;
				while( NULL != ptExtItems )
				{
					if( NULL != ptExtItems->pcItems )
					{
						uiStrlength += ptExtItems->usItems_Lenth + 1;

						ptcTemp = ( char * )OS_MemReallocate( system_p, ptcTemp, uiStrlength );
						if( NULL == ptcTemp )
						{
							return 1;
						}
						else
						{
							for( uiI = 0; uiI < ( ptExtItems->usItems_Lenth + 1 ); uiI++ )
							{
								ptcTemp[uiJ] = ptExtItems->pcItems[uiI];
								if( ptcTemp[uiJ]  == '\0' )
								{
									break;
								}

								if( ptcTemp[uiJ] == '\x8a')
								{
									ptcTemp[uiJ] = '\n';
									while( ptExtItems->pcItems[uiI+1] == '\x8a' )
									{
										uiI++;
									}
								}
								uiJ++;
							}
						}
					pbiinfo("ptExtItems->pcItems = %s\n", ptExtItems->pcItems);
					}
					ptExtItems = ptExtItems->next;
				}
				

				if( NULL != ptExtend_Event->text )
				{
					uiStrlength += ptExtend_Event->text_length + 1;

					ptcTemp = ( char * )OS_MemReallocate( system_p, ptcTemp, uiStrlength );
					if( NULL == ptcTemp )
					{
						//printf("8555 NULL == ptcTemp\n");
						pbiinfo(("8555 NULL == ptcTemp\n"));
						return 1;
					}				
					else
					{
						for( uiI = 0; uiI < ( ptExtend_Event->text_length + 1 ); uiI++ )
						{
							ptcTemp[uiJ] = ptExtend_Event->text[uiJ];
							if( ptcTemp[uiJ]  == '\0' )
							{
								break;
							}
							if( ptcTemp[uiJ] == '\x8a')
							{
								ptcTemp[uiJ] = '\n';
								while( ptExtend_Event->text[uiI+1] == '\x8a' )
								{
									uiI++;
								}
							}
							
							uiJ++;
						}
					}
					pbiinfo("879 ptExtend_Event->text  = %s\n", ptExtend_Event->text );
				}
				
				ptExtend_Event = ptExtend_Event->next;
			}
		}
		
		EPG_Unlock();

		if( NULL != ptcTemp)
		{
			ptcTemp[uiJ] =  '\0';
			if(*buflen <uiJ)
			{
				memcpy(show_buffer,ptcTemp,*buflen);
			}
			else
			{
				memcpy(show_buffer,ptcTemp,uiJ);				
				*buflen =uiJ;

			}
			OS_MemDeallocate(system_p,ptcTemp);
			return 0;
		}
		else
		{
			pbiinfo("NULL == ptcTemp\n");
			return 1;
		}	
	}
	else
	{
		return 1;
	}
	return 0;

	
}


/******************************************************************************
* Function : App_Get_Pf_Event
* parameters :	
*		pService(in) : 频道信息
*		flag(in):		  当前后续标志(0当前 1后续)	
*		
* Return :	
*		成功: P或F的event指针
*		失败: NULL
*		
*		
* Description :
*		输入一个节目信息指针、当前后续标志，输出当前或是后续事件信息指针
*
* Author : SCJ	2009/11/23
******************************************************************************/
EVENT *App_Get_Pf_Event(const U16 serv_id,const U16 ts_id,const U16 on_id, U8 flag)
{
	EVENT_LIST	*ptEventList=NULL;
	EVENT_LIST	*ptTmpEventNode = NULL;
	EVENT		*ptEvent=NULL;

	if( ( PRESENT_EVENT_FLAG != flag )&&( FOLLOW_EVENT_FLAG != flag ) )
	{
	
		pbiinfo("%s   %d  error %d %d %d  flag \n",__FUNCTION__,__LINE__,serv_id,ts_id,on_id,flag);
		return NULL;
	}
	
	ptEventList = Get_PF_Events( serv_id, ts_id, on_id, flag );

	if( NULL != ptEventList )
	{
		ptEvent = ptEventList->event;
	}
	else
	{
	//	pbiinfo("%s   %d  error %d %d %d  flag \n",__FUNCTION__,__LINE__,serv_id,ts_id,on_id,flag);
	}

	while( NULL != ptEventList )
	{
		ptTmpEventNode = ptEventList->next;
		OS_MemDeallocate(system_p, ptEventList );
		ptEventList = ptTmpEventNode;
	}

	
	//pbiinfo("\n %s  line %d eventid %d  starttime [%x:%x]===NOTGET====++++++++++++++++++++++++++++++++++++++++\n",
		//__FUNCTION__,__LINE__,ptEvent->event_id,ptEvent->start_time.mjd_date,ptEvent->start_time.bcd_time);
	return ptEvent;
}



/******************************************************************************
* Function : App_Get_Event_Info
* parameters :	
*		pService(in) : 		频道信息
*		cur_date(in):		 	界面中选择的天数(周一到周日)
		number(in):			界面中选中的某一个事件的序号
*		
* Return :	
*		成功: event 通过parse_eit，保存在内存中的event信息
*		失败: NULL
*		
*		
* Description :
*		通过输入节目信息、第几天、和在当前索引的第几个，在now、next或是schedule里面找到第number个
		匹配的事件，返回这个事件的指针
*
* Author : SCJ	2009/11/24
******************************************************************************/
EVENT *App_Get_Event_Info(const U16 serv_id,const U16 ts_id,const U16 on_id,U8 cur_date, U16 number)
{
	time_td_t			tStart_Time,
					tLocal_Time;
	EVENT			*ptCurEvent = NULL, 
					*ptNowEvent = NULL, 
					*ptNextEvent = NULL;
	//EVENT_SCHEDULE 	*ptSchedule = NULL;
	//EIT_TABLE		*ptCurTable = NULL;
	//EIT_SECTION		*ptCurSection = NULL;
	U32				uiCur_event_number = 0,
					//uiI = 0,
					uiJ = 0,
					schedule_total_number = 0;
	time_orig_t 		cmp_utc_time;
	EVENT_LIST		*ptEvent_list=NULL;
	


	if( cur_date > 6 ) 
	{
		pbiinfo("param error  %d   param =%d \n",__LINE__,cur_date);
		return NULL;
	}

	if( (short)number <= 0 )
	{
		pbiinfo("param error  %d   param =%d \n",__LINE__,number);
		return NULL;	
	}


	ptNowEvent = App_Get_Pf_Event( serv_id,ts_id,on_id,PRESENT_EVENT_FLAG );
	ptNextEvent = App_Get_Pf_Event( serv_id,ts_id,on_id, FOLLOW_EVENT_FLAG );
	tLocal_Time = Get_Local_Td_time();

	if( ( ptNowEvent != NULL )
		&&( _CmpEventEndTime( ptNowEvent, tLocal_Time ) )
		&&( 0 == cur_date ) )/*检查当前时间是否过期*/
	{
		uiCur_event_number++;
		if( uiCur_event_number == number )/*判断是否是符合条件的事件，如果符合，写入数组中*/
		{
			
			//pbiinfo("\n %s	line %d eventid %d	starttime [%x:%x]===NOTGET====++++++++++++++++++++++++++++++++++++++++\n",
				//__FUNCTION__,__LINE__,ptNowEvent->event_id,ptNowEvent->start_time.mjd_date,ptNowEvent->start_time.bcd_time);
			return ptNowEvent;
		}
			
	}
	else
	{
		ptNowEvent =NULL;

	}
	if( ( ptNextEvent != NULL )
		&&( _CmpEventEndTime( ptNextEvent, tLocal_Time ) )
		&&( 0 == cur_date ) )
	{
		if(  ( ptNowEvent != NULL )
			&&( CMP_EVENT_UTC_L( ptNowEvent,ptNextEvent ) ) )/*判断是否与NOW重复*/
		{
				App_Get_Event_Start_Time( &tStart_Time, ptNextEvent );

			/*处理后续时间不在当天的情况*/
			if( ( tStart_Time.year == tLocal_Time.year )
				&&( tStart_Time.month == tLocal_Time.month )
				&&( tStart_Time.date== tLocal_Time.date ) )
			{
				uiCur_event_number++;
				if( uiCur_event_number == number )/*判断是否是符合条件的事件，如果符合，写入数组中*/
				{
				
					//pbiinfo("\n %s	line %d eventid %d	starttime [%x:%x]===NOTGET====++++++++++++++++++++++++++++++++++++++++\n",
					//	__FUNCTION__,__LINE__,ptNextEvent->event_id,ptNextEvent->start_time.mjd_date,ptNextEvent->start_time.bcd_time);
					return ptNextEvent;
				}	
			
			}
			else
			{
				pbiinfo("epg---line %d \n",__LINE__);	
				ptNextEvent =NULL;

			}
		}
		else
		{
			pbiinfo("epg---line %d \n",__LINE__);	
			ptNextEvent =NULL;

		}
	}	
	else
	{
		pbiinfo("epg---line %d \n",__LINE__);	
		ptNextEvent =NULL;

	}
	if( 0 == cur_date )/*处理当天的EPG需要判断SCHEDULE里面是否有和P\F重复的事件*/
	{
		cmp_utc_time = Get_Local_Time();
		schedule_total_number = Get_Sche_Events_NumOfDay( on_id, ts_id, serv_id, 
								cmp_utc_time.mjd_date, 0 );

	
		for( uiJ = 0; uiJ < schedule_total_number; uiJ++ )
		{
			ptEvent_list= Get_Sche_Events_OfDay(on_id, ts_id, serv_id,
								cmp_utc_time.mjd_date, 0, uiJ, 1 );

			if( !ptEvent_list )
				continue;
			
			ptCurEvent = ptEvent_list->event;
			OS_MemDeallocate(system_p, ptEvent_list);
			
			if( !ptCurEvent )
				continue;

			/*判断事件是否过期，并与now和next不重复*/
			if( _CmpEventEndTime( ptCurEvent, tLocal_Time ) 
				&&( CMP_EVENT_UTC_L( ptNowEvent,ptCurEvent ) ) 
				&&( CMP_EVENT_UTC_L( ptNextEvent,ptCurEvent ) ) )
			{
				uiCur_event_number++;
				if( uiCur_event_number == number )/*判断是否是符合条件的事件，如果符合，写入数组中*/
				{
					
					//pbiinfo("\n %s	line %d eventid %d	starttime [%x:%x]===NOTGET====++++++++++++++++++++++++++++++++++++++++\n",
						//__FUNCTION__,__LINE__,ptCurEvent->event_id,ptCurEvent->start_time.mjd_date,ptCurEvent->start_time.bcd_time);
					return ptCurEvent;
				}	
			}		
		}
	}
	else
	{
		cmp_utc_time.mjd_date = Time_Convert_YMD_to_MJD(  tLocal_Time.year,
											tLocal_Time.month, ( tLocal_Time.date + cur_date ) );
		
		

		ptEvent_list= Get_Sche_Events_OfDay( on_id, ts_id, serv_id,
						cmp_utc_time.mjd_date,0,number-1,1);

		if( ptEvent_list != NULL )
		{
			ptCurEvent = ptEvent_list->event;
			OS_MemDeallocate(system_p, ptEvent_list);
			pbiinfo("epg---line %d \n",__LINE__);	

			
			//pbiinfo("\n %s	line %d eventid %d	starttime [%x:%x]=======++++++++++++++++++++++++++++++++++++++++\n",
				//__FUNCTION__,__LINE__,ptCurEvent->event_id,ptCurEvent->start_time.mjd_date,ptCurEvent->start_time.bcd_time);
			
			return ptCurEvent;
		}
	}
	return NULL;
}

/******************************************************************************
* Function : App_Get_Event_List_Info
* parameters :	
*		pService(in) : 		频道信息
*		cur_date(in):		 	界面中选择的天数
*		number(in):			界面中选中的某一个事件的序号
*		one_page_max(in):	界面中一页显示的最大事件数
*		peventlist(out):		通过parse_eit，保存在内存中的event信息，输出
*		total_number(out):		在EIT中查找符合条件的事件后计算得出
*		
* Return :	
*		成功: 0 有对应的事件
*		失败: 1 没有对应的事件
*		
*		
* Description :
*		通过输入节目信息、第几天、当前EPG页面的第一个事件的编号，event数组的头指针，数组的长度
		，在now、next或是schedule找到符合条件的事件，写入到输入的数组中，并计算节目的总数，输出一
		个符合条件的event数组，输出符合条件所有事件的总数
*
* Author : SCJ	2009/11/25
******************************************************************************/
U32 App_Get_Event_List_Info( const U16 serv_id,const U16 ts_id,const U16 on_id, U8 cur_date, U16 number, 
										U16 one_page_max, EVENT *peventlist, U16 *total_number )
{
	time_td_t			tStart_Time,
					tLocal_Time;
	EVENT			*ptCurEvent = NULL, 
					*ptNowEvent = NULL, 
					*ptNextEvent = NULL;
	//EVENT_SCHEDULE 	*ptSchedule = NULL;
	//EIT_TABLE		*ptCurTable = NULL;
	//EIT_SECTION		*ptCurSection = NULL;
	U32				uiCur_event_number = 0,
					uiI = 0,
					uiJ = 0,
					schedule_total_number = 0;
	time_orig_t 		cmp_utc_time;
	EVENT_LIST		*ptEvent_list=NULL;
	

	if( cur_date > 6 )
	{
		pbiinfo("param error  %d   param =%d \n",__LINE__,cur_date);
		return 1;
	}

	if( (short)number <= 0 )
	{
		pbiinfo("param error  %d   param =%d \n",__LINE__,number);
		return 1;
	}
	
	if( NULL == peventlist )
	{
		
		pbiinfo("param error  %d    \n",__LINE__);
		return 1;
	}

	if( NULL == total_number )
	{
		
		pbiinfo("param error  %d  \n",__LINE__);
		return 1;
	}
	

	memset( peventlist, 0, one_page_max * sizeof( EVENT ) );
	*total_number = 0;
		

	ptNowEvent = App_Get_Pf_Event( serv_id,ts_id,on_id, PRESENT_EVENT_FLAG );
	ptNextEvent = App_Get_Pf_Event( serv_id,ts_id,on_id, FOLLOW_EVENT_FLAG );
	tLocal_Time = Get_Local_Td_time();

	if( ( ptNowEvent != NULL )
		&&( _CmpEventEndTime( ptNowEvent, tLocal_Time ) )
		&&( 0 == cur_date ) )/*检查当前时间是否过期*/
	{
		uiCur_event_number++;
		if( uiCur_event_number == number )/*判断是否是符合条件的事件，如果符合，写入数组中*/
		{
			if( uiI <= one_page_max )
			{
				peventlist[uiI] = *ptNowEvent;
				uiI++;
			}
		}
			
	}
	else
	{
		ptNowEvent=NULL;
		//pbiinfo(" %s  error line %d \n",__FILE__,__LINE__);
	}

	if( ( ptNextEvent != NULL )
		&&( _CmpEventEndTime( ptNextEvent, tLocal_Time ) )
		&&( 0 == cur_date ) )
	{
		if(  ( ptNowEvent != NULL )
			&&( CMP_EVENT_UTC_L( ptNowEvent,ptNextEvent ) ) )/*判断是否与NOW重复*/
		{
				App_Get_Event_Start_Time( &tStart_Time, ptNextEvent );

			/*处理后续时间不在当天的情况*/
			if( ( tStart_Time.year == tLocal_Time.year )
				&&( tStart_Time.month == tLocal_Time.month )
				&&( tStart_Time.date== tLocal_Time.date ) )
			{
				//(*total_number)++;
				uiCur_event_number++;
				if( uiCur_event_number >= number )/*判断是否是符合条件的事件，如果符合，写入数组中*/
				{
					if( uiI <  one_page_max )
					{
						peventlist[uiI] = *ptNextEvent;
						uiI++;
					}
				}	
			}
			else
			{
				ptNextEvent =NULL;

			}
		}
		else
		{
			ptNextEvent =NULL;
		}
		
	}
	else
	{
		ptNextEvent =NULL;
	}
	*total_number = uiCur_event_number;
#if 1	
	if( 0 == cur_date )/*处理当天的EPG需要判断SCHEDULE里面是否有和P\F重复的事件*/
	{
		cmp_utc_time = Get_Local_Time();

		schedule_total_number = Get_Sche_Events_NumOfDay(on_id, ts_id, serv_id, 
								cmp_utc_time.mjd_date, cmp_utc_time.bcd_time );

		for( uiJ = 0; uiJ < schedule_total_number; uiJ++ )
		{
			ptEvent_list= Get_Sche_Events_OfDay( on_id, ts_id, serv_id,
								cmp_utc_time.mjd_date, cmp_utc_time.bcd_time, uiJ, 1 );

			if( !ptEvent_list )
			{
				continue;
			}
#if 0
                    U16 year = 0;
                    U16 month = 0;
                    U16 date = 0;
                    U16 hour = 0;
                    U16 min = 0;
                    U16 sec = 0;
                    Time_Convert_MJD_to_YMD(ptEvent_list->event->start_time.mjd_date,&year,&month,&date);
                    Time_Convert_BCD_to_HMS(ptEvent_list->event->start_time.bcd_time, &hour, &min, &sec);
                     pbiinfo("[%s %d]----event id %d----[%d-%d-%d]----[%d:%d:%d]-------\n", DEBUG_LOG, ptEvent_list->event->event_id, year, month, date, hour, min, sec );
#endif                        
			ptCurEvent = ptEvent_list->event;
			OS_MemDeallocate(system_p, ptEvent_list);
			
			if( !ptCurEvent )
			{
				continue;
			}

			/*判断事件是否过期，并与now和next不重复*/
			if( _CmpEventEndTime( ptCurEvent, tLocal_Time ) 
				&& ( CMP_EVENT_UTC_L( ptNowEvent,ptCurEvent ) ) 
				&& ( CMP_EVENT_UTC_L( ptNextEvent,ptCurEvent ) ))
			{
				//(*total_number)++;
				uiCur_event_number++;
				if( uiCur_event_number >= number )/*判断是否是符合条件的事件，如果符合，写入数组中*/
				{
					if( uiI  < one_page_max )
					{
						peventlist[uiI] = *ptCurEvent;
						uiI++;
					}
				}	
			}
		}
		*total_number = uiCur_event_number;
	}
	else
	{

		cmp_utc_time.mjd_date = Time_Convert_YMD_to_MJD(  tLocal_Time.year,
											tLocal_Time.month, ( tLocal_Time.date + cur_date ) );
		
		schedule_total_number = Get_Sche_Events_NumOfDay( on_id, ts_id, serv_id,
							cmp_utc_time.mjd_date, 0 );

		for( uiJ = number; uiJ < one_page_max + number; uiJ++ )
		{

			ptEvent_list= Get_Sche_Events_OfDay( on_id, ts_id, serv_id,
							cmp_utc_time.mjd_date,0,uiJ-1,1);

			if( !ptEvent_list )
				continue;

			ptCurEvent = ptEvent_list->event;
			OS_MemDeallocate(system_p, ptEvent_list);

			if( !ptCurEvent )
				continue;
			
			
			if( uiI < one_page_max )
			{
				peventlist[uiI] = *ptCurEvent;
				uiI++;
			}			
		}
		*total_number = schedule_total_number;
	}
	#endif
	return ( uiI == 0 )?( 1 ):( 0 );
}


U32 App_Get_Event_List_InfoByNibble( const U16 serv_id,const U16 ts_id,const U16 on_id, U8 cur_date, U16 number, 
										U16 one_page_max, EVENT *peventlist, U16 *total_number ,U16 nibble)
{
	time_td_t			tStart_Time,
					tLocal_Time;
	EVENT			*ptCurEvent = NULL, 
					*ptNowEvent = NULL, 
					*ptNextEvent = NULL;
	//EVENT_SCHEDULE 	*ptSchedule = NULL;
	//EIT_TABLE		*ptCurTable = NULL;
	//EIT_SECTION		*ptCurSection = NULL;
	U32				uiCur_event_number = 0,
					uiI = 0,
					uiJ = 0,
					schedule_total_number = 0;
	time_orig_t 		cmp_utc_time;
	EVENT_LIST		*ptEvent_list=NULL;
	U16    nibblevalue =nibble<<12;

	if( cur_date > 6 )
	{
		pbiinfo("param error  %d   param =%d \n",__LINE__,cur_date);
		return 1;
	}

	if( (short)number <= 0 )
	{
		pbiinfo("param error  %d   param =%d \n",__LINE__,number);
		return 1;
	}
	
	if( NULL == peventlist )
	{
		
		pbiinfo("param error  %d    \n",__LINE__);
		return 1;
	}

	if( NULL == total_number )
	{
		
		pbiinfo("param error  %d  \n",__LINE__);
		return 1;
	}
	

	memset( peventlist, 0, one_page_max * sizeof( EVENT ) );
	*total_number = 0;
		

	ptNowEvent = App_Get_Pf_Event( serv_id,ts_id,on_id, PRESENT_EVENT_FLAG );
	ptNextEvent = App_Get_Pf_Event( serv_id,ts_id,on_id, FOLLOW_EVENT_FLAG );
	tLocal_Time = Get_Local_Td_time();

	if( ( ptNowEvent != NULL )
		&&( _CmpEventEndTime( ptNowEvent, tLocal_Time ) )
		&&( 0 == cur_date ) )/*检查当前时间是否过期*/
	{
		if(ptNowEvent->content_nibble_level ==nibblevalue)
		{
			uiCur_event_number++;
			if( uiCur_event_number == number )/*判断是否是符合条件的事件，如果符合，写入数组中*/
			{
				if( uiI <= one_page_max )
				{
					peventlist[uiI] = *ptNowEvent;
					uiI++;
				}
			}
		}
			
	}
	else
	{
		ptNowEvent=NULL;
		//pbiinfo(" %s  error line %d \n",__FILE__,__LINE__);
	}

	if( ( ptNextEvent != NULL )
		&&( _CmpEventEndTime( ptNextEvent, tLocal_Time ) )
		&&( 0 == cur_date ) 
		&&(ptNextEvent->content_nibble_level ==nibblevalue))
	{
		if(  ( ptNowEvent != NULL )
			&&( CMP_EVENT_UTC_L( ptNowEvent,ptNextEvent ) ) )/*判断是否与NOW重复*/
		{
				App_Get_Event_Start_Time( &tStart_Time, ptNextEvent );

			/*处理后续时间不在当天的情况*/
			if( ( tStart_Time.year == tLocal_Time.year )
				&&( tStart_Time.month == tLocal_Time.month )
				&&( tStart_Time.date== tLocal_Time.date ) )
			{
				//(*total_number)++;
				uiCur_event_number++;
				if( uiCur_event_number >= number )/*判断是否是符合条件的事件，如果符合，写入数组中*/
				{
					if( uiI <  one_page_max )
					{
						peventlist[uiI] = *ptNextEvent;
						uiI++;
					}
				}	
			}
			else
			{
				ptNextEvent =NULL;

			}
		}
		else
		{
			ptNextEvent =NULL;
		}
		
	}
	else
	{
		ptNextEvent =NULL;
	}
	*total_number = uiCur_event_number;
#if 1	
	if( 0 == cur_date )/*处理当天的EPG需要判断SCHEDULE里面是否有和P\F重复的事件*/
	{
		cmp_utc_time = Get_Local_Time();

		schedule_total_number = Get_Sche_Events_NumOfDay(on_id, ts_id, serv_id, 
								cmp_utc_time.mjd_date, cmp_utc_time.bcd_time );

		for( uiJ = 0; uiJ < schedule_total_number; uiJ++ )
		{
			ptEvent_list= Get_Sche_Events_OfDay( on_id, ts_id, serv_id,
								cmp_utc_time.mjd_date, cmp_utc_time.bcd_time, uiJ, 1 );

			if( !ptEvent_list )
			{
				continue;
			}
#if 0
                    U16 year = 0;
                    U16 month = 0;
                    U16 date = 0;
                    U16 hour = 0;
                    U16 min = 0;
                    U16 sec = 0;
                    Time_Convert_MJD_to_YMD(ptEvent_list->event->start_time.mjd_date,&year,&month,&date);
                    Time_Convert_BCD_to_HMS(ptEvent_list->event->start_time.bcd_time, &hour, &min, &sec);
                     pbiinfo("[%s %d]----event id %d----[%d-%d-%d]----[%d:%d:%d]-------\n", DEBUG_LOG, ptEvent_list->event->event_id, year, month, date, hour, min, sec );
#endif                        
			ptCurEvent = ptEvent_list->event;
			OS_MemDeallocate(system_p, ptEvent_list);
			
			if( !ptCurEvent )
			{
				continue;
			}
			
			if(ptCurEvent->content_nibble_level !=nibblevalue)
				continue;
			/*判断事件是否过期，并与now和next不重复*/
			if( _CmpEventEndTime( ptCurEvent, tLocal_Time ) 
				&& ( CMP_EVENT_UTC_L( ptNowEvent,ptCurEvent ) ) 
				&& ( CMP_EVENT_UTC_L( ptNextEvent,ptCurEvent ) ))
			{
				//(*total_number)++;
				uiCur_event_number++;
				if( uiCur_event_number >= number )/*判断是否是符合条件的事件，如果符合，写入数组中*/
				{
					if( uiI  < one_page_max )
					{
						peventlist[uiI] = *ptCurEvent;
						uiI++;
					}
				}	
			}
		}
		*total_number = uiCur_event_number;
	}
	else
	{

		cmp_utc_time.mjd_date = Time_Convert_YMD_to_MJD(  tLocal_Time.year,
											tLocal_Time.month, ( tLocal_Time.date + cur_date ) );
		
		schedule_total_number = Get_Sche_Events_NumOfDay( on_id, ts_id, serv_id,
							cmp_utc_time.mjd_date, 0 );

		for( uiJ = number; uiJ < one_page_max + number; uiJ++ )
		{

			ptEvent_list= Get_Sche_Events_OfDay( on_id, ts_id, serv_id,
							cmp_utc_time.mjd_date,0,uiJ-1,1);

			if( !ptEvent_list )
				continue;

			ptCurEvent = ptEvent_list->event;
			OS_MemDeallocate(system_p, ptEvent_list);

			if( !ptCurEvent )
				continue;
			if(ptCurEvent->content_nibble_level !=nibblevalue)
				continue;
			if( uiI < one_page_max )
			{
				peventlist[uiI] = *ptCurEvent;
				uiI++;
			}			
		}
		*total_number = schedule_total_number;
	}
	#endif
	return ( uiI == 0 )?( 1 ):( 0 );
}




