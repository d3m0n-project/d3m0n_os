ENTRY(_start)

SECTIONS
{
    . = 0x8000;

    .text : ALIGN(4)
    {
        KEEP(*(.text.boot))
        KEEP(*(.text.start))
        *(.text*)
    }

    .rodata : ALIGN(4)
    {
        *(.rodata*)
    }

    .data : ALIGN(4)
    {
        *(.data*)
    }

    .bss : ALIGN(8)
    {
        __bss_start = .;

        *(.bss*)
        *(COMMON)

        __bss_end = .;
    }

    . = ALIGN(8);
    __kernel_end = .;

    .heap : ALIGN(8)
    {
        __heap_start = .;
        . += 0x01000000;
        __heap_end = .;
    }

    . = ALIGN(16);
    . += 0x4000;

    stack_top = .;
}