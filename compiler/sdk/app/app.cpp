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
	this->focused_control = nullptr;
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
			//uint64_t t = time(0);
			current->draw(display);
			//printf("   drawn %s in %llums\n", current->name.c_str(), (time(0) - t) / 1000);
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
	event_t	evt = {.type=EVENT_UNKNOWN, .timestamp=0, .x=0, .y=0, .key=0, .modifiers=0};
	int ret = evtpop(&evt);
	return ret && dispatch_event(&evt);
}

static Control	*find_key_control(Control *control, bool keypress)
{
	for (Control *current = control; current; current = current->next)
	{
		if (!current->visible || !current->enabled)
			continue;
		Control *child = find_key_control(current->controls, keypress);
		if (child)
			return child;
		if (keypress)
		{
			if (!current->on_keypress.empty())
				return current;
		}
		else if (!current->on_keyrelease.empty())
			return current;
	}
	return nullptr;
}

bool Window::dispatch_event(event_t *event)
{
	if (event && !EVENT_IS_KEYBOARD(event->type) && !EVENT_IS_MOUSE(event->type) && !on_event.empty())
	{
		on_event(event, this);
		if (event->type == EVENT_TERMINAL_OUTPUT)
		{
			static uint32_t pending_output_bytes = 0;
			uint32_t len = event->modifiers;
			if (len > sizeof(uint64_t) + 3 * sizeof(int32_t))
				len = sizeof(uint64_t) + 3 * sizeof(int32_t);
			const char *output = (const char *)&event->timestamp;
			bool flush = false;
			for (uint32_t i = 0; i < len; ++i)
				if (output[i] == '\n' || output[i] == '\r' || output[i] == ' ')
					flush = true;
			pending_output_bytes += len;
			if (pending_output_bytes >= 80)
				flush = true;
			if (flush)
				pending_output_bytes = 0;
			return flush;
		}
		return true;
	}
	if (event && EVENT_IS_KEYBOARD(event->type))
	{
		if (event->type == EVENT_KEYPRESS && !this->on_keypress.empty())
		{
			this->on_keypress(event, this);
			return true;
		}
		if (event->type == EVENT_KEYRELEASE && !this->on_keyrelease.empty())
		{
			this->on_keyrelease(event, this);
			return true;
		}

		Control *target = this->focused_control;
		if (target && target->visible && target->enabled)
		{
			if (event->type == EVENT_KEYPRESS && !target->on_keypress.empty())
			{
				target->on_keypress(event, target);
				return true;
			}
			if (event->type == EVENT_KEYRELEASE && !target->on_keyrelease.empty())
			{
				target->on_keyrelease(event, target);
				return true;
			}
		}

		target = find_key_control(this->controls, event->type == EVENT_KEYPRESS);
		if (target)
		{
			if (event->type == EVENT_KEYPRESS)
				target->on_keypress(event, target);
			else
				target->on_keyrelease(event, target);
			return true;
		}
		return false;
	}

	if (event && EVENT_IS_MOUSE(event->type))
	{
		int window_width = this->width.get();
		int window_height = this->height.get();
		for (Control *control = this->controls; control; control = control->next)
			control->layout(0, 0, window_width, window_height);
		
		Control *clicked = find_clicked_control(this->controls, event->x, event->y);
		if (clicked)
		{
			this->focused_control = clicked;
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

// define application values
event_t Application::pending_event = {
	.type = EVENT_UNKNOWN,
	.timestamp = 0,
	.x = 0,
	.y = 0,
	.key = 0,
	.modifiers = 0
};
volatile uint32_t Application::event_pending = 0;
thread_t Application::events = 0;
