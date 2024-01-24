#include "hashmap.h"
#include <stdlib.h>
#include <xxh3.h>
#include "oom.h"

Hashmap *hashmap_create(size_t bucket_size, HashType hash) {
	Hashmap *map = malloc(sizeof(*map));
	if (!map) oom();

	if (hash != HASHMAP_TYPE_DEFAULT && hash != HASHMAP_TYPE_XXH3) return NULL;

	if (!bucket_size) bucket_size = 4096;
	map->size = bucket_size;
	map->hash = hash;
	map->bucket = malloc(sizeof(map->bucket[0]) * map->size);
	if (!map->bucket) oom();

	return map;
}

HashNode *hashmap_get(Hashmap *map, void *key, size_t keylen) {
	uint64_t hash = hashmap_hash(map, key, keylen);
	size_t ind = hash % map->size;

	HashNode *curr = map->bucket[ind];
	while (curr) {
		if (curr->keylen == keylen && !memcmp(curr->key, key, keylen))
			return curr;
		curr = curr->next;
	}

	return NULL;
}

HashNode *hashmap_set(Hashmap *map, void *key, size_t keylen, void *value) {
	uint64_t hash = hashmap_hash(map, key, keylen);
	size_t ind = hash % map->size;

	if (!map->bucket[ind]) {
		HashNode *node = malloc(sizeof(*node));
		if (!node) oom();
		node->keylen = keylen;
		node->key = malloc(node->keylen);
		node->next = NULL;
		node->value = value;

		map->bucket[ind] = node;
		return node;
	}

	HashNode *curr = map->bucket[ind];
	HashNode *prev = map->bucket[ind];
	while (curr) {
		if (curr->keylen == keylen && !memcmp(curr->key, key, keylen))
			return curr;
		prev = curr;
		curr = curr->next;
	}

	HashNode *node = malloc(sizeof(*node));
	node->keylen = keylen;
	node->key = malloc(node->keylen);
	if (!node->key) oom();
	node->value = value;
	node->next = NULL;

	prev->next = node;
	return node;
}

uint64_t hashmap_hash(Hashmap *map, void *key, size_t keylen) {
	return XXH64(key, keylen, 0);
}

void hashmap_destroy(Hashmap *map) {
	HashNode **iter = map->bucket;
	HashNode **end = map->bucket + map->size;
	while (iter < end) {
		HashNode *curr = *iter;
		while (curr) {
			HashNode *next = curr->next;
			free(curr->key);
			free(curr);
			curr = next;
		}

		iter++;
	}

	free(map->bucket);
	free(map);
}
