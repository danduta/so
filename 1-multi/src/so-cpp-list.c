#include <so-cpp-list.h>

int list_alloc(list_t *l, void (*free)(void *))
{
    list_t res = calloc(sizeof(struct list_node), 1);
    if (!res)
        exit(ENOMEM);

    res->free = free;
    *l = res;
    return EXIT_SUCCESS;
}

void list_dealloc(list_t l)
{
    if (!l)
        return;

    struct list_node *curr = l->next;
    while (curr)
    {
        struct list_node *next = curr->next;

        if (curr->data)
            l->free(curr->data);

        free(curr);

        curr = next;
    }

    free(l);
}

int list_insert(list_t l, void *data)
{
    if (!l)
        return EINVAL;

    struct list_node *new_node = malloc(sizeof(struct list_node));
    if (!new_node)
        return ENOMEM;

    new_node->data = data;
    new_node->next = NULL;

    if (!l->next) {
        l->next = new_node;
        l->tail = new_node;
        return EXIT_SUCCESS;
    }

    l->tail->next = new_node;
    l->tail = new_node;

    return EXIT_SUCCESS;
}

struct list_node *list_remove(list_t l, void *data)
{
    return NULL;
}

void debug_print_list(list_t l)
{
    if (!l)
        return;

    TRACE(("["));
    list_t curr = l->next;
    while (curr)
    {
        TRACE(("%s", curr->data));
        if (curr->next)
            TRACE((", "));

        curr = curr->next;
    }

    TRACE(("]\n"));
}
