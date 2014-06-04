

	.global mainmem
	.global	gram
	.global	tram
	.global	pcg
	.global	font_ank

	.section	.bss
	.align 2
mainmem:		.ds.b	0x10000
gram:			.ds.b	0xc000
tram:			.ds.l	0x800
pcg:			.ds.b	0x1800
font_ank:		.ds.b	0x800
gram2:			.ds.b	0xc000

	.end

