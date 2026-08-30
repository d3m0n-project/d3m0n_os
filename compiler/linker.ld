ENTRY(_start)

SECTIONS
{
    . = 0x02000000;

    .text : ALIGN(4)
    {
        . = ALIGN(4);
        *(.text*)
    }

    .rodata : ALIGN(4)
    {
        *(.rodata*)
    }

    .ARM.extab : ALIGN(4)
    {
        KEEP(*(.ARM.extab*))
    }

    .ARM.exidx : ALIGN(4)
    {
        __exidx_start = .;
        KEEP(*(.ARM.exidx*))
        __exidx_end = .;
    }

    .appicon : ALIGN(4)
    {
        __appicon_start = .;
        KEEP(*(.appicon))
        __appicon_end = .;
    }
    __appicon_size = __appicon_end - __appicon_start;

    .appmeta : ALIGN(4)
    {
        __appmeta_start = .;
        KEEP(*(.appmeta))
        __appmeta_end = .;
    }

    .data : ALIGN(4)
    {
        *(.data*)
        *(.got*)
        *(.got.plt*)
    }

    .bss (NOLOAD) : ALIGN(8)
    {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.debug*)
        *(.ARM.attributes)
        *(.dynamic)
        *(.dynsym)
        *(.dynstr)
        *(.hash)
        *(.gnu.hash)
        *(.interp)
    }
}