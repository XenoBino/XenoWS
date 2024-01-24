#ifndef XENO_XENOWS_HASHMAP_H_
#define XENO_XENOWS_HASHMAP_H_

#include <stddef.h>
#include <stdint.h>

typedef struct _HashNode HashNode;

struct _HashNode {
	void *key;
	size_t keylen;
	void *value;
	HashNode *next;
};

typedef enum {
	HASHMAP_TYPE_DEFAULT = 0,
	HASHMAP_TYPE_XXH3,
} HashType;

typedef struct {
	HashNode **bucket;
	size_t size;
	HashType hash;
} Hashmap;

Hashmap *hashmap_create(size_t bucket_size, HashType hash);
HashNode *hashmap_get(Hashmap *map, void *key, size_t keylen);
HashNode *hashmap_set(Hashmap *map, void *key, size_t keylen, void *value);
uint64_t hashmap_hash(Hashmap *map, void *key, size_t keylen);
void hashmap_destroy(Hashmap *map);

#endif
