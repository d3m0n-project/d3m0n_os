#pragma once

#include "app.hpp"
#include "sys.h"

#ifndef TOPBAR_HEIGHT
#define TOPBAR_HEIGHT 32
#endif

namespace SettingsLayout
{
	inline void place(Control &control, int x, int y, int width, int height)
	{
		control.location = Point(x, y);
		control.width = Size(width);
		control.height = Size(height);
	}

	inline void style_text(Text &text, const char *content, int size = 12)
	{
		text.content = content;
		text.color = Color(255, 255, 255);
		text.font_size = size;
	}
}

class SettingsWindow : public Window
{
public:
	SettingsWindow(const char *title) : Window(title, Size(320), Size(480 - TOPBAR_HEIGHT)) {}
	void draw(void) override
	{
		display->draw_rect(0, 0, width.get(), height.get(), bg_color);
		draw_controls();
		present();
	}
};
