SCREEN_WIDTH    equ 256
SCREEN_HEIGHT   equ 192
ATTR_WIDTH      equ SCREEN_WIDTH/8
ATTR_HEIGHT     equ SCREEN_HEIGHT/8
SCREEN_BASE     equ $4000
SCREEN_LENGTH   equ SCREEN_WIDTH*SCREEN_HEIGHT/8
ATTR_BASE       equ SCREEN_BASE+SCREEN_LENGTH
ATTR_LENGTH     equ ATTR_WIDTH*ATTR_HEIGHT
; ----------------------------
; Print null-terminated string
; Destroys DE,HL
; ----------------------------


print_str:
		pop ix						; Get format string address
		push af						; save AF for now. We want to be non-destructive.
print_str_next:
		ld a, (ix)
		inc ix
		and a
		jr z,print_str_done
		cp '%'
		jr z,escape_character
not_escape:
		call print_char
		jr print_str_next
print_str_done:
		pop af
		jp (ix)
escape_character:
		ld a,(ix)
		inc ix
		and a
		jr z,print_str_done
		cp 'x'
		jr z,embedded_hex
		cp 'c'
		jr z,embedded_char
		cp 'd'
		jr z,embedded_int
		jp not_escape
embedded_hex:
		pop af
		pop hl
		push af
		call print_hex16
		jp print_str_next
embedded_char:
		pop de
		pop af
		push de
		call print_hex8
		jp print_str_next
embedded_int:
		pop af
		pop hl
		push af
		call print_dec16
		jp print_str_next
        ret

; ----------------------------
; Print 2-digit hex (A)
; ----------------------------
print_hex8:
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
; Print 4-digit hex (HL)
; ----------------------------
print_hex16:
		push af
		ld a, h
		call print_hex8
		ld a, l
		call print_hex8
		pop af
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
print_dec16:
		push af
		push bc
		push hl
		ld b, 5           ; max 5 digits for 16-bit decimal
		ld c, 0

		; store digits in reverse order in stack
store_digits:
		call divmod10
		push af            ; store remainder (digit) on stack
		inc c
		ld a, h
		or l
		jr z, print_digits  ; if HL = 0, we're done
		djnz store_digits

print_digits:
		ld b,c
digit_loop:
		pop af
		add a, '0'         ; convert to ASCII
                push bc
		call print_char
                pop bc
		djnz digit_loop
		pop hl
		pop bc
		pop af
		ret

; Input:  HL = 16-bit unsigned dividend
; Output: HL = quotient, A = remainder

divmod10:
        push bc
        ld c,10
div_hl_c:
        xor a           ; Clear A (remainder = 0)
        ld b, 16        ; We'll shift 16 bits
div_loop:
        add hl, hl      ; Shift HL left, MSB into carry
        rla             ; Shift A left through carry (A = remainder)
        jr c, skip_cmp  ; If carry set, remainder ? 128 ? definitely ? C
        cp c            ; Compare remainder to divisor
        jr c, skip_sub  ; If A < C, skip subtraction
skip_cmp:
        sub c           ; A -= C (remainder -= divisor)
        inc l           ; L = L + 1 (quotient bit set)
skip_sub:
        djnz div_loop   ; Repeat for all bits
        pop bc
        ret             ; L = quotient, A = remainder
   

clear_screen:
        xor a
        ld (char_screen_x),a
        ld (char_screen_y),a
        ld hl,SCREEN_BASE
        ld bc,SCREEN_LENGTH
        ld a,$00
        call fill_mem
        ld hl,ATTR_BASE
        ld bc,ATTR_LENGTH
        ld a,%01000111    ; Attribute, INK=7, PAPER=0
        call fill_mem
        ret
; Address
;          15 14 13 12 11 10  9  8    7  6  5  4  3  2  1  0
; Bitmap    0  1  0 y4 y3  0  0  0 : y2 y1 y0 x4 x3 x2 x1 x0  
; Attr      0  1  0  1  1  0 y4 y3 : y2 y1 y0 x4 x3 x2 x1 x0  
print_char:
        cp 13
        jr z,.cr
        cp 10
        jr z,.lf
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
.char_loop
        ld a,(hl)
        ld (de),a
        inc d
        inc hl
        djnz .char_loop
        pop de
        ; Update attributes
        ld a,d
        rrca
        rrca
        rrca
        or %01011000
        ld d,a
        ld a,%01000111
        ld (de),a

        ; Update screen coordinate
        ld a,(char_screen_x)
        inc a
.nextline
        and 31
        ld (char_screen_x),a
        jr nz,.same_line
        ld a,(char_screen_y)
        inc a
        cp 24
        jr c,.not_eos
        ld a,0
.not_eos
        ld (char_screen_y),a
.same_line
        ret
.cr
        xor a
        jr .nextline
.lf
        ret

char_screen_x:  
        db 0
char_screen_y: 
        db 0


character_set:
        incbin assets/charset.bin
