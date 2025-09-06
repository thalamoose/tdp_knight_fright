initialize_system:
again:

        nextreg CLOCK_SEL,$02                 ; 14Mhz
        call configure_memory
        call clear_screen
        call print_str
        db "Booting KnightFright...\r\n"
        db __TIME__," ",__DATE__,"\r\n",0
        call initialize_interrupts
        call initialize_display
        call initialize_audio
;    call initialize_filesystem
        ei
        ret
initialize_game:
        call initialize_sprites
        call initialize_tilemap
        call initialize_particles
        ; Just set up the first sprite for now.
        ld ix,animated_objects
        ld (ix+OBJECT.X),128
        ld (ix+OBJECT.Y),96
        ld (ix+OBJECT.frame_index),0
        ld (ix+OBJECT.base_index),0
        ld (ix+OBJECT.total_frames),1
        ld (ix+OBJECT.flags),(1<<ANIM_ACTIVE)
        ld (ix+OBJECT.anim_speed),5
        ld (ix+OBJECT.anim_delay),0
        ld de,OBJECT.size
        add ix,de

        ld (ix+OBJECT.X),160
        ld (ix+OBJECT.Y),120
        ld (ix+OBJECT.frame_index),0
        ld (ix+OBJECT.base_index),1
        ld (ix+OBJECT.total_frames),3
        ld (ix+OBJECT.flags),(1<<ANIM_ACTIVE)
        ld (ix+OBJECT.anim_speed),5
        ld (ix+OBJECT.anim_delay),0
        add ix,de

        ld (ix+OBJECT.X),120
        ld (ix+OBJECT.Y),160
        ld (ix+OBJECT.frame_index),0
        ld (ix+OBJECT.base_index),0
        ld (ix+OBJECT.total_frames),8
        ld (ix+OBJECT.flags),(1<<ANIM_ACTIVE)|(1<<ANIM_PINGPONG)
        ld (ix+OBJECT.anim_speed),10
        ld (ix+OBJECT.anim_delay),0
        add ix,de

        ld b,128
.make_lp
        push bc
        call debug_add_particle
        pop bc
        djnz .make_lp
        ret
debug_add_particle:
        ld hl,160-16
        call get_random
        and 0x07
        add l
        ld l,a
        ld de,64
        call get_random
        and 0x07
        add e
        ld e,a

        call get_random
        sra a
        sra a
        inc a
        ld b,a
        call get_random
        sra a
        sra a
        inc a
        ld c,a
        call add_particle
        ret

; Initial memory map:
; Work      $0000..$3fff
; Screen    $4000..$7fff [mapped to pages 14,15]
; Code      $8000..$bfff [mapped to pages 4,5]
; Paged     $c000..$ffff [mapped as needed]

configure_memory:
        nextreg MMU_SLOT_2,ULA_SHADOW_PAGE
        nextreg MMU_SLOT_3,ULA_SHADOW_PAGE+1
        ld bc,MEM_PAGING_CONTROL_PORT
        ret
initialize_interrupts:
        ret
