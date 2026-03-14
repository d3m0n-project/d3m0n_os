.section .bss
.align 16

NUM_CPUS = 4
STACK_SIZE = 16384

stack_table:
    .rept NUM_CPUS
    .skip STACK_SIZE
    .endr

stack_table_end:

.section .text
.global _start

_start:
    mrs x0, mpidr_el1
    and x0, x0, #0xff          // lower 8 bits
    mov x1, x0                 // save CPU id

    ldr x2, =stack_table
    add x2, x2, x1, lsl #14    // 16 KB per stack
    add sp, x2, #STACK_SIZE

    cbz x1, primary_cpu

secondary_cpu:
    bl secondary_entry
hang_secondary:
    wfe
    b hang_secondary

primary_cpu:
    mov x20, x0     // save dtb ptr
    bl kernel_main
hang_primary:
    wfe
    b hang_primary

secondary_entry:
    // CPU-specific init here
    ret
