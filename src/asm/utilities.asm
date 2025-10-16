        include "include/hardware.inc"
        include "include/memorymap.inc"
        SECTION code_user
        global _nextreg
        global _copy_palette
        global _memcpy
        global _memset
check_reset:
        xor a
        ret

_nextreg:
        ld ix,0
        add ix,sp
        ld a,(ix+2)
        ld (@here+2),a
        ld a,(ix+3)
@here:  nextreg 0,a
        ret

; A  - Fill value
; HL - Base address
; BC - Length
_memset:
        ld ix,0
        add ix,sp
        ld bc,(ix+2)
        ld de,(ix+4)
        ld a,(ix+6)
        ld (de),a
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
_copy_palette:
        nextreg MMU_SLOT_6,PALETTE_PAGE
        sla a
        sla a
        sla a
        sla a
        nextreg PALETTE_CONTROL,a                           ; 
        ld b,0
        nextreg PALETTE_INDEX,0
@next_palette:
        ld a,(hl)
        inc hl
        nextreg PALETTE_VALUE_9,a                       ; Write R2R1R0G2G1G0B2B1
        ld a,(hl)
        inc hl
        nextreg PALETTE_VALUE_9,a
        djnz @next_palette
        ret

; 16-bit Galois LFSR using polynomial 0xB400
; State in RNG_SEEDL (low), RNG_SEEDH (high)
; Returns: HL = new 16-bit random value
; Affected: AF
get_random_16:
        ld      hl,(random_seed)
        ld      a,l
        rra                        ; shift right through carry
        ld      l,a
        ld      a,h
        rra
        ld      h,a
        jr      nc,@no_xor
        ld      de,0xB400
        xor     d                  ; h ^= 0xB4
        ld      h,a
        ld      a,l
        xor     e                  ; l ^= 0x00
        ld      l,a
@no_xor:
        ld      (random_seed),hl
        ret

; Galois LFSR: x^8 + x^6 + x^5 + x^4 + 1 (one common polynomial)
; new = (seed >> 1) ^ ( (seed & 1) ? 0xB8 : 0 )
; returns A=new
get_random:
        push bc
        ld   a,(random_seed) ; 7T
        srl   a               ; 8T   ; shift right with carry into bit7 (but we prefer logical >>)
        jr   nc,@no_xor         ; 7T (taken or not impacts timing)
        xor  0B8h            ; 7T
@no_xor:
        ld   (random_seed),a ; 7T
        ld   a,(random_seed_2) ; 7T
        srl   a               ; 8T   ; shift right with carry into bit7 (but we prefer logical >>)
        jr   nc, @no_xor2     ; 7T (taken or not impacts timing)
        xor  0B8h            ; 7T
@no_xor2:
        ld   (random_seed_2),a ; 7T
        xor b
        pop bc
        ret                  ;10T


        SECTION data_user
random_seed:
        dw 0xbaad
random_seed_2:
        dw 0xf00d
; total roughly 39–48 T depending on branch timing — comfortably under 50T.