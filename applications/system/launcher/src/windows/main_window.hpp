#pragma once

#include "app.hpp"
#include "runtime/vector.hpp"
#include "folder_view_popup.hpp"

// must be vertical (horizontal mode not handled yet)
#define ICONS_PER_ROW		4
#define ICONS_PER_COL		5

#define ICONS_PADDING		20

#define ICON_TEXT_SIZE		7
#define ICON_WIDTH			((this->width.get() / ICONS_PER_ROW) - ICONS_PADDING) - (ICONS_PADDING / ICONS_PER_ROW)
#define ICON_HEIGHT			((this->height.get() / ICONS_PER_COL) - ICONS_PADDING) - (ICONS_PADDING / ICONS_PER_COL)
#define ICON_IMAGE_SIZE		(ICON_WIDTH)

typedef struct
{
	Rect	touch_area;
	Image	icon;
	Text	name;
}	t_app_controls;

class MainWindow : public Window
{
private:
	t_app_controls		app_slots[ICONS_PER_ROW * ICONS_PER_COL];
	int					settings_fd;
	Image				wallpaper;
	FolderViewPopup		popup;
public:
	void	add_icon(int row, int col, const char *app_name)
	{
		Rect	*touch_area = &this->app_slots[col * ICONS_PER_ROW + row].touch_area;
		Image	*tmp_image = &this->app_slots[col * ICONS_PER_ROW + row].icon;
		Text	*tmp_text = &this->app_slots[col * ICONS_PER_ROW + row].name;

		touch_area->width = Size(ICON_WIDTH);
		touch_area->height = Size(ICON_IMAGE_SIZE + ICON_TEXT_SIZE * 2);
		touch_area->location = Point(
			ICONS_PADDING + row * (ICON_WIDTH + ICONS_PADDING),
			ICONS_PADDING + col * (ICON_HEIGHT + ICONS_PADDING)
		);
		touch_area->on_click = [this](event_t *event, Control *target) { this->app_icon_on_click(event, target); };
		touch_area->name = app_name;

		tmp_image->width = Size(ICON_IMAGE_SIZE);
		tmp_image->height = Size(ICON_IMAGE_SIZE);
		tmp_image->source = "/themes/default_dark/icons/modules.png";

		tmp_text->width = Size(ICON_IMAGE_SIZE);
		tmp_text->location = Point(0, ICON_IMAGE_SIZE);
		tmp_text->font_size = ICON_TEXT_SIZE;
		tmp_text->text_align = ANCHOR_CENTER_X;
		tmp_text->content = app_name;
		touch_area->add_control(tmp_image);
		touch_area->add_control(tmp_text);

		this->add_control(touch_area);
	}

	MainWindow(void): Window("launcher", Size(320), Size(480 - 32))
	{
		this->settings_fd = open("/desktop.config", O_CREATE | O_WRITE | O_READ);
		if (!settings_fd)
			fprintf(stderr, "Could not open desktop config\n");
		
		wallpaper.width = this->width;
		wallpaper.height = this->height;
		wallpaper.source = "/wallpapers/default.svg";
		this->add_control(&wallpaper);


		// get app categories list
		char path[] = "/apps";
		uint32_t buf[4096 / sizeof(uint32_t)];
		int count = getdents(path, (char **)buf, sizeof(buf));
		if (count < 0)
		{
			printf("ls: cannot list /apps\n");
			return;
		}

		char **entries = (char **)buf;
		int col = 0;
		int row = 0;
		for (int i = 0; i < count; i++)
		{
			if (!strcmp(entries[i], ".") || !strcmp(entries[i], ".."))
				continue;
			if (col >= ICONS_PER_COL)
				break;
			
			add_icon(row, col, entries[i]);
			
			row++;
			if (row >= ICONS_PER_ROW)
			{
				row = 0;
				col++;
			}
		}
	};

	void	app_icon_on_click(event_t *e, Control *target)
	{
		(void)e;
		this->popup.prepare_popup_content(target->name.c_str());
		this->popup.visible = true;
	}

	void	draw(void) override
	{
		draw_controls();
		popup.draw();
		present();
	}

	bool	handle_event(event_t *event)
	{
		if (!event || !EVENT_IS_MOUSE(event->type))
			return false;
		
		if (popup.visible)
		{
			if (!popup.contains(event->x, event->y))
			{
				popup.visible = false;
				return true;
			}
			popup.dispatch_event(event);
			return true;
		}
		return this->dispatch_event(event);
	}
};
