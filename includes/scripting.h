#ifndef SCRIPTING_H
#define SCRIPTING_H

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
	void				(*fn)(void **);
	e_arg_type			args_type[MAX_ARGS];
	const char			*name;
}	t_fn_def;

/* io */
void	fn_log(void **args);
void	fn_alert(void **args);

/* template */
void	template_load(void **args);

/* apps */
void	fn_app_list(void **args);
void	fn_app_open(void **args);
void	fn_app_exit(void **args);

extern	t_fn_def	funcs[];

#endif