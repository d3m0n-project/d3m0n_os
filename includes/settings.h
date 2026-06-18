#ifndef SETTINGS_H
#define SETTINGS_H

typedef struct s_conf
{
	// style
	char	wallpaper[128];
	char	theme[128];
	int		splash_time;

	// clock
	int		time_mode;
}	t_conf;

#endif