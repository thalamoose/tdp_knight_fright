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

;
; This code must be at the beginning so that it matches the
		SECTION code_compiler
		include "src/asm/bootstrap.asm"

        SECTION BANK_32
        org $c000
		incbin "build/assets/kfsprites.bin"
        incbin "build/assets/kftiles.bin"

		SECTION BANK_33
		org $c000
        incbin "build/assets/shape_01.map"
        incbin "build/assets/shape_02.map"
        ;
        ; All palettes go here
        ;
		SECTION BANK_34
        org $c000
; 256 colours for the layer 2 backdrop
splash_palette:
        incbin "build/assets/kfback.pal"
; 256 colours for the sprites
sprite_palette:
        incbin "build/assets/kfback.pal"
player_palette:
        incbin "build/assets/kfplayer.pal"
; 256 colours for the tilemap
tilemap_palette:
        incbin "build/assets/kftiles.pal"
