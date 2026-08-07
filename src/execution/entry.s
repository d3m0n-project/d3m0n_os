.global enter_user
.global swi_handler

.data
kernel_return:
	.word 0
kernel_cpsr:
	.word 0

.section .text
.extern syscall_handler

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
	@ restore kernel CPSR
	ldr r0, =kernel_cpsr
	ldr r1, [r0]
	msr cpsr_cxsf, r1


	@ jump back to kernel
	ldr r0, =kernel_return
	ldr lr, [r0]
	bx lr