        include "include/defines.inc"
        include "include/hardware.inc"
        include "include/memorymap.inc"

        SECTION code_user

bytes_for_tilemap equ (40*32*2)
        global _InitializeTilemap,update_tilemap
        extern memcpy_dma,fill_mem,tilemap_palette,copy_palette
        extern play_area_center_x
        extern tilemap_x
        extern tilemap_y
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
        nextreg TILEMAP_CLIP_WINDOW,256-8
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
tile_next_line:
        ld b,40
tile_next_char:
        ld (hl),a
        ld (hl),0 ;; TEST CODEf
        inc hl
        ld (hl),e
        inc hl
        inc a
        cp 96
        jr nz,tile_no_wrap
        xor a
tile_no_wrap:
        djnz tile_next_char
        dec c
        jr nz,tile_next_line

        ld hl,test_tilemap_table
        ld de,SWAP_BANK_0+$2000
        ld a,1
        call copy_tile_block
        ; default position until map is properly set up.
        ld hl,play_area_center_x
        ld (hl),PLAY_AREA_CELLS_WIDTH/2
        inc hl
        ld (hl),PLAY_AREA_CELLS_HEIGHT/2
        ld hl,tilemap_x
        ld (hl),12
        inc hl
        ld (hl),0
        ret
update_tilemap:
        ;ld a,(tilemap_x)
        ;inc a
        ;ld (tilemap_x),a
        ;ld a,(tilemap_y)
        ;inc a
        ;ld (tilemap_y),a

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
copy_next_line:
        push bc
        push de
copy_next_char:
        ldi                             ; Copy character code
        ld (de),a                       ; Set attribute
        inc de
        inc c                           ; Just checking to see if we
        dec c                           ; Still have more to do (cannot modify A)
        jr nz,copy_next_char
        pop de
        pop bc
        add de,40*2
        djnz copy_next_line
        ret

        align 16
test_tilemap_table:
        include "build/assets/shape_02.map"