.global start_first_process
.type start_first_process,%function

start_first_process:

    /*
     * r0 = process
     */

    ldr r1,[r0,#16]      @ mode

    cmp r1,#0
    beq start_kernel


start_user:

    ldr r1,[r0,#4]       @ user_sp

    msr cpsr_c,#0xDF
    mov sp,r1

    msr cpsr_c,#0xD2

    b restore_frame



start_kernel:

    /*
     * SVC stack
     */
    msr cpsr_c,#0xD3

    ldr r1,[r0,#0x54] @kernel stack offset

    add r1,r1,#(4*4096)

    mov sp,r1

    msr cpsr_c,#0xD2


restore_frame:

    ldr sp,[r0,#0]

    ldmfd sp!,{r1}
    msr spsr_cxsf,r1

    ldmfd sp!,{r0-r12,lr}

    subs pc,lr,#0
