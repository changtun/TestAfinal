
#include <stdlib.h>

#include "epg_common.h"
#include "cJSON.h"
#include "log/anpdebug.h"


static char *epg_outstr = NULL;

static int epg_bug = 1;
void epg_debug(char *str_temp)
{
    if(epg_bug == 0)
        return;
    DebugMessage("===   PXWANG	epg_debug	CallBy Function:%s", str_temp);
}

int NpEpg_Debug(int flag)
{
	epg_bug = flag;
	return 0;
}


int NpEpg_Init(void)
{
    return 0;
}

int NpEpg_Term(void)
{
    return 0;
}


int NpEpg_Start(void)
{
    epg_debug(__FUNCTION__);
    EPG_Start();
    return 0;
}


int NpEpg_Stop(void)
{
    epg_debug(__FUNCTION__);
    EPG_Stop();
    return 0;
}

int NpEpg_Pause(void)
{
    epg_debug(__FUNCTION__);
    EPG_Pause();
    return 0;
}

int NpEpg_Resume(void)
{
    epg_debug(__FUNCTION__);
    EPG_Resume();
    return 0;
}

char *NpEpg_GetPF(int serv_id, int ts_id, int on_id, int flag)
{
    time_td_t temp_time;

    EVENT_TEXT *temp_SHORT_EPG;
    ext_eit *temp_extended_EVENT;
    Extended_Items *temp_Extended_Items;

    char short_str[24];


    cJSON *root, *short_epg, *extended_epg, *items, *fir, *second;
    EVENT *new_event = NULL;
    int i = 0, j = 0;

    char *out;
    int strLen;

    char *conv_temp = NULL;

	epg_debug(__FUNCTION__);
    new_event = App_Get_Pf_Event(serv_id, ts_id, on_id, flag);

    if(new_event == NULL)
    {
        return NULL;
    }


    root = cJSON_CreateObject();

    cJSON_AddNumberToObject(root, "serv_id", serv_id);
    cJSON_AddNumberToObject(root, "ts_id", ts_id);
    cJSON_AddNumberToObject(root, "on_id", on_id);
    cJSON_AddNumberToObject(root, "P/F", flag);

    cJSON_AddNumberToObject(root, "event_id", new_event->event_id);

    App_Get_Event_Start_Time(&temp_time, new_event);

    cJSON_AddNumberToObject(root, "start_year", temp_time.year);
    cJSON_AddNumberToObject(root, "start_month", temp_time.month);
    cJSON_AddNumberToObject(root, "start_date", temp_time.date);
    cJSON_AddNumberToObject(root, "start_hour", temp_time.hour);
    cJSON_AddNumberToObject(root, "start_minute", temp_time.minute);
    cJSON_AddNumberToObject(root, "start_second", temp_time.second);
    cJSON_AddNumberToObject(root, "start_week", temp_time.week);

    App_Get_Event_End_Time(&temp_time, new_event);
    cJSON_AddNumberToObject(root, "end_year", temp_time.year);
    cJSON_AddNumberToObject(root, "end_month", temp_time.month);
    cJSON_AddNumberToObject(root, "end_date", temp_time.date);
    cJSON_AddNumberToObject(root, "end_hour", temp_time.hour);
    cJSON_AddNumberToObject(root, "end_minute", temp_time.minute);
    cJSON_AddNumberToObject(root, "end_second", temp_time.second);
    cJSON_AddNumberToObject(root, "end_week", temp_time.week);

    memset(&short_str, 0, 24);
    memcpy(&short_str, &(new_event->duration), 3);


    cJSON_AddStringToObject(root, "duration", short_str);
    cJSON_AddNumberToObject(root, "nibble_level", new_event->content_nibble_level);

    if(new_event->parental_rating != NULL)
    {
        memset(&short_str, 0, 24);
        memcpy(&short_str, &(new_event->parental_rating->country_code), 3);
        cJSON_AddStringToObject(root, "parental_country", &short_str);
        cJSON_AddNumberToObject(root, "parental_rate", new_event->parental_rating->rate);
    }
    if(new_event->short_epg != NULL)
    {
        cJSON_AddItemToObject(root, "short", short_epg = cJSON_CreateArray());
        i = 0;

        temp_SHORT_EPG = new_event->short_epg;
        while(temp_SHORT_EPG != NULL)
        {
            cJSON_AddItemToArray(short_epg, fir = cJSON_CreateObject());
            memset(&short_str, 0, 24);
            memcpy(&short_str, &(temp_SHORT_EPG->ISO_639_language_code), 3);
            cJSON_AddStringToObject(fir, "short_language", &short_str);

            if(temp_SHORT_EPG->event_name != NULL)
            {
                DebugMessage("=========   PXWANG	CALL BY strlen(temp_SHORT_EPG->event_name):%d", strlen(temp_SHORT_EPG->event_name));
                conv_temp = Code_ToChar((int)(temp_SHORT_EPG->event_name[0]), temp_SHORT_EPG->event_name, temp_SHORT_EPG->name_length);
                if(conv_temp != NULL)
                {
                    cJSON_AddStringToObject(fir, "short_name", conv_temp);
                    free(conv_temp);
                }
            }

            if(temp_SHORT_EPG->short_event != NULL)
            {
                conv_temp = Code_ToChar((int)(temp_SHORT_EPG->short_event[0]), temp_SHORT_EPG->short_event, temp_SHORT_EPG->short_length);
                if(conv_temp != NULL)
                {
                    cJSON_AddStringToObject(fir, "short_event", conv_temp);
                    free(conv_temp);
                }
            }

            temp_SHORT_EPG = temp_SHORT_EPG->next;
            i++;
        }
        cJSON_AddNumberToObject(root, "short_num", i);
    }

    if(new_event->extended_epg != NULL)
    {
        cJSON_AddItemToObject(root, "extended", extended_epg = cJSON_CreateArray());
        i = 0;
        temp_extended_EVENT = new_event->extended_epg;
        while(temp_extended_EVENT != NULL)
        {
            cJSON_AddItemToArray(extended_epg, fir = cJSON_CreateObject());

            cJSON_AddNumberToObject(fir, "extended_descriptor_number", temp_extended_EVENT->descriptor_number);
            cJSON_AddNumberToObject(fir, "extended_last_descriptor_number", temp_extended_EVENT->last_descriptor_number);
            memset(&short_str, 0, 24);
            memcpy(&short_str, &(temp_extended_EVENT->ISO_639_language_code), 3);
            cJSON_AddStringToObject(fir, "extended_language", &short_str);
            if(temp_extended_EVENT->text != NULL)
            {
                conv_temp = Code_ToChar((int)(temp_extended_EVENT->text[0]), temp_extended_EVENT->text, temp_extended_EVENT->text_length);
                if(conv_temp != NULL)
                {
                    cJSON_AddStringToObject(fir, "extended_text", conv_temp);
                    free(conv_temp);
                }
            }
            cJSON_AddItemToObject(fir, "extended", items = cJSON_CreateArray());
            j = 0;
            temp_Extended_Items = temp_extended_EVENT->ExtItems;
            while(temp_Extended_Items != NULL)
            {
                cJSON_AddItemToArray(items, second = cJSON_CreateObject());
                if(temp_Extended_Items->pcItemsName != NULL)
                {
                    conv_temp = Code_ToChar((int)(temp_Extended_Items->pcItemsName[0]), temp_Extended_Items->pcItemsName, temp_Extended_Items->usItemsName_Lenth);
                    if(conv_temp != NULL)
                    {
                        cJSON_AddStringToObject(second, "ExtItems_ItemsName", conv_temp);
                        free(conv_temp);
                    }
                }


                if(temp_Extended_Items->pcItems != NULL)
                {
                    conv_temp = Code_ToChar((int)(temp_Extended_Items->pcItems[0]), temp_Extended_Items->pcItems, temp_Extended_Items->usItems_Lenth);
                    if(conv_temp != NULL)
                    {
                        cJSON_AddStringToObject(second, "ExtItems_Items", conv_temp);
                        free(conv_temp);
                    }
                }
                temp_Extended_Items = temp_Extended_Items->next;
                j++;
            }
            cJSON_AddNumberToObject(fir, "ExtItems_num", j);
            temp_extended_EVENT = temp_extended_EVENT->next;
            i++;
        }
        cJSON_AddNumberToObject(root, "extern_num", i);
    }

    out = cJSON_Print(root);
    cJSON_Delete(root);

    if(epg_outstr != NULL)
    {
        free(epg_outstr);
        epg_outstr = NULL;
    }

    strLen = strlen(out);
    if(strLen > 0)
    {
        epg_outstr = (char *)malloc(strLen + 10);
        if(epg_outstr == NULL)
        {
            return epg_outstr;
        }
        sprintf( epg_outstr, "%s", out);
    }

    free(out);

    return epg_outstr;
}



void NpEpg_Retstr_Free(void)
{
	epg_debug(__FUNCTION__);
    if(epg_outstr != NULL)
    {
        free(epg_outstr);
        epg_outstr = NULL;
    }
}



