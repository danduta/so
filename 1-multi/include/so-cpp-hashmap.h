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

int map_alloc(struct hashmap **, size_t, double, unsigned long long(void *), int(void *, void *));
void map_dealloc(struct hashmap *);
int map_insert(struct hashmap **, void *, void *);
struct hashmap_entry *map_get(struct hashmap *, void *);
struct hashmap_entry *map_remove(struct hashmap *, void *);
void debug_print_map(struct hashmap *);

#endif
