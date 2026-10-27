#pragma once

#include "app.hpp"
#include "runtime/vector.hpp"

// must be vertical (horizontal mode not handled yet)
#define ICONS_PER_ROW		4
#define ICONS_PER_COL		5

#define ICONS_PADDING		20

#define ICON_WIDTH			((this->width.get() / ICONS_PER_ROW) - ICONS_PADDING) - (ICONS_PADDING / ICONS_PER_ROW)
#define ICON_HEIGHT			((this->height.get() / ICONS_PER_COL) - ICONS_PADDING) - (ICONS_PADDING / ICONS_PER_COL)
#define ICON_IMAGE_SIZE		(ICON_WIDTH)

typedef struct
{
	Image	icon;
	Text	name;
}	t_app_controls;

class MainWindow : public Window
{
private:
	t_app_controls	app_slots[ICONS_PER_ROW * ICONS_PER_COL];
	int				settings_fd;
	Image			wallpaper;
public:
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
						
			Image	*tmp_image = &this->app_slots[col * ICONS_PER_ROW + row].icon;
			Text	*tmp_text = &this->app_slots[col * ICONS_PER_ROW + row].name;

			tmp_image->width = Size(ICON_IMAGE_SIZE);
			tmp_image->height = Size(ICON_IMAGE_SIZE);
			tmp_image->source = "/themes/default_dark/icons/modules.png";
			tmp_image->location = Point(
				ICONS_PADDING + row * (ICON_WIDTH + ICONS_PADDING),
				ICONS_PADDING + col * (ICON_HEIGHT + ICONS_PADDING)
			);
			tmp_text->width = Size(ICON_IMAGE_SIZE);
			tmp_text->location = Point(tmp_image->location.x, tmp_image->location.y + ICON_IMAGE_SIZE);
			//tmp_text->text_align = 0;
			tmp_text->content = entries[i];
			tmp_text->color = Color("#ffffff");
			tmp_text->bg_color = Color(0, 0, 0, 0);

			this->add_control(tmp_image);
			this->add_control(tmp_text);
			row++;
			if (row >= ICONS_PER_ROW)
			{
				row = 0;
				col++;
			}
		}


		
	};
};
