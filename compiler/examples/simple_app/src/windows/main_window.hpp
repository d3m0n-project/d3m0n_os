#pragma once

#include "app.hpp"

class MainWindow : public Window
{
private:
	Rect	rect1;
	Text	text1;
public:
	MainWindow(void): Window("Main Window", Size(320), Size(436)) {
		rect1.location = Point(0, 0);
		rect1.bg_color = Color(0xFF000000);
		rect1.width = Size(200);
		rect1.height = Size(50);

		text1.location = Point(10, 10);
		text1.color = Color(0xFFFFFFFF);
		text1.content = "Hello World!";

		this->add_control(&rect1);
		this->add_control(&text1);
	};
};
