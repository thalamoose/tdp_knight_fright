


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
