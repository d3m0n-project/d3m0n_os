# d3m0n Embedded Functions

This document describes the built-in functions available to d3m0n scripts.

## Summary

1. [Logging Functions](#1-logging-functions)
2. [Apps Functions](#2-apps-functions)
3. [Templates](#3-templates)
4. [Settings](#4-settings)
5. [Windows](#5-windows)
6. [Control States](#6-control-states)
7. [System](#7-system)
8. [String Manipulation](#8-string-manipulation)

---

# 1. Logging Functions

## `alert(text: str);`

Displays a temporary notification popup on the screen.

Example:

```c
alert("File saved successfully");
```

---

## `log(text: str, color: str);`

Writes a message to the system console.

The `color` parameter uses the color names defined in [Colors.md](../Colors.md).

Example:

```c
log("Application started", "GREEN");
```

---

# 2. Apps Functions

## `app.open(package: str);`

Opens an application from the application list.

The package format is defined in the [package convention](../package_convention.md).

Example:

```c
app.open("system.settings");
```

---

## `app.exit();`

Closes the current application and returns to the launcher.

---

## `app.list()`

Returns an iterable list containing all installed applications.

Each entry contains:

| Field | Description |
|-|-|
| `name` | Application display name |
| `icon` | Application icon path |
| `package` | Application package identifier |

Example:

```c
for (name, icon, package) in app.list() {
    log(name, "WHITE");
}
```

---

# 3. Templates

## `template.load(path: str, parent_id: str, x: int, y: int, ...strings);`

Loads a UI template file and places it on the screen at relative coordinates (`x`, `y`) with the parent with name `parent_id` if found (use `""` as `parent_id` if you dont want to attach to a parent).

Additional string arguments replace `$N` placeholders:

- `$0` → first string argument
- `$1` → second string argument
- etc.

Example:

Template:

`app_dir/layouts/test.template`

```text
# d3m0n layout

Image:
    src="/themes/default_dark/icons/default.bmp";
    location="0, 0";
    width="65px";
    height="65px";

Text:
    content="$0";
    location="0, 65";
```

Script:

```c
template.load("layouts/test.template", "", 10, 10, "Hello World!");
```

---

> [!NOTE]
> Templates can have an associated source file.
>
> A template:
>
> ```
> layouts/test.template
> ```
>
> can have a source file:
>
> ```
> src/test.template.src
> ```

---

# 4. Settings

## `settings.set(key: str, value: str);`

Sets a configuration value.

Creates the key if it does not already exist.

Example:

```c
settings.set("theme", "dark");
```

---

## `settings.get(key: str) -> str`

Returns the value of a configuration key.

Returns an empty string if the key does not exist.

Example:

```c
theme = settings.get("theme");
```

---

# 5. Windows

## `window.open(name: str);`

Opens a window from the current application.

Example:

```c
window.open("main.layout");
```

---

## `window.goback();`

Returns to the previously opened window.

---

# 6. Control States

## `state.set(control: str, key: str, value: str);`

Changes a property of an existing control.

Parameters:

| Parameter | Description |
|-|-|
| `control` | Control identifier |
| `key` | Property name |
| `value` | New value |

Example:

```c
state.set("myText", "content", "Hello World!");
```

---

# 7. System

## `system.get(name: str) -> str`

Returns a system variable.

Available variables:

| Name | Description |
|-|-|
| `battery_charging` | Battery charging state (`true`/`false`) |
| `battery_percentage` | Current battery level |
| `read_speed` | Average storage read speed in Mb/s |

Example:

```c
speed = system.get("read_speed");
```

---

### `system.version() -> str`

Returns the current running kernel version number.

---

### `system.version_name() -> str`

Returns the current running kernel version name.

---

# 8. String Manipulation

All string functions are available through the `string` namespace.

---

## Length and validation

### `string.len(text: str) -> int`

Returns the length of a string.

---

### `string.is_empty(text: str) -> bool`

Returns true if the string is empty.

---

### `string.is_digit(text: str) -> bool`

Returns true if all characters are digits.

---

### `string.is_alpha(text: str) -> bool`

Returns true if all characters are alphabetic.

---

### `string.is_alphanum(text: str) -> bool`

Returns true if all characters are letters or numbers.

---

### `string.is_space(text: str) -> bool`

Returns true if all characters are whitespace.

---

# Case Conversion

## `string.lower(text: str) -> str`

Returns a lowercase copy.

---

## `string.upper(text: str) -> str`

Returns an uppercase copy.

---

# Searching

## `string.contains(text: str, search: str) -> bool`

Checks if `search` exists inside `text`.

---

## `string.starts_with(text: str, prefix: str) -> bool`

Checks if a string begins with a prefix.

---

## `string.ends_with(text: str, suffix: str) -> bool`

Checks if a string ends with a suffix.

---

# Comparison

## `string.cmp(a: str, b: str) -> int`

Compares two strings.

Returns:

- negative value if `a < b`
- zero if `a == b`
- positive value if `a > b`

---

## `string.equal(a: str, b: str) -> bool`

Checks whether two strings are identical.

---

# Modification

## `string.trim(text: str) -> str`

Removes whitespace from both sides.

---

## `string.trim_left(text: str) -> str`

Removes whitespace from the beginning.

---

## `string.trim_right(text: str) -> str`

Removes whitespace from the end.

---

## `string.replace(text: str, old: str, new: str) -> str`

Replaces occurrences of a substring.

Example:

```c
string.replace("hello world", "world", "d3m0n");
```

Result:

```text
hello d3m0n
```

---

## `string.reverse(text: str) -> str`

Returns the reversed string.

Example:

```c
string.reverse("abc");
```

Result:

```text
cba
```

---

# Substrings

## `string.substr(text: str, start: int, len: int) -> str`

Returns a portion of a string.

Example:

```c
string.substr("abcdef", 2, 3);
```

Result:

```text
cde
```

---

# Joining

## `string.join(string1: str, string2: str) -> str`

Joins two strings using a separator.

Example:

```c
string.join("hello", " world");
```

Result:

```text
hello world
```