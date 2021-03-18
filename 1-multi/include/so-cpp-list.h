#ifndef _SO_CPP_LIST_H_
#define _SO_CPP_LIST_H_ 1

#include <so-cpp-utils.h>

struct list_node
{
    void *data;
    struct list_node *next;
    void (*free)(void *);
};

typedef struct list_node *list_t;
typedef struct list_node *node_t;

int list_alloc(list_t *, void(void *));
void list_dealloc(list_t);
int list_insert(list_t, void *);
struct list_node *list_remove(list_t, void *);
void debug_print_list(list_t);

#endif
