#ifndef APP_INFO_H
#define APP_INFO_H

#include "elf.h"

int		parse_app_info(elf_header_32 *header, char *buffer);

#endif