
initialize_display:
        nextreg LAYER_2_RAM_BANK, LAYER_2_PAGE/2
        ld bc,LAYER_2_ACCESS                            ; Enable layer 2 bitmap
        ld a,2
        out (c),a
        nextreg GLOBAL_TRANSPARENCY,0                 ; set global transparancy value
        nextreg PALETTE_CONTROL,%00000000
        nextreg PALETTE_INDEX,$88                       ; set "bright+black" ULA colour
        nextreg PALETTE_VALUE_8,$e3                     ; set BRIGHT+BLACK to transparent
        nextreg PALETTE_INDEX,$18                       ; set BRIGHT BLACK to transparent
        nextreg PALETTE_VALUE_8,0
        nextreg SPRITE_AND_LAYERS,%00001001             ; enable sprites, SUL
        nextreg DISPLAY_CONTROL_1,%11000000             ; Enable layer 2, ULA to shadow bank (bank 7, page 14,15)

        nextreg LAYER_2_CONTROL, %00010000              ; Enable 320 mode for layer 2
        ; Set clip to 4,4,314,252 to give us an 8 pixel border for clipping. We don't bother
        ; clipping the actual shape as we're guaranteed any particle effects are below 8x8 in
        ; size.

        nextreg LAYER_2_CLIP,(4)>>1                     ; X1 (Y1 in 320 mode)
        nextreg LAYER_2_CLIP,((319-4)>>1)               ; X2 (Y2 in 320 mode)
        nextreg LAYER_2_CLIP,(4)                        ; Y1 (X1 in 320 mode)
        nextreg LAYER_2_CLIP,(255-4)                    ; Y2 (X2 in 320 mode)
        ret

; black border area shows idle time
wait_vsync:
        ld a,0                              ; black border
        out (ULA_PORT),a
        halt
        ld a,1                              ; blue border
        out (ULA_PORT),a
        ret
upload_sprites:
        ;
        ; Upload test sprites
        ;
        nextreg MMU_SLOT_6,SPRITES_PAGE
        nextreg MMU_SLOT_7,SPRITES_PAGE+1
.copy_sprite_page
        ld a,0                                          ; Select sprite 0
        ld hl,SWAP_BANK_0
.next_sprite
        ld bc,SPRITE_SLOT_PORT
        out (c),a
        ld bc,SPRITE_PATTERN_PORT
        ld d,a
        ld e,0
.next_upload
        outi
        inc b
        dec e
        jr nz,.next_upload
        ld a,d
        inc a
        cp 64
        jr nz,.next_sprite
        ret
