
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
        ldir

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