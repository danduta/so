#include <so-cpp.h>
#include <so-cpp-list.h>

int cpp_parse_cli_args(int argc, char* argv[], map_t* symbol_table, list_t directories, list_t files, char* out_filename) {
    int res, idx;
    for (idx = 1; idx < argc; idx++) {
        if (strncmp(argv[idx], "-D", 2) == 0) {
            char* symbol = calloc(BUFFER_LIMIT, sizeof(char));
            if (!symbol)
                return ENOMEM;

            char* symbol_loc = NULL;
            if (strlen(argv[idx]) == 2)
                symbol_loc = argv[++idx];
            else
                symbol_loc = argv[idx] + 2;

            char* mapping = calloc(BUFFER_LIMIT, sizeof(char));
            if (!mapping)
                return ENOMEM;

            char* equal = strchr(symbol_loc, '=');
            if (equal) {
                strcpy(mapping, equal + 1);
                strncpy(symbol, symbol_loc, equal - symbol_loc);
            } else {
                strcpy(symbol, symbol_loc);
            }

            TRACE(("[PARSING] Found symbol: %s = %s\n", symbol, mapping));

            if (res = map_insert(symbol_table, symbol, mapping))
                return res;
        } else if (strncmp(argv[idx], "-I", 2) == 0) {
            char* directory = calloc(BUFFER_LIMIT, sizeof(char));
            if (!directory)
                return ENOMEM;

            char* directory_loc = NULL;
            if (strlen(argv[idx]) == 2)
                directory_loc = argv[++idx];
            else
                directory_loc = argv[idx] + 2;

            strcpy(directory, directory_loc);

            TRACE(("[PARSING] Found directory: %s\n", directory));
            
            if (res = list_insert(directories, directory))
                return res;
        } else if (strcmp(argv[idx], "-o") == 0) {
            strcpy(out_filename, argv[++idx]);
        } else {
            char* infile_name = calloc(BUFFER_LIMIT, sizeof(char));
            DIE (!infile_name, "failed to allocate buffer");

            strcpy(infile_name, argv[idx]);

            TRACE(("[PARSING] Found input file: %s\n", infile_name));

            list_insert(files, infile_name);
        }
    }

    return EXIT_SUCCESS;
}

int cpp_get_file_list(list_t* files, list_t filenames) {
    int res = list_alloc(files, fclose);
    if (res)
        return res;

    node_t curr = filenames->next;
    while (curr) {
        FILE* fp = fopen(curr->data, "r+");
        if (fp) {
            list_insert(*files, fp);
            TRACE(("[PARSING] Found file for reading: %s\n", curr->data));
        }


        curr = curr->next;
    }

    return EXIT_SUCCESS;
}

FILE* cpp_get_filep(char* filename) {
    if (!filename)
        return NULL;

    return fopen(filename, "r+");
}
