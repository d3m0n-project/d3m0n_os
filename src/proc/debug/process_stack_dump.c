#include "proc.h"

typedef struct s_irq_frame
{
	uint32_t	spsr;
	uint32_t	r[13]; // r0->r12
	uint32_t	lr;
}	t_irq_frame;

static const char	*mode_name(uint32_t cpsr_mode)
{
	switch (cpsr_mode & 0x1F)
	{
		case 0x10: return "USR";
		case 0x11: return "FIQ";
		case 0x12: return "IRQ";
		case 0x13: return "SVC";
		case 0x17: return "ABT";
		case 0x1B: return "UND";
		case 0x1F: return "SYS";
		default:   return "???";
	}
}

void process_dump_regs(t_process *p)
{
	t_irq_frame	*f;
	if (!p)
	{
		log("DUMP: null process\n", LOG_ERROR);
		return;
	}

	log("=== Register dump: %s [pid %lu] ===\n", 0, p->proc_name, p->pid);
	log("    mode=%s state=0x%x\n", 0, p->mode ? "USER" : "KERNEL", p->state);
	log("    irq_sp=0x%x  user_sp(native sp)=0x%x\n", 0, p->irq_sp, p->user_sp);
	log("    kernel_stack=0x%x  user_stack=0x%x\n", 0, (uint32_t)p->kernel_stack, (uint32_t)p->user_stack);

	if (!p->irq_sp)
	{
		log("    irq_sp is NULL, cannot read frame\n", LOG_ERROR);
		return;
	}

	uint32_t stack_lo = (uint32_t)p->kernel_stack;
	uint32_t stack_hi = stack_lo + (KERNEL_STACK_PAGES * 4096);
	if (p->irq_sp < stack_lo || p->irq_sp > stack_hi - IRQ_FRAME_SIZE)
		log("    !! irq_sp OUT OF BOUNDS for this process's kernel_stack [0x%x - 0x%x]\n", LOG_ERROR, stack_lo, stack_hi);

	f = (t_irq_frame *)p->irq_sp;
	log("    SPSR = 0x%x  (mode=%s)\n", 0, f->spsr, mode_name(f->spsr));
	log("    LR   = 0x%x  (return addr, already -4 corrected)\n", 0, f->lr);
	log("    native LR = 0x%x\n", 0, p->user_lr);

	log("    r0 =0x%x  r1 =0x%x  r2 =0x%x  r3 =0x%x\n", 0, f->r[0], f->r[1], f->r[2], f->r[3]);
	log("    r4 =0x%x  r5 =0x%x  r6 =0x%x  r7 =0x%x\n", 0, f->r[4], f->r[5], f->r[6], f->r[7]);
	log("    r8 =0x%x  r9 =0x%x  r10=0x%x  r11=0x%x\n", 0, f->r[8], f->r[9], f->r[10], f->r[11]);
	log("    r12=0x%x\n", 0, f->r[12]);
}
