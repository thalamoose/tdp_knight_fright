        include "include/hardware.inc"
SCREEN_WIDTH    equ 256
SCREEN_HEIGHT   equ 192
ATTR_WIDTH      equ SCREEN_WIDTH/8
ATTR_HEIGHT     equ SCREEN_HEIGHT/8
SCREEN_BASE     equ $0000
SCREEN_LENGTH   equ SCREEN_WIDTH*SCREEN_HEIGHT/8
ATTR_BASE       equ SCREEN_BASE+SCREEN_LENGTH
ATTR_LENGTH     equ ATTR_WIDTH*ATTR_HEIGHT
; ----------------------------
; Print null-terminated string
; Destroys DE,HL
; ----------------------------

        SECTION code_user

        global _x_printf, _memcpy_dma
_x_printf:
        ld hl, 2
        add hl,sp
        ld e,(hl)
        inc hl
        ld d,(hl)
        inc hl
        ex de,hl
        ld a,(ula_scroll_offset)
        add a
        add a
        add a
        nextreg ULA_SCROLL_Y,a
        ; de - points to parameter list
        ; hl - points to the format string
print_str_next:
        ld a, (hl)
        inc hl
        and a
        jr z,print_str_done
        cp '%'
        jr z,escape_character
not_escape:
        call print_char
        jr print_str_next
print_str_done:
        ret
escape_character:
        ld a,(hl)
        inc hl
        and a
        jr z,print_str_done
        cp 'x'
        jr z,embedded_hex
        cp 'c'
        jr z,embedded_char
        cp 'd'
        jr z,embedded_int
        cp 's'
        jr z,embedded_string
        jp not_escape
embedded_hex:
        ex de,hl
        ld c,(hl)
        inc hl
        ld b,(hl)
        inc hl
        ex de,hl
        call print_hex16_bc
        jp print_str_next
embedded_char:
        ex de,hl
        ld a,(hl)
        inc hl
        ex de,hl
        call print_hex8_a
        jp print_str_next
embedded_int:
        ex de,hl
        ld c,(hl)
        inc hl
        ld b,(hl)
        inc hl
        ex de,hl
        bit 7,b
        jr z,@positive
        push hl
        push de
        push bc
        ld a,'-'
        call print_char
        pop bc
        pop de
        ld hl,0
        sbc hl,bc
        ld bc,hl
        pop hl

@positive:
        call print_dec16_bc
        jp print_str_next
embedded_string:
        push hl
        ex de,hl
        ld e,(hl)
        inc hl
        ld d,(hl)
        inc hl
        ex de,hl
@embedded_str_next:
        ld a,(hl)
        inc hl
        and a
        jr z,@embedded_str_done
        call print_char
        jr @embedded_str_next
@embedded_str_done:
        pop hl
        jp print_str_next
        ret

; ----------------------------
; Print 2-digit hex (A)
; ----------------------------
print_hex8_a:
        push af
        push af
        rra
        rra
        rra
        rra
        and 0x0f
        call print_hex_digit
        pop af
        and 0x0f
        call print_hex_digit
        pop af
        ret

; ----------------------------
; Print 4-digit hex (bc)
; ----------------------------
print_hex16_bc:
        ld a, b
        call print_hex8_a
        ld a, c
        call print_hex8_a
        ret

print_hex_digit:
        cp 10
        jr c, num_digit
        add a, 'a' - 10
        jp print_char
num_digit:
        add a, '0'
        jp print_char

; Routine to print unsigned 16-bit HL as decimal
print_dec16_bc:
        push de
        ld d, 5           ; max 5 digits for 16-bit decimal
        ld e, 0

        ; store digits in reverse order in stack
store_digits:
        call divmod10
        push af            ; store remainder (digit) on stack
        inc e
        ld a, b
        or c
        jr z, print_digits  
        dec d
        jr nz,store_digits

print_digits:
        ld d,e
digit_loop:
        pop af
        add a, '0'         ; convert to ASCII
        call print_char
        dec d
        jr nz,digit_loop
        pop de
        ret

; Input:  BC = 16-bit unsigned dividend
; Output: BC = quotient, A = remainder

divmod10:
        push de
        ld e,10
div_hl_c:
        xor a           ; Clear A (remainder = 0)
        ld d, 16        ; We'll shift 16 bits
div_loop:
        sla c           ; Shift BC left, MSB in to carry
        rl b
        rla             ; Shift A left through carry (A = remainder)
        jr c, skip_cmp  ; If carry set, remainder ? 128 ? definitely ? C
        cp e            ; Compare remainder to divisor
        jr c, skip_sub  ; If A < C, skip subtraction
skip_cmp:
        sub e           ; A -= E (remainder -= divisor)
        inc c           ; C = C + 1 (quotient bit set)
skip_sub:
        dec d
        jr nz,div_loop   ; Repeat for all bits
        pop de
        ret             ; C = quotient, A = remainder
   

; Address
;          15 14 13 12 11 10  9  8    7  6  5  4  3  2  1  0
; Bitmap    0  1  0 y4 y3  0  0  0 : y2 y1 y0 x4 x3 x2 x1 x0  
; Attr      0  1  0  1  1  0 y4 y3 : y2 y1 y0 x4 x3 x2 x1 x0  
print_char:
        push hl
        push de
        push bc
        cp '\n'
        jr z,@crlf
        sub ' '
        ld l,a
        ld h,0
        add hl,hl
        add hl,hl
        add hl,hl
        ld bc,character_set
        add hl,bc
        ld a,(char_screen_y)
        ld d,a
        ld a,(ula_scroll_offset)
        add d
        cp 24
        jr c,@nowrap
        sub 24
@nowrap:
        ld d,a
        ; Shuffle y[2..0] to addr [7..5]
        and %00000111
        rrca
        rrca
        rrca
        ld e,a
        ; include x coordinate
        ld a,(char_screen_x)
        or e
        ld e,a
        ; Shuffle y[4..3] to addr [12..11]
        ld a,d
        and %00011000
        ; Set addr [15..13] to %010
        or  SCREEN_BASE>>8
        ld d,a
        push de
        ld b,8
@char_loop:
        ld a,(hl)
        ld (de),a
        inc hl
        inc d
        djnz @char_loop
        pop de
        ; Update attributes
        ld a,d
        rrca
        rrca
        rrca
        or %00011000
        ld d,a
        ld a,%01000111
        ld (de),a

        ; Update screen coordinate
        ld a,(char_screen_x)
        inc a
@nextline:
        and 31
        ld (char_screen_x),a
        jr nz,@same_line
        ld a,(char_screen_y)
        inc a
        cp 24
        jr c,@not_eos
        ;
        ; Scroll the screen
        ;
        ld a,(ula_scroll_offset)
        inc a
        cp 24
        jr c,@nowrap2
        xor a
@nowrap2:
        ld (ula_scroll_offset),a
        ld a,23
@not_eos:
        ld (char_screen_y),a
@same_line:
        pop bc
        pop de
        pop hl
        ret
@crlf:
        ld a,' '
        call print_char
        ld a,(char_screen_x)
        and a
        jr nz,@crlf
        pop bc
        pop de
        pop hl
        ret

        SECTION data_user
ula_scroll_offset:
        db 0
char_screen_x:  
        db 0
char_screen_y: 
        db 0

character_set:
        incbin "assets/charset.bin"
