#include <so-cpp.h>
#include <so-cpp-list.h>

int cpp_parse_cli_args(int argc, char *argv[], map_t *symbol_table, list_t directories, char* in_filename, char *out_filename)
{
    int res, idx;
    for (idx = 1; idx < argc; idx++)
    {
        if (strncmp(argv[idx], "-D", 2) == 0)
        {
            char *symbol = calloc(BUFFER_LIMIT, sizeof(char));
            if (!symbol)
                return ENOMEM;

            char *symbol_loc = NULL;
            if (strlen(argv[idx]) == 2)
                symbol_loc = argv[++idx];
            else
                symbol_loc = argv[idx] + 2;

            char *mapping = calloc(BUFFER_LIMIT, sizeof(char));
            if (!mapping)
                return ENOMEM;

            char *equal = strchr(symbol_loc, '=');
            if (equal)
            {
                strcpy(mapping, equal + 1);
                strncpy(symbol, symbol_loc, equal - symbol_loc);
            }
            else
            {
                strcpy(symbol, symbol_loc);
            }

            TRACE(("[PARSING] Found symbol: %s = %s\n", symbol, mapping));

            if (res = map_insert(symbol_table, symbol, mapping))
                return res;
        }
        else if (strncmp(argv[idx], "-I", 2) == 0)
        {
            char *directory = calloc(BUFFER_LIMIT, sizeof(char));
            if (!directory)
                return ENOMEM;

            char *directory_loc = NULL;
            if (strlen(argv[idx]) == 2)
                directory_loc = argv[++idx];
            else
                directory_loc = argv[idx] + 2;

            strcpy(directory, directory_loc);

            TRACE(("[PARSING] Found directory: %s\n", directory));

            if (res = list_insert(directories, directory))
                return res;
        }
        else if (strcmp(argv[idx], "-o") == 0)
        {
            strcpy(out_filename, argv[++idx]);
            TRACE(("[PARSING] Found output file: %s\n", out_filename));
        }
        else
        {
            strcpy(in_filename, argv[idx]);
            TRACE(("[PARSING] Found input file: %s\n", in_filename));
        }
    }

    TRACE(("\n"));

    return EXIT_SUCCESS;
}

int cpp_get_file_list(list_t *files, list_t filenames)
{
    int res = list_alloc(files, fclose);
    if (res)
        return res;

    node_t curr = filenames->next;
    while (curr)
    {
        FILE *fp = fopen(curr->data, "r+");
        if (fp)
        {
            list_insert(*files, fp);
            TRACE(("[PARSING] Found file for reading: %s\n", curr->data));
        }

        curr = curr->next;
    }

    return EXIT_SUCCESS;
}

#define DELIMITERS "\t []{}<>=+-*/%!&|^.,:;()\\.\n"

FILE* cpp_get_included_file(char* filename, list_t directories) {
    if (!filename)
        return NULL;

    char buffer[BUFFER_LIMIT];

    TRACE(("[PARSING] Trying to reach file %s...\n", filename));

    FILE* res = fopen(filename, "r+");
    if (res)
        return res;

    node_t curr = directories->next;
    while (curr) {
        strcpy(buffer, curr->data);
        strcat(buffer, "/");
        strcat(buffer, filename);

        TRACE(("[PARSING] Trying to reach file %s...\n", buffer));

        res = fopen(buffer, "r+");
        if (res)
            return res;

        curr = curr->next;
    }

    TRACE(("[PARSING] Unable to reach file %s.\n", filename));

    return NULL;
}

int cpp_parse_input_file(map_t *symbol_table, char *input_filename, char *output_filename, list_t directories)
{
    if (!symbol_table || !input_filename)
        return EINVAL;

    int res;

    FILE *input = fopen(input_filename, "r");
    if (!input)
        return EINVAL;

    FILE *output = fopen(output_filename, "w+");
    if (!output)
        return EINVAL;

    TRACE(("[PARSING] Parsing input file: %s.\n\n", input_filename));

    if (res = cpp_recursive_parse_included_file(symbol_table, input, output, directories))
        return res;

    fclose(input);

    return EXIT_SUCCESS;
}

int cpp_recursive_parse_included_file(map_t *symbol_table, FILE *input, FILE* output, list_t directories)
{
    char buffer[BUFFER_LIMIT];
    int res;

    while (fgets(buffer, BUFFER_LIMIT, input))
    {
        char *p;

        if (p = strstr(buffer, "#include"))
        {
            char *apostrophe = strchr(buffer, '\"');
            char *arrow = strchr(buffer, '<');

            char *included_filename_loc, *closure;
            char included_filename[BUFFER_LIMIT];

            if (!apostrophe)
            {
                included_filename_loc = arrow + 1;
                closure = strchr(arrow + 1, '>');

                strncpy(included_filename, included_filename_loc, closure - included_filename_loc);
            }
            else if (!arrow)
            {
                included_filename_loc = apostrophe + 1;
                closure = strchr(apostrophe + 1, '\"');

                strncpy(included_filename, included_filename_loc, closure - included_filename_loc);
            }
            else
            {
                char *closest = arrow - buffer < apostrophe - buffer ? arrow + 1 : apostrophe + 1;
                included_filename_loc = closest;

                if (closest == arrow + 1)
                {
                    closure = strchr(arrow + 1, '>');
                    strncpy(included_filename, included_filename_loc, closure - included_filename_loc);
                }
                else
                {
                    closure = strchr(arrow + 1, '\"');
                    strncpy(included_filename, included_filename_loc, closure - included_filename_loc);
                }
            }
            
            included_filename[closure - included_filename_loc] = '\0';

            TRACE(("[INCLUDED FILE] %s\n", included_filename));

            FILE* included_file = cpp_get_included_file(included_filename, directories);
            if (!included_file)
                continue;

            TRACE(("[PARSING] Parsing included file: %s\n", included_filename));

            if (res = cpp_recursive_parse_included_file(symbol_table, included_file, output, directories))
                return res;

            fclose(included_file);
        } else if (p = strstr(buffer, "#define")) { 
            char *symbol = calloc(BUFFER_LIMIT, sizeof(char));
            if (!symbol)
                return ENOMEM;

            char *symbol_loc = strchr(p, ' ') + 1;

            char *mapping = calloc(BUFFER_LIMIT, sizeof(char));
            if (!mapping)
                return ENOMEM;

            char *space = strchr(symbol_loc, ' ');
            if (space)
            {
                strncpy(symbol, symbol_loc, space - symbol_loc);

                char* string_literal = strchr(space, '"');
                char* end;
                if (string_literal)
                    end = strrchr(buffer, '"');
                else
                    end = strchr(buffer, '\n');

                while (strchr(buffer, '\\')) {
                    strncat(mapping, space, strlen(space) - 2);
                    fgets(buffer, BUFFER_LIMIT, input);
                    space = buffer;
                }

                end = strchr(buffer, '\n') - 1;

                strncat(mapping, space + 1, end - space);
            }
            else
            {
                strncpy(symbol, symbol_loc, strlen(symbol_loc) - 1);
            }

            TRACE(("[PARSING] Found symbol: %s = %s\n", symbol, mapping));

            if (res = map_insert(symbol_table, symbol, mapping))
                return res;
        } else {
            fputs(buffer, output);
        }
    }

    return EXIT_SUCCESS;
}