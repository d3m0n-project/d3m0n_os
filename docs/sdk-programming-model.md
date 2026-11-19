# SDK application programming model {#sdk-programming-model}

The current GUI SDK is a C++ API. Applications construct a `Window`, create
controls as C++ objects, connect callbacks, and pass the window to
`Application::run()`. See the generated reference for full declarations and
method details; the snippets here follow `compiler/template` and
`applications/testing/hello_world`.

## Entry point and manifest

An application has a C entry point and an `APP_INFO` declaration. The macro is
defined in `app.hpp`; it embeds the manifest fields and includes the icon file
into the executable. Keep the icon path valid relative to the app build
directory.

```cpp
#include "sys.h"
#include "stdio.hpp"
#include "app.hpp"

APP_INFO(
    "Hello World",             // name shown to the system
    1.0,                        // app version
    "Your name",               // author
    "A small example app",     // description
    "icon.ico"                 // embedded icon path
)

#include "windows/main_window.hpp"

extern "C" int main(void)
{
    MainWindow window;
    Application::run(window);
    return 0;
}
```

`Application::run()` draws the initial window and runs the application event
loop. Apps that perform initialization which can fail may catch exceptions,
as the starter and SDK examples do. A window passed to `run()` remains active
for the lifetime of the loop.

## Windows and controls

Derive a window type from `Window`. The constructor takes a title, width, and
height. Controls derive from `Control`; common public properties include
`location`, `width`, `height`, `margin_*`, `color`, `bg_color`, `visible`, and
`enabled`. Add controls to the window with `add_control()`.

```cpp
class MainWindow : public Window
{
    Rect background;
    Text label;

public:
    MainWindow() : Window("Main Window", Size(320), Size(448))
    {
        background.location = Point(0, 0);
        background.width = Size(200);
        background.height = Size(50);
        background.bg_color = Color(0xFFFF0000);

        label.location = Point(10, 10);
        label.content = "Hello World!";
        label.color = Color(255, 255, 255);
        label.font_size = 16;

        add_control(&background);
        add_control(&label);
    }
};
```

Controls should be stored as members (or otherwise outlive their use by the
window), because the window keeps pointers to them. `Size(n)` is a fixed
dimension in pixels. A relative dimension can be expressed with
`Size(percent, true)` and is resolved against the parent size during layout.
`Color` accepts packed 32-bit values, RGB/RGBA components, or a `#RRGGBB`
string. A packed value uses the SDK's expected color representation; inspect
`Color` in the API reference if constructing it from raw integers.

## Available controls

The SDK currently defines these controls under `app/controls`:

- `Rect`: filled rectangular or rounded area; can contain child controls.
- `Text`: text with automatic dimensions when width or height is unset.
- `Button` and `RoundButton`: text or image button controls.
- `TextBox`: displays text and supports password masking through its `type`.
- `CheckBox`, `RadioButton`, and `Switch`: selection controls.
- `Image`: image display control.
- `ListView`: list and row controls.
- `ProgressBar`: progress display.
- `Hscroll` and `Vscroll`: horizontal and vertical scrolling controls.
- `WebView`: web content control.
- `InlineSvgIcon`: inline SVG icon control.

Refer to each class's generated page for fields, drawing behavior, and current
limitations. The presence of a control class does not imply that every
interaction is implemented; behavior is determined by its source and the
window event dispatcher.

## Events and redraws

`Control` exposes `on_click`, `on_keypress`, and `on_keyrelease` callbacks. A
control callback receives an `event_t *` and the `Control *` that received the
event. `Window` also exposes key, general event, and idle callbacks. For
example:

```cpp
button.on_click = [](event_t *event, Control *control) {
    (void)event;
    (void)control;
    // Update application state here.
};
```

The event definitions and key constants are in `app/events.h`. The standard
`Application::run()` loop dispatches queued events and redraws the active
window when a registered callback is dispatched. If writing a
custom event loop, call `Window::draw()` before polling so control bounds are
initialized, then use `poll()` or `dispatch_event()` according to the API.

## Drawing and system APIs

`Display` in `app/graphics.hpp` supplies framebuffer primitives, text, PNG,
and SVG drawing. Applications using the control framework generally override
control properties and drawing behavior rather than calling these primitives
directly. The broader SDK also includes system calls and types (`sys.h`,
`types.h`), memory and standard library helpers, parsing utilities, fonts,
icons, and locale/configuration helpers. Use the generated API index to find
these modules and their declarations.

## Current source of truth

The starter at `compiler/template` and maintained C++ apps in `applications/`
are authoritative for app construction and build behavior. Old documentation
files in `docs/` about `.layout` files, `.src` scripts, declarative controls,
and the prior app package format describe a retired system and are not part of
this guide.
