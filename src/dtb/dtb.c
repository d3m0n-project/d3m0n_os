#include "dtb.h"
#include "uart.h"

static uint8_t *dtb_base;
static uint32_t *struct_block;
static char *strings_block;

static inline uint32_t be32(uint32_t v)
{
    return __builtin_bswap32(v);
}

static inline uint64_t be64(uint64_t v)
{
    return __builtin_bswap64(v);
}

static int str_eq(const char *a, const char *b)
{
    while (*a || *b)
    {
        if (*a != *b)
            return 0;
        a++;
        b++;
    }
    return 1;
}

void dtb_init(void *dtb)
{
    fdt_header *h = (fdt_header *)dtb;

    if (be32(h->magic) != FDT_MAGIC)
        return;

    dtb_base = (uint8_t *)dtb;

    struct_block = (uint32_t *)(dtb_base + be32(h->off_dt_struct));
    strings_block = (char *)(dtb_base + be32(h->off_dt_strings));
}

void *dtb_find_prop(const char *node, const char *prop, uint32_t *len)
{
    uint32_t *p = struct_block;
    int depth = 0;
    int node_match = 0;

    while (1)
    {
        uint32_t token = be32(*p++);
        if (token == FDT_BEGIN_NODE)
        {
            char *name = (char *)p;
			uart_print(name);
			uart_putc('\n');
            if (str_eq(name, node))
                node_match = 1;
            else
                node_match = 0;
            while (*name)
                name++;
            name++;
            p = (uint32_t *)(((uintptr_t)name + 3) & ~3);
            depth++;
        }
        else if (token == FDT_END_NODE)
        {
            depth--;
            node_match = 0;
        }
        else if (token == FDT_PROP)
        {
            uint32_t plen = be32(*p++);
            uint32_t nameoff = be32(*p++);

            char *pname = strings_block + nameoff;
            void *data = p;

            if (node_match && str_eq(pname, prop))
            {
                if (len)
                    *len = plen;
                return data;
            }

            p = (uint32_t *)((uintptr_t)p + ((plen + 3) & ~3));
        }
        else if (token == FDT_NOP)
            continue;
        else if (token == FDT_END)
            break;
        else
            break;
    }
    return 0;
}