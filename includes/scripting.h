#ifndef SCRIPTING_H
#define SCRIPTING_H

typedef struct s_fn_def
{
	void		(*fn)(void **);
	const char	*name;
}	t_fn_def;


/* io */
void	fn_log(void **args);
void	fn_alert(void **args);




#define FUNCS_SIZE 2
extern	t_fn_def	funcs[FUNCS_SIZE];

#endif