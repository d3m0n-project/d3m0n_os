#include "filesystem.h"
#include "peripheral.h"
#include "time.h"

#define EMMC_BASE			(PERIPHERAL_BASE + 0x300000)

#define CMD0				0
#define CMD2				2
#define CMD3				3
#define CMD7				7
#define CMD8				8
#define CMD16				16
#define CMD17				17
#define CMD24				24
#define CMD55				55
#define ACMD41				41

#define INT_CMD_DONE		(1 << 0)
#define INT_DATA_DONE		(1 << 1)
#define INT_WRITE_RDY		(1 << 4)
#define INT_READ_RDY		(1 << 5)
#define INT_ERROR_MASK		0xFFFF0000

#define SR_CMD_INHIBIT		(1 << 0)
#define SR_DAT_INHIBIT		(1 << 1)

#define C1_CLK_INTLEN		(1 << 0)
#define C1_CLK_STABLE		(1 << 1)
#define C1_CLK_EN			(1 << 2)
#define C1_SRST_HC			(1 << 24)

#define CMD_INDEX(idx)		((idx) << 24)
#define CMD_RSPNS_NONE		(0 << 16)
#define CMD_RSPNS_136		(1 << 16)
#define CMD_RSPNS_48		(2 << 16)
#define CMD_RSPNS_48B		(3 << 16)
#define CMD_CRCCHK_EN		(1 << 19)
#define CMD_IXCHK_EN		(1 << 20)
#define CMD_ISDATA			(1 << 21)

#define TM_BLKCNT_EN		(1 << 1)
#define TM_DAT_DIR_READ		(1 << 4)

typedef struct emmc_registers
{
	volatile uint32_t EMMC_ARG2;
	volatile uint32_t EMMC_BLKSIZECNT;
	volatile uint32_t EMMC_ARG1;
	volatile uint32_t EMMC_CMDTM;
	volatile uint32_t EMMC_RESP0;
	volatile uint32_t EMMC_RESP1;
	volatile uint32_t EMMC_RESP2;
	volatile uint32_t EMMC_RESP3;
	volatile uint32_t EMMC_DATA;
	volatile uint32_t EMMC_STATUS;
	volatile uint32_t EMMC_CONTROL0;
	volatile uint32_t EMMC_CONTROL1;
	volatile uint32_t EMMC_INTERRUPT;
	volatile uint32_t EMMC_INT_MASK;
	volatile uint32_t EMMC_INT_EN;
}	emmc_registers;

typedef enum
{
	RESP_NONE,
	RESP_48BITS,
	RESP_136BITS,
	RESP_48BITS_BUSY
}	response_size;

volatile emmc_registers	*emmc_reg = (volatile emmc_registers *)EMMC_BASE;
static uint32_t			rca;
static uint32_t			is_sdhc;

uint32_t wait_cmd_done(uint32_t timeout)
{
	while(!(emmc_reg->EMMC_INTERRUPT & INT_CMD_DONE) && timeout--);
	emmc_reg->EMMC_INTERRUPT = INT_CMD_DONE;
	return timeout;
}

uint32_t	wait_data_done(uint32_t timeout)
{
	while(!(emmc_reg->EMMC_INTERRUPT & INT_DATA_DONE) && timeout--);
	emmc_reg->EMMC_INTERRUPT = INT_DATA_DONE;
	return timeout;
}

static uint32_t wait_interrupt(uint32_t mask, uint32_t timeout)
{
	while (!(emmc_reg->EMMC_INTERRUPT & mask) && timeout--);
	if (emmc_reg->EMMC_INTERRUPT & INT_ERROR_MASK)
	{
		emmc_reg->EMMC_INTERRUPT = INT_ERROR_MASK;
		return 0;
	}
	if (emmc_reg->EMMC_INTERRUPT & mask)
	{
		emmc_reg->EMMC_INTERRUPT = mask;
		return timeout;
	}
	return 0;
}

static int	send_cmd(uint32_t cmd_idx, uint32_t arg, response_size resp_type)
{
	uint32_t cmdtm;

	// wait until CMD line free
	while(emmc_reg->EMMC_STATUS & SR_CMD_INHIBIT) ;
	if (cmd_idx == CMD17 || cmd_idx == CMD24)
		while (emmc_reg->EMMC_STATUS & SR_DAT_INHIBIT) ;

	emmc_reg->EMMC_INTERRUPT = 0xFFFFFFFF;

	emmc_reg->EMMC_ARG1 = arg;

	cmdtm = CMD_INDEX(cmd_idx);
	if (resp_type == RESP_48BITS)
		cmdtm |= CMD_RSPNS_48;
	else if (resp_type == RESP_136BITS)
		cmdtm |= CMD_RSPNS_136;
	else if (resp_type == RESP_48BITS_BUSY)
		cmdtm |= CMD_RSPNS_48B;
	else
		cmdtm |= CMD_RSPNS_NONE;
	if (cmd_idx == CMD17 || cmd_idx == CMD24)
		cmdtm |= CMD_ISDATA;
	if (cmd_idx == CMD17 || cmd_idx == CMD24)
		cmdtm |= TM_BLKCNT_EN;
	if (cmd_idx == CMD17)
		cmdtm |= TM_DAT_DIR_READ;
	if (cmd_idx != CMD0 && cmd_idx != ACMD41)
		cmdtm |= CMD_CRCCHK_EN;
	if (cmd_idx != CMD0 && cmd_idx != CMD2 && cmd_idx != ACMD41)
		cmdtm |= CMD_IXCHK_EN;

	emmc_reg->EMMC_CMDTM = cmdtm;

	if(!wait_cmd_done(1000000))
		return -1;
	if (emmc_reg->EMMC_INTERRUPT & INT_ERROR_MASK)
	{
		emmc_reg->EMMC_INTERRUPT = INT_ERROR_MASK;
		return -1;
	}

	return 0;
}

int	sd_init()
{
	uint32_t	c1;
	int			timeout;

	// reset controller
	emmc_reg->EMMC_CONTROL1 |= C1_SRST_HC;
	while(emmc_reg->EMMC_CONTROL1 & C1_SRST_HC);

	// enable internal clock + timeout
	c1 = emmc_reg->EMMC_CONTROL1;
	c1 &= ~0x0000FFF0;
	c1 |= (0xE << 16);
	c1 |= (0x80 << 8);
	c1 |= C1_CLK_INTLEN;
	emmc_reg->EMMC_CONTROL1 = c1;
	timeout = 1000000;
	while (!(emmc_reg->EMMC_CONTROL1 & C1_CLK_STABLE) && timeout--);
	if (timeout <= 0)
		return -1;

	// enable SD clock
	emmc_reg->EMMC_CONTROL1 |= C1_CLK_EN;

	// enable interrupts
	emmc_reg->EMMC_INT_EN = 0xFFFFFFFF;
	emmc_reg->EMMC_INT_MASK = 0xFFFFFFFF;
	emmc_reg->EMMC_INTERRUPT = 0xFFFFFFFF;

	// reset card
	if(send_cmd(CMD0, 0, RESP_NONE) < 0)
		return -1;
	sleep(1);

	// voltage check
	if(send_cmd(CMD8, 0x1AA, RESP_48BITS) < 0)
		return -1;
	if ((emmc_reg->EMMC_RESP0 & 0xFFF) != 0x1AA)
		return -1;

	// ACMD41 loop
	timeout = 100;
	is_sdhc = 0;
	while(timeout--)
	{
		if(send_cmd(CMD55, 0, RESP_48BITS) < 0)
			continue;
		if(send_cmd(ACMD41, 0x40FF8000, RESP_48BITS) < 0)
			continue;

		if(emmc_reg->EMMC_RESP0 & (1 << 31))
		{
			if (emmc_reg->EMMC_RESP0 & (1 << 30))
				is_sdhc = 1;
			break; // card ready
		}

		usleep(100000);
	}
	if(timeout <= 0)
		return -1;

	// read CID
	if(send_cmd(CMD2, 0, RESP_136BITS) < 0)
		return -1;

	// get RCA
	if(send_cmd(CMD3, 0, RESP_48BITS) < 0)
		return -1;
	rca = emmc_reg->EMMC_RESP0 >> 16;

	// select card
	if(send_cmd(CMD7, rca << 16, RESP_48BITS) < 0)
		return -1;

	// set block length
	if (!is_sdhc)
		if(send_cmd(CMD16, SECTOR_SIZE, RESP_48BITS) < 0)
			return -1;

	return 0;
}

int block_read(uint32_t lba, uint8_t* buffer)
{
	uint32_t addr;

	if (!buffer)
		return -1;
	emmc_reg->EMMC_BLKSIZECNT = (1 << 16) | 512;
	addr = is_sdhc ? lba : (lba * SECTOR_SIZE);

	if(send_cmd(CMD17, addr, RESP_48BITS) < 0)
		return -1;
	if(!wait_interrupt(INT_READ_RDY, 1000000))
		return -1;

	for(int i=0; i<128; i++)
		((uint32_t*)buffer)[i] = emmc_reg->EMMC_DATA;

	if(!wait_data_done(1000000))
		return -1;

	return 0;
}

int block_write(uint32_t lba, const uint8_t* buffer)
{
	uint32_t addr;

	if (!buffer)
		return -1;
	emmc_reg->EMMC_BLKSIZECNT = (1 << 16) | 512;
	addr = is_sdhc ? lba : (lba * SECTOR_SIZE);

	if(send_cmd(CMD24, addr, RESP_48BITS) < 0)
		return -1;
	if(!wait_interrupt(INT_WRITE_RDY, 1000000))
		return -1;

	for(int i=0; i<128; i++)
		emmc_reg->EMMC_DATA = ((uint32_t*)buffer)[i];

	if(!wait_data_done(1000000))
		return -1;

	return 0;
}
