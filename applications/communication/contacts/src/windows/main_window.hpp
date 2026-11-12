#pragma once

#include "app.hpp"

class MainWindow : public Window
{
private:
	Rect background;
	Text title_text;
	Rect cards[4];
	Rect avatars[4];
	Text initials[4];
	Text names[4];
	const char *contacts[4] = {"Arnaud", "Bertrand", "Colin", "Damien"};
	uint32_t avatar_colors[4] = {0xFFC5A267, 0xFFC1CE9B, 0xFFD3EBFF, 0xFFA6B6FF};

public:
	MainWindow() : Window("Contacts", Size(320), Size(436))
	{
		bg_color = Color(0xFFFFFFFF);
		background.location = Point(0, 0);
		background.width = Size(320);
		background.height = Size(436);
		background.bg_color = Color(0xFF000000);
		add_control(&background);

		title_text.location = Point(0, 0);
		title_text.width = Size(320);
		title_text.height = Size(30);
		title_text.content = "Contacts";
		title_text.color = Color(0xFFFFFFFF);
		title_text.font_size = 14;
		title_text.text_align = ANCHOR_CENTER_X | ANCHOR_CENTER_Y;
		add_control(&title_text);

		for (int i = 0; i < 4; ++i)
		{
			int y = 40 + i * 40;
			cards[i].location = Point(12, y);
			cards[i].width = Size(285);
			cards[i].height = Size(36);
			cards[i].radius = 10;
			cards[i].bg_color = Color(0xFF202020);
			add_control(&cards[i]);

			avatars[i].location = Point(5, 5);
			avatars[i].width = Size(25);
			avatars[i].height = Size(25);
			avatars[i].radius = 12;
			avatars[i].bg_color = Color(avatar_colors[i]);
			cards[i].add_control(&avatars[i]);

			initials[i].location = Point(0, 0);
			initials[i].width = Size(25);
			initials[i].height = Size(25);
			char initial[2] = {contacts[i][0], '\0'};
			initials[i].content = initial;
			initials[i].color = Color(0xFFFFFFFF);
			initials[i].font_size = 12;
			initials[i].text_align = ANCHOR_CENTER_X | ANCHOR_CENTER_Y;
			avatars[i].add_control(&initials[i]);

			names[i].location = Point(45, 5);
			names[i].width = Size(230);
			names[i].height = Size(25);
			names[i].content = contacts[i];
			names[i].color = Color(0xFFFFFFFF);
			names[i].font_size = 12;
			names[i].text_align = ANCHOR_LEFT | ANCHOR_CENTER_Y;
			cards[i].add_control(&names[i]);
		}
	}
};
