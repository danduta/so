#include <so-cpp-hashmap.h>
#include <so-cpp-list.h>
#include <so-cpp-utils.h>

int main(int argc, char const *argv[])
{
    DIE(argc < 2, "Not enough paramenters! Usage:\nso-cpp [-D <SYMBOL>[=<MAPPING>]] [-I <DIR>] [<INFILE>] [ [-o] <OUTFILE>]");
    
    int idx;
    char outfile_name[BUFFER_LIMIT];

    struct hashmap* symbol_table = map_alloc(10, 0.75, hash_string, compare_strings);
    DIE(!symbol_table, "failed to allocate table");

    list directories = list_alloc();
    DIE(!directories, "failed to allocate list");

    list files = list_alloc();
    DIE(!files, "failed to allocate list");

    for (idx = 1; idx < argc; idx++) {
        if (strncmp(argv[idx], "-D", 2) == 0) {
            char* symbol = calloc(BUFFER_LIMIT, sizeof(char));
            DIE (!symbol, "failed to allocate buffer");

            char* symbol_loc = NULL;
            if (strlen(argv[idx]) == 2)
                symbol_loc = argv[++idx];
            else
                symbol_loc = argv[idx] + 2;

            char* mapping = calloc(BUFFER_LIMIT, sizeof(char));
            DIE (!mapping, "failed to allocate buffer");

            char* equal = strchr(symbol_loc, '=');
            if (equal) {
                strcpy(mapping, equal + 1);
                strncpy(symbol, symbol_loc, equal - symbol_loc);
            } else {
                strcpy(symbol, symbol_loc);
            }

            map_insert(&symbol_table, symbol, mapping);
        } else if (strncmp(argv[idx], "-I", 2) == 0) {
            char* directory = calloc(BUFFER_LIMIT, sizeof(char));
            DIE (!directory, "failed to allocate buffer");

            char* directory_loc = NULL;
            if (strlen(argv[idx]) == 2)
                directory_loc = argv[++idx];
            else
                directory_loc = argv[idx] + 2;

            strcpy(directory, directory_loc);
            
            list_insert(directories, directory);
        } else if (strcmp(argv[idx], "-o") == 0) {
            strcpy(outfile_name, argv[++idx]);
        } else {
            char* infile_name = calloc(BUFFER_LIMIT, sizeof(char));
            DIE (!infile_name, "failed to allocate buffer");

            strcpy(infile_name, argv[idx]);

            list_insert(files, infile_name);
        }
    }

    debug_print_map(symbol_table);
    debug_print_list(files);
    debug_print_list(directories);

    map_dealloc(symbol_table);
    list_dealloc(directories);
    list_alloc(files);

    return 0;
}
