#include <so-cpp-hashmap.h>
#include <so-cpp-utils.h>
#include <stdlib.h>


struct hashmap_entry {
    void* key;
    void* value;

    struct hashmap_entry* next;
};

struct hashmap {
    size_t size;
    size_t capacity;
    double load_factor;

    struct hashmap_entry* elements;
    unsigned long long (*hash)(void* data);
};

struct hashmap* map_alloc(size_t initial_size) {
    DIE(initial_size < 1, "map initial size");

    malloc(sizeof(struct hashmap*));
}