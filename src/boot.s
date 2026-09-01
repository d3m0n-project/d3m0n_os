.section .vectors, "ax"
.align 5
.global _vectors

_vectors:
	ldr pc, [pc,#0x18]
	ldr pc, [pc,#0x18]
	ldr pc, [pc,#0x18]
	ldr pc, [pc,#0x18]
	ldr pc, [pc,#0x18]
	ldr pc, [pc,#0x18]
	ldr pc, [pc,#0x18]
	ldr pc, [pc,#0x18]

	.align 2
	.word _start
	.word undefined_handler
	.word swi_handler
	.word prefetch_abort_handler
	.word data_abort_handler
	.word reserved_handler
	.word irq_handler
	.word fiq_handler

.size _vectors, .-_vectors


.section .text.boot
.global _start

.extern kernel_main
.extern kernel_panic
.extern __bss_start
.extern __bss_end
.extern stack_top
.extern _vectors

.global irq_handler
.type irq_handler, %function
.extern current_process
.extern process_context_valid

.extern swi_handler


_start:
	# set supervisor mode, disable interrupts
	msr	cpsr_c, #0xD3


	# copy exception vectors to 0x00000000
	ldr r0, =_vectors
	mov r1, #0
	mov r2, #64

copy_vectors:
	ldrb r3, [r0], #1
	strb r3, [r1], #1
	subs r2, r2, #1
	bne copy_vectors

stack_setup:
	ldr	r0, =stack_top

vector_table_defs:
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
    sub lr, lr, #4
    stmfd sp!, {r0-r12,lr}

    mrs r0, spsr
    stmfd sp!, {r0}

    ldr r1, =current_process
    ldr r1, [r1]


    mov r3, sp
    ldr r2, [r1,#0]
    mov r4, r2
    mov r5, #15
1:
    ldr r6, [r3], #4
    str r6, [r4], #4
    subs r5, r5, #1
    bne 1b
    add sp, sp, #60

    ldr r2, [r1,#20]           @ mode
    cmp r2, #0                 @ PROCESS_KERNEL
    ldreq r3, =0x13            @ SVC
    ldrne r3, =0x1F            @ SYS

    mrs r2, cpsr
    bic r2, r2, #0x1F
    orr r2, r2, r3
    msr cpsr_c, r2

    mov r3, sp
    str r3, [r1,#4]            @ save sp
    str lr, [r1,#8]            @ save lr

    msr cpsr_c, #0xD2          @ back to IRQ

    bl irq_dispatch

    ldr r1, =current_process
    ldr r1, [r1]

    mov r0, r1
    bl process_context_valid
    cmp r0, #0
    bne 2f
    ldr r0, =bad_process_context
    bl panic
    b .

2:
    ldr r1, =current_process
    ldr r1, [r1]

    ldr sp, [r1,#0]
    ldmfd sp!, {r0}
    msr spsr_cxsf, r0

    ldr r2, [r1,#20]
    cmp r2, #0
    ldreq r3, =0x13
    ldrne r3, =0x1F

    mrs r0, cpsr
    bic r0, r0, #0x1F
    orr r0, r0, r3
    msr cpsr_c, r0

    ldr r0, [r1,#4]
    mov sp, r0
    ldr r0, [r1,#8]
    mov lr, r0

    msr cpsr_c, #0xD2

    ldmfd sp!, {r0-r12,lr}
    ldr sp, =stack_top
    sub sp, sp, #0x1000
    subs pc, lr, #0

.section .rodata
bad_process_context:
    .asciz "SCHEDULER: invalid process context\n"

fiq_handler:
	stmfd	sp!, {r0-r12, lr}
	mov	r0, sp
	mov	r1, #6
	bl	kernel_panic
	b	.
