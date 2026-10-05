#ifndef SVG_H
#define SVG_H

#include "types.h"
#include "xml.h"

#ifdef __cplusplus
	#include "app/graphics.hpp"
	#include "sys.h"
	#include "stdio.hpp"
	#define DISPLAY_FUNC(name)	Display::name
	#define DISPLAY(name)		display->name
	#define PRINT(fmt, data)	printf(fmt, data);
	#define FUNC_TYPE			inline
	#include "app/HTML/html_colors.h"
#else
	#include "../compiler/sdk/lib/app/HTML/html_colors.h"
	#include "display/display.h"
	#include "filesystem/filesystem.h"
	#include "memory/memory.h"
	#include "libft.h"
	#include "time.h"
	#define strnstr(a, b, c)		ft_strnstr(a, b, c)
	#define strlen(a)				ft_strlen(a)
	#define strchr(a, b)			ft_strchr(a, b)
	#define isspace(a)				ft_isspace(a)
	#define strlcpy(a, b, c)		ft_strlcpy(a, b, c)
	#define PRINT(fmt, data)		log(fmt, 0, data);//usleep(1000000);

	#define DISPLAY_FUNC(name)		name
	#define Display					int
	#define DISPLAY(name)			name
	#define this					0
	#define malloc					kmalloc
	#define free					kfree
	#define FUNC_TYPE				inline static 
#endif

#define SVG_TOKEN_COUNT		1024

/** @brief print_range operation. */
static void	print_range(const char *buffer, unsigned start, unsigned end)
{
	while (start < end)
	{
		PRINT("%c", buffer[start++]);
	}
}

/** @brief draw_attribute operation. */
static unsigned	draw_attribute(char *element_name, const char *buffer, const sxmltok_t *tokens, unsigned pos, unsigned count, int x, int y, int w, int h, uint32_t override_color)
{
    const sxmltok_t	*key;
    unsigned		i;

    if (pos >= count)
        return pos;

    key = &tokens[pos];

	PRINT(element_name, 0);
	PRINT(": ", 0);
    print_range(buffer, key->startpos, key->endpos);
    PRINT(": ", 0);

    pos++;

    /*
     * Everything after the CDATA key until the next CDATA
     * belongs to this attribute's value.
     */
    for (i = pos; i < count; i++)
    {
        if (tokens[i].type == SXML_CDATA)
            break;

        if (tokens[i].type == SXML_CHARACTER)
            print_range(buffer,
                        tokens[i].startpos,
                        tokens[i].endpos);

        pos++;
    }

    PRINT("\n", 0);
	(void)x;
	(void)y;
	(void)w;
	(void)h;
	(void)override_color;

    return pos;
}

/** @brief draw_xml_tree operation. */
static void draw_xml_tree(const char *buffer, const sxmltok_t *tokens, unsigned num_tokens, int x, int y, int w, int h, uint32_t override_color)
{
    unsigned i = 0;
    while (i < num_tokens)
    {
        const sxmltok_t *token = &tokens[i];
        if (token->type == SXML_STARTTAG)
        {
            unsigned j;
            unsigned child_start;
            unsigned depth;
            char	*element_name = (char *)malloc(token->endpos - token->startpos + 2);
			strlcpy(element_name, &buffer[token->startpos], token->endpos - token->startpos + 1);
			PRINT("'%s'\n", element_name);

			j = i + 1;
			while (j <= i + token->size)
			{
				if (tokens[j].type != SXML_CDATA)
				{
					j++;
					continue;
				}
				j = draw_attribute(element_name, buffer, tokens, j, i + token->size + 1, x, y, w, h, override_color);
			}
			free(element_name);


			child_start = i + token->size + 1;
			depth = 1;
			while (child_start < num_tokens && depth)
			{
				if (tokens[child_start].type == SXML_STARTTAG)
					depth++;
				else if (tokens[child_start].type == SXML_ENDTAG)
					depth--;

				// end element
				if (depth != 0)
					child_start++;
			}

			// recurcively search xml children
			if (child_start > i + token->size + 1)
				draw_xml_tree(buffer, tokens + i + token->size + 1, child_start - (i + token->size + 1), x, y, w, h, override_color);

			i = child_start + 1;
			continue;
        }
        i++;
    }
}

/** @brief DISPLAY_FUNC operation. */
FUNC_TYPE void	DISPLAY_FUNC(draw_svg_buff)(int x, int y, int w, int h, const char *buffer, size_t buffer_len, uint32_t override_color)
{
    sxmltok_t	*tokens = (sxmltok_t *)malloc(SVG_TOKEN_COUNT * sizeof(sxmltok_t));
    sxml_t		parser;
    sxmlerr_t	err;

    sxml_init(&parser);
    while (1)
    {
        err = sxml_parse(&parser, buffer, buffer_len, tokens, SVG_TOKEN_COUNT);
        if (err == SXML_SUCCESS)
            break;

        if (err == SXML_ERROR_TOKENSFULL)
        {
            /*
             * For this particular use we need the complete token
             * stream to recursively walk the anchors.
             *
             * Increase SVG_TOKEN_COUNT if this happens.
             */
            PRINT("SVG: too many XML tokens\n", 0);
            return;
        }

        if (err == SXML_ERROR_BUFFERDRY)
        {
            /*
             * This implementation intentionally expects the
             * complete SVG to fit in buffer.
             */
            PRINT("SVG: too large for buffer\n", 0);
            return;
        }

        PRINT("SVG: invalid SVG\n", 0);
        return;
    }

    draw_xml_tree(buffer, tokens, parser.ntokens, x, y, w, h, override_color);
}

/** @brief DISPLAY_FUNC operation. */
FUNC_TYPE int	DISPLAY_FUNC(draw_svg)(int x, int y, int w, int h, const char *path, uint32_t override_color)
{
	if (!path || w <= 0 || h <= 0)
		return 1;
	int fd = open(path, O_READ);
	if (fd < 0)
		return 1;
	char *svg = (char *)malloc(65536);
	if (!svg)
		return 1;

	int total = 0;
	int amount;
	while (total < 65535 && (amount = read(fd, svg + total, 65535 - total)) > 0)
		total += amount;
	close(fd);
	if (!total)
	{
		free(svg);
		return 1;
	}
	svg[total] = '\0';
	draw_svg_buff(x, y, w, h, svg, total, override_color);
	free(svg);
	return 0;
}

#endif