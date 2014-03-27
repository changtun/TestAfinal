/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : 
* Description : 
* Author :
* History :
*	2013/10/15 : Initial Version
******************************************************************************/
#ifndef __DAL_TIMESHIFT_H
#define __DAL_TIMESHIFT_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************/
/*              Includes											*/
/*******************************************************/
#include "pvlist.h"
#include "dal_message.h"

/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/

/*******************************************************/
/*              Exported Types			                */
/*******************************************************/


#define DAL_TIMESHIFT_TASK_STACK_SIZE		1024*8
#define DAL_TIMESHIFT_TASK_PRIOITY			5
#define DAL_TIMESHIFT_QUEUE_DEPTH			20
#define DAL_TIMESHIFT_QUEUE_DATA_SIZE		DAL_MESSAGE_SIZE

#define TIMESHIFT_DATA_CHUNK_SIZE		188*128		//23.5 k



typedef struct TSHIFT_struct_s{
	struct pvlist_head tTIMESHIFT_HEADER;
	U32	total_num;			/*ts stream total num*/
	DAL_Sem_t sem_request;
}TSHIFT_struct_t;


typedef struct TSHIFT_Addr_Node_s{
	struct 	pvlist_head tNode;
	U32		time_info;
	C8		http_address[256];
}TSHIFT_Addr_Node_t;

typedef struct Current_timeshift_s{
	U32 	time_info;
	C8 		http_address[256];
}Current_timeshift_t;



int check_refresh_timeshift_list(void);
S32 DAL_preDownloadStart(void);
S32 dal_timeshift_download_stop(void);
void dal_timeshift_download_start( DAL_Message_t* pMsg);




#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __XXXXXXX_H */
/* End of __DAL_timeshift_H.h  --------------------------------------------------------- */


