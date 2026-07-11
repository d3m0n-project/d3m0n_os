#include "controls.h"
#include "log.h"
#include "time.h"
#include "scripting.h"
#include "libft.h"
#include "battery.h"
#include "icons.h"

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
	draw_text(*current_pos + TOPBAR_PADDING, TOPBAR_PADDING, size / 2, size, clock, theme_color, 0); 
	*current_pos += (size / 2) * ft_strlen(clock) + TOPBAR_PADDING * 2;
}

static void	draw_battery(t_conf *conf, int *current_pos, uint32_t theme_color_fg)
{
	char		battery_percentage[5] = "100%";
	int			charging = is_charging();
	int			battery_level = get_battery_level();
	int			size = TOPBAR_HEIGHT - (2*TOPBAR_PADDING);

	char		*battery_levels[] = {conf->icon_battery_alert, conf->icon_battery_20, conf->icon_battery_30, conf->icon_battery_50, conf->icon_battery_60, conf->icon_battery_80, conf->icon_battery_90, conf->icon_battery_100};
	char		*battery_levels_charging[] = {conf->icon_battery_alert, conf->icon_battery_charging_20, conf->icon_battery_charging_30, conf->icon_battery_charging_50, conf->icon_battery_charging_60, conf->icon_battery_charging_80, conf->icon_battery_charging_90, conf->icon_battery_charging_100};
	
	if (battery_level < 100)
	{
		if (battery_level >= 10)
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
	int final_idx = 0;
	if (battery_level < 20)
		final_idx = 0;
	else if (battery_level < 30)
		final_idx = 1;
	else if (battery_level < 50)
		final_idx = 2;
	else if (battery_level < 60)
		final_idx = 3;
	else if (battery_level < 80)
		final_idx = 4;
	else if (battery_level < 90)
		final_idx = 5;
	else if (battery_level < 100)
		final_idx = 6;
	else
		final_idx = 7;

	char	*icon_name = (charging)?battery_levels_charging[final_idx]:battery_levels[final_idx];
	BmpTexture	*battery_icon = get_icon(icon_name, conf);
	if (!battery_icon)
		return;

	draw_text(*current_pos + TOPBAR_PADDING, TOPBAR_PADDING, size/2, size, battery_percentage, theme_color_fg, 0);
	*current_pos += (size/2) * 4 + TOPBAR_PADDING + TOPBAR_PADDING;
	
	draw_bmp(*current_pos + TOPBAR_PADDING, TOPBAR_PADDING, size, size, battery_icon, theme_color_fg);
	*current_pos += TOPBAR_HEIGHT;
}

void	draw_connections(t_conf *conf, int *current_pos, uint32_t theme_color_fg)
{
	char		*wifi_states[] = {
		conf->icon_wifi_off,
		conf->icon_wifi_level0,
		conf->icon_wifi_level1,
		conf->icon_wifi_level2,
		conf->icon_wifi_level3,
		conf->icon_wifi_level4
	};
	int			size = TOPBAR_HEIGHT - (2 * TOPBAR_PADDING);
	int			wifi_level = 5; // TODO: WIFI
	if (wifi_level < 0 || wifi_level > 5)
		wifi_level = 0;
	
	BmpTexture	*wifi_icon = get_icon(wifi_states[wifi_level], conf);
	if (!wifi_icon)
		return;
		
	draw_bmp(*current_pos + TOPBAR_PADDING, TOPBAR_PADDING, size, size, wifi_icon, theme_color_fg);
	*current_pos += TOPBAR_HEIGHT;
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
		draw_battery(conf, &current_pos, theme_color_fg);

		draw_connections(conf, &current_pos, theme_color_fg);

		draw_clock(conf, &current_pos, theme_color_fg);
		
		if (!window->is_launcher)
		{
			int size = TOPBAR_HEIGHT - TOPBAR_PADDING * 2;
			draw_text(current_pos + TOPBAR_PADDING, TOPBAR_PADDING, size / 2, size, window->title, DISPLAY_COLORS[MAGENTA], 0);
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