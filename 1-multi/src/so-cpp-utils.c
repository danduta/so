#include <so-cpp-utils.h>

int compare_strings(void* s1, void* s2) {
	return strcmp((char*) s1, (char*) s2);
}

u_int64_t hash_string(void* s) {
    if (!s)
        return 0;

    const int p = 31;
    const int m = 1e9 + 9;

    u_int64_t result = 0;
    u_int64_t poly = 1;

    for (int i = 0; i < strlen(s); i++) {
        char c = ((char*) s)[i];
        result = (result + (c - 'a' + 1) * poly) % m;
        poly = (poly * p) % m;
    }

    return result;
}