#ifndef SCRIPTING_H
#define SCRIPTING_H

#include "controls.h"

typedef struct s_app
{
	char	*name;
	char	*icon;
	char	*package;
}	t_app;

extern t_app	*apps;

#define MAX_ARGS	32

typedef enum {
	ARG_NULL=0,
	ARG_STR,
	ARG_INT,
	ARG_OPTIONAL=64,
}	e_arg_type;

char	*get_arg_type_name(e_arg_type type);

typedef struct s_fn_def
{
	char*				(*fn)(void **);
	e_arg_type			args_type[MAX_ARGS];
	const char			*name;
	int					is_return;
}	t_fn_def;

typedef struct s_window_stack
{
	char	*package;
	char	*window;
}	t_window_stack;


/* io */
char*	fn_log(void **args);
char*	fn_alert(void **args);

/* template */
char*	template_load(void **args);

/* apps */
char*	fn_app_list(void **args);
char*	fn_app_open(void **args);
char*	fn_app_exit(void **args);

/* settings */
char*	fn_settings_set(void **args);
char*	fn_settings_get(void **args);

/* windows */
char	*fn_window_open(void **args);
char	*fn_window_go_back(void **args);

/* control state */
char	*fn_state_set(void **args);
char	*fn_state_get(void **args);

/* system */
char	*fn_system_var_get(void **args);


/* string manipulation */
char	*fn_str_len(void **args);
char	*fn_str_is_empty(void **args);
char	*fn_str_is_digit(void **args);
char	*fn_str_is_alpha(void **args);
char	*fn_str_is_alnum(void **args);
char	*fn_str_is_space(void **args);
char	*fn_str_substr(void **args);
char	*fn_str_join(void **args);
char	*fn_str_cmp(void **args);
char	*fn_str_equal(void **args);
char	*fn_str_starts_with(void **args);
char	*fn_str_ends_with(void **args);
char	*fn_str_contains(void **args);
char	*fn_str_lower(void **args);
char	*fn_str_upper(void **args);
char	*fn_str_trim(void **args);
char	*fn_str_trim_left(void **args);
char	*fn_str_trim_right(void **args);
char	*fn_str_replace(void **args);
char	*fn_str_reverse(void **args);

#define HISTORY_STACK_SIZE	128

extern t_fn_def			funcs[];
extern t_window_stack	window_stack[];

#endif

// this part is used due to circular import
#ifndef SCRIPTING_TO_CONTROLS_H
#define SCRIPTING_TO_CONTROLS_H
typedef enum e_script_var_type
{
	SCRIPT_VAR_NONE = 0,
	SCRIPT_VAR_INT,
	SCRIPT_VAR_STR
} t_script_var_type;
#define SCRIPT_VAR_NAME_MAX		31

typedef struct s_script_var
{
	char				name[SCRIPT_VAR_NAME_MAX + 1];
	t_script_var_type	type;
	int					ival;
	char				*sval;
	int					used;
}	t_script_var;
#endif