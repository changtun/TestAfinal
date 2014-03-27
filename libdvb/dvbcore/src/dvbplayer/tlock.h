/* TLock.h
 * 通用线程递归锁
 */

#ifndef _TLock_h_
#define _TLock_h_

#define ST_PLATFORM 1
#ifdef ST_PLATFORM
//#include <task.h>
//#include <semaphor.h>

//#include "PbiAssert.h"
typedef struct tagTLock
{
	U32 uSemId;
	U32 tid;	
	int count;
} TLock;

#ifdef __cplusplus
extern "C" {
#endif

int TLock_create(TLock* lock); /* 成功返回0，失败返回非0 */
void TLock_destroy(TLock* lock);
int TLock_try_lock(TLock* lock, S32 timeout); /* 成功返回0，失败返回非0；timeout，>=-1，-1表示无限等待，0表示立即返回 */
void TLock_lock(TLock* lock);
void TLock_unlock(TLock* lock);

#ifdef __cplusplus
}
#endif

#else
	#error platform should not be supported!
#endif /* #ifdef ST_PLATFORM */

#endif /* #ifndef _TLock_h_ */

