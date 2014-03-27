/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name : dialog_epg_common.h
* Description : EPG api
* History :
*	Date               Modification                                Name
*	----------         ------------                     ----------
*	2006/05/21         Created                                 LSZ
*	2006/05/22	      Modified					 DGF
******************************************************************************/
#ifndef 	Epg_Common_H
#define 	Epg_Common_H

#ifdef __cplusplus
extern "C" {
#endif
#include "systime.h"
#include "epg.h"

#define PRESENT_EVENT_FLAG								0
#define FOLLOW_EVENT_FLAG								1


U32 App_Get_Event_Start_Time(time_td_t *time_date, const EVENT *pEvent);
U32 App_Get_Event_End_Time(time_td_t *time_date, const EVENT *pEvent);

U16 App_time_compare_all_param( time_td_t time1, time_td_t time2);
EVENT *App_Get_Pf_Event(const U16 serv_id,const U16 ts_id,const U16 on_id, U8 flag);
EVENT *App_Get_Event_Info(const U16 serv_id,const U16 ts_id,const U16 on_id,U8 cur_date, U16 number);

void App_Register_cb(EpgNotifyFunc epg_notify_func);
U8 App_Get_Service_Rating(const U16 serv_id,const U16 ts_id,const U16 on_id);
U32 App_Get_event_Info_For_Dialog(const U16 serv_id,const U16 ts_id,const U16 on_id, U8 flag,
											U8 cur_date, U16 number, char *show_buffer,U16 *buflen);

U32 App_Get_Event_Cur_Time(time_td_t *time_date, const EVENT *pEvent);
U32 App_Get_Event_List_Info( const U16 serv_id,const U16 ts_id,const U16 on_id, U8 cur_date, U16 number, 
										U16 one_page_max, EVENT *peventlist, U16 *total_number );
U32 App_Get_Event_List_InfoByNibble( const U16 serv_id,const U16 ts_id,const U16 on_id, U8 cur_date, U16 number, 
										U16 one_page_max, EVENT *peventlist, U16 *total_number ,U16 nibble);


#ifdef __cplusplus
}
#endif

#endif

