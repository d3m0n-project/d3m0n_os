#pragma once

#include "app.hpp"

class MainWindow : public Window
{
private:
	Rect rect_transparent;
	Rect rect_black;
	Rect rect_dark_grey;
	Rect rect_grey;
	Rect rect_white;
	Rect rect_red;
	Rect rect_dark_red;
	Rect rect_orange;
	Rect rect_yellow;
	Rect rect_lime;
	Rect rect_green;
	Rect rect_dark_cyan;
	Rect rect_cyan;
	Rect rect_dark_blue;
	Rect rect_blue;
	Rect rect_dark_magenta;
	Rect rect_magenta;

	Text text_title;

public:
	MainWindow(void) : Window("Main Window", Size(320), Size(436))
	{
		int rect_step = this->height.get() * 0.05;
		Size rect_height(rect_step);
		int increment_step = 0;

		// Transparent
		rect_transparent.location = Point(0, increment_step++ * rect_step);
		rect_transparent.bg_color = Color(0x00000000);
		rect_transparent.width = this->width;
		rect_transparent.height = rect_height;

		// Black
		rect_black.location = Point(0, increment_step++ * rect_step);
		rect_black.bg_color = Color(0xFF000000);
		rect_black.width = this->width;
		rect_black.height = rect_height;

		// Dark grey
		rect_dark_grey.location = Point(0, increment_step++ * rect_step);
		rect_dark_grey.bg_color = Color(0xFF404040);
		rect_dark_grey.width = this->width;
		rect_dark_grey.height = rect_height;

		// Grey
		rect_grey.location = Point(0, increment_step++ * rect_step);
		rect_grey.bg_color = Color(0xFF808080);
		rect_grey.width = this->width;
		rect_grey.height = rect_height;

		// White
		rect_white.location = Point(0, increment_step++ * rect_step);
		rect_white.bg_color = Color(0xFFFFFFFF);
		rect_white.width = this->width;
		rect_white.height = rect_height;

		// Red
		rect_red.location = Point(0, increment_step++ * rect_step);
		rect_red.bg_color = Color(0xFFFF0000);
		rect_red.width = this->width;
		rect_red.height = rect_height;

		// Dark red
		rect_dark_red.location = Point(0, increment_step++ * rect_step);
		rect_dark_red.bg_color = Color(0xFF8B0000);
		rect_dark_red.width = this->width;
		rect_dark_red.height = rect_height;

		// Orange
		rect_orange.location = Point(0, increment_step++ * rect_step);
		rect_orange.bg_color = Color(0xFFFFA500);
		rect_orange.width = this->width;
		rect_orange.height = rect_height;

		// Yellow
		rect_yellow.location = Point(0, increment_step++ * rect_step);
		rect_yellow.bg_color = Color(0xFFFFFF00);
		rect_yellow.width = this->width;
		rect_yellow.height = rect_height;

		// Lime
		rect_lime.location = Point(0, increment_step++ * rect_step);
		rect_lime.bg_color = Color(0xFF00FF00);
		rect_lime.width = this->width;
		rect_lime.height = rect_height;

		// Green
		rect_green.location = Point(0, increment_step++ * rect_step);
		rect_green.bg_color = Color(0xFF008000);
		rect_green.width = this->width;
		rect_green.height = rect_height;

		// Dark cyan
		rect_dark_cyan.location = Point(0, increment_step++ * rect_step);
		rect_dark_cyan.bg_color = Color(0xFF008B8B);
		rect_dark_cyan.width = this->width;
		rect_dark_cyan.height = rect_height;

		// Cyan
		rect_cyan.location = Point(0, increment_step++ * rect_step);
		rect_cyan.bg_color = Color(0xFF00FFFF);
		rect_cyan.width = this->width;
		rect_cyan.height = rect_height;

		// Dark blue
		rect_dark_blue.location = Point(0, increment_step++ * rect_step);
		rect_dark_blue.bg_color = Color(0xFF00008B);
		rect_dark_blue.width = this->width;
		rect_dark_blue.height = rect_height;

		// Blue
		rect_blue.location = Point(0, increment_step++ * rect_step);
		rect_blue.bg_color = Color(0xFF0000FF);
		rect_blue.width = this->width;
		rect_blue.height = rect_height;

		// Dark magenta
		rect_dark_magenta.location = Point(0, increment_step++ * rect_step);
		rect_dark_magenta.bg_color = Color(0xFF8B008B);
		rect_dark_magenta.width = this->width;
		rect_dark_magenta.height = rect_height;

		// Magenta
		rect_magenta.location = Point(0, increment_step++ * rect_step);
		rect_magenta.bg_color = Color(0xFFFF00FF);
		rect_magenta.width = this->width;
		rect_magenta.height = rect_height;

		// Title
		text_title.location = Point(0, increment_step++ * rect_step);
		text_title.color = Color(0xFFFF0000);
		text_title.content = "Color Test";
		text_title.font_size = 20;
		text_title.width = this->width;
		text_title.height = Size(15);

		this->add_control(&rect_transparent);
		this->add_control(&rect_black);
		this->add_control(&rect_dark_grey);
		this->add_control(&rect_grey);
		this->add_control(&rect_white);
		this->add_control(&rect_red);
		this->add_control(&rect_dark_red);
		this->add_control(&rect_orange);
		this->add_control(&rect_yellow);
		this->add_control(&rect_lime);
		this->add_control(&rect_green);
		this->add_control(&rect_dark_cyan);
		this->add_control(&rect_cyan);
		this->add_control(&rect_dark_blue);
		this->add_control(&rect_blue);
		this->add_control(&rect_dark_magenta);
		this->add_control(&rect_magenta);

		this->add_control(&text_title);
	}
};