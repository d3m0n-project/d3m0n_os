# d3m0n OS SDK and app development

This documentation describes the app system implemented by the current
`compiler/sdk` and `compiler/template`: applications are C++ programs built by
`d3c`, with SDK metadata and an icon embedded into a `.d3m0n` executable.
Generated API pages are based on the SDK source tree.

## Build an app

- [Install `d3c`, initialize a project, and build it](d3c-app-development.md)
- [Application structure, windows, controls, and events](sdk-programming-model.md)
- [SDK API reference](annotated.html)

## Project and firmware context

The starter project is in `compiler/template`. Maintained examples include
`applications/testing/hello_world` and `applications/system/launcher`. Their
Makefiles build `.d3m0n` outputs with `d3c`.

When building the firmware, `applications/builder.sh` finds application
Makefiles and invokes their `export` targets. The top-level `make export`
builds the OS image and applications by default; `BUILD_APPS=0` skips app
builds. See [app development](d3c-app-development.md) for the full flow.

## Generate this site

From the repository root, run:

```sh
doxygen Doxyfile
```

Doxygen writes HTML into `docs/html`. Graphviz diagrams are included when the
`dot` executable is available. The left navigation sidebar has a Guides
section with direct links to the app build and SDK programming guides.
