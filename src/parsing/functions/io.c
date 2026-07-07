#include "scripting.h"
#include "parsing.h"
#include "log.h"

char*	fn_log(void **args)
{
	char	*message = ((char **)args)[0];
	char	*color = ((char **)args)[1];

	uint32_t	color_code = lp_parse_color(color);
	int b = (color_code & 0x00FF0000) >> 16;
	int g = (color_code & 0x0000FF00) >> 8;
	int r = (color_code & 0x000000FF);
	log("\x1b[38;2;%i;%i;%im%s\033[0m\n", LOG_NONE | LOG_INDENT, r, g, b, message);
	return 0;
}

char*	fn_alert(void **args)
{
	char	*message = ((char **)args)[0];

	uint32_t	color_code = 0x000000FF;
	draw_text(SCREEN_WIDTH / 2, 0, -1, -1, (const char *)message, color_code, 0);
	return 0;
}

char*	fn_app_list(void **args)
{
	(void)args;
	log("app.list() is only supported as a loop source in for (name, icon) in app.list()\n", LOG_INFO | LOG_INDENT);
	return 0;
}