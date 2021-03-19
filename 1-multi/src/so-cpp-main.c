#include <so-cpp-hashmap.h>
#include <so-cpp-list.h>
#include <so-cpp-utils.h>
#include <so-cpp.h>

int main(int argc, char const *argv[])
{
    int idx;
    char outfile_name[BUFFER_LIMIT] = "", infile_name[BUFFER_LIMIT] = "";
    int res;

    struct hashmap *symbol_table;
    if (res = map_alloc(&symbol_table, 10, 0.75, hash_string, compare_strings))
        goto free_mem;

    list_t directories;
    if (res = list_alloc(&directories, &free))
        goto free_mem;

    if (res = cpp_parse_cli_args(argc, argv, &symbol_table, directories, infile_name, outfile_name))
        goto free_mem;

    if (infile_name[0] == '\0')
        fscanf(stdin, "%s", infile_name);

    /* if (outfile_name[0] == '\0') {
        char* extension = strchr(infile_name, '.');
        strncpy(outfile_name, infile_name, extension - infile_name);
        strcat(outfile_name, PREPROC_EXTENS);
        TRACE(("[PARSING] No output file was given. Outputting to %s.\n\n", outfile_name));
    } */

    if (res = cpp_parse_input_file(&symbol_table, infile_name, outfile_name, directories))
        goto free_mem;

free_mem:

    debug_print_map(symbol_table);
    map_dealloc(symbol_table);
    list_dealloc(directories);

    return res;
}
