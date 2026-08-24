#include "log.h"
#include "proc.h"
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

static uint32_t	read_dfsr(void)
{
	uint32_t value;

	asm volatile(
		"mrc p15, 0, %0, c5, c0, 0"
		: "=r"(value)
	);

	return value;
}

const char	*dfsr_fault_type(uint32_t dfsr)
{
	uint32_t fs = ((dfsr >> 6) & 0x10) | (dfsr & 0x0f);

	switch (fs) {
	case 0x01:
		return "Alignment fault";

	case 0x02:
		return "Debug event";

	case 0x03:
		return "Access flag fault, section";

	case 0x04:
		return "Fault on instruction cache maintenance";

	case 0x05:
		return "Translation fault, section";

	case 0x06:
		return "Access flag fault, page";

	case 0x07:
		return "Translation fault, page";

	case 0x08:
		return "Precise external abort";

	case 0x09:
		return "Domain fault, section";

	case 0x0a:
		return "External abort on translation, section";

	case 0x0b:
		return "Domain fault, page";

	case 0x0c:
		return "External abort on translation, page";

	case 0x0d:
		return "Permission fault, section";

	case 0x0e:
		return "Permission fault, page";

	case 0x10:
		return "TLB conflict abort";

	case 0x11:
		return "Implementation-defined fault";

	case 0x12:
		return "Implementation-defined fault";

	case 0x13:
		return "Implementation-defined fault";

	case 0x14:
		return "Implementation-defined fault";

	case 0x16:
		return "External abort";

	case 0x18:
		return "Synchronous parity error";

	case 0x19:
		return "Asynchronous parity error";

	case 0x1c:
		return "Synchronous parity error on translation";

	case 0x1e:
		return "External abort on translation";

	default:
		return "Unknown/Reserved fault";
	}
}

void	kernel_panic(ExceptionFrame *frame, uint32_t exception_id)
{
	log("=== KERNEL PANIC ===\n", LOG_ERROR);
	process_list();

	if (current_process)
		check_stack_canary(current_process);

	log("=== PANIC DETAIL ===\n", LOG_ERROR);
	log("PC:                0x%X\n", LOG_ERROR | LOG_INDENT, frame->pc);
	log("TYPE:              %s\n", LOG_ERROR | LOG_INDENT, get_exception_name(exception_id));

	if (exception_id == 3)
	{
		uint32_t fault_addr = read_dfar();
		uint32_t dfsr = read_dfsr();
		log("FAULT ADDR:        0x%X\n", LOG_ERROR | LOG_INDENT, fault_addr);
		log("FAULT TYPE:        %s\n", LOG_ERROR | LOG_INDENT, dfsr_fault_type(dfsr));
	}
	if (exception_id == 0)
	{
		log("INSTRUCTION:       0x%X\n", LOG_ERROR | LOG_INDENT, *(uint32_t *)frame->pc);
	}


	uart_print("\n");

	#if DEBUG == 1
	log("DEBUG TIPS: Use this command to retrieve which line in the code triggered this crash:\n	>> arm-none-eabi-addr2line -e obj/kernel.elf 0x%X\n", LOG_INFO, frame->pc);
	#endif

	while (1)
		asm volatile("wfi");
}