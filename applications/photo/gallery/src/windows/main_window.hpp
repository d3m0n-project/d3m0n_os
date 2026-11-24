#pragma once

#include "app.hpp"

#ifndef TOPBAR_HEIGHT
#define TOPBAR_HEIGHT 32
#endif

class MainWindow : public Window
{
	Text title, empty_title, empty_message;
	Rect divider, empty_card;

public:
	MainWindow() : Window("Gallery", Size(320), Size(480 - TOPBAR_HEIGHT))
	{
		printf("[o] loaded gallery\n");
		bg_color = Color(255, 255, 255);
		title.location = Point(18, 18);
		title.width = Size(284);
		title.height = Size(30);
		title.content = "Gallery";
		title.color = Color(25, 25, 25);
		title.font_size = 18;

		divider.location = Point(16, 56);
		divider.width = Size(288);
		divider.height = Size(1);
		divider.bg_color = Color(220, 220, 220);

		empty_card.location = Point(20, 108);
		empty_card.width = Size(280);
		empty_card.height = Size(190);
		empty_card.radius = 12;
		empty_card.bg_color = Color(245, 245, 245);

		empty_title.location = Point(32, 166);
		empty_title.width = Size(256);
		empty_title.height = Size(30);
		empty_title.content = "No photos yet";
		empty_title.color = Color(45, 45, 45);
		empty_title.font_size = 14;
		empty_title.text_align = ANCHOR_CENTER_X;

		empty_message.location = Point(32, 198);
		empty_message.width = Size(256);
		empty_message.height = Size(42);
		empty_message.content = "Photos you take will appear here.";
		empty_message.color = Color(105, 105, 105);
		empty_message.font_size = 9;
		empty_message.text_align = ANCHOR_CENTER_X;

		add_control(&title);
		add_control(&divider);
		add_control(&empty_card);
		add_control(&empty_title);
		add_control(&empty_message);
	}
};
