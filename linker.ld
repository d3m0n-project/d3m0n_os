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

    .rodata : { *(.rodata*) }
    .data   : { *(.data*) }

    .bss : {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }

    . = ALIGN(8);
    . += 0x4000;
    stack_top = .;
}