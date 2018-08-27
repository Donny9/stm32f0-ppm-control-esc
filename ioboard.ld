OUTPUT_ARCH(arm)

ENTRY(Reset_Handler)

MEMORY
{
	RAM(xrw): ORIGIN = 0x20000000, LENGTH = 0x00005000 /* 20 KB */
	FLASH(xrw): ORIGIN = 0x08000000, LENGTH = 0x00020000 /* 128 KB */
}

SECTIONS
{
	. = ALIGN(4);
	.text :
	{
		KEEP(*(.vector_table))
		*(.text*)
	} > FLASH

	. = ALIGN(4);
	__text_end__ = .;

	.data : AT(__text_end__)
	{
		__data_start__ = .;
		*(vtable)
		*(.data*)
		. = ALIGN(4);
		__data_end__ = .;
	} > RAM

	. = ALIGN(4);
	.bss :
	{
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		__bss_end__ = .;
	} > RAM
	
	. = ALIGN(4);

	__LogbufBase = .;
	__LogbufSize = __StackLimit - __LogbufBase;

	__StackLimit = __StackTop - 4K;
	__StackTop = ORIGIN(RAM) + LENGTH(RAM);
	PROVIDE(__stack = __StackTop);
}
