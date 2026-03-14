#ifndef DTB_H
#define DTB_H

#include "types.h"

#define FDT_MAGIC 0xd00dfeed

#define FDT_BEGIN_NODE  1
#define FDT_END_NODE    2
#define FDT_PROP        3
#define FDT_NOP         4
#define FDT_END         9

typedef struct
{
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
}	fdt_header;

void	dtb_init(void *dtb);
void	*dtb_find_prop(const char *node, const char *prop, uint32_t *len);

#endif