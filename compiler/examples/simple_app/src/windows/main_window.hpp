#pragma once

#include "app.hpp"

class MainWindow : public Window
{
private:
	Rect	rect1;
public:
	MainWindow(void): Window("Main Window", Size(320), Size(480)) {
		rect1.location = Point(0, 0);
		rect1.bg_color = Color(0xFFFFFFFF);
		rect1.width = Size(200);
		rect1.height = Size(50);

		this->add_control(&rect1);
	};
};