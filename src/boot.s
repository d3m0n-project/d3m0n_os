.section .text
.global _start

_start:
    mrs x0, mpidr_el1
    and x0, x0, #3
    cbz x0, primary_cpu

secondary_cpu:
    wfe
    b secondary_cpu

primary_cpu:
    ldr x0, =stack_top
    mov sp, x0
    bl kernel_main

hang:
    wfe
    b hang

.section .bss
.align 12
stack:
    .skip 4096
stack_top:
