### Load a Layout Template
`template.load(path, x, y, "string 1", "string 2", ...);`

`path` (string) where is the layout template is located

`x`, `y` (int) location of the template to be placed at

`string 1`, `string 2`... (string) replaces the "$N" string in the template with this content

ex:
```
# d3m0n layout

Image:
	src="/themes/default_dark/icons/default.bmp";
	location="0, 0";
	width="65px";
	height="65px";

Text:
	content="$1";
	location="0, 65";
```

`template.load("/path/to", 10, 10, "Hello World!");`

