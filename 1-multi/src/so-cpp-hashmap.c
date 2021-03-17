#include <so-cpp-hashmap.h>

typedef struct hashmap_entry {
    void* key;
    void* value;

    u_int64_t hash_value;

    struct hashmap_entry* next;
} *entry_t;


typedef struct hashmap {
    size_t size;
    size_t capacity;
    double load_factor;

    u_int64_t (*hash)(void*);
    int (*compare)(void*, void*);

    struct hashmap_entry** elements;
} *map_t;

struct hashmap* map_alloc(size_t initial_size, double load_factor, u_int64_t (*hash)(void*), int (*compare)(void*, void*)) {
    if (initial_size < 1 || load_factor > 1)
        return NULL;

    if (!hash)
        return NULL;

    map_t result = malloc(sizeof(struct hashmap));
    DIE(!result, "malloc");

    result->size = 0;
    result->capacity = initial_size;
    result->elements = calloc(sizeof(entry_t), initial_size);
    DIE(!result->elements, "malloc");

    result->hash = hash;
    result->load_factor = load_factor;
}

void traverse(entry_t list, void (*operation)(entry_t)) {
    if (!list || !operation)
        return;

    entry_t curr = list;
    while (curr) {
        entry_t next = curr->next;
        operation(curr);
        curr = next;
    }
}

void free_entry(entry_t entry) { 
    if (!entry)
        return;

    free(entry);
}

void map_dealloc(struct hashmap* map) {
    if (!map)
        return;

    for (int i = 0; i < map->size; i++) {
        entry_t entry = map->elements[i];

        if (!entry)
            continue;

        traverse(entry, free_entry);
    }

    free(map->elements);
    free(map);
}

void map_insert(struct hashmap* map, void* key, void* value) {
    if (!map || !map->elements)
        return;

    if (!key || !value)
        return;

    // treat load factor and resizing

    DIE (!map->hash, "no hash");

    u_int64_t hash_value = map->hash(key);
    size_t index = hash_value % map->capacity;

    // printf("%u %u\n", index, map->capacity);
    printf("%d %s %s\n", index, key, value);

    entry_t current_entry = calloc(sizeof(struct hashmap_entry), 1);
    DIE(!current_entry, "calloc");

    current_entry->hash_value = hash_value;
    current_entry->key = key;
    current_entry->value = value;
    current_entry->next = map->elements[index];

    map->elements[index] = current_entry;
    map->size++;
}

struct hashmap_entry* map_remove(struct hashmap* map, void* key) {
    return NULL;
}

void print(entry_t entry) {
    fprintf(stdout, "\t{%s = %s, %lu}\n", (char*) entry->key, (char*) entry->value, entry->hash_value);
}

void debug_print(struct hashmap* map) {
    if (!map) {
        return;
    }

    for (int i = 0; i < map->capacity; i++) {
        fprintf(stdout, "[%d]:\n", i);
        if (!map->elements[i])
            fprintf(stdout, "\tNULL\n");
        else
            traverse(map->elements[i], print);
    }
}
