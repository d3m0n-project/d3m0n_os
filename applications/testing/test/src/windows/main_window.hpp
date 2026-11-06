#pragma once

#include "app.hpp"
#include "stdio.hpp"

class MainWindow : public Window
{
private:
	Text	terminal;
	string	command;
public:
	MainWindow(void): Window("Main Window", Size(320), Size(436)) {
		this->bg_color = Color(0, 0, 0);

		terminal.location = Point(0, 0);
		terminal.color = Color(0xFFFFFFFF);
		terminal.bg_color = Color(0, 0, 0);
		terminal.width = this->width;
		terminal.height = this->height;
		terminal.content = "~ $ ";
		terminal.text_align = ANCHOR_LEFT | ANCHOR_TOP;
		terminal.font_size = 8;
		terminal.on_keypress = [this](event_t *event, Control *) {
			if (!event)
				return;
			if (event->key == 8) // backspace
			{
				// TODO:
				return;
			}
			if (event->key == 9) // tab
			{
				// TODO:
				return;
			}
			if (event->key == 10) // enter
			{
				char **splitted = split(this->command.c_str(), ' ');
				if (!splitted)
					return; // skip if error
				// TODO: execute command
				int pid = exec(splitted[0], (const char **)(splitted+sizeof(char *)));
				printf("pid: %i\n", pid);

				while (pstate(pid) != PROC_ZOMBIE)
					continue;

				printf("finished %i\n", pid);

				cleanup_splitted(splitted);
				return;
			}
			printf("pressed %i\n", event->key);
			terminal.content = terminal.content + string((const char *)&event->key);
		};

		// reset command

		command = "";

		this->add_control(&terminal);
	};
};
