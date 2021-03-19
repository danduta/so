#include <so-cpp-hashmap.h>
#include <so-cpp-list.h>
#include <so-cpp-utils.h>
#include <so-cpp.h>

int main(int argc, char const *argv[])
{
    int idx;
    char outfile_name[BUFFER_LIMIT] = "", infile_name[BUFFER_LIMIT] = "", *current_file, *buffer;
    int res;
    map_t symbol_table;
    list_t directories;

    if (res = map_alloc(&symbol_table, 10, 0.75, hash_string, compare_strings))
        goto free_mem;

    if (res = list_alloc(&directories, &free))
        goto free_mem;

    if (res = cpp_parse_cli_args(argc, argv, &symbol_table, directories, infile_name, outfile_name))
        goto free_mem;


    if (infile_name[0] == '\0')
        fscanf(stdin, "%s", infile_name);

    current_file = strrchr(infile_name, '/');
    if (current_file) {
        buffer = calloc(BUFFER_LIMIT, sizeof(char));
        if (!buffer) {
            res = ENOMEM;
            goto free_mem;
        }

        strncpy(buffer, infile_name, current_file - infile_name + 1);
        buffer[current_file - infile_name + 1] = '\0';
        list_insert(directories, buffer);
    }

    if (res = cpp_parse_input_file(&symbol_table, infile_name, outfile_name, directories))
        goto free_mem;

free_mem:

    debug_print_map(symbol_table);
    map_dealloc(symbol_table);
    list_dealloc(directories);

    return res;
}
