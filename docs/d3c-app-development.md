# Build a d3m0n OS application with d3c {#d3c-app-development}

The supported application workflow is C++ source compiled by `d3c`. The
compiler links the SDK runtime and emits a `.d3m0n` executable containing the
application metadata and icon. The starter project and its Makefile are the
reference for a new app.

## Requirements and checkout

Build `d3c` on Linux with `g++`, GNU Make, GNU `ld`, and `objcopy`. Clone this
repository including its submodules:

```sh
git clone --recurse-submodules https://github.com/d3m0n-project/d3m0n_os.git
cd d3m0n_os
```

## Install d3c

From the repository root:

```sh
make -C compiler
```

The default target builds and installs `d3c` with the SDK included. The default
is a per-user installation under `~/com.ares`; the Makefile also attempts to
create `~/.local/bin/d3c`. Add the install directory to `PATH` if needed:

```sh
export PATH="$PATH:$HOME/com.ares"
```

For a system-wide install, run `make -C compiler install
INSTALL_FOR_EVERYONE=1` with permissions to write under `/opt`. It installs
under `/opt/com.ares` and attempts to link `/bin/d3c`. `INCLUDE_SDK=0` creates
a compiler without the embedded SDK and is
not suitable for the standard app workflow. Re-running `make -C compiler`
rebuilds and installs the compiler. `make -C compiler remove` removes its
installed executable.

## Initialize a project

Choose a working directory and initialize a project there:

```sh
d3c init project-name
cd project-name
```

The initializer creates `project-name` and copies the installed compiler
template into it. Use a new directory name without spaces or shell punctuation.
The template contains:

```text
project-name/
├── Makefile
├── icon.ico
└── src/
    ├── main.cpp
    └── windows/
        └── main_window.hpp
```

`src/main.cpp` defines `APP_INFO(...)`, includes the main window, and exports
`extern "C" int main(void)`. The example window is implemented in
`src/windows/main_window.hpp`.

## Build the app

Build from the project directory:

```sh
make
```

The template Makefile finds all `.cpp` files below `src`, compiles them with
`d3c`, and links the result to `project-name.d3m0n` in the project directory.
The output name is based on the current directory name. `make clean` removes
object files; `make fclean` also removes the app output. The Makefile supports
`APP_PATH` to choose an output directory, for example:

```sh
make APP_PATH=./build/
```

You can also compile a single translation unit directly, as in
`d3c src/main.cpp -o my-app.d3m0n`, though the project Makefile is preferred
when an app has multiple source files.

## Put the app on the device

Copy the generated `.d3m0n` file to the device's apps directory. Firmware apps
are organized under `/apps/` by package path, for example the launcher is
configured at `/apps/system/launcher/launcher.d3m0n`. The launcher discovers
`.d3m0n` files in app directories. The top-level firmware build can build and
stage repository applications automatically:

```sh
make export
```

The `applications/builder.sh` script runs each app Makefile's `export` target
with an output path under `rootfs/apps/<application-path>/`. Set
`BUILD_APPS=0` to skip that step. The root `make disk` target builds a test disk
after building apps; `make export` builds the bootable SD image. Device
installation details depend on the firmware image and are outside the app
compiler itself.

## GPU assembly

The compiler also offers VideoCore IV assembly mode when built with its SDK:

```sh
d3c --gpu mycode.qasm -o mycode.bin
```

Options following `--gpu` are passed to the bundled assembler. See
`compiler/vc4asm/README.md` and run the assembler help for its specific options.
