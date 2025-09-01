; Load splash asset, and display
; Exits when game type is set up
show_loading_splash:
        call copy_splash_bitmap
        call upload_sprites

        ret
copy_splash_bitmap:
        ld d,SPLASH_PAGE
        ld e,LAYER_2_PAGE
        ld b,5
        ; Copy static background to layer 2
.next_page
        ld a,d
        nextreg MMU_SLOT_6,a
        ld a,e
        nextreg MMU_SLOT_7,a
        call copy_splash
        inc d
        inc e
        djnz .next_page
        ld hl,splash_palette
        ld a,%001                       ; Layer 2, first palette
        call copy_palette

        ret
copy_splash:
        push bc
        push de
        ld hl,SWAP_BANK_0
        ld de,SWAP_BANK_1
        ld bc,$2000
        ldir
        pop de
        pop bc
        ret


; Transitions to black, probably
clear_loading_splash:
        ret

show_title:
        ret

