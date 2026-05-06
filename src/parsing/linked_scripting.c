#include "controls.h"
#include "log.h"
#include "libft.h"
#include "scripting.h"

static void	*get_func_from_name(char *fn_name)
{
	size_t	len = ft_strlen(fn_name) + 1;
	for (int i=0; i<FUNCS_SIZE; i++)
	{
		if (ft_strncmp(funcs[i].name, fn_name, len) == 0)
			return funcs[i].fn;
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

	// find the next free line
	while (script->next)
   		script = script->next;

	t_script_chain *new_node = ft_calloc(sizeof(t_script_chain), 1);
	if (!new_node)
	{
		log("Could not allocate script next line\n", LOG_ERROR | LOG_INDENT);
		return 1;
	}
	
	new_node->func = get_func_from_name(function_name);
	if (!new_node->func)
	{
		log("Unknown function: '%s'\n", LOG_ERROR | LOG_INDENT, function_name);
		return 1;
	}
	char	**args = ft_split(args_text, ',');
	if (!args)
	{
		log("Could not allocate memory for arguments\n", LOG_ERROR | LOG_INDENT);
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
			int tmp = 0;
			while (arg[tmp] && arg[tmp] == ' ')
				tmp++;
			if (arg[tmp] && (arg[tmp] == '+' || arg[tmp] == '-'))
				tmp++;
			while (arg[tmp] && (arg[tmp] >= '0' && arg[tmp] <= '9'))
				tmp++;
			if (tmp == arg_len) // int type
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
	script->next = new_node;
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