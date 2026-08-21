#ifndef APP_HPP
#define APP_HPP

#include "types.h"
#include "string.hpp"
#include "app/color.hpp"
#include "app/point.hpp"
#include "app/size.hpp"


extern "C" {
	typedef struct AppMetadata
	{
		uint32_t		magic;
		uint32_t		version;

		const char		*name;
		const char		*author;
		const char		*description;

		uint32_t		icon_size;
		const uint8_t	*icon;
		uint32_t		end_magic;
	}	AppMetadata;

	#define APP_METADATA	__attribute__((section(".appmeta"), used))
	#define APP_INFO(NAME, AUTHOR, DESCRIPTION, ICON, ICON_SIZE) \
		extern "C" const AppMetadata app_metadata APP_METADATA = { \
			0x35455234, \
			1, \
			NAME, \
			AUTHOR, \
			DESCRIPTION, \
			ICON_SIZE, \
			ICON, \
			0x35455234 \
		};
}


class Control
{
public:
	Control(void);
	~Control(void);

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
	Color				color;
	Color				bg_color;

	Control				*controls;
	Control				*next;

	void				draw(void);
};

class Window
{
	string				title;
	Size				width;
	Size				height;
	Color				bg_color;
	bool				top_bar;
	Control				*controls;
public:
	Window(string title, Size &width, Size &height);
	~Window();

	void	draw(void);
};



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

#endif