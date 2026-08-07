.global start_first_process
.type start_first_process,%function

start_first_process:

    /*
     * r0 = current_process
     */


    /*
     * set System mode stack first
     */
    ldr r1,[r0,#4]        @ user_sp
    msr cpsr_c,#0xDF      @ System mode
    mov sp,r1


    /*
     * return to IRQ mode
     */
    msr cpsr_c,#0xD2


    /*
     * restore fake IRQ frame
     */
    ldr sp,[r0]


    ldmfd sp!,{r1}
    msr spsr_cxsf,r1


    ldmfd sp!,{r0-r12,lr}


    subs pc,lr,#0