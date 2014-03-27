#ifndef _LINUX_LIST_H
#define _LINUX_LIST_H
#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifdef __KERNEL__



#define  smp_wmb()      

#define inline
#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif


/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})
	

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

struct pvlist_head {
	struct pvlist_head *next, *prev;
};

static inline void prefetch(const void *x) {;}

#define PVLIST_HEAD_INIT(name) { &(name), &(name) }

#define PVLIST_HEAD(name) \
	struct pvlist_head name = PVLIST_HEAD_INIT(name)

static inline void PV_INIT_LIST_HEAD(struct pvlist_head *list)
{
	list->next = list;
	list->prev = list;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
#ifndef CONFIG_DEBUG_LIST
static inline void __pvlist_add(struct pvlist_head *new,
			      struct pvlist_head *prev,
			      struct pvlist_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}
#else
extern void __pvlist_add(struct pvlist_head *new,
			      struct pvlist_head *prev,
			      struct pvlist_head *next);
#endif

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
#ifndef CONFIG_DEBUG_LIST
static inline void pvlist_add(struct pvlist_head *new, struct pvlist_head *head)
{
	__pvlist_add(new, head, head->next);
}
#else
extern void pvlist_add(struct pvlist_head *new, struct pvlist_head *head);
#endif


/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void pvlist_add_tail(struct pvlist_head *new, struct pvlist_head *head)
{
	__pvlist_add(new, head->prev, head);
}


/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __pvlist_del(struct pvlist_head * prev, struct pvlist_head * next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
#ifndef CONFIG_DEBUG_LIST
static inline void pvlist_del(struct pvlist_head *entry)
{
	__pvlist_del(entry->prev, entry->next);
//	entry->next = NULL;
//	entry->prev = NULL;
}
#else
extern void pvlist_del(struct pvlist_head *entry);
#endif



/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void pvlist_replace(struct pvlist_head *old,
				struct pvlist_head *new)
{
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

static inline void pvlist_replace_init(struct pvlist_head *old,
					struct pvlist_head *new)
{
	pvlist_replace(old, new);
	PV_INIT_LIST_HEAD(old);
}


/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void pvlist_del_init(struct pvlist_head *entry)
{
	__pvlist_del(entry->prev, entry->next);
	PV_INIT_LIST_HEAD(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void pvlist_move(struct pvlist_head *list, struct pvlist_head *head)
{
	__pvlist_del(list->prev, list->next);
	pvlist_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void pvlist_move_tail(struct pvlist_head *list,
				  struct pvlist_head *head)
{
	__pvlist_del(list->prev, list->next);
	pvlist_add_tail(list, head);
}

/**
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int pvlist_is_last(const struct pvlist_head *list,
				const struct pvlist_head *head)
{
	return list->next == head;
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int pvlist_empty(const struct pvlist_head *head)
{
	return head->next == head;
}

/**
 * list_empty_careful - tests whether a list is empty and not being modified
 * @head: the list to test
 *
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is list_del_init(). Eg. it cannot be used
 * if another CPU could re-list_add() it.
 */
static inline int pvlist_empty_careful(const struct pvlist_head *head)
{
	struct pvlist_head *next = head->next;
	return (next == head) && (next == head->prev);
}

static inline void __pvlist_splice(struct pvlist_head *list,
				 struct pvlist_head *head)
{
	struct pvlist_head *first = list->next;
	struct pvlist_head *last = list->prev;
	struct pvlist_head *at = head->next;

	first->prev = head;
	head->next = first;

	last->next = at;
	at->prev = last;
}

/**
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void pvlist_splice(struct pvlist_head *list, struct pvlist_head *head)
{
	if (!pvlist_empty(list))
		__pvlist_splice(list, head);
}

//{{{ add by CK
/**
 * list_splice - exchange two lists
 * @list_p: the list one.
 * @list_n: the list two.
 */
static inline void pvlist_exchange_head(struct pvlist_head *list_p, struct pvlist_head *list_n)
{
	struct pvlist_head temp = *list_n;
	list_n->next->prev = list_p;
	list_n->prev->next = list_p;
	list_p->next->prev = list_n;
	list_p->prev->next = list_n;
	list_n->next = list_p->next;
	list_n->prev = list_p->prev;
	list_p->next = temp.next;
	list_p->prev = temp.prev;
}

//}}} add by CK
//{{{ add by LYN
static inline void pvlist_swap(struct pvlist_head *list_p,
				struct pvlist_head *list_n)
{
	list_p->next = list_n->next;
	list_p->next->prev = list_p;
	list_n->prev = list_p->prev;
	list_n->prev->next = list_n;
	list_p->prev = list_n;
	list_n->next = list_p;
}

typedef int		(*listCompFunc)	(struct pvlist_head *list_p,struct pvlist_head *list_n);
static inline void pvlist_sort(struct pvlist_head *list,listCompFunc CompFunc) 
{
	struct pvlist_head		*pos;
	unsigned char						SwapFlag = 1;
	
	while(SwapFlag)
	{
		SwapFlag = 0;
		pos = list->next;
		while(pos->next != list)
		{
			if((*CompFunc)(pos,pos->next))
			{
				pvlist_swap(pos,pos->next);
				SwapFlag = 1;
			}
			else
				pos= pos->next;			
		}
	}
}
//add by LYN}}}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void pvlist_splice_init(struct pvlist_head *list,
				    struct pvlist_head *head)
{
	if (!pvlist_empty(list)) {
		__pvlist_splice(list, head);
		PV_INIT_LIST_HEAD(list);
	}
}



/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define pvlist_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define pvlist_first_entry(ptr, type, member) \
	pvlist_entry((ptr)->next, type, member)

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define pvlist_for_each(pos, head) \
	for (pos = (head)->next; prefetch(pos->next), pos != (head); \
        	pos = pos->next)

/**
 * __list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 *
 * This variant differs from list_for_each() in that it's the
 * simplest possible list iteration code, no prefetching is done.
 * Use this for code that knows the list to be very short (empty
 * or 1 entry) most of the time.
 */
#define __pvlist_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define pvlist_for_each_prev(pos, head) \
	for (pos = (head)->prev; prefetch(pos->prev), pos != (head); \
        	pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define pvlist_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define pvlist_for_each_entry(pos, head, member)				\
	for (pos = pvlist_entry((head)->next, typeof(*pos), member);	\
	     prefetch(pos->member.next), &pos->member != (head); 	\
	     pos = pvlist_entry(pos->member.next, typeof(*pos), member))



#else
#warning "don't include kernel headers in userspace"
#endif /* __KERNEL__ */
#endif
