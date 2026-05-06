#ifndef SCRIPTING_H
#define SCRIPTING_H

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




#define FUNCS_SIZE 3
extern	t_fn_def	funcs[FUNCS_SIZE];

#endif