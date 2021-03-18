#ifndef _SO_CPP_H_
#define _SO_CPP_H_ 1

#include <so-cpp-hashmap.h>
#include <so-cpp-list.h>
#include <so-cpp-utils.h>

int cpp_parse_cli_args(int, char*[], map_t*, list_t, list_t, char*);
FILE* cpp_get_filep(char*);
int cpp_get_file_list(list_t*, list_t);

#endif
