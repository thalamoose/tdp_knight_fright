	include "include/hardware.inc"
	include "include/memorymap.inc"

		org 0x8000

		SECTION code_compiler
		SECTION rodata_compiler
		SECTION data_compiler
		SECTION bss_compiler

;
; This code must be at the beginning so that it matches the
		SECTION code_compiler
		include "src/bootstrap.asm"
