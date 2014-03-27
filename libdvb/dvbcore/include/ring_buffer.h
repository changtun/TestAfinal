/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : 
* Description : 
* Author : zyliu
* History :
*	2010/11/4 : Initial Version
******************************************************************************/
#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

//#define RING_BUFFER_DEBUG
#include <pthread.h>


#define rb_malloc(size)				malloc(size)
#define rb_free(ptr)				free(ptr)

#define rb_memcpy(des,src,size) 	memcpy(des,src,size)
#define rb_bzero(ptr,size)			memset(ptr,0,size)


#define rb_mutex_init(p_mutex)		pthread_mutex_init(p_mutex,NULL)
#define rb_mutex_destroy(p_mutex)	pthread_mutex_destroy(p_mutex)
#define rb_mutex_lock(p_mutex)		pthread_mutex_lock(p_mutex)
#define rb_mutex_trylock(p_mutex)	pthread_mutex_trylock(p_mutex)
#define rb_mutex_unlock(p_mutex)	pthread_mutex_unlock(p_mutex)

typedef pthread_mutex_t				rb_mutex_t;
typedef struct ring_buffer_s
{
	unsigned char*		p_head;
	unsigned char*		p_tail;
	unsigned int		buf_size;
	
	unsigned int		data_size;
	unsigned char*		p_write;
	unsigned char*		p_read;

	rb_mutex_t			mutex;
	int					tag;
}ring_buffer_t;

int ring_buffer_init(ring_buffer_t* p_ring_buffer, unsigned int size);
int ring_buffer_init_with_mem(ring_buffer_t* p_ring_buffer, void* mem_addr, unsigned int size);
int ring_buffer_release(ring_buffer_t* p_ring_buffer);
int ring_buffer_release_with_mem(ring_buffer_t* p_ring_buffer);

int	ring_buffer_is_init(ring_buffer_t* p_ring_buffer);
int ring_buffer_flush(ring_buffer_t* p_ring_buffer);
int ring_buffer_write(ring_buffer_t* p_ring_buffer, unsigned char* p_data, unsigned int size);
int ring_buffer_read(ring_buffer_t* p_ring_buffer, unsigned char* p_data, unsigned int size);
int ring_buffer_freesize(ring_buffer_t* p_ring_buffer, unsigned int* p_freesize);
int ring_buffer_datasize(ring_buffer_t* p_ring_buffer, unsigned int* p_datasize);


#ifdef RING_BUFFER_DEBUG
int ring_buffer_debug(ring_buffer_t* p_ring_buffer);
int ring_buffer_test( void);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef __RING_BUFFER_H */
/* End of ring_buffer.h  --------------------------------------------------------- */









