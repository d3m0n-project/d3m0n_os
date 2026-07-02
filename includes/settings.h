#ifndef SETTINGS_H
#define SETTINGS_H

#define STRING_SIZE	128

typedef struct s_conf
{
	// style
	char	wallpaper[STRING_SIZE];
	char	theme[STRING_SIZE];
	int		splash_time;

	// clock
	int		time_mode;

	// launcher
	char	launcher[STRING_SIZE];
}	t_conf;

int		parse_config(t_conf *config);
t_conf	*get_config();

#endif