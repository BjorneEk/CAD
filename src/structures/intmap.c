/*==========================================================*
 *
 * @author Gustaf Franzén :: https://github.com/BjorneEk;
 *
 * hashmap
 *
 *==========================================================*/


#include "intmap.h"
#include <stdlib.h>
#include <string.h>


intmap_t	*IMAP_new(u64_t len, hash_f hashf)
{
	intmap_t	*res;

	res = malloc(sizeof(intmap_t));
	res->buckets = calloc(len, sizeof(struct intbucket));
	res->n_buckets = 0;
	res->len = len;
	res->hashf = hashf;

	return res;
}

static u64_t hash_idx(intmap_t *m, const void *key, u64_t kwidth)
{
	return m->hashf(key, kwidth) % m->len;
}

static inline bool intbucket_equals(u64_t hash, const void *key, u64_t kwidth, const struct intbucket *b)
{
	return (hash == b->hash && kwidth == b->kwidth && memcmp(key, b->key, kwidth) == 0);
}

static struct intbucket *get_intbucket(intmap_t *m, const void *key, u64_t kwidth)
{
	struct intbucket *b;
	u64_t hash;
	u64_t idx;


	hash = m->hashf(key, kwidth);
	idx = hash % m->len;
	b = &m->buckets[idx];

	do {
		if (intbucket_equals(hash, key, kwidth, b))
			return b;
	} while((b = b->next) != NULL);
	return NULL;
}



bool	IMAP_contins_key(intmap_t *m, const void *key, u64_t kwidth)
{
	return get_intbucket(m, key, kwidth) != NULL;
}

void IMAP_remove(intmap_t *m, void *key, u64_t kwidth)
{
	struct intbucket *b, *tmp;
	u64_t hash;
	u64_t idx;


	hash = m->hashf(key, kwidth);
	idx = hash % m->len;
	b = &m->buckets[idx];

	if(b->next == NULL && intbucket_equals(hash, key, kwidth, b)) {
		memset(b, 0, sizeof(struct bucket));
		return;
	} else if (intbucket_equals(hash, key, kwidth, b)) {
		tmp = b->next;
		memcpy(b, tmp, sizeof(struct bucket));
		free(tmp);
		return;
	}

	while (b->next != NULL) {

		if (intbucket_equals(hash, key, kwidth, b->next)) {
			tmp = b->next;
			b->next = b->next->next;
			free(tmp);
			return;
		}
		b = b->next;
	}

}

u64_t	IMAP_get(intmap_t *m, const void *key, u64_t kwidth)
{
	struct intbucket *entry;

	entry = get_intbucket(m, key, kwidth);

	if(entry == NULL)
		return 0;

	return entry->value;
}

struct intbucket new_intbucket(intmap_t *m, void *key, u64_t kwidth, u64_t value)
{
	struct intbucket res;

	res.value	= value;
	res.key		= key;

	res.hash	= m->hashf(key, kwidth);
	res.next	= NULL;
	res.kwidth	= kwidth;
	return res;
}
struct intbucket *new_intbucket_ptr(intmap_t *m, void *key, u64_t kwidth, u64_t value)
{
	struct intbucket *res;

	res = malloc(sizeof(struct intbucket));
	res->value	= value;
	res->key	= key;

	res->hash	= m->hashf(key, kwidth);
	res->next	= NULL;
	res->kwidth	= kwidth;
	return res;
}

void	IMAP_add(intmap_t *m, void *key, u64_t kwidth, u64_t value)
{
	u64_t i;
	struct intbucket *b;

	++m->n_buckets;
	i = hash_idx(m, key, kwidth);

	if(m->buckets[i].key == NULL) {
		m->buckets[i] = new_intbucket(m, key, kwidth, value);
	} else {
		for(b = &m->buckets[i]; b->next != NULL; b = b->next)
			;
		b->next = new_intbucket_ptr(m, key, kwidth, value);
	}
}

void free_intbucket(struct intbucket b)
{
	struct intbucket *next, *tmp;

	if(!b.next) {
		return;
	}
	next = b.next;

	do {
		tmp = next;
		next = next->next;
		free(tmp);
	} while(next != NULL);
}

void		IMAP_free(intmap_t **m)
{
	u64_t i;

	for(i = 0; i < (*m)->len; i++)
		free_intbucket((*m)->buckets[i]);
	free((*m)->buckets);
	free(*m);
}