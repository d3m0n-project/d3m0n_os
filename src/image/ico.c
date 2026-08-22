#include "ico.h"

typedef struct
{
	uint16_t	reserved;
	uint16_t	type;
	uint16_t	count;
}	ICONDIR;

typedef struct
{
	uint8_t		width;
	uint8_t		height;
	uint8_t		color_count;
	uint8_t		reserved;
	uint16_t	planes;
	uint16_t	bit_count;
	uint32_t	bytes_in_resource;
	uint32_t	image_offset;
}	ICONDIRENTRY;

int	load_ico_from_buff(char *raw, size_t size, ICO_icon *out)
{
	ICONDIR dir = {0};
	if (!raw || !out)
		goto invalid_ico;

	if (size < sizeof(ICONDIR))
		goto invalid_ico;

	ft_memcpy(&dir, raw, sizeof(dir));
	if (dir.reserved != 0)
		goto invalid_ico;

	if (dir.type != 1)
		goto invalid_ico;

	if (dir.count == 0)
		goto invalid_ico;

	if (dir.count > (size - sizeof(ICONDIR)) / sizeof(ICONDIRENTRY))
		goto invalid_ico;

	ICONDIRENTRY *entries = (ICONDIRENTRY *)(raw + sizeof(ICONDIR));
	ICO_icon best = {0};
	uint32_t best_area = 0;
	int found = 0;

	for (uint16_t i = 0; i < dir.count; i++)
	{
		ICONDIRENTRY entry = {0};
		ft_memcpy(&entry, &entries[i], sizeof(entry));

		uint32_t width = entry.width ? entry.width : 256;
		uint32_t height = entry.height ? entry.height : 256;

		if (entry.reserved != 0)
			continue;

		if (entry.bytes_in_resource == 0)
			continue;

		if (entry.image_offset > size)
			continue;

		if (entry.bytes_in_resource > size - entry.image_offset)
			continue;

		uint32_t area = width * height;
		if (!found || area > best_area)
		{
			best.pixels = 0;
			best.width = width;
			best.height = height;
			best_area = area;
			found = 1;

			if (load_dib_from_buff(
				raw + entry.image_offset,
				entry.bytes_in_resource,
				&best) != 0)
				goto invalid_ico;
		}
	}

	if (!found)
		goto invalid_ico;

	*out = best;
	log("ICO: %lux%lu\n", 0, best.width, best.height);
	return 0;

invalid_ico:
	log("ICO: Invalid ICO, could not parse!\n", LOG_ERROR);
	return 1;
}