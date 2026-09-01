.global swi_handler

.section .text
.extern syscall_handler
.extern current_process
.extern process_exit_current
.extern process_context_valid
.extern panic
.extern stack_top


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
	add sp, sp, #(14*4)          @ discard SWI's frame
	bl process_exit_current

	msr cpsr_c,#0xD2

	ldr r1, =current_process
	ldr r1, [r1]
	mov r0, r1
	bl process_context_valid
	cmp r0, #0
	bne 1f
	ldr r0, =bad_process_context
	bl panic
	b .

1:
	@ reload process
	ldr r1, =current_process
	ldr r1, [r1]
	ldr sp, [r1,#0]
	ldmfd sp!,{r0}
	msr spsr_cxsf,r0

	ldr r2,[r1,#20]
	cmp r2,#0
	ldreq r3,=0xD3
	ldrne r3,=0xDF
	msr cpsr_c,r3
	ldr r0,[r1,#4]
	mov sp,r0
	ldr r0,[r1,#8]
	mov lr,r0
	msr cpsr_c,#0xD2

	ldmfd sp!,{r0-r12,lr}
	ldr sp, =stack_top
	sub sp, sp, #0x1000
	subs pc,lr,#0

.section .rodata
bad_process_context:
	.asciz "PROCESS EXIT: invalid process context\n"
