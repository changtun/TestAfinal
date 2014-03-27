/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : 
* Description : 
* Author : zyliu
* History :
*	2010/5/10 : Initial Version
******************************************************************************/
#ifndef __FLV_PARSE_H
#define __FLV_PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************/
/*              Includes											*/
/*******************************************************/

/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/

// 只从头开始的固定范围内的数据里解析需要的信息，这样也方便控制下载数据量	
#define FLV_PARSE_INFO_MAX_DATA_SIZE		(5*1024) 	

/*******************************************************/
/*              Exported Types			                */
/*******************************************************/
typedef struct flv_info_s
{
	double			duration;
	double			width;
	double			height;
	double			framerate;
	unsigned int    header_metadata_len;
}flv_info_t;

typedef int (*flv_parse_input_t)(unsigned int offset, unsigned char* p_buf, unsigned int size);
/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/
void flv_parse_set_input(flv_parse_input_t input_func);
int flv_parse_perform( flv_info_t* p_info);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __FLV_PARSE_H */
/* End of XXXXXXXX.h  --------------------------------------------------------- */

