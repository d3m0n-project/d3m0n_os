#ifndef PARSING_H
#define PARSING_H

#include "types.h"
#include "controls.h"
#include "log.h"
#include "memory.h"
#include "filesystem.h"

int	parse_layout(const char *path, t_window *win);

int	parse_source(const char *path, t_window *win);

#endif