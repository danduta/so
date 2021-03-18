#include <so-cpp-hashmap.h>
#include <so-cpp-list.h>
#include <so-cpp-utils.h>
#include <so-cpp.h>

int main(int argc, char const *argv[])
{
    DIE(argc < 2, "Not enough paramenters! Usage:\nso-cpp [-D <SYMBOL>[=<MAPPING>]] [-I <DIR>] [<INFILE>] [ [-o] <OUTFILE>]");

    int idx;
    char outfile_name[BUFFER_LIMIT] = "", infile_name[BUFFER_LIMIT];
    int res;

    struct hashmap *symbol_table;
    if (res = map_alloc(&symbol_table, 10, 0.75, hash_string, compare_strings))
        return res;

    list_t directories;
    if (res = list_alloc(&directories, &free))
        return res;

    if (res = cpp_parse_cli_args(argc, argv, &symbol_table, directories, infile_name, outfile_name))
        return res;

    if (outfile_name[0] == '\0') {
        char* extension = strstr(infile_name, C_EXTENS);
        strncpy(outfile_name, infile_name, extension - infile_name);
        strcat(outfile_name, PREPROC_EXTENS);
        TRACE(("[PARSING] No output file was given. Outputting to %s.\n\n", outfile_name));
    }

    if (res = cpp_parse_input_file(&symbol_table, infile_name, outfile_name, directories))
        return res;

    debug_print_map(symbol_table);
    map_dealloc(symbol_table);
    list_dealloc(directories);

    return 0;
}
