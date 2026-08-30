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

.macro UART_CHAR char
    push {r0, r1}
    ldr r0, =0x20201000
    mov r1, #\char
    str r1, [r0, #0]
    pop {r0, r1}
.endm

irq_handler:
    sub lr, lr, #4

    @ save lr_irq (interrupted PC) and spsr into IRQ-mode registers temporarily
    @ we have lr_irq and spsr available here

    @ switch to SYS mode to access lr_usr
    @ but first save the IRQ context on IRQ stack
    stmfd sp!, {r0-r12, lr}    @ save interrupted regs + lr_irq onto sp_irq

    mrs r0, spsr
    stmfd sp!, {r0}             @ save SPSR onto sp_irq

    ldr r1, =current_process
    ldr r1, [r1]

    str sp, [r1, #0]            @ save irq_sp

    @ save native sp and lr_usr (user) or sp_svc (kernel)
    ldr r2, [r1, #16]           @ mode
    cmp r2, #0
    beq 1f

    @ user process: save sp_usr and lr_usr via SYS mode
    msr cpsr_c, #0xDF           @ SYS mode, IRQs disabled
    str sp, [r1, #4]            @ save sp_usr
    @ push lr_usr onto the user's own stack so it's preserved
    stmfd sp!, {lr}
    str sp, [r1, #4]            @ update user_sp to include saved lr_usr
    msr cpsr_c, #0xD2           @ back to IRQ
    b 2f

1:  @ kernel process: save sp_svc
    msr cpsr_c, #0xD3           @ SVC mode, IRQs disabled
    str sp, [r1, #4]            @ save sp_svc
    msr cpsr_c, #0xD2           @ back to IRQ

2:
    bl irq_dispatch

    ldr r1, =current_process
    ldr r1, [r1]

    @ restore native sp and lr_usr
    ldr r2, [r1, #16]           @ mode
    cmp r2, #0
    beq 3f

    @ user process: restore sp_usr and lr_usr
    msr cpsr_c, #0xDF
    ldr sp, [r1, #4]            @ restore sp_usr (points at saved lr_usr)
    ldmfd sp!, {lr}             @ pop lr_usr
    str sp, [r1, #4]            @ restore user_sp to pre-lr_usr-push value
    msr cpsr_c, #0xD2
    b 4f

3:  @ kernel process: restore sp_svc
    msr cpsr_c, #0xD3
    ldr sp, [r1, #4]
    msr cpsr_c, #0xD2

4:
    ldr sp, [r1, #0]            @ restore irq_sp
    ldmfd sp!, {r0}
    msr spsr_cxsf, r0
    ldmfd sp!, {r0-r12, lr}
    subs pc, lr, #0

fiq_handler:
	stmfd	sp!, {r0-r12, lr}
	mov	r0, sp
	mov	r1, #6
	bl	kernel_panic
	b	.
