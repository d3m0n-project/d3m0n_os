#ifndef CONTROLS_H
#define CONTROLS_H

#include "types.h"

typedef struct s_point
{
	int	x;
	int	y;
}	t_point;

typedef enum
{
	ANCHOR_CENTER_X=1,
	ANCHOR_CENTER_Y=2,
	ANCHOR_LEFT=4,
	ANCHOR_RIGHT=8,
	ANCHOR_TOP=16,
	ANCHOR_BOTTOM=32
}	e_control_anchor;

typedef struct s_control
{
	// public attributes
	int					margin_top;
	int					margin_left;
	int					margin_right;
	int					margin_bottom;
	int					width;
	int					height;
	char				name[25];
	uint8_t				visible;
	uint8_t				enabled;
	t_point				location;
	uint32_t			color;
	uint32_t			bg_color;
	struct s_control	*children;
	

	// private attributes
	e_control_anchor	p_anchor;
	e_control_anchor	p_location_override;
	struct s_control	*p_next;
}	t_control;


typedef	struct s_window
{
	char		title[25];
	int			width;
	int			height;
	uint32_t	bg_color;
	uint8_t		top_bar;
	t_control	*controls;
}	t_window;

int		create_window(t_window *out, const char *title, int w, int h);
void	add_control(t_window *to, t_control *control);

#endif