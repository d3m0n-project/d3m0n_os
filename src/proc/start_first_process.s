.global start_first_process
.type start_first_process,%function

start_first_process:
    @ r0 = process
    ldr r1,[r0,#16]      @ mode
    ldr r2,[r0,#4]       @ native sp (kernel_stack top or user_stack top)

    cmp r1,#0
    moveq r3,#0xD3       @ SVC (kernel)
    movne r3,#0xDF       @ SYS (user, banked with USR)

    msr cpsr_c,r3
    mov sp,r2
    msr cpsr_c,#0xD2

restore_frame:
    ldr sp,[r0,#0]
    ldmfd sp!,{r1}
    msr spsr_cxsf,r1
    ldmfd sp!,{r0-r12,lr}
    subs pc,lr,#0
