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


irq_handler:
    @ IRQ return address correction
    sub lr, lr, #4


    @ Save interrupted context
    stmfd sp!, {r0-r12,lr}

    mrs r0, spsr
    stmfd sp!, {r0}


    ldr r1, =current_process
    ldr r1, [r1]


    @Save IRQ frame
    str sp, [r1,#0]



    @ USER processes
    ldr r2, [r1,#16]       @ mode

    cmp r2, #0             @ PROCESS_KERNEL
    beq no_save_user_stack



    @ switch to SYS mode
    mrs r2,cpsr
    bic r2,r2,#0x1f
    orr r2,r2,#0x1f        @ SYS
    msr cpsr_c,r2


    @ Save user stack
    mov r3,sp
    str r3,[r1,#4]


    @ return to IRQ mode
    msr cpsr_c,#0xD2


no_save_user_stack:
    bl irq_dispatch

    ldr r1, =current_process
    ldr r1, [r1]

	@ restore IRQ frame
    ldr sp,[r1,#0]

	@ restore SPSR
    ldmfd sp!,{r0}
    msr spsr_cxsf,r0



    @ USER processes
    ldr r2,[r1,#16]

    cmp r2,#0
    beq no_restore_user_stack


    @ switch SYS mode
    msr cpsr_c,#0xDF


    @ restore user stack
    ldr r0,[r1,#4]
    mov sp,r0



    @ back IRQ
    msr cpsr_c,#0xD2


no_restore_user_stack:
    @ restore regs
    ldmfd sp!,{r0-r12,lr}
    subs pc,lr,#0

fiq_handler:
	stmfd	sp!, {r0-r12, lr}
	mov	r0, sp
	mov	r1, #6
	bl	kernel_panic
	b	.
