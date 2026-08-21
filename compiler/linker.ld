SECTIONS
{
    . = 0x00000000;

    .text :
    {
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .appmeta :
    {
        . = ALIGN(4);
        __appmeta_start = .;
        KEEP(*(.appmeta))
        __appmeta_end = .;
    }

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        *(.bss*)
        *(COMMON)
    }
}