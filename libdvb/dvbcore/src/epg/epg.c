#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "pvddefs.h"
#include "osapi.h"

//#include "descriptors.h"
//#include "table.h"

#include "psi_si.h"
#include "background.h"

#include "glib.h"
#include "epg.h"
#include "pbitrace.h"


#if 0
#define FILE_TRACE_ENABLE (1)
#if (PBI_TRACE_ENABLE && FILE_TRACE_ENABLE)
#include "pbi_trace.h"
PBI_TRACE_SET_MODULE(PVC);
#else
#include "pbi_trace_dummy.h"
#endif

#endif



/************MACRO DEFINE***********************************/
#define EIT_TABLE_MAX_LENGTH			(4096)
#define DATA_BUFFER_SIZE				(EIT_TABLE_MAX_LENGTH)
#define EIT_BUFFER_SIZE					(EIT_TABLE_MAX_LENGTH * 2)


/*********GLOBAL PUBLIC VARIABLE***********************************/


static U32 eit_size = 0;
static U32 eit_schedule_size = 0;

static U32 pf_number = 0;
static U32 schedule_number = 0;

static U32 valid_section_number_pf = 0;
static U32 nouse_section_number_pf = 0;

static U32 valid_section_number = 0;
static U32 nouse_section_number = 0;

static GTree *schedule_section_tree = NULL;
static GTree *schedule_event_tree = NULL;
static GTree *pf_section_tree = NULL;
static GTree *pf_event_tree = NULL;


static void *_EITBuf_Malloc( U32 nSizeOfBytes );
static void _EITBuf_Free( void *pMem );

static void _release_eit_event_rating(PARENTAL_RATE *rating);
static void _release_eit_event_text(EVENT_TEXT *text);
static void _release_eit_extended_event(ext_eit *extended);
static void _release_eit_event(EVENT *node);
static void _release_schedule_event_rating(PARENTAL_RATE *rating);
static void _release_schedule_event_text(EVENT_TEXT *text);
static void _release_schedule_extended_event(ext_eit *extended);
static void _release_schedule_event(EVENT *node);


static U8 _Parse_Extended_Items(Extended_Items *ptExtItems,
                                U8 *pucCase, U32 *puiSize);
static void _Free_Extended_Items(Extended_Items *ptExtItems,
                                 U32 *puiSize);

static U8 _Parse_Extended_Items_List(ext_eit *ptExtend_EPG,
                                     U8 *pucCase, U32 *puiSize);
static U8 _Free_Extended_Items_List( Extended_Items *ptExtItems, U32 *puiSize );

static U8 _Parse_Extended_Event( ext_eit *ptExtend_EPG, U8 *pucCase, U32 *puiSize );
static void _Free_Extended_Event( ext_eit *ptExtend_EPG, U32 *puiSize );

static U32 _Parse_Short_Event( EVENT_TEXT *ptShort_Event, U8 *pucCase, U32 *puiSize );
static void _Free_Short_Event( EVENT_TEXT *ptShort_EPG, U32 *puiSize );

static U32 _Parse_Parental_Rate( PARENTAL_RATE **ptRate, U8 *pucCase, U32 *puiSize );
static void _Free_Parental_Rate( PARENTAL_RATE *ptParentalRate, U32 *puiSize );

static U32 _Parse_Event( EVENT *ptEvent, U8 *pucEvent, U32 *puiSize, int *puiDescLen);
static void _Free_Event( EVENT *ptEvent, U32 *puiSize );

static gint __eit_comparefunc_normal(gconstpointer a, gconstpointer b);  //compare by original_network_id,transport_stream_id,service_id,table_id,start_time in order
static gint __eit_searchfunc_schedule_num(gpointer key,gpointer data);
static gint __eit_auxiliaryfunc_normal(gpointer value, gpointer data);
static gint __eit_comparefunc_nibble(gconstpointer a, gconstpointer b);  //compare by content_nibble_level ,start_time ,service_id in order
static gint __eit_comparefunc_section(gconstpointer a, gconstpointer b);
static gint __eit_searchfunc_section_tableid(gpointer key,gpointer data);
static gint __eit_searchfunc_section(gpointer key,gpointer data);
static gint __eit_comparefunc_pf(gconstpointer a, gconstpointer b);
static gint __eit_delsection_pf(gpointer key, gpointer value, gpointer data);
static gint __eit_del_event_pf(gpointer key, gpointer value, gpointer data);
static gint __eit_delsection_schedule(gpointer key, gpointer value, gpointer data);
static gint __eit_del_event_schedule(gpointer key, gpointer value, gpointer data);
static void __nomem_except_handle(void);



/********* LOCAL FUNCTIONS***********************************/
static void *_EITBuf_Malloc( U32 nSizeOfBytes )
{
    void* pRet = NULL;

    if( !nSizeOfBytes ) return NULL;

    pRet = OS_MemAllocate( system_p, nSizeOfBytes );/*yangweiqing modify 10-04-26*/
//	pRet = small_mem_allocate(nSizeOfBytes);
    return pRet;
}

static void _EITBuf_Free( void *pMem )
{
    if( !pMem ) return;

    OS_MemDeallocate( system_p, pMem );/*yangweiqing modify 10-04-26*/
//	small_mem_deallocate(pMem);
    return ;
}

static void _release_eit_event_rating(PARENTAL_RATE *rating)
{
    PARENTAL_RATE	*cur_rate=NULL,
                     *next_rate=NULL;

    cur_rate=rating;
    while(cur_rate!=NULL)
    {
        next_rate=cur_rate->next;
        _EITBuf_Free(cur_rate);
        cur_rate=NULL;
        eit_size-=sizeof(PARENTAL_RATE);
        //assert(eit_size>0);
        cur_rate=next_rate;
        next_rate=NULL;
    }
}
static void _release_eit_event_text(EVENT_TEXT *text)
{
    EVENT_TEXT		*cur_short=NULL,
                     *next_short=NULL;

    cur_short=text;
    while( cur_short != NULL )
    {
        if(cur_short->event_name!=NULL)
        {
            _EITBuf_Free(cur_short->event_name);
            cur_short->event_name=NULL;
            eit_size-=(cur_short->name_length+1);
            //assert(eit_size>=0);
            cur_short->event_name=NULL;
        }
        if(cur_short->short_event!=NULL)
        {
            _EITBuf_Free(cur_short->short_event);
            cur_short->short_event=NULL;
            eit_size-=(cur_short->short_length+1);
            //assert(eit_size>=0);
            cur_short->short_event=NULL;
        }
        next_short = cur_short->next;
        _EITBuf_Free(cur_short);
        cur_short=NULL;
        eit_size-=sizeof(EVENT_TEXT);
        //assert(eit_size>=0);
        cur_short = next_short;
    }
}

static void _release_eit_extended_event(ext_eit *extended)
{
    ext_eit		*cur_extend=NULL,
                 *next_extend=NULL;
    cur_extend=extended;
    while(cur_extend!=NULL)
    {
        if(cur_extend->ExtItems !=NULL)
        {
            _Free_Extended_Items_List(cur_extend->ExtItems, &eit_size);
            cur_extend->ExtItems=NULL;
        }
        if(cur_extend->text!=NULL)
        {
            _EITBuf_Free(cur_extend->text);
            cur_extend->text=NULL;
            eit_size-=(cur_extend->text_length+1);

        }
        next_extend=cur_extend->next;
        _EITBuf_Free(cur_extend);
        cur_extend=NULL;
        eit_size-=sizeof(ext_eit);
        //assert(eit_size>=0);
        cur_extend=next_extend;
    }
}

static void _release_eit_event(EVENT *node)
{
    if(node->parental_rating!=NULL)
    {
        _release_eit_event_rating(node->parental_rating);
    }
    if(node->short_epg!=NULL)
    {
        _release_eit_event_text(node->short_epg);
    }
    node->short_epg=NULL;
    if(node->extended_epg!=NULL)
    {
        _release_eit_extended_event(node->extended_epg);
    }
    node->extended_epg=NULL;
    _EITBuf_Free(node);
    eit_size-=sizeof(EVENT);

}



static void _release_schedule_event_rating(PARENTAL_RATE *rating)
{
    PARENTAL_RATE	*cur_rate=NULL,
                     *next_rate=NULL;

    cur_rate=rating;
    while(cur_rate!=NULL)
    {
        next_rate=cur_rate->next;
        _EITBuf_Free(cur_rate);
        cur_rate=NULL;
        eit_schedule_size-=sizeof(PARENTAL_RATE);
        cur_rate=next_rate;
        next_rate=NULL;
    }
}

static void _release_schedule_event_text(EVENT_TEXT *text)
{
    EVENT_TEXT		*cur_short=NULL,
                     *next_short=NULL;

    cur_short=text;
    while( cur_short != NULL )
    {
        if(cur_short->event_name!=NULL)
        {
            _EITBuf_Free(cur_short->event_name);
            cur_short->event_name=NULL;
            eit_schedule_size-=(cur_short->name_length+1);
        }
        if(cur_short->short_event!=NULL)
        {
            _EITBuf_Free(cur_short->short_event);
            cur_short->short_event=NULL;
            eit_schedule_size-=(cur_short->short_length+1);

        }
        next_short = cur_short->next;
        _EITBuf_Free(cur_short);
        cur_short=NULL;
        eit_schedule_size-=sizeof(EVENT_TEXT);
        cur_short = next_short;
    }
}
static void _release_schedule_extended_event(ext_eit *extended)
{
    ext_eit		*cur_extend=NULL,
                 *next_extend=NULL;
    cur_extend=extended;
    while(cur_extend!=NULL)
    {
        if(cur_extend->ExtItems!=NULL)
        {
            _Free_Extended_Items_List(cur_extend->ExtItems, &eit_schedule_size);
            cur_extend->ExtItems=NULL;
        }
        if(cur_extend->text!=NULL)
        {
            _EITBuf_Free(cur_extend->text);
            cur_extend->text=NULL;
            eit_schedule_size-=(cur_extend->text_length+1);
        }
        next_extend=cur_extend->next;
        _EITBuf_Free(cur_extend);
        cur_extend=NULL;
        eit_schedule_size-=sizeof(ext_eit);
        cur_extend=next_extend;
    }
}

static void _release_schedule_event(EVENT *node)
{
    if(node->parental_rating!=NULL)
    {
        _release_schedule_event_rating(node->parental_rating);
    }
    if(node->short_epg!=NULL)
    {
        _release_schedule_event_text(node->short_epg);
    }
    node->short_epg=NULL;
    if(node->extended_epg!=NULL)
    {
        _release_schedule_extended_event(node->extended_epg);
    }
    node->extended_epg=NULL;

    _EITBuf_Free(node);
    eit_schedule_size-=sizeof(EVENT);
    //assert(eit_schedule_size>0);
}


static gint __eit_comparefunc_normal(gconstpointer	a, gconstpointer	b)
{
    EVENT *new_event,*node_event;

    new_event = (EVENT *)a;
    node_event = (EVENT *)b;

    if(new_event->section->on_id > node_event->section->on_id)
    {
        return 1;
    }
    if(new_event->section->on_id < node_event->section->on_id)
    {
        return -1;
    }
    else
    {
        if(new_event->section->ts_id > node_event->section->ts_id)
        {
            return 1;
        }
        if(new_event->section->ts_id < node_event->section->ts_id)
        {
            return -1;
        }
        else
        {
            if(new_event->section->service_id> node_event->section->service_id)
            {
                return 1;
            }
            if(new_event->section->service_id < node_event->section->service_id)
            {
                return -1;
            }
            else
            {
                if(new_event->section->table_id > node_event->section->table_id)
                {
                    return 1;
                }
                if(new_event->section->table_id < node_event->section->table_id)
                {
                    return -1;
                }
                else
                {
                    if(new_event->start_time.mjd_date > node_event->start_time.mjd_date)
                    {
                        return 1;
                    }
                    if(new_event->start_time.mjd_date < node_event->start_time.mjd_date)
                    {
                        return -1;
                    }
                    else
                    {
                        if(new_event->start_time.bcd_time > node_event->start_time.bcd_time)
                        {
                            return 1;
                        }
                        if(new_event->start_time.bcd_time < node_event->start_time.bcd_time)
                        {
                            return -1;
                        }
                    }
                }
            }
        }
    }

    return 0;
}

static gint __eit_searchfunc_schedule_num(gpointer key, gpointer data)
{
    EVENT *compare_event,*node_event;

    compare_event=(EVENT *)data;
    node_event=(EVENT *)key;

    if(compare_event->section->on_id > node_event->section->on_id)
    {
        return 1;
    }
    if(compare_event->section->on_id < node_event->section->on_id)
    {
        return -1;
    }
    else
    {
        if(compare_event->section->ts_id > node_event->section->ts_id)
        {
            return 1;
        }
        if(compare_event->section->ts_id < node_event->section->ts_id)
        {
            return -1;
        }
        else
        {
            if(compare_event->section->service_id> node_event->section->service_id)
            {
                return 1;
            }
            if(compare_event->section->service_id < node_event->section->service_id)
            {
                return -1;
            }
            else
            {
                if(compare_event->start_time.mjd_date > node_event->start_time.mjd_date)
                {
                    return 1;
                }
                if(compare_event->start_time.mjd_date < node_event->start_time.mjd_date)
                {
                    return -1;
                }
                else
                {
#if 0
                    /*{{{add for 将现在时间和endtime比较  by xyfeng  2011-9-12   */
                    time_td_t time_td;
                    time_orig_t time_orig;

                    App_Get_Event_End_Time( &time_td,  node_event );  // pxwang 2012-8-14
                    time_orig = Time_Convert_Td_to_Orig( time_td );
                    if(compare_event->start_time.bcd_time > time_orig.bcd_time )
                    {
                        return 1;
                    }
                    /*}}}add   by xyfeng  2011-9-12  */

#endif
                    if(compare_event->start_time.bcd_time > node_event->start_time.bcd_time)
                    {
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}

static gint __eit_auxiliaryfunc_normal(gpointer value, gpointer data)
{
    EVENT_LIST *prev = NULL;
    prev = *(EVENT_LIST**)data;
    if(NULL == prev)
    {
        prev=OS_MemAllocate(system_p, sizeof(EVENT_LIST));
        if(NULL == prev)
        {
            assert(0);
        }
        memset(prev,0,sizeof(EVENT_LIST));
    }
    else
    {
        prev->next=OS_MemAllocate(system_p, sizeof(EVENT_LIST));
        if(NULL == prev->next)
        {
            assert(0);
        }
        memset(prev->next,0,sizeof(EVENT_LIST));
        prev->next->prev=prev;
        prev=prev->next;
    }
    prev->event=(EVENT *)value;
    *(EVENT_LIST**)data=prev;

    return 0;
}

static gint __eit_comparefunc_nibble(gconstpointer a, gconstpointer b)
{
    EVENT *new_event,*node_event;

    new_event=(EVENT *)a;
    node_event=(EVENT *)b;

    if(new_event->content_nibble_level > node_event->content_nibble_level)
    {
        return 1;
    }
    if(new_event->content_nibble_level < node_event->content_nibble_level)
    {
        return -1;
    }
    else
    {
        if(new_event->start_time.mjd_date > node_event->start_time.mjd_date)
        {
            return 1;
        }
        if(new_event->start_time.mjd_date < node_event->start_time.mjd_date)
        {
            return -1;
        }
        else
        {
            if(new_event->start_time.bcd_time > node_event->start_time.bcd_time)
            {
                return 1;
            }
            if(new_event->start_time.bcd_time < node_event->start_time.bcd_time)
            {
                return -1;
            }
            else
            {
                if(new_event->section->service_id > node_event->section->service_id)
                {
                    return 1;
                }
                if(new_event->section->service_id < node_event->section->service_id)
                {
                    return -1;
                }
            }
        }
    }

    return 0;
}

static gint __eit_comparefunc_section(gconstpointer a, gconstpointer b)
{
    eit_section *new_section,*node_section;

    new_section=(eit_section *)a;
    node_section=(eit_section *)b;

    if(new_section->on_id > node_section->on_id)
    {
        return 1;
    }
    if(new_section->on_id < node_section->on_id)
    {
        return -1;
    }
    else
    {
        if(new_section->ts_id > node_section->ts_id)
        {
            return 1;
        }
        if(new_section->ts_id < node_section->ts_id)
        {
            return -1;
        }
        else
        {
            if(new_section->service_id > node_section->service_id)
            {
                return 1;
            }
            if(new_section->service_id < node_section->service_id)
            {
                return -1;
            }
            else
            {
                if(new_section->table_id > node_section->table_id)
                {
                    return 1;
                }
                if(new_section->table_id < node_section->table_id)
                {
                    return -1;
                }
                else
                {
                    if(new_section->section_number > node_section->section_number)
                    {
                        return 1;
                    }
                    if(new_section->section_number < node_section->section_number)
                    {
                        return -1;
                    }
                }
            }
        }
    }

    return 0;
}

static gint __eit_searchfunc_section_tableid(gpointer	key,gpointer	data)
{
    eit_section *new_section,*node_section;
    new_section=(eit_section *)data;
    node_section=(eit_section *)key;

    if(new_section->on_id > node_section->on_id)
    {
        return 1;
    }
    if(new_section->on_id < node_section->on_id)
    {
        return -1;
    }
    else
    {
        if(new_section->ts_id > node_section->ts_id)
        {
            return 1;
        }
        if(new_section->ts_id < node_section->ts_id)
        {
            return -1;
        }
        else
        {
            if(new_section->service_id > node_section->service_id)
            {
                return 1;
            }
            if(new_section->service_id < node_section->service_id)
            {
                return -1;
            }
            else
            {
                if(new_section->table_id > node_section->table_id)
                {
                    return 1;
                }
                if(new_section->table_id < node_section->table_id)
                {
                    return -1;
                }
                else
                {
                    if(new_section->section_number > node_section->section_number)
                    {
                        return 1;
                    }
                    if(new_section->section_number < node_section->section_number)
                    {
                        return -1;
                    }
                }
            }
        }
    }

    return 0;
}

static gint __eit_searchfunc_section(gpointer	key,gpointer	data)
{
    eit_section *new_section,*node_section;

    new_section=(eit_section *)data;
    node_section=(eit_section *)key;

    if(new_section->on_id > node_section->on_id)
    {
        return 1;
    }
    if(new_section->on_id < node_section->on_id)
    {
        return -1;
    }
    else
    {
        if(new_section->ts_id > node_section->ts_id)
        {
            return 1;
        }
        if(new_section->ts_id < node_section->ts_id)
        {
            return -1;
        }
        else
        {
            if(new_section->service_id > node_section->service_id)
            {
                return 1;
            }
            if(new_section->service_id < node_section->service_id)
            {
                return -1;
            }
            else
            {
                if(new_section->section_number > node_section->section_number)
                {
                    return 1;
                }
                if(new_section->section_number < node_section->section_number)
                {
                    return -1;
                }
            }
        }
    }

    return 0;
}


static gint __eit_comparefunc_pf(gconstpointer	a, gconstpointer	b)
{


    EVENT *new_event,*node_event;
    new_event=(EVENT *)a;
    node_event=(EVENT *)b;

    if(new_event->section->on_id > node_event->section->on_id)
        return 1;
    if(new_event->section->on_id < node_event->section->on_id)
        return -1;
    else
    {
        if(new_event->section->ts_id > node_event->section->ts_id)
            return 1;
        if(new_event->section->ts_id < node_event->section->ts_id)
            return -1;
        else
        {
            if(new_event->section->service_id> node_event->section->service_id)
                return 1;
            if(new_event->section->service_id < node_event->section->service_id)
                return -1;
            else
            {
                if(new_event->section->table_id > node_event->section->table_id)
                    return 1;
                if(new_event->section->table_id < node_event->section->table_id)
                    return -1;
                else
                {
                    if(new_event->section->section_number> node_event->section->section_number)
                        return 1;
                    if(new_event->section->section_number < node_event->section->section_number)
                        return -1;

                }

            }


        }



    }
    return 0;
}


static gint __eit_delsection_pf	(gpointer	key, gpointer	value, gpointer data)
{
    eit_section *section=(eit_section *)value;

    _EITBuf_Free(section);
    eit_size-=sizeof(eit_section);

    return 0;
}

static gint __eit_del_event_pf(gpointer key, gpointer	value, gpointer data)
{
    EVENT *event=(EVENT *)value;

    _release_eit_event(event);

    return 0;
}


static gint __eit_delsection_schedule	(gpointer	key, gpointer	value, gpointer data)
{
    eit_section *section = (eit_section *)value;

    _EITBuf_Free(section);
    eit_schedule_size -= sizeof(eit_section);

    return 0;
}



static gint __eit_del_event_schedule(gpointer key, gpointer	value, gpointer data)
{
    EVENT *event = (EVENT *)value;

    _release_schedule_event(event);

    return 0;
}

static void __nomem_except_handle(void)
{
    g_tree_traverse(schedule_section_tree, __eit_delsection_schedule, G_IN_ORDER, NULL);
    g_tree_traverse(schedule_event_tree, __eit_del_event_schedule, G_IN_ORDER, NULL);
    g_tree_traverse(pf_section_tree, __eit_delsection_pf, G_IN_ORDER, NULL);
    g_tree_traverse(pf_event_tree, __eit_del_event_pf, G_IN_ORDER, NULL);
    g_tree_destroy(schedule_section_tree);
    g_tree_destroy(schedule_event_tree);
    g_tree_destroy(pf_section_tree);
    g_tree_destroy(pf_event_tree);
//	printf("###eit_size = %d, eit_schedule_size = %d\n", eit_size, eit_schedule_size );
    if((eit_size != 0) || (eit_schedule_size != 0))
    {
        eit_size = 0;
        eit_schedule_size = 0;
    }
    schedule_section_tree = g_tree_new(__eit_comparefunc_section);
    schedule_event_tree = g_tree_new(__eit_comparefunc_normal);
    pf_section_tree = g_tree_new(__eit_comparefunc_section);
    pf_event_tree = g_tree_new(__eit_comparefunc_normal);
}

static U8 _Parse_Extended_Items( Extended_Items *ptExtItems, U8 *pucCase, U32 *puiSize )
{
    int				uiE_ItemsName_Len = 0,
                    uiE_Items_Len = 0,
                    uiS_Real_Len = 0;
    char  			TempBuf[500];

    ptExtItems->next = NULL;
    uiS_Real_Len = uiE_ItemsName_Len = pucCase[0];
    if(uiE_ItemsName_Len > 0)
    {
        memset( TempBuf, 0, 500 );
        memcpy( TempBuf, pucCase + 1, uiE_ItemsName_Len );
        //uiS_Real_Len = Unicode_String_Check(TempBuf, uiE_ItemsName_Len);
        if(uiS_Real_Len > 0)
        {
            ptExtItems->pcItemsName = ( char* )_EITBuf_Malloc( uiS_Real_Len + 1 );
            if( ptExtItems->pcItemsName == NULL )
            {
                return 0;
            }
            memset( ptExtItems->pcItemsName, 0, uiS_Real_Len + 1 );
            *puiSize += ( uiS_Real_Len + 1 );
            assert( *puiSize > 0 );
            memcpy( ptExtItems->pcItemsName, TempBuf, uiS_Real_Len );
            ptExtItems->pcItemsName[ uiS_Real_Len ] = 0;
        }
        else
        {
            ptExtItems->pcItemsName = NULL;
        }
    }
    else
    {
        ptExtItems->pcItemsName = NULL;
    }
    ptExtItems->usItemsName_Lenth = uiS_Real_Len;

    uiS_Real_Len = uiE_Items_Len = pucCase[uiE_ItemsName_Len + 1];
    if(uiE_Items_Len > 0)
    {
        memset( TempBuf, 0, 500 );
        memcpy( TempBuf, pucCase + 2 + uiE_ItemsName_Len, uiE_Items_Len );
        //uiS_Real_Len = Unicode_String_Check(TempBuf, uiE_Items_Len);
        if(uiS_Real_Len > 0)
        {
            ptExtItems->pcItems = ( char* )_EITBuf_Malloc( uiS_Real_Len + 1 );
            if( ptExtItems->pcItems == NULL )
            {
                return 0;
            }
            memset( ptExtItems->pcItems, 0, uiS_Real_Len + 1 );
            *puiSize += ( uiS_Real_Len + 1 );
            assert( *puiSize > 0 );
            memcpy( ptExtItems->pcItems, TempBuf, uiS_Real_Len );
            ptExtItems->pcItems[ uiS_Real_Len ] = 0;
        }
        else
        {
            ptExtItems->pcItems = NULL;
        }
    }
    else
    {
        ptExtItems->pcItems = NULL;
    }
    ptExtItems->usItems_Lenth = uiS_Real_Len;
    return 1;
}

static void _Free_Extended_Items( Extended_Items *ptExtItems, U32 *puiSize )
{
    if( ptExtItems->pcItemsName != NULL )
    {
        _EITBuf_Free( ptExtItems->pcItemsName );
        ptExtItems->pcItemsName=NULL;
        *puiSize -= ( ptExtItems->usItemsName_Lenth + 1 );
        assert( *puiSize > 0 );
    }
    if( ptExtItems->pcItems != NULL )
    {
        _EITBuf_Free( ptExtItems->pcItems);
        ptExtItems->pcItems=NULL;
        *puiSize -= ( ptExtItems->usItems_Lenth+ 1 );
        assert( *puiSize > 0 );
    }
   // memset( ptExtItems, 0, sizeof( ext_eit ) ); del by zxguan 2013-03-05 free bug.
}

static U8 _Parse_Extended_Items_List( ext_eit *ptExtend_EPG, U8 *pucCase, U32 *puiSize )
{
    int				uiE_Items_Len = 0;
    Extended_Items	tExtItems;
    Extended_Items	*ptExtItems;
    Extended_Items	*ptExtItems_tmp;
    int				iI = 0;

    uiE_Items_Len = pucCase[0];
    for(iI = 0; iI < uiE_Items_Len; )
    {
        memset(&tExtItems, 0, sizeof(Extended_Items));
        if(0 == _Parse_Extended_Items(&tExtItems, pucCase + 1 + iI, puiSize))
        {
            _Free_Extended_Items(&tExtItems, puiSize);
            return 0;
        }
        ptExtItems = (Extended_Items*)_EITBuf_Malloc( sizeof(Extended_Items) );
        if(ptExtItems == NULL)
        {
            _Free_Extended_Items(&tExtItems, puiSize);
            return 0;
        }
        memcpy(ptExtItems, &tExtItems, sizeof(Extended_Items));
        if(ptExtend_EPG->ExtItems == NULL)
        {
            ptExtend_EPG->ExtItems = ptExtItems;
        }
        else
        {
            ptExtItems_tmp = ptExtend_EPG->ExtItems;
            while(ptExtItems_tmp->next != NULL)
            {
                ptExtItems_tmp = ptExtItems_tmp->next;
            }
            ptExtItems_tmp->next = ptExtItems;
        }
        *puiSize += ( sizeof(Extended_Items) );
        assert( *puiSize > 0 );
        iI += (pucCase[1 + iI]) + pucCase[(pucCase[1 + iI])+(1+iI)+1] + 2;
    }
    return 1;
}
static U8 _Free_Extended_Items_List( Extended_Items *ptExtItems, U32 *puiSize )
{
    Extended_Items	*ptExtItems_tmp,*ptExtItems_pre;

    ptExtItems_tmp = ptExtItems;
    while(ptExtItems_tmp)
    {
        _Free_Extended_Items(ptExtItems_tmp, puiSize);
        ptExtItems_pre=ptExtItems_tmp;
        ptExtItems_tmp = ptExtItems_tmp->next;
        _EITBuf_Free(ptExtItems_pre);
        *puiSize -= ( sizeof(Extended_Items) );

    }
    return 0;
}
static U8 _Parse_Extended_Event( ext_eit *ptExtend_EPG, U8 *pucCase, U32 *puiSize )
{
    int				uiE_Items_Len = 0,
                    uiE_Text_Len = 0,
                    uiS_Real_Len = 0;
    char  			TempBuf[500];

    ptExtend_EPG->next = NULL;
    ptExtend_EPG->descriptor_number = pucCase[ 2 ] & 0xf0 >> 4;
    ptExtend_EPG->last_descriptor_number = pucCase[ 2 ] & 0x0f;
    ptExtend_EPG->ISO_639_language_code[ 0 ] = pucCase[ 3 ];
    ptExtend_EPG->ISO_639_language_code[ 1 ] = pucCase[ 4 ];
    ptExtend_EPG->ISO_639_language_code[ 2 ] = pucCase[ 5 ];
    uiE_Items_Len = pucCase[6];
    if( uiE_Items_Len > 0 )
    {
        if(0 == _Parse_Extended_Items_List(ptExtend_EPG, pucCase + 6, puiSize))
            return 0;
    }
    else
    {
        ptExtend_EPG->ExtItems= NULL;
    }

    uiS_Real_Len = uiE_Text_Len = pucCase[ 7 + uiE_Items_Len ];
    if( uiE_Text_Len > 0 )
    {
        memset( TempBuf, 0, 500 );
        memcpy( TempBuf, pucCase + 8+ uiE_Items_Len, uiE_Text_Len );
#if 0
        uiS_Real_Len = Unicode_String_Check(TempBuf, uiE_Text_Len);//zmm add for convert unicode to GB 2006-06-02
#endif
        if(uiS_Real_Len > 0)
        {
            ptExtend_EPG->text = ( char* )_EITBuf_Malloc( uiS_Real_Len + 1 );
            if( ptExtend_EPG->text == NULL )
            {
                return 0;
            }
            memset( ptExtend_EPG->text, 0, uiS_Real_Len + 1 );
            *puiSize += ( uiS_Real_Len + 1 );
            assert( *puiSize > 0 );
            memcpy( ptExtend_EPG->text, TempBuf, uiS_Real_Len );
            ptExtend_EPG->text[ uiS_Real_Len ] = 0;
        }
        else
        {
            ptExtend_EPG->text = NULL;
        }
    }
    else
    {
        ptExtend_EPG->text = NULL;
    }

    ptExtend_EPG->text_length = uiS_Real_Len;
    return 1;
}

static void _Free_Extended_Event( ext_eit *ptExtend_EPG, U32 *puiSize )
{
    Extended_Items	*ptExtItems_Tmp;
    Extended_Items	*ptExtItems_Next;

    if( ptExtend_EPG->ExtItems != NULL )
    {
        ptExtItems_Tmp = ptExtend_EPG->ExtItems;
        while(ptExtItems_Tmp != NULL)
        {
            ptExtItems_Next = ptExtItems_Tmp->next;
            _Free_Extended_Items(ptExtItems_Tmp, puiSize);
            _EITBuf_Free( ptExtItems_Tmp );
            *puiSize -= sizeof(Extended_Items);
            assert( *puiSize > 0 );
            ptExtItems_Tmp = ptExtItems_Next;
        }
        ptExtend_EPG->ExtItems=NULL;
    }
    if( ptExtend_EPG->text != NULL )
    {
        _EITBuf_Free( ptExtend_EPG->text);
        *puiSize -= ( ptExtend_EPG->text_length + 1 );
        assert( *puiSize > 0 );
        ptExtend_EPG->text=NULL;
    }
    memset( ptExtend_EPG, 0, sizeof( ext_eit ) );
}


static U32 _Parse_Short_Event( EVENT_TEXT *ptShort_Event, U8 *pucCase, U32 *puiSize )
{
    int					uiS_Name_Len = 0,
                        uiS_Text_Len = 0,
                        uiS_Real_Len = 0,
                        uiS_free_Len = 0;
    char  TempBuf[500];

    ptShort_Event->next = NULL;
    ptShort_Event->ISO_639_language_code[0] = pucCase[2];
    ptShort_Event->ISO_639_language_code[1] = pucCase[3];
    ptShort_Event->ISO_639_language_code[2] = pucCase[4];
    uiS_Real_Len = uiS_Name_Len = pucCase[5];
    if( uiS_Name_Len > 0 )
    {
        memset( TempBuf, 0, 500 );
        memcpy( TempBuf, pucCase + 6, uiS_Name_Len );
#if 0
        uiS_Real_Len = Unicode_String_Check(TempBuf, uiS_Name_Len);//zmm add for convert unicode to GB 2006-06-02
#endif
        if(uiS_Real_Len > 0)
        {

            ptShort_Event->event_name = ( char* )_EITBuf_Malloc( uiS_Real_Len + 1 );
            if( ptShort_Event->event_name == NULL )
            {
                return 0;
            }
            memset( ptShort_Event->event_name, 0, uiS_Real_Len + 1 );
            uiS_free_Len=( uiS_Real_Len + 1 );
            *puiSize += ( uiS_Real_Len + 1 );
            assert( *puiSize > 0 );
            memcpy( ptShort_Event->event_name, TempBuf, uiS_Real_Len );
            ptShort_Event->event_name[ uiS_Real_Len ] = 0;
        }
        else
        {
            ptShort_Event->event_name = NULL;
        }
    }
    else
    {
        ptShort_Event->event_name = NULL;
    }
    ptShort_Event->name_length = uiS_Real_Len;

    uiS_Real_Len = uiS_Text_Len=pucCase[ 6 + uiS_Name_Len ];
    if( uiS_Text_Len > 0 )
    {
        memset( TempBuf, 0, 500 );
        memcpy( TempBuf, pucCase + 7 + uiS_Name_Len, uiS_Text_Len );
#if 0
        uiS_Real_Len = Unicode_String_Check(TempBuf, uiS_Text_Len);//zmm add for convert unicode to GB 2006-06-02
#endif
        if(uiS_Real_Len > 0)
        {
            ptShort_Event->short_event = ( char* )_EITBuf_Malloc( uiS_Real_Len + 1 );
            if( ptShort_Event->short_event == NULL )
            {

                return 0;
            }
            memset( ptShort_Event->short_event, 0, uiS_Real_Len + 1 );
            *puiSize += ( uiS_Real_Len + 1 );
            assert( *puiSize > 0 );
            memcpy( ptShort_Event->short_event, TempBuf, uiS_Real_Len );
            ptShort_Event->short_event[ uiS_Real_Len ] = 0;
        }
        else
        {
            ptShort_Event->short_event = NULL;
        }
    }
    else
    {
        ptShort_Event->short_event = NULL;
    }
    ptShort_Event->short_length = uiS_Real_Len;
    return 1;
}


static void _Free_Short_Event( EVENT_TEXT *ptShort_EPG, U32 *puiSize )
{
    if( ptShort_EPG->event_name != NULL )
    {
        _EITBuf_Free( ptShort_EPG->event_name );
        ptShort_EPG->event_name=NULL;
        *puiSize -= ( ptShort_EPG->name_length + 1 );
        assert( *puiSize > 0 );
    }
    if( ptShort_EPG->short_event != NULL )
    {
        _EITBuf_Free( ptShort_EPG->short_event );
        ptShort_EPG->short_event=NULL;
        *puiSize -= ( ptShort_EPG->short_length + 1 );
        assert( *puiSize > 0 );
    }
    memset( ptShort_EPG, 0, sizeof( EVENT_TEXT ) );
}
static U32 _Parse_Parental_Rate( PARENTAL_RATE **ptRate, U8 *pucCase, U32 *puiSize )
{

    int				uiP_Len = 0,
                    uiP_rate_num = 0;
    PARENTAL_RATE	*ptP_pre_rate=NULL,
                     *ptP_cur_rate=NULL;

    uiP_Len = pucCase[1];
    while( uiP_Len > 0 )
    {
        ptP_cur_rate = ( PARENTAL_RATE* )_EITBuf_Malloc( sizeof( PARENTAL_RATE ) );
        if( ptP_cur_rate == NULL )
        {
            //assert( 0 );
            return 0;
        }
        memset( ptP_cur_rate, 0 ,sizeof( PARENTAL_RATE ) );
        *puiSize += sizeof( PARENTAL_RATE );
        assert( *puiSize > 0 );
        ptP_cur_rate->country_code[ 0 ] = pucCase[ 2 + uiP_rate_num*4 ];
        ptP_cur_rate->country_code[ 1 ] = pucCase[ 3 + uiP_rate_num*4 ];
        ptP_cur_rate->country_code[ 2 ] = pucCase[ 4 + uiP_rate_num*4 ];
        ptP_cur_rate->rate = pucCase[ 5 + uiP_rate_num*4 ];
        ptP_cur_rate->next=NULL;
        uiP_rate_num += 1;
        uiP_Len -= 4;
        if( ptP_pre_rate != NULL )
        {
            ptP_pre_rate->next = ptP_cur_rate;
        }
        else
        {
            *ptRate = ptP_cur_rate;
        }
        ptP_pre_rate = ptP_cur_rate;
        ptP_cur_rate = ptP_cur_rate->next;
    }
    //check_parental(new_node->parental_rating);//sgy just to test parental rate 20060220
    return 1;
}
static void  _Free_Parental_Rate( PARENTAL_RATE *ptParentalRate, U32 *puiSize )
{
    PARENTAL_RATE		*ptCurRate = NULL,
                         *ptNextRate = NULL;

    ptCurRate = ptParentalRate;
    while( ptCurRate != NULL )
    {
        ptNextRate = ptCurRate->next;
        _EITBuf_Free( ptCurRate );
        *puiSize -= sizeof( PARENTAL_RATE );
        ptCurRate = ptNextRate;
        ptNextRate = NULL;
    }
    return ;
}
static U32 _Parse_Event( EVENT *ptEvent, U8 *pucEvent, U32 *puiSize, int *puiDescLen)
{
    int				uiE_DescLen = 0;
    U8		*pucCase = NULL;
    EVENT_TEXT		tShortEPG;
    ext_eit			tExtEPG;

    memset( ptEvent, 0, sizeof( EVENT ) );

    ptEvent->event_id = ( pucEvent[ 0 ] << 8 ) | pucEvent[ 1 ];
    ptEvent->start_time.mjd_date = ( pucEvent[ 2 ] << 8 ) | pucEvent[ 3 ];
    ptEvent->start_time.bcd_time = ( pucEvent[ 4 ] << 16 ) | ( pucEvent[ 5 ] << 8 ) |pucEvent[ 6 ];
    ptEvent->start_time = Time_Convert_UTC_to_Local(ptEvent->start_time);
    ptEvent->duration[ 0 ] = pucEvent[ 7 ];
    ptEvent->duration[ 1 ] = pucEvent[ 8 ];
    ptEvent->duration[ 2 ] = pucEvent[ 9 ];
    ptEvent->content_nibble_level = 0;
    ptEvent->parental_rating = NULL;
    //running_status=pevent[10]&0xe0>>5;
    //free_ca_mode=pevent[10]&0x10>>4;
    *puiDescLen = uiE_DescLen = ( ( pucEvent[ 10 ] & 0x0f ) << 8 ) | pucEvent[ 11 ];
    pucCase = pucEvent + 12;

#if 0
    U16 year = 0;
    U16 month = 0;
    U16 date = 0;
    U16 hour = 0;
    U16 min = 0;
    U16 sec = 0;
    Time_Convert_MJD_to_YMD(ptEvent->start_time.mjd_date,&year,&month,&date);
    Time_Convert_BCD_to_HMS(ptEvent->start_time.bcd_time, &hour, &min, &sec);
    if (49975 < ptEvent->event_id && ptEvent->event_id  < 50000)
    {
        pbiinfo("%d-%d-%d\n", year, month, date);
        pbiinfo("%d:%d:%d\n", hour, min, sec);
        pbiinfo("\ %s	line %d !  event [%d]  starttime[%x][%x] duration[%x]\n",__FUNCTION__,__LINE__ \
                ,ptEvent->event_id,ptEvent->start_time.mjd_date,ptEvent->start_time.bcd_time,ptEvent->duration[0]);
    }
#endif
    while( uiE_DescLen > 0 )
    {
        switch( pucCase[ 0 ] )
        {
        case SHORT_EVENT_DESCRIPTOR_TAG:
        {

            EVENT_TEXT		*ptcurShortEPG = NULL,
                             *ptpreShortEPG = NULL,
                              *ptnextShortEPG = NULL;

            memset( &tShortEPG, 0, sizeof( EVENT_TEXT ) );
            if( _Parse_Short_Event( &tShortEPG, pucCase, puiSize ) == 0 )
            {
                _Free_Short_Event( &tShortEPG, puiSize );
            }
            ptcurShortEPG = ( EVENT_TEXT* )_EITBuf_Malloc( sizeof( EVENT_TEXT ) );
            if( ptcurShortEPG == NULL )
            {
                _Free_Short_Event( &tShortEPG, puiSize );
                return 0;
            }
            *puiSize += sizeof( EVENT_TEXT );

            memcpy( ptcurShortEPG, &tShortEPG, sizeof( EVENT_TEXT ) );
            ptpreShortEPG = ptnextShortEPG = ptEvent->short_epg;
            while( ptnextShortEPG )
            {
                ptpreShortEPG = ptnextShortEPG;
                ptnextShortEPG = ptnextShortEPG->next;
            }
            if( ptpreShortEPG != NULL )
            {
                ptpreShortEPG->next = ptcurShortEPG;
            }
            else
            {
                ptEvent->short_epg = ptcurShortEPG;
            }

        }
        break;
        case EXTENDED_EVENT_DESCRIPTOR_TAG:
        {
            ext_eit		*ptcurExtEPG = NULL,
                         *ptpreExtEPG = NULL,
                          *ptnextExtEPG = NULL;

            memset( &tExtEPG, 0, sizeof( ext_eit ) );
            if( _Parse_Extended_Event( &tExtEPG, pucCase, puiSize) == 0 )
            {
                _Free_Extended_Event( &tExtEPG, puiSize );
            }
            ptcurExtEPG = ( ext_eit* )_EITBuf_Malloc( sizeof( ext_eit ) );
            if( ptcurExtEPG == NULL )
            {
                _Free_Extended_Event( &tExtEPG, puiSize );
                return 0;
            }
            *puiSize += sizeof( ext_eit );
            assert( *puiSize > 0 );
            memcpy( ptcurExtEPG, &tExtEPG, sizeof( ext_eit ) );
            ptpreExtEPG = ptnextExtEPG = ptEvent->extended_epg;
            while( ptnextExtEPG )
            {
                ptpreExtEPG = ptnextExtEPG;
                ptnextExtEPG = ptnextExtEPG->next;
            }
            if( ptpreExtEPG != NULL )
            {
                ptpreExtEPG->next = ptcurExtEPG;
            }
            else
            {
                ptEvent->extended_epg = ptcurExtEPG;
            }
        }
        break;
        case PARENTAL_RATING_DESCRIPTOR_TAG:
        {

            PARENTAL_RATE	*ptcurRate = NULL,
                             *ptpreRate = NULL,
                              *ptnextRate = NULL;
            if( _Parse_Parental_Rate( &ptcurRate, pucCase, puiSize )==0 )
            {
                _Free_Parental_Rate( ptcurRate,puiSize);
                return 0;
            }
            ptpreRate = ptnextRate = ptEvent->parental_rating;
            while( ptnextRate )
            {
                ptpreRate = ptnextRate;
                ptnextRate = ptnextRate->next;
            }
            if( ptpreRate != NULL )
            {
                ptpreRate->next = ptcurRate;
            }
            else
            {
                ptEvent->parental_rating = ptcurRate;
            }

        }
        break;
        case CONTENT_DESCRIPTOR_TAG:
        {
            U16 uiNibble = 0;

            uiNibble = pucCase[ 2 ]<<8 |pucCase[ 3 ];
            ptEvent->content_nibble_level = uiNibble;

        }
        break;

#if NVOD_USED
        case TIME_SHIFTED_EVENT_DESCRIPTOR_TAG:
        {
            int tse_lenth = 0;

            tse_lenth = pucCase[1];
            if(tse_lenth > 0)
            {
                ptEvent->ref_serv_id = (pucCase[2]<<8) | pucCase[3];
                ptEvent->ref_event_id = (pucCase[4]<<8) | pucCase[5];
            }
            else
            {
                ptEvent->ref_serv_id = 0xFFFF;
                ptEvent->ref_event_id = 0xFFFF;
            }
        }
        break;
#endif
        default:
            break;
        }
        uiE_DescLen -= ( pucCase[ 1 ] + 2 );
        pucCase += ( pucCase[ 1 ] + 2 );
    }
    return 1;
}
static void _Free_Event( EVENT *ptEvent, U32 *puiSize )
{
    ext_eit		*ptCurExtEPG = NULL,
                 *ptNextExtEPG = NULL;

    if( ptEvent->short_epg != NULL )
    {
        _Free_Short_Event( ptEvent->short_epg, puiSize );
        _EITBuf_Free( ptEvent->short_epg );
        ptEvent->short_epg=NULL;
        *puiSize -= sizeof( EVENT_TEXT );
        assert( *puiSize > 0 );
    }
    if( ptEvent->parental_rating != NULL )
    {
        _Free_Parental_Rate( ptEvent->parental_rating, puiSize );
        ptEvent->parental_rating=NULL;
    }
    ptCurExtEPG = ptEvent->extended_epg;
    while( ptCurExtEPG != NULL )
    {
        ptNextExtEPG = ptCurExtEPG->next;
        _Free_Extended_Event( ptCurExtEPG, puiSize );
        _EITBuf_Free( ptCurExtEPG );
        *puiSize -= sizeof( ext_eit );
        assert( *puiSize > 0 );
        ptCurExtEPG = ptNextExtEPG;
        ptNextExtEPG = NULL;
    }
    ptEvent->extended_epg=NULL;
    memset( ptEvent, 0 , sizeof( EVENT ) );
}


static U8 _Parse_EIT(U8 * buffer, void **parsed_result)
{
    eit_section		section;
    eit_section		*old_section = NULL;
    EVENT			*event = NULL;
    EVENT			*pre_event = NULL;
    U8				*pevent = NULL;
    U32				section_length = 0;
    int 			descriptor_events = 0,
                    descriptors_loop_length = 0;
    //U16 tmp_ts_id;
    U32 crc = 0;

    if((eit_size + eit_schedule_size) >= EIT_MAX_BUFFER)
    {
        goto no_mem;
    }

    if( (buffer == NULL) || (parsed_result == NULL) )
    {
        return 0;
    }

    section_length = (((buffer[1]&0x0f)<<8)|buffer[2]);
    if((section_length < 27)||(section_length > 4093))
    {
        return 0;
    }

    section.table_id=buffer[0];
    section.service_id=(buffer[3]<<8)|buffer[4];
    section.version_number=(buffer[5]&0x3e)>>1;
    section.section_number=buffer[6];
    section.ts_id=(buffer[8]<<8)|buffer[9];
    section.on_id=(buffer[10]<<8)|buffer[11];
    section.section_crc = (U32)((buffer[section_length-1]<<24)
                                + (buffer[section_length]<<16)
                                + (buffer[section_length+1]<<8)
                                +  buffer[section_length+2]);

    /*
    	if(!check_crc(buffer))
    	{
    		return eTBL_OTHER_ERROR;
    	}
    */
    //pbiinfo("\ %s	line %d !  tableid [%d] serid[%d] version[%d] tsid[%d] onid[%d] \n",__FUNCTION__,__LINE__
    //,section.table_id,section.service_id,section.version_number,section.ts_id,section.on_id);

    if((section.table_id == 0x4e) || (section.table_id == 0x4f))
    {
        if(section.section_number > 1)
        {
            return 0;
        }

        old_section = g_tree_search(pf_section_tree, __eit_searchfunc_section_tableid, &section);
        if( (NULL != old_section)
                && (old_section->version_number == section.version_number)
                && (old_section->section_crc == section.section_crc) )
        {
            nouse_section_number_pf++;
            return 0;
        }
        valid_section_number_pf++;

        if(NULL == old_section)
        {
            old_section = (eit_section*)_EITBuf_Malloc(sizeof(eit_section));
            if(NULL == old_section)
            {
                goto no_mem;
            }
            eit_size += sizeof(eit_section);
            memcpy(old_section, &section, sizeof(eit_section));
            old_section->head = NULL;
            g_tree_insert(pf_section_tree, old_section, old_section);
        }
        else
        {
            EVENT * tmp_event = NULL;

            old_section->version_number = section.version_number;
            old_section->section_crc = section.section_crc;
            event = old_section->head;
            old_section->head = NULL;
            while(NULL != event)
            {
                tmp_event = event;
                event = event->next;
                g_tree_remove(pf_event_tree, tmp_event);
                _release_eit_event(tmp_event);
            }
        }

        descriptor_events = section_length-11-4;
        pevent = buffer+14;
        while(descriptor_events > 0)
        {
            EVENT	tEvent;

            memset(&tEvent, 0, sizeof(EVENT));

            if( _Parse_Event(&tEvent, pevent, &eit_size, &descriptors_loop_length) == 0 )
            {
                _Free_Event(&tEvent, &eit_size);
                pbiinfo( "_EITBuf_Malloc: EIT Parse EVENT P/F Descriptor error section_number = %d, event_id = 0x%x\n",
                         section.section_number, tEvent.event_id );
                //	section.section_number, tEvent.event_id ));
                goto no_mem;
            }
            descriptor_events -= ( descriptors_loop_length + 12 );
            pevent += ( descriptors_loop_length + 12 );

            event = (EVENT*)_EITBuf_Malloc(sizeof(EVENT));
            if(event == NULL)
            {
                _Free_Event(&tEvent, &eit_size);
                goto no_mem;
            }
            eit_size += sizeof(EVENT);
            memcpy(event, &tEvent, sizeof(EVENT));
            event->section = old_section;
            if( NULL == old_section->head )
            {
                old_section->head = pre_event = event;
            }
            else
            {
                pre_event->next = event;
                pre_event = event;
            }
            pf_number++;
            g_tree_insert(pf_event_tree, event, event);
        }
    }
    else
    {
        old_section = g_tree_search(schedule_section_tree, __eit_searchfunc_section_tableid, &section);
        if( (NULL != old_section)
                && (old_section->version_number == section.version_number)
                && (old_section->section_crc == section.section_crc) )
        {
            nouse_section_number++;
            return 0;
        }

        valid_section_number++;
        if(NULL == old_section)
        {
            old_section = (eit_section*)_EITBuf_Malloc(sizeof(eit_section));
            if(NULL == old_section)
            {
                goto no_mem;
            }

            eit_schedule_size += sizeof(eit_section);
            memcpy(old_section, &section, sizeof(eit_section));
            old_section->head = NULL;
            g_tree_insert(schedule_section_tree, old_section, old_section);
        }
        else
        {
            EVENT * tmp_event = NULL;
            old_section->version_number = section.version_number;
            old_section->section_crc = section.section_crc;
            event = old_section->head;
            old_section->head = NULL;
            while(NULL != event)
            {
                tmp_event = event;
                event = event->next;
                g_tree_remove(schedule_event_tree, tmp_event);
                _release_schedule_event(tmp_event);
            }
        }

        descriptor_events = section_length-11-4;
        pevent = buffer+14;
        while(descriptor_events>0)
        {
            EVENT	tEvent;

            memset(&tEvent, 0, sizeof(EVENT));

            if( _Parse_Event(&tEvent, pevent, &eit_schedule_size, &descriptors_loop_length) == 0 )
            {
                _Free_Event(&tEvent, &eit_schedule_size);
                pbiinfo( "_EITBuf_Malloc: EIT Parse EVENT Schedule Descriptor error section_number = %d, event_id = 0x%x\n",
                         section.section_number, tEvent.event_id );
                //		section.section_number, tEvent.event_id ));
                goto no_mem;
            }
            descriptor_events -= ( descriptors_loop_length + 12 );
            pevent += ( descriptors_loop_length + 12 );

            event = (EVENT*)_EITBuf_Malloc(sizeof(EVENT));
            if(NULL == event)
            {
                _Free_Event(&tEvent, &eit_schedule_size);
                goto no_mem;
            }
            eit_schedule_size += sizeof(EVENT);
            memcpy(event, &tEvent, sizeof(EVENT));

            event->section = old_section;
            if(NULL == old_section->head)
            {
                old_section->head = pre_event = event;
            }
            else
            {
                pre_event->next = event;
                pre_event = event;
            }
            schedule_number++;
            g_tree_insert(schedule_event_tree, event, event);
        }
    }

    *parsed_result = old_section;
    return 1;

no_mem:

    __nomem_except_handle();

    return 0;
}


////////////////////////////////////////////////////////////////////////////////
static U32 guEit_Msg_Queue = 0xFFFFFFFF;
static U32 guEit_Access_Sem = 0xFFFFFFFF;
static U32 guEit_Parse_task = 0xFFFFFFFF;
static const U32 guEit_Stack_Size = 8 * 1024;	/* 8K */
static const U32 guEit_Priority = 1;

static U32 guEit_Process_Sem = 0xFFFFFFFF;
static U8  gucEit_Pause_Flag = 0;

static EpgNotifyFunc gpfEpg_Notify_Func = NULL;

void _eit_callback_func(SYS_Table_Msg_t message)
{
    if( gucEit_Pause_Flag == 1 )
    {
        if((NULL != message.pfFreeBuffer) && (NULL != message.buffer))
        {
            message.pfFreeBuffer(message.buffer);
        }
    }
    else
    {
        if( OS_SUCCESS != OS_QueueSend(guEit_Msg_Queue, &message,
                                       sizeof(SYS_Table_Msg_t), OS_PEND, 0) )
        {
            if((NULL != message.pfFreeBuffer) && (NULL != message.buffer))
            {
                message.pfFreeBuffer(message.buffer);
            }
        }
    }
}


static void _eit_parse_task(void* param)
{

    while(1)
    {
        SYS_Table_Msg_t msg;
        U32				actual_size = 0;
        S32				recv_ret = OS_SUCCESS;

        eit_section		*new_sect = NULL;

        OS_SemWait(guEit_Process_Sem);

        /* receive EIT section */
        memset(&msg, 0, sizeof(SYS_Table_Msg_t));
        recv_ret = OS_QueueReceive(guEit_Msg_Queue, &msg, sizeof(SYS_Table_Msg_t),
                                   &actual_size, 100/* ms */);


        if( recv_ret < 0 )		/* 没有收到数据 */
        {
            OS_SemSignal(guEit_Process_Sem);
            continue;
        }

        if( gucEit_Pause_Flag == 1 )
        {
            pbiinfo(" %s   line %d !\n",__FUNCTION__,__LINE__);
            if((NULL != msg.buffer) && (NULL != msg.pfFreeBuffer))
            {
                msg.pfFreeBuffer(msg.buffer);
            }
            OS_SemSignal(guEit_Process_Sem);
            continue;
        }

        if( OS_SUCCESS != EPG_Try_Lock(100) )
        {
            if((NULL != msg.buffer) && (NULL != msg.pfFreeBuffer))
            {
                msg.pfFreeBuffer(msg.buffer);
            }
            OS_SemSignal(guEit_Process_Sem);
            continue;
        }

        if( 1 == _Parse_EIT(msg.buffer, (void **)&new_sect) )
        {

            if( NULL != gpfEpg_Notify_Func )
            {
                gpfEpg_Notify_Func(new_sect->service_id,new_sect->ts_id,new_sect->on_id);
                //pbiinfo("\n %s   line %d %%%%%%%%%%%%%%%%%%%%changed!!!!!!!!!!!!\n",__FUNCTION__,__LINE__);
            }
        }

        if((NULL != msg.buffer) && (NULL != msg.pfFreeBuffer))
        {
            msg.pfFreeBuffer(msg.buffer);
        }

        EPG_Unlock();

        OS_SemSignal(guEit_Process_Sem);
    }
}

S32 EPG_Init(EpgNotifyFunc epg_notify_func)
{
    SYS_Table_InitParam_t	tInitParam;
    S32 os_err = OS_SUCCESS;

    eit_size = 0;
    eit_schedule_size = 0;

    if(NULL == schedule_section_tree)
    {
        schedule_section_tree = g_tree_new(__eit_comparefunc_section);
    }
    if(NULL == schedule_event_tree)
    {
        schedule_event_tree = g_tree_new(__eit_comparefunc_normal);
    }
    if(NULL == pf_section_tree)
    {
        pf_section_tree = g_tree_new(__eit_comparefunc_section);
    }
    if(NULL == pf_event_tree)
    {
        pf_event_tree = g_tree_new(__eit_comparefunc_normal);
    }

    /* create message queue */
    os_err = OS_QueueCreate(&guEit_Msg_Queue, "Eit_Msg_Queue",
                            16, sizeof(SYS_Table_Msg_t), 0);
    if(OS_SUCCESS != os_err)
    {
        //	printf("EPG_Init, create guEit_Msg_Queue failed!\n");
        return -1;
    }

    os_err = OS_SemCreate(&guEit_Access_Sem, "Eit_Access_Sem", 1, 0);
    if(OS_SUCCESS != os_err)
    {
        //	printf("EPG_Init, created guEit_Access_Sem failed!\n");
        OS_QueueDelete(guEit_Msg_Queue);
        return -1;
    }

    os_err = OS_SemCreate(&guEit_Process_Sem, "Eit_Process_Sem", 0, 0);
    if(OS_SUCCESS != os_err)
    {
        //	printf("EPG_Init, created guEit_Process_Sem failed!\n");
        OS_QueueDelete(guEit_Msg_Queue);
        OS_SemDelete(guEit_Access_Sem);
        return -1;
    }

    os_err = OS_TaskCreate(&guEit_Parse_task, "Eit_Parse_Task", _eit_parse_task,
                           NULL, NULL, guEit_Stack_Size, guEit_Priority, 0);
    if(OS_SUCCESS != os_err)
    {
        //	printf("EPG_Init, created guEit_Parse_task failed!\n");
        OS_QueueDelete(guEit_Msg_Queue);
        OS_SemDelete(guEit_Access_Sem);
        OS_SemDelete(guEit_Process_Sem);
        return -1;
    }

    gpfEpg_Notify_Func = epg_notify_func;

    tInitParam.callback = _eit_callback_func;
    tInitParam.p_semaphore = 0;
    SYS_BackGround_TableInit(SYS_BG_EIT, tInitParam);

    return 0;
}


void EPG_Start(void)
{
    SYS_Table_Param_t tEIT_Param;

    tEIT_Param.pid = EIT_PID;
    tEIT_Param.table_id = 0;
    tEIT_Param.timeout = ONESEC/2;
    tEIT_Param.pn = 0;

    SYS_BackGround_TableStart(SYS_BG_EIT, tEIT_Param);
    pbiinfo("EPG Start %s   line %d !\n",__FUNCTION__,__LINE__);
    gucEit_Pause_Flag = 0;
    OS_SemSignal(guEit_Process_Sem);
}

void EPG_Stop(void)
{
    SYS_BackGround_TableStop(SYS_BG_EIT);

    gucEit_Pause_Flag = 1;
    OS_SemWait_Timeout(guEit_Process_Sem,1000);

    /* clean message queue */
    {
        SYS_Table_Msg_t msg;
        U32				actual_size = 0;

        memset(&msg, 0, sizeof(SYS_Table_Msg_t));
        while( OS_QueueReceive(guEit_Msg_Queue, &msg, sizeof(SYS_Table_Msg_t),
                               &actual_size, OS_CHECK) > 0 )
        {
            if((NULL != msg.buffer) && (NULL != msg.pfFreeBuffer))
            {
                msg.pfFreeBuffer(msg.buffer);
                memset(&msg, 0, sizeof(SYS_Table_Msg_t));
            }
        }
    }

    /* release all sections and events. */

    //__nomem_except_handle();scj del 101013
}

void EPG_Pause(void)
{
    SYS_BackGround_TablePause(SYS_BG_EIT);

    gucEit_Pause_Flag = 1;
    OS_SemWait(guEit_Process_Sem);

    /* clean message queue */
    {
        SYS_Table_Msg_t msg;
        U32				actual_size = 0;

        memset(&msg, 0, sizeof(SYS_Table_Msg_t));
        while( OS_QueueReceive(guEit_Msg_Queue, &msg, sizeof(SYS_Table_Msg_t),
                               &actual_size, OS_CHECK) > 0 )
        {
            if((NULL != msg.buffer) && (NULL != msg.pfFreeBuffer))
            {
                msg.pfFreeBuffer(msg.buffer);
                memset(&msg, 0, sizeof(SYS_Table_Msg_t));
            }
        }
    }

    /* release all sections and events. */
    //__nomem_except_handle();scj del 101013
}

void EPG_Resume(void)
{
    SYS_BackGround_TableResume(SYS_BG_EIT);

    gucEit_Pause_Flag = 0;
    OS_SemSignal(guEit_Process_Sem);
}


S32 EPG_Try_Lock(U32 milisecond)
{
    int iSem_Error;

    if( milisecond == 0xFFFFFFFF )
    {
        iSem_Error = OS_SemWait( guEit_Access_Sem );
    }
    else
    {
        iSem_Error = OS_SemWait_Timeout( guEit_Access_Sem, milisecond );
    }

    return iSem_Error;
}

void EPG_Lock(void)
{
    OS_SemWait(guEit_Access_Sem);
}

void EPG_Unlock(void)
{
    OS_SemSignal(guEit_Access_Sem);
}

void EPG_Register_cb(EpgNotifyFunc epg_notify_func)
{

    if(epg_notify_func )
    {
        gpfEpg_Notify_Func =epg_notify_func;
    }

}


EVENT_LIST * Get_PF_Events
(
    U16 serv_id,
    U16 ts_id,
    U16 on_id,
    U8  flag
)
{
    EVENT_LIST	*head = NULL;
    EVENT_LIST	*tail = NULL;
    EVENT_LIST	*new_event = NULL;
    EVENT		*event = NULL;
    eit_section	*section_node = NULL;
    eit_section	section;

    section.on_id			= on_id;
    section.ts_id			= ts_id;
    section.service_id		= serv_id;
    section.table_id		= 0x4E;
    section.section_number	= flag;

    section_node = g_tree_search(pf_section_tree, __eit_searchfunc_section, &section);
    if(NULL == section_node)
    {
      //  pbiinfo("Get_PF_Events   error %d \n",__LINE__);
        return NULL;
    }

    event = section_node->head;
    while(NULL != event)
    {
        new_event = OS_MemAllocate(system_p, sizeof(EVENT_LIST));
        if( NULL == new_event )
        {
            return head;
        }
        new_event->prev = new_event->next = NULL;
        new_event->event = event;

        if(NULL == head)
        {
            head = tail = new_event;
        }
        else
        {
            tail->next = new_event;
            new_event->prev = tail;
            tail = new_event;
            new_event = NULL;
        }

        event = event->next;
    }

    return head;
}


U32	Get_Sche_Events_NumOfDay
(
    U16 on_id,
    U16 ts_id,
    U16 serv_id,
    U32 mjd_date,
    U32 bcd_time
)
{
    EVENT		event;
    eit_section	section;
    U32			ret_num;

    section.on_id = on_id;
    section.ts_id = ts_id;
    section.service_id = serv_id;

    event.section = &section;
    event.start_time.mjd_date = mjd_date;
    event.start_time.bcd_time = bcd_time;

    ret_num = g_tree_search_num(schedule_event_tree, __eit_searchfunc_schedule_num,
                                &event, NULL, NULL, 0, 0xFFFFFFFF);
    //printf("\n##################################################################\n");
    //printf("get event num, on: %d, ts: %d, serv: %d, ret_num = %d.\n", on_id, ts_id, serv_id, ret_num);

    return ret_num;
}


EVENT_LIST * Get_Sche_Events_OfDay
(
    U16 on_id,
    U16 ts_id,
    U16 serv_id,
    U32 mjd_date,
    U32 bcd_time,
    U32 offset,
    U32 num
)
{
    EVENT_LIST	*head = NULL;
    EVENT		event;
    eit_section	section;
    U32 total = 0;
    U32 i = 0;

    section.on_id = on_id;
    section.ts_id = ts_id;
    section.service_id = serv_id;

    event.section = &section;
    event.start_time.mjd_date = mjd_date;
    event.start_time.bcd_time = bcd_time;

    //printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    //printf("get sche events, on: %d, ts: %d, serv: %d.\n", on_id, ts_id, serv_id);
    total = g_tree_search_num(schedule_event_tree, __eit_searchfunc_schedule_num,
                              &event, __eit_auxiliaryfunc_normal,
                              &head, offset, num);
    if(NULL == head)
    {
        return NULL;
    }

    while(NULL != head->prev)
    {
        i++;
        head=head->prev;
    }
    i++;
    if(total != i)
        assert(0);
    if(total != num)
        assert(0);
    return head;
}


void EPG_DeleteSCH(void)
{
}



S32 EPG_Filter_SCH_Reset(U16 serviceID)
{
    return 0;
}

void EPG_Register_Callback(EpgNotifyFunc epg_notify_func)
{
    EPG_Try_Lock( 0xFFFFFFFF );

    gpfEpg_Notify_Func = epg_notify_func;
    EPG_Unlock();
}




