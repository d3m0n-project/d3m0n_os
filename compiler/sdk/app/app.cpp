#include "app.hpp"
#include "stdio.hpp"
#include "app/graphics.hpp"

Display		display;

Window::Window(const char *title, const Size &width, const Size &height)
{
	printf("creating window!\n");
	this->title = string(title);
	this->bg_color = Color(255, 255, 255);
	this->width = width;
	this->height = height;
	this->top_bar = true;
	this->controls = nullptr;
	printf("window created!\n");
}

Window::~Window()
{
	// nothing to do
}

void	Window::draw(void)
{
	Control	*current = this->controls;
	while (current)
	{
		current->draw(display);
		current = current->next;
	}
}






Control::Control(void)
{
	this->margin_top = Size(0);
	this->margin_left = Size(0);
	this->margin_right = Size(0);
	this->margin_bottom = Size(0);
	this->width = Size(50);
	this->height = Size(30);
	this->name = "Unknown Control";
	this->visible = true;
	this->enabled = true;
	this->location = Point(0, 0);
	this->color = Color(255, 255, 255);
	this->bg_color = Color(0, 0, 0);
	this->controls = nullptr;
	this->next = nullptr;
}

void	Control::draw(Display &display)
{
	(void)display;
	printf("draw() function is not yet defined.\n");
	return;
}