#ifndef TEXT_HPP
#define TEXT_HPP

#include "helpers.hpp"

extern Display *display;

/** @brief Control that draws text within its bounds. */
class Text : public Control
{
public:
	string	content;
	int		font_size;
	int		text_align;

	Text() : content(), font_size(0), text_align(0) {}

	/** @brief Computes an intrinsic size from the text when dimensions are unset. */
	void	layout(int parent_x, int parent_y, int parent_width, int parent_height) override
	{
		Control::layout(parent_x, parent_y, parent_width, parent_height);
		int available_width = width.get(parent_width);
		int available_height = height.get(parent_height);
		if (available_width && available_height)
			return;
		int content_width = available_width - margin_left.get() - margin_right.get();
		int content_height = available_height - margin_top.get() - margin_bottom.get();
		int size = font_size;
		if (size <= 0)
			size = available_width && available_height ? control_font_size(0, content_width, content_height, content.c_str()) : 8;
		if (size <= 0)
			size = 8;
		int lines = 1;
		for (int i = 0; content[i]; ++i)
			if (content[i] == '\n')
				++lines;
		if (!available_width)
			computed_width = display->text_width(size * 2, content.c_str()) + margin_left.get() + margin_right.get();
		if (!available_height)
			computed_height = size * 2 * lines + margin_top.get() + margin_bottom.get();
	}

	/** @brief Draws this control and, where applicable, its child controls. */
	void	draw(Display *display) override
	{
		int x, y, w, h;
		control_inner(*this, x, y, w, h);
		if ((uint32_t)bg_color)
			display->draw_rect(x, y, w, h, bg_color);
		control_text(display, *this, this->content.c_str(), this->text_align, this->font_size);
	}
};
#endif
