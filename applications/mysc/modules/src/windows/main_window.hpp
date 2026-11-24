#pragma once

#include "app.hpp"

#ifndef TOPBAR_HEIGHT
#define TOPBAR_HEIGHT 32
#endif

class MainWindow : public Window
{
	Rect cards[5], info_panel;
	Text title, subtitle, names[5], descriptions[5], info_title, info_text;
	int selected;

	void select_module(int index)
	{
		static const char *details[5] = {
			"Manage wireless network connections.",
			"Pair and manage nearby Bluetooth devices.",
			"Send and receive infrared commands.",
			"Configure radio frequency tools.",
			"Run USB keyboard emulation scripts."
		};
		selected = index;
		info_title.content = names[index].content;
		info_text.content = details[index];
	}

public:
	MainWindow() : Window("Modules", Size(320), Size(480 - TOPBAR_HEIGHT)), selected(0)
	{
		static const char *module_names[5] = {"Wi-Fi", "Bluetooth", "Infrared", "Radio", "badUSB"};
		static const char *module_descriptions[5] = {"Wireless networking", "Nearby device pairing", "Infrared controls", "Radio tools", "USB keyboard emulation"};
		bg_color = Color(18, 22, 29);
		title.location = Point(16, 4);
		title.width = Size(288);
		title.height = Size(28);
		title.content = "Modules";
		title.color = Color(255, 255, 255);
		title.font_size = 16;
		subtitle.location = Point(16, 34);
		subtitle.width = Size(288);
		subtitle.height = Size(22);
		subtitle.content = "Browse available system modules";
		subtitle.color = Color(165, 177, 193);
		subtitle.font_size = 8;
		for (int i = 0; i < 5; ++i)
		{
			cards[i].location = Point(16, 62 + i * 42);
			cards[i].width = Size(288);
			cards[i].height = Size(36);
			cards[i].bg_color = Color(38, 45, 56);
			cards[i].on_click = [this, i](event_t *, Control *) { select_module(i); };
			names[i].location = Point(26, 63 + i * 42);
			names[i].width = Size(100);
			names[i].height = Size(18);
			names[i].content = module_names[i];
			names[i].color = Color(255, 255, 255);
			names[i].font_size = 10;
			descriptions[i].location = Point(26, 80 + i * 42);
			descriptions[i].width = Size(260);
			descriptions[i].height = Size(14);
			descriptions[i].content = module_descriptions[i];
			descriptions[i].color = Color(170, 183, 198);
			descriptions[i].font_size = 7;
			add_control(&cards[i]);
			add_control(&names[i]);
			add_control(&descriptions[i]);
		}
		info_panel.location = Point(16, 280);
		info_panel.width = Size(288);
		info_panel.height = Size(76);
		info_panel.bg_color = Color(29, 37, 48);
		info_title.location = Point(26, 289);
		info_title.width = Size(268);
		info_title.height = Size(18);
		info_title.color = Color(100, 180, 255);
		info_title.font_size = 10;
		info_text.location = Point(26, 312);
		info_text.width = Size(268);
		info_text.height = Size(34);
		info_text.color = Color(220, 225, 232);
		info_text.font_size = 8;
		add_control(&info_panel);
		add_control(&info_title);
		add_control(&info_text);
		select_module(0);
	}
};
