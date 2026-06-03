.section .text.boot
.global _start
.extern kernel_main
.extern __bss_start
.extern __bss_end
.extern stack_top

_start:
    ldr sp, =stack_top

    /* clear the .bss section */
    ldr r0, =__bss_start
    ldr r1, =__bss_end
    mov r2, #0
bss_clear_loop:
    cmp r0, r1
    beq bss_clear_done
    str r2, [r0], #4
    b bss_clear_loop
bss_clear_done:

    bl kernel_main

hang:
    wfe
    b hang
