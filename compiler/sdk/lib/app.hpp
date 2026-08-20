#ifndef APP_HPP
#define APP_HPP

#include "types.h"
#include "string.hpp"
#include "app/color.hpp"

class Control
{
public:
	int					margin_top;
	int					margin_left;
	int					margin_right;
	int					margin_bottom;
	int					width;
	int					height;
	string				name;
	uint8_t				visible;
	uint8_t				enabled;
	t_point				location;
	Color				color;
	Color				bg_color;

	Control				*children;
};

class App
{
private:

public:
	App();
	~App();
};

#endif