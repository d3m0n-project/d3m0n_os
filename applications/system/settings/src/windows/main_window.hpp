#pragma once

#include "settings_window.hpp"
#include "style_window.hpp"
#include "about_window.hpp"

class MainWindow : public SettingsWindow
{
	StyleWindow style_window;
	AboutWindow about_window;
	Rect style_option, about_option;
	Text heading, style_label, about_label;

public:
	MainWindow() : SettingsWindow("Settings")
	{
		bg_color = Color(0, 0, 0);
		SettingsLayout::place(heading, 16, 2, 288, 32);
		SettingsLayout::style_text(heading, "Settings", 16);
		SettingsLayout::place(style_option, 16, 48, 288, 48);
		style_option.bg_color = Color(40, 40, 40);
		style_option.on_click = [this](event_t *, Control *) { show_window(style_window); };
		SettingsLayout::place(style_label, 28, 48, 260, 48);
		SettingsLayout::style_text(style_label, "Wallpaper & Style", 10);
		SettingsLayout::place(about_option, 16, 104, 288, 48);
		about_option.bg_color = Color(40, 40, 40);
		about_option.on_click = [this](event_t *, Control *) { show_window(about_window); };
		SettingsLayout::place(about_label, 28, 104, 260, 48);
		SettingsLayout::style_text(about_label, "About", 10);
		add_control(&heading);
		add_control(&style_option);
		add_control(&style_label);
		add_control(&about_option);
		add_control(&about_label);
	}
};
