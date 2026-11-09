# d3m0n terminal app

Simple d3m0n terminal app.

The terminal supports file redirection for a single command:

- `command < input.txt` reads the child's standard input from a file.
- `command > output.txt` creates or truncates a file for standard output.
- `command >> output.txt` appends standard output to a file.

The kernel exposes `exec_stdio(path, argv, stdio_fds)` for launching a process
with redirected standard streams. Each entry is a parent descriptor returned
by `open()`, or `-1` to keep the default console stream. The parent keeps the
opened descriptors alive until the child exits.
