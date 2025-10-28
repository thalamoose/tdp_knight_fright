
play_area_center_x      db      0
play_area_center_y      db      0
                        ALIGN 256
play_area_map           block   PLAYMAP_ENTRY*PLAY_AREA_CELLS_WIDTH*PLAY_AREA_CELLS_HEIGHT,0


                        ALIGN 256
player_object:          block PLAYER
animated_objects:       block OBJECT*MAX_OBJECTS

active_object_count:    db 0
active_sprite_count:    db 0

particle_index:         db 0
debug_max_particles:    db 64
        ALIGN 16
particle_slot:          db 0
        ALIGN 64
particle_objects:       block PARTICLE*MAX_PARTICLES,0
; This is referenced from the center of the screen.
; Signed byte
        ALIGN 4
vsync_counter           dw 0,0
tilemap_x:              db 0
tilemap_y:              db 0
joystick_buttons:       dw 0
mouse_x:                dw 0
mouse_y:                dw 0
mouse_button:
                        dw 0
scancodes:              db 0,0,0,0,0,0,0,0
key_press:              db 0

random_seed:            dw 0xc0de
random_seed_2:          dw 0xf00d

