/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : 
* Description : 
* Author : zyliu
* History :
*	2011/4/11 : Initial Version
******************************************************************************/
#ifndef __DAL_PARSE_H
#define __DAL_PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************/
/*              Includes											*/
/*******************************************************/

/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/

/*******************************************************/
/*              Exported Types			                */
/*******************************************************/
typedef struct dal_parse_buffer_s
{
	unsigned char*			pbuffer;
	unsigned int			buf_size;			
}dal_parse_buffer_t;
/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/

int DAL_ParseTaskCreate( void);
int DAL_ParseTaskDelete( void);
int DAL_ParseStart( void);
int DAL_ParseStop( void);

DAL_QueueID_t DAL_GetParseQueueID(void);

BOOL DAL_ParseIsStop( void);

int DAL_ParseTest(void);


#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __XXXXXXX_H */
/* End of __DAL_PARSE_H.h  --------------------------------------------------------- */

