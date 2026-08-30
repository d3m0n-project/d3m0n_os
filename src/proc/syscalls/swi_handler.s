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
    add sp, sp, #(14*4)
    bl process_exit_current

    msr cpsr_c, #0xD2

    ldr r1, =current_process
    ldr r1, [r1]

    ldr r2, [r1, #16]           @ mode
    cmp r2, #0
    beq 1f

    @ user: restore sp_usr and lr_usr
    msr cpsr_c, #0xDF
    ldr sp, [r1, #4]
    ldmfd sp!, {lr}             @ pop saved lr_usr
    str sp, [r1, #4]
    msr cpsr_c, #0xD2
    b 2f

1:  @ kernel: restore sp_svc
    msr cpsr_c, #0xD3
    ldr sp, [r1, #4]
    msr cpsr_c, #0xD2

2:
    ldr sp, [r1, #0]
    ldmfd sp!, {r0}
    msr spsr_cxsf, r0
    ldmfd sp!, {r0-r12, lr}
    subs pc, lr, #0