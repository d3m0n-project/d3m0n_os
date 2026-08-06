.global start_first_process
.type start_first_process,%function


start_first_process:
	ldr sp, [r0]

	ldmia sp!, {r1}
	msr spsr_cxsf, r1

	ldmia sp!, {r0-r12, lr}

	mov pc, lr