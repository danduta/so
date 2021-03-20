#include <so-cpp-hashmap.h>

int map_alloc(
    map_t *map,
    size_t initial_size,
    double load_factor,
    unsigned long long (*hash)(void *),
    int (*compare)(void *, void *))
{
    map_t result;

    if (initial_size < 1 || load_factor > 1)
        return EINVAL;

    if (!hash)
        return EINVAL;

    result = malloc(sizeof(struct hashmap));
    if (!result)
        return ENOMEM;

    result->size = 0;
    result->capacity = initial_size;
    result->elements = calloc(sizeof(struct hashmap_entry *), initial_size);
    if (!result->elements)
        return ENOMEM;

    result->hash = hash;
    result->compare = compare;
    result->load_factor = load_factor;

    *map = result;

    return EXIT_SUCCESS;
}

void traverse(entry_t list, void (*operation)(entry_t))
{
    entry_t curr, next;

    if (!list || !operation)
        return;

    curr = list;
    while (curr)
    {
        next = curr->next;
        operation(curr);
        curr = next;
    }
}

void free_entry(entry_t entry)
{
    if (!entry)
        return;

    if (entry->key)
        free(entry->key);
    if (entry->value)
        free(entry->value);

    free(entry);
}

void map_dealloc(struct hashmap *map)
{
    int i;
    entry_t entry;

    if (!map)
        return;

    for (i = 0; i < map->capacity; i++)
    {
        entry = map->elements[i];

        if (!entry)
            continue;

        traverse(entry, free_entry);
    }

    free(map->elements);
    free(map);
}

int map_insert(struct hashmap **pmap, void *key, void *value)
{
    map_t map, new_map;
    entry_t curr;
    unsigned long long hash_value;
    size_t index;
    int i;

    if (!pmap || !(*pmap)->elements)
        return EINVAL;

    map = *pmap;

    if (!key || !value)
        return EINVAL;

    if (((double)map->size / map->capacity) >= map->load_factor)
    {
        new_map;
        if (map_alloc(&new_map, 2 * map->capacity, map->load_factor, map->hash, map->compare))
            return ENOMEM;

        for (i = 0; i < map->capacity; i++)
        {
            if (!map->elements[i])
                continue;

            curr = map->elements[i];
            while (curr)
            {
                if (map_insert(&new_map, curr->key, curr->value))
                    return ENOMEM;

                curr = curr->next;
            }
        }

        map_dealloc(*pmap);

        *pmap = new_map;
        map = new_map;
    }

    if (!map->hash)
        return EINVAL;

    hash_value = map->hash(key);
    index = hash_value % map->capacity;

    curr = map->elements[index];
    while (curr) {
        if (!(map->compare(curr->key, key))) {
            free(curr->value);
            curr->value = value;
            return EXIT_SUCCESS;
        }

        curr = curr->next;
    }

    curr = calloc(1, sizeof(struct hashmap_entry));
    if (!curr)
        return ENOMEM;

    curr->hash_value = hash_value;
    curr->key = key;
    curr->value = value;
    curr->next = map->elements[index];

    map->elements[index] = curr;
    map->size++;

    return EXIT_SUCCESS;
}

entry_t map_get(map_t map, void *key) {
    unsigned long long hash_value;
    size_t index;
    entry_t existing;

    if (!map || !key)
        return NULL;

    hash_value = map->hash(key);
    index = hash_value % map->capacity;

    existing = map->elements[index];
    while (existing) {
        if (!(map->compare(existing->key, key)))
            return existing;

        existing = existing->next;
    }

    return NULL;
}

struct hashmap_entry *map_remove(struct hashmap *map, void *key)
{
    entry_t entry, curr;
    int index;
    if (!map || !key)
        return NULL;

    entry = map_get(map, key);
    if (!entry)
        return NULL;

    index = entry->hash_value % map->capacity;

    if (entry == map->elements[index])
        map->elements[index] = entry->next;
    else {
        curr = map->elements[index];
        while (curr->next != entry)
            curr = curr->next;

        curr->next = entry->next;
    }

    free_entry(entry);

    map->size--;
    map->load_factor = ((double) map->size) / map->capacity;

    return entry;
}

void print(entry_t entry)
{
    TRACE(("\t{%s = %s, %llu}\n", (char *)entry->key, (char *)entry->value, entry->hash_value));
}

void debug_print_map(struct hashmap *map)
{
    int i;
    if (!map)
        return;

    for (i = 0; i < map->capacity; i++)
    {
        TRACE(("[%d]:\n", i));
        if (!map->elements[i])
            TRACE(("\tNULL\n"));
        else
            traverse(map->elements[i], print);
    }
}
