# d3c C++ application template

This is the starter copied by `d3c init project-name`. It builds a native
C++ d3m0n OS application:

```sh
d3c init hello
cd hello
make
```

The Makefile compiles each `.cpp` file under `src/` with `d3c` and links
`hello.d3m0n`. `APP_INFO(...)` in `src/main.cpp` embeds the app name, version,
author, description, and `icon.ico`. The program's C entry point creates a
`Window` and starts `Application::run()`.

Edit `src/main.cpp` and `src/windows/main_window.hpp` to customize the sample.
Add more C++ source files below `src/`; the Makefile discovers them
recursively. Keep controls alive for as long as the window uses them, typically
by storing them as window members.

See the repository [app development guide](../../docs/d3c-app-development.md)
for the compiler installation and build workflow, and the
[SDK programming guide](../../docs/sdk-programming-model.md) for windows,
controls, dimensions, colors, and events.
