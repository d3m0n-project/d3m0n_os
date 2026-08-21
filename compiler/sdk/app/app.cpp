#include "app.hpp"
#include "stdio.hpp"

Window::Window(string title, Size &width, Size &height)
{
	this->title = title;
	this->bg_color = Color(255, 255, 255);
	this->width = width;
	this->height = height;
	this->top_bar = true;
	this->controls = nullptr;
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
		current->draw();
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

void	Control::draw(void)
{
	printf("draw() function is not yet defined.\n");
	return;
}