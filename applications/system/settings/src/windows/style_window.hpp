#pragma once

#include "settings_window.hpp"

#ifndef SYS_SET_CONFIG
#define SYS_SET_CONFIG 29
#endif

class StyleWindow : public SettingsWindow
{
	Rect theme_row, clock_row, back;
	Text title, theme_label, clock_label, back_label;
	Switch theme_switch, clock_switch;

	static int setting(const char *key)
	{
		t_config_value value = {};
		return query_config(key, &value) == 0 && value.type == CONFIG_QUERY_INTEGER ? value.value.integer : 0;
	}

	static void save(const char *key, bool enabled)
	{
		char value[2] = { enabled ? '1' : '0', '\0' };
		syscall(SYS_SET_CONFIG, (uint32_t)key, (uint32_t)value, 0, 0);
	}

public:
	StyleWindow() : SettingsWindow("Wallpaper & Style")
	{
		bg_color = Color(0, 0, 0);
		SettingsLayout::place(title, 16, 2, 288, 32);
		SettingsLayout::style_text(title, "Wallpaper & Style", 16);
		SettingsLayout::place(theme_row, 16, 48, 288, 48);
		theme_row.bg_color = Color(40, 40, 40);
		SettingsLayout::place(theme_label, 28, 48, 210, 48);
		SettingsLayout::style_text(theme_label, "Light Theme", 10);
		SettingsLayout::place(theme_switch, 254, 60, 40, 24);
		theme_switch.color = Color(255, 180, 0);
		theme_switch.bg_color = Color(100, 100, 100);
		theme_switch.checked = setting("theme") != 0;
		theme_switch.on_click = [this](event_t *, Control *) { theme_switch.checked = !theme_switch.checked; save("theme", theme_switch.checked); };
		SettingsLayout::place(clock_row, 16, 104, 288, 48);
		clock_row.bg_color = Color(40, 40, 40);
		SettingsLayout::place(clock_label, 28, 104, 210, 48);
		SettingsLayout::style_text(clock_label, "Use 24h clock", 10);
		SettingsLayout::place(clock_switch, 254, 116, 40, 24);
		clock_switch.color = Color(255, 180, 0);
		clock_switch.bg_color = Color(100, 100, 100);
		clock_switch.checked = setting("time_mode") != 0;
		clock_switch.on_click = [this](event_t *, Control *) { clock_switch.checked = !clock_switch.checked; save("time_mode", clock_switch.checked); };
		SettingsLayout::place(back, 12, 0, 68, 36);
		back.bg_color = Color(35, 35, 35);
		back.on_click = [this](event_t *, Control *) { return_to_parent(); };
		SettingsLayout::place(back_label, 16, 0, 60, 36);
		SettingsLayout::style_text(back_label, "< Back", 9);
		add_control(&title);
		add_control(&back);
		add_control(&back_label);
		add_control(&theme_row);
		add_control(&theme_label);
		add_control(&theme_switch);
		add_control(&clock_row);
		add_control(&clock_label);
		add_control(&clock_switch);
	}
};
