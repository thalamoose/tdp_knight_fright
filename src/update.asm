update_user_input:
        ret
        ld a,(joystick_buttons)
        push af
        ld a,(mouse_button)
        push af
        ld a,(mouse_x)
        push af
        ld a,(mouse_y)
        push af
        call print_str
        db "Mouse X:%c, Y:%c, BTN:%c\r\n"
        db "Joystick BTN:%c\r\n",0
        call get_key
        ret z
        call print_char
        ret

update_sprites:
        ld a,(joystick_buttons)
        ld ix,animated_objects
        ld bc,(ix+OBJECT.X)
        bit JOYPAD_L_LEFT,a
        jr z,.no_dec_x
        dec bc
.no_dec_x
        bit JOYPAD_L_RIGHT,a
        jr z,.no_inc_x
        inc bc
.no_inc_x
        ld (ix+OBJECT.X),bc
        ld bc,(ix+OBJECT.Y)
        bit JOYPAD_L_UP,a
        jr z,.no_dec_y
        dec bc
.no_dec_y
        bit JOYPAD_L_DOWN,a
        jr z,.no_inc_y
        inc bc
.no_inc_y
        ld (ix+OBJECT.Y),bc

        ld de,OBJECT.size
        add ix,de

        ld a,(ix+OBJECT.X)
        add 1
        ld (ix+OBJECT.X),a
        ld a,(ix+OBJECT.X+1)
        adc 0
.no_wrap        
        ld (ix+OBJECT.X+1),a
        cp $01
        jr nz,.no_reset_x
        ld a,(ix+OBJECT.X)
        cp $40
        jr nz,.no_reset_x
        ret nz
        ld (ix+OBJECT.X),0
        ld (ix+OBJECT.X+1),0
.no_reset_x
        add ix,de
        inc (ix+OBJECT.Y)
        ret

