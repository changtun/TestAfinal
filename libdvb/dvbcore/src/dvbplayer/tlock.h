/* TLock.h
 * ͨ���̵߳ݹ���
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

int TLock_create(TLock* lock); /* �ɹ�����0��ʧ�ܷ��ط�0 */
void TLock_destroy(TLock* lock);
int TLock_try_lock(TLock* lock, S32 timeout); /* �ɹ�����0��ʧ�ܷ��ط�0��timeout��>=-1��-1��ʾ���޵ȴ���0��ʾ�������� */
void TLock_lock(TLock* lock);
void TLock_unlock(TLock* lock);

#ifdef __cplusplus
}
#endif

#else
	#error platform should not be supported!
#endif /* #ifdef ST_PLATFORM */

#endif /* #ifndef _TLock_h_ */

