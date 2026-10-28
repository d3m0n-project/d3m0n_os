#pragma once

#include "app.hpp"
#include "runtime/vector.hpp"

#define POPUP_WIDTH		200
#define POPUP_HEIGHT	200

class FolderViewPopup : public Window
{
private:
	Rect	bg;
	Image	test;
public:
	FolderViewPopup(void): Window("folder_view", Size(320), Size(480 - 32))
	{
		this->bg_color = Color(0, 0, 0, 0);

		this->bg.location = Point(this->width.get() / 2 - POPUP_WIDTH / 2, this->height.get() / 2 - POPUP_HEIGHT / 2);
		this->bg.width = Size(POPUP_WIDTH);
		this->bg.height = Size(POPUP_HEIGHT);
		this->bg.bg_color = Color(255, 0, 0);

		this->add_control(&bg);
	};
};
