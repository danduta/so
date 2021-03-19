#ifndef _SO_CPP_HASHMAP_H_
#define _SO_CPP_HASHMAP_H_ 1

#include <so-cpp-utils.h>

struct hashmap_entry
{
    void *key;
    void *value;

    unsigned long long hash_value;

    struct hashmap_entry *next;
};

struct hashmap
{
    size_t size;
    size_t capacity;
    double load_factor;

    unsigned long long (*hash)(void *);
    int (*compare)(void *, void *);

    struct hashmap_entry **elements;
};

typedef struct hashmap *map_t;
typedef struct hashmap_entry *entry_t;

int map_alloc(map_t*, size_t, double, unsigned long long(void *), int(void *, void *));
void map_dealloc(map_t);
int map_insert(map_t*, void *, void *);
entry_t map_get(map_t, void *);
entry_t map_remove(map_t, void *);
void debug_print_map(map_t);

#endif
