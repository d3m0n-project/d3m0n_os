#include "controls.h"
#include "display.h"

int	create_window(t_window *out, const char *title, int w, int h)
{
	int	i = 0;
	if (!title)
		return 1;
	if (w < 0 || h < 0 || w > SCREEN_WIDTH || h > SCREEN_HEIGHT)
		return 1;
	while (title[i] && i < 24)
	{
		out->title[i] = title[i];
		i++;
	}
	if (title[i])
		return 1;
	out->title[i] = '\0';
	out->width = w;
	out->height = h;
	out->top_bar = 1;
	out->controls = 0;
	return 0;
}

void	add_control(t_window *to, t_control *control)
{
	t_control	*current = to->controls;

	while (current)
		current = current->p_next;
	current->p_next = control;
	control->p_next = 0;
}