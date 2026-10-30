#pragma once

#include "app.hpp"
#include "../app_icon.hpp"

#define POPUP_WIDTH       280
#define POPUP_HEIGHT      300
#define POPUP_MAX_APPS    9

#define ICON_SIZE         48
#define GRID_COLUMNS      3
#define GRID_CELL_WIDTH   80
#define GRID_CELL_HEIGHT  76

class FolderViewPopup : public Window
{
private:
    Rect background;
    Text heading;

    t_app_icon app_list[POPUP_MAX_APPS];
    int app_count;

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

    void add_app(int index, const char *category, const char *app_name)
    {
        char path[256] = "/apps/";

        if (!append_path(path, sizeof(path), category) ||
            !append_path(path, sizeof(path), "/") ||
            !append_path(path, sizeof(path), app_name) ||
            !append_path(path, sizeof(path), "/") ||
            !append_path(path, sizeof(path), app_name) ||
            !append_path(path, sizeof(path), ".d3m0n"))
            return;

        int fd = open(path, O_READ);

        if (fd < 0)
            return;

        close(fd);

        t_app_icon *app = &app_list[app_count];

        app->touch_area.name = path;

        int column = index % GRID_COLUMNS;
        int row = index / GRID_COLUMNS;

        int x = 20 + column * GRID_CELL_WIDTH;
        int y = 55 + row * GRID_CELL_HEIGHT;

        // full clickable area
        app->touch_area.location = Point(x, y);
        app->touch_area.width = Size(ICON_SIZE);
        app->touch_area.height = Size(ICON_SIZE + 22);
        app->touch_area.visible = true;

        // icon
        app->icon.location = Point(0, 0);
        app->icon.width = Size(ICON_SIZE);
        app->icon.height = Size(ICON_SIZE);
        app->icon.source = "/themes/default_dark/icons/modules.png";

        // application name
        app->name.location = Point(0, ICON_SIZE + 2);
        app->name.width = Size(ICON_SIZE);
        app->name.height = Size(18);
        app->name.font_size = 7;
        app->name.text_align = ANCHOR_CENTER_X;
        app->name.content = app_name;

        app->touch_area.on_click = [this](event_t *, Control *target)
        {
            this->visible = false;

            uint32_t pid = exec(target->name.c_str(), nullptr);

            // wait for proc exit
            while (pstate(pid) != PROC_ZOMBIE)
                __asm__ volatile("wfi");

            this->visible = true;
        };

        app_count++;
    }

public:
    bool visible;

    FolderViewPopup()
        : Window("folder_view", Size(320), Size(480 - 32)),
          app_count(0),
          visible(false)
    {
        // Popup background.
        background.location = Point(
            (this->width.get() - POPUP_WIDTH) / 2,
            (this->height.get() - POPUP_HEIGHT) / 2
        );

        background.width = Size(POPUP_WIDTH);
        background.height = Size(POPUP_HEIGHT);

        background.bg_color = Color(32, 32, 32, 245);
        background.name = "folder_popup";

        // Folder heading.
        heading.location = Point(0, 15);
        heading.width = Size(POPUP_WIDTH);
        heading.height = Size(24);
        heading.font_size = 11;
        heading.color = Color("#ffffff");
        heading.text_align = ANCHOR_CENTER_X;

        background.add_control(&heading);

        // Initialize app slots.
        for (int i = 0; i < POPUP_MAX_APPS; ++i)
        {
            app_list[i].touch_area.visible = false;

            app_list[i].touch_area.add_control(&app_list[i].icon);
            app_list[i].touch_area.add_control(&app_list[i].name);

            background.add_control(&app_list[i].touch_area);
        }

        add_control(&background);
    }

    bool contains(int x, int y)
    {
        background.layout(0, 0, width.get(), height.get());

        return
            x >= background.computed_location.x &&
            x < background.computed_location.x + background.computed_width &&
            y >= background.computed_location.y &&
            y < background.computed_location.y + background.computed_height;
    }

    void prepare_popup_content(const char *category)
    {
        // Hide previously loaded applications.
        for (int i = 0; i < app_count; ++i)
            app_list[i].touch_area.visible = false;

        app_count = 0;

        heading.content = category ? category : "";

        if (!category || !category[0])
            return;

        char category_path[160] = "/apps/";

        if (!append_path(category_path, sizeof(category_path), category))
            return;

        uint32_t entries_buffer[4096 / sizeof(uint32_t)];

        int count = getdents(
            category_path,
            (char **)entries_buffer,
            sizeof(entries_buffer)
        );

        if (count < 0)
            return;

        char **entries = (char **)entries_buffer;

        for (int i = 0; i < count && app_count < POPUP_MAX_APPS; ++i)
        {
            if (!strcmp(entries[i], ".") ||
                !strcmp(entries[i], ".."))
                continue;

            // Use app_count for positioning so failed applications
            // don't leave empty spaces in the grid.
            add_app(app_count, category, entries[i]);
        }
    }

    void draw() override
    {
        if (!visible)
            return;

        draw_controls();
    }
};
