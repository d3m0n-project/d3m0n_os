#pragma once

#include "app.hpp"
#include "stdio.hpp"

#ifndef APP_KEY_ARROW_UP
	#define APP_KEY_ARROW_UP	0x100
	#define APP_KEY_ARROW_DOWN	0x101
#endif

#define MAX_COMMAND_SIZE		255
#define MAX_HISTORY				255
#define MAX_DISPLAY_SIZE		((this->width.get() / (this->terminal.font_size)) * (this->height.get() / (this->terminal.font_size * 2)))
#define PATH_ENV_VAR			(char *)"/programs/bin:/programs/test"

class MainWindow : public Window
{
private:
	Text	terminal;
	string	console_text;
	char	command[MAX_COMMAND_SIZE];
	size_t	command_i = 0;
	char	**history = 0;
	size_t	history_next = 0;
	size_t	history_count = 0;
	size_t	history_navigation = 0;
	char	history_scratch[MAX_COMMAND_SIZE];
	int		active_pid = 0;
	int		active_opened[3] = {-1, -1, -1};
	char	cwd[255];

	void	render_input(void)
	{
		terminal.content = console_text + string(command);
	}

	void	append_console(const char *text)
	{
		if (!text || !*text)
			return;
		string combined = console_text + string(text);
		size_t skip = combined.length() > MAX_DISPLAY_SIZE ? combined.length() - MAX_DISPLAY_SIZE : 0;
		if (skip)
		{
			while (skip < combined.length() && combined[skip] != '\n')
				++skip;
			if (skip < combined.length())
				++skip;
		}
		console_text = string(combined.c_str() + skip);
	}

	void	set_command(const char *text)
	{
		strlcpy(command, text ? text : "", sizeof(command));
		command_i = strlen(command);
		render_input();
	}

	void	next_prompt(const char *message = 0)
	{
		append_console(message);
		memset(command, 0, sizeof(command));
		command_i = 0;
		history_navigation = 0;
		append_console(this->cwd);
		append_console(" $ ");
		render_input();
	}

	void	save_history(void)
	{
		if (!command_i)
			return;
		if (history_count)
		{
			size_t previous = (history_next + MAX_HISTORY - 1) % MAX_HISTORY;
			if (!strcmp(history[previous], command))
				return;
		}
		strlcpy(history[history_next], command, MAX_COMMAND_SIZE);
		history_next = (history_next + 1) % MAX_HISTORY;
		if (history_count < MAX_HISTORY)
			++history_count;
		history_navigation = 0;
	}

	void	history_up(void)
	{
		if (!history_count || history_navigation >= history_count)
			return;
		if (history_navigation == 0)
			strlcpy(history_scratch, command, sizeof(history_scratch));
		++history_navigation;
		size_t index = (history_next + MAX_HISTORY - history_navigation) % MAX_HISTORY;
		set_command(history[index]);
	}

	void	history_down(void)
	{
		if (!history_navigation)
			return;
		--history_navigation;
		if (!history_navigation)
			set_command(history_scratch);
		else
		{
			size_t index = (history_next + MAX_HISTORY - history_navigation) % MAX_HISTORY;
			set_command(history[index]);
		}
	}

	int		find_relative(char *out, char *in, char *cwd)
	{
		if (path_join(out, 128, cwd, in) < 0)
			return 0;

		printf("path: %s\n", out);

		int fd = open(out, O_READ);
		if (fd < 0)
			return 0;
		
		close(fd);
		return 1;
	}

	int		find_env_binary(char *out, const char *command_name, const char *env_paths)
	{
		if (!out || !command_name || !*command_name || !env_paths)
			return 0;

		const char *path_start = env_paths;
		while (1)
		{
			const char *path_end = path_start;
			while (*path_end && *path_end != ':')
				++path_end;

			char directory[MAX_COMMAND_SIZE];
			size_t path_length = path_end - path_start;
			if (path_length < sizeof(directory))
			{
				memcpy(directory, path_start, path_length);
				directory[path_length] = '\0';
				if (!path_length)
					strlcpy(directory, cwd, sizeof(directory));

				if (path_join(out, MAX_COMMAND_SIZE, directory, command_name) == 0)
				{
					int fd = open(out, O_READ);
					if (fd >= 0)
					{
						close(fd);
						return 1;
					}
				}
			}

			if (!*path_end)
				break;
			path_start = path_end + 1;
		}
		return 0;
	}

	int		resolve_directory(char *out, size_t out_size, const char *path)
	{
		if (!path || !*path)
			return 0;
		if (path[0] == '/')
		{
			if (strlcpy(out, path, out_size) >= out_size)
				return 0;
		}
		else if (path_join(out, out_size, cwd, path) < 0)
			return 0;

		// normalize
		char normalized[255];
		size_t starts[255];
		size_t input_i = 0;
		size_t output_i = 1;
		size_t depth = 0;
		normalized[0] = '/';
		while (out[input_i])
		{
			while (out[input_i] == '/')
				++input_i;
			size_t start = input_i;
			while (out[input_i] && out[input_i] != '/')
				++input_i;
			size_t length = input_i - start;
			if (!length || (length == 1 && out[start] == '.'))
				continue;
			if (length == 2 && out[start] == '.' && out[start + 1] == '.')
			{
				if (depth)
					output_i = starts[--depth];
				continue;
			}
			if (output_i > 1)
				normalized[output_i++] = '/';
			if (output_i + length >= sizeof(normalized))
				return 0;
			starts[depth++] = output_i > 1 ? output_i - 1 : 1;
			memcpy(normalized + output_i, out + start, length);
			output_i += length;
		}
		normalized[output_i] = '\0';
		char *entries[32];
		if (getdents(normalized, entries, sizeof(entries)) < 0)
			return 0;
		return strlcpy(out, normalized, out_size) < out_size;
	}

	void	handle_child_key(int key)
	{
		if (key == 8)
		{
			if (command_i)
			{
				terminal_input(active_pid, key);
				command[--command_i] = '\0';
				render_input();
			}
			return;
		}
		if (key == 10)
		{
			terminal_input(active_pid, '\n');
			append_console(command);
			append_console("\n");
			memset(command, 0, sizeof(command));
			command_i = 0;
			render_input();
			return;
		}
		if (key == 9)
		{
			terminal_input(active_pid, '\t');
			return;
		}
		if (command_i + 1 < MAX_COMMAND_SIZE && key >= 32 && key < 127)
		{
			terminal_input(active_pid, key);
			command[command_i++] = (char)key;
			command[command_i] = '\0';
			render_input();
		}
	}

	void submit_command(void)
	{
		append_console(command);
		append_console("\n");
		if (!command_i)
		{
			next_prompt();
			return;
		}
		save_history();
		char **parts = split(command, ' ');
		if (!parts)
		{
			next_prompt("could not parse command\n");
			return;
		}
		if (!parts[0])
		{
			cleanup_splitted(parts);
			next_prompt("invalid command\n");
			return;
		}
		if (!strcmp(parts[0], "exit"))
		{
			int status = parts[1] ? atoi(parts[1]) : 0;
			cleanup_splitted(parts);
			exit(status);
			return;
		}
		if (!strcmp(parts[0], "pwd"))
		{
			if (parts[1])
				next_prompt("pwd: too many arguments\n");
			else
			{
				append_console(cwd);
				append_console("\n");
				next_prompt();
			}
			cleanup_splitted(parts);
			return;
		}
		if (!strcmp(parts[0], "cd"))
		{
			const char *target = parts[1] ? parts[1] : "/";
			char new_cwd[sizeof(cwd)];
			if ((parts[1] && parts[2]) || !resolve_directory(new_cwd, sizeof(new_cwd), target))
				next_prompt("cd: directory not found\n");
			else
			{
				strlcpy(cwd, new_cwd, sizeof(cwd));
				next_prompt();
			}
			cleanup_splitted(parts);
			return;
		}

		const char *argv[MAX_COMMAND_SIZE / 2 + 1];
		int argc = 0;
		int stdio_fds[3] = {-1, -1, -1};
		int opened[3] = {-1, -1, -1};
		bool parse_error = false;
		for (int i = 0; parts[i]; ++i)
		{
			if (!strcmp(parts[i], "<") || !strcmp(parts[i], ">") || !strcmp(parts[i], ">>"))
			{
				int stream = parts[i][0] == '<' ? 0 : 1;
				if (!parts[i + 1] || opened[stream] >= 0)
				{
					parse_error = true;
					break;
				}
				int flags = stream == 0 ? O_READ : (O_WRITE | O_CREATE | (parts[i][1] == '>' ? O_APPEND : O_TRUNC));
				opened[stream] = open(parts[++i], flags);
				if (opened[stream] < 0)
				{
					parse_error = true;
					break;
				}
				stdio_fds[stream] = opened[stream];
				continue;
			}
			argv[argc++] = parts[i];
		}
		argv[argc] = 0;
		if (parse_error || argc == 0)
		{
			for (int i = 0; i < 3; ++i)
				if (opened[i] >= 0)
					close(opened[i]);
			cleanup_splitted(parts);
			next_prompt("invalid command or redirection\n");
			return;
		}

		// find realpath of binary
		int pid = 0;
		char found_binary[MAX_COMMAND_SIZE] = {0};
		if (argv[0] && argv[0][0] == '/') // full path
			pid = exec_stdio(argv[0], argv + 1, stdio_fds);
		else if (argv[0] && argv[0][0] == '.' && argv[0][1] == '/' && find_relative(found_binary, (char *)(argv[0]), this->cwd)) // relative path
			pid = exec_stdio(found_binary, argv + 1, stdio_fds);
		else if (argv[0] && find_env_binary(found_binary, argv[0], PATH_ENV_VAR)) // env binary
			pid = exec_stdio(found_binary, argv + 1, stdio_fds);

		// start binary
		if (pid <= 0)
		{
			for (int i = 0; i < 3; ++i)
				if (opened[i] >= 0)
					close(opened[i]);
			cleanup_splitted(parts);
			next_prompt("command not found\n");
			return;
		}
		active_pid = pid;
		for (int i = 0; i < 3; ++i)
			active_opened[i] = opened[i];
		memset(command, 0, sizeof(command));
		command_i = 0;
		render_input();
		cleanup_splitted(parts);
	}

public:
	MainWindow(void): Window("Terminal", Size(320), Size(436))
	{
		this->bg_color = Color(0, 0, 0);
		terminal.location = Point(0, 0);
		terminal.color = Color(0xFFFFFFFF);
		terminal.bg_color = Color(0, 0, 0);
		terminal.width = this->width;
		terminal.height = this->height;
		terminal.text_align = ANCHOR_LEFT | ANCHOR_TOP;
		terminal.font_size = 8;
		memset(command, 0, sizeof(command));
		memset(history_scratch, 0, sizeof(history_scratch));
		memset(cwd, 0, 255);
		cwd[0] = '/';
		history = (char **)calloc(MAX_HISTORY, sizeof(char *));
		if (history)
		{
			for (size_t i = 0; i < MAX_HISTORY; ++i)
			{
				history[i] = (char *)calloc(MAX_COMMAND_SIZE, sizeof(char));
				if (!history[i])
				{
					fprintf(stderr, "Could not allocate terminal history\n");
					exit(1);
				}
			}
		}
		else
		{
			fprintf(stderr, "Could not allocate terminal history\n");
			exit(1);
		}
		append_console(this->cwd);
		append_console(" $ ");
		render_input();

		terminal.on_keypress = [this](event_t *event, Control *) {
			if (!event)
				return;
			int key = event->key;
			if (active_pid > 0)
			{
				handle_child_key(key);
				return;
			}
			if (key == APP_KEY_ARROW_UP)
			{
				history_up();
				return;
			}
			if (key == APP_KEY_ARROW_DOWN)
			{
				history_down();
				return;
			}
			if (key == 8) // backspace
			{
				if (command_i)
				{
					history_navigation = 0;
					command[--command_i] = '\0';
					render_input();
				}
				return;
			}
			if (key == 10) // enter
			{
				submit_command();
				return;
			}
			if (key == 9) // tab
				return;
			if (command_i + 1 < MAX_COMMAND_SIZE && key >= 32 && key < 127)
			{
				history_navigation = 0;
				command[command_i++] = (char)key;
				command[command_i] = '\0';
				render_input();
			}
		};

		this->on_event = [this](event_t *event, Window *) {
			if (!event)
				return;
			if (event->type == EVENT_TERMINAL_EXIT && active_pid > 0)
			{
				for (int i = 0; i < 3; ++i)
				{
					if (active_opened[i] >= 0)
						close(active_opened[i]);
					active_opened[i] = -1;
				}
				active_pid = 0;
				next_prompt(event->key == PROC_CRASHED ? "process crashed\n" : 0);
				return;
			}
			if (event->type != EVENT_TERMINAL_OUTPUT)
				return;
			
			uint32_t len = event->modifiers;
			if (len > sizeof(uint64_t) + 3 * sizeof(int32_t))
				len = sizeof(uint64_t) + 3 * sizeof(int32_t);
			
			char output[21];
			memcpy(output, &event->timestamp, len);
			output[len] = '\0';
			for (uint32_t i = 0; i < len; ++i)
				if (output[i] == '\r') output[i] = '\n';
			
			append_console(output);
			render_input();
		};

		this->add_control(&terminal);
	}
};
