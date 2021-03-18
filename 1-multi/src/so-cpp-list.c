#include <so-cpp-list.h>

struct list_node {
    void* data;
    struct list_node* next;
};

list list_alloc() {
    if (!calloc(sizeof(struct list_node), 1))
        exit(ENOMEM);
}

void list_dealloc(list l) {
    if (!l)
        return;

    struct list_node* curr = l->next;
    while (curr) {
        struct list_node* next = curr->next;
        
        if (curr->data)
            free(curr->data);
        free(curr);

        curr = next;
    }

    free(l);
}

void list_insert(list l, void* data) {
    if (!l)
        return;

    struct list_node* new_node = malloc(sizeof(struct list_node));
    if (!new_node)
        exit(ENOMEM);

    new_node->data = data;
    new_node->next = l->next;

    l->next = new_node;
}

struct list_node* list_remove(list l, void* data) {
    return NULL;
}

void debug_print_list(list l) {
    if (!l)
        return;

    TRACE(("["));
    list curr = l->next;
    while (curr) {
        TRACE(("%s", curr->data));
        if (curr->next)
            TRACE((", "));

        curr = curr->next;
    }

    TRACE(("]\n"));
}
