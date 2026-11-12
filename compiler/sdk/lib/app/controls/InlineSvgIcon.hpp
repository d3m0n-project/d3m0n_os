#pragma once

#include "app.hpp"
#include "app/parsing/svg.h"

class InlineSvgIcon : public Control
{
	char *data = 0;
	size_t data_size = 0;
public:
	int load(const char *buffer, size_t size)
	{
		if (!buffer || !size || size == (size_t)-1)
			return 1;
		char *copy = (char *)malloc(size + 1);
		if (!copy)
			return 1;
		memcpy(copy, buffer, size);
		copy[size] = '\0';
		if (data)
			free(data);
		data = copy;
		data_size = size;
		return 0;
	}

	void draw(Display *display) override
	{
		uint32_t color = ((this->color.value & 0xff000000) > 0)?this->color.value:0x00000000;
		if (data)
			display->draw_svg_buff(computed_location.x, computed_location.y, computed_width, computed_height, data, data_size, color);
	}

	~InlineSvgIcon() override
	{
		if (data)
			free(data);
	}
};