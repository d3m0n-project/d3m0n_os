#ifndef APP_HPP
#define APP_HPP

#include "types.h"
#include "string.hpp"
#include "app/color.hpp"
#include "app/point.hpp"
#include "app/size.hpp"
#include "app/graphics.hpp"
#include "app/events.h"
#include "functional.hpp"
#include "runtime/runtime.h"


#ifdef __cplusplus
extern "C" {
#endif
	#include "app/app_manifest.h"

	#define APP_METADATA	__attribute__((section(".appmeta"), used))
	#define APP_INFO(NAME, VERSION, AUTHOR, DESCRIPTION, ICON) \
		asm( \
			".section .appicon, \"a\", %progbits\n" \
			".align 4\n" \
			".global __appicon_start\n" \
			".global __appicon_end\n" \
			"__appicon_start:\n" \
			".incbin \"" ICON "\"\n" \
			"__appicon_end:\n" \
		); \
		extern const unsigned char			__appicon_start[]; \
		extern const unsigned char			__appicon_size[]; \
		const AppMetadata app_metadata	APP_METADATA = { \
			APP_MANIFEST_MAGIC, \
			VERSION, \
			NAME, \
			AUTHOR, \
			DESCRIPTION, \
			(uint32_t)(uintptr_t)__appicon_size, \
			__appicon_start, \
			APP_MANIFEST_MAGIC \
		};
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/** @brief Base class for controls that can be laid out and drawn in a window. */
class Control
{
public:
	Control(void);
	virtual			~Control(void) {}
	/** @brief Adds a control to this control’s child list. */
	void			add_control(Control *control);
	/** @brief Computes this control’s position and size relative to its parent. */
	virtual void	layout(int parent_x, int parent_y, int parent_width, int parent_height);

	Size				margin_top;
	Size				margin_left;
	Size				margin_right;
	Size				margin_bottom;
	Size				width;
	Size				height;
	string				name;
	bool				visible;
	bool				enabled;
	Point				location;
	Point				computed_location;
	int					computed_width;
	int					computed_height;
	Color				color;
	Color				bg_color;

	Control				*controls;
	Control				*next;
	Control				*parent;

	// events
	std::function<void(event_t *, Control *)>	on_click;
	std::function<void(event_t *, Control *)>	on_keypress;
	std::function<void(event_t *, Control *)>	on_keyrelease;

	/** @brief Draws the window’s visible controls. */
	virtual void		draw(Display *drawing_function)
	{
		(void)drawing_function;
	}
};

/** @brief Top-level container that owns controls and renders an application window. */
class Window
{
public:
	string				title;
	Size				width;
	Size				height;
	Color				bg_color;
	bool				top_bar;
	Control				*controls;
	Control				*focused_control;

	std::function<void(event_t *, Window *)>	on_keypress;
	std::function<void(event_t *, Window *)>	on_keyrelease;
	std::function<void(event_t *, Window *)>	on_event;
	std::function<void(Window *)>	on_idle;

	Window(const char *title, const Size &width, const Size &height);
	~Window();
	/** @brief Adds a control to this control’s child list. */
	void			add_control(Control *control);

	/** @brief Draws the window’s visible controls. */
	virtual void	draw(void);
	void			draw_controls(void);
	void			present(void);

	/** @brief Dispatch one queued event; call draw() once first to initialize hit-test bounds. */
	bool			poll(void);
	bool			dispatch_event(event_t *event);
	void			show_window(Window &child);
	void			return_to_parent(void);
};

class Application
{
private:
	static event_t				pending_event;
	static volatile uint32_t	event_pending;
	static thread_t				events;

	static void	event_thread(void)
	{
		while (1)
		{
			if (event_pending)
				continue;
			event_t event = {.type=EVENT_UNKNOWN, .timestamp=0, .x=0, .y=0, .key=0, .modifiers=0};
			if (evtpop(&event))
			{
				pending_event = event;
				__asm__ volatile("" ::: "memory");
				event_pending = 1;
			}
		}
	}

public:
	Application() {}
	~Application() {}
	static Window *&active_window(void)
	{
		static Window *window = 0;
		return window;
	}
	static Window **parent_stack(void)
	{
		static Window *stack[8] = {};
		return stack;
	}
	static int &parent_depth(void)
	{
		static int depth = 0;
		return depth;
	}

	static void	run(Window &main_window)
	{
		active_window() = &main_window;
		main_window.draw(); 
		events = thread_create(event_thread);
		if (!events)
		{
			fprintf(stderr, "Could not start event thread\n");
			while (1)
			{
				Window *window = active_window();
				if (!window) window = &main_window;
				if (!window->on_idle.empty())
					window->on_idle(window);
				event_t event = {.type=EVENT_UNKNOWN, .timestamp=0, .x=0, .y=0, .key=0, .modifiers=0};
				if (evtpop(&event) && window->dispatch_event(&event))
				{
					Window *current = active_window();
					if (current) current->draw();
				}
			}
		}
		while (1)
		{
			Window *window = active_window();
			if (!window) window = &main_window;
			if (!window->on_idle.empty())
				window->on_idle(window);
			if (event_pending)
			{
				__asm__ volatile("" ::: "memory");
				event_t event = pending_event;
				event_pending = 0;
				__asm__ volatile("" ::: "memory");
				if (window->dispatch_event(&event))
				{
					Window *current = active_window();
					if (current) current->draw();
				}
			}
		}
	}
};

inline void Window::show_window(Window &child)
{
	int &depth = Application::parent_depth();
	if (depth >= 8)
		return;
	Application::parent_stack()[depth++] = this;
	Application::active_window() = &child;
	child.draw();
}

inline void Window::return_to_parent(void)
{
	int &depth = Application::parent_depth();
	if (depth > 0)
	{
		Window *parent = Application::parent_stack()[--depth];
		Application::active_window() = parent;
		parent->draw();
	}
}
#endif



// controls
#include "app/controls/TextBox.hpp"
#include "app/controls/Rect.hpp"
#include "app/controls/RoundButton.hpp"
#include "app/controls/Hscroll.hpp"
#include "app/controls/ListView.hpp"
#include "app/controls/ProgressBar.hpp"
#include "app/controls/Switch.hpp"
#include "app/controls/Vscroll.hpp"
#include "app/controls/Text.hpp"
#include "app/controls/RadioButton.hpp"
#include "app/controls/Image.hpp"
#include "app/controls/CheckBox.hpp"
#include "app/controls/Button.hpp"
#include "app/controls/WebView.hpp"
#include "app/controls/InlineSvgIcon.hpp"

#endif
