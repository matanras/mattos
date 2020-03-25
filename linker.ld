ENTRY(main)
OUTPUT_FORMAT("binary")
bootsector_addr = 0x7c00;
kernel_addr = 0xa000;

SECTIONS
{
	.bootsector bootsector_addr : AT(bootsector_addr)
	{
		FILL(0xebfe) /* fill with jmp $ */
		__bootsector_start__ = .;
		*(.bootsector.text)
		*(.bootsector.data)
		__bootsector_end__ = .;
		. += 510 - (__bootsector_end__ - __bootsector_start__);
		BYTE(0x55)
		BYTE(0xaa)
	}
	
	.text.entry kernel_addr : AT(ADDR(.bootsector) + SIZEOF(.bootsector))
	{
		__kernel_code_start__ = .;
		*(.text.entry)
	}

	.text :
	{
		*(.text)
		*(.rodata)
	}

	.data :
	{
		*(.data)
	}

	__kernel_num_sectors__ = ((. - __kernel_code_start__) / 512) + 1;

	.bss :
	{
		*(.bss)
	}

	/DISCARD/ :
	{
		*(.eh_frame)
		*(.comment)
		*(.note*)
	}
}

