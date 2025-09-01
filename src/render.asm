render:
        call update_tilemap
        call convert_objects_to_sprites
        ret


convert_objects_to_sprites:
        ld ix,animated_objects
        ld b,MAX_OBJECTS
        ld c,0                                          ; Active sprite count
.update_loop
        bit ANIM_ACTIVE,(ix+OBJECT.flags)
        jr z,.not_active
        ; now form up the hardware sprite registers
        ld a,c
        inc c
        nextreg SPRITE_INDEX,a
        ; Update ATTR 0 - X
        ld a,(ix+OBJECT.X)
        nextreg SPRITE_ATTR_0,a
        ; Update ATTR 1 - Y
        ld a,(ix+OBJECT.Y)
        nextreg SPRITE_ATTR_1,a
        ; Update ATTR 2
        ld a,0
        bit 0,(ix+OBJECT.X+1)
        jr z,.no_ext_x
        or %0000001
.no_ext_x:
        nextreg SPRITE_ATTR_2,a
        ; Update ATTR 3
        ld a,(ix+OBJECT.base_index)
        add (ix+OBJECT.frame_index)
        ld d,a
        or %11000000                ; Visible and enable ATTR4
        nextreg SPRITE_ATTR_3,a
        ; Update ATTR 4
        ld a,0
        bit 0,(ix+OBJECT.Y+1)
        jr z,.no_ext_y
        or %00000001
.no_ext_y
        bit ANIM_4_BIT,(ix+OBJECT.flags)
        jr z,.not_4_bit
        set 7,a
.not_4_bit
        bit 6,d
        jr z,.not_ext_idx
        or %01000000
.not_ext_idx
        nextreg SPRITE_ATTR_4,a
.not_active
        ld de,OBJECT.size
        add ix,de
        djnz .update_loop
        ;
        ; Deactivate any remaining sprites
        ;
        cp a
        jr z,.no_update
        ld b,c
        ld a,(active_sprite_count)
        ld e,a
.clear_loop
        ld a,b
        nextreg SPRITE_INDEX, a
        nextreg SPRITE_ATTR_3,0
        inc b
        cp e
        jr nz,.clear_loop
.no_update
        ld a,c
        ld (active_sprite_count),a
        ret