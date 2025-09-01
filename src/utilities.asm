
check_reset:
        ret



; 0xe000 remapped to page number
; A - Page number
set_page:
        nextreg $57,a
        ret



; A  - Fill value
; HL - Base address
; BC - Length
fill_mem:
        ld de,hl
        ld (hl),a
        inc de
        dec bc
        ldir
        ret

; Copy a 9 bit RGB palette
; hl - base of palette in the palette page
; a - palette to write to:
;       %000 - ULA 1st,   %100 - ULA 2nd
;       %001 - L2 1st,    %101 - L2 2nd
;       %010 - SPR 1st,   %110 - SPR 2nd
;       %011 - tile 1st,  %111 - tile 2nd
copy_palette:
        nextreg MMU_SLOT_6,PALETTE_PAGE
        sla a
        sla a
        sla a
        sla a
        nextreg PALETTE_CONTROL,a                           ; 
        ld b,0
        nextreg PALETTE_INDEX,0
.next_palette
        ld a,(hl)
        inc hl
        nextreg PALETTE_VALUE_9,a                       ; Write R2R1R0G2G1G0B2B1
        ld a,(hl)
        inc hl
        nextreg PALETTE_VALUE_9,a
        djnz .next_palette
        ret
