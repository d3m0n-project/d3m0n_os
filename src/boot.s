.section .vectors, "ax"
.align 5
.global _vectors

_vectors:
	ldr	pc, =_start
	ldr	pc, =undefined_handler
	ldr	pc, =swi_handler
	ldr	pc, =prefetch_abort_handler
	ldr	pc, =data_abort_handler
	ldr	pc, =reserved_handler
	ldr	pc, =irq_handler
	ldr	pc, =fiq_handler


.section .text.boot
.global _start

.extern kernel_main
.extern kernel_panic
.extern __bss_start
.extern __bss_end
.extern stack_top
.extern _vectors


_start:
	# set supervisor mode, disable interrupts
	msr	cpsr_c, #0xD3


	# setup base vectors address
	ldr	r0, =_vectors
	mcr	p15, 0, r0, c12, c0, 0


	# setup stacks
	ldr	r0, =stack_top


	/* FIQ */
	msr	cpsr_c, #0xD1
	mov	sp, r0
	sub	r0, r0, #0x1000


	/* IRQ */
	msr	cpsr_c, #0xD2
	mov	sp, r0
	sub	r0, r0, #0x1000


	/* Abort */
	msr	cpsr_c, #0xD7
	mov	sp, r0
	sub	r0, r0, #0x1000


	/* Undefined */
	msr	cpsr_c, #0xDB
	mov	sp, r0
	sub	r0, r0, #0x1000


	/* Supervisor */
	msr	cpsr_c, #0xD3
	mov	sp, r0


	# clear BSS
	ldr	r0, =__bss_start
	ldr	r1, =__bss_end
	mov	r2, #0

bss_loop:
	cmp	r0, r1
	beq	bss_done

	str	r2, [r0], #4
	b	bss_loop


bss_done:
	bl	kernel_main


hang:
	wfe
	b	hang



# exception handlers
undefined_handler:
	stmfd	sp!, {r0-r12, lr}
	mov	r1, #0
	mov	r0, sp
	bl	kernel_panic
	b	.



swi_handler:
	stmfd	sp!, {r0-r12, lr}
	mov	r1, #1
	mov	r0, sp
	bl	kernel_panic
	b	.



prefetch_abort_handler:
	sub	lr, lr, #4

	stmfd	sp!, {r0-r12, lr}
	mov	r1, #2
	mov	r0, sp
	bl	kernel_panic
	b	.



data_abort_handler:
	sub	lr, lr, #8

	stmfd	sp!, {r0-r12, lr}
	mov	r1, #3
	mov	r0, sp
	bl	kernel_panic
	b	.



reserved_handler:
	stmfd	sp!, {r0-r12, lr}
	mov	r1, #4
	mov	r0, sp
	bl	kernel_panic
	b	.



irq_handler:
	stmfd	sp!, {r0-r12, lr}
	mov	r1, #5
	mov	r0, sp
	bl	kernel_panic
	b	.


fiq_handler:
	stmfd	sp!, {r0-r12, lr}
	mov	r0, sp
	mov	r1, #6
	bl	kernel_panic
	b	.
