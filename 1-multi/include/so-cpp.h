#ifndef _SO_CPP_H_
#define _SO_CPP_H_ 1

#include <so-cpp-hashmap.h>
#include <so-cpp-list.h>
#include <so-cpp-utils.h>

#define C_EXTENS        ".c"
#define PREPROC_EXTENS  ".i"
#define DELIMITERS "\t []{}<>=+-*/%!&|^.,:;()\\.\n"

int cpp_parse_cli_args(int, const char *[], map_t*, list_t, char*, char*);
int cpp_parse_input_file(map_t*, char*, char*, list_t);

#endif
