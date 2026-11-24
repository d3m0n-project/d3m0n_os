#pragma once

#include "app.hpp"

#ifndef TOPBAR_HEIGHT
#define TOPBAR_HEIGHT 32
#endif

class MainWindow : public Window
{
	Rect bottom_bar;
	RoundButton shutter;
	Image gallery_icon;

public:
	MainWindow() : Window("Camera", Size(320), Size(480 - TOPBAR_HEIGHT))
	{
		printf("succesfully loaded camera\n");
		bg_color = Color(0, 0, 0);
		bottom_bar.location = Point(0, height.get() - 50);
		bottom_bar.width = Size(320);
		bottom_bar.height = Size(50);
		bottom_bar.bg_color = Color(32, 32, 32);

		gallery_icon.location = Point(20, height.get() - 43);
		gallery_icon.width = Size(35);
		gallery_icon.height = Size(35);
		gallery_icon.source = "/themes/default_dark/icons/gallery.png";
		gallery_icon.on_click = [](event_t *, Control *)
		{
			exec("/apps/photo/gallery/gallery.d3m0n", nullptr);
		};

		shutter.location = Point((width.get() - 35) / 2, height.get() - 43);
		shutter.width = Size(35);
		shutter.height = Size(35);
		shutter.radius = 18;
		shutter.bg_color = Color(128, 128, 128);
		shutter.on_click = [](event_t *, Control *) { printf("take a photo\n"); };

		add_control(&bottom_bar);
		add_control(&gallery_icon);
		add_control(&shutter);
	}
};
