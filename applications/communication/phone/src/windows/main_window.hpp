#pragma once

#include "app.hpp"

class MainWindow : public Window
{
private:
	Rect	background;
	Text	phone_number;
	Text	call_status;
	Button	keypad[12];
	Rect	call_button;
	Text	call_label;
	Button	backspace_button;
	char	number[32];

	void update_number()
	{
		phone_number.content = number;
	}

	void erase_last_digit()
	{
		int length = 0;
		while (number[length]) ++length;
		if (length > 0)
			number[length - 1] = '\0';
		update_number();
	}

public:
	MainWindow() : Window("Phone", Size(320), Size(480-32)), number{}
	{
		bg_color = Color(0xFF202020);
		background.location = Point(0, 0);
		background.width = this->width;
		background.height = this->height;
		background.bg_color = Color(0xFF202020);
		add_control(&background);

		phone_number.location = Point(30, 50);
		phone_number.width = Size(265);
		phone_number.height = Size(55);
		phone_number.color = Color(0xFFFFFFFF);
		phone_number.font_size = 20;
		phone_number.text_align = ANCHOR_CENTER_X | ANCHOR_CENTER_Y;
		add_control(&phone_number);

		call_status.location = Point(30, 108);
		call_status.width = Size(265);
		call_status.height = Size(20);
		call_status.color = Color(0xFFFFA500);
		call_status.font_size = 9;
		call_status.text_align = ANCHOR_CENTER_X | ANCHOR_CENTER_Y;
		add_control(&call_status);

		for (int i = 0; i < 12; ++i)
		{
			int x = 76 + (i % 3) * 60;
			int y = 140 + (i / 3) * 60;
			const char keys[] = "123456789*0#";
			char label[2] = {keys[i], '\0'};
			keypad[i].location = Point(x, y);
			keypad[i].width = Size(48);
			keypad[i].height = Size(48);
			keypad[i].content = label;
			keypad[i].color = Color(0xFFFFFFFF);
			keypad[i].font_size = 17;
			keypad[i].text_align = ANCHOR_CENTER_X | ANCHOR_CENTER_Y;
			keypad[i].on_click = [this, i](event_t *, Control *) {
				int length = 0;
				while (number[length]) ++length;
				if (length < (int)sizeof(number) - 1)
				{
					number[length] = keypad[i].content[0];
					number[length + 1] = '\0';
					update_number();
					call_status.content = "";
				}
			};
			add_control(&keypad[i]);
		}

		call_button.location = Point(135, 390);
		call_button.width = Size(50);
		call_button.height = Size(50);
		call_button.radius = 25;
		call_button.bg_color = Color(0xFF00AA00);
		call_button.on_click = [this](event_t *, Control *) {
			if (number[0])
			{
				call_status.content = string("Calling: ") + string(number);
				printf("calling: %s\n", number);
			}
		};
		add_control(&call_button);
		call_label.location = Point(0, 0);
		call_label.width = Size(50);
		call_label.height = Size(50);
		call_label.content = "Call";
		call_label.color = Color(0xFFFFFFFF);
		call_label.font_size = 9;
		call_label.text_align = ANCHOR_CENTER_X | ANCHOR_CENTER_Y;
		call_button.add_control(&call_label);

		backspace_button.location = Point(204, 399);
		backspace_button.width = Size(50);
		backspace_button.height = Size(32);
		backspace_button.content = "DEL";
		backspace_button.color = Color(0xFFFFFFFF);
		backspace_button.font_size = 10;
		backspace_button.text_align = ANCHOR_CENTER_X | ANCHOR_CENTER_Y;
		backspace_button.on_click = [this](event_t *, Control *) { erase_last_digit(); };
		add_control(&backspace_button);
	}
};
