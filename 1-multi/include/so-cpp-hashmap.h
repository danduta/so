#ifndef _SO_CPP_HASHMAP_H_
#define _SO_CPP_HASHMAP_H_ 1

#include <so-cpp-utils.h>

struct hashmap;
struct hashmap_entry;

struct hashmap* map_alloc(size_t, double, unsigned long long (void*), int (void*, void*));
void map_dealloc(struct hashmap*);
void map_insert(struct hashmap**, void*, void*);
struct hashmap_entry* map_get(struct hashmap*, void*);
struct hashmap_entry* map_remove(struct hashmap*, void*);
void debug_print(struct hashmap*);

#endif
