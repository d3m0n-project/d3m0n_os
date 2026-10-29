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

Window::~Window() {}

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
	draw_controls();
	present();
}

void	Window::draw_controls(void)
{
	Control *current = this->controls;
	int window_width = this->width.get();
	int window_height = this->height.get();
	while (current)
	{
		current->layout(0, 0, window_width, window_height);
		if (current->visible)
		{
			uint64_t t = time(0);
			current->draw(display);
			printf("   drawn %s in %llums\n", current->name.c_str(), (time(0) - t) / 1000);
		}
		current = current->next;
	}
}

void Window::present(void) { display->present(); }

static Control	*find_clicked_control(Control *control, int x, int y)
{
	for (Control *current = control; current; current = current->next)
	{
		if (!current->visible || !current->enabled || x < current->computed_location.x || x >= current->computed_location.x + current->computed_width || y < current->computed_location.y || y >= current->computed_location.y + current->computed_height)
			continue;
		Control *child = find_clicked_control(current->controls, x, y);
		if (child)
			return child;
		if (!current->on_click.empty())
			return current;
	}
	return nullptr;
}

bool	Window::poll(void)
{
	event_t	evt = {.type=EVENT_UNKNOWN, .timestamp=0, .x=0, .y=0};
	int ret = evtpop(&evt);
	return ret && dispatch_event(&evt);
}

bool Window::dispatch_event(event_t *event)
{
	if (event && EVENT_IS_MOUSE(event->type))
	{
		int window_width = this->width.get();
		int window_height = this->height.get();
		for (Control *control = this->controls; control; control = control->next)
			control->layout(0, 0, window_width, window_height);
		Control *clicked = find_clicked_control(this->controls, event->x, event->y);
		if (clicked)
		{
			clicked->on_click(event, clicked);
			return true;
		}
	}
	return false;
}

Control::Control(void)
{
	this->margin_top = Size(0);
	this->margin_left = Size(0);
	this->margin_right = Size(0);
	this->margin_bottom = Size(0);
	this->width = Size(0);
	this->height = Size(0);
	this->name = "Unnamed Control";
	this->visible = true;
	this->enabled = true;
	this->location = Point(0, 0);
	this->computed_location = Point(0, 0);
	this->computed_width = 0;
	this->computed_height = 0;
	this->color = Color(255, 255, 255);
	this->bg_color = Color(0, 0, 0, 0);
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

void	Control::layout(int parent_x, int parent_y, int parent_width, int parent_height)
{
	int width = this->width.get(parent_width);
	int height = this->height.get(parent_height);
	if (!width)
		width = 50;
	if (!height)
		height = 30;
	int x = this->location.x;
	int y = this->location.y;
	if (width < 0)
		width = 0;
	if (height < 0)
		height = 0;
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
