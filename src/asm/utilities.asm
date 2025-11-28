        include "include/hardware.inc"
        include "include/memorymap.inc"
        SECTION code_user
        global _nextreg
        global _nextreg16
        global _CopyPalette
        global _CopyPalettePartial
        global _memcpy_dma
        global _memset
        global _random8
        global _random16
        global _port_in
        global _port_out
        global _WaitVSync
        global _SetColour
        global _fp_mul_f88
        global _bcd_add
        global _DebugTiming
check_reset:
        xor a
        ret

_nextreg:
        ld hl,2
        add hl,sp
        ld a,(hl)
        ld bc, NEXTREG_SELECT_PORT
        out (c),a
        inc hl
        ld a,(hl)
        inc b
        out (c),a
        ret
_nextreg16:
        ld hl,2
        add hl,sp
        ld a,(hl)
        ld bc, NEXTREG_SELECT_PORT
        out (c),a
        inc hl
        inc b
        ld a,(hl)
        inc hl
        out (c),a
        ret
_port_in:
        ld hl,2
        add hl,sp
        ld c,(hl)
        inc hl
        ld b,(hl)
        in a,(c)
        ld l,a
        ret

_port_out:
        ld hl,2
        add hl,sp
        ld c,(hl)
        inc hl
        ld b,(hl)
        inc hl
        ld a,(hl)
        out (c),a
        ret
; A  - Fill value
; HL - Base address
; BC - Length
_memset:
        ld hl,2
        add hl,sp
        ld e,(hl)
        inc hl
        ld d,(hl)
        inc hl
        ld a,(hl)
        inc hl
        ld c,(hl)
        inc hl
        ld b,(hl)
        ld hl,de
        ld (de),a
        inc de
        dec bc
        ld a,b
        or c
        ret z
        ldir
        ret

; Copy a 9 bit RGB palette
; hl - base of palette in the palette page
; a - palette to write to:
;       %000 - ULA 1st,   %100 - ULA 2nd
;       %001 - L2 1st,    %101 - L2 2nd
;       %010 - SPR 1st,   %110 - SPR 2nd
;       %011 - tile 1st,  %111 - tile 2nd
_CopyPalette:
        ld hl,2
        add hl,sp
        ld e,(hl)
        inc hl
        ld d,(hl)
        inc hl
        ld a,(hl)
        ld hl,de
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

; Copy a 9 bit RGB palette
; hl - base of palette in the palette page
; a - palette to write to:
;       %000 - ULA 1st,   %100 - ULA 2nd
;       %001 - L2 1st,    %101 - L2 2nd
;       %010 - SPR 1st,   %110 - SPR 2nd
;       %011 - tile 1st,  %111 - tile 2nd
_CopyPalettePartial:
        ld hl,2
        add hl,sp
        ld e,(hl)
        inc hl
        ld d,(hl)
        inc hl
        ld a,(hl)
        inc hl
        sla a
        sla a
        sla a
        sla a
        nextreg PALETTE_CONTROL,a                           ; 
        ld a,(hl)
        inc hl
        nextreg PALETTE_INDEX,a
        ld b,(hl)
        ld hl,de
@next_palette:
        ld a,(hl)
        inc hl
        nextreg PALETTE_VALUE_9,a                       ; Write R2R1R0G2G1G0B2B1
        ld a,(hl)
        inc hl
        nextreg PALETTE_VALUE_9,a
        djnz @next_palette
        ret

_DebugTiming:
        ret
        ld hl,2
        add hl,sp
        ld a,(hl)
        out (ULA_PORT),a
        ret

_WaitVSync:
        ld a,0                              ; black border
        ;out (ULA_PORT),a
        halt
        ld a,1                              ; blue border
        ;out (ULA_PORT),a
        ret

_SetColour:
        ld hl, 2
        add hl,sp
        ld a,(hl)                               ; Palette type
        inc hl
        add a
        add a
        add a
        add a
        nextreg PALETTE_CONTROL, a
        ld a,(hl)                               ; Colour Index
        nextreg PALETTE_INDEX, a
        inc hl
        ld a,(hl)                               ; Colour Value
        nextreg PALETTE_VALUE_9,a
        inc hl
        ld a,(hl)
        nextreg PALETTE_VALUE_9,a
        ret

        global _fp_mul_F88
#if 0
    uint8_t a_lo = a & 0xFF;
    uint8_t a_hi = a >> 8;
    uint8_t b_lo = b & 0xFF;
    uint8_t b_hi = b >> 8;

    uint16_t p0 = (uint16_t)a_lo * b_lo;        // low * low
    uint16_t p1 = (uint16_t)a_lo * b_hi;        // low * high
    uint16_t p2 = (uint16_t)a_hi * b_lo;        // high * low
    // ignore a_hi * b_hi (would overflow 16 bits)

    // Combine, keeping only low 16 bits
    uint16_t result = p0 + ((p1 + p2) << 8);

#endif
a_lo equ 0
a_hi equ 1
b_lo equ 2
b_hi equ 3
_fp_mul_f88:
        push ix
        ld ix,4
        add ix,sp

        ld d,(ix+0)  
        ld e,(ix+3)
        mul d,e
        ld hl,de
        ld d,(ix+1)
        ld e,(ix+2)
        mul d,e
        add hl,de
        ld h,l
        ld l,0
        ld d,(ix+0)
        ld e,(ix+2)
        mul d,e
        add hl,de

        pop ix
        ret

_bcd_add:
        ld hl,2
        add hl,sp
        ld c,(hl)
        inc hl
        ld b,(hl)
        inc hl
        ld a,(hl)
        ld l,c
        ld h,b
        add (hl)
        daa
        ld (hl),a
        inc hl
@digit:
        ld a,0
        adc (hl)
        daa
        ld (hl),a
        inc hl
        ret nc
        djnz @digit
        ret
        


; 16-bit Galois LFSR using polynomial 0xB400
; State in RNG_SEEDL (low), RNG_SEEDH (high)
; Returns: HL = new 16-bit random value
; Affected: AF
_random16:
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
; returns L=new
_random8:
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
        ld l,a
        ld a,r
        xor l
        ld l,a
        ret                  ;10T


        SECTION data_user
random_seed:
        dw 0xbaad
random_seed_2:
        dw 0xf00d
; total roughly 39–48 T depending on branch timing — comfortably under 50T.

        