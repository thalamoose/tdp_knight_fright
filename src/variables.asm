


player_object:
        block PLAYER
animated_objects:
        block OBJECT*MAX_OBJECTS

active_object_count:
        db 0
active_sprite_count:
        db 0

particle_index:
        db 0
particles_active:
        db 0
debug_max_particles:
        db 64
        ALIGN 16
particle_slot:
        db 0
        ALIGN 64
particle_objects:
        block PARTICLE*MAX_PARTICLES,0
; This is referenced from the center of the screen.
; Signed byte
        ALIGN 4
vsync_counter dw 0,0
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
scancodes:
        db 0,0,0,0,0,0,0,0
key_press:
        defb 0

random_seed:
        defw 0xc0de
random_seed_2:
        defw 0xf00d

