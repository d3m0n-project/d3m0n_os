#include "controls.h"
#include "log.h"
#include "time.h"
#include "scripting.h"
#include "libft.h"

static void	draw_clock(t_conf *conf, int *current_pos, uint32_t theme_color)
{
	size_t	time = (time_us() % (1000 * 60 * 24 * 1000)) / (1000*1000*60); // minute of the day
	int		hours = time / 60;
	int		minutes = time % 60;

	char	clock[9] = "00:00 AM";
	if (conf->time_mode == 1) // 24h clock
		clock[5] = '\0';
	else if (hours >= 12)
	{
		clock[6] = 'P';
		if (hours > 12)
			hours %= 12;
	}
	clock[0] = '0' + (hours / 10);
	clock[1] = '0' + hours % 10;

	clock[3] = '0' + (minutes / 10);
	clock[4] = '0' + minutes % 10;

	int size = TOPBAR_HEIGHT - TOPBAR_PADDING * 2;
	draw_text(*current_pos + TOPBAR_PADDING, TOPBAR_PADDING, size / 2, size, clock, theme_color, 0); // TODO: change topbar
	*current_pos += (size / 2) * ft_strlen(clock) + TOPBAR_PADDING * 2;
}

static void	draw_battery(int *current_pos, uint32_t theme_color_fg)
{
	BmpTexture	battery_icon = {0};
	char		battery_percentage[5] = "100%";
	int			battery_level = 100; // TODO: battery calculation
	int			size = TOPBAR_HEIGHT - (2*TOPBAR_PADDING);
	if (bmp_load_image(&battery_icon, "/themes/material-design-icons/device/battery_full.bmp"))
	{
		log("TOPBAR: Could not load battery icon\n", LOG_ERROR);
		return;
	}
	if (battery_level < 100)
	{
		if (battery_level > 10)
		{
			battery_percentage[0] = '0' + (battery_level / 10);
			battery_percentage[1] = '0' + (battery_level % 10);
			battery_percentage[2] = '%';
			battery_percentage[3] = '\0';
		} else {
			battery_percentage[0] = '0' + (battery_level % 10);
			battery_percentage[1] = '%';
			battery_percentage[2] = '\0';
		}
	}
	draw_text(*current_pos + TOPBAR_PADDING, TOPBAR_PADDING, size/2, size, battery_percentage, theme_color_fg, 0);
	*current_pos += (size/2) * 4 + TOPBAR_PADDING + TOPBAR_PADDING;
	
	draw_bmp(*current_pos + TOPBAR_PADDING, TOPBAR_PADDING, size, size, &battery_icon, theme_color_fg);
	*current_pos += TOPBAR_HEIGHT;
	free_bmp_texture(&battery_icon);
}

void	draw_connections(int *current_pos, uint32_t theme_color_fg)
{
	static char	*wifi_states[6] = {"notification/wifi_off.bmp", "device/signal_wifi_0_bar.bmp", "device/signal_wifi_1_bar.bmp", "device/signal_wifi_2_bar.bmp", "device/signal_wifi_3_bar.bmp", "device/signal_wifi_4_bar.bmp"};
	BmpTexture	wifi_icon = {0};
	int			size = TOPBAR_HEIGHT - (2*TOPBAR_PADDING);
	int			wifi_level = 1; // TODO: WIFI
	if (wifi_level < 0 || wifi_level > 5)
		wifi_level = 0;
	char		*image_path = path_add("/themes/material-design-icons/", wifi_states[wifi_level]);
	if (!image_path || bmp_load_image(&wifi_icon, image_path))
	{
		log("TOPBAR: Could not load wifi icon\n", LOG_ERROR);
		if (image_path)
			free(image_path);
		return;
	}
	log("image_path = %p\n", LOG_INFO, image_path);

	if (!image_path)
		return;
	free(image_path);
	draw_bmp(*current_pos + TOPBAR_PADDING, TOPBAR_PADDING, size, size, &wifi_icon, theme_color_fg);
	*current_pos += TOPBAR_HEIGHT;
	free_bmp_texture(&wifi_icon);
}

void	draw_topbar(t_window *window)
{
	t_conf		*conf = get_config();
	uint32_t	theme_color_fg = (conf->theme)?LIGHT_THEME_FG:DARK_THEME_FG;
	uint32_t	theme_color_bg = (conf->theme)?LIGHT_THEME_BG:DARK_THEME_BG;
	if (window->top_bar)
	{
		int current_pos = 0;
		draw_rect(0, 0, window->width, TOPBAR_HEIGHT, theme_color_bg);


		// draw battery and connection status
		draw_battery(&current_pos, theme_color_fg);

		draw_connections(&current_pos, theme_color_fg);

		draw_clock(conf, &current_pos, theme_color_fg);
		
		if (!window->is_launcher)
		{
			int size = TOPBAR_HEIGHT - TOPBAR_PADDING * 2;
			draw_text(current_pos, TOPBAR_PADDING, size / 2, size, window->title, DISPLAY_COLORS[MAGENTA], 0);
			current_pos += (size / 2) * ft_strlen(window->title) + TOPBAR_PADDING;
		}
	}
	if (!window->is_launcher)
	{
		int cross_s = TOPBAR_HEIGHT - (2 * TOPBAR_PADDING);
		int cross_x = SCREEN_WIDTH - (cross_s + TOPBAR_PADDING);
		int cross_y = TOPBAR_PADDING;
		
		draw_text(cross_x, cross_y, cross_s, cross_s, "X", DISPLAY_COLORS[RED], 0); // draw exit icon
		// add close event if not added yet
		if (window->events[0].type == EVENT_UNDEFINED)
		{
			window->events[0].script = init_script(0);
			if (!window->events[0].script)
			{
				panic("Could not allocate the app close cross script\n");
				return;
			}
			window->events[0].type = EVENT_ON_CLICK;
			window->events[0].script->func = (void *)fn_app_exit;
			window->events[0].script->args = 0;
			window->events[0].script->next = 0;
			window->events[0].override_trigger_corners[0] = (t_point){.x=cross_x, .y=cross_y};
			window->events[0].override_trigger_corners[1] = (t_point){.x=cross_x+cross_s, .y=cross_y+cross_s};
		}
	}
}