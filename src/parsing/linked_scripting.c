#include "controls.h"
#include "log.h"
#include "libft.h"
#include "scripting.h"
#include "parsing.h"

#define SCRIPT_MAX_VARS		64
#define SCRIPT_VAR_NAME_MAX	31
#define IF_STACK_MAX		32


t_app	*apps;

typedef enum e_cond_op
{
	COND_NONZERO = 0,
	COND_EQ,
	COND_NE,
	COND_GT,
	COND_LT,
	COND_GE,
	COND_LE
} t_cond_op;

typedef enum e_script_var_type
{
	SCRIPT_VAR_NONE = 0,
	SCRIPT_VAR_INT,
	SCRIPT_VAR_STR
} t_script_var_type;

typedef struct s_script_var
{
	char				name[SCRIPT_VAR_NAME_MAX + 1];
	t_script_var_type	type;
	int					ival;
	char				*sval;
	int					used;
} t_script_var;

typedef struct s_call_node
{
	t_fn_def	*def;
	char		**raw_args;
} t_call_node;

typedef struct s_cond_node
{
	char	*lhs;
	int		op;
	char	*rhs;
} t_cond_node;

typedef struct s_for_apps_node
{
	char	*name_var;
	char	*icon_var;
	char	*package_var;
} t_for_apps_node;

static t_script_var	g_vars[SCRIPT_MAX_VARS];

static void	op_var_set(void **args);
static void	op_var_inc(void **args);
static void	op_var_dec(void **args);
static void	op_if(void **args);
static void	op_else(void **args);
static void	op_end(void **args);
static void	op_while(void **args);
static void	op_for_apps(void **args);
static void	op_call(void **args);

static int	is_space_char(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

static int	is_ident_start(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

static int	is_ident_char(char c)
{
	return (is_ident_start(c) || (c >= '0' && c <= '9'));
}

static char	*trim_in_place(char *s)
{
	int i;
	int j;

	if (!s)
		return 0;
	i = 0;
	while (s[i] && is_space_char(s[i]))
		i++;
	j = ft_strlen(s);
	while (j > i && is_space_char(s[j - 1]))
		j--;
	s[j] = '\0';
	return s + i;
}

static void	rstrip_semicolon(char *s)
{
	int i;

	if (!s)
		return;
	i = ft_strlen(s);
	while (i > 0 && is_space_char(s[i - 1]))
		i--;
	if (i > 0 && s[i - 1] == ';')
		s[i - 1] = '\0';
}

static char	*dup_cstr(const char *s)
{
	char	*out;
	size_t	len;

	if (!s)
		return 0;
	len = ft_strlen(s) + 1;
	out = (char *)ft_calloc(len, sizeof(char));
	if (!out)
		return 0;
	ft_strlcpy(out, s, len);
	return out;
}

static void	int_to_buf(int value, char *out, int out_size)
{
	char				tmp[16];
	int				i;
	int				j;
	int				neg;
	unsigned int	v;

	if (!out || out_size <= 0)
		return;
	if (value == 0)
	{
		out[0] = '0';
		if (out_size > 1)
			out[1] = '\0';
		return;
	}
	neg = (value < 0);
	v = (unsigned int)(neg ? -value : value);
	i = 0;
	while (v > 0 && i < (int)sizeof(tmp) - 1)
	{
		tmp[i++] = (char)('0' + (v % 10));
		v /= 10;
	}
	j = 0;
	if (neg && j < out_size - 1)
		out[j++] = '-';
	while (i > 0 && j < out_size - 1)
		out[j++] = tmp[--i];
	out[j] = '\0';
}

static int	parse_identifier_token(const char *s, int *consumed)
{
	int i;

	if (!s || !is_ident_start(s[0]))
		return 0;
	i = 1;
	while (s[i] && is_ident_char(s[i]))
		i++;
	if (consumed)
		*consumed = i;
	return 1;
}

static int	parse_int_literal(const char *s, int *out)
{
	int i;

	if (!s || !s[0])
		return 0;
	i = 0;
	while (s[i] && is_space_char(s[i]))
		i++;
	if (s[i] == '+' || s[i] == '-')
		i++;
	if (!(s[i] >= '0' && s[i] <= '9'))
		return 0;
	while (s[i] >= '0' && s[i] <= '9')
		i++;
	while (s[i] && is_space_char(s[i]))
		i++;
	if (s[i] != '\0')
		return 0;
	if (out)
		*out = ft_atoi((char *)s);
	return 1;
}

static int	var_find(const char *name)
{
	int i;

	i = 0;
	while (i < SCRIPT_MAX_VARS)
	{
		if (g_vars[i].used && ft_strcmp(g_vars[i].name, (char *)name) == 0)
			return i;
		i++;
	}
	return -1;
}

static int	var_get_or_create(const char *name)
{
	int idx;
	int i;

	idx = var_find(name);
	if (idx >= 0)
		return idx;
	i = 0;
	while (i < SCRIPT_MAX_VARS)
	{
		if (!g_vars[i].used)
		{
			g_vars[i].used = 1;
			g_vars[i].type = SCRIPT_VAR_NONE;
			g_vars[i].ival = 0;
			g_vars[i].sval = 0;
			ft_bzero(g_vars[i].name, sizeof(g_vars[i].name));
			ft_strlcpy(g_vars[i].name, name, sizeof(g_vars[i].name));
			return i;
		}
		i++;
	}
	return -1;
}

static void	var_set_int(const char *name, int value)
{
	int idx;

	idx = var_get_or_create(name);
	if (idx < 0)
	{
		log("Too many script variables, cannot create '%s'\n", LOG_ERROR | LOG_INDENT, name);
		return;
	}
	if (g_vars[idx].type == SCRIPT_VAR_STR && g_vars[idx].sval)
	{
		free(g_vars[idx].sval);
		g_vars[idx].sval = 0;
	}
	g_vars[idx].type = SCRIPT_VAR_INT;
	g_vars[idx].ival = value;
}

static void	var_set_str(const char *name, const char *value)
{
	int idx;
	char *copy;

	idx = var_get_or_create(name);
	if (idx < 0)
	{
		log("Too many script variables, cannot create '%s'\n", LOG_ERROR | LOG_INDENT, name);
		return;
	}
	copy = dup_cstr(value ? value : "");
	if (!copy)
		return;
	if (g_vars[idx].type == SCRIPT_VAR_STR && g_vars[idx].sval)
		free(g_vars[idx].sval);
	g_vars[idx].type = SCRIPT_VAR_STR;
	g_vars[idx].sval = copy;
}

static int	var_get_int(const char *name, int *out)
{
	int idx;

	idx = var_find(name);
	if (idx < 0 || g_vars[idx].type == SCRIPT_VAR_NONE)
		return 0;
	if (g_vars[idx].type == SCRIPT_VAR_INT)
	{
		if (out)
			*out = g_vars[idx].ival;
		return 1;
	}
	if (g_vars[idx].type == SCRIPT_VAR_STR)
	{
		if (!parse_int_literal(g_vars[idx].sval, out))
		{
			if (out)
				*out = 0;
		}
		return 1;
	}
	return 0;
}

static const char	*var_get_str(const char *name)
{
	int idx;
	static char int_buf[16];

	idx = var_find(name);
	if (idx < 0 || g_vars[idx].type == SCRIPT_VAR_NONE)
		return 0;
	if (g_vars[idx].type == SCRIPT_VAR_STR)
		return g_vars[idx].sval;
	if (g_vars[idx].type == SCRIPT_VAR_INT)
	{
		ft_bzero(int_buf, sizeof(int_buf));
		int_to_buf(g_vars[idx].ival, int_buf, (int)sizeof(int_buf));
		return int_buf;
	}
	return 0;
}

static t_fn_def	*get_def_from_name(char *fn_name)
{
	int i = 0;
	while (funcs[i].name)
	{
		if (ft_strcmp((char *)funcs[i].name, fn_name) == 0)
			return &funcs[i];
		i++;
	}
	return 0;
}

static int	append_script_node(t_script_chain *script, t_script_chain *new_node)
{
	t_script_chain *tail;

	if (!script || !new_node)
		return 1;
	if (!script->func && !script->next && !script->args)
	{
		ft_memcpy(script, new_node, sizeof(t_script_chain));
		free(new_node);
		return 0;
	}
	tail = script;
	while (tail->next)
		tail = tail->next;
	tail->next = new_node;
	return 0;
}

static int	make_node_args1(t_script_chain *node, void *a0)
{
	void **args;

	args = (void **)ft_calloc(2, sizeof(void *));
	if (!args)
		return 1;
	args[0] = a0;
	args[1] = 0;
	node->args = args;
	return 0;
}

static int	make_node_args2(t_script_chain *node, void *a0, void *a1)
{
	void **args;

	args = (void **)ft_calloc(3, sizeof(void *));
	if (!args)
		return 1;
	args[0] = a0;
	args[1] = a1;
	args[2] = 0;
	node->args = args;
	return 0;
}

static int	eval_int_expr_rec(const char **p, int *ok);

static void	skip_spaces_expr(const char **p)
{
	while (**p && is_space_char(**p))
		(*p)++;
}

static int	eval_factor(const char **p, int *ok)
{
	int sign;
	int value;
	int consumed;
	char ident[64];
	int i;

	skip_spaces_expr(p);
	sign = 1;
	if (**p == '+')
		(*p)++;
	else if (**p == '-')
	{
		sign = -1;
		(*p)++;
	}
	skip_spaces_expr(p);
	if (**p == '(')
	{
		(*p)++;
		value = eval_int_expr_rec(p, ok);
		skip_spaces_expr(p);
		if (**p != ')')
		{
			*ok = 0;
			return 0;
		}
		(*p)++;
		return sign * value;
	}
	if (**p >= '0' && **p <= '9')
	{
		value = 0;
		while (**p >= '0' && **p <= '9')
		{
			value = value * 10 + (**p - '0');
			(*p)++;
		}
		return sign * value;
	}
	if (parse_identifier_token(*p, &consumed))
	{
		i = 0;
		while (i < consumed && i < (int)sizeof(ident) - 1)
		{
			ident[i] = (*p)[i];
			i++;
		}
		ident[i] = '\0';
		(*p) += consumed;
		if (!var_get_int(ident, &value))
			value = 0;
		return sign * value;
	}
	*ok = 0;
	return 0;
}

static int	eval_term(const char **p, int *ok)
{
	int lhs;
	int rhs;
	char op;

	lhs = eval_factor(p, ok);
	if (!*ok)
		return 0;
	while (1)
	{
		skip_spaces_expr(p);
		op = **p;
		if (op != '*' && op != '/')
			break;
		(*p)++;
		rhs = eval_factor(p, ok);
		if (!*ok)
			return 0;
		if (op == '*')
			lhs *= rhs;
		else
		{
			if (rhs == 0)
			{
				*ok = 0;
				return 0;
			}
			lhs /= rhs;
		}
	}
	return lhs;
}

static int	eval_int_expr_rec(const char **p, int *ok)
{
	int lhs;
	int rhs;
	char op;

	lhs = eval_term(p, ok);
	if (!*ok)
		return 0;
	while (1)
	{
		skip_spaces_expr(p);
		op = **p;
		if (op != '+' && op != '-')
			break;
		(*p)++;
		rhs = eval_term(p, ok);
		if (!*ok)
			return 0;
		if (op == '+')
			lhs += rhs;
		else
			lhs -= rhs;
	}
	return lhs;
}

static int	eval_int_expr(const char *expr, int *out)
{
	const char *p;
	int ok;
	int v;

	p = expr;
	ok = 1;
	v = eval_int_expr_rec(&p, &ok);
	skip_spaces_expr(&p);
	if (!ok || *p != '\0')
		return 0;
	if (out)
		*out = v;
	return 1;
}

static int	parse_arg_type_loose(char *arg, e_arg_type expected)
{
	int i;
	int j;
	int consumed;

	if (!arg)
		return ARG_NULL;
	i = 0;
	while (arg[i] == ' ')
		i++;
	j = ft_strlen(arg) - 1;
	while (j > i && arg[j] == ' ')
		j--;
	if (j >= i && arg[i] == '"' && arg[j] == '"')
		return ARG_STR;
	if (expected == ARG_INT && eval_int_expr(arg, 0))
		return ARG_INT;
	if (parse_identifier_token(arg + i, &consumed))
	{
		int k = i + consumed;
		while (arg[k] == ' ')
			k++;
		if (arg[k] == '\0')
			return ARG_STR;
	}
	if (expected != ARG_INT && eval_int_expr(arg, 0))
		return ARG_INT;
	return ARG_NULL;
}

static int	validate_arg_types(char **args, t_fn_def *def, const char *function_name)
{
	int i;
	int has_optional;
	e_arg_type expected;
	e_arg_type actual;

	if (!args || !def)
		return 1;
	i = 0;
	has_optional = 0;
	while (def->args_type[i] != ARG_NULL || has_optional)
	{
		if (def->args_type[i] & ARG_OPTIONAL)
			has_optional = 1;
		if (!args[i])
		{
			if (has_optional)
				return 0;
			log("Function '%s' expects more arguments\n", LOG_ERROR | LOG_INDENT, function_name);
			return 1;
		}
		expected = (e_arg_type)(def->args_type[i] & ~ARG_OPTIONAL);
		actual = (e_arg_type)parse_arg_type_loose(args[i], expected);
		if (actual == ARG_NULL || expected == ARG_NULL)
		{
			log("Invalid argument syntax for function '%s': '%s'\n", LOG_ERROR | LOG_INDENT, function_name, args[i]);
			return 1;
		}
		if (actual != expected)
		{
			char *expected_str = get_arg_type_name(expected);
			char *actual_str = get_arg_type_name(actual);
			log("Argument type mismatch in '%s' at arg %i which is equal to: '%s': expected %s, got %s\n", LOG_ERROR | LOG_INDENT, function_name, i + 1, args[i], expected_str, actual_str);
			return 1;
		}
		i++;
	}
	if (args[i])
	{
		log("Function '%s' received too many arguments\n", LOG_ERROR | LOG_INDENT, function_name);
		return 1;
	}
	return 0;
}

static int	parse_condition_expr(char *expr, t_cond_node *cond)
{
	char *lhs;
	char *rhs;
	char *op_pos;
	int op;

	lhs = trim_in_place(expr);
	rhs = 0;
	op = COND_NONZERO;
	op_pos = ft_strnstr(lhs, "==", ft_strlen(lhs));
	if (!op_pos)
		op_pos = ft_strnstr(lhs, "!=", ft_strlen(lhs));
	if (!op_pos)
		op_pos = ft_strnstr(lhs, ">=", ft_strlen(lhs));
	if (!op_pos)
		op_pos = ft_strnstr(lhs, "<=", ft_strlen(lhs));
	if (!op_pos)
		op_pos = ft_strnstr(lhs, ">", ft_strlen(lhs));
	if (!op_pos)
		op_pos = ft_strnstr(lhs, "<", ft_strlen(lhs));
	if (op_pos)
	{
		if (op_pos[0] == '=' && op_pos[1] == '=')
			op = COND_EQ;
		else if (op_pos[0] == '!' && op_pos[1] == '=')
			op = COND_NE;
		else if (op_pos[0] == '>' && op_pos[1] == '=')
			op = COND_GE;
		else if (op_pos[0] == '<' && op_pos[1] == '=')
			op = COND_LE;
		else if (op_pos[0] == '>')
			op = COND_GT;
		else
			op = COND_LT;
		if (op == COND_GT || op == COND_LT)
		{
			op_pos[0] = '\0';
			rhs = trim_in_place(op_pos + 1);
		}
		else
		{
			op_pos[0] = '\0';
			op_pos[1] = '\0';
			rhs = trim_in_place(op_pos + 2);
		}
		lhs = trim_in_place(lhs);
		if (!rhs || !rhs[0])
			return 1;
	}
	if (!lhs || !lhs[0])
		return 1;
	cond->lhs = dup_cstr(lhs);
	if (!cond->lhs)
		return 1;
	cond->rhs = rhs ? dup_cstr(rhs) : 0;
	cond->op = op;
	if (rhs && !cond->rhs)
	{
		free(cond->lhs);
		return 1;
	}
	return 0;
}

static int	add_control_node(void (*fn)(void **), t_script_chain *script)
{
	t_script_chain *node;

	node = (t_script_chain *)ft_calloc(sizeof(t_script_chain), 1);
	if (!node)
		return 1;
	node->func = fn;
	node->args = 0;
	node->next = 0;
	return append_script_node(script, node);
}

static int	add_var_set_node(char *name, char *expr, t_script_chain *script)
{
	t_script_chain *node;
	char *name_dup;
	char *expr_dup;

	node = (t_script_chain *)ft_calloc(sizeof(t_script_chain), 1);
	if (!node)
		return 1;
	name_dup = dup_cstr(name);
	expr_dup = dup_cstr(expr);
	if (!name_dup || !expr_dup)
	{
		if (name_dup)
			free(name_dup);
		if (expr_dup)
			free(expr_dup);
		free(node);
		return 1;
	}
	node->func = op_var_set;
	if (make_node_args2(node, name_dup, expr_dup))
	{
		free(name_dup);
		free(expr_dup);
		free(node);
		return 1;
	}
	return append_script_node(script, node);
}

static int	add_var_incdec_node(char *name, int is_inc, t_script_chain *script)
{
	t_script_chain *node;
	char *name_dup;

	node = (t_script_chain *)ft_calloc(sizeof(t_script_chain), 1);
	if (!node)
		return 1;
	name_dup = dup_cstr(name);
	if (!name_dup)
	{
		free(node);
		return 1;
	}
	node->func = is_inc ? op_var_inc : op_var_dec;
	if (make_node_args1(node, name_dup))
	{
		free(name_dup);
		free(node);
		return 1;
	}
	return append_script_node(script, node);
}

static int	add_cond_node(char *statement, t_script_chain *script, void (*fn)(void **))
{
	char *open;
	char *close;
	char *expr;
	t_cond_node *cond;
	t_script_chain *node;

	open = ft_strchr(statement, '(');
	close = ft_strrchr(statement, ')');
	if (!open || !close || close < open)
	{
		log("Invalid condition syntax: '%s'\n", LOG_ERROR | LOG_INDENT, statement);
		return 1;
	}
	*close = '\0';
	expr = trim_in_place(open + 1);
	cond = (t_cond_node *)ft_calloc(1, sizeof(t_cond_node));
	node = (t_script_chain *)ft_calloc(sizeof(t_script_chain), 1);
	if (!cond || !node)
	{
		if (cond)
			free(cond);
		if (node)
			free(node);
		return 1;
	}
	if (parse_condition_expr(expr, cond))
	{
		free(cond);
		free(node);
		log("Invalid condition expression: '%s'\n", LOG_ERROR | LOG_INDENT, expr);
		return 1;
	}
	node->func = fn;
	if (make_node_args1(node, cond))
	{
		free(cond->lhs);
		if (cond->rhs)
			free(cond->rhs);
		free(cond);
		free(node);
		return 1;
	}
	return append_script_node(script, node);
}

static int	parse_for_apps_header(char *statement, char **name_out, char **icon_out,
		char **package_out)
{
	char *open;
	char *close;
	char *inside;
	char *comma;
	char *comma2;
	char *after;
	char *name;
	char *icon;
	char *package;
	int consumed;

	*package_out = 0;
	open = ft_strchr(statement, '(');
	close = ft_strchr(statement, ')');
	if (!open || !close || close < open)
		return 1;
	*close = '\0';
	inside = trim_in_place(open + 1);
	comma = ft_strchr(inside, ',');
	if (!comma)
		return 1;
	*comma = '\0';
	name = trim_in_place(inside);
	icon = trim_in_place(comma + 1);
	comma2 = ft_strchr(icon, ',');
	if (comma2)
	{
		*comma2 = '\0';
		package = trim_in_place(comma2 + 1);
	}
	else
		package = 0;
	icon = trim_in_place(icon);
	if (!parse_identifier_token(name, &consumed) || name[consumed] != '\0')
		return 1;
	if (!parse_identifier_token(icon, &consumed) || icon[consumed] != '\0')
		return 1;
	if (package && (!parse_identifier_token(package, &consumed) || package[consumed] != '\0'))
		return 1;
	after = trim_in_place(close + 1);
	if (ft_strncmp(after, "in", 2) != 0)
		return 1;
	after = trim_in_place(after + 2);
	if (ft_strcmp(after, "app.list()") != 0)
		return 1;
	*name_out = dup_cstr(name);
	*icon_out = dup_cstr(icon);
	if (!*name_out || !*icon_out)
	{
		if (*name_out)
			free(*name_out);
		if (*icon_out)
			free(*icon_out);
		return 1;
	}
	if (package)
	{
		*package_out = dup_cstr(package);
		if (!*package_out)
		{
			free(*name_out);
			free(*icon_out);
			return 1;
		}
	}
	return 0;
}

static int	add_for_apps_node(char *statement, t_script_chain *script)
{
	t_for_apps_node *data;
	t_script_chain *node;

	data = (t_for_apps_node *)ft_calloc(1, sizeof(t_for_apps_node));
	node = (t_script_chain *)ft_calloc(sizeof(t_script_chain), 1);
	if (!data || !node)
	{
		if (data)
			free(data);
		if (node)
			free(node);
		return 1;
	}
	if (parse_for_apps_header(statement, &data->name_var, &data->icon_var,
			&data->package_var))
	{
		free(data);
		free(node);
		log("Invalid for app.list syntax: '%s'\n", LOG_ERROR | LOG_INDENT, statement);
		return 1;
	}
	node->func = op_for_apps;
	if (make_node_args1(node, data))
	{
		free(data->name_var);
		free(data->icon_var);
		if (data->package_var)
			free(data->package_var);
		free(data);
		free(node);
		return 1;
	}
	return append_script_node(script, node);
}

static int	add_call_node(char *statement, t_script_chain *script)
{
	int i;
	int j;
	char *function_name;
	char *args_text;
	t_fn_def *def;
	t_call_node *call;
	t_script_chain *node;
	char **raw_args;

	i = 0;
	while (statement[i] && statement[i] != '(')
		i++;
	if (!statement[i])
	{
		log("Could not find opening bracket for function call: '%s'\n", LOG_ERROR | LOG_INDENT, statement);
		return 1;
	}
	j = i + 1;
	while (statement[j] && statement[j] != ')')
		j++;
	if (!statement[j])
	{
		log("Could not find closing bracket for function call: '%s'\n", LOG_ERROR | LOG_INDENT, statement);
		return 1;
	}
	statement[i] = '\0';
	statement[j] = '\0';
	function_name = trim_in_place(statement);
	args_text = statement + i + 1;
	def = get_def_from_name(function_name);
	if (!def)
	{
		log("Unknown function: '%s'\n", LOG_ERROR | LOG_INDENT, function_name);
		return 1;
	}
	raw_args = ft_split(args_text, ',');
	if (!raw_args)
		return 1;
	if (validate_arg_types(raw_args, def, function_name))
	{
		int k = 0;
		while (raw_args[k])
			free(raw_args[k++]);
		free(raw_args);
		return 1;
	}
	call = (t_call_node *)ft_calloc(1, sizeof(t_call_node));
	node = (t_script_chain *)ft_calloc(sizeof(t_script_chain), 1);
	if (!call || !node)
	{
		if (call)
			free(call);
		if (node)
			free(node);
		return 1;
	}
	call->def = def;
	call->raw_args = raw_args;
	node->func = op_call;
	if (make_node_args1(node, call))
	{
		free(call);
		free(node);
		return 1;
	}
	return append_script_node(script, node);
}

int		linked_script_add_line(char *line, t_script_chain *script)
{
	char *statement;
	int consumed;
	char *eq;
	char *lhs;
	char *rhs;

	if (!line || !script)
		return 1;
	statement = trim_in_place(line);
	rstrip_semicolon(statement);
	statement = trim_in_place(statement);
	if (!statement[0] || statement[0] == '#')
		return 0;
	if (ft_strcmp(statement, "{") == 0)
		return 0;
	if (ft_strcmp(statement, "}") == 0)
		return add_control_node(op_end, script);
	if (ft_strncmp(statement, "else", 4) == 0 && (statement[4] == '\0' || is_space_char(statement[4]) || statement[4] == '{'))
		return add_control_node(op_else, script);
	if (ft_strncmp(statement, "if", 2) == 0 && (is_space_char(statement[2]) || statement[2] == '('))
		return add_cond_node(statement, script, op_if);
	if (ft_strncmp(statement, "while", 5) == 0 && (is_space_char(statement[5]) || statement[5] == '('))
		return add_cond_node(statement, script, op_while);
	if (ft_strncmp(statement, "for", 3) == 0 && (is_space_char(statement[3]) || statement[3] == '('))
		return add_for_apps_node(statement, script);

	if (parse_identifier_token(statement, &consumed))
	{
		if (statement[consumed] == '+' && statement[consumed + 1] == '+' && statement[consumed + 2] == '\0')
		{
			statement[consumed] = '\0';
			return add_var_incdec_node(statement, 1, script);
		}
		if (statement[consumed] == '-' && statement[consumed + 1] == '-' && statement[consumed + 2] == '\0')
		{
			statement[consumed] = '\0';
			return add_var_incdec_node(statement, 0, script);
		}
	}
	eq = ft_strchr(statement, '=');
	if (eq && eq[1] != '=' && (eq == statement || eq[-1] != '='))
	{
		*eq = '\0';
		lhs = trim_in_place(statement);
		rhs = trim_in_place(eq + 1);
		if (!lhs[0] || !parse_identifier_token(lhs, &consumed) || lhs[consumed] != '\0')
		{
			log("Invalid variable name in assignment: '%s'\n", LOG_ERROR | LOG_INDENT, lhs);
			return 1;
		}
		if (eval_int_expr(rhs, 0) == 0)
		{
			log("Invalid integer expression in assignment: '%s'\n", LOG_ERROR | LOG_INDENT, rhs);
			return 1;
		}
		return add_var_set_node(lhs, rhs, script);
	}
	if (ft_strchr(statement, '('))
		return add_call_node(statement, script);
	log("Unknown script statement: '%s'\n", LOG_ERROR | LOG_INDENT, statement);
	return 1;
}

static int	eval_condition(t_cond_node *cond)
{
	int lhs;
	int rhs;

	lhs = 0;
	if (!var_get_int(cond->lhs, &lhs))
		lhs = 0;
	if (cond->op == COND_NONZERO)
		return lhs != 0;
	rhs = 0;
	if (!cond->rhs)
		return 0;
	if (!eval_int_expr(cond->rhs, &rhs))
	{
		if (!var_get_int(cond->rhs, &rhs))
			rhs = 0;
	}
	if (cond->op == COND_EQ)
		return lhs == rhs;
	if (cond->op == COND_NE)
		return lhs != rhs;
	if (cond->op == COND_GT)
		return lhs > rhs;
	if (cond->op == COND_LT)
		return lhs < rhs;
	if (cond->op == COND_GE)
		return lhs >= rhs;
	if (cond->op == COND_LE)
		return lhs <= rhs;
	return 0;
}

static int	is_block_start(t_script_chain *n)
{
	return (n && (n->func == op_if || n->func == op_while || n->func == op_for_apps));
}

static t_script_chain	*find_block_end(t_script_chain *start)
{
	t_script_chain *cur;
	int depth;

	cur = start->next;
	depth = 0;
	while (cur)
	{
		if (is_block_start(cur))
			depth++;
		else if (cur->func == op_end)
		{
			if (depth == 0)
				return cur;
			depth--;
		}
		cur = cur->next;
	}
	return 0;
}

static void	find_if_else_end(t_script_chain *if_node, t_script_chain **else_node, t_script_chain **end_node)
{
	t_script_chain *cur;
	int depth;

	*else_node = 0;
	*end_node = 0;
	cur = if_node->next;
	depth = 0;
	while (cur)
	{
		if (is_block_start(cur))
			depth++;
		else if (cur->func == op_end)
		{
			if (depth == 0)
			{
				*end_node = cur;
				return;
			}
			depth--;
		}
		else if (cur->func == op_else && depth == 0)
			*else_node = cur;
		cur = cur->next;
	}
}

static int	eval_string_arg(char *raw, char **out)
{
	char *t;
	int len;
	const char *var;

	t = trim_in_place(raw);
	len = ft_strlen(t);
	if (len >= 2 && t[0] == '"' && t[len - 1] == '"')
	{
		t[len - 1] = '\0';
		*out = dup_cstr(t + 1);
		return (*out == 0);
	}
	var = var_get_str(t);
	if (var)
		*out = dup_cstr(var);
	else
		*out = dup_cstr(t);
	return (*out == 0);
}

static int	eval_call_and_run(t_call_node *call)
{
	void **eval_args;
	int i;
	e_arg_type expected;

	eval_args = (void **)ft_calloc(MAX_ARGS + 1, sizeof(void *));
	if (!eval_args)
		return 1;
	i = 0;
	while (call->raw_args && call->raw_args[i])
	{
		expected = (e_arg_type)(call->def->args_type[i] & ~ARG_OPTIONAL);
		if (expected == ARG_STR)
		{
			char *tmp = dup_cstr(call->raw_args[i]);
			if (!tmp || eval_string_arg(tmp, (char **)&eval_args[i]))
			{
				if (tmp)
					free(tmp);
				return 1;
			}
			free(tmp);
		}
		else if (expected == ARG_INT)
		{
			int *v = (int *)ft_calloc(1, sizeof(int));
			if (!v)
				return 1;
			if (!eval_int_expr(call->raw_args[i], v))
				*v = 0;
			eval_args[i] = v;
		}
		i++;
	}
	call->def->fn(eval_args);
	i = 0;
	while (eval_args[i])
	{
		free(eval_args[i]);
		i++;
	}
	free(eval_args);
	return 0;
}

static void	exec_range(t_script_chain *start, t_script_chain *stop)
{
	t_script_chain *cur;
	
	cur = start;
	while (cur && cur != stop)
	{
		if (cur->func == op_if)
		{
			t_script_chain *else_node;
			t_script_chain *end_node;
			find_if_else_end(cur, &else_node, &end_node);
			if (!end_node)
			{
				log("Missing closing } for if block\n", LOG_ERROR | LOG_INDENT);
				return;
			}
			if (eval_condition((t_cond_node *)cur->args[0]))
				exec_range(cur->next, else_node ? else_node : end_node);
			else if (else_node)
				exec_range(else_node->next, end_node);
			cur = end_node->next;
			continue;
		}
		if (cur->func == op_while)
		{
			t_script_chain *end_node;
			end_node = find_block_end(cur);
			if (!end_node)
			{
				log("Missing closing } for while block\n", LOG_ERROR | LOG_INDENT);
				return;
			}
			while (eval_condition((t_cond_node *)cur->args[0]))
				exec_range(cur->next, end_node);
			cur = end_node->next;
			continue;
		}
		if (cur->func == op_for_apps)
		{
			t_script_chain *end_node;
			t_for_apps_node *d;
			int i;

			end_node = find_block_end(cur);
			if (!end_node)
			{
				log("Missing closing } for for block\n", LOG_ERROR | LOG_INDENT);
				return;
			}
			d = (t_for_apps_node *)cur->args[0];
			i = 0;
			while (apps[i].name)
			{
				var_set_str(d->name_var, apps[i].name);
				var_set_str(d->icon_var, apps[i].icon);
				if (d->package_var)
					var_set_str(d->package_var, apps[i].package);
				exec_range(cur->next, end_node);
				i++;
			}
			cur = end_node->next;
			continue;
		}
		if (cur->func == op_else || cur->func == op_end)
			return;
		if (cur->func == op_call)
			eval_call_and_run((t_call_node *)cur->args[0]);
		else if (cur->func)
			cur->func(cur->args);
		cur = cur->next;
	}
}

void	exec_script(t_script_chain *script)
{
	int i;

	i = 0;
	while (i < SCRIPT_MAX_VARS)
	{
		if (g_vars[i].used && g_vars[i].type == SCRIPT_VAR_STR && g_vars[i].sval)
			free(g_vars[i].sval);
		g_vars[i].used = 0;
		g_vars[i].type = SCRIPT_VAR_NONE;
		g_vars[i].sval = 0;
		g_vars[i].ival = 0;
		i++;
	}
	exec_range(script, 0);
	draw_window(get_current_window());
}

static void	op_var_set(void **args)
{
	int value;
	char *name;
	char *expr;

	if (!args || !args[0] || !args[1])
		return;
	name = (char *)args[0];
	expr = (char *)args[1];
	if (!eval_int_expr(expr, &value))
		value = 0;
	var_set_int(name, value);
}

static void	op_var_inc(void **args)
{
	int v;
	char *name;

	if (!args || !args[0])
		return;
	name = (char *)args[0];
	if (!var_get_int(name, &v))
		v = 0;
	var_set_int(name, v + 1);
}

static void	op_var_dec(void **args)
{
	int v;
	char *name;

	if (!args || !args[0])
		return;
	name = (char *)args[0];
	if (!var_get_int(name, &v))
		v = 0;
	var_set_int(name, v - 1);
}

static void	op_if(void **args)
{
	(void)args;
}

static void	op_else(void **args)
{
	(void)args;
}

static void	op_end(void **args)
{
	(void)args;
}

static void	op_while(void **args)
{
	(void)args;
}

static void	op_for_apps(void **args)
{
	(void)args;
}

static void	op_call(void **args)
{
	(void)args;
}

void	free_script(t_script_chain *script)
{
	t_script_chain *cur;

	cur = script;
	while (cur)
	{
		if (cur->args)
		{
			if (cur->func == op_call && cur->args[0])
			{
				t_call_node *call = (t_call_node *)cur->args[0];
				int i = 0;
				if (call->raw_args)
				{
					while (call->raw_args[i])
						free(call->raw_args[i++]);
					free(call->raw_args);
				}
				free(call);
			}
			else if ((cur->func == op_if || cur->func == op_while) && cur->args[0])
			{
				t_cond_node *c = (t_cond_node *)cur->args[0];
				free(c->lhs);
				if (c->rhs)
					free(c->rhs);
				free(c);
			}
			else if (cur->func == op_for_apps && cur->args[0])
			{
				t_for_apps_node *f = (t_for_apps_node *)cur->args[0];
				free(f->name_var);
				free(f->icon_var);
				if (f->package_var)
					free(f->package_var);
				free(f);
			}
			else
			{
				int i = 0;
				while (cur->args[i])
				{
					free(cur->args[i]);
					i++;
				}
			}
			free(cur->args);
		}
		cur = cur->next;
	}
}

t_script_chain	*init_script(t_script_chain *script)
{
	if (!script)
	{
		script = ft_calloc(sizeof(t_script_chain), 1);
		if (!script)
		{
			log("Could not allcate script\n", LOG_ERROR | LOG_INDENT);
			return 0;
		}
	}
	else
		script->next = 0;
	return script;
}
