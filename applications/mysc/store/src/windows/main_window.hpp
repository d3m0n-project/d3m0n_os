#pragma once

#include "app.hpp"

#ifndef TOPBAR_HEIGHT
#define TOPBAR_HEIGHT 32
#endif

class MainWindow : public Window
{
	Rect banner, cards[4];
	Text title, subtitle, section, names[4], details[4], footer;
	RoundButton open_buttons[4];

	static void launch(int index)
	{
		static const char *paths[4] = {
			"/apps/photo/gallery/gallery.d3m0n",
			"/apps/mysc/calculator/calculator.d3m0n",
			"/apps/mysc/files/files.d3m0n",
			"/apps/mysc/modules/modules.d3m0n"
		};
		int pid = exec(paths[index], nullptr);
		if (pid <= 0)
		{
			fprintf(stderr, "Failed to start '%s'\n", paths[index]);
			return;
		}
		while (pstate(pid) != PROC_ZOMBIE)
			__asm__ volatile("wfi");
	}

public:
	MainWindow() : Window("d3m0n Store", Size(320), Size(480 - TOPBAR_HEIGHT))
	{
		static const char *app_names[4] = {"Gallery", "Calculator", "Files", "Modules"};
		static const char *app_details[4] = {"Browse your photo collection", "Everyday arithmetic", "Explore files on this device", "Manage device modules"};
		bg_color = Color(18, 22, 29);
		banner.location = Point(14, 8);
		banner.width = Size(292);
		banner.height = Size(66);
		banner.radius = 8;
		banner.bg_color = Color(44, 82, 135);
		title.location = Point(26, 16);
		title.width = Size(270);
		title.height = Size(26);
		title.content = "d3m0n Store";
		title.color = Color(255, 255, 255);
		title.font_size = 16;
		subtitle.location = Point(26, 43);
		subtitle.width = Size(270);
		subtitle.height = Size(18);
		subtitle.content = "Discover apps for your device";
		subtitle.color = Color(225, 235, 248);
		subtitle.font_size = 8;
		section.location = Point(16, 84);
		section.width = Size(288);
		section.height = Size(22);
		section.content = "Featured apps";
		section.color = Color(255, 255, 255);
		section.font_size = 11;
		for (int i = 0; i < 4; ++i)
		{
			int y = 112 + i * 62;
			cards[i].location = Point(16, y);
			cards[i].width = Size(288);
			cards[i].height = Size(54);
			cards[i].bg_color = Color(36, 43, 54);
			names[i].location = Point(26, y + 6);
			names[i].width = Size(170);
			names[i].height = Size(20);
			names[i].content = app_names[i];
			names[i].color = Color(255, 255, 255);
			names[i].font_size = 10;
			details[i].location = Point(26, y + 27);
			details[i].width = Size(174);
			details[i].height = Size(18);
			details[i].content = app_details[i];
			details[i].color = Color(170, 183, 198);
			details[i].font_size = 7;
			open_buttons[i].location = Point(224, y + 12);
			open_buttons[i].width = Size(68);
			open_buttons[i].height = Size(30);
			open_buttons[i].radius = 6;
			open_buttons[i].content = "Open";
			open_buttons[i].font_size = 8;
			open_buttons[i].color = Color(255, 255, 255);
			open_buttons[i].bg_color = Color(58, 112, 184);
			open_buttons[i].on_click = [i](event_t *, Control *) { launch(i); };
			add_control(&cards[i]);
			add_control(&names[i]);
			add_control(&details[i]);
			add_control(&open_buttons[i]);
		}
		footer.location = Point(16, 372);
		footer.width = Size(288);
		footer.height = Size(28);
		footer.content = "Showing apps included with d3m0n OS";
		footer.color = Color(150, 162, 178);
		footer.font_size = 7;
		add_control(&banner);
		add_control(&title);
		add_control(&subtitle);
		add_control(&section);
		add_control(&footer);
	}
};
