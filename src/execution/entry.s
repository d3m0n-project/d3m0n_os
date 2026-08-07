.global enter_user
.global swi_handler

.data
kernel_return:
	.word 0
kernel_cpsr:
	.word 0

.section .text
.extern syscall_handler
.extern current_process

enter_user:
	@ save kernel continuation
	ldr r2, =kernel_return
	str lr, [r2]

	@ save kernel CPSR
	ldr r2, =kernel_cpsr
	mrs r3, cpsr
	str r3, [r2]

	@ user stack
	cps #0x1f
	mov sp, r1
	cps #0x13


	@ enter user mode
	mrs r2, cpsr
	bic r2, r2, #0x1f
	orr r2, r2, #0x10
	msr spsr_cxsf, r2

	mov lr, r0
	movs pc, lr


kernel_resume:
	bx lr


swi_handler:
	stmfd sp!, {r0-r12, lr}

	mov r0, sp
	bl syscall_handler
	mov r12, r0
	cmp r12, #1
	beq exit_user_mode

	ldmfd sp!, {r0-r12, lr}
	movs pc, lr


exit_user_mode:
	add sp, sp, #(14*4)

	msr cpsr_c,#0xD2                 @ switch to IRQ mode

	ldr r1, =current_process
	ldr r1, [r1]

	ldr sp, [r1,#0]                  @ proc->sp (saved IRQ frame)

	ldmfd sp!,{r0}                   @ SPSR
	msr spsr_cxsf,r0

	ldr r2,[r1,#16]                  @ proc->mode
	cmp r2,#0                        @ PROCESS_KERNEL?
	beq 1f

	@ user process
	msr cpsr_c,#0xDF                 @ SYS mode
	ldr r0,[r1,#4]                   @ proc->user_sp
	mov sp,r0
	msr cpsr_c,#0xD2                 @ back to IRQ mode

1:
	ldmfd sp!,{r0-r12,lr}
	subs pc,lr,#0
