/*
 *	 epg.h					ver 1.0
 *
 * (c)	Copyright  Federal Technologies Limited	2001 - 2002
 *
 *      Source file name	: epg.h
 *
 *		  AUTHOR(S) NAME		: SKN Apr. 2001
 *
 *		  Original Work		: Definition for EIT Process.
 *
 * =======================
 * IMPROVEMENTS THOUGHT  OF
 * =======================
 *				None
 * ==============
 * Modification History
 * ==============
 * Date			Initials			Modification
 * ===================================
 *
*/

#ifndef __EPG_H_
#define __EPG_H_

#include "pvddefs.h"
#include "systime.h"


#ifdef __cplusplus
extern "C" {
#endif



#define EIT_MAX_BUFFER				(7*1024*1024)//modify by LSZ 2006-07-01



/*******FOR EPG EVENT LIST***********/
typedef struct TAG_PARENTAL_RATE
{
	char							country_code[3];
	U8							rate;
	struct TAG_PARENTAL_RATE	*next;
}PARENTAL_RATE;

typedef struct TAG_SHORT_EPG
{
	char		        			ISO_639_language_code[3];
	U8						name_length;
	char						*event_name;
	U8						short_length;
	char						*short_event;	
	struct TAG_SHORT_EPG	*next;
}EVENT_TEXT;

    typedef struct extended_Items
    {
        U16              			usItemsName_Lenth;
        char 				*pcItemsName;
        U16              			usItems_Lenth;
        char 				*pcItems;
        struct extended_Items	*next;
    } Extended_Items;

    typedef struct extended_EVENT
    {
        U16              			descriptor_number;
        U16             			last_descriptor_number;
        char		        		ISO_639_language_code[3];
        Extended_Items		*ExtItems;
        U16              			text_length;
        char 				*text;
        struct extended_EVENT *next;
    } ext_eit;

typedef struct eit_SECTION
{	
	U16								on_id;
	U16								ts_id;
	U16								service_id;
	U8 								table_id;
	U16						section_number;
	U8						version_number;	
	U32						section_crc;//liqian 20110614同步云南filter死机修改
	struct TAG_EVENT			*head;
}eit_section;

typedef struct TAG_EVENT
{
	U16						event_id;
	time_orig_t				start_time;
	char					duration[3];
	U16						content_nibble_level;	/*high 8bit is nibble1 and nibble2, low 8bit is user_nibble
													ADD BY DGF 20060505*/
	struct TAG_PARENTAL_RATE	*parental_rating;
#if NVOD_USED
	U16						ref_serv_id;
	U16						ref_event_id;
#endif
	struct TAG_SHORT_EPG	*short_epg;
	struct extended_EVENT		*extended_epg;	
	struct eit_SECTION			*section;
	struct TAG_EVENT			*next;
}EVENT;

typedef struct TAG_EPG_EVENT_LIST
{
	struct 	TAG_EVENT			*event;
	struct	TAG_EPG_EVENT_LIST	*prev;
	struct	TAG_EPG_EVENT_LIST	*next;
}EVENT_LIST;


//typedef void (* EpgNotifyFunc)(eit_section * new_sect);
typedef void (* EpgNotifyFunc)(U16 sid,U16 tsid,U16 onid);


S32  EPG_Init(EpgNotifyFunc epg_notify_func);
void EPG_Start(void);
void EPG_Stop(void);
void EPG_Pause(void);
void EPG_Resume(void);

S32  EPG_Try_Lock(U32 milisecond);
void EPG_Lock(void);
void EPG_Unlock(void);
void EPG_Register_Callback(EpgNotifyFunc epg_notify_func);

EVENT_LIST * Get_PF_Events
(
	U16 serv_id,
	U16 ts_id,
	U16 on_id,
	U8  flag
);

U32	Get_Sche_Events_NumOfDay
(
	U16 on_id,
	U16 ts_id,
	U16 serv_id,
	U32 mjd_date,
	U32 bcd_time
);

EVENT_LIST * Get_Sche_Events_OfDay
(
	U16 on_id,
	U16 ts_id,
	U16 serv_id,
	U32 mjd_date,
	U32 bcd_time,
	U32 offset,
	U32 num
);


#ifdef __cplusplus
}
#endif

#endif	/* __EPG_H_ */

