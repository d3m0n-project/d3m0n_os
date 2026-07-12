#ifndef PARSING_H
#define PARSING_H

#include "types.h"
#include "controls.h"
#include "log.h"
#include "memory.h"
#include "filesystem.h"

uint32_t	lp_parse_color(char *str);
t_control	*lp_find_control_by_name(t_control *list, char *name);
int			lp_apply_control_attr(t_control *control, t_window *win, t_control *parent, char *key, char *value);
int			parse_layout(const char *path, t_window *win, char **replacements, int offset_x, int offset_y);

int			parse_source(const char *path, t_window *win, char **replacements);

int			parse_manifest(const char *path, t_window *win);

char		*get_setting(const char *path, const char *key, int	*not_found);

#endif