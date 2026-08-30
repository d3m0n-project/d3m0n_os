.global start_first_process
.type start_first_process,%function

start_first_process:
    ldr r1, [r0, #16]           @ mode
    ldr r2, [r0, #4]            @ user_sp / native sp

    cmp r1, #0
    bne 1f

    @ kernel: set sp_svc
    msr cpsr_c, #0xD3
    mov sp, r2
    b 2f

1:  @ user: set sp_usr
    msr cpsr_c, #0xDF
    mov sp, r2

2:
    msr cpsr_c, #0xD2

restore_frame:
    ldr sp, [r0, #0]            @ irq_sp
    ldmfd sp!, {r1}
    msr spsr_cxsf, r1
    ldmfd sp!, {r0-r12, lr}
    subs pc, lr, #0