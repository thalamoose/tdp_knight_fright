


animated_objects:
        block OBJECT.size*MAX_OBJECTS,0

active_object_count:
        db 0
active_sprite_count:
        db 0

; This is referenced from the center of the screen.
; Signed byte
tilemap_x:
        db 0
tilemap_y:
        db 0
joystick_buttons:
        defw 0
mouse_x:
        defw 0
mouse_y:
        defw 0
mouse_button:
        defw 0
key_press:
        defb 0
