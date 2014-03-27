/* $Id: mempool.c,v 1.7 2004/03/23 15:34:45 chris_reid Exp $ */
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif



//#define TEST_MEMPOOL

/*
 * This is used to allocate a list of fixed size objects in a static
 * memory pool aside from the concerns of alloc/free
 */

/* $Id: mempool.c,v 1.7 2004/03/23 15:34:45 chris_reid Exp $ */
#include "mempool.h"

static alloc_func_t			g_alloc_func;
static dealloc_func_t		g_dealloc_func;


/* Function: int mempool_set_alloc_func(alloc_func_t alloc_func, 
 *                                                     dealloc_func_t dealloc_func)
 *
 * Purpose: 	设置mempool 使用的分配和释放内存的接口函数，设为NULL 则使用默认接口
 * Args: 		alloc_func		分配内存函数
 *			dealloc_func		释放内存函数
 * Returns: 0 on success, 1 on failure
 */ 
int mempool_set_alloc_func(alloc_func_t alloc_func, dealloc_func_t dealloc_func)
{
	g_alloc_func = alloc_func;
	g_dealloc_func = dealloc_func;
	return 0;
}


/* Function: int mempool_init(MemPool *mempool,
 *                            PoolCount num_objects, size_t obj_size)
 * 
 * Purpose: initialize a mempool object and allocate memory for it
 * Args: mempool - pointer to a MemPool struct
 *       num_objects - number of items in this pool
 *       obj_size    - size of the items
 * 
 * Returns: 0 on success, 1 on failure
 */ 

int mempool_init(MemPool *mempool, const char* name, PoolCount num_objects, size_t obj_size)
{
    PoolCount i;
    
    if(mempool == NULL || name == NULL)
        return 1;

    if(num_objects < 1)
        return 1;

    if(obj_size < 1)
        return 1;

	memset(mempool,0,sizeof(MemPool));
	strncpy(mempool->name, name, ( strlen(name)>=sizeof(mempool->name) ? (sizeof(mempool->name)-1) : strlen(name)) );

	if( mempool_sem_create( &(mempool->sem), mempool->name, 1) != 0)
	{
		return -1;
	}
	
	mempool->obj_size = obj_size;
    
    /* this is the basis pool that represents all the *data pointers
       in the list */
    if( g_alloc_func != NULL)
    	mempool->datapool = g_alloc_func(num_objects*obj_size);
	else
    	mempool->datapool = calloc(num_objects, obj_size);
    
    //printf("mempool->datapool: 0x%x   size: 0x%x   obj: 0x%x\n",mempool->datapool,num_objects*obj_size, obj_size);
	
    if(mempool->datapool == NULL)
    {
    	mempool_sem_delete(mempool->sem);
        return 1;
    }

	if( g_alloc_func != NULL)
		 mempool->listpool = g_alloc_func(num_objects*sizeof(SDListItem));
	else
    	mempool->listpool = calloc(num_objects, sizeof(SDListItem));

    if(mempool->listpool == NULL)
    {
        /* well, that sucked, lets clean up */
        fprintf(stderr, "mempool: listpool is null\n");
        free(mempool->datapool);
		mempool_sem_delete(mempool->sem);
        return 1;
    }

	if( g_alloc_func != NULL)
		mempool->bucketpool = g_alloc_func(num_objects*sizeof(MemBucket));
	else
    	mempool->bucketpool = calloc(num_objects, sizeof(MemBucket));

    if(mempool->bucketpool == NULL)
    {
        fprintf(stderr, "mempool: bucketpool is null\n");
        free(mempool->datapool);
        free(mempool->listpool);
		mempool_sem_delete(mempool->sem);
        return 1;
    }

    /* sets up the 2 memory lists */
    if(sf_sdlist_init(&mempool->used_list, NULL))
    {
        fprintf(stderr, "mempool: used_list failed\n");
        free(mempool->datapool);
        free(mempool->listpool);
        free(mempool->bucketpool);
		mempool_sem_delete(mempool->sem);
        return 1;
    }

    if(sf_sdlist_init(&mempool->free_list, NULL))
    {
        fprintf(stderr, "mempool: free_list failed\n");
        free(mempool->datapool);
        free(mempool->listpool);
        free(mempool->bucketpool);
		mempool_sem_delete(mempool->sem);
        return 1;
    }


    for(i=0; i<num_objects; i++)
    {
        SDListItem *itemp;
        MemBucket *bp;

        bp = &mempool->bucketpool[i];
        itemp = &mempool->listpool[i];
        
        /* each bucket knows where it resides in the list */
        bp->key = itemp;

#ifdef TEST_MEMPOOL        
        printf("listpool: %p itemp: %p diff: %u\n",
               mempool->listpool, itemp,
               (((char *) itemp) -
                ((char *) mempool->listpool)));
#endif /* TEST_MEMPOOL */
               
        bp->data = ((char *) mempool->datapool) + (i * mempool->obj_size);
        
#ifdef TEST_MEMPOOL        
        printf("datapool: %p bp.data: %p diff: %u\n",
               mempool->datapool,
               mempool->datapool + (i * mempool->obj_size),
               (((char *) bp->data) -
                ((char *) mempool->datapool)));
#endif /* TEST_MEMPOOL */
        

        if(sf_sdlist_append(&mempool->free_list,                           
                            &mempool->bucketpool[i],
                            &mempool->listpool[i]))
        {
            fprintf(stderr, "mempool: free_list_append failed\n");
            free(mempool->datapool);
            free(mempool->listpool);
            free(mempool->bucketpool);
			mempool_sem_delete(mempool->sem);
            return 1;
        }

        mempool->free++;
    }

    mempool->used = 0;
    mempool->total = num_objects;

    return 0;
}

/* Function: int mempool_destroy(MemPool *mempool) 
 * 
 * Purpose: destroy a set of mempool objects
 * Args: mempool - pointer to a MemPool struct
 * 
 * Returns: 0 on success, 1 on failure
 */ 
int mempool_destroy(MemPool *mempool)
{
    if(mempool == NULL)
        return 1;

	if( g_dealloc_func != NULL)
	{
		g_dealloc_func(mempool->listpool);
		g_dealloc_func(mempool->datapool);
		g_dealloc_func(mempool->bucketpool);
	}
	else
	{
	    free(mempool->listpool);
		free(mempool->datapool);
		free(mempool->bucketpool);
	}

	mempool_sem_delete(mempool->sem);

    /* TBD - callback to free up every stray pointer */
    memset(mempool, 0, sizeof(MemPool));
    
    return 0;    
}


/* Function: MemBucket *mempool_alloc(MemPool *mempool);
 * 
 * Purpose: allocate a new object from the mempool
 * Args: mempool - pointer to a MemPool struct
 * 
 * Returns: a pointer to the mempool object on success, NULL on failure
 */ 
void* mempool_alloc(MemPool *mempool)
{
    SDListItem *li = NULL;
    MemBucket *b;
    
    if(mempool == NULL)
    {
    	printf(" mempool == NULL \n");
        return NULL;
    }

    if(mempool->free <= 0)
    {
    	printf(" mempool == NULL \n");
        return NULL;
    }

	mempool_sem_wait(mempool->sem);

    /* get one item off the free_list,
       put one item on the usedlist
     */

    li = mempool->free_list.head;
	
	//printf(" li: 0x%x\n", mempool->free_list.head);

    mempool->free--;
    if((li == NULL) || sf_sdlist_remove(&mempool->free_list, li))
    {
        printf("Failure on sf_sdlist_remove\n");
		mempool_sem_signal(mempool->sem);
        return NULL;
    }
        
    
    mempool->used++;

    if(sf_sdlist_append(&mempool->used_list, li->data, li))
    {
        printf("Failure on sf_sdlist_append\n");
		mempool_sem_signal(mempool->sem);
        return NULL;
    }

	{/*
		int 		j = 0;
		//for( j =0; j< mempool->total; j++)
			for( j =0; j< 5; j++)
		{
			printf("%d   0x%x\n", j, mempool->listpool[j].data);
		}
	*/}

	//printf("-------li->data: 0x%x\n",li->data);
	
    /* TBD -- make configurable */
    b = li->data;

	//printf("mempool_alloc----------	 0x%x   %d\n", b->data, mempool->obj_size);

    memset(b->data, 0, mempool->obj_size);

	//printf("--   0x%x   %d\n", mempool->listpool[0].data, mempool->obj_size);

	//printf("------------------------------mempool used %d  free %d\n", mempool->used, mempool->free);

	mempool_sem_signal(mempool->sem);
	//printf(" ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^mempool_alloc ok 0x%x  free: %d\n", b->data, mempool->free);
	return b->data;
}

void mempool_bug(MemPool *mempool)
{
	int 		j = 0;
	
	for( j =0; j< 5; j++)
	{
		printf("%d	 0x%x\n", j, (S32)mempool->listpool[j].data);
	}
}


int mempool_free(MemPool *mempool, void* p)
{   
	MemBucket*		obj;
	unsigned int	index;

	mempool_sem_wait(mempool->sem);

	if(( (unsigned int)p -  (unsigned int)(mempool->datapool)) % mempool->obj_size != 0)
	{
	printf("failure   0x%x  0x%x  0x%x ", (unsigned int)p, (unsigned int)(mempool->datapool), mempool->obj_size);
		mempool_sem_signal(mempool->sem);
		return -1;
	}
	index = ( (unsigned int)p - (unsigned int)(mempool->datapool)) / mempool->obj_size;
	obj = &(mempool->bucketpool[index]);

	//printf("mempool_free------free membucket: %d   0x%x  0x%x\n",index, obj->data, p);
	
    if(sf_sdlist_remove(&mempool->used_list, obj->key))
    {
        printf("failure on remove from used_list");
		mempool_sem_signal(mempool->sem);
        return -1;
    }
    
    mempool->used--;

    /* put the address of the membucket back in the list */
    if(sf_sdlist_append(&mempool->free_list, obj, obj->key))
    {
        printf("failure on add to free_list");
		mempool_sem_signal(mempool->sem);
        return -1;
    }

	mempool_sem_signal(mempool->sem);

    mempool->free++;   
		//printf("------------------------------mempool used %d  free %d\n", mempool->used, mempool->free);
    return 0;
}







#ifdef TEST_MEMPOOL

#define SIZE 36
int mempool_test(void)
{
    MemPool test;
    MemBucket *bucks[SIZE];
    MemBucket *bucket = NULL;
    int i;
    long long a = 1;

    //char *stuffs[4] = { "eenie", "meenie", "minie", "moe" };
    char *stuffs2[36] =
        {  "1eenie", "2meenie", "3minie", " 4moe",
           "1xxxxx", "2yyyyyy", "3zzzzz", " 4qqqq",
           "1eenie", "2meenie", "3minie", " 4moe",
           "1eenie", "2meenie", "3minie", " 4moe",
           "1eenie", "2meenie", "3minie", " 4moe",
           "1eenie", "2meenie", "3minie", " 4moe",
           "1eenie", "2meenie", "3minie", " 4moe",
           "1eenie", "2meenie", "3minie", " 4moe",
           "1eenie", "2meenie", "3minie", " 4moe"
        };
    
    if(mempool_init(&test, "test_mempool",36, 256))
    {
        printf("error in mempool initialization\n");
    }

    for(i = 0; i < 36; i++)
    {
        if((bucks[i] = mempool_alloc(&test)) == NULL)
        {
            printf("error in mempool_alloc: i=%d\n", i);
            continue;
        }

        bucket = bucks[i];

        bucket->data = strncpy(bucket->data, stuffs2[i], 256);
        printf("bucket->key: %p\n", bucket->key);
        printf("bucket->data: %s\n", (char *) bucket->data);
    }

    for(i = 0; i < 2; i++)
    {
        mempool_free(&test, bucks[i]);
        bucks[i] = NULL;
    }
    
    for(i = 0; i < 14; i++)
    {
        if((bucks[i] = mempool_alloc(&test)) == NULL)
        {
            printf("error in mempool_alloc: i=%d\n", i);
            continue;
        }

        bucket = bucks[i];

        bucket->data = strncpy(bucket->data, stuffs2[i], 256);
        printf("bucket->key: %p\n", bucket->key);
        printf("bucket->data: %s\n", (char *) bucket->data);
    }

    printf("free: %u, used: %u\n", test.free, test.used);

    
    return 0;
}

#endif /* TEST_MEMPOOL */





int new_mempool_test(void)
{
	MemPool test;
	int		iret = 0;
	void*	array[40];
	int		i = 0;

	memset(array,0,sizeof(array));

	
	
	iret = mempool_init(&test, "ntest_mempool",36, 256);
	if( iret != 0)
	{
		printf("mempool_init faild\n");
		return -1;
	}

	i = 0;
	while(1)
	{
		array[i] = mempool_alloc( &test);
		if( array[i] == NULL)
		{
			break;
		}
		printf("mempool_alloc ok %d\n", i);
		i++;
	}

	i = 0;
	while(1)
	{
		iret = mempool_free( &test, array[i]);
		if( iret != 0)
		{
			break;
		}
		array[i] = NULL;
		printf("mempool_free ok %d\n", i);
		i++;
	}

	
	i = 0;
	while(1)
	{
		array[i] = mempool_alloc( &test);
		if( array[i] == NULL)
		{
			break;
		}
		printf("mempool_alloc ok %d\n", i);
		i++;
	}


	return 0;
}





