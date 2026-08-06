.global enable_irq
.type enable_irq, %function

.global disable_irq
.type disable_irq, %function

enable_irq:
	mrs	r0, cpsr        @ read CPSR
	bic	r0, r0, #0x80   @ clear I bit (enable IRQ)
	msr	cpsr_c, r0      @ write back
	bx	lr


disable_irq:
	mrs	r0, cpsr
	orr	r0, r0, #0x80
	msr	cpsr_c, r0
	bx	lr
