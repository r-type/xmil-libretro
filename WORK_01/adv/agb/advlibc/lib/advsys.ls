OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
__text_start		= 0x08000000;
mainmem				= 0x02000000;
gram				= 0x02010000;
tram				= 0x0201c000;
pcg					= 0x0201e000;
font_ank			= 0x0201f800;
__ewram_start		= 0x0202c000;
__eheap_end			= 0x02040000;
z80core				= 0x03000000;
dma					= 0x03000040;
z80flag				= 0x03000080;
iocore				= 0x03000380;
crtc				= 0x03000598;
vramupd				= 0x030005e0;
__iwram_start		= 0x03000de0;
__sp_usr			= 0x03008000 - 0x100;
__sp_irq			= 0x03008000 - 0x60;
__intr_vector_buf	= 0x03007ffc;
__sp_usr_offset		= __sp_usr - 0x3000000;
SECTIONS
{
	.text __text_start :
	{
		*(.init)
		*(SORT(.init$*))
		*(.text)
		*(SORT(.text$*))
		*(.gnu.warning)
		*(.gnu.linkonce.t*)
		*(.glue_7)
		*(.glue_7t)
		. = ALIGN(4);
	} = 0xff

	__text_end = .;
	.rodata :
	{
		*(.rodata)
		*all.rodata*(*)
		*(.roda)
		*(.rodata.*)
		*(.gnu.linkonce.r*)
		SORT(CONSTRUCTORS)
		. = ALIGN(4);
	} = 0xff

	.ctors :
	{
		__ctor_list = .;
		LONG (-1);
		*(SORT(.crt0c.*));
		*(SORT(.ctors.*));
		*(.ctors);
		*(.ctor);
		LONG (0);
		. = ALIGN(4);
	} = 0

	.dtors :
	{
		__dtor_list = .;
		LONG (-1);
		*(SORT(.crt0d.*));
		*(SORT(.dtors.*));
		*(.dtors);
		*(.dtor);
		LONG (0);
		. = ALIGN(4);
	} = 0

	.eh_frame :
	{
		KEEP (*(.eh_frame))
		. = ALIGN(4);
	} = 0

	.gcc_except_table :
	{
		*(.gcc_except_table)
		. = ALIGN(4);
	} = 0

	__iwram_lma = ADDR(.text) + SIZEOF(.text) + SIZEOF(.rodata) + SIZEOF(.ctors) + SIZEOF(.dtors) + SIZEOF(.eh_frame) + SIZEOF(.gcc_except_table);
	.iwram __iwram_start : AT (__iwram_lma)
	{
		__iwram_start = ABSOLUTE(.);
		*(.iwdata0)
		*(.iwdata1)
		*(.iwdata2)
		*(.iwdata3)
		*(.iwdata4)
		*(.iwdata5)
		*(.iwram)
		. = ALIGN(4);
	} = 0xff
	__iwram_end = .;

	.bss ALIGN(4) :
	{
		__bss_start = ABSOLUTE(.);
		__bss_start__ = ABSOLUTE(.);
		*(.dynbss)
		*(.gnu.linkonce.b*)
		*(COMMON)
		. = ALIGN(4);
	}
	__bss_end = .;
	__bss_end__ = .;

	__data_lma = ADDR(.text) + SIZEOF(.text) + SIZEOF(.rodata) + SIZEOF(.ctors) + SIZEOF(.dtors) + SIZEOF(.eh_frame) + SIZEOF(.gcc_except_table) + SIZEOF(.iwram);
	.data ALIGN(4) : AT (__data_lma)
	{
		__data_start = ABSOLUTE(.);
		*(.data)
		*(.data.*)
		*(.gnu.linkonce.d*)
		CONSTRUCTORS
		*(.iwram.text)
		. = ALIGN(4);
	} = 0xff
	__data_end  =  .;
	PROVIDE (edata = .);

	__ewram_lma = ADDR(.text) + SIZEOF(.text) + SIZEOF(.rodata) + SIZEOF(.ctors) + SIZEOF(.dtors) + SIZEOF(.eh_frame) + SIZEOF(.gcc_except_table) + SIZEOF(.iwram) + SIZEOF(.data);
	.ewram __ewram_start : AT (__ewram_lma)
	{
		__ewram_start = ABSOLUTE(.);
		*(.ewram)
/*		*(.text)			*/
/*		*(SORT(.text$*))	*/
		. = ALIGN(4);
	} = 0xff
	__ewram_end = .;

	_end = .;
	__end__ = .;
	PROVIDE (end = .);

	__extromimage = ADDR(.text) + SIZEOF(.text) + SIZEOF(.rodata) + SIZEOF(.ctors) + SIZEOF(.dtors) + SIZEOF(.eh_frame) + SIZEOF(.gcc_except_table) + SIZEOF(.iwram) + SIZEOF(.data) + SIZEOF(.ewram);

	__rom_append = ADDR(.text) + SIZEOF(.text) + SIZEOF(.rodata) + SIZEOF(.ctors) + SIZEOF(.dtors) + SIZEOF(.eh_frame) + SIZEOF(.gcc_except_table) + SIZEOF(.iwram) + SIZEOF(.data) + SIZEOF(.ewram) + 0x6a800;

}

