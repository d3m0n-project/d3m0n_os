#ifndef HTML_COLORS_H
#define HTML_COLORS_H


#include "types.h"

typedef struct s_HTMLColor
{
	const char	*name;
	const int	*value;
}	t_HTMLColor;

#define HTML_COLORS_COUNT	140
static const t_HTMLColor	html_colors_list[HTML_COLORS_COUNT] = {
	{.name="MediumVioletRed", .value=(const int [3]){199, 21, 133}},
	{.name="DeepPink", .value=(const int [3]){255, 20, 147}},
	{.name="PaleVioletRed", .value=(const int [3]){219, 112, 147}},
	{.name="HotPink", .value=(const int [3]){255, 105, 180}},
	{.name="LightPink", .value=(const int [3]){255, 182, 193}},
	{.name="Pink", .value=(const int [3]){255, 192, 203}},
	{.name="DarkRed", .value=(const int [3]){139, 0, 0}},
	{.name="Red", .value=(const int [3]){255, 0, 0}},
	{.name="Firebrick", .value=(const int [3]){178, 34, 34}},
	{.name="Crimson", .value=(const int [3]){220, 20, 60}},
	{.name="IndianRed", .value=(const int [3]){205, 92, 92}},
	{.name="LightCoral", .value=(const int [3]){240, 128, 128}},
	{.name="Salmon", .value=(const int [3]){250, 128, 114}},
	{.name="DarkSalmon", .value=(const int [3]){233, 150, 122}},
	{.name="LightSalmon", .value=(const int [3]){255, 160, 122}},
	{.name="OrangeRed", .value=(const int [3]){255, 69, 0}},
	{.name="Tomato", .value=(const int [3]){255, 99, 71}},
	{.name="DarkOrange", .value=(const int [3]){255, 140, 0}},
	{.name="Coral", .value=(const int [3]){255, 127, 80}},
	{.name="Orange", .value=(const int [3]){255, 165, 0}},
	{.name="DarkKhaki", .value=(const int [3]){189, 183, 107}},
	{.name="Gold", .value=(const int [3]){255, 215, 0}},
	{.name="Khaki", .value=(const int [3]){240, 230, 140}},
	{.name="PeachPuff", .value=(const int [3]){255, 218, 185}},
	{.name="Yellow", .value=(const int [3]){255, 255, 0}},
	{.name="PaleGoldenrod", .value=(const int [3]){238, 232, 170}},
	{.name="Moccasin", .value=(const int [3]){255, 228, 181}},
	{.name="PapayaWhip", .value=(const int [3]){255, 239, 213}},
	{.name="LightGoldenrodYellow", .value=(const int [3]){250, 250, 210}},
	{.name="LemonChiffon", .value=(const int [3]){255, 250, 205}},
	{.name="LightYellow", .value=(const int [3]){255, 255, 224}},
	{.name="Maroon", .value=(const int [3]){128, 0, 0}},
	{.name="Brown", .value=(const int [3]){165, 42, 42}},
	{.name="SaddleBrown", .value=(const int [3]){139, 69, 19}},
	{.name="Sienna", .value=(const int [3]){160, 82, 45}},
	{.name="Chocolate", .value=(const int [3]){210, 105, 30}},
	{.name="DarkGoldenrod", .value=(const int [3]){184, 134, 11}},
	{.name="Peru", .value=(const int [3]){205, 133, 63}},
	{.name="RosyBrown", .value=(const int [3]){188, 143, 143}},
	{.name="Goldenrod", .value=(const int [3]){218, 165, 32}},
	{.name="SandyBrown", .value=(const int [3]){244, 164, 96}},
	{.name="Tan", .value=(const int [3]){210, 180, 140}},
	{.name="Burlywood", .value=(const int [3]){222, 184, 135}},
	{.name="Wheat", .value=(const int [3]){245, 222, 179}},
	{.name="NavajoWhite", .value=(const int [3]){255, 222, 173}},
	{.name="Bisque", .value=(const int [3]){255, 228, 196}},
	{.name="BlanchedAlmond", .value=(const int [3]){255, 235, 205}},
	{.name="Cornsilk", .value=(const int [3]){255, 248, 220}},
	{.name="Indigo", .value=(const int [3]){75, 0, 130}},
	{.name="Purple", .value=(const int [3]){128, 0, 128}},
	{.name="DarkMagenta", .value=(const int [3]){139, 0, 139}},
	{.name="DarkViolet", .value=(const int [3]){148, 0, 211}},
	{.name="DarkSlateBlue", .value=(const int [3]){72, 61, 139}},
	{.name="BlueViolet", .value=(const int [3]){138, 43, 226}},
	{.name="DarkOrchid", .value=(const int [3]){153, 50, 204}},
	{.name="Fuchsia", .value=(const int [3]){255, 0, 255}},
	{.name="Magenta", .value=(const int [3]){255, 0, 255}},
	{.name="SlateBlue", .value=(const int [3]){106, 90, 205}},
	{.name="MediumSlateBlue", .value=(const int [3]){123, 104, 238}},
	{.name="MediumOrchid", .value=(const int [3]){186, 85, 211}},
	{.name="MediumPurple", .value=(const int [3]){147, 112, 219}},
	{.name="Orchid", .value=(const int [3]){218, 112, 214}},
	{.name="Violet", .value=(const int [3]){238, 130, 238}},
	{.name="Plum", .value=(const int [3]){221, 160, 221}},
	{.name="Thistle", .value=(const int [3]){216, 191, 216}},
	{.name="Lavender", .value=(const int [3]){230, 230, 250}},
	{.name="MidnightBlue", .value=(const int [3]){25, 25, 112}},
	{.name="Navy", .value=(const int [3]){0, 0, 128}},
	{.name="DarkBlue", .value=(const int [3]){0, 0, 139}},
	{.name="MediumBlue", .value=(const int [3]){0, 0, 205}},
	{.name="Blue", .value=(const int [3]){0, 0, 255}},
	{.name="RoyalBlue", .value=(const int [3]){65, 105, 225}},
	{.name="SteelBlue", .value=(const int [3]){70, 130, 180}},
	{.name="DodgerBlue", .value=(const int [3]){30, 144, 255}},
	{.name="DeepSkyBlue", .value=(const int [3]){0, 191, 255}},
	{.name="CornflowerBlue", .value=(const int [3]){100, 149, 237}},
	{.name="SkyBlue", .value=(const int [3]){135, 206, 235}},
	{.name="LightSkyBlue", .value=(const int [3]){135, 206, 250}},
	{.name="LightSteelBlue", .value=(const int [3]){176, 196, 222}},
	{.name="LightBlue", .value=(const int [3]){173, 216, 230}},
	{.name="PowderBlue", .value=(const int [3]){176, 224, 230}},
	{.name="Teal", .value=(const int [3]){0, 128, 128}},
	{.name="DarkCyan", .value=(const int [3]){0, 139, 139}},
	{.name="LightSeaGreen", .value=(const int [3]){32, 178, 170}},
	{.name="CadetBlue", .value=(const int [3]){95, 158, 160}},
	{.name="DarkTurquoise", .value=(const int [3]){0, 206, 209}},
	{.name="MediumTurquoise", .value=(const int [3]){72, 209, 204}},
	{.name="Turquoise", .value=(const int [3]){64, 224, 208}},
	{.name="Aqua", .value=(const int [3]){0, 255, 255}},
	{.name="Cyan", .value=(const int [3]){0, 255, 255}},
	{.name="Aquamarine", .value=(const int [3]){127, 255, 212}},
	{.name="PaleTurquoise", .value=(const int [3]){175, 238, 238}},
	{.name="LightCyan", .value=(const int [3]){224, 255, 255}},
	{.name="DarkGreen", .value=(const int [3]){0, 100, 0}},
	{.name="Green", .value=(const int [3]){0, 128, 0}},
	{.name="DarkOliveGreen", .value=(const int [3]){85, 107, 47}},
	{.name="ForestGreen", .value=(const int [3]){34, 139, 34}},
	{.name="SeaGreen", .value=(const int [3]){46, 139, 87}},
	{.name="Olive", .value=(const int [3]){128, 128, 0}},
	{.name="OliveDrab", .value=(const int [3]){107, 142, 35}},
	{.name="MediumSeaGreen", .value=(const int [3]){60, 179, 113}},
	{.name="LimeGreen", .value=(const int [3]){50, 205, 50}},
	{.name="Lime", .value=(const int [3]){0, 255, 0}},
	{.name="SpringGreen", .value=(const int [3]){0, 255, 127}},
	{.name="MediumSpringGreen", .value=(const int [3]){0, 250, 154}},
	{.name="DarkSeaGreen", .value=(const int [3]){143, 188, 143}},
	{.name="MediumAquamarine", .value=(const int [3]){102, 205, 170}},
	{.name="YellowGreen", .value=(const int [3]){154, 205, 50}},
	{.name="LawnGreen", .value=(const int [3]){124, 252, 0}},
	{.name="Chartreuse", .value=(const int [3]){127, 255, 0}},
	{.name="LightGreen", .value=(const int [3]){144, 238, 144}},
	{.name="GreenYellow", .value=(const int [3]){173, 255, 47}},
	{.name="PaleGreen", .value=(const int [3]){152, 251, 152}},
	{.name="MistyRose", .value=(const int [3]){255, 228, 225}},
	{.name="AntiqueWhite", .value=(const int [3]){250, 235, 215}},
	{.name="Linen", .value=(const int [3]){250, 240, 230}},
	{.name="Beige", .value=(const int [3]){245, 245, 220}},
	{.name="WhiteSmoke", .value=(const int [3]){245, 245, 245}},
	{.name="LavenderBlush", .value=(const int [3]){255, 240, 245}},
	{.name="OldLace", .value=(const int [3]){253, 245, 230}},
	{.name="AliceBlue", .value=(const int [3]){240, 248, 255}},
	{.name="Seashell", .value=(const int [3]){255, 245, 238}},
	{.name="GhostWhite", .value=(const int [3]){248, 248, 255}},
	{.name="Honeydew", .value=(const int [3]){240, 255, 240}},
	{.name="FloralWhite", .value=(const int [3]){255, 250, 240}},
	{.name="Azure", .value=(const int [3]){240, 255, 255}},
	{.name="MintCream", .value=(const int [3]){245, 255, 250}},
	{.name="Snow", .value=(const int [3]){255, 250, 250}},
	{.name="Ivory", .value=(const int [3]){255, 255, 240}},
	{.name="White", .value=(const int [3]){255, 255, 255}},
	{.name="Black", .value=(const int [3]){0, 0, 0}},
	{.name="DarkSlateGray", .value=(const int [3]){47, 79, 79}},
	{.name="DimGray", .value=(const int [3]){105, 105, 105}},
	{.name="SlateGray", .value=(const int [3]){112, 128, 144}},
	{.name="Gray", .value=(const int [3]){128, 128, 128}},
	{.name="LightSlateGray", .value=(const int [3]){119, 136, 153}},
	{.name="DarkGray", .value=(const int [3]){169, 169, 169}},
	{.name="Silver", .value=(const int [3]){192, 192, 192}},
	{.name="LightGray", .value=(const int [3]){211, 211, 211}},
	{.name="Gainsboro", .value=(const int [3]){220, 220, 220}}
};

inline static int strncmp_no_alpha(char *s1, char *s2, size_t length)
{
	size_t i = 0;
	while ((s1[i] || s2[i]) && i < length)
	{
		char c1 = s1[i];
		char c2 = s2[i];

		if (c1 >= 'A' && c1 <= 'Z')
			c1 += 'a' - 'A';
		if (c2 >= 'A' && c2 <= 'Z')
			c2 += 'a' - 'A';

		if (c1 != c2)
			return ((unsigned char)c1 - (unsigned char)c2);

		i++;
	}

	return (0);
}

inline static	uint32_t	get_html_color_from_name(const char *name, size_t name_length)
{
	for (int i=0; i<HTML_COLORS_COUNT; i++)
	{
		if (!strncmp_no_alpha((char *)name, (char *)html_colors_list[i].name, name_length))
		{
			int r = (int)html_colors_list[i].value[0];
			int g = (int)html_colors_list[i].value[1];
			int b = (int)html_colors_list[i].value[2];
			return (0xff000000u | (b << 16) | (g << 8) | r);
		}
	}
	return 0x00000000;
}

#endif