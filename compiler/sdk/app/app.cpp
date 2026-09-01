#include "app.hpp"
#include "app/graphics.hpp"

Display *display = 0;

Window::Window(const char *title, const Size &width, const Size &height)
{
	if (!display)
		display = new Display();
	this->title = string(title);
	this->bg_color = Color(255, 255, 255);
	this->width = width;
	this->height = height;
	this->top_bar = true;
	this->controls = nullptr;
}

Window::~Window()
{
}

void Window::add_control(Control *control)
{
	if (!control)
		return;
	control->parent = nullptr;
	control->next = nullptr;
	if (!this->controls)
	{
		this->controls = control;
		return;
	}
	Control *current = this->controls;
	while (current->next)
		current = current->next;
	current->next = control;
}

void Window::draw(void)
{
	Control *current = this->controls;
	int window_width = this->width.get();
	int window_height = this->height.get();
	while (current)
	{
		current->layout(0, this->top_bar ? 24 : 0, window_width, window_height - (this->top_bar ? 24 : 0));
		if (current->visible)
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
	this->computed_location = Point(0, 0);
	this->computed_width = 0;
	this->computed_height = 0;
	this->color = Color(255, 255, 255);
	this->bg_color = Color(0, 0, 0);
	this->controls = nullptr;
	this->next = nullptr;
	this->parent = nullptr;
}

void Control::add_control(Control *control)
{
	if (!control || control == this)
		return;
	control->parent = this;
	control->next = nullptr;
	if (!this->controls)
	{
		this->controls = control;
		return;
	}
	Control *current = this->controls;
	while (current->next)
		current = current->next;
	current->next = control;
}

void Control::layout(int parent_x, int parent_y, int parent_width, int parent_height)
{
	int width = this->width.get(parent_width);
	int height = this->height.get(parent_height);
	int x = this->location.x;
	int y = this->location.y;
	if (width < 0) width = 0;
	if (height < 0) height = 0;
	this->computed_location = Point(parent_x + x, parent_y + y);
	this->computed_width = width;
	this->computed_height = height;
	Control *child = this->controls;
	while (child)
	{
		child->layout(this->computed_location.x, this->computed_location.y, width, height);
		child = child->next;
	}
}
