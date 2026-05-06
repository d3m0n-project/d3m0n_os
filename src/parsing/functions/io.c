#include "scripting.h"
#include "parsing.h"
#include "log.h"

void	fn_log(void **args)
{
	char	*message = ((char **)args)[0];
	char	*color = ((char **)args)[1];

	uint32_t	color_code = lp_parse_color(color);
	int b = (color_code & 0x00FF0000) >> 16;
	int g = (color_code & 0x0000FF00) >> 8;
	int r = (color_code & 0x000000FF);
	log("\x1b[38;2;%i;%i;%im%s\033[0m\n", LOG_NONE | LOG_INDENT, r, g, b, message);
}

void	fn_alert(void **args)
{
	char	*message = ((char **)args)[0];

	uint32_t	color_code = 0x000000FF;
	draw_text(SCREEN_WIDTH / 2, 0, -1, -1, (const char *)message, color_code, 0);
}