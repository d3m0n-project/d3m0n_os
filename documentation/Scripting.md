# d3m0n embeded in functions

## Summary
1. [Logging Functions](#logging_functions)
2. [Apps Functions](#apps_functions)
3. [Templates](#templates)
4. [Settings](#settings)
5. [Windows](#windows)


---
### 1. Logging Functions
- `alert(text: str);`

Popup a toast on the screen.

---
- `log(text: str, color: str);`

Log a message in the console, check [here](../Colors.md) for color parameter.



### 2. Apps Functions
- `app.open(packet: str);`

Opens a package from the application list, note that package format is defined in [this convention](../package_convention.md).

---
- `app.exit();`

Quit current app, go back to launcher application.

---
- ```
	for (name, icon, package) in app.list() {
		...
	}
	```

Fetches the list of all the applications on the device and execute a piece of code for each of them.

### 3. Templates
- `template.load(path: str, x: int, y: int, "string 1", "string 2", ...);`

Loads the template located at `path` at the location (`x`,`y`) onto the screen. Note that `string 1`, `string 2`... (string) replaces all the `$N` string in the template with this content.

#### e.g.
`app_dir/layouts/test.template` -> 
```
# d3m0n layout

Image:
	src="/themes/default_dark/icons/default.bmp";
	location="0, 0";
	width="65px";
	height="65px";

Text:
	content="$0";  # will be replaced by 'Hello World!'
	location="0, 65";
```

CODE: `template.load("layouts/test.template", 10, 10, "Hello World!");`

> [!NOTE]
> Templates can have a source file associated, just place it in `app_dir/src/` with the layout name followed by `.src`: `layouts/test.template` --> `src/test.template.src`

### 4. Settings
- `settings.set(key: str, value: str);`

Modify the value of a key in the device config or add a new one if it is not added yet.

---
- `settings.get(key: str);`

Fetch the value of a key in the configuration of the device. Returns an empty string if not found.


### 5. Windows
- `window.open(name: str);`

Opens a window in the current loaded app, `name` is the window name ex: `window.open("main.layout");` results at loading the main window.

---
- `window.goback();`

Go back to the last window opened.

### 6. Control States
- `state.set(control_name: str, key: str, value: str);`

Sets a control's field value dynamically. e.g: `state.set("myText", "content", "My New Text");`.