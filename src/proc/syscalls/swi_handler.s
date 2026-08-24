.global swi_handler

.section .text
.extern syscall_handler
.extern current_process


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
	add sp, sp, #(14*4)          @ discard this SWI's own frame — we never return to it

	bl process_exit_current       @ marks exiting proc ZOMBIE, removes it, sets NEW current_process

	@ current_process is now a DIFFERENT, valid process — safe to restore from here
	msr cpsr_c,#0xD2

	ldr r1, =current_process
	ldr r1, [r1]

	ldr sp, [r1,#0]
	ldmfd sp!,{r0}
	msr spsr_cxsf,r0

	ldr r2,[r1,#16]
	cmp r2,#0
	beq 1f

	msr cpsr_c,#0xDF
	ldr r0,[r1,#4]
	mov sp,r0
	msr cpsr_c,#0xD2

1:
	ldmfd sp!,{r0-r12,lr}
	subs pc,lr,#0