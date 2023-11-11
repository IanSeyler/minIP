OUTPUT_FORMAT("binary")
OUTPUT_ARCH("i386:x86-64")
ENTRY(main)
SECTIONS
{
	. = 0xFFFF800000000000;
	.text : {
		*(.text)
		. = ALIGN(16);
	}
	.data : {
		*(.data)
		*(.rodata)
		. = ALIGN(16);
	}
	__bss_start = .;
	.bss : {
		bss = .; _bss = .; __bss = .;
		*(.bss);
	}
	end = .; _end = .; __end = .;
}