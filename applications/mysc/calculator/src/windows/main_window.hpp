#pragma once

#include "app.hpp"

#ifndef TOPBAR_HEIGHT
#define TOPBAR_HEIGHT 32
#endif

class MainWindow : public Window
{
	Rect display_panel;
	Text display_text;
	Button keys[16];
	int current_value;
	int stored_value;
	char operation;
	bool replace_display;

	void show_value(int value)
	{
		char *digits = itoa(value);
		display_text.content = digits;
		free(digits);
	}

	void press(int index)
	{
		static const char labels[16] = {'7','8','9','/','4','5','6','*','1','2','3','-','C','0','=','+'};
		char key = labels[index];
		if (key >= '0' && key <= '9')
		{
			if (replace_display || !strcmp(display_text.content.c_str(), "0"))
			{
				display_text.content = "";
				replace_display = false;
			}
			char digit[2] = {key, '\0'};
			display_text.content = display_text.content + digit;
			current_value = atoi(display_text.content.c_str());
			return;
		}
		if (key == 'C')
		{
			current_value = stored_value = 0;
			operation = 0;
			replace_display = true;
			show_value(0);
			return;
		}
		if (key == '=')
		{
			if (operation == '+') current_value = stored_value + current_value;
			else if (operation == '-') current_value = stored_value - current_value;
			else if (operation == '*') current_value = stored_value * current_value;
			else if (operation == '/')
			{
				if (current_value == 0)
				{
					display_text.content = "Cannot divide by 0";
					operation = 0;
					replace_display = true;
					return;
				}
				current_value = stored_value / current_value;
			}
			operation = 0;
			show_value(current_value);
			replace_display = true;
			return;
		}
		stored_value = current_value;
		operation = key;
		replace_display = true;
	}

public:
	MainWindow() : Window("Calculator", Size(320), Size(480 - TOPBAR_HEIGHT)),
		current_value(0), stored_value(0), operation(0), replace_display(true)
	{
		bg_color = Color(22, 25, 32);
		display_panel.location = Point(16, 16);
		display_panel.width = Size(288);
		display_panel.height = Size(58);
		display_panel.bg_color = Color(38, 43, 52);
		display_text.location = Point(24, 16);
		display_text.width = Size(272);
		display_text.height = Size(58);
		display_text.content = "0";
		display_text.color = Color(255, 255, 255);
		display_text.font_size = 20;
		display_text.text_align = ANCHOR_RIGHT;

		static const char labels[16] = {'7','8','9','/','4','5','6','*','1','2','3','-','C','0','=','+'};
		for (int i = 0; i < 16; ++i)
		{
			int row = i / 4;
			int column = i % 4;
			keys[i].location = Point(16 + column * 74, 88 + row * 68);
			keys[i].width = Size(64);
			keys[i].height = Size(58);
			char label[2] = {labels[i], '\0'};
			keys[i].content = label;
			keys[i].font_size = 16;
			keys[i].color = Color(255, 255, 255);
			keys[i].bg_color = (labels[i] == '+' || labels[i] == '-' || labels[i] == '*' || labels[i] == '/' || labels[i] == '=')
				? Color(68, 110, 180) : (labels[i] == 'C' ? Color(150, 70, 70) : Color(48, 54, 65));
			keys[i].on_click = [this, i](event_t *, Control *) { press(i); };
			add_control(&keys[i]);
		}
		add_control(&display_panel);
		add_control(&display_text);
	}
};
