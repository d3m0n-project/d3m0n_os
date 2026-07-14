#include "filesystem.h"
#include "peripheral.h"
#include "time.h"

#define EMMC_BASE		(PERIPHERAL_BASE + 0x300000)

#define CMD0			0
#define CMD2			2
#define CMD3			3
#define CMD7			7
#define CMD8			8
#define CMD12			12
#define CMD16			16
#define CMD17			17
#define CMD18			18
#define CMD23			23
#define CMD24			24
#define CMD25			25
#define CMD55			55
#define ACMD41			41
#define ACMD23			23

#define INT_CMD_DONE	(1 << 0)
#define INT_DATA_DONE	(1 << 1)
#define INT_WRITE_RDY	(1 << 4)
#define INT_READ_RDY	(1 << 5)
#define INT_ERROR_MASK	0xFFFF0000

#define SR_CMD_INHIBIT	(1 << 0)
#define SR_DAT_INHIBIT	(1 << 1)
#define SR_READ_AVAIL	(1 << 11)
#define SR_WRITE_AVAIL	(1 << 10)

#define C1_CLK_INTLEN	(1 << 0)
#define C1_CLK_STABLE	(1 << 1)
#define C1_CLK_EN		(1 << 2)

#define C1_SRST_HC		(1 << 24)
#define C1_SRST_CMD		(1 << 25)
#define C1_SRST_DATA	(1 << 26)

#define CMD_INDEX(a)	((a) << 24)

#define CMD_RSPNS_NONE	(0 << 16)
#define CMD_RSPNS_136	(1 << 16)
#define CMD_RSPNS_48	(2 << 16)
#define CMD_RSPNS_48B	(3 << 16)

#define CMD_CRCCHK_EN	(1 << 19)
#define CMD_IXCHK_EN	(1 << 20)
#define CMD_ISDATA		(1 << 21)

#define TM_DMA_EN		(1 << 0)
#define TM_BLKCNT_EN	(1 << 1)
#define TM_AUTO_CMD12	(1 << 2)
#define TM_DAT_DIR_READ	(1 << 4)

#define EMMC_TIMEOUT	1000000U


typedef struct
{
	volatile uint32_t	ARG2;
	volatile uint32_t	BLKSIZECNT;
	volatile uint32_t	ARG1;
	volatile uint32_t	CMDTM;
	volatile uint32_t	RESP0;
	volatile uint32_t	RESP1;
	volatile uint32_t	RESP2;
	volatile uint32_t	RESP3;
	volatile uint32_t	DATA;
	volatile uint32_t	STATUS;
	volatile uint32_t	CONTROL0;
	volatile uint32_t	CONTROL1;
	volatile uint32_t	INTERRUPT;
	volatile uint32_t	IRPT_MASK;
	volatile uint32_t	IRPT_EN;
	volatile uint32_t	CONTROL2;
}	emmc_regs_t;


typedef enum
{
	RESP_NONE,
	RESP_48,
	RESP_136,
	RESP_48_BUSY
}	response_type_t;


static volatile emmc_regs_t	*emmc = (volatile emmc_regs_t *)EMMC_BASE;

static uint32_t	rca;
static uint32_t	is_sdhc;


static int	send_cmd(uint32_t cmd, uint32_t arg, response_type_t resp);


static inline void	mmio_barrier(void)
{
	for (volatile int i = 0; i < 1; i++)
		;
}



static void	short_delay(void)
{
	for (volatile int i = 0; i < 10000; i++)
		continue;
}


static int	wait_interrupt(uint32_t mask, uint32_t timeout)
{
	while (timeout--)
	{
		uint32_t status = emmc->INTERRUPT;

		if (status & INT_ERROR_MASK)
		{
			emmc->INTERRUPT = status;
			return -1;
		}

		if (status & mask)
		{
			emmc->INTERRUPT = mask;
			return 0;
		}
	}

	return -1;
}


static int	wait_status_clear(uint32_t mask, uint32_t timeout)
{
	while (timeout--)
	{
		if (!(emmc->STATUS & mask))
			return 0;
	}

	return -1;
}


static int	wait_status_set(uint32_t mask, uint32_t timeout)
{
	while (timeout--)
	{
		if (emmc->STATUS & mask)
			return 0;
	}

	return -1;
}


static int	wait_control1_clear(uint32_t mask, uint32_t timeout)
{
	while (timeout--)
	{
		if (!(emmc->CONTROL1 & mask))
			return 0;
	}

	return -1;
}


static int	wait_control1_set(uint32_t mask, uint32_t timeout)
{
	while (timeout--)
	{
		if (emmc->CONTROL1 & mask)
			return 0;
	}

	return -1;
}


static int	sd_reset_cmd(void)
{
	emmc->CONTROL1 |= C1_SRST_CMD;
	return wait_control1_clear(C1_SRST_CMD, EMMC_TIMEOUT);
}


static int	sd_reset_data(void)
{
	emmc->CONTROL1 |= C1_SRST_DATA;
	return wait_control1_clear(C1_SRST_DATA, EMMC_TIMEOUT);
}


static int	wait_cmd_inhibit(void)
{
	return wait_status_clear(SR_CMD_INHIBIT, EMMC_TIMEOUT);
}


static int	wait_dat_inhibit(void)
{
	return wait_status_clear(SR_DAT_INHIBIT, EMMC_TIMEOUT);
}


static int	sd_set_clock(uint32_t divider)
{
	uint32_t	c1;
	uint32_t	freq_select;
	uint32_t	upper_bits;


	emmc->CONTROL1 &= ~C1_CLK_EN;

	short_delay();

	freq_select = divider & 0xFF;
	upper_bits = (divider >> 8) & 0x3;
	c1 = emmc->CONTROL1;
	c1 &= ~(0xFFF << 0);
	c1 |= freq_select << 8;
	c1 |= upper_bits << 6;
	c1 |= C1_CLK_INTLEN;

	emmc->CONTROL1 = c1;
	mmio_barrier();
	if (wait_control1_set(C1_CLK_STABLE, EMMC_TIMEOUT) < 0)
		return -1;


	emmc->CONTROL1 |= C1_CLK_EN;
	mmio_barrier();
	short_delay();
	return 0;
}


static int	send_cmd(uint32_t cmd, uint32_t arg, response_type_t resp)
{
	uint32_t cmdtm = 0;


	if (wait_cmd_inhibit() < 0)
		return -1;

	if (cmd == CMD17 || cmd == CMD18 || cmd == CMD24 || cmd == CMD25)
	{
		if (wait_dat_inhibit() < 0)
			return -1;
	}
	emmc->INTERRUPT = 0xFFFFFFFF;
	emmc->ARG1 = arg;
	cmdtm |= CMD_INDEX(cmd);
	switch (resp)
	{
		case RESP_NONE:
			cmdtm |= CMD_RSPNS_NONE;
			break;

		case RESP_48:
			cmdtm |= CMD_RSPNS_48;
			break;

		case RESP_136:
			cmdtm |= CMD_RSPNS_136;
			break;

		case RESP_48_BUSY:
			cmdtm |= CMD_RSPNS_48B;
			break;
	}


	if (cmd != CMD0 && cmd != ACMD41)
		cmdtm |= CMD_CRCCHK_EN;

	if (cmd != CMD0 && cmd != CMD2 && cmd != ACMD41)
		cmdtm |= CMD_IXCHK_EN;

	if (cmd == CMD17 || cmd == CMD18 || cmd == CMD24 || cmd == CMD25)
	{
		cmdtm |= CMD_ISDATA;
		cmdtm |= TM_BLKCNT_EN;

		if (cmd == CMD18 || cmd == CMD25)
			cmdtm |= TM_AUTO_CMD12;
	}


	if (cmd == CMD17 || cmd == CMD18)
		cmdtm |= TM_DAT_DIR_READ;


	mmio_barrier();
	emmc->CMDTM = cmdtm;
	if (wait_interrupt(INT_CMD_DONE, EMMC_TIMEOUT) < 0)
	{
		sd_reset_cmd();
		return -1;
	}


	return 0;
}


int	sd_init()
{
	uint32_t	timeout;
	uint32_t	app_rca = 0;

	rca = 0;
	is_sdhc = 0;
	// reset controller
	emmc->CONTROL1 |= C1_SRST_HC;

	if (wait_control1_clear(C1_SRST_HC, EMMC_TIMEOUT) < 0)
		return -1;


	short_delay();

	emmc->CONTROL0 = 0;

	// data timeout
	emmc->CONTROL1 |= (0xE << 16);

	// identification clock
	if (sd_set_clock(148) < 0)
		return -1;


	emmc->IRPT_EN = 0xFFFFFFFF;
	emmc->IRPT_MASK = 0xFFFFFFFF;
	emmc->INTERRUPT = 0xFFFFFFFF;


	// GO_IDLE_STATE
	if (send_cmd(CMD0, 0, RESP_NONE) < 0)
		return -1;


	short_delay();

	// SD 2.0 check
	if (send_cmd(CMD8, 0x1AA, RESP_48) < 0)
		return -1;

	if ((emmc->RESP0 & 0xFFF) != 0x1AA)
		return -1;

	timeout = 1000;
	while (timeout--)
	{
		if (send_cmd(CMD55, app_rca, RESP_48) < 0)
			continue;


		// ACMD41
		if (send_cmd(ACMD41, 0x40300000, RESP_48) < 0)
			continue;

		if (emmc->RESP0 & (1 << 31))
		{
			if (emmc->RESP0 & (1 << 30))
				is_sdhc = 1;

			break;
		}
		for (volatile int i = 0; i < 100000; i++)
			continue;
	}
	if (timeout == 0)
		return -1;

	// CID
	if (send_cmd(CMD2, 0, RESP_136) < 0)
		return -1;


	// get RCA
	if (send_cmd(CMD3, 0, RESP_48) < 0)
		return -1;

	rca = emmc->RESP0 >> 16;

	// select card
	if (send_cmd(CMD7, rca << 16, RESP_48_BUSY) < 0)
		return -1;

	// wait card ready
	if (wait_dat_inhibit() < 0)
		return -1;

	for (volatile int i = 0; i < 500000; i++)
		continue;

	// standard capacity card
	if (!is_sdhc)
	{
		if (send_cmd(
			CMD16,
			SECTOR_SIZE,
			RESP_48) < 0)
			return -1;
	}

	// safe speed: ~25MHz
	if (sd_set_clock(4) < 0)
		return -1;

	return 0;
}

static void	copy_from_fifo(uint8_t *buffer)
{
	volatile uint32_t *dst32 = (volatile uint32_t *)buffer;
	for (int i = 0; i < 128; i++)
		dst32[i] = emmc->DATA;
}


static void	copy_to_fifo(const uint8_t *buffer)
{
	// Faster RAM -> FIFO copy: use 32-bit loads.
	const uint32_t *src32 = (const uint32_t *)buffer;
	for (int i = 0; i < 128; i++)
		emmc->DATA = src32[i];
}



int	block_read_multi(uint32_t lba, uint32_t count, uint8_t *buffer)
{
	uint32_t	addr;


	if (!buffer || count == 0)
		return -1;


	addr = is_sdhc?lba:lba * SECTOR_SIZE;


	emmc->BLKSIZECNT = (count << 16) | SECTOR_SIZE;


	if (send_cmd(CMD18, addr, RESP_48) < 0)
		return -1;

	for (uint32_t block = 0; block < count; block++)
	{
		if (wait_status_set(SR_READ_AVAIL, EMMC_TIMEOUT) < 0)
		{
			sd_reset_data();
			return -1;
		}
		copy_from_fifo(buffer);
		buffer += SECTOR_SIZE;
	}



	if (wait_interrupt(INT_DATA_DONE, EMMC_TIMEOUT) < 0)
	{
		sd_reset_data();
		return -1;
	}
	return 0;
}

int	block_write_multi(uint32_t lba, uint32_t count, const uint8_t *buffer)
{
	uint32_t	addr;


	if (!buffer || count == 0)
		return -1;

	addr = is_sdhc?lba:lba * SECTOR_SIZE;


	//ACMD23=block count
	if (send_cmd(CMD55, rca << 16, RESP_48) < 0)
		return -1;


	if (send_cmd(ACMD23, count, RESP_48) < 0)
		return -1;

	emmc->BLKSIZECNT = (count << 16) | SECTOR_SIZE;
	if (send_cmd(CMD25, addr, RESP_48) < 0)
		return -1;

	for (uint32_t block = 0; block < count; block++)
	{
		if (wait_status_set(SR_WRITE_AVAIL, EMMC_TIMEOUT) < 0)
		{
			sd_reset_data();
			return -1;
		}
		copy_to_fifo(buffer);
		buffer += SECTOR_SIZE;
	}


	if (wait_interrupt(INT_DATA_DONE, EMMC_TIMEOUT) < 0)
	{
		sd_reset_data();
		return -1;
	}


	return 0;
}
