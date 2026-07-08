#include "log.h"
#include "libft.h"

typedef struct
{
	uint32_t r[13];
	uint32_t pc;
}	ExceptionFrame;

const char	*get_exception_name(uint32_t id)
{
	switch (id)
	{
		case 0:
			return "UNDEFINED INSTRUCTION";
		case 1:
			return "SWI";
		case 2:
			return "PREFETCH ABORT";
		case 3:
			return "DATA ABORT";
		case 4:
			return "RESERVED";
		case 5:
			return "IRQ";

		case 6:
			return "FIQ";
	}
	return "UNKNOWN";
}

static inline	uint32_t read_dfar(void)
{
	uint32_t value;

	asm volatile(
		"mrc p15, 0, %0, c6, c0, 0"
		: "=r"(value)
	);

	return value;
}

void	kernel_panic(ExceptionFrame *frame, uint32_t exception_id)
{
	log("=== KERNEL PANIC ===\n", LOG_ERROR);
	log("PC:               0x%X\n", LOG_ERROR | LOG_INDENT, frame->pc);
	log("TYPE:             %s\n", LOG_ERROR | LOG_INDENT, get_exception_name(exception_id));

	if (exception_id == 3)
	{
		uint32_t fault_addr = read_dfar();
		log("FAULT ADDR:       0x%X\n", LOG_ERROR | LOG_INDENT, fault_addr);
	}


	uart_print("\n");

	#if DEBUG == 1
	log("DEBUG TIPS: Use this command to retrieve which line in the code triggered this crash:\n    >> arm-none-eabi-addr2line -e obj/kernel.elf 0x%X\n", LOG_INFO, frame->pc);
	#endif

	while (1)
		asm volatile("wfi");
}