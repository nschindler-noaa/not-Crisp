/** Generic doubly-linked list type */

#ifndef LIST_H
#define LIST_H

#ifndef NULL
#define NULL 0
#endif

/** \brief Generic list item for general use */

struct _list {
    struct _list *next;
    struct _list *prev;
    void *item;
};

typedef struct _list LIST;


/* user callable routines */

extern LIST *make_list_entry();
extern void  free_list   ( /* LIST *listp, void (*user_fn)() */  );
extern void list_append  ( /* LIST **headp, LIST *node */ 	 );
extern void list_insert  ( /* LIST **headp, LIST *node */  	 );
extern LIST *list_insert_item  ( /* LIST **headp, void *item */  );
extern void list_delete  ( /* LIST **headp, LIST *entry,
			      void (*del_fn)(void *item) */ 	 );
extern LIST *item_in_list( /* LIST *listp, void *item */  	 );
extern LIST *list_find   ( /* LIST *listp, void *item,
			      int (*comp_fn)(void *item1, void *item2) */ );

#endif /*LIST_H*/
