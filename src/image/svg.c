#include "image/icons.h"
#include "filesystem/filesystem.h"
#include "log.h"

int	svg_read_file(t_svg *output, const char *path)
{
	int			fd;
	size_t		total;
	size_t		amount;
	static char	tmp_buff[4096];

	if (!output || !path)
		return (1);

	output->data = 0;
	total = 0;

	fd = open(path, O_READ);
	if (fd < 0)
	{
		log("ICONS: Could not find svg icon: '%s'\n", LOG_ERROR, path);
		return (1);
	}

	while (1)
	{
		amount = read(fd, tmp_buff, sizeof(tmp_buff));
		if (amount == 0)
			break;
		total += amount;
	}

	close(fd);

	output->data = ft_calloc(total + 1, sizeof(char));
	if (!output->data)
	{
		log("ICONS: Could not allocate memory for svg icon: '%s'\n",
			LOG_ERROR, path);
		return (1);
	}

	fd = open(path, O_READ);
	if (fd < 0)
	{
		kfree(output->data);
		output->data = 0;
		log("ICONS: Could not find svg icon: '%s'\n", LOG_ERROR, path);
		return (1);
	}

	total = 0;
	while (1)
	{
		amount = read(fd, ((char *)output->data) + total, 4096);
		if (amount == 0)
			break;
		total += amount;
	}

	close(fd);
	((uint8_t *)output->data)[total] = '\0';
	output->size = total;
	return (0);
}