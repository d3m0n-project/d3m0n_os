#pragma once

#include "app.hpp"
#include "runtime/vector.hpp"

class MainWindow : public Window
{
private:
	Vector<Image>	app_slots;

	Image	wallpaper;
public:
	MainWindow(void): Window("launcher", Size(320), Size(480 - 32))
	{
		wallpaper.location = Point(0, 0);
		wallpaper.width = Size(320);
		wallpaper.height = Size(320);
		wallpaper.source = "/test.svg";
		//wallpaper.bg_color = Color("#ff00ff");

		this->add_control(&wallpaper);
	};
};
