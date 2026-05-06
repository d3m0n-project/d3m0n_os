#include "controls.h"
#include "log.h"
#include "libft.h"
#include "scripting.h"

static t_fn_def	*get_def_from_name(char *fn_name)
{
	for (int i = 0; i < FUNCS_SIZE; i++)
	{
		if (ft_strcmp((char *)funcs[i].name, fn_name) == 0)
			return &funcs[i];
	}
	return 0;
}

static e_arg_type	parse_arg_type(char *arg)
{
	int i;
	int j;

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
	if (arg[i] == '+' || arg[i] == '-')
		i++;
	while (arg[i] >= '0' && arg[i] <= '9')
		i++;
	while (arg[i] == ' ')
		i++;
	return (arg[i] == '\0') ? ARG_INT : ARG_NULL;
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
		actual = parse_arg_type(args[i]);
		if (actual == ARG_NULL || expected == ARG_NULL)
		{
			log("Invalid argument syntax for function '%s': '%s'\n", LOG_ERROR | LOG_INDENT, function_name, args[i]);
			return 1;
		}
		if (actual != expected)
		{
			char *expected_str = get_arg_type_name(expected);
			char *actual_str = get_arg_type_name(actual);
			log("Argument type mismatch in '%s' at arg %i: expected %s, got %s\n", LOG_ERROR | LOG_INDENT, function_name, i + 1, expected_str, actual_str);
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

int		linked_script_add_line(char *line, t_script_chain *script)
{
	log("Adding a linked script line...\n", LOG_INFO | LOG_INDENT);
	int	i = 0;
	while (line[i] && line[i] != '(')
		i++;
	if (!line[i])
	{
		log("Could not find opening bracker for function call: '%s'\n", LOG_ERROR | LOG_INDENT, line);
		return 1;
	}

	int j = i + 1;
	while (line[j] && line[j] != ')')
		j++;
	if (!line[j])
	{
		log("Could not find closing bracker for function call: '%s'\n", LOG_ERROR | LOG_INDENT, line);
		return 1;
	}
	// split the function name and the function args
	line[i] = '\0';
	line[j] = '\0';
	char *function_name = line;
	char *args_text = line + i + 1;
	t_fn_def *def = get_def_from_name(function_name);

	t_script_chain *new_node = ft_calloc(sizeof(t_script_chain), 1);
	if (!new_node)
	{
		log("Could not allocate script next line\n", LOG_ERROR | LOG_INDENT);
		return 1;
	}
	
	if (!def)
	{
		log("Unknown function: '%s'\n", LOG_ERROR | LOG_INDENT, function_name);
		free(new_node);
		return 1;
	}
	new_node->func = def->fn;
	char	**args = ft_split(args_text, ',');
	if (!args)
	{
		log("Could not allocate memory for arguments\n", LOG_ERROR | LOG_INDENT);
		free(new_node);
		return 1;
	}
	if (validate_arg_types(args, def, function_name))
	{
		int idx = 0;
		while (args[idx])
		{
			free(args[idx]);
			idx++;
		}
		free(args);
		free(new_node);
		return 1;
	}
	int k = 0;
	while (args[k])
	{
		char	*arg = args[k];
		int		arg_len = ft_strlen(arg);
		int		l = 0;
		int		r = arg_len - 1;
		while (arg[l] == ' ')
			l++;
		while (r > l && arg[r] == ' ')
			r--;
		

		if (arg[l] == '"' && arg[r] == '"') // str arg
		{
			char	*new_str = ft_calloc((r - l), sizeof(char));
			if (!new_str)
			{
				log("Could not allocate memory for arguments\n", LOG_ERROR | LOG_INDENT);
				// TODO: free
				return 1;
			}
			ft_strlcpy(new_str, arg + l + 1, (r - l));
			free(args[k]);
			args[k] = new_str;
		}
		else
		{
			if (parse_arg_type(arg) == ARG_INT) // int type
			{
				int nb = ft_atoi(arg);
				free(args[k]);
				args[k] = malloc(sizeof(int));
				if (!args[k])
				{
					log("Could not allocate memory for argument\n", LOG_ERROR | LOG_INDENT);
					// TODO: free all args
					return 1;
				}
				*(int *)args[k] = nb;
			}
			else
			{
				log("Unknown arg type: '%s'\n", LOG_ERROR | LOG_INDENT, arg);
				// TODO: free all
				return 1;
			}
		}
		k++;
	}
	new_node->args = (void **)args;
	new_node->next = 0;
	if (!script->func && !script->next && !script->args) // no line set already
	{
		ft_memcpy(script, new_node, sizeof(t_script_chain));
		free(new_node);
	}
	else
	{
		// find the next free line
		while (script->next)
			script = script->next;
		script->next = new_node;
	}
	
	return 0;
}

void	exec_script(t_script_chain *script)
{
	t_script_chain	*current = script;
	int	i = 0;

	while (current)
	{
		i++;
		if (current->func)
			current->func(current->args);
		else
			log("No executable function given at l.%i\n", LOG_ERROR | LOG_INDENT, i);
		current = current->next;
	}
}

void	free_script(t_script_chain *script)
{
	t_script_chain	*current = script;

	while (current)
	{
		int k = 0;
		while (current->args[k])
		{
			free(current->args[k]);
			k++;
		}
		current = current->next;
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