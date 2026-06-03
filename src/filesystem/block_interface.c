#include "filesystem.h"
#include "peripheral.h"
#include "time.h"

#define EMMC_BASE		(PERIPHERAL_BASE + 0x300000)

#define CMD0			0
#define CMD2			2
#define CMD3			3
#define CMD7			7
#define CMD8			8
#define CMD16			16
#define CMD17			17
#define CMD24			24
#define CMD55			55
#define ACMD41			41

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

#define TM_BLKCNT_EN	(1 << 1)
#define TM_DAT_DIR_READ	(1 << 4)

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

static volatile emmc_regs_t	*emmc = (volatile emmc_regs_t*)EMMC_BASE;

static uint32_t	rca;
static uint32_t	is_sdhc;

static inline	void mmio_barrier()
{
	asm volatile(
		"mcr p15, 0, %0, c7, c10, 5"
		:
		: "r"(0)
		: "memory");
}

static void	short_delay()
{
	for (volatile int i = 0; i < 10000; i++)
		;
}

static int	wait_interrupt(uint32_t mask, uint32_t timeout)
{
	while (timeout--)
	{
		uint32_t irpt = emmc->INTERRUPT;

		if (irpt & INT_ERROR_MASK)
		{
			emmc->INTERRUPT = irpt;
			return -1;
		}

		if (irpt & mask)
		{
			emmc->INTERRUPT = mask;
			return 0;
		}
	}

	return -1;
}

static void sd_reset_cmd()
{
	emmc->CONTROL1 |= C1_SRST_CMD;

	while (emmc->CONTROL1 & C1_SRST_CMD)
		;
}

static void sd_reset_data()
{
	emmc->CONTROL1 |= C1_SRST_DATA;

	while (emmc->CONTROL1 & C1_SRST_DATA)
		;
}

static int wait_cmd_inhibit()
{
	uint32_t timeout = 1000000;

	while ((emmc->STATUS & SR_CMD_INHIBIT) && timeout--)
		;

	return timeout ? 0 : -1;
}

static int wait_dat_inhibit()
{
	uint32_t timeout = 1000000;

	while ((emmc->STATUS & SR_DAT_INHIBIT) && timeout--)
		;

	return timeout ? 0 : -1;
}

static void sd_set_clock(uint32_t divider)
{
	uint32_t c1;
	uint32_t freq_select;
	uint32_t upper_bits;

	emmc->CONTROL1 &= ~C1_CLK_EN;

	short_delay();

	freq_select = divider & 0xFF;
	upper_bits = (divider >> 8) & 0x3;

	c1 = emmc->CONTROL1;

	c1 &= ~0xFFF;

	c1 |= (freq_select << 8);
	c1 |= (upper_bits << 6);

	c1 |= C1_CLK_INTLEN;

	emmc->CONTROL1 = c1;

	short_delay();

	while (!(emmc->CONTROL1 & C1_CLK_STABLE))
		;

	emmc->CONTROL1 |= C1_CLK_EN;

	short_delay();
}

static int send_cmd(uint32_t cmd, uint32_t arg, response_type_t resp)
{
	uint32_t cmdtm = 0;

	if (wait_cmd_inhibit() < 0)
		return -1;

	if (cmd == CMD17 || cmd == CMD24)
	{
		if (wait_dat_inhibit() < 0)
			return -1;
	}

	emmc->INTERRUPT = 0xFFFFFFFF;

	mmio_barrier();

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

	if (cmd != CMD0 &&
		cmd != CMD2 &&
		cmd != ACMD41)
		cmdtm |= CMD_IXCHK_EN;

	if (cmd == CMD17 || cmd == CMD24)
	{
		cmdtm |= CMD_ISDATA;
		cmdtm |= TM_BLKCNT_EN;
	}

	if (cmd == CMD17)
		cmdtm |= TM_DAT_DIR_READ;

	mmio_barrier();

	emmc->CMDTM = cmdtm;

	if (wait_interrupt(INT_CMD_DONE, 1000000) < 0)
	{
		sd_reset_cmd();
		return -1;
	}

	return 0;
}

int sd_init()
{
	uint32_t timeout;
	uint32_t app_rca = 0;

	emmc->CONTROL1 |= C1_SRST_HC;

	while (emmc->CONTROL1 & C1_SRST_HC)
		;

	short_delay();

	emmc->CONTROL0 = 0;

	emmc->CONTROL1 |= (0xE << 16);

	sd_set_clock(148);

	emmc->IRPT_EN = 0xFFFFFFFF;
	emmc->IRPT_MASK = 0xFFFFFFFF;

	emmc->INTERRUPT = 0xFFFFFFFF;

	if (send_cmd(CMD0, 0, RESP_NONE) < 0)
		return -1;

	short_delay();

	if (send_cmd(CMD8, 0x1AA, RESP_48) < 0)
		return -1;

	if ((emmc->RESP0 & 0xFFF) != 0x1AA)
		return -1;

	timeout = 1000;

	while (timeout--)
	{
		if (send_cmd(CMD55, app_rca, RESP_48) < 0)
			continue;

		if (send_cmd(ACMD41, 0x40300000, RESP_48) < 0)
			continue;

		if (emmc->RESP0 & (1 << 31))
		{
			if (emmc->RESP0 & (1 << 30))
				is_sdhc = 1;

			break;
		}

		for (volatile int i = 0; i < 100000; i++)
			;
	}

	if (timeout == 0)
		return -1;

	if (send_cmd(CMD2, 0, RESP_136) < 0)
		return -1;

	if (send_cmd(CMD3, 0, RESP_48) < 0)
		return -1;

	rca = emmc->RESP0 >> 16;

	if (send_cmd(CMD7, rca << 16, RESP_48) < 0)
		return -1;

	for (volatile int i = 0; i < 500000; i++)
		;

	if (!is_sdhc)
	{
		if (send_cmd(CMD16, SECTOR_SIZE, RESP_48) < 0)
			return -1;
	}

	sd_set_clock(4);

	return 0;
}

int block_read(uint32_t lba, uint8_t* buffer)
{
	uint32_t addr;

	if (!buffer)
		return -1;

	addr = is_sdhc
		? lba
		: lba * SECTOR_SIZE;

	emmc->BLKSIZECNT =
		(1 << 16) | SECTOR_SIZE;

	if (send_cmd(
		CMD17,
		addr,
		RESP_48) < 0)
		return -1;

	if (wait_interrupt(
		INT_READ_RDY,
		1000000) < 0)
	{
		sd_reset_data();
		return -1;
	}

	mmio_barrier();

	for (int i = 0; i < 128; i++)
	{
		uint32_t timeout = 1000000;

		while (!(emmc->STATUS & SR_READ_AVAIL) && timeout--)
			;

		if (!timeout)
		{
			sd_reset_data();
			return -1;
		}

		((uint32_t*)buffer)[i] = emmc->DATA;
	}

	if (wait_interrupt(
		INT_DATA_DONE,
		1000000) < 0)
	{
		sd_reset_data();
		return -1;
	}

	return 0;
}

int block_write(uint32_t lba, const uint8_t* buffer)
{
	uint32_t addr;

	if (!buffer)
		return -1;

	addr = is_sdhc
		? lba
		: lba * SECTOR_SIZE;

	emmc->BLKSIZECNT =
		(1 << 16) | SECTOR_SIZE;

	if (send_cmd(
		CMD24,
		addr,
		RESP_48) < 0)
		return -1;

	if (wait_interrupt(
		INT_WRITE_RDY,
		1000000) < 0)
	{
		sd_reset_data();
		return -1;
	}

	mmio_barrier();

	for (int i = 0; i < 128; i++)
	{
		uint32_t timeout = 1000000;

		while (!(emmc->STATUS & SR_WRITE_AVAIL) && timeout--)
			;

		if (!timeout)
		{
			sd_reset_data();
			return -1;
		}

		emmc->DATA =
			((uint32_t*)buffer)[i];
	}

	if (wait_interrupt(
		INT_DATA_DONE,
		1000000) < 0)
	{
		sd_reset_data();
		return -1;
	}

	return 0;
}