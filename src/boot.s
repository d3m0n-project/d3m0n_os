.section .text.boot
.global _start
.extern kernel_main
.extern __bss_start
.extern __bss_end
.extern stack_top

_start:
    .section .text.boot
.global _start
.extern kernel_main
.extern __bss_start
.extern __bss_end
.extern stack_top

_start:
    ldr sp, =stack_top
    bl kernel_main

hang:
    wfe
    b hang
