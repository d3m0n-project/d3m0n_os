#pragma once

#include "app.hpp"
#include "app/controls/helpers.hpp"

#ifndef TOPBAR_HEIGHT
#define TOPBAR_HEIGHT			32
#endif

#define FILES_VISIBLE_ROWS		9
#define FILES_PATH_CAPACITY		256
#define FILES_ENTRY_CAPACITY	64

class MainWindow : public Window
{
	Text	title, path_text, status;
	Button	up_button;
	Button	rows[FILES_VISIBLE_ROWS];
	char	current_path[FILES_PATH_CAPACITY];
	char	entry_names[FILES_VISIBLE_ROWS][FILES_ENTRY_CAPACITY];

	bool make_entry_path(int index, char *out, size_t capacity)
	{
		out[0] = '\0';
		if (strlcpy(out, current_path, capacity) >= capacity)
			return false;
		if (strcmp(current_path, "/") && strlcat(out, "/", capacity) >= capacity)
			return false;
		if (strlcat(out, entry_names[index], capacity) >= capacity)
			return false;
		return true;
	}

	void refresh_directory()
	{
		path_text.content = current_path;
		status.content = "";
		for (int i = 0; i < FILES_VISIBLE_ROWS; ++i)
			rows[i].visible = false;

		uint32_t entries_buffer[4096 / sizeof(uint32_t)];
		int count = getdents(current_path, (char **)entries_buffer, sizeof(entries_buffer));
		if (count < 0)
		{
			status.content = "Cannot open this directory";
			return;
		}
		char **entries = (char **)entries_buffer;
		int shown = 0;
		for (int i = 0; i < count && shown < FILES_VISIBLE_ROWS; ++i)
		{
			if (!strcmp(entries[i], ".") || !strcmp(entries[i], ".."))
				continue;
			strlcpy(entry_names[shown], entries[i], FILES_ENTRY_CAPACITY);
			rows[shown].content = entry_names[shown];
			rows[shown].visible = true;
			++shown;
		}
		if (shown == 0)
			status.content = "This folder is empty";
	}

	void open_entry(int index)
	{
		char path[FILES_PATH_CAPACITY];
		if (!make_entry_path(index, path, sizeof(path)))
		{
			status.content = "Path is too long";
			return;
		}
		uint32_t entries_buffer[1024 / sizeof(uint32_t)];
		if (getdents(path, (char **)entries_buffer, sizeof(entries_buffer)) >= 0)
		{
			strlcpy(current_path, path, sizeof(current_path));
			refresh_directory();
		}
		else
		{
			status.content = "Selected: ";
			status.content = status.content + entry_names[index];
		}
	}

	void go_up()
	{
		if (!strcmp(current_path, "/"))
			return;
		size_t length = strlen(current_path);
		while (length > 1 && current_path[length - 1] == '/')
			current_path[--length] = '\0';
		while (length > 1 && current_path[length - 1] != '/')
			current_path[--length] = '\0';
		if (length > 1)
			current_path[length - 1] = '\0';
		if (!current_path[0])
			strlcpy(current_path, "/", sizeof(current_path));
		refresh_directory();
	}

public:
	MainWindow() : Window("Files", Size(320), Size(480 - TOPBAR_HEIGHT))
	{
		bg_color = Color(20, 24, 30);
		strlcpy(current_path, "/", sizeof(current_path));
		title.location = Point(0, 4);
		title.width = this->width;
		title.height = Size(26);
		title.content = "Files";
		title.text_align = ANCHOR_CENTER_X | ANCHOR_CENTER_Y;
		title.color = Color(255, 255, 255);
		title.font_size = 15;

		up_button.location = Point(16, 60);
		up_button.width = Size(288);
		up_button.height = Size(32);
		up_button.content = "..";
		up_button.font_size = 9;
		up_button.text_align = ANCHOR_LEFT | ANCHOR_CENTER_Y;
		up_button.color = Color(255, 255, 255);
		up_button.bg_color = Color(55, 65, 78);
		up_button.on_click = [this](event_t *, Control *) { go_up(); };

		path_text.location = Point(16, 35);
		path_text.width = Size(288);
		path_text.height = Size(20);
		path_text.color = Color(170, 190, 210);
		path_text.font_size = 8;

		status.location = Point(16, height.get() - 32);
		status.width = Size(288);
		status.height = Size(28);
		status.color = Color(190, 200, 212);
		status.font_size = 8;
		for (int i = 1; i < FILES_VISIBLE_ROWS; ++i)
		{
			int row_idx = i - 1;
			rows[row_idx].location = Point(16, 60 + i * 36);
			rows[row_idx].width = Size(288);
			rows[row_idx].height = Size(32);
			rows[row_idx].font_size = 9;
			rows[row_idx].text_align = ANCHOR_LEFT | ANCHOR_CENTER_Y;
			rows[row_idx].color = Color(255, 255, 255);
			rows[row_idx].bg_color = Color(39, 46, 56);
			rows[row_idx].visible = false;
			rows[row_idx].on_click = [this, row_idx](event_t *, Control *) { open_entry(row_idx); };
			add_control(&rows[row_idx]);
		}
		add_control(&title);
		add_control(&up_button);
		add_control(&path_text);
		add_control(&status);
		refresh_directory();
	}
};
