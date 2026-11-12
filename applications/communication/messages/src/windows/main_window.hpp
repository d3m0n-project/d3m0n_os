#pragma once

#include "app.hpp"

class MainWindow : public Window
{
private:
	Rect background;
	Text title_text;
	Rect cards[2];
	Rect avatars[2];
	Text initials[2];
	Text names[2];
	Text previews[2];
	Text dates[2];
	const char *contacts[2] = {"Arnaud", "Bertrand"};
	const char *messages[2] = {"You: Hello World!", "You: Hello World2!"};
	const char *days[2] = {"Wed", "Thu"};
	uint32_t avatar_colors[2] = {0xFFA6B6FF, 0xFFD3EBFF};

public:
	MainWindow() : Window("Messages", Size(320), Size(436))
	{
		bg_color = Color(0xFF000000);
		background.location = Point(0, 0);
		background.width = Size(320);
		background.height = Size(436);
		background.bg_color = Color(0xFF000000);
		add_control(&background);

		title_text.location = Point(0, 0);
		title_text.width = Size(320);
		title_text.height = Size(44);
		title_text.content = "Messages";
		title_text.color = Color(0xFFFFFFFF);
		title_text.font_size = 16;
		title_text.text_align = ANCHOR_CENTER_X | ANCHOR_CENTER_Y;
		add_control(&title_text);

		for (int i = 0; i < 2; ++i)
		{
			int y = 48 + i * 55;
			cards[i].location = Point(16, y);
			cards[i].width = Size(288);
			cards[i].height = Size(50);
			cards[i].radius = 10;
			cards[i].bg_color = Color(0xFF202020);
			add_control(&cards[i]);

			avatars[i].location = Point(8, 8);
			avatars[i].width = Size(35);
			avatars[i].height = Size(35);
			avatars[i].radius = 17;
			avatars[i].bg_color = Color(avatar_colors[i]);
			cards[i].add_control(&avatars[i]);

			char initial[2] = {contacts[i][0], '\0'};
			initials[i].location = Point(0, 0);
			initials[i].width = Size(35);
			initials[i].height = Size(35);
			initials[i].content = initial;
			initials[i].color = Color(0xFFFFFFFF);
			initials[i].font_size = 14;
			initials[i].text_align = ANCHOR_CENTER_X | ANCHOR_CENTER_Y;
			avatars[i].add_control(&initials[i]);

			names[i].location = Point(55, 5);
			names[i].width = Size(150);
			names[i].height = Size(20);
			names[i].content = contacts[i];
			names[i].color = Color(0xFFFFFFFF);
			names[i].font_size = 10;
			names[i].text_align = ANCHOR_LEFT | ANCHOR_CENTER_Y;
			cards[i].add_control(&names[i]);

			previews[i].location = Point(55, 25);
			previews[i].width = Size(200);
			previews[i].height = Size(18);
			previews[i].content = messages[i];
			previews[i].color = Color(0xFFDBDBDB);
			previews[i].font_size = 7;
			previews[i].text_align = ANCHOR_LEFT | ANCHOR_CENTER_Y;
			cards[i].add_control(&previews[i]);

			dates[i].location = Point(218, 5);
			dates[i].width = Size(55);
			dates[i].height = Size(20);
			dates[i].content = days[i];
			dates[i].color = Color(0xFFDBDBDB);
			dates[i].font_size = 9;
			dates[i].text_align = ANCHOR_RIGHT | ANCHOR_CENTER_Y;
			cards[i].add_control(&dates[i]);
		}
	}
};
