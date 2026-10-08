.global swi_handler

.section .text
.extern syscall_handler
.extern current_process
.extern process_exit_current
.extern process_context_valid
.extern panic
.extern mmu_switch_current
.extern stack_top


swi_handler:
	ldr sp, =stack_top
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

	bl process_exit_current

.Lexit_park:
	wfe
	b .Lexit_park