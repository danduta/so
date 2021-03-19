#include <so-cpp.h>
#include <so-cpp-list.h>

int cpp_parse_cli_args(int argc, const char *argv[], map_t *symbol_table, list_t directories, char* in_filename, char *out_filename)
{
    int res, idx;
    char *symbol, *symbol_loc, *mapping, *equal, *directory, *directory_loc, *filename_loc;
    for (idx = 1; idx < argc; idx++)
    {
        if (strncmp(argv[idx], "-D", 2) == 0)
        {
            symbol = calloc(BUFFER_LIMIT, sizeof(char));
            if (!symbol)
                return ENOMEM;

            symbol_loc = NULL;
            if (strlen(argv[idx]) == 2)
                symbol_loc = argv[++idx];
            else
                symbol_loc = argv[idx] + 2;

            mapping = calloc(BUFFER_LIMIT, sizeof(char));
            if (!mapping)
                return ENOMEM;

            equal = strchr(symbol_loc, '=');
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
            directory = calloc(BUFFER_LIMIT, sizeof(char));
            if (!directory)
                return ENOMEM;

            directory_loc = NULL;
            if (strlen(argv[idx]) == 2)
                directory_loc = argv[++idx];
            else
                directory_loc = argv[idx] + 2;

            strcpy(directory, directory_loc);

            TRACE(("[PARSING] Found directory: %s\n", directory));

            if (res = list_insert(directories, directory))
                return res;
        }
        else if (strncmp(argv[idx], "-o", 2) == 0)
        {
            if (strlen(argv[idx]) == 2)
                filename_loc = argv[++idx];
            else
                filename_loc = argv[idx] + 2;

            strcpy(out_filename, filename_loc);
            TRACE(("[PARSING] Found output file: %s\n", out_filename));
        }
        else if (argv[idx][0] == '-')
        {
            return EINVAL;
        }
        else
        {
            if (in_filename[0] != '\0' && out_filename[0] != '\0')
                return EINVAL;

            if (in_filename[0] != '\0') {
                strcpy(out_filename, argv[idx]);
                TRACE(("[PARSING] Found output file: %s\n", out_filename));

                continue;
            }

            strcpy(in_filename, argv[idx]);
            TRACE(("[PARSING] Found input file: %s\n", in_filename));
        }
    }

    TRACE(("\n"));

    return EXIT_SUCCESS;
}

FILE* cpp_get_included_file(char* filename, list_t directories) {
    char buffer[BUFFER_LIMIT];
    FILE* res;
    node_t curr;

    if (!filename)
        return NULL;

    TRACE(("[PARSING] Trying to reach file %s...\n", filename));

    res = fopen(filename, "r+");
    if (res)
        return res;

    curr = directories->next;
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
    int res;
    FILE *input, *output;
    if (!symbol_table || !input_filename)
        return EINVAL;


    input = fopen(input_filename, "r");
    if (!input)
        return EINVAL;

    if (output_filename[0] == '\0')
        output = stdout;
    else {
        output = fopen(output_filename, "w+");
        if (!output)
            return EINVAL;
    }

    TRACE(("[PARSING] Parsing input file: %s.\n\n", input_filename));

    res = cpp_recursive_parse_included_file(symbol_table, input, output, directories);

    fclose(input);

    if (output != stdout)
        fclose(output);

    return res;
}

void cpp_process_line(map_t symbol_table, char* line) {
    char symbol[BUFFER_LIMIT], buffer[BUFFER_LIMIT], *symbol_start, *next_delimiter;
    entry_t entry;
    FILE* included_file;

    int i = 0;

    if (!symbol_table || !line)
        return;

    while (*(line + i)) {
        if (strchr(DELIMITERS, line[i])) {
            i++;
            continue;
        }

        symbol_start = line + i;
        next_delimiter = line + i;
        while (!(strchr(DELIMITERS, *next_delimiter++)))
            i++;

        strncpy(symbol, symbol_start, next_delimiter - symbol_start - 1);
        symbol[next_delimiter - symbol_start - 1] = '\0';

        entry = map_get(symbol_table, symbol);
        if (!entry) {
            i++;
            continue;
        }

        strcpy(buffer, next_delimiter - 1);
        symbol_start[0] = '\0';
        strcat(line, entry->value);
        symbol_start[strlen(entry->value)] = '\0';
        strcat(line, buffer);

        i = symbol_start + strlen(entry->value) - line;

        i++;
    }
}

int cpp_skip_inactive_if(FILE* input, char* buffer) {

    int ifs = 0;
    while (fgets(buffer, BUFFER_LIMIT, input)) {
        TRACE(("[IF] Skipping inactive if line...%s", buffer));
        if (strstr(buffer, "#if"))
            ifs++;
        else if (strstr(buffer, "#endif")) {;
            if (!ifs)
                return EXIT_SUCCESS;

            ifs--;
        } else if (strstr(buffer, "#elif")) {
            if (!ifs)
                return EXIT_FAILURE;
        } else if (strstr(buffer, "#else")) {
            if (!ifs)
                return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}

int cpp_recursive_parse_included_file(map_t *symbol_table, FILE *input, FILE* output, list_t directories)
{
    char buffer[BUFFER_LIMIT], *p, *apostrophe, *arrow, *included_filename_loc, included_filename[BUFFER_LIMIT];
    char *closest, *closure, *symbol, *mapping, *space, *newl, *symbol_loc, *c;
    char symbol_buf[BUFFER_LIMIT];
    int res, truth_value, exists, is_first;

    FILE* included_file;

    while (fgets(buffer, BUFFER_LIMIT, input))
    {
        char *p;

        if (!strcmp(buffer, "\n"))
            continue;

        if (p = strstr(buffer, "#include"))
        {
            apostrophe = strchr(buffer, '\"');
            arrow = strchr(buffer, '<');

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
                closest = arrow - buffer < apostrophe - buffer ? arrow + 1 : apostrophe + 1;
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

            included_file = cpp_get_included_file(included_filename, directories);
            if (!included_file)
                return EINVAL;

            TRACE(("[PARSING] Parsing included file: %s\n", included_filename));

            if (res = cpp_recursive_parse_included_file(symbol_table, included_file, output, directories))
                return res;

            fclose(included_file);
        } else if (p = strstr(buffer, "#define ")) {
            cpp_process_line(*symbol_table, buffer);

            symbol = calloc(BUFFER_LIMIT, sizeof(char));
            if (!symbol)
                return ENOMEM;

            symbol_loc = strchr(p, ' ') + 1;

            mapping = calloc(BUFFER_LIMIT, sizeof(char));
            if (!mapping)
                return ENOMEM;

            space = strchr(symbol_loc, ' ');
            if (!space)
            {
                strncpy(symbol, symbol_loc, strlen(symbol_loc) - 1);
                TRACE(("[PARSING] Found symbol without mapping: %s\n", symbol));

                if (res = map_insert(symbol_table, symbol, mapping))
                    return res;

                continue;
            }

            strncpy(symbol, symbol_loc, space - symbol_loc);

            if (!strchr(space, '\\')) {
                strncpy(mapping, space + 1, strlen(space + 1) - 1);
                mapping[strlen(space) - 1] = '\0';

                TRACE(("[PARSING] Found symbol: %s = %s\n", symbol, mapping));

                if (res = map_insert(symbol_table, symbol, mapping))
                    return res;

                continue;
            }

            memmove(buffer, space + 1, strlen(space + 1) + 1);
            is_first = 1;

            while (strchr(buffer, '\\')) {
                c = buffer;
                while (*c == ' ' || *c == '\t')
                    c++;

                memmove(buffer, c, strlen(c) + 1);
                c = strchr(buffer, '\\');
                *c = '\0';
                strcat(mapping, buffer);
                if (!is_first)
                    strcat(mapping, " ");

                fgets(buffer, BUFFER_LIMIT, input);
                is_first = 0;
            }

            c = buffer;
            while (*c == ' ' || *c == '\t')
                c++;

            memmove(buffer, c, strlen(c) + 1);
            c = strchr(buffer, '\n');
            *c = '\0';
            strcat(mapping, buffer);
            fgets(buffer, BUFFER_LIMIT, input);

            TRACE(("[PARSING] Found symbol: %s = %s\n", symbol, mapping));

            if (res = map_insert(symbol_table, symbol, mapping))
                return res;
        } else if ((p = strstr(buffer, "#ifdef ")) || (p = strstr(buffer, "#ifndef "))) {
            truth_value = *(p + 3) == 'd' ? 1 : 0;
            symbol_loc = strchr(buffer, ' ') + 1;
            
            strcpy(symbol_buf, symbol_loc);
            newl = strchr(symbol_buf, '\n');
            *newl = '\0';

            exists = map_get(*symbol_table, symbol_buf) ? 1 : 0;
            TRACE(("[IF%cDEF] Looking for symbol %s. Truth value: %d, exists: %d.\n", truth_value ? '\0' : 'N', symbol_buf, truth_value, exists));
            if (truth_value == exists)
                continue;

            cpp_skip_inactive_if(input, buffer);
        } else if (p = strstr(buffer, "#if ")) {
            do {
                cpp_process_line(*symbol_table, buffer);

                space = strchr(buffer, ' ');
                if (space[1] != '0') {
                    TRACE(("[IF] Active if branch! Condition: %s", space+1));
                    break;
                }
            } while (cpp_skip_inactive_if(input, buffer));
        } else if (p = strstr(buffer, "#elif")) {
            while (cpp_skip_inactive_if(input, buffer)) {}
        } else if (p = strstr(buffer, "else")) {
            while (cpp_skip_inactive_if(input, buffer)) {}
        } else if (p = strstr(buffer, "#endif")) {
            continue;
        } else if (p = strstr(buffer, "#undef")) {
            symbol_loc = strchr(buffer, ' ') + 1;
            
            strncpy(symbol_buf, symbol_loc, strlen(symbol_loc) - 1);
            symbol_buf[strlen(symbol_loc) - 1] = '\0';

            map_remove(*symbol_table, symbol_buf);
            TRACE(("[PARSING] Removed symbol: %s, %d\n", symbol_buf, strlen(symbol_buf)));
        } else {
            cpp_process_line(*symbol_table, buffer);
            fputs(buffer, output);
        }
    }

    return EXIT_SUCCESS;
}