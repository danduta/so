#include <so-cpp-hashmap.h>
#include <so-cpp-list.h>
#include <so-cpp-utils.h>

int main(int argc, char const *argv[])
{
    DIE(argc < 2, "Not enough paramenters! Usage:\nso-cpp [-D <SYMBOL>[=<MAPPING>]] [-I <DIR>] [<INFILE>] [ [-o] <OUTFILE>]");

    int idx;
    char outfile_name[BUFFER_LIMIT] = "";
    int res;

    struct hashmap *symbol_table;
    if (res = map_alloc(&symbol_table, 10, 0.75, hash_string, compare_strings))
        return res;

    list_t directories;
    if (res = list_alloc(&directories, &free))
        return res;

    list_t filenames;
    if (res = list_alloc(&filenames, &free))
        return res;

    if (res = cpp_parse_cli_args(argc, argv, &symbol_table, directories, filenames, outfile_name))
        return res;

    list_t files;
    if (res = cpp_get_file_list(&files, filenames))
        return res;

    /* debug_print_map(symbol_table);
    debug_print_list(filenames);
    debug_print_list(directories); */

    if (res = cpp_parse_defines(&symbol_table, files))
        return res;

    map_dealloc(symbol_table);
    list_dealloc(directories);
    list_dealloc(filenames);
    list_dealloc(files);

    return 0;
}
