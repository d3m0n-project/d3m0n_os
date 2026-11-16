#pragma once

#include "settings_window.hpp"

class AboutWindow : public SettingsWindow
{
	Rect card, back;
	Text title, section, battery_name, battery_value, charge_name, charge_value, read_name, read_value;
	Text version_title, version, version_name, author, back_label;

	static void set_stat(const char *key, Text &target, const char *suffix)
	{
		t_config_value value = {};
		if (query_config(key, &value) != 0 || value.type != CONFIG_QUERY_INTEGER)
		{
			target.content = "Unavailable";
			return;
		}
		char *number = itoa(value.value.integer);
		target.content = string(number) + suffix;
		free(number);
	}

public:
	AboutWindow() : SettingsWindow("Settings - About")
	{
		bg_color = Color(0, 0, 0);
		SettingsLayout::place(title, 96, 2, 208, 32);
		SettingsLayout::style_text(title, "About", 16);
		SettingsLayout::place(card, 16, 48, 288, 290);
		card.bg_color = Color(24, 24, 24);
		SettingsLayout::place(section, 24, 56, 272, 28);
		SettingsLayout::style_text(section, "Device information", 12);
		SettingsLayout::place(battery_name, 24, 96, 180, 24);
		SettingsLayout::style_text(battery_name, "Battery percentage", 9);
		SettingsLayout::place(battery_value, 210, 96, 80, 24);
		SettingsLayout::style_text(battery_value, "", 9);
		set_stat("battery_percentage", battery_value, "%");
		SettingsLayout::place(charge_name, 24, 124, 180, 24);
		SettingsLayout::style_text(charge_name, "Is Charging", 9);
		SettingsLayout::place(charge_value, 210, 124, 80, 24);
		SettingsLayout::style_text(charge_value, "", 9);
		set_stat("battery_charging", charge_value, "");
		if (!strcmp(charge_value.content.c_str(), "1")) charge_value.content = "Yes";
		else if (!strcmp(charge_value.content.c_str(), "0")) charge_value.content = "No";
		SettingsLayout::place(read_name, 24, 152, 180, 24);
		SettingsLayout::style_text(read_name, "Disk Read", 9);
		SettingsLayout::place(read_value, 210, 152, 80, 24);
		SettingsLayout::style_text(read_value, "", 9);
		set_stat("read_speed", read_value, " Mb/s");
		SettingsLayout::place(version_title, 24, 196, 272, 24);
		SettingsLayout::style_text(version_title, "Kernel Information", 10);
		SettingsLayout::place(version, 24, 224, 272, 24);
		SettingsLayout::style_text(version, "2.0.0", 9);
		SettingsLayout::place(version_name, 24, 248, 272, 24);
		SettingsLayout::style_text(version_name, "outset", 9);
		SettingsLayout::place(author, 24, 292, 272, 24);
		author.content = "4re5 group";
		author.color = Color(255, 165, 0);
		author.font_size = 9;
		SettingsLayout::place(back, 12, 0, 68, 36);
		back.bg_color = Color(35, 35, 35);
		back.on_click = [this](event_t *, Control *) { return_to_parent(); };
		SettingsLayout::place(back_label, 16, 0, 60, 36);
		SettingsLayout::style_text(back_label, "< Back", 9);
		add_control(&title);
		add_control(&back);
		add_control(&back_label);
		add_control(&card);
		add_control(&section);
		add_control(&battery_name);
		add_control(&battery_value);
		add_control(&charge_name);
		add_control(&charge_value);
		add_control(&read_name);
		add_control(&read_value);
		add_control(&version_title);
		add_control(&version);
		add_control(&version_name);
		add_control(&author);
	}
};
