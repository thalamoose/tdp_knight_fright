; Uses Kempston interface
read_controllers:
        in a,(KEMPSTON_JOYSTICK_PORT)
        ld (joystick_buttons),a
        ret
read_mouse:
        push bc
        ld bc,KEMPSTON_MOUSE_X_PORT
        in a,(c)
        ld (mouse_x),a
        ld h,a
        ld bc,KEMPSTON_MOUSE_Y_PORT
        in a,(c)
        ld (mouse_y),a
        ld l,a
        ld bc,KEMPSTON_MOUSE_BTN_PORT
        in a,(c)
        ld (mouse_button),a
        pop bc
        ret

; ------------------------------------------------------------
; read_keyboard  (Next-style: select row by loading B and doing IN A,(C))
; Returns ASCII of first key pressed in key_press (0 if none)
; Uses local labels, standard Z80 instructions
; ------------------------------------------------------------

read_keyboard:
    xor     a
    ld      (key_press),a       ; default = no key pressed
    ld      c,$FE               ; low byte for IN (C)
    ld      d,0                 ; row index 0..7

    ld      b,%01111111
    ld      h,%00011111
.row_loop:
    rlc     b
    in      a,(c)               ; IN A,(C) -> hardware returns column bits for BC
    cpl                         ; invert (keyboard is active-low)
    and     h                   ; mask columns 0..4
    jr      z,.no_key_in_row

    ; --- find lowest set bit -> column index in E ---
    ld      e,-1
.col_scan:
    inc     e
    srl     a
    jr      nc,.col_scan

    ; --- offset = row*5 + column ---
    ld      a,d
    sla     a
    sla     a
    add     a,d                ; a = row*5
    add     a,e                ; add column
    ld      hl,key_table
    ld      e,a
    ld      d,0
    add     hl,de
    ld      a,(hl)
    ld      (key_press),a
    ret

.no_key_in_row:
    inc     d
    ld      a,d
    cp      8
    jr      nz,.row_loop
    ret

; Key table: 8 rows × 5 columns (row-major)
key_table:
    defb "^","Z","X","C","V"                    ; Port FEFE
    defb "A","S","D","F","G"                    ; Port FDFE
    defb "Q","W","E","R","T"                    ; Port FBFE
    defb "1","2","3","4","5"                    ; Port F7FE
    defb "0","9","8","7","6"                    ; Port EFFE 
    defb "P","O","I","U","Y"                    ; Port DFFE
    defb 13 ,"L","K","J","H"                    ; Port BFFE
    defb " ","~","M","N","B"                    ; Port 7FFE

