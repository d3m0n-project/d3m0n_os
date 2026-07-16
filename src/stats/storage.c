#include "stats.h"
#include "filesystem.h"
#include "time.h"

#define BLOCK_COUNT	300

int	get_read_speed(void)
{
	uint8_t		buffer[BLOCK_COUNT * SECTOR_SIZE];
	uint64_t	t = time_us() / 1000;
	block_read_multi(0, BLOCK_COUNT, buffer);
	t = ((time_us() / 1000) - t);
	if (t <= 0)
	{
		log("READ_SPEED: Too small block count to mesure!\n", LOG_ERROR);
		return 0;
	}
	return (1000 * (BLOCK_COUNT * SECTOR_SIZE) / t) * 8 / 1000000;
}