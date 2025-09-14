
bytes_for_tilemap equ (40*32*2)
initialize_tilemap:
        ; normally, colour 0 is transparent but the test image has it as colour 1
        ; ULA has already been mapped to it's alternate pages at 14,15. Tilemap can
        ; use all of pages 10,11.

        nextreg TRANS_TILEMAP_INDEX,0
        nextreg TILEMAP_CONTROL,%10000000
        nextreg TILEMAP_CHAR_ADDRESS,$0000>>8     ; Start character data right after tilemap
        nextreg TILEMAP_BASE_ADDRESS,$2000>>8
        
        nextreg TILEMAP_CLIP_WINDOW,8>>1
        nextreg TILEMAP_CLIP_WINDOW,(320-8)>>1
        nextreg TILEMAP_CLIP_WINDOW,8
        nextreg TILEMAP_CLIP_WINDOW,(256-8)
        ;
        ; Copy tilemap character data to tilemap area
        ;
        nextreg MMU_SLOT_6,TILES_PAGE
        nextreg MMU_SLOT_7,TILEMAP_PAGE
        ld hl,SWAP_BANK_0
        ld de,SWAP_BANK_1
        ld bc,$2000
        call memcpy_dma
        ;
        ; TEMPORARY FOR TESTING UNTIL TILEMAP PROPERLY SORTED
        ;
        ld hl,SWAP_BANK_0
        ld de,SWAP_BANK_1+32
        ld bc,$1fe0
        call memcpy_dma
        ; 
        ; END TESTING CODE
        ;
        ld hl,SWAP_BANK_1
        ld bc,32
        ld a,0
        call fill_mem

        ;
        ; Copy tile palettes
        ;
        nextreg MMU_SLOT_6,PALETTE_PAGE
        ld hl,tilemap_palette
        ld a,%011
        call copy_palette

        nextreg MMU_SLOT_6,TILEMAP_PAGE
        nextreg MMU_SLOT_7,TILEMAP_PAGE+1
        ; Fill tilemap area
        ld hl,SWAP_BANK_0+$2000
        ld e,%00000001      ; Tile attributes
        ld c,32
.next_lin
        ld b,40
.next_char
        ld (hl),a
        ld (hl),0 ;; TEST CODEf
        inc hl
        ld (hl),e
        inc hl
        inc a
        cp 96
        jr nz,.no_wrap
        xor a
.no_wrap
        djnz .next_char
        dec c
        jr nz,.next_lin

        ld hl,test_tilemap_table
        ld de,SWAP_BANK_0+$2000
        ld a,1
        call copy_tile_block
        ret

update_tilemap:
        ld a,(tilemap_x)
        inc a
        ld (tilemap_x),a
        ld a,(tilemap_y)
        inc a
        ld (tilemap_y),a

        ; 
        ; update hw registers
        ;
        ld a,(tilemap_x)
        neg
        nextreg TILEMAP_OFFSET_X_H,0
        nextreg TILEMAP_OFFSET_X_L,a
        ld a,(tilemap_y)
        neg
        nextreg TILEMAP_OFFSET_Y,a
        ret

; HL - Base of tile
; DE - Target address in texture map
;  A - Attribute
copy_tile_block:
        ld c,(hl)                       ; c=width,
        inc hl
        ld b,(hl)                       ; b=height
        inc hl
.next_line
        push bc,de
.next_char
        ldi                             ; Copy character code
        ld (de),a                       ; Set attribute
        inc de
        inc c                           ; Just checking to see if we
        dec c                           ; Still have more to do (cannot modify A)
        jr nz,.next_char
        pop de,bc
        add de,40*2
        djnz .next_line
        ret

        align 16
test_tilemap_table:
        include "build/assets/shape_01.map"