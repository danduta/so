#ifndef _SO_CPP_LIST_H_
#define _SO_CPP_LIST_H_ 1

#include <so-cpp-utils.h>

typedef struct list_node* list;

list list_alloc();
void list_dealloc(list);
void list_insert(list, void*);
struct list_node* list_remove(list, void*);
void debug_print_list(list);

#endif
