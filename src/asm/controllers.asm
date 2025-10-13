; Uses Kempston interface.
; Simulate keyboard input as follows:
; A - up
; Z - down
; L - left
; P - right
; SP - button
; We can make this redefinable by having a table with port
; and mask combinations.
read_controllers:
        in a,(KEMPSTON_JOYSTICK_PORT)
        ld b,a
        ld a,(scancodes+1)              ; A
        bit 0,a
        jr nz,.no_up
        set JOYPAD_L_UP,b
.no_up:
        ld a,(scancodes+0)              ; Z
        bit 1,a
        jr nz,.no_down
        set JOYPAD_L_DOWN,b
.no_down:
        ld a,(scancodes+6)              ; L
        bit 1,a
        jr nz,.no_left
        set JOYPAD_L_LEFT,b
.no_left
        ld a,(scancodes+5)              ; P
        bit 0,a
        jr nz,.no_right
        set JOYPAD_L_RIGHT,b
.no_right
        ld a,(scancodes+7)              ; space
        bit 0,a
        jr nz,.no_fire
        set JOYPAD_R_DOWN,b
.no_fire
        ld a,b
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

; Reads from the keyboard matrix,
; stores this in memory for quick key scan, 
read_keyboard:
        ld b,%11111110      ; Bit line to activate
        ld c,$fe            ; ULA port
        ld hl,scancodes
.read_lp:
        in a,(c)
        ld (hl),a
        inc hl
        rlc b
        jr c,.read_lp        ; we terminate as soon as the zero shifts in to carry
        ret 

; ------------------------------------------------------------
; Takes previously read scancodes and turns it in to a keycode
; ------------------------------------------------------------
get_key:
    xor     a
    ld      (key_press),a       ; default = no key pressed
    ld      d,0                 ; row index 0..7
    ld      hl,scancodes
.row_loop:
    ld      a,(hl)
    inc     hl
    cpl                         ; invert (keyboard is active-low)
    and     %000011111          ; mask columns 0..4
    jr      z,.no_key_in_row

    ; --- find lowest set bit -> column index in E ---
    ld      e,-1
.col_scan:
    inc     e
    srl     a
    jr      nc,.col_scan

    ; --- offset = row*5 + column ---
    ld      a,d
    add     a,e                ; add column
    ld      hl,key_table
    add     hl,a
    ld      a,(hl)
    ld      (key_press),a
    ret

.no_key_in_row:
    ld      a,d
    add     5
    ld      d,a
    cp      8*5
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

