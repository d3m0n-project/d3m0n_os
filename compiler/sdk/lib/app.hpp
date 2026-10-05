#ifndef APP_HPP
#define APP_HPP

#include "types.h"
#include "string.hpp"
#include "app/color.hpp"
#include "app/point.hpp"
#include "app/size.hpp"
#include "app/graphics.hpp"


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
/** @brief Represents the Control type. */
class Control
{
public:
	Control(void);
	virtual ~Control(void) {}
	/** @brief add_control operation. */
	void	add_control(Control *control);
	/** @brief layout operation. */
	void	layout(int parent_x, int parent_y, int parent_width, int parent_height);

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

	/** @brief draw operation. */
	virtual void		draw(Display *drawing_function)
	{
		(void)drawing_function;
	}
};

/** @brief Represents the Window type. */
class Window
{
public:
	string				title;
	Size				width;
	Size				height;
	Color				bg_color;
	bool				top_bar;
	Control				*controls;

	Window(const char *title, const Size &width, const Size &height);
	~Window();
	/** @brief add_control operation. */
	void	add_control(Control *control);

	/** @brief draw operation. */
	void	draw(void);
};
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

#endif
