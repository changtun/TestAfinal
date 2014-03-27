/* $Id: mempool.h,v 1.3 2003/10/20 15:03:19 chrisgreen Exp $ */
/*
** Copyright (C) 2002 Martin Roesch <roesch@sourcefire.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#ifndef _MEMPOOL_H
#define _MEMPOOL_H

#include "sf_sdlist.h"

#include "osapi.h"


#define MEMPOOL_MAX_NAME_LEN		32
typedef unsigned int PoolCount;

typedef struct _MemBucket
{
    SDListItem *key;
    int used;
    void *data;
} MemBucket;

typedef struct _MemPool
{
    void **datapool; /* memory buffer for MemBucket->data */
    
    MemBucket *bucketpool; /* memory buffer */

    SDListItem *listpool; /* list of things to use for memory bufs */

    PoolCount free; /*  free block count */
    PoolCount used;  /* used block count */

    PoolCount total;
    
    sfSDList free_list;
    sfSDList used_list;
    
    size_t obj_size;
	char	name[MEMPOOL_MAX_NAME_LEN];
	unsigned int sem;
} MemPool;

typedef void* (*alloc_func_t)(unsigned int size);
typedef void  (*dealloc_func_t)(void* p);

#define mempool_sem_create(p_sem,name,value)	OS_SemCreate(p_sem, name, value, 0)
#define mempool_sem_delete(sem)					OS_SemDelete(sem)
#define mempool_sem_wait(sem)					OS_SemWait(sem)
#define mempool_sem_signal(sem)					OS_SemSignal(sem)

//add liuzhiyuan
int mempool_set_alloc_func(alloc_func_t alloc_func, dealloc_func_t dealloc_func);

int mempool_init(MemPool *mempool, const char* name, PoolCount num_objects, size_t obj_size);
int mempool_destroy(MemPool *mempool);

void* mempool_alloc(MemPool *mempool);
int mempool_free(MemPool *mempool, void* p);

int mempool_test( void);

void mempool_bug(MemPool *mempool);


#endif /* _MEMPOOL_H */

