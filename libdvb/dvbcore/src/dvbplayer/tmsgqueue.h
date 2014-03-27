/* TMsgQueue.h
 * 消息队列
 */

#ifndef _TMsgQueue_h_
#define _TMsgQueue_h_

#include "tlock.h"
#include "dvbplayer_proc.h"

#ifdef __cplusplus
extern "C"{
#endif


/*消息结构体*/		
typedef struct tagQueueMsg
{
    int type;						/*消息类型*/
    int serial_num;				 /*要比较的流水号*/
    DVBPlayer_Action_Param_u param1;	/*参数1*/
    void* param2;				/*参数2*/
} QueueMsg;

/*消息队列*/
typedef struct tagMsgQueue 
{
	TLock lock;
	U32		 	read_sem;
	U32			write_sem;
	QueueMsg* msgs;
	int max_count;
	int count;
	int read_index;
	int write_index;
}MsgQueue;


int MsgQueue_create(MsgQueue* msg_q, int max_count); 
void MsgQueue_destroy(MsgQueue* msg_q);
void MsgQueue_post(MsgQueue* msg_q, DVBPlayer_Action_Type_e type, DVBPlayer_Action_Param_u param1, void* param2);
void MsgQueue_post_front(MsgQueue* msg_q, DVBPlayer_Action_Type_e type, int serial_num,  DVBPlayer_Action_Param_u param1, void* param2);
//void MsgQueue_send(MsgQueue* msg_q, int type, DVBPlayer_Message_t param1, void* param2);
int MsgQueue_take(MsgQueue * msg_q,
				   DVBPlayer_Action_Type_e * type,
				   DVBPlayer_Action_Param_u * param1,
				   void** param2,
				   int * serial_num);
int MsgQueue_try_take(MsgQueue* msg_q,
					   U32 milisecond,
					   DVBPlayer_Action_Type_e * type,
					   DVBPlayer_Action_Param_u* param1,
					   void** param2,
					   int * serial_num);

int MsgQueue_CompareMsgType(MsgQueue* msg_q,int type);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _TMsgQueue_h_ */


