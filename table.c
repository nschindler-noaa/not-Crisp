/** \file
 *
 *	\brief A simple hashed lookup table.  It makes the assumption that
 * equivalent pointers will not necessarily have the same address.
 */

#include <stdlib.h>
#include <string.h>
#include "table.h"
#include "strtools.h"

/* DEFAULT_BITS Shouldn't be > sizeof(HashIndex) */
#define DEFAULT_BITS		16
#define DEFAULT_MASK		0x007f0000
#define DEFAULT_SIZE		(1 << DEFAULT_BITS)
#define BIT_MULTIPLIER		5

#define BUCKET(table, index)		(table)->hash_buckets[index]

static struct bucket   *new_bucket();
static void		bucket_free();
static TableDatum	find_datum_in_bucket();
static TableBucket    **find_bucket_pointer();

static HashKey		default_key();
static HashIndex	default_hash();

Table *init_table(table_size, hash_function, key_function, test_function)
	HashIndex	table_size;
	HashIndex	(*hash_function)();
	HashKey		(*key_function)();
	int		(*test_function)();
{
    Table *new_table;

    if (!(new_table = (Table *) malloc(sizeof(Table))))
	return 0;

    bzero((char *) new_table, sizeof(Table));
    if (!table_size)	table_size	= DEFAULT_SIZE;
    if (!hash_function) hash_function	= default_hash;
    if (!key_function)  key_function	= default_key;

#ifndef NO_COMPARISON
    if (!test_function) test_function   = strcmp;
#endif

    if (!(new_table->hash_buckets = (TableBucket **)
	  malloc(table_size * sizeof(TableBucket *)))) {
	free(new_table);
	return 0;
    }
    bzero((char *) new_table->hash_buckets, table_size * sizeof(TableBucket *));

    new_table->hash_size	= table_size;
    new_table->hash		= hash_function;
    new_table->get_key		= key_function;
    new_table->comparison	= test_function;
    return new_table;
}

TableDatum table_insert(table, datum)
	Table		*table;
	TableDatum	datum;
{
    HashIndex	hash_index;
    TableDatum	duplicate;
    HashKey	key;
    TableBucket	*bucket;

    if ((hash_index = (*table->hash)(key = (*table->get_key)(datum)))
	>= table->hash_size)
	return 0;

    if (BUCKET(table, hash_index)) {
#ifdef NO_COMPARISON
	if (!table->comparison) {
	    bucket_free(BUCKET(table, hash_index));
	    BUCKET(table, hash_index) = 0;
	}
#else
	if ((duplicate = find_datum_in_bucket(table, hash_index, key)) != NULL)
	    return duplicate;
	++table->collision_count;
#endif
    }

    ++table->insertion_count;

    if (!(bucket = new_bucket(datum)))
	return 0;
    bucket->next = BUCKET(table, hash_index);
    BUCKET(table, hash_index) = bucket;
    return datum;
}

TableDatum table_lookup(table, key)
	Table		*table;
	HashKey		key;
{
    HashIndex hash_index;

    if ((hash_index = (*table->hash)(key)) >= table->hash_size)
	return 0;
    return find_datum_in_bucket(table, hash_index, key);
}

TableDatum table_delete(table, datum)
	Table		*table;
	TableDatum	datum;
{
    HashIndex		hash_index;
    HashKey		hash_key;
    TableBucket	      **bucket_ptr;
    TableBucket	       *old_bucket;

    hash_key = (*table->get_key)(datum);
    if ((hash_index = (*table->hash)(hash_key)) >= table->hash_size)
	return 0;
    bucket_ptr = find_bucket_pointer(table, hash_index, hash_key);
    if (!*bucket_ptr) return 0;
    old_bucket = *bucket_ptr;
    *bucket_ptr = (*bucket_ptr)->next;
    bucket_free(old_bucket);
    return datum;
}

static TableBucket *new_bucket(datum)
	TableDatum datum;
{
    TableBucket *alloced_bucket;

    if (!(alloced_bucket = (TableBucket *) malloc(sizeof(TableBucket))))
	return 0;
    bzero((char *)alloced_bucket, sizeof(TableBucket));
    alloced_bucket->datum = datum;
    return alloced_bucket;
}

/*
 * Bug: this does not free the datum
 */
static void bucket_free(bucket)
	TableBucket	*bucket;
{
    free(bucket);
}

static TableBucket **find_bucket_pointer(table, index, key)
	Table	       *table;
	HashIndex	index;
	HashKey		key;
{
    TableBucket		**bucket_ptr;

    if (!table) return 0;
    bucket_ptr = &table->hash_buckets[index];

#ifdef NO_COMPARISON
    if (!table->comparison)
	return bucket_ptr;
#endif

    while (*bucket_ptr) {
	if (!(*table->comparison)(key,(*table->get_key)((*bucket_ptr)->datum)))
	    return bucket_ptr;
	bucket_ptr = &(*bucket_ptr)->next;
    }
    return bucket_ptr;
}

static TableDatum find_datum_in_bucket(table, index, key)
	Table		*table;
	HashIndex	index;
	HashKey		key;
{
    TableBucket		**bucket;

    bucket = find_bucket_pointer(table, index, key);
    return (*bucket) ? (*bucket)->datum : 0;
}

static HashKey default_key(datum)
	TableDatum	datum;
{
    return datum;
}

/**
 * The default hash function assumes character string keys.  This function
 *	assumes that DEFAULT_BITS/8 + sizeof(char) <= sizeof(HashKey)
 */
static HashIndex default_hash(key)
	HashKey		key;
{
    unsigned	index;
    HashIndex	hash_index = 0;
    HashIndex	overflow;
    HashIndex	shifted_char;
    char	*conversion;

    conversion = (char *) key;
    index = strlen(conversion);
    while (index--) {
	/* This should be heavily optimized */

	shifted_char	= conversion[index] <<
	    ((index*BIT_MULTIPLIER) % DEFAULT_BITS);
	if ((overflow = (shifted_char & DEFAULT_MASK)) != 0) {
	    shifted_char &= ~(overflow);
	    shifted_char |= overflow >> DEFAULT_BITS;
	}
	hash_index 	^= shifted_char;
    }
    return hash_index;
}

