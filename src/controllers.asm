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

joystick_buttons:
        defw 0
mouse_x:
        defw 0
mouse_y:
        defw 0
mouse_button:
        defw 0
read_keyboard:
        ret
last_key:
        defb 0