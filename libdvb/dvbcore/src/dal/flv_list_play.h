/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : 
* Description : 
* Author : zyliu
* History :
*	2011/5/12 : Initial Version
******************************************************************************/
#ifndef __FLV_LIST_PLAY_H
#define __FLV_LIST_PLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************/
/*              Includes											*/
/*******************************************************/

/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/
#define FLV_LIST_MAX_SIZE		64
#define FLV_URL_MAX_LEN			256

/*******************************************************/
/*              Exported Types			                */
/*******************************************************/
typedef struct flv_media_info_s
{
	char					url[FLV_URL_MAX_LEN];
	double					duration;					// ��flv Ƭ�γ���ʱ��
	double					time_start;					// flv Ƭ����ʼʱ�䣬��������flv ��Ӱʱ��
	double					time_end;					//  flv Ƭ�ν���ʱ�䣬��������flv ��Ӱʱ��
}flv_media_info_t;

typedef struct flv_play_list_s
{
	flv_media_info_t		list[FLV_LIST_MAX_SIZE];	// flv info	list 
	unsigned int			num;						// flv elememnts number in list
	unsigned int			cur_play;					// currnet play elements in list
	U32						sem;						// list opterate sem
}flv_play_list_t;

typedef enum flvlist_parse_result_e
{
	FLVLIST_PARSE_SUCCESS,
	FLVLIST_PARSE_FAILED,
	FLVLIST_PARSE_FAILED_LATER_RETRY,
	FLVLIST_PARSE_RETRY_SUCCESS,
}flvlist_parse_result_t;

typedef void (*parse_callback_func)(flvlist_parse_result_t result);
/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/
int flv_list_create_list(void);
flv_play_list_t* flv_list_get_list(void);
//int flv_list_clear(void);

int flv_list_create_list_1(void);
flv_play_list_t* flv_list_get_list_1(void);
//int flv_list_clear_1(void);

int flv_list_convert(flv_play_list_t* p_list);

/*����flvlist ��ˢ�²���*/
int flv_list_set_url(flv_play_list_t* p_list, int index, char* url);

int flv_list_get_url_by_index(flv_play_list_t* p_list,int index, char* url);


/*Ӧ�ð�˳�������flv�ĵ�ַ˳����ӵ�list*/
int flv_list_add_url( flv_play_list_t* p_list, char* url);

int flv_list_clear( flv_play_list_t* p_list);

/* ��ȡÿ��flvƬ��ͷ��Ϣ�������Ϣ����Ҫ��ʱ��*/
int flv_list_parse_start_sync( flv_play_list_t* p_list);

/*����seek ��λ��*/
int flv_list_get_seek_info(flv_play_list_t* p_list, double msec_in_whole, unsigned int* p_index, double* p_msec_in_part);


void flv_list_debug( flv_play_list_t* p_list);





/*  ��ȡÿ��flvƬ��ͷ��Ϣ�������Ϣ, �첽��ʽ*/
int flv_list_parse_start_async( flv_play_list_t* p_list, parse_callback_func callback, int parse_mode);
int flv_list_parse_stop( void);
int flv_list_parse_free_buffer(void);


#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __FLV_LIST_PLAY_H */
/* End of XXXXXXXX.h  --------------------------------------------------------- */
