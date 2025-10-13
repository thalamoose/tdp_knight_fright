	include "include/hardware.inc"
	include "include/memorymap.inc"

		org 0x8000

		SECTION code_user
		SECTION code_compiler
		SECTION rodata_compiler
		SECTION data_user
		SECTION data_compiler
		SECTION bss_user
		SECTION bss_compiler

