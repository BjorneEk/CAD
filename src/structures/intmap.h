/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * hashmap header file
 *
 *==========================================================*/

#ifndef _INTMAP_H_
#define _INTMAP_H_

#include "../util/types.h"
#include "hashmap.h"
#include <stdbool.h>

struct intbucket {

	u64_t hash;

	void *key;
	u64_t kwidth;

	u64_t value;

	struct intbucket *next;
};


typedef struct intmap {

	struct intbucket *buckets;

	u64_t len;

	u64_t n_buckets;

	hash_f hashf;
} intmap_t;

/**
 * allocates and returns a new hashmap with the given type,
 * length and hash functions
 *
 * @param vwidth: the size in bytes of the values to be stored in the hashmap
 * @param len	: the length of the hashmap
 * @param hashf	: the hashfunction of the hashmap
 **/
intmap_t	*IMAP_new(u64_t len, hash_f hashf);

/**
 * frees a hashmap and its contents fully, this includes freeing the
 * key and value pointers themselves
 *
 * @param m: the hashmap to be freed
 **/
void		IMAP_free(intmap_t **m);


/**
 * returns true if the hashmap contains a entry with the given key
 *
 * @param m	: the hashmap
 * @param key	: the key to search for
 * @param kwidth: the width in bytes of the @key
 **/
bool		IMAP_contins_key(intmap_t *m, const void *key, u64_t kwidth);

/**
 * if the hashmap contains the given key return the value, else return NULL
 *
 * @param m	: the hashmap
 * @param key	: the key to search for
 * @param kwidth: the width in bytes of the @key
 **/
u64_t		IMAP_get(intmap_t *m, const void *key, u64_t kwidth);

/**
 * remove the entrie with the coresponding key
 *
 * @param m	: the hashmap
 * @param key	: the key to search for
 * @param kwidth: the width in bytes of the @key
 **/
void		IMAP_remove(intmap_t *m, void *key, u64_t kwidth);

/**
 * adds the entry created by @key and @value to the hashmap
 *
 * @param m	: the hashmap
 * @param key	: the key to the entry
 * @param kwidth: the width in bytes of the @key
 * @param value	: the value to be added
 **/
void		IMAP_add(intmap_t *m, void *key, u64_t kwidth, u64_t value);

#endif /* _INTMAP_H_ */