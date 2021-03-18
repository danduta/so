#include <so-cpp-hashmap.h>

int map_alloc(map_t *map, size_t initial_size, double load_factor, unsigned long long (*hash)(void *), int (*compare)(void *, void *))
{
    if (initial_size < 1 || load_factor > 1)
        return EINVAL;

    if (!hash)
        return EINVAL;

    map_t result = malloc(sizeof(struct hashmap));
    if (!result)
        return ENOMEM;

    result->size = 0;
    result->capacity = initial_size;
    result->elements = calloc(sizeof(entry_t), initial_size);
    DIE(!result->elements, "malloc");

    result->hash = hash;
    result->load_factor = load_factor;

    *map = result;

    return EXIT_SUCCESS;
}

void traverse(entry_t list, void (*operation)(entry_t))
{
    if (!list || !operation)
        return;

    entry_t curr = list;
    while (curr)
    {
        entry_t next = curr->next;
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
    if (!map)
        return;

    int i;
    for (i = 0; i < map->capacity; i++)
    {
        entry_t entry = map->elements[i];

        if (!entry)
            continue;

        traverse(entry, free_entry);
    }

    free(map->elements);
    free(map);
}

int map_insert(struct hashmap **pmap, void *key, void *value)
{
    if (!pmap || !(*pmap)->elements)
        return EINVAL;

    map_t map = *pmap;

    if (!key || !value)
        return EINVAL;

    if (((double)map->size / map->capacity) >= map->load_factor)
    {
        map_t new_map;
        if (map_alloc(&new_map, 2 * map->capacity, map->load_factor, map->hash, map->compare))
            return ENOMEM;

        int i;
        for (i = 0; i < map->capacity; i++)
        {
            if (!map->elements[i])
                continue;

            entry_t curr = map->elements[i];
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

    unsigned long long hash_value = map->hash(key);
    size_t index = hash_value % map->capacity;

    entry_t current_entry = calloc(1, sizeof(struct hashmap_entry));
    if (!current_entry)
        return ENOMEM;

    current_entry->hash_value = hash_value;
    current_entry->key = key;
    current_entry->value = value;
    current_entry->next = map->elements[index];

    map->elements[index] = current_entry;
    map->size++;

    return EXIT_SUCCESS;
}

struct hashmap_entry *map_remove(struct hashmap *map, void *key)
{
    return NULL;
}

void print(entry_t entry)
{
    TRACE(("\t{%s = %s, %llu}\n", (char *)entry->key, (char *)entry->value, entry->hash_value));
}

void debug_print_map(struct hashmap *map)
{
    if (!map)
    {
        return;
    }

    int i;
    for (i = 0; i < map->capacity; i++)
    {
        TRACE(("[%d]:\n", i));
        if (!map->elements[i])
            TRACE(("\tNULL\n"));
        else
            traverse(map->elements[i], print);
    }
}
