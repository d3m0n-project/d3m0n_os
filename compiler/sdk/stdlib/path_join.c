#include "stdlib.h"

int	path_join(char *out, size_t out_size, const char *base, const char *name)
{
    if (!out || !out_size || !base || !name)
        return -1;

    size_t base_len = strlen(base);
    size_t name_start = 0;
    while (name[name_start] == '/')
        name_start++;

    if (name[name_start] == '.' && name[name_start + 1] == '/')
        name_start += 2;

    size_t name_len = strlen(name + name_start);
    while (base_len > 1 && base[base_len - 1] == '/')
        base_len--;

    int need_separator = base_len > 0 && name_len > 0 && base[base_len - 1] != '/';
    size_t required = base_len + need_separator + name_len + 1;
    if (required > out_size)
        return -1;

    memcpy(out, base, base_len);
    size_t pos = base_len;
    if (need_separator)
        out[pos++] = '/';

    memcpy(out + pos, name + name_start, name_len);
    out[pos + name_len] = '\0';

    return 0;
}