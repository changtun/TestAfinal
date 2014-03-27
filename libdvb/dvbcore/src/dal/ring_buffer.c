/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name :
* Description : 
* History : 
*	    Date			Modification			Name
*	----------			------------			----------
*	2010/11/4			Created					zyliu
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ring_buffer.h"

#define RING_BUFFER_TAG (0xFFFFABCD)
#define RingBuffer_Error(_x_)		{ printf("                          -----[RingBuffer_Error]----- "); printf _x_; }

int ring_buffer_init(ring_buffer_t* p_ring_buffer, unsigned int size)
{
	if(p_ring_buffer == NULL)
	{
		return -1;
	}
	p_ring_buffer->p_head = (unsigned char*)rb_malloc(sizeof(unsigned char)* size);
	if( p_ring_buffer->p_head == NULL)
	{
		return -2;
	}
	p_ring_buffer->buf_size = size;
	p_ring_buffer->data_size = 0;
	p_ring_buffer->p_tail = p_ring_buffer->p_head + p_ring_buffer->buf_size;	
	p_ring_buffer->p_write = p_ring_buffer->p_head;
	p_ring_buffer->p_read = p_ring_buffer->p_head;
	p_ring_buffer->tag = RING_BUFFER_TAG;

	if( rb_mutex_init(&(p_ring_buffer->mutex)) != 0)
	{
		rb_free(p_ring_buffer->p_head);
		rb_bzero(p_ring_buffer, sizeof(ring_buffer_t));
		return -3;
	}

	#ifdef RING_BUFFER_DEBUG
	ring_buffer_debug(p_ring_buffer);
	#endif	
	return 0;
}

int ring_buffer_init_with_mem(ring_buffer_t* p_ring_buffer, void* mem_addr, unsigned int size)
{
	if(p_ring_buffer == NULL)
	{
		return -1;
	}
	p_ring_buffer->p_head = (unsigned char*)mem_addr;
	if( p_ring_buffer->p_head == NULL)
	{
		return -2;
	}
	p_ring_buffer->buf_size = size;
	p_ring_buffer->data_size = 0;
	p_ring_buffer->p_tail = p_ring_buffer->p_head + p_ring_buffer->buf_size;	
	p_ring_buffer->p_write = p_ring_buffer->p_head;
	p_ring_buffer->p_read = p_ring_buffer->p_head;
	p_ring_buffer->tag = RING_BUFFER_TAG;

	if( rb_mutex_init(&(p_ring_buffer->mutex)) != 0)
	{
		rb_free(p_ring_buffer->p_head);
		rb_bzero(p_ring_buffer, sizeof(ring_buffer_t));
		return -3;
	}

#ifdef RING_BUFFER_DEBUG
	ring_buffer_debug(p_ring_buffer);
#endif	
	return 0;
}



int ring_buffer_release(ring_buffer_t* p_ring_buffer)
{
	if(p_ring_buffer == NULL || p_ring_buffer->tag != RING_BUFFER_TAG)
	{
		return -1;
	}
	if(p_ring_buffer->p_head != NULL)
	{
		rb_free(p_ring_buffer->p_head);
	}

	if( rb_mutex_destroy( &(p_ring_buffer->mutex)) != 0)
	{
		rb_bzero(p_ring_buffer, sizeof(ring_buffer_t));
		return -2;
	}
	rb_bzero(p_ring_buffer, sizeof(ring_buffer_t));

	#ifdef RING_BUFFER_DEBUG
	ring_buffer_debug(p_ring_buffer);
	#endif	
	
	return 0;
}

int ring_buffer_release_with_mem(ring_buffer_t* p_ring_buffer)
{
	if(p_ring_buffer == NULL || p_ring_buffer->tag != RING_BUFFER_TAG)
	{
		return -1;
	}
	//if(p_ring_buffer->p_head != NULL)
	//{
		//rb_free(p_ring_buffer->p_head);
	//}

	if( rb_mutex_destroy( &(p_ring_buffer->mutex)) != 0)
	{
		rb_bzero(p_ring_buffer, sizeof(ring_buffer_t));
		return -2;
	}
	rb_bzero(p_ring_buffer, sizeof(ring_buffer_t));

#ifdef RING_BUFFER_DEBUG
	ring_buffer_debug(p_ring_buffer);
#endif	
	
	return 0;
}


int	ring_buffer_is_init(ring_buffer_t* p_ring_buffer)
{
	if( p_ring_buffer == NULL || p_ring_buffer->tag != RING_BUFFER_TAG)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int ring_buffer_flush(ring_buffer_t* p_ring_buffer)
{
	if( p_ring_buffer == NULL || p_ring_buffer->tag != RING_BUFFER_TAG)
	{
		RingBuffer_Error(("ring_buffer_flush error\n"));
		return -1;
	}
	
	rb_mutex_lock( &(p_ring_buffer->mutex));
	p_ring_buffer->p_read = p_ring_buffer->p_head;
	p_ring_buffer->p_write = p_ring_buffer->p_head;
	p_ring_buffer->data_size = 0;
	rb_mutex_unlock(&(p_ring_buffer->mutex));

	return 0;
}


int ring_buffer_write(ring_buffer_t* p_ring_buffer, unsigned char* p_data, unsigned int size)
{
	if(p_ring_buffer == NULL || p_ring_buffer->tag != RING_BUFFER_TAG)
	{
		return -1;
	}

	rb_mutex_lock( &(p_ring_buffer->mutex));
	
	if(p_ring_buffer->buf_size - p_ring_buffer->data_size < size)
	{
		rb_mutex_unlock(&(p_ring_buffer->mutex));
		return -2;
	}
	if( (p_ring_buffer->p_write >= p_ring_buffer->p_read && p_ring_buffer->p_tail - p_ring_buffer->p_write >= size)
		|| (p_ring_buffer->p_write < p_ring_buffer->p_read))
	{
		rb_memcpy( p_ring_buffer->p_write, p_data, size);
		p_ring_buffer->p_write += size;
		if( p_ring_buffer->p_write == p_ring_buffer->p_tail)
		{
			p_ring_buffer->p_write = p_ring_buffer->p_head;
		}
	}
	else
	{
		unsigned int copy_size1;
		unsigned int copy_size2;

		copy_size1 = p_ring_buffer->p_tail - p_ring_buffer->p_write;
		rb_memcpy( p_ring_buffer->p_write, p_data, copy_size1);
		p_ring_buffer->p_write = p_ring_buffer->p_head;

		copy_size2 = size - copy_size1;
		rb_memcpy( p_ring_buffer->p_write, p_data+copy_size1, copy_size2);
		p_ring_buffer->p_write += copy_size2;	
	}
	p_ring_buffer->data_size += size;

	#ifdef RING_BUFFER_DEBUG
	ring_buffer_debug(p_ring_buffer);
	#endif	

	rb_mutex_unlock(&(p_ring_buffer->mutex));
	return 0;
}

int ring_buffer_read(ring_buffer_t* p_ring_buffer, unsigned char* p_data, unsigned int size)
{
	if(p_ring_buffer == NULL || p_ring_buffer->tag != RING_BUFFER_TAG)
	{
		return -1;
	}

	rb_mutex_lock(&(p_ring_buffer->mutex));
	
	if( p_ring_buffer->data_size < size)
	{
		rb_mutex_unlock(&(p_ring_buffer->mutex));
		return -2;
	}

	if( (p_ring_buffer->p_read >= p_ring_buffer->p_write && p_ring_buffer->p_tail - p_ring_buffer->p_read >= size)
		||(p_ring_buffer->p_read < p_ring_buffer->p_write))
	{
		rb_memcpy( p_data, p_ring_buffer->p_read, size);
		p_ring_buffer->p_read += size;
		if( p_ring_buffer->p_read == p_ring_buffer->p_tail)
		{
			p_ring_buffer->p_read = p_ring_buffer->p_head;
		}
	}
	else
	{
		unsigned int copy_size1;
		unsigned int copy_size2;

		copy_size1 = p_ring_buffer->p_tail - p_ring_buffer->p_read;
		rb_memcpy( p_data, p_ring_buffer->p_read, copy_size1);
		p_ring_buffer->p_read = p_ring_buffer->p_head;

		copy_size2 = size - copy_size1;
		rb_memcpy( p_data + copy_size1, p_ring_buffer->p_read, copy_size2);
		p_ring_buffer->p_read += copy_size2;
	}
	p_ring_buffer->data_size -= size;
		
	#ifdef RING_BUFFER_DEBUG
	ring_buffer_debug(p_ring_buffer);
	#endif

	rb_mutex_unlock(&(p_ring_buffer->mutex));
	return 0;
}


int ring_buffer_freesize(ring_buffer_t* p_ring_buffer, unsigned int* p_freesize)
{
	if(p_ring_buffer == NULL || p_ring_buffer->tag != RING_BUFFER_TAG)
	{
		return -1;
	}
	rb_mutex_lock(&(p_ring_buffer->mutex));
	*p_freesize = p_ring_buffer->buf_size - p_ring_buffer->data_size;
	rb_mutex_unlock(&(p_ring_buffer->mutex));
	return 0;
}

int ring_buffer_datasize(ring_buffer_t* p_ring_buffer, unsigned int* p_datasize)
{
	if(p_ring_buffer == NULL || p_ring_buffer->tag != RING_BUFFER_TAG)
	{
		return -1;
	}
	rb_mutex_lock(&(p_ring_buffer->mutex));
	*p_datasize = p_ring_buffer->data_size;
	rb_mutex_unlock(&(p_ring_buffer->mutex));
	return 0;
}


#ifdef RING_BUFFER_DEBUG
int ring_buffer_debug(ring_buffer_t* p_ring_buffer)
{
	printf("ring_buffer head: 0x%x\n", p_ring_buffer->p_head);
	printf("ring_buffer tail: 0x%x\n", p_ring_buffer->p_tail);
	printf("ring_buffer data: %u\n", p_ring_buffer->data_size);
	printf("ring_buffer write: %d\n", p_ring_buffer->p_write - p_ring_buffer->p_head);
	printf("ring_buffer read: %d\n", p_ring_buffer->p_read - p_ring_buffer->p_head);
	printf("ring_buffer size: %u\n\n", p_ring_buffer->buf_size);
	return 0;
}

int ring_buffer_test( void)
{
	ring_buffer_t	ring_buffer;
	unsigned char	buf[20];
	unsigned char	buf1[20];
	int				iret;

	ring_buffer_init( &ring_buffer, 20);

	for(;;)
	{
		iret = ring_buffer_write(&ring_buffer, buf, 8);
		if( iret != 0)
		{
			break;
		}
	}
	ring_buffer_read( &ring_buffer, buf1, 4);

	
	iret = ring_buffer_write(&ring_buffer, buf, 8);
	if( iret != 0)
	{
		exit(1);
	}


	iret = ring_buffer_read(&ring_buffer, buf1, 8);
	if( iret != 0)
	{
		exit(1);
	}
	iret = ring_buffer_read(&ring_buffer, buf1, 6);
	if( iret != 0)
	{
		exit(1);
	}
			iret = ring_buffer_read(&ring_buffer, buf1, 6);
	if( iret != 0)
	{
		exit(1);
	}

		
	ring_buffer_release(&ring_buffer);
	return 0;
}


int main()
{
	ring_buffer_test();
	return 0;
}
#endif
