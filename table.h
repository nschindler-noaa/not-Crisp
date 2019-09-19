/** \file
 *
 *	\brief Designed to be configurable by substituting the TableDatum and
 * HashKey typedefs for your own specific ones.  I hope I succeeded.
 *
 *	Default initialization (all init function parameters == 0) gives
 * a string hash table of size (2 to the power of DEFAULT_BITS (#defined
 * as 16 in table.c)), with a simple O(Length[string]) hash function,
 * which behaves quite well on _almost_ equivalent text strings.
 */

typedef void		*TableDatum;
typedef void		*HashKey;

typedef unsigned long	HashIndex;

/* Buckets are linked lists, for now */
typedef struct bucket {
    TableDatum		datum;
    struct bucket	*next;
} TableBucket;

typedef struct table {
    unsigned long	insertion_count;
    unsigned long	collision_count;

    HashIndex	hash_size;		/* The number of buckets */
    TableBucket **hash_buckets;		/* An array of TableBuckets 
					 * (size hash_size) */
#ifdef __STDC__
    HashIndex	(*hash)(HashKey);
    HashKey	(*get_key)(TableDatum);
    int		(*comparison)(HashKey, HashKey);
#else
    HashIndex	(*hash)();		/* The table's hash function */
    HashKey	(*get_key)();		/* Get a TableDatum's HashKey
					 * (defaults to "return datum") */
    int		(*comparison)();	/* Compare two HashKeys, using the
					 * same returns as strcmp(3). */
#endif
} Table;

#ifdef __STDC__
Table		*init_table (HashIndex, HashIndex (*)(TableDatum),
	HashKey (*)(TableDatum), int (*)(HashKey, HashKey));
TableDatum	table_insert(Table *, TableDatum);
TableDatum	table_lookup(Table *, HashKey);
TableDatum	table_delete(Table *, TableDatum);
#else
Table		*init_table();
TableDatum	table_insert();
TableDatum	table_lookup();
TableDatum	table_delete();
#endif

