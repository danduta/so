#ifndef _SO_CPP_HASHMAP_
#define _SO_CPP_HASHMAP_

#include <unistd.h>

struct hashmap;
struct hashmap_entry;

struct hashmap* map_alloc(size_t);
void map_dealloc(struct hashmap*);
size_t map_insert(struct hashmap*, char*, char*);
struct hashmap_entry* map_get(struct hashmap*, char*);
struct hashmap_entry* map_remove(struct hashmap*, char*);


#endif