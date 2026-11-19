# d3c compiler and SDK

`compiler/` contains the official `d3c` compiler, its embedded application
SDK, the app starter template, and the VideoCore IV assembler. The app system
uses native C++ programs and produces `.d3m0n` executables with an embedded
manifest and icon.

## Install

From the repository root, build and install for the current user:

```sh
make -C compiler
```

The default installation directory is `~/com.ares`; add it to `PATH` if
needed. For a system-wide install, use:

```sh
make -C compiler install INSTALL_FOR_EVERYONE=1
```

See the [app development guide](../docs/d3c-app-development.md) for
requirements, install details, and firmware integration.

## Create and build an app

```sh
d3c init my-app
cd my-app
make
```

The template builds `my-app.d3m0n`. It includes a Makefile, `icon.ico`, a C++
entry point in `src/main.cpp`, and a sample window in `src/windows/`. The
application model, controls, and event callbacks are explained in the
[SDK programming guide](../docs/sdk-programming-model.md); public declarations
are documented in the generated [SDK reference](../docs/README.md).

## Compiler modes

`d3c` compiles C/C++ source and links the SDK runtime into an app. Its GPU mode
passes assembler arguments following `--gpu` to the bundled VideoCore IV
assembler:

```sh
d3c --gpu shader.qasm -o shader.bin
```

Use `d3c --version` to inspect the compiler build and SDK availability.
