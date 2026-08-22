#ifndef APP_HPP
#define APP_HPP

#include "types.h"
#include "string.hpp"
#include "app/color.hpp"
#include "app/point.hpp"
#include "app/size.hpp"


extern "C" {
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