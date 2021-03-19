#include <so-cpp-utils.h>

int compare_strings(void *s1, void *s2)
{
    return strcmp((char *)s1, (char *)s2);
}

unsigned long long hash_string(void *s)
{
    const int p = 31;
    const int m = 1e9 + 9;

    unsigned long long result = 0;
    unsigned long long poly = 1;

    int i;
    char c;

    if (!s)
        return 0;


    for (i = 0; i < strlen(s); i++)
    {
        c = ((char *)s)[i];
        result = (result + (c - 'a' + 1) * poly) % m;
        poly = (poly * p) % m;
    }

    return result;
}

void dbg_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}