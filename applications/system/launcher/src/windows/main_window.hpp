#pragma once

#include "app.hpp"
#include "runtime/vector.hpp"
#include "../app_icon.hpp"

#define ICONS_PER_ROW		4
#define ICONS_PADDING		12

#define ICON_TEXT_SIZE		7
#define ICON_WIDTH			60
#define ICON_HEIGHT			76
#define ICON_IMAGE_SIZE		48

#define MAX_DESKTOP_APPS	128

class MainWindow : public Window
{
private:
    t_app_icon	app_slots[MAX_DESKTOP_APPS];
    int			app_count;
    int			settings_fd;
    Image		wallpaper;

    static bool	append_path(char *dst, size_t capacity, const char *part)
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

    void add_icon(const char *category, const char *app_name)
    {
        if (app_count >= MAX_DESKTOP_APPS)
            return;

        char path[256] = "/apps/";

        if (!append_path(path, sizeof(path), category) ||
            !append_path(path, sizeof(path), "/") ||
            !append_path(path, sizeof(path), app_name) ||
            !append_path(path, sizeof(path), "/") ||
            !append_path(path, sizeof(path), app_name) ||
            !append_path(path, sizeof(path), ".d3m0n"))
            return;

		// do not display self app in app list
		if (!strcasecmp(path, this->self_path))
			return;

        // verify that the executable exists
        int fd = open(path, O_READ);
        if (fd < 0)
            return;

        close(fd);

        int index = app_count;
        int col = index % ICONS_PER_ROW;
        int row = index / ICONS_PER_ROW;

        t_app_icon *app = &app_slots[index];
        Rect *touch_area = &app->touch_area;
        Image *icon = &app->icon;
        Text *text = &app->name;

        // touch area
        touch_area->name = path;

        touch_area->width = Size(ICON_WIDTH);
        touch_area->height = Size(ICON_HEIGHT);
        touch_area->location = Point(
            ICONS_PADDING + col * (ICON_WIDTH + ICONS_PADDING),
            ICONS_PADDING + row * (ICON_HEIGHT + ICONS_PADDING)
        );
        touch_area->on_click = [this](event_t *event, Control *target)
        {
            this->app_icon_on_click(event, target);
        };

        // application icon
        icon->location = Point(
            (ICON_WIDTH - ICON_IMAGE_SIZE) / 2,
            0
        );

        icon->width = Size(ICON_IMAGE_SIZE);
        icon->height = Size(ICON_IMAGE_SIZE);

        icon->source = "/themes/default_dark/icons/modules.png";

        // app name
        text->location = Point(0, ICON_IMAGE_SIZE + 3);
        text->width = Size(ICON_WIDTH);
        text->height = Size(ICON_TEXT_SIZE * 2);

        text->font_size = ICON_TEXT_SIZE;
        text->text_align = ANCHOR_CENTER_X;
        text->content = app_name;

        touch_area->add_control(icon);
        touch_area->add_control(text);

        // add icon directly to the desktop
        this->add_control(touch_area);

        app_count++;
    }

    void	load_category(const char *category)
    {
        char category_path[256] = "/apps/";

        if (!append_path(category_path, sizeof(category_path), category))
            return;

        uint32_t entries_buffer[4096 / sizeof(uint32_t)];

        int count = getdents(category_path, (char **)entries_buffer, sizeof(entries_buffer));
        if (count < 0)
            return;

        char **entries = (char **)entries_buffer;
        for (int i = 0; i < count && app_count < MAX_DESKTOP_APPS; ++i)
        {
            if (!strcmp(entries[i], ".") || !strcmp(entries[i], ".."))
                continue;

            add_icon(category, entries[i]);
        }
    }

    void	load_all_apps()
    {
        char path[] = "/apps";
        uint32_t entries_buffer[4096 / sizeof(uint32_t)];
        int count = getdents(path, (char **)entries_buffer, sizeof(entries_buffer));
        if (count < 0)
        {
            printf("launcher: cannot list applications\n");
            return;
        }

        char **entries = (char **)entries_buffer;
        for (int i = 0; i < count && app_count < MAX_DESKTOP_APPS; ++i)
        {
            if (!strcmp(entries[i], ".") || !strcmp(entries[i], ".."))
                continue;

            load_category(entries[i]);
        }
    }

public:
	char	*self_path;

    MainWindow(char *_self_path) : Window("launcher", Size(320), Size(480 - 32)), app_count(0), settings_fd(-1), self_path(_self_path)
    {
        // desktop config
        settings_fd = open("/desktop.config", O_CREATE | O_WRITE | O_READ);
        if (settings_fd < 0)
            printf("launcher: could not open desktop config\n");

        // wallpaper
        wallpaper.width = this->width;
        wallpaper.height = this->height;
        wallpaper.source = "/wallpapers/default.svg";

        this->add_control(&wallpaper);

        // load all apps
        load_all_apps();
    }

    void app_icon_on_click(event_t *event, Control *target)
    {
        (void)event;
        if (!target)
            return;

        uint32_t pid = exec(target->name.c_str(), nullptr);

        // wait for proc to stop
        while (pstate(pid) != PROC_ZOMBIE)
            __asm__ volatile("wfi");
    }

    void draw() override
    {
        draw_controls();
        present();
    }
};