#include <so-cpp-hashmap.h>
#include <so-cpp-utils.h>

int main(int argc, char const *argv[])
{
    struct hashmap* map = map_alloc(10, 0.75, &hash_string, &compare_strings);
    DIE(!map, "alloc");

    char names[][LINE_LIMIT] = {
        "dan", "duta",
        "ana-maria", "radu",
        "leonardo", "davinci",
        "donald", "trump"
    };

    for (int i = 0; i < 8; i +=2) {
        map_insert(map, names[i], names[i+1]);
    }

    debug_print(map);
    map_dealloc(map);

    return 0;
}
