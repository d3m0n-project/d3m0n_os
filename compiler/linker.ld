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

	.ARM.extab :
	{
		. = ALIGN(4);
		KEEP(*(.ARM.extab*))
	}

	.ARM.exidx :
	{
		. = ALIGN(4);
		__exidx_start = .;

		KEEP(*(.ARM.exidx*))

		. = ALIGN(4);
		__exidx_end = .;
	}

	.appicon :
	{
		. = ALIGN(4);
		__appicon_start = .;
		KEEP(*(.appicon))
		__appicon_end = .;
	}
	__appicon_size = __appicon_end - __appicon_start;

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

	/DISCARD/ :
	{
		*(.comment)
		*(.debug*)
		*(.ARM.attributes)
	}
}