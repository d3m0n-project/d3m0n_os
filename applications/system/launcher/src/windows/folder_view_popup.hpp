#pragma once

#include "app.hpp"

#define POPUP_WIDTH 280
#define POPUP_HEIGHT 360
#define POPUP_MAX_APPS 18
#define POPUP_ROW_HEIGHT 16

class FolderViewPopup : public Window
{
private:
	Rect	background;
	Text	heading;
	Text	app_rows[POPUP_MAX_APPS];
	int		app_count;

	static bool append_path(char *dst, size_t capacity, const char *part)
	{
		size_t length = strlen(dst);
		for (size_t i = 0; part[i]; ++i)
		{
			if (length + 1 >= capacity)
				return false;
			dst[length++] = part[i];
		}
		dst[length] = '\0';
		return true;
	}

	void add_app(const char *category, const char *app_name)
	{
		char path[256] = "/apps/";
		if (!append_path(path, sizeof(path), category) || !append_path(path, sizeof(path), "/") ||
			!append_path(path, sizeof(path), app_name) || !append_path(path, sizeof(path), "/") ||
			!append_path(path, sizeof(path), app_name) || !append_path(path, sizeof(path), ".d3m0n"))
			return;

		int fd = open(path, O_READ);
		if (fd < 0)
			return;
		close(fd);

		Text *row = &app_rows[app_count];
		row->name = path;
		row->content = app_name;
		row->visible = true;
		row->location = Point(12, 36 + app_count * POPUP_ROW_HEIGHT);
		row->width = Size(POPUP_WIDTH - 24);
		row->height = Size(POPUP_ROW_HEIGHT);
		row->font_size = 8;
		row->color = Color("#ffffff");
		row->on_click = [this](event_t *, Control *target)
		{
			this->visible = false;
			exec(target->name.c_str(), nullptr);
		};
		app_count++;
	}

public:
	bool visible;

	FolderViewPopup(void) : Window("folder_view", Size(320), Size(480 - 32)), app_count(0), visible(false)
	{
		background.location = Point((this->width.get() - POPUP_WIDTH) / 2, (this->height.get() - POPUP_HEIGHT) / 2);
		background.width = Size(POPUP_WIDTH);
		background.height = Size(POPUP_HEIGHT);
		background.bg_color = Color(32, 32, 32, 255);
		background.name = "popup_background";

		heading.location = Point(12, 10);
		heading.width = Size(POPUP_WIDTH - 24);
		heading.height = Size(20);
		heading.font_size = 10;
		heading.color = Color("#ffffff");
		background.add_control(&heading);
		for (int i = 0; i < POPUP_MAX_APPS; ++i)
		{
			app_rows[i].visible = false;
			background.add_control(&app_rows[i]);
		}
		add_control(&background);
	}

	bool contains(int x, int y)
	{
		background.layout(0, 0, width.get(), height.get());
		return x >= background.computed_location.x &&
			x < background.computed_location.x + background.computed_width &&
			y >= background.computed_location.y &&
			y < background.computed_location.y + background.computed_height;
	}

	void prepare_popup_content(const char *category)
	{
		for (int i = 0; i < app_count; ++i)
			app_rows[i].visible = false;

		app_count = 0;
		heading.content = category;
		if (!category || !category[0])
			return;

		char category_path[160] = "/apps/";
		if (!append_path(category_path, sizeof(category_path), category))
			return;

		uint32_t entries_buffer[4096 / sizeof(uint32_t)];
		int count = getdents(category_path, (char **)entries_buffer, sizeof(entries_buffer));
		if (count < 0)
			return;

		char **entries = (char **)entries_buffer;
		for (int i = 0; i < count && app_count < POPUP_MAX_APPS; ++i)
		{
			if (!strcmp(entries[i], ".") || !strcmp(entries[i], ".."))
				continue;
			add_app(category, entries[i]);
		}
	}

	void draw(void) override
	{
		if (!visible)
			return;
		draw_controls();
	}
};
