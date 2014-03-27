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
	double					duration;					// 该flv 片段持续时间
	double					time_start;					// flv 片段起始时间，相对于真个flv 电影时长
	double					time_end;					//  flv 片段结束时间，相对于真个flv 电影时长
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

/*用于flvlist 的刷新操作*/
int flv_list_set_url(flv_play_list_t* p_list, int index, char* url);

int flv_list_get_url_by_index(flv_play_list_t* p_list,int index, char* url);


/*应该按顺序把所有flv的地址顺序添加到list*/
int flv_list_add_url( flv_play_list_t* p_list, char* url);

int flv_list_clear( flv_play_list_t* p_list);

/* 获取每个flv片段头信息中相关信息，主要是时长*/
int flv_list_parse_start_sync( flv_play_list_t* p_list);

/*计算seek 的位置*/
int flv_list_get_seek_info(flv_play_list_t* p_list, double msec_in_whole, unsigned int* p_index, double* p_msec_in_part);


void flv_list_debug( flv_play_list_t* p_list);





/*  获取每个flv片段头信息中相关信息, 异步方式*/
int flv_list_parse_start_async( flv_play_list_t* p_list, parse_callback_func callback, int parse_mode);
int flv_list_parse_stop( void);
int flv_list_parse_free_buffer(void);


#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __FLV_LIST_PLAY_H */
/* End of XXXXXXXX.h  --------------------------------------------------------- */
