#include "parsing.h"
#include "get_next_line.h"
#include "libft.h"
#include "controls.h"

static int	get_control_id_by_name(char *name, t_window *window, t_control **ctrl)
{
	t_control	*current = window->controls;
	int			current_id = 1;
	size_t		name_len = ft_strlen(name) + 1;
	if (ft_strncmp(name, "Window", 7) == 0)
		return 0;
	while (current)
	{
		if (ft_strncmp(current->name, name, name_len) == 0)
		{
			if (ctrl)
				*ctrl = current;
			return current_id;
		}
		current = current->p_next;
		current_id++;
	}
	return -1;
}

static e_event_type	get_event_type(char *name)
{
	if (!ft_strncmp(name, "OnCreate", 9))
		return EVENT_ON_CREATE;
	if (!ft_strncmp(name, "OnClick", 8))
		return EVENT_ON_CLICK;
	return EVENT_UNDEFINED;
}

static char	*replace_line_placeholders(const char *line, char **replacements)
{
	size_t	out_len = 0;
	size_t	i = 0;

	if (!line)
		return (0);
	if (!replacements)
		return (ft_strdup(line));
	while (line[i])
	{
		if (line[i] == '$' && ft_isdigit(line[i + 1]))
		{
			size_t	j = i + 1;
			int		idx = 0;
			while (line[j] && ft_isdigit(line[j]))
			{
				idx = (idx * 10) + (line[j] - '0');
				j++;
			}
			if (replacements[idx])
				out_len += ft_strlen(replacements[idx]);
			else
				out_len += j - i;
			i = j;
			continue;
		}
		out_len++;
		i++;
	}
	char	*out = ft_calloc(out_len + 1, sizeof(char));
	size_t	k = 0;
	i = 0;
	if (!out)
		return (0);
	while (line[i])
	{
		if (line[i] == '$' && ft_isdigit(line[i + 1]))
		{
			size_t	j = i + 1;
			int		idx = 0;
			while (line[j] && ft_isdigit(line[j]))
			{
				idx = (idx * 10) + (line[j] - '0');
				j++;
			}
			if (replacements[idx])
			{
				size_t	rlen = ft_strlen(replacements[idx]);
				ft_memcpy(out + k, replacements[idx], rlen);
				k += rlen;
			}
			else
			{
				ft_memcpy(out + k, line + i, j - i);
				k += j - i;
			}
			i = j;
			continue;
		}
		out[k] = line[i];
		k++;
		i++;
	}
	return (out);
}

int	parse_source(const char *path, t_window *win, char **replacements)
{
	char		*line = 0;
	char		*replaced_line = 0;
	int			fd;
	int			i;
	int			line_nb = 0;
	int			event_id = 0; // start at id=1 to keep id=0 to the closing script
	int			current_control_id = -1;

	if (!win)
		return 1;
	log("Parsing source file: '%s'\n", LOG_INFO, path);
	fd = open(path, O_READ);
	if (fd < 0)
	{
		log("Could not open file: %s\n", LOG_ERROR | LOG_INDENT, path);
		return 1;
	}
	// calculate the next event id
	while (!win->events[event_id + 1].type == EVENT_UNDEFINED && event_id + 1 < MAX_WINDOW_EVENTS)
		event_id++;
	while ((line = get_next_line(fd)) != 0)
	{
		replaced_line = replace_line_placeholders(line, replacements);
		free(line);
		line = replaced_line;
		if (!line)
		{
			close(fd);
			return 1;
		}
		i = 0;
		line_nb++;
		// remove new lines
		for (int j = ft_strlen(line); j>=0; j--)
			if (line[j] == '\n' || line[j] == '\r')
				line[j] = '\0';

		// skip whitespaces
		while (line[i] && ((line[i] >= 9 && line[i] <= 13) || line[i] == ' '))
			i++;
		
		if (line[i] == '\0') // skip empty lines
		{
			free(line);
			continue;
		}

		if ((i > 0 && current_control_id == -1)) // invalid: defining nothing
		{
			log("Invalid line at l:%i, '%s'\n", LOG_ERROR | LOG_INDENT, line_nb, line);
			free(line);
			close(fd);
			return 1;
		}
		else if (i == 0)
		{
			if (line[i] == '#' || line[i] == '\0') // skip comments and empty lines
			{
				free(line);
				continue;
			}
			int j = 0;
			while (line[i+j] && line[i + j] != '.')
				j++;
			line[i+j] = 0;

			char	*control_name = line + i;
			char	*event_name = line + i + j + 1;
			int k = 0;
			// isolate the event name
			while (event_name[k + 1] && ft_strncmp(event_name + k, "=>", 2) != 0)
				k++;

			if (event_name[k] == '\0')
			{
				log("Invalid event definition at l.%i: '%s'\n", LOG_ERROR | LOG_INDENT, line_nb, event_name);
				free(line);
				close(fd);
				return 1;
			}
			while (k >= 0 && (event_name[k - 1] == ' ' || event_name[k - 1] == '\t'))
				k--;
			event_name[k] = 0;
			event_id++;
			win->events[event_id].script = init_script(0);
			if (!win->events[event_id].script)
			{
				free(line);
				close(fd);
				return 1;
			}
			win->events[event_id].type = get_event_type(event_name);
			if (win->events[event_id].type == EVENT_UNDEFINED)
			{
				log("Unknown event type: '%s'\n", LOG_ERROR | LOG_INDENT, event_name);
				free(line);
				close(fd);
				return 1;
			}

			log("Assigning event type '%s' to control: '%s'\n", LOG_WARNING | LOG_INDENT, event_name, control_name);
			t_control	*ctrl = 0;
			current_control_id = get_control_id_by_name(control_name, win, &ctrl);
			if (current_control_id == -1)
			{
				log("Could not find control with name: '%s'\n", LOG_ERROR | LOG_INDENT, control_name);
				free(line);
				close(fd);
				return 1;
			}
			// control_id, 0=Window, 1,2,3... = other
			win->events[event_id].affected_control_id = current_control_id;
			if (!ctrl && current_control_id != 0)
				panic("Could not access the control pointer\n");
			else if (current_control_id != 0)
			{
				win->events[event_id].trigger_corners[0] = ctrl->location;
				win->events[event_id].trigger_corners[1] = ctrl->location;
				win->events[event_id].trigger_corners[1].x += ctrl->width;
				win->events[event_id].trigger_corners[1].y += ctrl->height;
			}
		} else if (current_control_id != -1 && i > 0) {
			if (linked_script_add_line(line + i, win->events[event_id].script))
			{
				log("Failed to add line to script\n", LOG_ERROR | LOG_INDENT);
				free(line);
				close(fd);
				return 1;
			}
		} else {
			log("Invalid line at l.%i: '%s'\n", LOG_ERROR | LOG_INDENT, line_nb, line + i);
			free(line);
			close(fd);
			return 1;
		}

		free(line);
	}
	close(fd);
	return 0;
}
